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

#include "PhysicsFullSynchronisationModel.h"

#include <gmtl/Generate.h>

#include "MessageSizeCounter.h"

#include "Physics.h"
#include "PhysicsObjectManager.h"
#include "PhysicsObjectInterface.h"
#include "PhysicsMessageFunctions.h"

//#include <inVRs/SystemCore/SystemCore.h>

const unsigned PhysicsFullSynchronisationModel::PHYSICSFULLSYNCHRONISATION_MESSAGEID = 5;

using namespace oops;

//*****************//
// PUBLIC METHODS: //
//*****************//

PhysicsFullSynchronisationModel::PhysicsFullSynchronisationModel(unsigned updateInterval,
		int convergenceAlgorithm, float convergenceTime) {

	this->updateInterval = updateInterval;
	this->convergenceAlgorithm = convergenceAlgorithm;
	this->convergenceTime = convergenceTime;
	simulationStepSize = 0;
	stepsUntilUpdate = updateInterval;
	counter = new MessageSizeCounter(physics);
	counter->setLogFileName("PhysicsFullSyncModel.log");
} // PhysicsFullSynchronisationModel

PhysicsFullSynchronisationModel::~PhysicsFullSynchronisationModel() {
	counter->dumpResults();
	delete counter;
} // ~PhysicsFullSynchronisationModel


//*******************************************************//
// PUBLIC METHODS INHERITED FROM: SynchronisationModel: *//
//*******************************************************//

void PhysicsFullSynchronisationModel::synchroniseBeforeStep() {
	
} // synchroniseBeforeStep

void PhysicsFullSynchronisationModel::synchroniseAfterStep() {

	handleServerSynchronisation();
	handleClientSynchronisation();
} // synchroniseAfterStep

void PhysicsFullSynchronisationModel::handleServerSynchronisation() {
	int i, size;
	NetMessage* msg;
//	std::vector<PhysicsObjectInterface*> objects;
	std::vector<RigidBody*> rigidBodies;
	std::vector<Joint*> joints;
	TransformationData rigidBodyTrans;
	gmtl::Vec3f linearVel;
	gmtl::Vec3f angularVel;

	stepsUntilUpdate--;
	
	if (stepsUntilUpdate > 0) {
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
	msgFunctions::encode(PHYSICSFULLSYNCHRONISATION_MESSAGEID, msg);
	msgFunctions::encode((unsigned)physics->getSimulationTime(), msg);

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

	network->sendMessageUDP(msg, PHYSICS_MODULE_ID);

	counter->countBytes(msg);
	counter->stepFinished();
	
	stepsUntilUpdate = updateInterval;
	
	delete msg;
} // synchroniseAfterStep

void PhysicsFullSynchronisationModel::handleClientSynchronisation() {
	int i, size;
	TransformationData prediction = identityTransformation();
	unsigned simulationTime;
	std::vector<RigidBody*> rigidBodies;
	TransformationWriterInterface* writer;
	bool result;

	if (convergenceAlgorithm == SNAPPING)
		return;
	
	getRemoteSimulatedRigidBodies(objectManager, rigidBodies);
//	objectManager->getRemoteSimulatedObjects(objects);
	
	size = rigidBodies.size();
	if (size <= 0)
		return;

	simulationTime = physics->getSimulationTime();

	for (i=0; i < size; i++) {
		if (convergenceAlgorithm == LINEAR) {
			result = calculateLinearConvergence(prediction.position, prediction.orientation, rigidBodies[i], simulationTime);
		} // if
		else if (convergenceAlgorithm == QUADRIC) {
			result = calculateQuadricConvergence(prediction.position, prediction.orientation, rigidBodies[i], simulationTime);
		} // else if
		else {
			printd(ERROR, "PhysicsFullSynchronisationModel::synchroniseBeforeStep(): invalid convergence-Algorithm %i set!\n", (int)convergenceAlgorithm);
			result = false;
		} // else

		if (result) {
			writer = rigidBodies[i]->getTransformationWriter();
			if (writer)
				writer->updateTransformation(prediction);
		} // if
	} // for

} // handleClientSynchronisation

bool PhysicsFullSynchronisationModel::calculateLinearConvergence(gmtl::Vec3f& newPos, 
		gmtl::Quatf& newOri, RigidBody* rigidBody, unsigned simulationTime) {
	
	float dt;
	float percentage;
	float remainingTime;
	TransformationData trans;
	gmtl::Vec3f linearVel, angularVel;
	TransformationData targetTrans = identityTransformation();
	RigidBodyState* state = convergenceState[rigidBody->getID()];
	
	if (!state)
		return false;
	
	if (simulationStepSize == 0)
		simulationStepSize = physics->getStepSize();

	dt = (simulationTime-state->simulationTime) * simulationStepSize;

	if (dt >= convergenceTime)
		return false;
	
	percentage = dt / convergenceTime;
	remainingTime = convergenceTime - dt;

	rigidBody->getTransformation(trans);
	rigidBody->getLinearVelocity(linearVel);
	rigidBody->getAngularVelocity(angularVel);
	calculateFutureTransformation(targetTrans, trans, linearVel, angularVel, remainingTime);
	
	gmtl::lerp(newPos, percentage, state->position, targetTrans.position);	
	gmtl::slerp(newOri, percentage, state->orientation, targetTrans.orientation);

	return true;
} // calculatePredictionLinear

bool PhysicsFullSynchronisationModel::calculateQuadricConvergence(gmtl::Vec3f& newPos,
		gmtl::Quatf& newOri, RigidBody* rigidBody, unsigned simulationTime) {
	
	float dt;
	float percentage;
	float remainingTime;
	TransformationData trans = identityTransformation();
	TransformationData rigidBodyTrans;
	gmtl::Vec3f linearVel, angularVel;
	TransformationData targetTrans = identityTransformation();
	RigidBodyState* state = convergenceState[rigidBody->getID()];
	
	if (!state)
		return false;
	
	if (simulationStepSize == 0)
		simulationStepSize = physics->getStepSize();

	dt = (simulationTime-state->simulationTime) * simulationStepSize;

	if (dt >= convergenceTime)
		return false;
	
	percentage = dt / convergenceTime;
	remainingTime = convergenceTime - dt;

	trans.position = state->position;
	trans.orientation = state->orientation;
	
	calculateFutureTransformation(targetTrans, trans, state->linearVel, state->angularVel, remainingTime);
	
	rigidBody->getTransformation(rigidBodyTrans);
	
	gmtl::lerp(newPos, percentage, targetTrans.position, rigidBodyTrans.position);	
	gmtl::slerp(newOri, percentage, targetTrans.orientation, rigidBodyTrans.orientation);

	return true;
} // calculateQuadricConvergence

void PhysicsFullSynchronisationModel::calculateFutureTransformation(TransformationData& result, 
		TransformationData trans, gmtl::Vec3f linearVel, gmtl::Vec3f angularVel,
		float dt) {

	gmtl::Vec3f rotationAxis;
	gmtl::Vec3f angularChange;
	gmtl::AxisAnglef rotationChange;
	gmtl::Quatf deltaRotation;

	result = trans;
	result.position += linearVel*dt;

	angularChange = angularVel*dt;
	rotationAxis = angularChange;
	gmtl::normalize(rotationAxis);
	rotationChange = gmtl::AxisAnglef(gmtl::length(angularChange), rotationAxis);
	gmtl::set(deltaRotation, rotationChange);
	
	result.orientation *= deltaRotation;
} // calculateFutureTransformation

//**********************************************************//
// PROTECTED METHODS INHERITED FROM: SynchronisationModel: *//
//**********************************************************//
	
void PhysicsFullSynchronisationModel::handleSyncMessage(NetMessage* msg) {

	TransformationData trans = identityTransformation();
	TransformationData currentTrans;
	unsigned simulationTime;
//	PhysicsObjectID destinationObjectID;
//	PhysicsObjectInterface* object = NULL;
//	std::vector<RigidBody*> rigidBodies;
	RigidBody* rigidBody;
	RigidBodyState* state;
	uint64_t rigidBodyID;
	gmtl::Vec3f linearVel, angularVel;
	gmtl::Vec3f currentLinearVel, currentAngularVel;
	unsigned syncTypeID;
	unsigned localSimulationTime = physics->getSimulationTime();

	msgFunctions::decode(syncTypeID, msg);
	if (syncTypeID != PHYSICSFULLSYNCHRONISATION_MESSAGEID) {
		printd(ERROR, "PhysicsFullSynchronisationModel::handleSyncMessage(): got sync-message for invalid SynchronisationModel (ID=%u)!\n");
		return;
	} // if

	msgFunctions::decode(simulationTime, msg);
	
	while (!msg->finished())
	{
		msgFunctions::decode(rigidBodyID, msg);
		rigidBody = getRigidBodyById(objectManager, rigidBodyID);
		
		if (!rigidBody) {
			printd(ERROR, "PhysicsFullSynchronisationModel::handleSyncMessage(): sync-message for unknown RigidBody found!\n"); 
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
		
		if (convergenceAlgorithm == SNAPPING) {
			rigidBody->setTransformation(trans, true);
			rigidBody->setLinearVelocity(linearVel);
			rigidBody->setAngularVelocity(angularVel);
		} // if
		else if (convergenceAlgorithm == LINEAR || convergenceAlgorithm == QUADRIC) {
			state = convergenceState[rigidBodyID];
			if (!state) {
				state = new RigidBodyState;
				convergenceState[rigidBodyID] = state;
			} // if

			rigidBody->getTransformation(currentTrans);
			rigidBody->getLinearVelocity(currentLinearVel);
			rigidBody->getAngularVelocity(currentAngularVel);
			
			rigidBody->setTransformation(trans, false);
			rigidBody->setLinearVelocity(linearVel);
			rigidBody->setAngularVelocity(angularVel);
			
			state->position = currentTrans.position;
			state->orientation = currentTrans.orientation;
			state->linearVel = currentLinearVel;
			state->angularVel = currentAngularVel;
			state->simulationTime = localSimulationTime;
		} // else if
		else
			printd(ERROR, "PhysicsFullSynchronisationModel::handleSyncMessage(): unknown convergenceAlgorithm %i set!\n", convergenceAlgorithm);
	} // while

} // handleSynchronisationModel

void PhysicsFullSynchronisationModel::handleClientInputMessage(NetMessage* msg) {
	
} // handleClientInputMessage

bool PhysicsFullSynchronisationModel::needPhysicsCalculation() {
	return true;
} // needPhysicsCalculation
