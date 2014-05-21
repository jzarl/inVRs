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

#include "SynchronisationModel.h"

#include <assert.h>
#include <set>

#include "PhysicsMessageFunctions.h"
#include "PhysicsObjectManager.h"
#include "Physics.h"

#include <inVRs/SystemCore/SystemCore.h>

using namespace oops;

//******************//
// PUBLIC METHODS: *//
//******************//

SynchronisationModel::SynchronisationModel()
{
	objectManager = NULL;
	network = (NetworkInterface*)SystemCore::getModuleByName("Network");
	assert(network);
//	messageListLock = dynamic_cast<Lock*>(ThreadManager::the()->getLock("SynchronisationModel::messageListLock"));
//	assert(messageListLock);
	physics = (Physics*)SystemCore::getModuleByName("Physics");
	assert(physics);
} // SynchronisationModel

SynchronisationModel::~SynchronisationModel()
{

} // ~SynchronisationModel

//*********************//
// PROTECTED METHODS: *//
//*********************//

void SynchronisationModel::handleMessages() {
	int i;
	NetMessage* msg;
	unsigned messageType;

	int messageCount = network->sizeRecvList(PHYSICS_MODULE_ID);

//	currentSimulationTime = physics->getSimulationTime();

//	messageListLock->aquire();

		for (i=0; i < messageCount; i++)
			messageList.push_back(network->pop(PHYSICS_MODULE_ID));


		for (i=0; i < (int)messageList.size(); i++)
		{
			msg = messageList[i];
			msgFunctions::decode(messageType, msg);
			
			if (messageType == MSGTYPE_SYNC)
				handleSyncMessage(msg);
			else if (messageType == MSGTYPE_CLIENTINPUT)
				handleClientInputMessage(msg);
			else
				printd(ERROR, "SynchronisationModel::handleMessages(); unknown message tag %i found!\n", messageType);
			
			delete msg;
		} // for
		messageList.clear();
//	messageListLock->release();
} // handleMessages

void SynchronisationModel::setPhysicsObjectManager(PhysicsObjectManager* objectManager)
{
	this->objectManager = objectManager;
} // setPhysicsObjectManager

//void SynchronisationModel::synchroniseAllToClient(PhysicsObjectInterface* obj, NetMessage* msg)
//{
//	obj->synchroniseAllToClient(msg);
//} // synchroniseAllToClient
//
//void SynchronisationModel::synchroniseAllToDRClient(PhysicsObjectInterface* obj, NetMessage* msg)
//{
//	obj->synchroniseAllToDRClient(msg);
//} // synchroniseAllToDRClient
//
//void SynchronisationModel::synchroniseAllToServer(PhysicsObjectInterface* obj, NetMessage* msg)
//{
//	obj->synchroniseAllToServer(msg);
//} // synchroniseAllToServer
//
//void SynchronisationModel::synchroniseChangesToClient(PhysicsObjectInterface* obj, NetMessage* msg)
//{
//	obj->synchroniseChangesToClient(msg);
//} // synchroniseChangesToClient
//
//void SynchronisationModel::synchroniseChangesToDRClient(PhysicsObjectInterface* obj, NetMessage* msg)
//{
//	obj->synchroniseChangesToDRClient(msg);
//} // synchroniseChangesToDRClient
//
//void SynchronisationModel::synchroniseChangesToServer(PhysicsObjectInterface* obj, NetMessage* msg)
//{
//	obj->synchroniseChangesToServer(msg);
//} // synchroniseChangesToServer
//
//void SynchronisationModel::synchroniseObjectTransformation(PhysicsObjectInterface* obj, NetMessage* msg) {
//	obj->synchroniseObjectTransformation(msg);
//} // synchroniseObjectTransformation
//
//void SynchronisationModel::getRigidBodiesForSync(PhysicsObjectInterface* obj, std::vector<RigidBody*> &rigidBodyList) {
//	obj->getRigidBodiesForSync(rigidBodyList);
//} // getRigidBodiesForSync

bool SynchronisationModel::needPhysicsCalculation() {
	return false;
} // needPhysicsCalculation

void SynchronisationModel::getLocalSimulatedRigidBodies(PhysicsObjectManager* manager, std::vector<RigidBody*>& dst) {
	manager->getLocalSimulatedRigidBodies(dst);
} // getLocalSimulatedRigidBodies

void SynchronisationModel::getRemoteSimulatedRigidBodies(PhysicsObjectManager* manager, std::vector<RigidBody*>& dst) {
	manager->getRemoteSimulatedRigidBodies(dst);
} // getRemoteSimulatedRigidBodies

void SynchronisationModel::getLocalSimulatedJoints(PhysicsObjectManager* manager, std::vector<Joint*>& dst) {
	manager->getLocalSimulatedJoints(dst);
} // getLocalSimulatedJoints
 
void SynchronisationModel::getRemoteSimulatedJoints(PhysicsObjectManager* manager, std::vector<Joint*>& dst) {
	manager->getRemoteSimulatedJoints(dst);
} // getRemoteSimulatedJoints

RigidBody* SynchronisationModel::getRigidBodyById(PhysicsObjectManager* manager, OopsObjectID id) {
	return manager->getRigidBodyById(id);
} // getRigidBodyById

Joint* SynchronisationModel::getJointById(PhysicsObjectManager* manager, OopsObjectID id) {
	return manager->getJointById(id);
} // getJointById

void SynchronisationModel::setSimulationTime(unsigned simulationTime) {
	if (physics)
		physics->simulationTime = simulationTime;
	else
		printd(WARNING, "SynchronisationModel::setSimulationTime(): Physics module not set! Ignoring new simulationTime!\n");
} // setSimulationTime
