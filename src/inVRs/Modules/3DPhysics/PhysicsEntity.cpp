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

#include "PhysicsEntity.h"

#include "PhysicsObjectManager.h"
#include "PhysicsMessageFunctions.h"

#include <inVRs/SystemCore/MessageFunctions.h>
#include <inVRs/SystemCore/TransformationManager/TransformationPipe.h>
#include <inVRs/SystemCore/TransformationManager/TransformationManager.h>
#include <inVRs/SystemCore/TransformationManager/TransformationManagerEvents.h>

//******************//
// PUBLIC METHODS: *//
//******************//

PhysicsEntity::PhysicsEntity(unsigned short id, unsigned short environmentId,
	unsigned short instanceId, EntityType *type)
	: Entity(id, environmentId, instanceId, type)
{
	physicsTransformationPipe = NULL;
	objectManager = NULL;
	physicsPipeCreationInitiated = false;
} // PhysicsEntity

PhysicsEntity::~PhysicsEntity()
{
	if (physicsTransformationPipe)
		closePhysicsTransformationPipe();
} // ~PhysicsEntity

void PhysicsEntity::setPhysicsTransformationPipe(TransformationPipe* pipe)
{
	// TODO: ADDED FOR TESTING PURPOSES
		unsigned short envId, idInEnv;
		split(environmentBasedId, envId, idInEnv);
		printd(INFO, "PhysicsEntity::setPhysicsTransformationPipe(): setting Physics-Pipe for Entity with ID %i in environment with ID %i\n", idInEnv, envId);
	// DONE

	this->physicsTransformationPipe = pipe;
} // setPhysicsTransformationPipe

TransformationPipe* PhysicsEntity::getPhysicsTransformationPipe()
{
	return physicsTransformationPipe;
} // getPhysicsTransformationPipe

void PhysicsEntity::setPhysicsObjectManager(PhysicsObjectManager* objectManager)
{
	this->objectManager = objectManager;
} // setPhysicsObjectManager

//*******************************************************//
// PUBLIC METHODS INHERITED FROM: PhysicsObjectInterface //
//*******************************************************//

//bool PhysicsEntity::handleMessage(NetMessage* msg)
//{
//	bool error = false;
//	int function;
//	msgFunctions::decode(function, msg);
//
//	if (function < 0)
//	{
//		switch(function)
//		{
//			case FUNCTION_syncAllToClient:
//				handleSyncAllToClientMsg(msg);
//				break;
//			case FUNCTION_syncAllToDRClient:
//				handleSyncAllToDRClientMsg(msg);
//				break;
//			case FUNCTION_syncAllToServer:
//				handleSyncAllToServerMsg(msg);
//				break;
//			case FUNCTION_syncChangesToClient:
//				handleSyncChangesToClientMsg(msg);
//				break;
//			case FUNCTION_syncChangesToDRClient:
//				handleSyncChangesToDRClientMsg(msg);
//				break;
//			case FUNCTION_syncChangesToServer:
//				handleSyncChangesToServerMsg(msg);
//				break;
//			case FUNCTION_syncObjectTransformation:
//				handleSyncObjectTransformationMsg(msg);
//				break;
//			default:
//				printd(ERROR, "PhysicsEntity::handleMessage(): unknown function type %i!\n", function);
//				error = true;
//				break;
//		} // switch
//		return !error;
//	} // if
//	else
//		return handleMessage((unsigned)function, msg);
//} // handleMessage

//********************//
// PROTECTED METHODS: //
//********************//

void PhysicsEntity::setPhysicsTransformation(TransformationData trans)
{
	physicsTransformation = trans;
	if (!physicsTransformationPipe && !physicsPipeCreationInitiated)
		openPhysicsTransformationPipe();
	else if (!physicsTransformationPipe) // && physicsPipeCreationInitiated
		requestPhysicsTransformationPipe();

	if (physicsTransformationPipe)
		physicsTransformationPipe->push_back(trans);
} // setPhysicsTransformation

void PhysicsEntity::setEntityWorldTransformation(TransformationData trans)
{
	Entity::setWorldTransformation(trans);
} // setEntityWorldTransformation

void PhysicsEntity::openPhysicsTransformationPipe()
{
	unsigned short type, id;
	User* localUser;

	if (physicsTransformationPipe)
	{
		printd(WARNING, "PhysicsEntity::openPhysicsTransformationPipe(): Pipe already opened! Ignoring!\n");
		return;
	} // if

	split(typeBasedId, type, id);
	localUser  = UserDatabase::getLocalUser();

// TODO: ADDED FOR TESTING PURPOSES
	unsigned short envId, idInEnv;
	split(environmentBasedId, envId, idInEnv);
	printd(INFO, "PhysicsEntity::openPhysicsTransformationPipe(): opening Physics-Pipe for Entity with ID %i in environment with ID %i\n", idInEnv, envId);
// DONE

	TransformationManagerOpenPipeEvent* event = new TransformationManagerOpenPipeEvent(PHYSICS_MODULE_ID, WORLD_DATABASE_ID,
			2, 0, type, id, 0x0D000000, localUser->getId(), true);
	EventManager::sendEvent(event, EventManager::EXECUTE_LOCAL);

	physicsPipeCreationInitiated = true;
//	physicsTransformationPipe = TransformationManager::openMTPipe(PHYSICS_MODULE_ID, WORLD_DATABASE_ID, 2, 0, type, id, 0, localUser->getId());
} // openPhysicsTransformationPipe

void PhysicsEntity::requestPhysicsTransformationPipe()
{
	unsigned short type, id;
	User* localUser;

	split(typeBasedId, type, id);
	localUser  = UserDatabase::getLocalUser();

	physicsTransformationPipe = TransformationManager::getPipe(PHYSICS_MODULE_ID, WORLD_DATABASE_ID, 2, 0, type, id, 0, false, localUser);
} // requestPhysicsTransformationPipe

void PhysicsEntity::closePhysicsTransformationPipe()
{
	// TODO: ADDED FOR TESTING PURPOSES
		unsigned short envId, idInEnv;
		split(environmentBasedId, envId, idInEnv);
		printd(INFO, "PhysicsEntity::closePhysicsTransformationPipe(): closing Physics-Pipe for Entity with ID %i in environment with ID %i\n", idInEnv, envId);
	// DONE

	if (!physicsTransformationPipe)
	{
		printd(WARNING, "PhysicsEntity::closePhysicsTransformationPipe(): No open pipe found! Ignoring!\n");
		return;
	} // if

// NOTE: Method is (hopefully) always called out of application thread so no event is necessary

//	TransformationManagerClosePipeEvent* event = new TransformationManagerClosePipeEvent(physicsTransformationPipe);
//	EventManager::sendEvent(event, EventManager::EXECUTE_LOCAL);
	TransformationManager::closePipe(physicsTransformationPipe);
	physicsTransformationPipe = NULL;
} // closePhysicsTransformationPipe

TransformationData PhysicsEntity::getPhysicsTransformation()
{
	return physicsTransformation;
} // getPhysicsTransformation

//void PhysicsEntity::handleSyncAllToClientMsg(NetMessage* msg)
//{
//	TransformationData newTrans;
//	msgFunctions::decode(newTrans, msg);
//	// Calling method to write result directly to pipe
//	setPhysicsTransformation(newTrans);
//} // handleSyncAllToClientMsg
//
//void PhysicsEntity::handleSyncAllToDRClientMsg(NetMessage* msg)
//{
//// TODO: implement
//} // handleSyncAllToDRClientMsg
//
//void PhysicsEntity::handleSyncAllToServerMsg(NetMessage* msg)
//{
//// TODO: implement
//} // handleSyncAllToServerMsg
//
//void PhysicsEntity::handleSyncChangesToClientMsg(NetMessage* msg)
//{
//// TODO: implement
//} // handleSyncChangesToClientMsg
//
//void PhysicsEntity::handleSyncChangesToDRClientMsg(NetMessage* msg)
//{
//// TODO: implement
//} // handleSyncChangesToDRClientMsg
//
//void PhysicsEntity::handleSyncChangesToServerMsg(NetMessage* msg)
//{
//// TODO: implement
//} // handleSyncChangesToServerMsg

//**********************************************************//
// PROTECTED METHODS INHERITED FROM: PhysicsObjectInterface //
//**********************************************************//

//void PhysicsEntity::synchroniseAllToClient(NetMessage* msg)
//{
//	msgFunctions::encode((int)FUNCTION_syncAllToClient, msg);
//	msgFunctions::encode(physicsTransformation, msg);
//} // synchroniseAllToClient
//
//void PhysicsEntity::synchroniseAllToDRClient(NetMessage* msg)
//{
//	msgFunctions::encode((int)FUNCTION_syncAllToDRClient, msg);
////TODO: implement
//} // synchroniseAllToDRClient
//
//void PhysicsEntity::synchroniseAllToServer(NetMessage* msg)
//{
//	msgFunctions::encode((int)FUNCTION_syncAllToServer, msg);
////TODO: implement
//} // synchroniseAllToServer
//
//void PhysicsEntity::synchroniseChangesToClient(NetMessage* msg)
//{
//	msgFunctions::encode((int)FUNCTION_syncChangesToClient, msg);
////TODO: implement
//} // synchroniseChangesToClient
//
//void PhysicsEntity::synchroniseChangesToDRClient(NetMessage* msg)
//{
//	msgFunctions::encode((int)FUNCTION_syncChangesToDRClient, msg);
////TODO: implement
//} // synchroniseChangesToDRClient
//
//void PhysicsEntity::synchroniseChangesToServer(NetMessage* msg)
//{
//	msgFunctions::encode((int)FUNCTION_syncChangesToServer, msg);
////TODO: implement
//} // synchroniseChangesToServer
//
//void PhysicsEntity::synchroniseObjectTransformation(NetMessage* msg) {
//	msgFunctions::encode((int)FUNCTION_syncObjectTransformation, msg);
//} // synchroniseObjectTransformation
//
//std::vector<RigidBody*>& PhysicsEntity::getRigidBodiesForSync(
//		std::vector<RigidBody*> &rigidBodyList) {
//	return rigidBodyList;
//} // getRigidBodiesForSync
