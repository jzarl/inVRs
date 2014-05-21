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

#include "FullSynchronisationModel.h"

#include <set>

#include "Physics.h"
#include "PhysicsObjectManager.h"
#include "PhysicsObjectInterface.h"
#include "PhysicsMessageFunctions.h"
#ifdef INVRS_BUILD_TIME
#include "oops/RigidBodyMethodData.h"
#include "oops/Simulation.h"
#else
#include <inVRs/Modules/3DPhysics/oops/RigidBodyMethodData.h>
#include <inVRs/Modules/3DPhysics/oops/Simulation.h>
#endif

#include <inVRs/SystemCore/SystemCore.h>

const unsigned FullSynchronisationModel::FULLSYNCHRONISATION_MESSAGEID = 1;

using namespace oops;

//*****************//
// PUBLIC METHODS: //
//*****************//

FullSynchronisationModel::FullSynchronisationModel()
{
	counter = new MessageSizeCounter(physics);
	counter->setLogFileName("FullSyncModel.log");
	RigidBody::setInputListener(this);
} // FullSynchronisationModel

FullSynchronisationModel::~FullSynchronisationModel()
{
	counter->dumpResults();
	delete counter;
	RigidBody::setInputListener(NULL);
} // ~FullSynchronisationModel

//*******************************************************//
// PUBLIC METHODS INHERITED FROM: SynchronisationModel: *//
//*******************************************************//

void FullSynchronisationModel::synchroniseBeforeStep()
{
	//TODO: add message sorting to only use latest message!!!
	// nothing to do here
} // synchroniseBeforeStep

void FullSynchronisationModel::synchroniseAfterStep()
{
	int i, size;
	NetMessage* msg;
//	std::vector<PhysicsObjectInterface*> objects;
	std::vector<RigidBody*> rigidBodies;
	TransformationData rigidBodyTrans;

	getLocalSimulatedRigidBodies(objectManager, rigidBodies);
//	objectManager->getLocalSimulatedRigidBodies(rigidBodies);

	size = rigidBodies.size();
	if (size <= 0) {
		counter->stepFinished();
		return;
	}

	msg = new NetMessage;
	msgFunctions::encode((unsigned)MSGTYPE_SYNC, msg);
	msgFunctions::encode(FULLSYNCHRONISATION_MESSAGEID, msg);
	msgFunctions::encode((unsigned)physics->getSimulationTime(), msg);

	for (i=0; i < size; i++) {
		msgFunctions::encode(rigidBodies[i]->getID(), msg);
		rigidBodyTrans = rigidBodies[i]->getTransformation();
		msgFunctions::encode(rigidBodyTrans.position, msg);
		msgFunctions::encode(rigidBodyTrans.orientation, msg);
	} // for

	network->sendMessageUDP(msg, PHYSICS_MODULE_ID);

	counter->countBytes(msg);
	counter->stepFinished();

	delete msg;
} // synchroniseAfterStep

void FullSynchronisationModel::handleSyncMessage(NetMessage* msg) {

	//TODO: add message ordering!!!

//	int i;
//	uint64_t rigidBodyId;
	TransformationData trans = identityTransformation();
	unsigned simulationTime;
	OopsObjectID rigidBodyID;
	RigidBody* rigidBody = NULL;
//	std::vector<RigidBody*> rigidBodies;
	unsigned syncTypeID;

	msgFunctions::decode(syncTypeID, msg);
	if (syncTypeID != FULLSYNCHRONISATION_MESSAGEID) {
		printd(ERROR, "FullSynchronisationModel::handleSyncMessage(): got sync-message for invalid SynchronisationModel (ID=%u)!\n");
		return;
	} // if
	msgFunctions::decode(simulationTime, msg);
//			printd(INFO, "\nSingleServerPhysicsObjectManager::handleMessages(): SIMULATION TIME DT: %u\n\n", currentSimulationTime - simulationTime);

	while (!msg->finished())
	{
		msgFunctions::decode(rigidBodyID, msg);
		rigidBody = getRigidBodyById(objectManager, rigidBodyID);

		if (!rigidBody) {
			printd(WARNING, "FullSynchronisationModel::handleSyncMessage(): sync-message for unknown RigidBody found!\n");
			// keep the system alive, decode data for unknown rigid body
			msgFunctions::decode(trans.position, msg);
			msgFunctions::decode(trans.orientation, msg);
			break;
		} // if

		msgFunctions::decode(trans.position, msg);
		msgFunctions::decode(trans.orientation, msg);
		rigidBody->setTransformation_unchecked(trans, true);
	} // while

} // handleSyncMessage

void FullSynchronisationModel::handleClientInputMessage(NetMessage* msg) {
	OopsObjectID objectID;
	RigidBody* rigidBody;
	unsigned simulationTime;
	unsigned syncTypeID;
	unsigned method;

	msgFunctions::decode(syncTypeID, msg);
	if (syncTypeID != FULLSYNCHRONISATION_MESSAGEID) {
		printd(ERROR, "FullSynchronisationModel::handleClientInputMessage(): got message for invalid SynchronisationModel (ID=%u)!\n", syncTypeID);
		return;
	} // if

	msgFunctions::decode(simulationTime, msg);
	msgFunctions::decode(objectID, msg);
	msgFunctions::decode(method, msg);

	if (!objectID.getIsJoint() && objectManager->isSimulatingRigidBody(objectID)) {
		rigidBody = getRigidBodyById(objectManager, objectID);
		if (rigidBody) {
			callMethod(rigidBody, (RIGIDBODY_METHODTYPE)method, msg, false);
		} // if
		else
			printd(ERROR, "FullSynchronisationModel::handleClientInputMessage(): got message for non existing RigidBody!\n");
	} // if
} // handleClientInputMessage

void FullSynchronisationModel::handleRigidBodyInput(RigidBody* rigidBody, RIGIDBODY_METHODTYPE method, MethodParameter* parameter) {

	//TODO: NOT THREADSAFE YET!!!!!
	//      IS NOT CRITICAL YET SINCE SINGLESERVER_OBJECTMANAGER ONLY CHECKS FOR BOOLEAN VALUE
	//      WHICH DOES NOT CHANGE DURING RUNTIME
	if (objectManager->isSimulatingRigidBody(rigidBody->getID()))
		return;

	NetMessage* msg = new NetMessage();
	msgFunctions::encode((unsigned)MSGTYPE_CLIENTINPUT, msg);
	msgFunctions::encode(FULLSYNCHRONISATION_MESSAGEID, msg);
	msgFunctions::encode((unsigned)physics->getSimulationTime(), msg);
	msgFunctions::encode(rigidBody->getID(), msg);
	msgFunctions::encode((unsigned)method, msg);
	parameter->encode(msg);

	counter->countBytes(msg);
	network->sendMessageUDP(msg, PHYSICS_MODULE_ID);
} // handleRigidBodyInput
