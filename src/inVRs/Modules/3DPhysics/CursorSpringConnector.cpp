/*---------------------------------------------------------------------------*\
 *           interactive networked Virtual Reality system (inVRs)            *
 *                                                                           *
 *    Copyright (C) 2005-2009 by the Johannes Kepler University, Linz        *
 *                                                                           *
 *                            www.inVRs.org                                  *
 *                                                                           *
 *              contact: canthes@inVRs.org, rlander@inVRs.org                *
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *                                License                                    *
 *                                                                           *
 * This library is free software; you can redistribute it and/or modify it   *
 * under the terms of the GNU Library General Public License as published    *
 * by the Free Software Foundation, version 2.                               *
 *                                                                           *
 * This library is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Library General Public License for more details.                          *
 *                                                                           *
 * You should have received a copy of the GNU Library General Public         *
 * License along with this library; if not, write to the Free Software       *
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 *
\*---------------------------------------------------------------------------*/

#include "CursorSpringConnector.h"

#include <assert.h>

#include "Physics.h"
#include "OopsObjectID.h"
#include "SimplePhysicsEntity.h"
#include "ArticulatedPhysicsEntity.h"
#ifdef INVRS_BUILD_TIME
#include "oops/odeJoints/ODEJoints.h"
#else
#include <inVRs/Modules/3DPhysics/oops/odeJoints/ODEJoints.h>
#endif

#include <inVRs/SystemCore/UserDatabase/UserDatabase.h>
#include <inVRs/SystemCore/SystemCore.h>
#include "gmtl/QuatOps.h"
#include "gmtl/VecOps.h"
#include "gmtl/AxisAngle.h"
#include "gmtl/Generate.h"

float CursorSpringConnector::linearSpringConstant = 1.0f;
float CursorSpringConnector::angularSpringConstant = 0.01f;
float CursorSpringConnector::linearDampingConstant = 100.f;
float CursorSpringConnector::angularDampingConstant = 10.f;
float CursorSpringConnector::linearThreshold = 1.f;
float CursorSpringConnector::angularThreshold = M_PI;

using namespace oops;

OSG_USING_NAMESPACE

//*****************//
// PUBLIC METHODS: //
//*****************//

const unsigned CursorSpringConnector::CURSORSPRINGCONNECTOR_CLASSID = 3;

CursorSpringConnector::CursorSpringConnector(PhysicsEntity* entity, User* user)
{
	this->user = user;
	this->entity = entity;
	this->cursorTransformation = user->getCursorTransformation();

	physicsObjectID = PhysicsObjectID(CURSORSPRINGCONNECTOR_CLASSID, user->getId());

#if OPENSG_THREADMANAGER_GETLOCK_HAVE_BGLOBAL
	cursorTransformationLock = ThreadManager::the()->getLock("CursorSpringConnector::cursorTransformationLock",false);
#else
	cursorTransformationLock = dynamic_cast<Lock*>(ThreadManager::the()->getLock("CursorSpringConnector::cursorTransformationLock"));
#endif
	isInitialized = false;
	objectManager = NULL;
	rigidBody = NULL;

	rigidBodyId = 0;

// TODO: make configurable at runtime
/*	linearSpringConstant = 1.0f;
	linearDampingConstant = 100.0f;
	angularSpringConstant = 0.01f;
	angularDampingConstant = 10.f;
	linearThreshold = 1.f;
	angularThreshold = M_PI;
*/

//	connectionMode = SPRING_MODE;
//	connectionMode = VELOCITY_MODE;
	connectionMode = JOINT_MODE;
} // CursorSpringConnector

CursorSpringConnector::~CursorSpringConnector()
{
	Physics* physicsModule;
	physicsModule = (Physics*)SystemCore::getModuleByName("Physics");
	if (physicsModule) {
		physicsModule->removePhysicsObject(this);
		physicsModule->removeSimulationStepListener(this);
		physicsModule->removeSystemThreadListener(this);
	} // if
} // ~CursorSpringConnector

void CursorSpringConnector::setLinearConstants(float spring, float damping) {
	linearSpringConstant = spring;
	linearDampingConstant = damping;
} // setLinearConstants

void CursorSpringConnector::setAngularConstants(float spring, float damping) {
	angularSpringConstant = spring;
	angularDampingConstant = damping;
} // setAngularConstants


//*******************************************************//
// PUBLIC METHODS INHERITED FROM: PhysicsObjectInterface //
//*******************************************************//

void CursorSpringConnector::getRigidBodies(std::vector<oops::RigidBody*>& rigidBodyList) {
	if (!isInitialized) {
		isInitialized = initialize();
		if (!isInitialized)
			printd(ERROR, "CursorSpringConnector::getRigidBodies(): Could not initialize CursorSpringConnector!\n");
	} // if

	if (connectionMode == JOINT_MODE)
		rigidBodyList.push_back(cursorBody);
} // getRigidBodies

void CursorSpringConnector::getJoints(std::vector<oops::Joint*>& jointList) {
	if (!isInitialized) {
		isInitialized = initialize();
		if (!isInitialized)
			printd(ERROR, "CursorSpringConnector::getJoints(): Could not initialize CursorSpringConnector!\n");
	} // if

	if (connectionMode == JOINT_MODE)
		jointList.push_back(connectionJoint);
} // getJoints

PhysicsObjectID CursorSpringConnector::getPhysicsObjectID()
{
	return physicsObjectID;
} // getPhysicsObjectID

bool CursorSpringConnector::handleMessage(NetMessage* msg)
{
	return true;
} // handleMessage

void CursorSpringConnector::setPhysicsObjectManager(PhysicsObjectManager* objectManager)
{
	this->objectManager = objectManager;
} // setPhysicsObjectManager

//********************//
// PROTECTED METHODS: //
//********************//

void CursorSpringConnector::executeVelocityMode(float dt, TransformationData cursorTrans, TransformationData bodyTrans)
{
// FOLLOWING IS A CODE FOR MANIPULATION USING VELOCITIES
	gmtl::AxisAnglef axAng;
	gmtl::Vec3f correctingSpeed;
	TransformationData newBodyTrans;
	gmtl::Vec3f positionDifference;
	gmtl::Quatf invBodyOrientation, orientationDifference;

	multiply(newBodyTrans, cursorTrans, cursorToBodyOffset);

	positionDifference = newBodyTrans.position - bodyTrans.position;
	invBodyOrientation = bodyTrans.orientation;
	gmtl::invert(invBodyOrientation);
	orientationDifference = newBodyTrans.orientation * invBodyOrientation;
/*
	gmtl::Quatf testQuat, testDiff;
	testQuat = invBodyOrientation * newBodyTrans.orientation;
	testDiff = orientationDifference - testQuat;
	printd("Orientation difference = %f %f %f %f\n", t1estDiff[0], testDiff[1], testDiff[2], testDiff[3]);
	assert(testQuat == orientationDifference);
*/
	correctingSpeed = positionDifference * (0.5f/dt);
	rigidBody->setLinearVelocity(correctingSpeed);
	gmtl::set(axAng, orientationDifference);
	correctingSpeed = gmtl::Vec3f(axAng[0]*axAng[1], axAng[0]*axAng[2], axAng[0]*axAng[3]);
	correctingSpeed *= (1.0f/dt);
	rigidBody->setAngularVelocity(correctingSpeed);

// 	printd(INFO, "Correction Speed = %f %f %f\n", correctingSpeed[0], correctingSpeed[1], correctingSpeed[2]);
} // executeVelocityMode

void CursorSpringConnector::executeSpringMode(float dt, TransformationData cursorTrans, TransformationData bodyTrans)
{
// ORIGINAL SPRING SOLUTION:
	float mass;
	gmtl::Vec3f direction;
	gmtl::AxisAnglef axAng;
	gmtl::Vec3f force, torque;
	TransformationData currentAnchorTrans;
	gmtl::Vec3f positionDifference;
	gmtl::Quatf invAnchorOrientation, orientationDifference;
	gmtl::Vec3f cursorVelocity, bodyVelocity, velocityDifference;
	gmtl::Quatf invOldCursorOrientation, cursorAngularChange;

	mass = rigidBody->getMass();

	float maxForce = 2500.f;
	float maxTorque = 1250.f;

	multiply(currentAnchorTrans, bodyTrans, bodyToCursorOffset);

	positionDifference = cursorTrans.position - currentAnchorTrans.position;
	invAnchorOrientation = currentAnchorTrans.orientation;
	gmtl::invert(invAnchorOrientation);
	orientationDifference = cursorTrans.orientation * invAnchorOrientation;

	gmtl::AxisAnglef testAxAng;
	gmtl::set(testAxAng, orientationDifference);
	if (fabs(testAxAng[0]) > M_PI)
	{
		printd(INFO, "ORI_DIFF: angle: %f\t axis: %f %f %f\n", testAxAng[0]*180.f/M_PI, testAxAng[1], testAxAng[2], testAxAng[3]);

		if (testAxAng[0] > M_PI)
			testAxAng[0] -= 2*M_PI;
		else
			testAxAng[0] += 2*M_PI;
		gmtl::set(orientationDifference, testAxAng);
	} // if

	force = positionDifference;
	force *= linearSpringConstant;
	rigidBody->getPointVelocity(bodyVelocity, currentAnchorTrans.position);
// IGNORING CURSOR-SPEED DUE TO HIGH FORCE AND TORQUE VALUES!!!
// 	cursorVelocity = (cursorTrans.position - oldCursorTransformation.position) * dt;
	cursorVelocity = gmtl::Vec3f(0,0,0);
	velocityDifference = bodyVelocity - cursorVelocity;
// IGNORING DIRECTION OF FORCE AND TORQUE TO AVOID SWINGING
//	direction = force;
//	gmtl::normalize(direction);
//	normalPart = gmtl::dot(direction, velocityDifference);
//	velocityDifference = direction * normalPart;

	force -= (velocityDifference * linearDampingConstant);
	force *= mass;
	if (gmtl::length(force) > maxForce)
	{
		printd("Force: %f > %f\n", gmtl::length(force), maxForce);
		gmtl::normalize(force);
		force *= maxForce;
	} // if
	rigidBody->addForceAtPosition(force, currentAnchorTrans.position);

	gmtl::set(axAng, orientationDifference);
	torque = gmtl::Vec3f(axAng[0]*axAng[1], axAng[0]*axAng[2], axAng[0]*axAng[3]);
	torque *= angularSpringConstant;
	rigidBody->getAngularVelocity(bodyVelocity);
// IGNORING CURSOR-SPEED DUE TO HIGH FORCE AND TORQUE VALUES!!!
//	invOldCursorOrientation = oldCursorTransformation.orientation;
//	gmtl::invert(invOldCursorOrientation);
//	cursorAngularChange = cursorTrans.orientation * invOldCursorOrientation;
//	gmtl::set(axAng, cursorAngularChange);
//	cursorVelocity = gmtl::Vec3f(axAng[0]*axAng[1], axAng[0]*axAng[2], axAng[0]*axAng[3]) * dt;
	cursorVelocity = gmtl::Vec3f(0,0,0);
	velocityDifference = bodyVelocity - cursorVelocity;
// IGNORING DIRECTION OF FORCE ANDTORQUE TO AVOID SWINGING
// 	direction = torque;
// 	gmtl::normalize(direction);
// 	normalPart = gmtl::dot(direction, velocityDifference);
// 	velocityDifference = direction * normalPart;

 	torque -= (velocityDifference * angularDampingConstant);
	// TODO: maybe multiply with inertia tensor!!!
	torque *= mass;
	if (gmtl::length(torque) > maxTorque)
	{
		printd("Torque: %f > %f\n", gmtl::length(torque), maxTorque);
		gmtl::normalize(torque);
		torque *= maxTorque;
	} // if
 	rigidBody->addTorque(torque);
} // executeSpringMode

void CursorSpringConnector::executeForceTorqueMode(float dt, TransformationData cursorTrans, TransformationData bodyTrans)
{

} // executeForceTorqueMode

void CursorSpringConnector::executeJointMode(float dt, TransformationData cursorTrans, TransformationData bodyTrans)
{
	cursorBody->setTransformation_unchecked(cursorTrans);
	if (!rigidBody->isActive())
		rigidBody->setActive(true);
} // executeJointMode

bool CursorSpringConnector::initialize() {

	OopsObjectID cursorRigidBodyID;
	OopsObjectID cursorJointID;
	SimplePhysicsEntity* simpleEntity;
	ArticulatedPhysicsEntity* articulatedEntity;
	TransformationData cursorTrans, rigidBodyTrans;

	simpleEntity = dynamic_cast<SimplePhysicsEntity*>(entity);
	articulatedEntity = dynamic_cast<ArticulatedPhysicsEntity*>(entity);

	if (simpleEntity)
		rigidBody = simpleEntity->rigidBody;
	else if (articulatedEntity)
		rigidBody = articulatedEntity->articulatedBody->getMainBody();
	else
	{
		printd(ERROR, "CursorSpringConnector::initialize(): unsupported PhysicsEntity-ClassType found!\n");
		return false;
	} // else

	rigidBodyId = rigidBody->getID();

	// Access to cursorTransformation does not have to be locked yet until we
	// did not register as SystemThreadListener

	// cursorToBodyOffset = cursorTrans^(-1) * rigidBodyTrans
	cursorTrans = cursorTransformation;
	rigidBodyTrans = rigidBody->getTransformation();
	invert(cursorTrans);
	multiply(cursorToBodyOffset, cursorTrans, rigidBodyTrans);

	// rigidBodyTrans * bodyToCursorOffset = cursorTrans
	// bodyToCursorOffset = rigidBodyTrans^(-1) * cursorTrans
	cursorTrans = cursorTransformation;
	invert(rigidBodyTrans);
	multiply(bodyToCursorOffset, rigidBodyTrans, cursorTrans);

	oldCursorTransformation.position = cursorTrans.position;
	oldCursorTransformation.orientation = cursorTrans.orientation;

	Physics* physicsModule = ((Physics*)SystemCore::getModuleByName("Physics"));
	physicsModule->addSimulationStepListener(this);
	physicsModule->addSystemThreadListener(this);

	if (connectionMode == JOINT_MODE)
	{
		cursorBody = new oops::RigidBody(NULL);
		cursorBody->setGravityMode(false);
		cursorBody->setMass(std::numeric_limits<float>::max());
		cursorBody->setTransformation(cursorTrans);
		cursorRigidBodyID = OopsObjectID(false, CURSORSPRINGCONNECTOR_CLASSID, 0, user->getId());
		cursorBody->setID(cursorRigidBodyID.get());

		cursorJointID = OopsObjectID(true, CURSORSPRINGCONNECTOR_CLASSID, 0, user->getId());
		connectionJoint = new oops::SpringDamperJoint(cursorJointID.get(), cursorBody, rigidBody);
		connectionJoint->setLinearConstants(linearSpringConstant, linearDampingConstant);
		connectionJoint->setAngularConstants(angularSpringConstant, angularDampingConstant);
	} // if

	return true;
}

//*****************************************************************//
// PROTECTED METHODS INHERITED FROM: SystemThreadListenerInterface //
//*****************************************************************//

void CursorSpringConnector::systemUpdate(float dt)
{
#if OSG_MAJOR_VERSION >= 2
	cursorTransformationLock->acquire();
#else
	cursorTransformationLock->aquire();
#endif
		cursorTransformation = user->getCursorTransformation();
	cursorTransformationLock->release();
} // systemUpdate

//*******************************************************************//
// PROTECTED METHODS INHERITED FROM: SimulationStepListenerInterface //
//*******************************************************************//

void CursorSpringConnector::step(float dt, unsigned simulationTime)
{
	TransformationData localCursorTrans, rigidBodyTrans;

#if OSG_MAJOR_VERSION >= 2
	cursorTransformationLock->acquire();
#else
	cursorTransformationLock->aquire();
#endif
		localCursorTrans = cursorTransformation;
	cursorTransformationLock->release();

	Physics* physics = (Physics*)SystemCore::getModuleByName("Physics");
	if (!physics)
		return;

	Simulation* simulation = physics->getSimulation();
	if (!simulation)
		return;

	rigidBody = simulation->getRigidBodyByID(rigidBodyId);
	if (!rigidBody)
	{
		printd(WARNING, "CursorSpringConnector::executeJointMode(): RigidBody for manipulation not found in Simulation!\n");
		return;
	} // if

	rigidBodyTrans = rigidBody->getTransformation();

	if (connectionMode == VELOCITY_MODE)
		executeVelocityMode(dt, localCursorTrans, rigidBodyTrans);
	else if (connectionMode == SPRING_MODE)
		executeSpringMode(dt, localCursorTrans, rigidBodyTrans);
	else if (connectionMode == FORCETORQUE_MODE)
		executeForceTorqueMode(dt, localCursorTrans, rigidBodyTrans);
	else if (connectionMode == JOINT_MODE)
		executeJointMode(dt, localCursorTrans, rigidBodyTrans);
	else
		printd(ERROR, "CursorSpringConnector::step(): unknown connection mode!\n");

	oldCursorTransformation = localCursorTrans;
} // step
