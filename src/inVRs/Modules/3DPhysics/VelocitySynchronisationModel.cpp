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

#include "VelocitySynchronisationModel.h"

#include <set>
#include <gmtl/AxisAngle.h>
#include <gmtl/Generate.h>

#include "Physics.h"
#include "PhysicsObjectManager.h"
#include "PhysicsObjectInterface.h"
#include "PhysicsMessageFunctions.h"

#include <inVRs/SystemCore/SystemCore.h>
#include <inVRs/SystemCore/WorldDatabase/WorldDatabase.h>

const unsigned VelocitySynchronisationModel::VELOCITYSYNCHRONISATION_MESSAGEID = 3;

using namespace oops;

//*****************//
// PUBLIC METHODS: //
//*****************//

VelocitySynchronisationModel::VelocitySynchronisationModel(float linearThreshold,
		float angularThreshold, int convergenceAlgorithm, float convergenceTime)
{
	this->linearThreshold = linearThreshold;
	this->angularThreshold = angularThreshold;
	this->convergenceAlgorithm = convergenceAlgorithm;
	this->convergenceTime = convergenceTime;
	counter = new MessageSizeCounter(physics);
	simulationStepSize = 0;
	counter->setLogFileName("VelocitySyncModel.log");
	usedSyncModel = VELOCITYSYNCHRONISATION_MESSAGEID;
} // VelocitySynchronisationModel

VelocitySynchronisationModel::~VelocitySynchronisationModel()
{
	std::map<uint64_t, RigidBodyState*>::iterator it1;
	std::map<uint64_t, LinearConvergenceState*>::iterator it2;

	counter->dumpResults();
	delete counter;

	for (it1 = lastStateMap.begin(); it1 != lastStateMap.end(); ++it1) {
		if (it1->second)
			delete it1->second;
	} // for
	lastStateMap.clear();

	for (it2 = linearConvMap.begin(); it2 != linearConvMap.end(); ++it2) {
		if (it2->second)
			delete it2->second;
	} // for
	linearConvMap.clear();
} // ~VelocitySynchronisationModel

//*******************************************************//
// PUBLIC METHODS INHERITED FROM: SynchronisationModel: *//
//*******************************************************//

void VelocitySynchronisationModel::synchroniseBeforeStep()
{
	int i, size;
	TransformationData prediction = identityTransformation();
	unsigned simulationTime;
	std::vector<RigidBody*> rigidBodies;
//	std::vector<PhysicsObjectInterface*> objects;
	bool result;

	handleBufferedSyncMessages();

	getRemoteSimulatedRigidBodies(objectManager, rigidBodies);
//	objectManager->getRemoteSimulatedObjects(objects);


	size = rigidBodies.size();
	if (size <= 0)
		return;

	simulationTime = physics->getSimulationTime();

	for (i=0; i < size; i++) {
		if (convergenceAlgorithm == SNAPPING) {
			// SimulationTime has to be increased by one because this method is called before the step!!!
			result = calculatePrediction(prediction.position, prediction.orientation, rigidBodies[i], simulationTime+1);
		} // if
		else if (convergenceAlgorithm == LINEAR) {
			result = calculatePredictionLinear(prediction.position, prediction.orientation, rigidBodies[i], simulationTime+1);
		}
		else {
			printd(ERROR, "VelocitySynchronisationModel::synchroniseBeforeStep(): invalid convergence-Algorithm %i set!\n", (int)convergenceAlgorithm);
			result = false;
		} // else

		if (result)
			rigidBodies[i]->setTransformation(prediction, true);
	} // for

} // synchroniseBeforeStep

void VelocitySynchronisationModel::synchroniseAfterStep()
{
	int i, size;
	NetMessage* msg;
//	std::vector<PhysicsObjectInterface*> objects;
	std::vector<RigidBody*> rigidBodies;
	RigidBody* rigidBody;
	unsigned simulationTime;

	getLocalSimulatedRigidBodies(objectManager, rigidBodies);
//	objectManager->getLocalSimulatedObjects(objects);

	size = rigidBodies.size();
	if (size <= 0)
		return;

	simulationTime = physics->getSimulationTime();

	msg = new NetMessage;
	msgFunctions::encode((unsigned)MSGTYPE_SYNC, msg);
	msgFunctions::encode(usedSyncModel, msg);
	msgFunctions::encode((unsigned)physics->getSimulationTime(), msg);

	for (i=0; i < size; i++) {
		rigidBody = rigidBodies[i];
		if (rigidBody->isFixed() || !rigidBody->isActive()) {
			handleDeactivatedRigidBody(rigidBody, simulationTime, msg);
			continue;
		} // if
		else
			inactiveRepeatSend[rigidBody->getID()] = 3;

		if (!isPredictionStillValid(rigidBody, simulationTime)) {
			updatePredictionState(rigidBody, simulationTime);
			encodePredictionState(rigidBody, msg);
		} // if
	} // for

	network->sendMessageUDP(msg, PHYSICS_MODULE_ID);

	counter->countBytes(msg);
	counter->stepFinished();

	delete msg;
} // synchroniseAfterStep

//********************//
// PROTECTED METHODS: //
//********************//

void VelocitySynchronisationModel::handleDeactivatedRigidBody(RigidBody* rigidBody,
		unsigned simulationTime, NetMessage* msg) {

	std::map<uint64_t, int>::iterator it;
	uint64_t rigidBodyID = rigidBody->getID();
	gmtl::Vec3f zeroVec(0,0,0);

	it = inactiveRepeatSend.find(rigidBodyID);

	if (it == inactiveRepeatSend.end())
		inactiveRepeatSend[rigidBodyID] = 3;
	else if (it->second > 0)
		inactiveRepeatSend[rigidBodyID] = it->second - 1;
	else
		return;

	updatePredictionState(rigidBody, simulationTime, true);
	encodePredictionState(rigidBody, msg);
} // handleDeactivatedRigidBody

bool VelocitySynchronisationModel::isPredictionStillValid(RigidBody* rigidBody,
		unsigned simulationTime) {

	TransformationData currentTransformation;
	gmtl::Vec3f predictedPosition;
	gmtl::Quatf predictedOrientation;
	gmtl::Vec3f deltaPosition;
	gmtl::Quatf deltaOrientation;
	gmtl::AxisAnglef deltaRotation;

	currentTransformation = rigidBody->getTransformation();
	if (!calculatePrediction(predictedPosition, predictedOrientation, rigidBody, simulationTime))
		return false;

	deltaPosition = currentTransformation.position - predictedPosition;
	gmtl::invert(predictedOrientation);
	deltaOrientation = currentTransformation.orientation * predictedOrientation;
	gmtl::set(deltaRotation, deltaOrientation);

	if (gmtl::length(deltaPosition) > linearThreshold ||
			deltaRotation.getAngle() > angularThreshold)
		return false;

	return true;
} // isPredictionStillValid

bool VelocitySynchronisationModel::calculatePrediction(gmtl::Vec3f& newPos,
		gmtl::Quatf& newOri, RigidBody* rigidBody, unsigned simulationTime) {

	RigidBodyState* lastState = lastStateMap[rigidBody->getID()];

	if (lastState == NULL) {
		updatePredictionState(rigidBody, simulationTime);
		return false;
	} // if

	return calculatePredictionFromState(newPos, newOri, lastState, simulationTime);
} // calculatePrediction

bool VelocitySynchronisationModel::calculatePredictionFromState(gmtl::Vec3f& newPos,
		gmtl::Quatf& newOri, RigidBodyState* rigidBodyState, unsigned simulationTime) {

	float dt;
	gmtl::Vec3f rotationAxis;
	gmtl::Vec3f angularChange;
	gmtl::AxisAnglef rotationChange;
	gmtl::Quatf deltaRotation;

	if (simulationStepSize == 0)
		simulationStepSize = physics->getStepSize();

	dt = (simulationTime-rigidBodyState->simulationTime) * simulationStepSize;

	newPos = rigidBodyState->position + rigidBodyState->linearVel*dt;

	angularChange = rigidBodyState->angularVel*dt;
	rotationAxis = angularChange;
	gmtl::normalize(rotationAxis);
	rotationChange = gmtl::AxisAnglef(gmtl::length(angularChange), rotationAxis);
	gmtl::set(deltaRotation, rotationChange);

	newOri = rigidBodyState->orientation * deltaRotation;

	return true;
} // calculatePredictionFromState

bool VelocitySynchronisationModel::calculatePredictionLinear(gmtl::Vec3f& newPos,
		gmtl::Quatf& newOri, RigidBody* rigidBody, unsigned simulationTime) {

	uint64_t rigidBodyID = rigidBody->getID();

/*
	RigidBodyState* lastState = lastStateMap[rigidBodyID];

	if (!lastState) {
		updatePredictionState(rigidBody, simulationTime);
		return false;
	} // if
*/
	LinearConvergenceState* linearConvState	= linearConvMap[rigidBodyID];

	if (linearConvState == NULL)
		return calculatePrediction(newPos, newOri, rigidBody, simulationTime);

	float dt = (simulationTime-linearConvState->startSimulationTime) * simulationStepSize;

	if (dt >= convergenceTime)
		return calculatePrediction(newPos, newOri, rigidBody, simulationTime);

	float percentage = dt / convergenceTime;
	gmtl::lerp(newPos, percentage, linearConvState->startPoint, linearConvState->destPoint);
	gmtl::slerp(newOri, percentage, linearConvState->startOri, linearConvState->destOri);

	return true;
} // calculatePredictionLinear

void VelocitySynchronisationModel::initiateLinearConvergence(RigidBody* rigidBody,
		RigidBodyState* oldState, RigidBodyState* newState, unsigned simulationTime) {

	TransformationData trans;
	uint64_t rigidBodyID = rigidBody->getID();
	LinearConvergenceState* linearConvState;
	unsigned convergenceSimulationTime;

	if (simulationStepSize == 0)
		simulationStepSize = physics->getStepSize();

	linearConvState = linearConvMap[rigidBodyID];
	if (linearConvState == NULL) {
		linearConvState = new LinearConvergenceState;
		linearConvMap[rigidBodyID] = linearConvState;
	} // if

	trans = rigidBody->getTransformation();

	linearConvState->startPoint = trans.position;
	linearConvState->startOri = trans.orientation;

	convergenceSimulationTime = simulationTime + (unsigned)(convergenceTime / simulationStepSize);

	calculatePredictionFromState(linearConvState->destPoint, linearConvState->destOri,
			newState, convergenceSimulationTime);

	linearConvState->startSimulationTime = simulationTime;
} // initiateLinearConvergence

void VelocitySynchronisationModel::updatePredictionState(RigidBody* rigidBody,
		unsigned simulationTime, bool isInactive) {

	gmtl::Vec3f linearVel, angularVel;
	RigidBodyState* rigidBodyState = lastStateMap[rigidBody->getID()];
	gmtl::Vec3f zeroVec(0,0,0);

	if (!rigidBodyState) {
		rigidBodyState = new RigidBodyState;
		lastStateMap[rigidBody->getID()] = rigidBodyState;
		isInactive = true;
	}  // if

	TransformationData rigidBodyTrans = rigidBody->getTransformation();
	if (!isInactive) {
		rigidBody->getLinearVelocity(linearVel);
		rigidBody->getAngularVelocity(angularVel);
	} // if
	else {
		linearVel = zeroVec;
		angularVel = zeroVec;
	} // else

	// UPDATE LAST RIGID BODY STATE
	rigidBodyState->simulationTime = simulationTime;
	rigidBodyState->position = rigidBodyTrans.position;
	rigidBodyState->orientation = rigidBodyTrans.orientation;
	rigidBodyState->linearVel = linearVel;
	rigidBodyState->angularVel = angularVel;
} // updatePredictionState

void VelocitySynchronisationModel::encodePredictionState(RigidBody* rigidBody, NetMessage* msg) {

	RigidBodyState* rigidBodyState = lastStateMap[rigidBody->getID()];

	msgFunctions::encode(rigidBody->getID(), msg);
	msgFunctions::encode(rigidBodyState->position, msg);
	msgFunctions::encode(rigidBodyState->orientation, msg);
	msgFunctions::encode(rigidBodyState->linearVel, msg);
	msgFunctions::encode(rigidBodyState->angularVel, msg);
} // encodePredictionState

void VelocitySynchronisationModel::handleBufferedSyncMessages() {
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
		applySyncData(nextSyncData, localSimulationTime);
		delete nextSyncData;
		storedUpdates.erase(storedUpdates.find(localSimulationTime));
		lastUpdateTime = localSimulationTime;
	} // if
} // handleBufferedSyncMessages

void VelocitySynchronisationModel::applySyncData(SyncData* nextSyncData, unsigned localSimulationTime) {
	int i;

	SyncState* nextSyncState;
	RigidBody* rigidBody;
	RigidBodyState* rigidBodyState;

	for (i=0; i < (int)nextSyncData->states.size(); i++) {
		nextSyncState = nextSyncData->states[i];
		rigidBody = getRigidBodyById(objectManager, nextSyncState->rigidBodyID);

		if (!rigidBody) {
			printd(ERROR, "VelocitySynchronisationModel::handleBufferedSyncMessages(): sync-message for unknown RigidBody found!\n");
			// keep the system alive, decode data for unknown rigid body
			delete nextSyncState;
			break;
		} // if

		rigidBodyState = lastStateMap[nextSyncState->rigidBodyID];
		if (rigidBodyState == NULL) {
			rigidBodyState = new RigidBodyState;
			lastStateMap[nextSyncState->rigidBodyID] = rigidBodyState;
		} // if

		if (convergenceAlgorithm == LINEAR && convergenceTime > 0)
			initiateLinearConvergence(rigidBody, rigidBodyState, &nextSyncState->state, localSimulationTime);

		rigidBodyState->simulationTime = nextSyncState->state.simulationTime;
		rigidBodyState->position = nextSyncState->state.position;
		rigidBodyState->orientation = nextSyncState->state.orientation;
		rigidBodyState->linearVel = nextSyncState->state.linearVel;
		rigidBodyState->angularVel = nextSyncState->state.angularVel;
		delete nextSyncState;
	} // for

	nextSyncData->states.clear();

} // applySyncData


//**********************************************************//
// PROTECTED METHODS INHERITED FROM: SynchronisationModel: *//
//**********************************************************//

void VelocitySynchronisationModel::handleSyncMessage(NetMessage* msg) {

	unsigned syncTypeID;
	unsigned simulationTime;
	unsigned localSimulationTime = physics->getSimulationTime();
	SyncData* data;
	SyncState* state;
	static bool timerAdjustment = false;

	if (timerAdjustment) {
		printd(INFO, "VelocitySynchronisationModel::handleSyncMessage(): resetting timer to normal speed!\n");
		Physics::physicsTimer.setSpeed(1.0);
		timerAdjustment = false;
	} // if

	msgFunctions::decode(syncTypeID, msg);

	if (syncTypeID != VELOCITYSYNCHRONISATION_MESSAGEID) {
		printd(ERROR, "VelocitySynchronisationModel::handleSyncMessage(): got sync-message for invalid SynchronisationModel (ID=%u)!\n");
		return;
	} // if

	msgFunctions::decode(simulationTime, msg);

	data = new SyncData;
	data->simulationTime = simulationTime;
	while (!msg->finished())
	{
		state = new SyncState;
		msgFunctions::decode(state->rigidBodyID, msg);
		msgFunctions::decode(state->state.position, msg);
		msgFunctions::decode(state->state.orientation, msg);
		msgFunctions::decode(state->state.linearVel, msg);
		msgFunctions::decode(state->state.angularVel, msg);
		state->state.simulationTime = simulationTime;
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
void VelocitySynchronisationModel::handleSyncMessage(NetMessage* msg) {
	TransformationData trans = identityTransformation();
	unsigned simulationTime;
//	PhysicsObjectID destinationObjectID;
//	PhysicsObjectInterface* object = NULL;
	RigidBody* rigidBody;
	RigidBodyState* rigidBodyState;
	RigidBodyState newRigidBodyState;
	uint64_t rigidBodyID;
	unsigned localSimulationTime = physics->getSimulationTime();
	static bool firstRun = true;
	unsigned syncTypeID;

	msgFunctions::decode(syncTypeID, msg);
	if (syncTypeID != VELOCITYSYNCHRONISATION_MESSAGEID) {
		printd(ERROR, "VelocitySynchronisationModel::handleSyncMessage(): got sync-message for invalid SynchronisationModel (ID=%u)!\n");
		return;
	} // if

	msgFunctions::decode(simulationTime, msg);
//			printd(INFO, "\nSingleServerPhysicsObjectManager::handleMessages(): SIMULATION TIME DT: %u\n\n", currentSimulationTime - simulationTime);

	while (!msg->finished())
	{
		msgFunctions::decode(rigidBodyID, msg);

//		object = objectManager->getObjectById(destinationObjectID);
		rigidBody = getRigidBodyById(objectManager, rigidBodyID);

		if (!rigidBody) {
			printd(ERROR, "VelocitySynchronisationModel::handleSyncMessage(): sync-message for unknown RigidBody found!\n");
			// keep the system alive, decode data for unknown rigid body
			msgFunctions::decode(trans.position, msg);
			msgFunctions::decode(trans.orientation, msg);
			msgFunctions::decode(trans.position, msg);
			msgFunctions::decode(trans.position, msg);
			break;
		} // if

		rigidBodyState = lastStateMap[rigidBodyID];
		if (rigidBodyState == NULL) {
			rigidBodyState = new RigidBodyState;
			lastStateMap[rigidBodyID] = rigidBodyState;
		} // if

		if (localSimulationTime < simulationTime) {
			if (firstRun) {
				printd(ERROR,
					"VelocitySynchronisationModel::handleSyncMesage(): NOT SYNCED CLOCKS (USE NTP): message sent %f seconds in future!\n",
					(localSimulationTime - simulationTime)*simulationStepSize);
				firstRun = false;
			} // if
			newRigidBodyState.simulationTime = localSimulationTime;
		} // if
		else
			newRigidBodyState.simulationTime = simulationTime;
		msgFunctions::decode(newRigidBodyState.position, msg);
		msgFunctions::decode(newRigidBodyState.orientation, msg);
		msgFunctions::decode(newRigidBodyState.linearVel, msg);
		msgFunctions::decode(newRigidBodyState.angularVel, msg);

		if (convergenceAlgorithm == LINEAR && convergenceTime > 0)
			initiateLinearConvergence(rigidBody, rigidBodyState, &newRigidBodyState, localSimulationTime);

		rigidBodyState->simulationTime = newRigidBodyState.simulationTime;
		rigidBodyState->position = newRigidBodyState.position;
		rigidBodyState->orientation = newRigidBodyState.orientation;
		rigidBodyState->linearVel = newRigidBodyState.linearVel;
		rigidBodyState->angularVel = newRigidBodyState.angularVel;

//		trans.position = rigidBodyState->position;
//		trans.orientation = rigidBodyState->orientation;
//		rigidBodies[0]->setTransformation(trans, true);
	} // while

} // handleSyncMessage
*/

void VelocitySynchronisationModel::handleClientInputMessage(NetMessage* msg) {
	printd(WARNING, "VelocitySynchronisationModel::handleClientInputMessage(): not implemented yet!!!\n");
} // handleClientInputMessage
