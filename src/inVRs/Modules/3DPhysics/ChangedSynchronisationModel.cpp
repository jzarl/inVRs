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

#include "ChangedSynchronisationModel.h"

#include <set>

#include "Physics.h"
#include "PhysicsObjectManager.h"
#include "PhysicsObjectInterface.h"
#include "PhysicsMessageFunctions.h"

#include <inVRs/SystemCore/SystemCore.h>


const unsigned ChangedSynchronisationModel::CHANGEDSYNCHRONISATION_MESSAGEID = 2;

using namespace oops;

//*****************//
// PUBLIC METHODS: //
//*****************//

ChangedSynchronisationModel::ChangedSynchronisationModel()
{
	counter = new MessageSizeCounter(physics);
	counter->setLogFileName("ChangedSyncModel.log");
} // ChangedSynchronisationModel

ChangedSynchronisationModel::~ChangedSynchronisationModel()
{
	counter->dumpResults();
	delete counter;
} // ~ChangedSynchronisationModel

//*******************************************************//
// PUBLIC METHODS INHERITED FROM: SynchronisationModel: *//
//*******************************************************//

void ChangedSynchronisationModel::synchroniseBeforeStep()
{
	// nothing to do here
} // synchroniseBeforeStep

void ChangedSynchronisationModel::synchroniseAfterStep()
{
	int i, size;
	uint64_t rigidBodyID;
	NetMessage* msg;
//	std::vector<PhysicsObjectInterface*> objects;
	RigidBody* rigidBody;
	std::vector<RigidBody*> rigidBodies;
	TransformationData rigidBodyTrans;
	std::map<uint64_t, int>::iterator it;

	getLocalSimulatedRigidBodies(objectManager, rigidBodies);
//	objectManager->getLocalSimulatedObjects(objects);
	
	size = rigidBodies.size();
	if (size <= 0)
		return;

	msg = new NetMessage;
	msgFunctions::encode((unsigned)MSGTYPE_SYNC, msg);
	msgFunctions::encode(CHANGEDSYNCHRONISATION_MESSAGEID, msg);
	msgFunctions::encode((unsigned)physics->getSimulationTime(), msg);

	for (i=0; i < size; i++) {
		rigidBody = rigidBodies[i];
		rigidBodyID = rigidBody->getID();
		if (rigidBody->isFixed() || !rigidBody->isActive()) {
			it = inactiveRepeatSend.find(rigidBodyID);

			if (it == inactiveRepeatSend.end())
				inactiveRepeatSend[rigidBodyID] = 3;
			else if (it->second == 0)
				continue;
			else
				inactiveRepeatSend[rigidBodyID] = it->second - 1;
		} // if
		else
			inactiveRepeatSend[rigidBodyID] = 3;
			
		msgFunctions::encode(rigidBody->getID(), msg);
		rigidBodyTrans = rigidBody->getTransformation();
		msgFunctions::encode(rigidBodyTrans.position, msg);
		msgFunctions::encode(rigidBodyTrans.orientation, msg);
	} // for
	
	network->sendMessageUDP(msg, PHYSICS_MODULE_ID);

	counter->countBytes(msg);
	counter->stepFinished();
	
	delete msg;
} // synchroniseAfterStep

void ChangedSynchronisationModel::handleSyncMessage(NetMessage* msg) {
	TransformationData trans = identityTransformation();
	unsigned simulationTime;
	OopsObjectID rigidBodyID;
	RigidBody* rigidBody = NULL;
//	std::vector<RigidBody*> rigidBodies;
	unsigned syncTypeID;

	msgFunctions::decode(syncTypeID, msg);
	if (syncTypeID != CHANGEDSYNCHRONISATION_MESSAGEID) {
		printd(ERROR, "ChangedSynchronisationModel::handleSyncMessage(): got sync-message for invalid SynchronisationModel (ID=%u)!\n");
		return;
	} // if

	msgFunctions::decode(simulationTime, msg);
//			printd(INFO, "\nSingleServerPhysicsObjectManager::handleMessages(): SIMULATION TIME DT: %u\n\n", currentSimulationTime - simulationTime);
	
	while (!msg->finished())
	{
		msgFunctions::decode(rigidBodyID, msg);
		rigidBody = getRigidBodyById(objectManager, rigidBodyID);
		
		if (!rigidBody) {
			printd(ERROR, "ChangedSynchronisationModel::handleSyncMessage(): sync-message for unknown RigidBody found!\n"); 
			// keep the system alive, decode data for unknown rigid body
			msgFunctions::decode(trans.position, msg);
			msgFunctions::decode(trans.orientation, msg);
			break;
		} // if
		msgFunctions::decode(trans.position, msg);
		msgFunctions::decode(trans.orientation, msg);
		rigidBody->setTransformation(trans, true);
	} // while
	
} // handleSyncMessage

void ChangedSynchronisationModel::handleClientInputMessage(NetMessage* msg) {
	printd(WARNING, "ChangedSynchronisationModel::handleClientInputMessage(): not implemented yet!!!\n");
} // handleClientInputMessage
