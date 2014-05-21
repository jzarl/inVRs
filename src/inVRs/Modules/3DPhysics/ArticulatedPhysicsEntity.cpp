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

#include "ArticulatedPhysicsEntity.h"

#include <assert.h>

#include "Physics.h"
#include "SimplePhysicsEntity.h"
#include "PhysicsMessageFunctions.h"
#include "OopsPhysicsEntityTransformationWriter.h"

#ifdef INVRS_BUILD_TIME
#include "oops/Simulation.h"
#else
#include <inVRs/Modules/3DPhysics/oops/Simulation.h>
#endif
#include <inVRs/SystemCore/DebugOutput.h>
#include <inVRs/SystemCore/MessageFunctions.h>
#include <inVRs/SystemCore/SystemCore.h>
#include <inVRs/SystemCore/WorldDatabase/WorldDatabase.h>

const unsigned ArticulatedPhysicsEntity::ARTICULATEDPHYSICSENTITY_CLASSID = 2;

using namespace oops;

//*****************//
// PUBLIC METHODS: //
//*****************//

ArticulatedPhysicsEntity::ArticulatedPhysicsEntity(unsigned short id, unsigned short environmentId, unsigned short instanceId, EntityType *type) :
PhysicsEntity(id, environmentId, instanceId, type)
{
	articulatedBody = NULL;
	addedToSimulation = false;
	isInitialized = false;
	// typeBasedId is set in Entity-Constructor which is called from
	// the PhysicsEntity-Constructor
	physicsObjectID = PhysicsObjectID(ARTICULATEDPHYSICSENTITY_CLASSID, typeBasedId);
} // ArticulatedPhysicsEntity

ArticulatedPhysicsEntity::~ArticulatedPhysicsEntity()
{
	SimplePhysicsEntity* entity;
	unsigned entityId;
	unsigned rigidBodyIndex, entityIndex;
	std::map<unsigned, unsigned>::iterator it;

	Physics* physics = (Physics*)SystemCore::getModuleByName("Physics");

	if (physics && articulatedBody->isAddedToSimulation())
		physics->removePhysicsObject(this);

	// Clear rigidBody-Pointer in SimplePhysicsEntity
	for (it = rigidBodyMapping.begin(); it != rigidBodyMapping.end(); ++it)
	{
		rigidBodyIndex = (*it).first;
		entityIndex = (*it).second;
		entityId = this->environmentBasedId + entityIndex;
		entity = (SimplePhysicsEntity*)WorldDatabase::getEntityWithEnvironmentId(entityId);
		if (entity)
			entity->rigidBody = NULL;
	} // for

	delete articulatedBody;
} // ~ArticulatedPhysicsEntity

void ArticulatedPhysicsEntity::addForceAtPosition(gmtl::Vec3f& force,
	gmtl::Vec3f& position, bool relativeForce,	bool relativePosition)
{

//	NetMessage* msg = new NetMessage;
//	msgFunctions::encode((int)FUNCTION_addForceAtPosition, msg);
//	msgFunctions::encode(force, msg);
//	msgFunctions::encode(position, msg);
//	msgFunctions::encode(relativeForce, msg);
//	msgFunctions::encode(relativePosition, msg);
//	objectManager->handleClientInput(msg, physicsObjectID);
	articulatedBody->getMainBody()->addForceAtPosition(force, position, relativeForce, relativePosition);
} // addForceAtPosition

//************************************************//
// PUBLIC METHODS INNHERITED FROM: PhysicsEntity: //
//************************************************//

void ArticulatedPhysicsEntity::getRigidBodies(std::vector<oops::RigidBody*>& rigidBodyList) {
	if (!isInitialized) {
		isInitialized = initialize();
		if (!isInitialized)
			printd(ERROR, "ArticulatedPhysicsEntity::getRigidBodies(): Could not initialize ArticulatedPhysicsEntity!\n");
	} // if

	articulatedBody->getRigidBodies(rigidBodyList);
} // getRigidBodies

void ArticulatedPhysicsEntity::getJoints(std::vector<oops::Joint*>& jointList) {
	if (!isInitialized) {
		isInitialized = initialize();
		if (!isInitialized)
			printd(ERROR, "ArticulatedPhysicsEntity::getJoints(): Could not initialize ArticulatedPhysicsEntity!\n");
	} // if

	articulatedBody->getJoints(jointList);
} // getJoints


//bool ArticulatedPhysicsEntity::addToSimulation(oops::Simulation* simulation)
//{
//	RigidBody* body;
//	SimplePhysicsEntity* entity;
//	unsigned entityId;
//	unsigned rigidBodyIndex, entityIndex;
//	std::map<unsigned, unsigned>::iterator it;
//
//	bool finished = true;
//	finished = calculatePhysicsTransformation() || finished;
//	finished = simulation->addArticulatedBody(articulatedBody) || finished;
//
//	for (it = rigidBodyMapping.begin(); it != rigidBodyMapping.end(); ++it)
//	{
//		rigidBodyIndex = (*it).first;
//		entityIndex = (*it).second;
//		entityId = this->environmentBasedId + entityIndex;
//		entity = (SimplePhysicsEntity*)WorldDatabase::getEntityWithEnvironmentId(entityId);
//		assert(entity);
//		body = articulatedBody->getRigidBodyByID(rigidBodyIndex);
//		entity->setRigidBody(body, true);
//	} // for
//	return finished;
//} // addToSimulation
//
//bool ArticulatedPhysicsEntity::removeFromSimulation(oops::Simulation* simulation)
//{
//	return simulation->removeArticulatedBody(articulatedBody);
//} // removeFromSimulation

PhysicsObjectID ArticulatedPhysicsEntity::getPhysicsObjectID()
{
	return physicsObjectID;
} // getPhysicsObjectID

//*****************************************//
// PUBLIC METHODS INNHERITED FROM: Entity: //
//*****************************************//

void ArticulatedPhysicsEntity::setEnvironment(Environment* env)
{
	Entity::setEnvironment(env);
	if (articulatedBody && !addedToSimulation)
	{
		Physics* physicsModule = (Physics*)SystemCore::getModuleByName("Physics");
		assert(physicsModule);
		physicsModule->addPhysicsObject(this);
		addedToSimulation = true;
	} // if
} // setEnvironment

//****************************//
// DEPRECATED PUBLIC METHODS: //
//****************************//

// ArticulatedBody* ArticulatedPhysicsEntity::getArticulatedBody()
// {
// 	return articulatedBody;
// } // getArticulatedBody

// void ArticulatedPhysicsEntity::setActive(bool active)
// {
// 	articulatedBody->setActive(active);
// } // setActive

// bool ArticulatedPhysicsEntity::isActive()
// {
// 	return articulatedBody->isActive();
// } // isActive


//********************//
// PROTECTED METHODS: //
//********************//

//void ArticulatedPhysicsEntity::addForceAtPosition_SERVER(gmtl::Vec3f& force,
//	gmtl::Vec3f& position, bool relativeForce, bool relativePosition)
//{
//	articulatedBody->getMainBody()->addForceAtPosition(force, position, relativeForce, relativePosition);
//} // addForceAtPosition_SERVER

void ArticulatedPhysicsEntity::setArticulatedBody(ArticulatedBody* obj)
{
	this->articulatedBody = obj;

	std::string objectName = type->getName();
	articulatedBody->setName(objectName);
// 	physicsObject->setFixed(fixed);

	articulatedBody->getMainBody()->setTransformationWriter(new OopsPhysicsEntityTransformationWriter(this));
} // setPhysicsObject

bool ArticulatedPhysicsEntity::initialize()
{
	RigidBody* body;
	SimplePhysicsEntity* entity;
	unsigned entityId;
	unsigned rigidBodyIndex, entityIndex;
	std::map<unsigned, unsigned>::iterator it;

	if (!articulatedBody || !env)
		return false;

	TransformationData worldTrans = getWorldTransformation();
	TransformationData offsetTrans = articulatedBody->getOffsetTransformation();
	invert(offsetTrans);
	TransformationData result;
	multiply(result, worldTrans, offsetTrans);
	articulatedBody->setTransformation(result);

	//TODO: The solution with the following RigidBodies is a big HACK!!!
	for (it = rigidBodyMapping.begin(); it != rigidBodyMapping.end(); ++it)
	{
		rigidBodyIndex = (*it).first;
		entityIndex = (*it).second;
		entityId = this->environmentBasedId + entityIndex;
		entity = (SimplePhysicsEntity*)WorldDatabase::getEntityWithEnvironmentId(entityId);
		assert(entity);
		body = articulatedBody->getRigidBodyByID(rigidBodyIndex);
		entity->setRigidBody(body, true);
	} // for

	return true;
} // initialize

void ArticulatedPhysicsEntity::setRigidBodyMapping(unsigned rigidBodyIndex, unsigned succeedingEntityIndex)
{
	rigidBodyMapping[rigidBodyIndex] = succeedingEntityIndex;
} // setRigidBodyMapping

//***************************************************//
// PROTECTED METHODS INNHERITED FROM: PhysicsEntity: //
//***************************************************//

//bool ArticulatedPhysicsEntity::handleMessage(unsigned function, NetMessage* msg)
//{
//	bool unknown = false;
//	gmtl::Vec3f v1, v2;
//	bool b1, b2;
//
//	switch(function)
//	{
//		case FUNCTION_addForceAtPosition:
//			msgFunctions::decode(v1, msg);
//			msgFunctions::decode(v2, msg);
//			msgFunctions::decode(b1, msg);
//			msgFunctions::decode(b2, msg);
//			addForceAtPosition_SERVER(v1, v2, b1, b2);
//			break;
//		default:
//			unknown = true;
//			break;
//	} // switch
//	return !unknown;
//} // handleMessage
//
//void ArticulatedPhysicsEntity::handleSyncObjectTransformationMsg(NetMessage* msg) {
//
//} // handleSyncObjectTransformationMsg
