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

#include "oops/Simulation.h"

#include <assert.h>
#include <gmtl/MatrixOps.h>
#include <inVRs/SystemCore/DebugOutput.h>

#include "oops/RigidBody.h"
#include "oops/Geometries.h"

#include "oops/RigidBodyMethodData.h"
#include "oops/SimulationObjectInputListener.h"

#define DUMPBOOL(x)	((x) ? "TRUE" : "FALSE")

// Object Oriented Physics Simulation
namespace oops
{

SimulationObjectInputListener* RigidBody::inputListener = NULL;

//*****************************************************************************
// Begin RigidBody

//*****************//
// PUBLIC METHODS: //
//*****************//

RigidBody::RigidBody(Geometry* geometry)
{
	initialize(geometry, NULL, NULL);
} // RigidBody

RigidBody::RigidBody(Geometry* geometry,
	TransformationWriterInterface* transformWriter)
{
	initialize(geometry, transformWriter, NULL);
} // RigidBody

RigidBody::RigidBody(Geometry* geometry, RendererInterface* renderer)
{
	initialize(geometry, NULL, renderer);
} // RigidBody

RigidBody::RigidBody(Geometry* geometry,
	TransformationWriterInterface* transformWriter,
	RendererInterface* renderer)
{
	initialize(geometry, transformWriter, renderer);
} // RigidBody

RigidBody::~RigidBody()
{
	int i, j;
	RigidBody* obj;

	if (transformWriter)
		delete transformWriter;
	if (renderer)
		delete renderer;
	if (geometry)
		delete geometry;

	if (body)
		dBodyDestroy(body);

	for (i=0; i < (int)collisionList.size(); i++)
	{
		obj = collisionList[i];
		for (j=0; j < (int)obj->collisionList.size(); j++)
		{
			if (obj->collisionList[j] == this)
			{
				obj->collisionList.erase(obj->collisionList.begin() + j);
				break;
			} // if
		} // for
	} // for
	collisionList.clear();
	for (i=0; i < (int)ignoreCollisionList.size(); i++)
	{
		obj = ignoreCollisionList[i];
		for (j=0; j < (int)obj->ignoreCollisionList.size(); j++)
		{
			if (obj->ignoreCollisionList[j] == this)
			{
				obj->ignoreCollisionList.erase(obj->ignoreCollisionList.begin() + j);
				break;
			} // if
		} // for
	} // for
	ignoreCollisionList.clear();
} // ~RigidBody

RigidBody* RigidBody::clone()
{
	int i;
	RigidBody* result;
	RigidBody* opponent;
	Geometry* geometryClone = NULL;
	if (geometry)
		geometryClone = geometry->clone();
	result = new RigidBody(geometryClone);
//	if (geometryClone)
//		geometryClone->owner = result;
	result->transformation = this->transformation;
	result->offsetTransformation = this->offsetTransformation;

	result->mass = this->mass;
	result->visible = this->visible;
	result->staticForce = this->staticForce;
	result->staticTorque = this->staticTorque;
	result->fixed = this->fixed;
	result->name = this->name;
// TODO: Check what to do with ID (has to be cloned due to IDs in ArticulatedBodies!)
	result->id = this->id;

// TODO: built should always be false in the new object!!! (also geometry->built)
	result->built = this->built;
	result->ignoreTransformation = this->ignoreTransformation;

	if (renderer)
		result->setRenderer(this->renderer->clone());

	result->initialLinVel = this->initialLinVel;
	result->initialAngVel = this->initialAngVel;

	for (i=0; i < (int)this->collisionList.size(); i++)
	{
		opponent = this->collisionList[i];
		result->collisionList.push_back(opponent);
		opponent->collisionList.push_back(result);
	} // for

	for (i=0; i < (int)this->ignoreCollisionList.size(); i++)
	{
		opponent = this->ignoreCollisionList[i];
		result->ignoreCollisionList.push_back(opponent);
		opponent->ignoreCollisionList.push_back(result);
	} // for

	return result;
} // clone

void RigidBody::finalizeIntialization()
{
	if (geometry)
	{
		geometry->finalizeInitialization();
	} // if
} // finalizeInitialization

void RigidBody::addCollisionListener(CollisionListenerInterface* listener)
{
	collisionListener.push_back(listener);
} // addCollisionListener

bool RigidBody::removeCollisionListener(CollisionListenerInterface* listener)
{
	int i;
	for (i=0; i < (int)collisionListener.size(); i++)
	{
		if (listener == collisionListener[i])
			break;
	} // for
	if (i < (int)collisionListener.size())
	{
		collisionListener.erase(collisionListener.begin()+i);
		return true;
	} // if
	return false;
} // removeCollisionListener

void RigidBody::setTransformationWriter(TransformationWriterInterface* writer)
{
	assert(this->transformWriter == NULL);
	this->transformWriter = writer;

	if (transformWriter)
		transformWriter->setTransformationOffset(transformation, offsetTransformation);
} // setTransformationWriter

void RigidBody::setRenderer(RendererInterface* renderer)
{
	this->renderer = renderer;
	renderer->setObject(this);
} // setRenderer

void RigidBody::setOffsetTransformation(TransformationData& offset)
{
	this->offsetTransformation = offset;

	if (transformWriter)
		transformWriter->setTransformationOffset(transformation, offsetTransformation);
} // setOffsetTransformation

void RigidBody::setUserData(void* data)
{
	this->userData = data;
} // setUserData

bool RigidBody::isActive()
{
	return active;
} // isActive

bool RigidBody::isVisible()
{
	return visible;
} // isVisible

bool RigidBody::isFixed()
{
	return fixed;
} // isFixed

TransformationData& RigidBody::getTransformation(TransformationData& dst)
{
	dst = transformation;
	return dst;
} // getTransformation

const TransformationData& RigidBody::getTransformation()
{
	return transformation;
} // getTransformation

const TransformationData& RigidBody::getOffsetTransformation()
{
	return offsetTransformation;
} // getOffsetTransformation

TransformationWriterInterface* RigidBody::getTransformationWriter() {
	return transformWriter;
} // getTransformationWriter

float RigidBody::getMass()
{
	return mass;
} // getMass

gmtl::Matrix33f RigidBody::getInertiaTensor()
{
	int i,j;
	dMass m;
	gmtl::Matrix33f result;

	if (!built)
		return gmtl::MAT_IDENTITY33F;

	dBodyGetMass(body, &m);

	for (i=0; i < 3; i++)
		for (j=0; j < 3; j++)
			result[i][j] = m.I[i*4+j];

	return result;
} // getInertiaTensor

gmtl::Vec3f& RigidBody::getStaticForce(gmtl::Vec3f& dst)
{
	dst = staticForce;
	return dst;
} // getStaticForce

const gmtl::Vec3f& RigidBody::getStaticForce()
{
	return staticForce;
} // getStaticForce

gmtl::Vec3f& RigidBody::getStaticTorque(gmtl::Vec3f& dst)
{
	dst = staticTorque;
	return dst;
} // getStaticTorque

const gmtl::Vec3f& RigidBody::getStaticTorque()
{
	return staticTorque;
} // getStaticTorque

gmtl::Vec3f& RigidBody::getLinearVelocity(gmtl::Vec3f& dst)
{
	if (fixed || !built)
		dst = gmtl::Vec3f(0,0,0);
	else
	{
// TODO: insert check if dReal == float (Vec3f) to directly write into dst
		const dReal* velocity = dBodyGetLinearVel(body);
		dst = gmtl::Vec3f(velocity[0], velocity[1], velocity[2]);
	} // else
	return dst;
} // getVelocity

gmtl::Vec3f& RigidBody::getAngularVelocity(gmtl::Vec3f& dst)
{
	if (fixed || !built)
		dst = gmtl::Vec3f(0,0,0);
	else
	{
// TODO: insert check if dReal == float (Vec3f) to directly write into dst
		const dReal* angularVelocity = dBodyGetAngularVel(body);
		dst = gmtl::Vec3f(angularVelocity[0], angularVelocity[1], angularVelocity[2]);
	} // else
	return dst;
} // getAngularVelocity

gmtl::Vec3f& RigidBody::getForce(gmtl::Vec3f& dst)
{
	if (fixed || !built)
		dst = gmtl::Vec3f(0,0,0);
	else
	{
		const dReal* force = dBodyGetForce(body);
		dst = gmtl::Vec3f(force[0], force[1], force[2]);
	} // else
	return dst;
} // getForce

gmtl::Vec3f& RigidBody::getTorque(gmtl::Vec3f& dst)
{
	if (fixed || !built)
		dst = gmtl::Vec3f(0,0,0);
	else
	{
		const dReal* torque = dBodyGetTorque(body);
		dst = gmtl::Vec3f(torque[0], torque[1], torque[2]);
	} // else
	return dst;
} // getTorque

gmtl::Vec3f& RigidBody::getBodyPositionInWorld(gmtl::Vec3f& dst, gmtl::Vec3f& src)
{
	if (fixed || !built)
	{
		printf("WARNING: RigidBody::getBodyPositionInWorld(): relative body position can not be converted in world coordinates on fixed objects!\n");
		dst = gmtl::Vec3f(0,0,0);
	} // if
	else
	{
		dVector3 result;
		dBodyGetRelPointPos(body, src[0], src[1], src[2], result);
		dst = gmtl::Vec3f(result[0], result[1], result[2]);
	} // else
	return dst;
} // getBodyPositionInWorld

gmtl::Vec3f& RigidBody::getWorldPositionInBody(gmtl::Vec3f& dst, gmtl::Vec3f& src)
{
	if (fixed || !built)
	{
		printf("WARNING: RigidBody::getWorldPositionInBody(): world position can not be converted in body coordinates on fixed objects!\n");
		dst = gmtl::Vec3f(0,0,0);
	} // if
	else
	{
		dVector3 result;
		dBodyGetPosRelPoint(body, src[0], src[1], src[2], result);
		dst = gmtl::Vec3f(result[0], result[1], result[2]);
	} // else
	return dst;
} // getWorldPositionInBody

gmtl::Vec3f& RigidBody::getPointVelocity(gmtl::Vec3f& dst, gmtl::Vec3f& position, bool relative)
{
	if (fixed || !built)
		dst = gmtl::Vec3f(0,0,0);
	else
	{
		dVector3 result;
		if (!relative)
			dBodyGetPointVel(body, position[0], position[1], position[2], result);
		else
			dBodyGetRelPointVel(body, position[0], position[1], position[2], result);
		dst = gmtl::Vec3f(result[0], result[1], result[2]);
	} // else
	return dst;
} // getPointPosition

gmtl::Vec3f& RigidBody::convertWorldToBody(gmtl::Vec3f& src, gmtl::Vec3f& dst)
{
	if (fixed || !built)
	{
		printf("WARNING: RigidBody::convertWorldToBody(): cannot convert vector from world to body coordinates on fixed objects!\n");
		dst = gmtl::Vec3f(0,0,0);
	} // if
	else
	{
		dVector3 result;
		dBodyVectorFromWorld(body, src[0], src[1], src[2], result);
		dst = gmtl::Vec3f(result[0], result[1], result[2]);
	} // else
	return dst;
} // convertWorldToBody

gmtl::Vec3f& RigidBody::convertBodyToWorld(gmtl::Vec3f& src, gmtl::Vec3f& dst)
{
	if (fixed || !built)
	{
		printf("WARNING: RigidBody::convertBodyToWorld(): cannot convert vector from body to world coordinates on fixed objects!\n");
		dst = gmtl::Vec3f(0,0,0);
	} // if
	else
	{
		dVector3 result;
		dBodyVectorToWorld(body, src[0], src[1], src[2], result);
		dst = gmtl::Vec3f(result[0], result[1], result[2]);
	} // else
	return dst;
} // convertBodyToWorld

Geometry* RigidBody::getGeometry()
{
	return geometry;
} // getGeometry

dBodyID RigidBody::getODEBody()
{
	return body;
} // getODEBody

dGeomID RigidBody::getODEGeometry()
{
	if (!geometry)
		return 0;
	return geometry->getODEGeometry();
} // getODEGeometry

bool RigidBody::getGravityMode()
{
	return gravityMode;
} // getGravityMode

void* RigidBody::getUserData()
{
	return userData;
} // getUserData

std::string RigidBody::getType()
{
	return className;
} // getType

void RigidBody::setInputListener(SimulationObjectInputListener* listener) {
	RigidBody::inputListener = listener;
} // setInputListener

SimulationObjectInputListener* RigidBody::getInputListener() {
	return inputListener;
} // getInputListener


void RigidBody::collideWithRigidBody(RigidBody* obj)
{
	if (built && inputListener)
	if (built && inputListener) {
		fprintf(stderr, "RigidBody::collideWithRigidBody(): WARNING: Synchronisation of this method call not supported!\n");
//		MP_collideWithRigidBody parameter(obj);
//		listener->handleRigidBodyInput(this, RB_COLLIDEWITHRIGIDBODY, &parameter);
	} // if
	collisionList.push_back(obj);
	obj->collideWithRigidBodyInternal(this);
} // collideWithRigidBody

void RigidBody::dontCollideWithRigidBody(RigidBody* obj)
{
	if (built && inputListener) {
		fprintf(stderr, "RigidBody::collideWithRigidBody(): WARNING: Synchronisation of this method call not supported!\n");
//		MP_dontCollideWithRigidBody parameter(obj);
//		listener->handleRigidBodyInput(this, RB_DONTCOLLIDEWITHRIGIDBODY, &parameter);
	} // if
	ignoreCollisionList.push_back(obj);
	obj->dontCollideWithRigidBodyInternal(this);
} // dontCollideWithRigidBody

//********************************//
// PUBLIC SYNCHRONIZABLE METHODS: //
//********************************//

void RigidBody::setActive(bool active)
{
	if (fixed || !built)
		return;

	if (built && inputListener) {
		MP_setActive parameter(active);
		inputListener->handleRigidBodyInput(this, RB_SETACTIVE, &parameter);
	} // if
	setActive_unchecked(active);
} // setActive

void RigidBody::setActive_unchecked(bool active) {
	if (fixed || !built || active == this->active)
		return;

	if (active)
		dBodyEnable(body);
	else
		dBodyDisable(body);
	this->active = active;
} // setActive_unchecked

void RigidBody::setTransformation(TransformationData& trans, bool forwardToWriter)
{
	if (built && inputListener) {
		MP_setTransformation parameter(trans, forwardToWriter);
		inputListener->handleRigidBodyInput(this, RB_SETTRANSFORMATION, &parameter);
	} // if
	setTransformation_unchecked(trans, forwardToWriter);
} // setTransformation

void RigidBody::setTransformation_unchecked(TransformationData& trans, bool forwardToWriter) {
	this->transformation = trans;

	if (built && !ignoreTransformation)
	{
		if (!active)
			setActive(true);
		setODETransformation();
	} // if

	if (!built && transformWriter)
		transformWriter->setTransformationOffset(transformation, offsetTransformation);

	if (forwardToWriter && transformWriter)
		transformWriter->updateTransformation(trans);

/*	printd(INFO, "************************************************************************************************\nRigidBody::setTransformation() called with:\n");
	dumpTransformation(transformation);
	printd(INFO, "************************************************************************************************\n");*/
} // setTransformation

void RigidBody::setMass(float mass)
{
	if (built && inputListener) {
		MP_setMass parameter(mass);
		inputListener->handleRigidBodyInput(this, RB_SETMASS, &parameter);
	} // if

	setMass_unchecked(mass);
} // setMass

void RigidBody::setMass_unchecked(float mass)
{
	assert(mass > 0);
	this->mass = mass;

	if (fixed)
		return;
	if (built)
		setODEMass();
} // setMass

void RigidBody::setVisible(bool visible)
{
	if (built && inputListener) {
		MP_setVisible parameter(visible);
		inputListener->handleRigidBodyInput(this, RB_SETVISIBLE, &parameter);
	} // if

	setVisible_unchecked(visible);
} // setVisible

void RigidBody::setVisible_unchecked(bool visible)
{
	this->visible = visible;
	if (renderer)
		renderer->setVisible(visible);
} // setVisible

void RigidBody::addForce(gmtl::Vec3f& force, bool relative)
{
	if (built && inputListener) {
		MP_addForce parameter(force, relative);
		inputListener->handleRigidBodyInput(this, RB_ADDFORCE, &parameter);
	} // if

	addForce_unchecked(force, relative);
} // addForce

void RigidBody::addForce_unchecked(gmtl::Vec3f& force, bool relative)
{
	if (fixed || !built)
	{
		printf("WARNING: RigidBody::addForce() cannot add force to object (fixed = %s / built = %s)\n", DUMPBOOL(fixed), DUMPBOOL(built));
		return;
	} // if

	if (!active)
	{
		dBodyEnable(body);
		active = true;
	} // if

	if (!relative)
		dBodyAddForce(body, force[0], force[1], force[2]);
	else
		dBodyAddRelForce(body, force[0], force[1], force[2]);
} // addForce

void RigidBody::addTorque(gmtl::Vec3f& torque, bool relative)
{
	if (built && inputListener) {
		MP_addTorque parameter(torque, relative);
		inputListener->handleRigidBodyInput(this, RB_ADDTORQUE, &parameter);
	} // if

	addTorque_unchecked(torque, relative);
} // addTorque

void RigidBody::addTorque_unchecked(gmtl::Vec3f& torque, bool relative)
{
	if (fixed || !built)
	{
		printf("WARNING: RigidBody::addTorque() cannot add torque to object (fixed = %s / built = %s)\n", DUMPBOOL(fixed), DUMPBOOL(built));
		return;
	} // if

	if (!active)
	{
		dBodyEnable(body);
		active = true;
	} // if

	if (!relative)
		dBodyAddTorque(body, torque[0], torque[1], torque[2]);
	else
		dBodyAddRelTorque(body, torque[0], torque[1], torque[2]);
} // addTorque

void RigidBody::addForceAtPosition(gmtl::Vec3f& force, gmtl::Vec3f& position, bool relativeForce, bool relativePosition)
{
	if (built && inputListener) {
		MP_addForceAtPosition parameter(force, position, relativeForce, relativePosition);
		inputListener->handleRigidBodyInput(this, RB_ADDFORCEATPOSITION, &parameter);
	} // if

	addForceAtPosition_unchecked(force, position, relativeForce, relativePosition);
} // addForceAtPosition

void RigidBody::addForceAtPosition_unchecked(gmtl::Vec3f& force, gmtl::Vec3f& position, bool relativeForce, bool relativePosition)
{
	if (fixed || !built)
	{
		printf("WARNING: RigidBody::addForceAtPosition() cannot add force to object (fixed = %s / built = %s)\n", DUMPBOOL(fixed), DUMPBOOL(built));
		return;
	} // if

	if (!active)
	{
		dBodyEnable(body);
		active = true;
	} // if

	if (!relativePosition && !relativeForce)
		dBodyAddForceAtPos(body, force[0], force[1], force[2], position[0], position[1], position[2]);
	else if (!relativePosition && relativeForce)
		dBodyAddRelForceAtPos(body, force[0], force[1], force[2], position[0], position[1], position[2]);
	else if (relativePosition && !relativeForce)
		dBodyAddForceAtRelPos(body, force[0], force[1], force[2], position[0], position[1], position[2]);
	else
		dBodyAddRelForceAtRelPos(body, force[0], force[1], force[2], position[0], position[1], position[2]);
} // addForceAtPosition

void RigidBody::setForce(gmtl::Vec3f& force)
{
	if (built && inputListener) {
		MP_setForce parameter(force);
		inputListener->handleRigidBodyInput(this, RB_SETFORCE, &parameter);
	} // if

	setForce_unchecked(force);
} // setForce

void RigidBody::setForce_unchecked(gmtl::Vec3f& force)
{
	if (fixed || !built)
	{
		printf("WARNING: RigidBody::setForce() cannot set force to object (fixed = %s / built = %s)\n", DUMPBOOL(fixed), DUMPBOOL(built));
		return;
	} // if

	if (!active)
	{
		dBodyEnable(body);
		active = true;
	} // if

	dBodySetForce(body, force[0], force[1], force[2]);
} // setForce

void RigidBody::setTorque(gmtl::Vec3f& torque)
{
	if (built && inputListener) {
		MP_setTorque parameter(torque);
		inputListener->handleRigidBodyInput(this, RB_SETTORQUE, &parameter);
	} // if

	setTorque_unchecked(torque);
} // setTorque

void RigidBody::setTorque_unchecked(gmtl::Vec3f& torque)
{
	if (fixed || !built)
	{
		printf("WARNING: RigidBody::setTorque() cannot set torque to object (fixed = %s / built = %s)\n", DUMPBOOL(fixed), DUMPBOOL(built));
		return;
	} // if

	if (!active)
	{
		dBodyEnable(body);
		active = true;
	} // if

	dBodySetTorque(body, torque[0], torque[1], torque[2]);
} // setTorque

void RigidBody::setStaticForce(gmtl::Vec3f& force)
{
	if (built && inputListener) {
		MP_setStaticForce parameter(force);
		inputListener->handleRigidBodyInput(this, RB_SETSTATICFORCE, &parameter);
	} // if

	setStaticForce_unchecked(force);
} // setStaticForce

void RigidBody::setStaticForce_unchecked(gmtl::Vec3f& force)
{
	this->staticForce = force;

	if (gmtl::length(force) != 0)
		staticForceSet = true;
	else if (gmtl::length(staticTorque) == 0)
		staticForceSet = false;
} // setStaticForce

void RigidBody::setStaticTorque(gmtl::Vec3f& torque)
{
	if (built && inputListener) {
		MP_setStaticTorque parameter(torque);
		inputListener->handleRigidBodyInput(this, RB_SETSTATICTORQUE, &parameter);
	} // if

	setStaticTorque_unchecked(torque);
} // setStaticTorque

void RigidBody::setStaticTorque_unchecked(gmtl::Vec3f& torque)
{
	this->staticTorque = torque;

	if (gmtl::length(torque) != 0)
		staticForceSet = true;
	else if (gmtl::length(staticForce) == 0)
		staticForceSet = false;
} // setStaticTorque

void RigidBody::setLinearVelocity(gmtl::Vec3f velocity, bool relative)
{
	if (built && inputListener) {
		MP_setLinearVelocity parameter(velocity, relative);
		inputListener->handleRigidBodyInput(this, RB_SETLINEARVELOCITY, &parameter);
	} // if

	setLinearVelocity_unchecked(velocity, relative);
} // setLinearVelocity

void RigidBody::setLinearVelocity_unchecked(gmtl::Vec3f velocity, bool relative)
{
	gmtl::Vec3f vel;

	if (!fixed && !built)
		initialLinVel = velocity;
	else if (!fixed)
	{
		if (!active)
			setActive(true);

		if (relative)
		{
			convertBodyToWorld(velocity, vel);
			dBodySetLinearVel(body, vel[0], vel[1], vel[2]);
		} // if
		else
			dBodySetLinearVel(body, velocity[0], velocity[1], velocity[2]);
	} // else if
} // setLinearVelocity

void RigidBody::setAngularVelocity(gmtl::Vec3f velocity, bool relative)
{
	if (built && inputListener) {
		MP_setAngularVelocity parameter(velocity, relative);
		inputListener->handleRigidBodyInput(this, RB_SETANGULARVELOCITY, &parameter);
	} // if

	setAngularVelocity_unchecked(velocity, relative);
} // setAngularVelocity

void RigidBody::setAngularVelocity_unchecked(gmtl::Vec3f velocity, bool relative)
{
	gmtl::Vec3f vel;

	if (!fixed && !built)
		initialAngVel = velocity;
	else if (!fixed)
	{
		if (!active)
			setActive(true);

		if (relative)
		{
			convertBodyToWorld(velocity, vel);
			dBodySetAngularVel(body, vel[0], vel[1], vel[2]);
		} // if
		else
			dBodySetAngularVel(body, velocity[0], velocity[1], velocity[2]);
	} // else if
} // setAngularVelocity

void RigidBody::setFixed(bool fixed)//, dWorldID world)
{
	if (built && inputListener) {
		MP_setFixed parameter(fixed);
		inputListener->handleRigidBodyInput(this, RB_SETFIXED, &parameter);
	} // if

	setFixed_unchecked(fixed);
} // setFixed

void RigidBody::setFixed_unchecked(bool fixed)//, dWorldID world)
{
	dWorldID world = 0;

	if (dynamic_cast<Plane*>(geometry) != NULL)
		return;

	if (!this->fixed && fixed && built && geometry)
	{
		updateTransformationData();
		geometry->attachGeom(0);
//		dGeomSetBody(geometry->getODEGeometry(), body);
		dBodyDestroy(body);
	} // if
	else if (this->fixed && !fixed && built && geometry)
	{
		this->fixed = fixed;
		if (world == 0)
			world = this->world;
		body = dBodyCreate(world);
		setODETransformation();
		setODEMass();
		geometry->attachGeom(body);
//		dGeomSetBody(geometry->getODEGeometry(), body);
// TODO: check if everything which is done in build is also done here!!!
		dBodySetGravityMode(body, gravityMode);
	} // else if
	this->fixed = fixed;
} // setFixed

void RigidBody::setGravityMode(bool onOff)
{
	if (built && inputListener) {
		MP_setGravityMode parameter(onOff);
		inputListener->handleRigidBodyInput(this, RB_SETGRAVITYMODE, &parameter);
	} // if

	setGravityMode_unchecked(onOff);
} // setGravityMode

void RigidBody::setGravityMode_unchecked(bool onOff)
{
	this->gravityMode = onOff;

	if (!fixed && built)
		dBodySetGravityMode(body, gravityMode);
} // setGravityMode

void RigidBody::setColliding(bool doesCollide)
{
	this->colliding = doesCollide;
} // setColliding

void RigidBody::updateVisualRepresentation()
{
	if(renderer)
		renderer->setObject(this);
} // updateVisualRepresentation

//********************//
// PROTECTED METHODS: //
//********************//

void RigidBody::initialize(Geometry* geometry,
	TransformationWriterInterface* transformWriter,
	RendererInterface* renderer)
{
	this->className = "RigidBody";

	this->world = 0;
	this->space = 0;
	this->body = 0;
	this->ignoreTransformation = false;
	this->fixed = false;
	this->active = true;
	this->staticForceSet = false;
	this->colliding = true;

	if (geometry)
	{
		assert(geometry->owner == NULL);
		geometry->owner = this;
	} // if
	this->geometry = geometry;
	this->transformWriter = NULL;
	this->renderer = renderer;
	if (renderer)
		renderer->setObject(this);

	this->transformation = identityTransformation();
	this->offsetTransformation = identityTransformation();
	this->setTransformationWriter(transformWriter);
	this->mass = 1;
	this->built = false;
	this->setVisible(false);

	// AVOID ADDING OF OBJECT TO PLANE GEOMETRY
	if (dynamic_cast<Plane*>(geometry) != NULL)
		fixed = true;

	this->initialLinVel = gmtl::Vec3f(0,0,0);
	this->initialAngVel = gmtl::Vec3f(0,0,0);
	this->gravityMode = true;
	this->userData = NULL;
} // initialize

void RigidBody::updateTransformationData()
{
	assert (built);

	if (fixed)
		return;

	dReal const *pos = dBodyGetPosition(body);
	dReal const *quat = dBodyGetQuaternion(body);
//	transformation.position = gmtl::Vec3f(pos[0], pos[1], pos[2]);
//	transformation.orientation = gmtl::Quatf(quat[1], quat[2], quat[3], quat[0]);
	transformation.position[0] = pos[0];
	transformation.position[1] = pos[1];
	transformation.position[2] = pos[2];
	transformation.orientation[0] = quat[1];
	transformation.orientation[1] = quat[2];
	transformation.orientation[2] = quat[3];
	transformation.orientation[3] = quat[0];

// TODO: remove slow dynamic_cast!!!
	TriangleMesh* triMesh = dynamic_cast<TriangleMesh*>(geometry);
	if (triMesh)
	{
		triMesh->update(transformation);
	} // if

} // updateTransformationData

void RigidBody::build(dWorldID world, dSpaceID space)
{
	assert(!built);

	this->world = world;
	this->space = space;

	if (geometry)
		geometry->build(space);
	if (dynamic_cast<Plane*>(geometry))
	{
		built = true;
		ignoreTransformation = true;
		return;
	} // if

	if (!fixed)
	{
		body = dBodyCreate(world);
		dBodySetData(body, this);
		setODEMass();
		setODETransformation();
		if (geometry)
			geometry->attachGeom(body);
		dBodySetLinearVel(body, initialLinVel[0], initialLinVel[1], initialLinVel[2]);
		dBodySetAngularVel(body, initialAngVel[0], initialAngVel[1], initialAngVel[2]);
		dBodySetGravityMode(body, gravityMode);
	} // if
	else if (geometry)
	{
		geometry->positionGeometry(transformation.position, transformation.orientation);
		active = false;
	} // else
	else
	{
// TODO: implement also this feature, even if it doesn't make sense
		printf("ERROR: Building a fixed object without geometry isn't possible!\n");
	}
	built = true;
} // build

void RigidBody::destroy()
{
	if (!built) {
		if (body != 0) {
			fprintf(stderr,
					"RigidBody::destroy(): ERROR: found ODE-body but RigidBody is not built!\n");
			assert(false);
		} // if
		return;
	} // if

	if (geometry)
		geometry->destroy();

	if (!fixed)
	{
		dBodyDestroy(body);
		body = 0;
	} // if

	assert(body == 0);
} // destroy

void RigidBody::updateTransformation()
{
	if (!built)
		return;

	if (fixed && transformWriter)
		transformWriter->updateTransformation(transformation);
	else if (!fixed)
	{
		active = dBodyIsEnabled(body);
		if (active)
		{
			updateTransformationData();

			if (transformWriter)
				transformWriter->updateTransformation(transformation);
		} // if
	} // else if
} // updateTransformation

bool RigidBody::render()
{
	if (visible && renderer)
//	if (renderer)
	{
		renderer->render();
		return true;
	} // if
	return false;
} // render

void RigidBody::applyForce()
{
	assert(built);

	if (fixed || !staticForceSet)
		return;

	if (!active)
	{
		dBodyEnable(body);
		active = true;
	} // if

	dBodyAddForce(body, staticForce[0], staticForce[1], staticForce[2]);
	dBodyAddTorque(body, staticTorque[0], staticTorque[1], staticTorque[2]);
} // applyForce

bool RigidBody::canCollideWith(RigidBody* obj)
{
	int i;
	bool canCollide;
	if (!colliding || !obj->colliding)
	{
		canCollide = false;
		for (i=0; i < (int)collisionList.size(); i++)
		{
			if (collisionList[i] == obj)
			{
				canCollide = true;
				break;
			} // if
		} // for
	} // if
	else
	{
		canCollide = true;
		for (i=0; i < (int)ignoreCollisionList.size(); i++)
		{
			if (ignoreCollisionList[i] == obj)
			{
				canCollide = false;
				break;
			} // if
		} // for
	} // else
	return canCollide;
} // canCollideWith

void RigidBody::collideWithRigidBodyInternal(RigidBody* obj)
{
	collisionList.push_back(obj);
} // collideWithRigidBodyInternal

void RigidBody::dontCollideWithRigidBodyInternal(RigidBody* obj)
{
	ignoreCollisionList.push_back(obj);
} // dontCollideWithRigidBodyInternal

void RigidBody::collisionOccured(RigidBody* opponent, std::vector<ContactData>& contacts)
{
	int i;
	for (i=0; i < (int)collisionListener.size(); i++)
	{
		collisionListener[i]->notifyCollision(this, opponent, contacts);
	} // for
} // collisionOccured

void RigidBody::setODETransformation()
{
// TODO: check this assert since it does not make sense
// 	   THIS WAS ORIGINALLY TO ALLOW THIS OPERATION ONLY BEFORE
//	   AN OBJECT IS BUILT (ADDED TO SIMULATION) OR IF IT IS FIXED!!!
// 	assert(!built || !fixed);

	if (body)
	{
		dQuaternion q;
		dBodySetPosition(body, transformation.position[0],
			transformation.position[1],	transformation.position[2]);
		q[0] = transformation.orientation[3];
		q[1] = transformation.orientation[0];
		q[2] = transformation.orientation[1];
		q[3] = transformation.orientation[2];
		dBodySetQuaternion(body, q);
	} // if
	else if (geometry)
	{
		geometry->positionGeometry(transformation.position, transformation.orientation);
	} // else if
} // setODETransformation

void RigidBody::setODEMass()
{
	dMass m;
	if (geometry)
		geometry->setMass(mass, m);
	else
		dMassSetSphereTotal(&m, mass, 1);
// TODO: find a better solution for mass setting when no geometry is set!

	dBodySetMass(body, &m);
} // setODEMass


// End RigidBody
//*****************************************************************************

} // oops
