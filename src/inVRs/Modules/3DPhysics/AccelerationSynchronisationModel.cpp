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

#include "AccelerationSynchronisationModel.h"

#include <set>
#include <gmtl/AxisAngle.h>
#include <gmtl/Generate.h>

#include "Physics.h"
#include "PhysicsObjectManager.h"
#include "PhysicsObjectInterface.h"
#include "PhysicsMessageFunctions.h"

#include <inVRs/SystemCore/SystemCore.h>

const unsigned AccelerationSynchronisationModel::ACCELERATIONSYNCHRONISATION_MESSAGEID = 4;

using namespace oops;

//*****************//
// PUBLIC METHODS: //
//*****************//

AccelerationSynchronisationModel::AccelerationSynchronisationModel(float linearThreshold, 
		float angularThreshold,	int convergenceAlgorithm, float convergenceTime)
: VelocitySynchronisationModel(linearThreshold, angularThreshold, convergenceAlgorithm, convergenceTime)
{
	counter->setLogFileName("AccelerationSyncModel.log");
	usedSyncModel = ACCELERATIONSYNCHRONISATION_MESSAGEID;
}


AccelerationSynchronisationModel::~AccelerationSynchronisationModel()
{
	std::map<uint64_t, ExtendedRigidBodyState*>::iterator it1;

	for (it1 = lastExtendedStateMap.begin(); it1 != lastExtendedStateMap.end(); ++it1) {
		if (it1->second)
			delete it1->second;
	} // for
	lastExtendedStateMap.clear();
} // ~AccelerationSynchronisationModel


//*******************************************************//
// PUBLIC METHODS INHERITED FROM: SynchronisationModel: *//
//*******************************************************//

void AccelerationSynchronisationModel::synchroniseBeforeStep() {

	VelocitySynchronisationModel::synchroniseBeforeStep();

	int i, size;
//	std::vector<PhysicsObjectInterface*> objects;
	std::vector<RigidBody*> rigidBodies;
	gmtl::Vec3f linearVel;

	getLocalSimulatedRigidBodies(objectManager, rigidBodies);
//	objectManager->getLocalSimulatedObjects(objects);
	
	size = rigidBodies.size();
	if (size <= 0)
		return;

	for (i=0; i < size; i++) {
		rigidBodies[i]->getLinearVelocity(linearVel);
		lastVelocity[rigidBodies[i]->getID()] = linearVel;
	} // for
} // synchroniseBeforeStep

//********************//
// PROTECTED METHODS: //
//********************//

bool AccelerationSynchronisationModel::calculatePrediction(gmtl::Vec3f& newPos, 
		gmtl::Quatf& newOri, RigidBody* rigidBody, unsigned simulationTime) {

	ExtendedRigidBodyState* lastState = lastExtendedStateMap[rigidBody->getID()];
	
	if (lastState == NULL) {
		updatePredictionState(rigidBody, simulationTime);
		return false;
	} // if
	
	return calculatePredictionFromState(newPos, newOri, lastState, simulationTime);
} // calculatePrediction

bool AccelerationSynchronisationModel::calculatePredictionFromState(gmtl::Vec3f& newPos, 
		gmtl::Quatf& newOri, ExtendedRigidBodyState* rigidBodyState, 
		unsigned simulationTime) {

	float dt;
	gmtl::Vec3f rotationAxis;
	gmtl::Vec3f angularChange;
	gmtl::AxisAnglef rotationChange;
	gmtl::Quatf deltaRotation;
	
	if (simulationStepSize == 0)
		simulationStepSize = physics->getStepSize();
	
	dt = (simulationTime-rigidBodyState->simulationTime) * simulationStepSize;

	newPos = rigidBodyState->position + rigidBodyState->linearVel*dt + rigidBodyState->acceleration*(dt*dt)/2.f;

	angularChange = rigidBodyState->angularVel*dt;
	rotationAxis = angularChange;
	gmtl::normalize(rotationAxis);
	rotationChange = gmtl::AxisAnglef(gmtl::length(angularChange), rotationAxis);
	gmtl::set(deltaRotation, rotationChange);
	
	newOri = rigidBodyState->orientation * deltaRotation;
	
	return true;
} // calculatePrediction

void AccelerationSynchronisationModel::initiateLinearConvergence(RigidBody* rigidBody, 
		ExtendedRigidBodyState* oldState, ExtendedRigidBodyState* newState, unsigned simulationTime) {

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

void AccelerationSynchronisationModel::updatePredictionState(RigidBody* rigidBody, 
		unsigned simulationTime, bool isInactive) {

	gmtl::Vec3f linearVel, angularVel, acceleration, lastLinearVel;
	uint64_t rigidBodyID = rigidBody->getID();
	ExtendedRigidBodyState* rigidBodyState = lastExtendedStateMap[rigidBodyID];

	if (simulationStepSize == 0)
		simulationStepSize = physics->getStepSize();

	gmtl::Vec3f zeroVec(0,0,0);
	
	if (!rigidBodyState) {
		rigidBodyState = new ExtendedRigidBodyState;
		lastExtendedStateMap[rigidBodyID] = rigidBodyState;
		isInactive = true;
	}  // if
		
	TransformationData rigidBodyTrans = rigidBody->getTransformation();
	if (!isInactive) {
		rigidBody->getLinearVelocity(linearVel);
		rigidBody->getAngularVelocity(angularVel);
		lastLinearVel = lastVelocity[rigidBodyID];
		acceleration = (linearVel - lastLinearVel) * (1.f/simulationStepSize);
	} // if
	else {
		linearVel = zeroVec;
		angularVel = zeroVec;
		acceleration = zeroVec;
	} // else
	
	// UPDATE LAST RIGID BODY STATE
	rigidBodyState->simulationTime = simulationTime;
	rigidBodyState->position = rigidBodyTrans.position;
	rigidBodyState->orientation = rigidBodyTrans.orientation;
	rigidBodyState->linearVel = linearVel;
	rigidBodyState->angularVel = angularVel;
	rigidBodyState->acceleration = acceleration;
} // updatePredictionState

void AccelerationSynchronisationModel::encodePredictionState(RigidBody* rigidBody, NetMessage* msg) {

	ExtendedRigidBodyState* rigidBodyState = lastExtendedStateMap[rigidBody->getID()];
	
	msgFunctions::encode(rigidBody->getID(), msg);
	msgFunctions::encode(rigidBodyState->position, msg);
	msgFunctions::encode(rigidBodyState->orientation, msg);
	msgFunctions::encode(rigidBodyState->linearVel, msg);
	msgFunctions::encode(rigidBodyState->angularVel, msg);
	msgFunctions::encode(rigidBodyState->acceleration, msg);
} // encodePredictionState


//**********************************************************//
// PROTECTED METHODS INHERITED FROM: SynchronisationModel: *//
//**********************************************************//

void AccelerationSynchronisationModel::handleSyncMessage(NetMessage* msg) {
	TransformationData trans = identityTransformation();
	unsigned simulationTime;
//	PhysicsObjectID destinationObjectID;
//	PhysicsObjectInterface* object = NULL;
//	std::vector<RigidBody*> rigidBodies;
	RigidBody* rigidBody;
	ExtendedRigidBodyState* rigidBodyState;
	ExtendedRigidBodyState newRigidBodyState;
	uint64_t rigidBodyID;
	unsigned localSimulationTime = physics->getSimulationTime();
	static bool firstRun = true;
	unsigned syncTypeID;

	msgFunctions::decode(syncTypeID, msg);
	if (syncTypeID != ACCELERATIONSYNCHRONISATION_MESSAGEID) {
		printd(ERROR, "AccelerationSynchronisationModel::handleSyncMessage(): got sync-message for invalid SynchronisationModel (ID=%u)!\n");
		return;
	} // if
	
	msgFunctions::decode(simulationTime, msg);
//			printd(INFO, "\nSingleServerPhysicsObjectManager::handleMessages(): SIMULATION TIME DT: %u\n\n", currentSimulationTime - simulationTime);
	
	while (!msg->finished())
	{
		msgFunctions::decode(rigidBodyID, msg);
		rigidBody = getRigidBodyById(objectManager, rigidBodyID);
		
		if (!rigidBody) {
			printd(ERROR, "AccelerationSynchronisationModel::handleSyncMessage(): sync-message for unknown RigidBody found!\n"); 
			// keep the system alive, decode data for unknown rigid body
			msgFunctions::decode(trans.position, msg);
			msgFunctions::decode(trans.orientation, msg);
			msgFunctions::decode(trans.position, msg);
			msgFunctions::decode(trans.position, msg);
			msgFunctions::decode(trans.position, msg);
			break;
		} // if

		rigidBodyState = lastExtendedStateMap[rigidBodyID];
		if (rigidBodyState == NULL) {
			rigidBodyState = new ExtendedRigidBodyState;
			lastExtendedStateMap[rigidBodyID] = rigidBodyState;
		} // if
		
		if (localSimulationTime < simulationTime) {
			if (firstRun) {
				printd(ERROR, 
					"AccelerationSynchronisationModel::handleSyncMesage(): NOT SYNCED CLOCKS (USE NTP): message sent %f seconds in future!\n", 
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
		msgFunctions::decode(newRigidBodyState.acceleration, msg);
		
		if (convergenceAlgorithm == LINEAR && convergenceTime > 0)
			initiateLinearConvergence(rigidBody, rigidBodyState, &newRigidBodyState, localSimulationTime);
		
		rigidBodyState->simulationTime = newRigidBodyState.simulationTime;
		rigidBodyState->position = newRigidBodyState.position;
		rigidBodyState->orientation = newRigidBodyState.orientation;
		rigidBodyState->linearVel = newRigidBodyState.linearVel;
		rigidBodyState->angularVel = newRigidBodyState.angularVel;
		rigidBodyState->acceleration = newRigidBodyState.acceleration;
		
//		trans.position = rigidBodyState->position;
//		trans.orientation = rigidBodyState->orientation;
//		rigidBodies[0]->setTransformation(trans, true);
	} // while
	
} // handleSyncMessage

void AccelerationSynchronisationModel::handleClientInputMessage(NetMessage* msg) {
	printd(WARNING, "AccelerationSynchronisationModel::handleClientInputMessage(): not implemented yet!!!\n");
} // handleClientInputMessage
