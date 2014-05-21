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

#include "SynchronisePhysicsEvent.h"

#include <assert.h>

#include "Physics.h"
#include "PhysicsObjectManager.h"
#include "PhysicsMessageFunctions.h"

#include <inVRs/SystemCore/Timer.h>
#include <inVRs/SystemCore/SystemCore.h>
#include <inVRs/SystemCore/TransformationManager/TransformationManager.h>

//*****************//
// PUBLIC METHODS: //
//*****************//

SynchronisePhysicsEvent::SynchronisePhysicsEvent()
: Event(PHYSICS_MODULE_ID, PHYSICS_MODULE_ID, "SynchronisePhysicsEvent")
{
	objectManagerMsg = NULL;
	simulationStartTime = Physics::singleton->simulationStartTime;
	simulationTime = Physics::singleton->simulationTime;
	stepSize = Physics::singleton->stepSize;
}

SynchronisePhysicsEvent::~SynchronisePhysicsEvent()
{
	if (objectManagerMsg)
		delete objectManagerMsg;
} // ~SynchronisePhysicsEvent

void SynchronisePhysicsEvent::addPhysicsObjectManagerMsg(NetMessage* msg)
{
	this->objectManagerMsg = msg;
} // addPhysicsObjectManagerMsg

//****************************//
// DEPRECATED PUBLIC METHODS: //
//****************************//

// void SynchronisePhysicsEvent::addPipe(TransformationPipe* pipe)
// {
// 	PipeData *data = new PipeData;
// 	data->pipeId = pipe->getPipeId();
// 	data->priority = pipe->getPriority();
// 	data->userId = pipe->getOwner()->getId();
// 	pipeList.push_back(data);
// } // addPipe

//**************************************//
// PUBLIC METHODS INHERITED FROM: Event //
//**************************************//

void SynchronisePhysicsEvent::encode(NetMessage* message)
{
	printd(INFO, "SynchronisePhysicsEvent::encode(): BEGIN\n");
	std::map<unsigned, UserData*>::iterator it;

	Physics* physicsModule = Physics::singleton;
	assert(physicsModule);

	msgFunctions::encode(simulationStartTime, message);
	msgFunctions::encode(simulationTime, message);
	msgFunctions::encode(stepSize, message);

//	msgFunctions::encode((unsigned)((physicsModule->userDataMap).size()), message);
//	for (it = physicsModule->userDataMap.begin(); it != physicsModule->userDataMap.end(); ++it)
//	{
//		msgFunctions::encode(it->first, message);
//		msgFunctions::encode(it->second, message);
//	} // for

	assert(objectManagerMsg);
	message->appendMessage(objectManagerMsg);

	printd(INFO, "SynchronisePhysicsEvent::encode(): DONE\n");
} // encode

void SynchronisePhysicsEvent::decode(NetMessage* message)
{
	printd(INFO, "SynchronisePhysicsEvent::decode(): BEGIN\n");

	int i;
	unsigned numUserData;
	unsigned userId;
	UserData* data;

	msgFunctions::decode(simulationStartTime, message);
	msgFunctions::decode(simulationTime, message);
	msgFunctions::decode(stepSize, message);

//	msgFunctions::decode(numUserData, message);
//	for (i=0; i < (int)numUserData; i++)
//	{
//		data = new UserData;
//		msgFunctions::decode(userId, message);
//		msgFunctions::decode(data, message);
//		userDataMap[userId] = data;
//	} // for

	objectManagerMsg = message->detachMessage();
	assert(objectManagerMsg);

	printd(INFO, "SynchronisePhysicsEvent::decode(): DONE\n");
} // decode

void SynchronisePhysicsEvent::execute()
{
	printd(INFO, "SynchronisePhysicsEvent::execute(): BEGIN\n");

	std::map<unsigned, UserData*>::iterator it;

	Physics* physicsModule = Physics::singleton;
	assert(physicsModule);

	PhysicsObjectManager* objectManager = physicsModule->objectManager;
	assert(objectManager);

//	for (it = userDataMap.begin(); it != userDataMap.end(); ++it)
//		physicsModule->userDataMap[it->first] = it->second;

	objectManager->handleSynchronisationMessage(objectManagerMsg);

	physicsModule->stepSize = stepSize;

	printd(INFO, "SynchronisePhysicsEvent::execute(): SETTING simulationStartTime\n");
//	physicsModule->simulationStartTime = simulationStartTime;

	double systemTime = Physics::physicsTimer.getSystemTime();
	float stepSize = physicsModule->stepSize;

	printd(INFO, "SynchronisePhysicsEvent::execute(): calculating simulationTime: systemTime=%f / simulationStartTime=%f / stepSize=%f!\n", systemTime, simulationStartTime, stepSize);

//	physicsModule->simulationTime = (unsigned)(((physicsModule->physicsTimer.getSystemTime() - simulationStartTime) /  physicsModule->stepSize)+0.5f);

	printd(INFO, "SynchronisePhysicsEvent::execute(): END\n");
} // execute

std::string SynchronisePhysicsEvent::toString()
{
// TODO:
	return Event::toString();
} // toString
