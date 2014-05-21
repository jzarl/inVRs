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

#include "SimplePhysicsEntity.h"

#include <assert.h>

#include "OopsPhysicsEntityTransformationWriter.h"
#include "Physics.h"
#include "PhysicsMessageFunctions.h"
#include <inVRs/SystemCore/SystemCore.h>
#include <inVRs/SystemCore/MessageFunctions.h>
#include <inVRs/SystemCore/NetMessage.h>

const unsigned SimplePhysicsEntity::SIMPLEPHYSICSENTITY_CLASSID = 1;

using namespace oops;

//*****************//
// PUBLIC METHODS: //
//*****************//

SimplePhysicsEntity::SimplePhysicsEntity(unsigned short id, unsigned short environmentId, unsigned short instanceId, EntityType *type) :
PhysicsEntity(id, environmentId, instanceId, type)
{
	rigidBody = NULL;
	addedToSimulation = false;
	// typeBasedId is set in Entity-Constructor which is called from
	// the PhysicsEntity-Constructor
	physicsObjectID = PhysicsObjectID(SIMPLEPHYSICSENTITY_CLASSID, typeBasedId);
	rigidBodyInitialized = false;
} // SimplePhysicsEntity

SimplePhysicsEntity::~SimplePhysicsEntity()
{
	if (rigidBody)
	{
		if (rigidBody->isAddedToSimulation())
		{
			Physics* physics = (Physics*)SystemCore::getModuleByName("Physics");
			if (physics)
				physics->removePhysicsObject(this);
		} // if
		delete rigidBody;
	} // if
} // ~SimplePhysicsEntity

void SimplePhysicsEntity::addForce(gmtl::Vec3f& force, bool relative)
{
	if (rigidBody)
		rigidBody->addForce(force, relative);
	else
		printd(WARNING, "SimplePhysicsEntity::addForce(): Could not add force since no RigidBody is found!\n");
} // addForce

oops::RigidBody* SimplePhysicsEntity::getRigidBody()
{
	return rigidBody;
} // getRigidBody

//************************************************//
// PUBLIC METHODS INNHERITED FROM: PhysicsEntity: //
//************************************************//

void SimplePhysicsEntity::getRigidBodies(std::vector<oops::RigidBody*>& rigidBodyList) {

	if (!rigidBodyInitialized) {
		rigidBodyInitialized = calculatePhysicsTransformation();
		if (!rigidBodyInitialized)
			printd(ERROR, "SimplePhysicsEntity::getRigidBodies(): could not initialize RigidBody!\n");
	} // if

	if (rigidBody)
		rigidBodyList.push_back(rigidBody);
} // getRigidBodies

void SimplePhysicsEntity::getJoints(std::vector<oops::Joint*>& jointList) {
// No Joints to add here
} // getJoint

// DEPRECATED:
//bool SimplePhysicsEntity::addToSimulation(oops::Simulation* simulation)
//{
//	bool finished = true;
//	finished = calculatePhysicsTransformation() || finished;
//	finished = simulation->addRigidBody(rigidBody) || finished;
//	return finished;
//} // addToSimulation
//
//bool SimplePhysicsEntity::removeFromSimulation(oops::Simulation* simulation)
//{
//	return simulation->removeRigidBody(rigidBody);
//} // addToSimulation

PhysicsObjectID SimplePhysicsEntity::getPhysicsObjectID()
{
	return physicsObjectID;
} // getPhysicsObjectID

//*****************************************//
// PUBLIC METHODS INNHERITED FROM: Entity: //
//*****************************************//

void SimplePhysicsEntity::setEnvironment(Environment* env)
{
	Entity::setEnvironment(env);
	if (!addedToSimulation)
	{
		Physics* physicsModule = (Physics*)SystemCore::getModuleByName("Physics");
		assert(physicsModule);
		physicsModule->addPhysicsObject(this);
		addedToSimulation = true;
	} // if
} // setEnvironment

void SimplePhysicsEntity::setWorldTransformation(TransformationData trans)
{
	// Transformation should be written back to Entity from Physics Module
	if (rigidBody)
		rigidBody->setTransformation(trans);
	else
		Entity::setWorldTransformation(trans);
} // setWorldTransformation

//****************************//
// DEPRECATED PUBLIC METHODS: //
//****************************//

// SERVER_METHOD2(SimplePhysicsEntity, addForce, gmtl::Vec3f&, bool)(gmtl::Vec3f& force, bool relative)
// {
// 	rigidBody->addForce(force, relative);
// } // addForce_internal
//
// SERVER_METHOD2(SimplePhysicsEntity, addTorque, gmtl::Vec3f&, bool)(gmtl::Vec3f& torque, bool relative)
// {
// 	rigidBody->addTorque(torque, relative);
// } // addTorque_internal
//
// SERVER_METHOD4(SimplePhysicsEntity, addForceAtPosition, gmtl::Vec3f&, gmtl::Vec3f&, bool, bool)(gmtl::Vec3f& force, gmtl::Vec3f& position, bool relativeForce, bool relativePosition)
// {
// 	rigidBody->addForceAtPosition(force, position, relativeForce, relativePosition);
// } // addForceAtPosition_internal
//
// SERVER_METHOD2(SimplePhysicsEntity, setLinearVelocity, gmtl::Vec3f&, bool)(gmtl::Vec3f& velocity, bool relative)
// {
// 	rigidBody->setLinearVelocity(velocity, relative);
// } // setLinearVelocity_internal
//
// SERVER_METHOD2(SimplePhysicsEntity, setAngularVelocity, gmtl::Vec3f&, bool)(gmtl::Vec3f& angularVelocity, bool relative)
// {
// 	rigidBody->setAngularVelocity(angularVelocity, relative);
// }
//
// void SimplePhysicsEntity::setActive(bool active)
// {
// 	rigidBody->setActive(active);
// } // setActive
//
// bool SimplePhysicsEntity::isActive()
// {
// 	return rigidBody->isActive();
// } // isActive

// void SimplePhysicsEntity::update()
// {
// 	assert(physicsObject);
// 	calculatePhysicsTransformation();
// 	Entity::update();
// } // update

//********************//
// PROTECTED METHODS: //
//********************//

//void SimplePhysicsEntity::addForce_SERVER(gmtl::Vec3f& force, bool relative)
//{
//	rigidBody->addForce(force, relative);
//} // addForce_SERVER
//

void SimplePhysicsEntity::setRigidBody(RigidBody* obj, bool useBodyTransformation)
{
	TransformationData bodyTrans, offsetTrans, result;
	this->rigidBody = obj;

	std::string objectName = type->getName();
	rigidBody->setName(objectName);
	rigidBody->setFixed(fixed);

	rigidBody->setTransformationWriter(new OopsPhysicsEntityTransformationWriter(this));
	rigidBody->setUserData(this);

	if (useBodyTransformation)
	{
		bodyTrans = rigidBody->getTransformation();
		offsetTrans = rigidBody->getOffsetTransformation();
		multiply(result, bodyTrans, offsetTrans);
		Entity::setWorldTransformation(result);
	} // if
} // setPhysicsObject

bool SimplePhysicsEntity::calculatePhysicsTransformation()
{
	assert(env);
	if (rigidBody)
	{
		TransformationData worldTrans = getWorldTransformation();
		TransformationData offsetTrans = rigidBody->getOffsetTransformation();
		invert(offsetTrans);
		TransformationData result;
		multiply(result, worldTrans, offsetTrans);
		rigidBody->setTransformation_unchecked(result);
		return true;
	} // if

	return false;
} // calculatePhysicsTransformation

//***************************************************//
// PROTECTED METHODS INNHERITED FROM: PhysicsEntity: //
//***************************************************//

//bool SimplePhysicsEntity::handleMessage(unsigned function, NetMessage* msg)
//{
//	bool unknown = false;
//	gmtl::Vec3f vec1;
//	bool bool1;
//
//	switch(function)
//	{
//		case FUNCTION_addForce:
//			msgFunctions::decode(vec1, msg);
//			msgFunctions::decode(bool1, msg);
//			addForce_SERVER(vec1, bool1);
//			break;
//		default:
//			unknown = true;
//			break;
//	} // switch
//	return !unknown;
//} // handleMessage
//
//void SimplePhysicsEntity::synchroniseAllToDRClient(NetMessage* msg)
//{
//	TransformationData trans;
//	gmtl::Vec3f linearVel, angularVel;
//	msgFunctions::encode((int)FUNCTION_syncAllToDRClient, msg);
//	assert(rigidBody);
//	rigidBody->getTransformation(trans);
//	rigidBody->getLinearVelocity(linearVel);
//	rigidBody->getAngularVelocity(angularVel);
//	msgFunctions::encode(trans, msg);
//	msgFunctions::encode(linearVel, msg);
//	msgFunctions::encode(angularVel, msg);
//} // synchroniseAllToDRClient
//
//void SimplePhysicsEntity::handleSyncAllToDRClientMsg(NetMessage* msg)
//{
//	TransformationData trans, bodyTrans, diff;
//	gmtl::Vec3f linearVel, angularVel;
//	assert(rigidBody);
//	msgFunctions::decode(trans, msg);
//	msgFunctions::decode(linearVel, msg);
//	msgFunctions::decode(angularVel, msg);
//	bodyTrans = rigidBody->getTransformation();
//	invert(bodyTrans);
//	multiply(diff, bodyTrans, trans);
//	if (gmtl::length(diff.position) > 0.01f)
//	{
//		printd(INFO, "FOUND ERROR: ");
//		dumpTransformation(diff);
//	} // if
//	rigidBody->setTransformation(trans);
//	rigidBody->setLinearVelocity(linearVel);
//	rigidBody->setAngularVelocity(angularVel);
//} // handleSyncAllToDRClientMsg
//
//void SimplePhysicsEntity::synchroniseObjectTransformation(NetMessage* msg) {
//	TransformationData trans;
//	msgFunctions::encode((int)FUNCTION_syncObjectTransformation, msg);
//	assert(rigidBody);
//	rigidBody->getTransformation(trans);
//	msgFunctions::encode(trans.position, msg);
//	msgFunctions::encode(trans.orientation, msg);
//} // synchroniseObjectTransformation
//
//void SimplePhysicsEntity::handleSyncObjectTransformationMsg(NetMessage* msg) {
//	TransformationData trans, bodyTrans, diff;
//	trans = identityTransformation();
//	assert(rigidBody);
//	msgFunctions::decode(trans.position, msg);
//	msgFunctions::decode(trans.orientation, msg);
//	bodyTrans = rigidBody->getTransformation();
//// TODO: add method to reduce jumping corrections, maybe contact
////		 SynchronisationModel which does then some smoothing.
//	rigidBody->setTransformation(trans);
//} // handleSyncObjectTransformationMsg
//
//std::vector<RigidBody*>& SimplePhysicsEntity::getRigidBodiesForSync(std::vector<RigidBody*> &rigidBodyList) {
//	if (rigidBody)
//		rigidBodyList.push_back(rigidBody);
//	return rigidBodyList;
//} // getRigidBodiesForSync

//*******************************//
// DEPRECATED PROTECTED METHODS: //
//*******************************//

//void SimplePhysicsEntity::addForce_SERVER(gmtl::Vec3f& force, bool relative)
//{
//	rigidBody->addForce(force, relative);
//} // addForce_SERVER
//
//void SimplePhysicsEntity::setRigidBody(RigidBody* obj, bool useBodyTransformation)
//{
//	TransformationData bodyTrans, offsetTrans, result;
//	this->rigidBody = obj;
//
//	std::string objectName = type->getName();
//	rigidBody->setName(objectName);
//	rigidBody->setFixed(fixed);
//
//	rigidBody->setTransformationWriter(new OopsPhysicsEntityTransformationWriter(this));
//	rigidBody->setUserData(this);
//
//	if (useBodyTransformation)
//	{
//		bodyTrans = rigidBody->getTransformation();
//		offsetTrans = rigidBody->getOffsetTransformation();
//		multiply(result, bodyTrans, offsetTrans);
//		Entity::setWorldTransformation(result);
//	} // if
//} // setPhysicsObject
