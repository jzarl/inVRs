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

#include "PhysicsInputSynchronisationModel.h"

#include <set>

#include "MessageSizeCounter.h"

#include "Physics.h"
#include "PhysicsObjectManager.h"
#include "PhysicsObjectInterface.h"
#include "PhysicsMessageFunctions.h"

//#include <inVRs/SystemCore/SystemCore.h>

const unsigned PhysicsInputSynchronisationModel::PHYSICSINPUTSYNCHRONISATION_MESSAGEID = 6;

using namespace oops;

//*****************//
// PUBLIC METHODS: //
//*****************//

PhysicsInputSynchronisationModel::PhysicsInputSynchronisationModel(float updateInterval,
		float inputUpdateInterval) {
	this->updateInterval = updateInterval;
	this->inputUpdateInterval = inputUpdateInterval;
	timeUntilNextUpdate = updateInterval;
	timeUntilFrequentUpdate = inputUpdateInterval;
	simulationStepSize = 0;
	counter = new MessageSizeCounter(physics);
	counter->setLogFileName("PhysicsInputSyncModel.log");
	RigidBody::setInputListener(this);
} // PhysicsInputSynchronisationModel

PhysicsInputSynchronisationModel::~PhysicsInputSynchronisationModel() {
	counter->dumpResults();
	delete counter;
	RigidBody::setInputListener(NULL);
} // ~PhysicsInputSynchronisationModel


//*******************************************************//
// PUBLIC METHODS INHERITED FROM: SynchronisationModel: *//
//*******************************************************//

void PhysicsInputSynchronisationModel::synchroniseBeforeStep() {
	handleBufferedSyncMessages();
} // synchroniseBeforeStep

void PhysicsInputSynchronisationModel::synchroniseAfterStep() {

	int i, size;
	NetMessage* msg;
//	std::vector<PhysicsObjectInterface*> objects;
	std::vector<RigidBody*> rigidBodies;
	std::vector<Joint*> joints;
	TransformationData rigidBodyTrans;
	gmtl::Vec3f linearVel;
	gmtl::Vec3f angularVel;
	std::set<RigidBody*> addedRigidBodies;

	if (simulationStepSize == 0)
		simulationStepSize = physics->getStepSize();

	timeUntilNextUpdate -= simulationStepSize;
	timeUntilFrequentUpdate -= simulationStepSize;

	if ((modifiedRigidBodies.size() == 0 || timeUntilFrequentUpdate > 0) && timeUntilNextUpdate > 0) {
		counter->stepFinished();
		return;
	} // if

	getLocalSimulatedRigidBodies(objectManager, rigidBodies);
	getLocalSimulatedJoints(objectManager, joints);
//	objectManager->getLocalSimulatedObjects(objects);

	size = rigidBodies.size();
	if (size <= 0)
		return;

	msg = new NetMessage;
	msgFunctions::encode((unsigned)MSGTYPE_SYNC, msg);
	msgFunctions::encode(PHYSICSINPUTSYNCHRONISATION_MESSAGEID, msg);
	msgFunctions::encode((unsigned)physics->getSimulationTime(), msg);
	uint64_t seed = dRandGetSeed();
	msgFunctions::encode(seed, msg);

	if (timeUntilNextUpdate > 0) {
		for (i=0; i < (int)modifiedRigidBodies.size(); i++) {

			if (addedRigidBodies.find(modifiedRigidBodies[i]) != addedRigidBodies.end())
				continue;

			msgFunctions::encode(modifiedRigidBodies[i]->getID(), msg);

			rigidBodyTrans = modifiedRigidBodies[i]->getTransformation();
			modifiedRigidBodies[i]->getLinearVelocity(linearVel);
			modifiedRigidBodies[i]->getAngularVelocity(angularVel);
			msgFunctions::encode(rigidBodyTrans.position, msg);
			msgFunctions::encode(rigidBodyTrans.orientation, msg);
			msgFunctions::encode(linearVel, msg);
			msgFunctions::encode(angularVel, msg);
			addedRigidBodies.insert(modifiedRigidBodies[i]);
		} // for
		modifiedRigidBodies.clear();
		timeUntilFrequentUpdate = inputUpdateInterval;
	} else {

		for (i=0; i < size; i++) {
			msgFunctions::encode(rigidBodies[i]->getID(), msg);

			rigidBodyTrans = rigidBodies[i]->getTransformation();
			rigidBodies[i]->getLinearVelocity(linearVel);
			rigidBodies[i]->getAngularVelocity(angularVel);
			msgFunctions::encode(rigidBodyTrans.position, msg);
			msgFunctions::encode(rigidBodyTrans.orientation, msg);
			msgFunctions::encode(linearVel, msg);
			msgFunctions::encode(angularVel, msg);
		} // for
		timeUntilNextUpdate = updateInterval;
	} // else

	network->sendMessageUDP(msg, PHYSICS_MODULE_ID);

	counter->countBytes(msg);
	counter->stepFinished();

	delete msg;
} // synchroniseAfterStep

void PhysicsInputSynchronisationModel::handleBufferedSyncMessages() {
	unsigned minSimTime;
	int i, index;
	SyncData* nextSyncData;

	unsigned localSimulationTime = physics->getSimulationTime();

	while (immediateUpdates.size() > 0) {
		minSimTime = 0;
		for (i=0; i < (int)immediateUpdates.size(); i++) {
			if (minSimTime == 0 || minSimTime > immediateUpdates[i]->simulationTime) {
				nextSyncData = immediateUpdates[i];
				index = i;
				minSimTime = nextSyncData->simulationTime;
			} // if
		} // for
		applySyncData(nextSyncData, localSimulationTime);
		delete nextSyncData;
		immediateUpdates.erase(immediateUpdates.begin()+index);
		lastUpdateTime = minSimTime;
	} // while

	if (storedUpdates.find(localSimulationTime) != storedUpdates.end()) {
		nextSyncData = storedUpdates[localSimulationTime];
		dRandSetSeed((unsigned long)nextSyncData->randomSeed);
		applySyncData(nextSyncData, localSimulationTime);
		delete nextSyncData;
		storedUpdates.erase(storedUpdates.find(localSimulationTime));
		lastUpdateTime = localSimulationTime;
	} // if
} // handleBufferedSyncMessages

void PhysicsInputSynchronisationModel::applySyncData(SyncData* nextSyncData, unsigned localSimulationTime) {
	int i;

	TransformationData trans = identityTransformation();
	RigidBodyState* nextSyncState;
	RigidBody* rigidBody;

	for (i=0; i < (int)nextSyncData->states.size(); i++) {
		nextSyncState = nextSyncData->states[i];
		rigidBody = getRigidBodyById(objectManager, nextSyncState->rigidBodyID);

		if (!rigidBody) {
			printd(ERROR, "VelocitySynchronisationModel::handleBufferedSyncMessages(): sync-message for unknown RigidBody found!\n");
			// keep the system alive, decode data for unknown rigid body
			delete nextSyncState;
			break;
		} // if

		trans.position = nextSyncState->position;
		trans.orientation = nextSyncState->orientation;
		rigidBody->setTransformation(trans, true);
		rigidBody->setLinearVelocity(nextSyncState->linearVel);
		rigidBody->setAngularVelocity(nextSyncState->angularVel);
		delete nextSyncState;
	} // for

	nextSyncData->states.clear();

} // applySyncData


//**********************************************************//
// PROTECTED METHODS INHERITED FROM: SynchronisationModel: *//
//**********************************************************//

void PhysicsInputSynchronisationModel::handleSyncMessage(NetMessage* msg) {
	unsigned simulationTime;
	unsigned localSimulationTime = physics->getSimulationTime();
	unsigned syncTypeID;
	SyncData* data;
	RigidBodyState* state;
	static bool timerAdjustment = false;

	if (timerAdjustment) {
		printd(INFO, "VelocitySynchronisationModel::handleSyncMessage(): resetting timer to normal speed!\n");
		Physics::physicsTimer.setSpeed(1.0);
		timerAdjustment = false;
	} // if

	msgFunctions::decode(syncTypeID, msg);
	if (syncTypeID != PHYSICSINPUTSYNCHRONISATION_MESSAGEID) {
		printd(ERROR, "PhysicsInputSynchronisationModel::handleSyncMessage(): got sync-message for invalid SynchronisationModel (ID=%u)!\n");
		return;
	} // if

	msgFunctions::decode(simulationTime, msg);
	data = new SyncData;
	data->simulationTime = simulationTime;
	msgFunctions::decode(data->randomSeed, msg);
	while (!msg->finished())
	{
		state = new RigidBodyState;
		msgFunctions::decode(state->rigidBodyID, msg);
		msgFunctions::decode(state->position, msg);
		msgFunctions::decode(state->orientation, msg);
		msgFunctions::decode(state->linearVel, msg);
		msgFunctions::decode(state->angularVel, msg);
		data->states.push_back(state);
	} // while

	if (simulationTime > localSimulationTime + 20) {
		printd(INFO, "VelocitySynchronisationModel::handleSyncMessage(): Found large local delay of %i steps -> WARPING!\n", simulationTime-localSimulationTime);
		setSimulationTime(simulationTime);
		immediateUpdates.push_back(data);
	} // if
	else if (simulationTime > localSimulationTime + 2) {
		printd(INFO, "VelocitySynchronisationModel::handleSyncMessage(): Found local delay of %i steps! Accelerating timer!\n", simulationTime - localSimulationTime);
		Physics::physicsTimer.setSpeed(1.05);
		storedUpdates[simulationTime] = data;
		timerAdjustment = true;
	} // if
	else if (simulationTime < localSimulationTime) {
		printd(INFO, "VelocitySynchronisationModel::handleSyncMessage(): Found delayed sync-message of %i steps! Slowing down timer!\n", localSimulationTime - simulationTime);
		Physics::physicsTimer.setSpeed(0.5);
		immediateUpdates.push_back(data);
		timerAdjustment = true;
	} // else if
	else {
		storedUpdates[simulationTime] = data;
	} // else

} // handleSyncMessage

/*
void PhysicsInputSynchronisationModel::handleSyncMessage(NetMessage* msg) {

	TransformationData trans = identityTransformation();
	unsigned simulationTime;
//	PhysicsObjectID destinationObjectID;
//	PhysicsObjectInterface* object = NULL;
//	std::vector<RigidBody*> rigidBodies;
	RigidBody* rigidBody;
	uint64_t rigidBodyID;
	gmtl::Vec3f linearVel, angularVel;
	unsigned syncTypeID;

	msgFunctions::decode(syncTypeID, msg);
	if (syncTypeID != PHYSICSINPUTSYNCHRONISATION_MESSAGEID) {
		printd(ERROR, "PhysicsInputSynchronisationModel::handleSyncMessage(): got sync-message for invalid SynchronisationModel (ID=%u)!\n");
		return;
	} // if

	msgFunctions::decode(simulationTime, msg);

	while (!msg->finished())
	{
		msgFunctions::decode(rigidBodyID, msg);
		rigidBody = getRigidBodyById(objectManager, rigidBodyID);

		if (!rigidBody) {
			printd(ERROR, "PhysicsInputSynchronisationModel::handleSyncMessage(): sync-message for unknown RigidBody found!\n");
			// keep the system alive, decode data for unknown rigid body
			msgFunctions::decode(trans.position, msg);
			msgFunctions::decode(trans.orientation, msg);
			msgFunctions::decode(trans.position, msg);
			msgFunctions::decode(trans.position, msg);
			break;
		} // if

		msgFunctions::decode(trans.position, msg);
		msgFunctions::decode(trans.orientation, msg);
		msgFunctions::decode(linearVel, msg);
		msgFunctions::decode(angularVel, msg);
		rigidBody->setTransformation(trans, true);
		rigidBody->setLinearVelocity(linearVel);
		rigidBody->setAngularVelocity(angularVel);
	} // while

} // handleSynchronisationModel
*/
void PhysicsInputSynchronisationModel::handleClientInputMessage(NetMessage* msg) {
	OopsObjectID objectID;
	RigidBody* rigidBody;
	unsigned simulationTime;
	unsigned syncTypeID;
	unsigned method;

	msgFunctions::decode(syncTypeID, msg);
	if (syncTypeID != PHYSICSINPUTSYNCHRONISATION_MESSAGEID) {
		printd(ERROR, "PhysicsInputSynchronisationModel::handleClientInputMessage(): got message for invalid SynchronisationModel (ID=%u)!\n", syncTypeID);
		return;
	} // if

	msgFunctions::decode(simulationTime, msg);
	msgFunctions::decode(objectID, msg);
	msgFunctions::decode(method, msg);

	if (!objectID.getIsJoint() && !objectManager->isSimulatingRigidBody(objectID)) {
		rigidBody = getRigidBodyById(objectManager, objectID);
		if (rigidBody) {
			callMethod(rigidBody, (RIGIDBODY_METHODTYPE)method, msg, false);
		} // if
		else
			printd(ERROR, "PhysicsInputSynchronisationModel::handleClientInputMessage(): got message for non existing RigidBody!\n");
	} // if

} // handleClientInputMessage

bool PhysicsInputSynchronisationModel::needPhysicsCalculation() {
	return true;
} // needPhysicsCalculation

void PhysicsInputSynchronisationModel::handleRigidBodyInput(RigidBody* rigidBody, RIGIDBODY_METHODTYPE method, MethodParameter* parameter) {
	//TODO: implement client-input!!!!

	//TODO: NOT THREADSAFE YET!!!!!
	if (!objectManager->isSimulatingRigidBody(rigidBody->getID()))
		return;

	modifiedRigidBodies.push_back(rigidBody);
//	NetMessage* msg = new NetMessage();
//	msgFunctions::encode((unsigned)MSGTYPE_CLIENTINPUT, msg);
//	msgFunctions::encode(PHYSICSINPUTSYNCHRONISATION_MESSAGEID, msg);
//	msgFunctions::encode((unsigned)physics->getSimulationTime(), msg);
//	msgFunctions::encode(rigidBody->getID(), msg);
//	msgFunctions::encode((unsigned)method, msg);
//	parameter->encode(msg);
//
//	counter->countBytes(msg);
//	network->sendMessageUDP(msg, PHYSICS_MODULE_ID);

} // handleRigidBodyInput
