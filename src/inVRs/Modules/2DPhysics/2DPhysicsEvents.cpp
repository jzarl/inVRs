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

#include "2DPhysicsEvents.h"
#include "Simulation2D.h"
#include <inVRs/SystemCore/TransformationManager/TransformationManager.h>
#include <inVRs/SystemCore/TransformationManager/TransformationPipe.h>
#include <inVRs/SystemCore/SystemCore.h>

//Event* Physics2DSyncEventFactory::create(unsigned eventId)
//{
//	if (eventId == Physics2DSyncType)
//		return new Physics2DSyncEvent(NULL);
//
//	return NULL;
//}

Physics2DSyncEvent::Physics2DSyncEvent()
: Event(PHYSICS_MODULE_ID, TRANSFORMATION_MANAGER_ID, "Physics2DSyncEvent")
{
	bool fromNetwork;
	TransformationPipe* pipe;
	TransformationPipeIdData* data;
	Simulation2D* simulation = (Simulation2D*)SystemCore::getModuleByName("Simulation2D");
	assert(simulation);
	std::map<RigidBody*, TransformationPipe*>::iterator it;

	for (it = simulation->transformationPipes.begin(); it != simulation->transformationPipes.end(); ++it)
	{
		data = new TransformationPipeIdData;
		pipe = (*it).second;
		TransformationManager::unpackPipeId(pipe->getPipeId(), &(data->srcId), &(data->dstId), &(data->pipeType), &(data->objectClass), &(data->objectType), &(data->objectId), &fromNetwork);
		data->priority = pipe->getPriority();
		data->userId = pipe->getOwner()->getId();
		pipeList.push_back(data);
	} // for
} // Physics2DSyncEvent

Physics2DSyncEvent::Physics2DSyncEvent(void* dummy)
: Event(PHYSICS_MODULE_ID, TRANSFORMATION_MANAGER_ID, "Physics2DSyncEvent")
{
 // for
}// Physics2DSyncEvent

void Physics2DSyncEvent::decode(NetMessage* message)
{
	int i;
	unsigned numberOfPipes;
	TransformationPipeIdData* data;
	message->getUInt32(numberOfPipes);

	for (i=0; i < (int)numberOfPipes; i++)
	{
		data = new TransformationPipeIdData;
		message->getUInt32(data->srcId);
		message->getUInt32(data->dstId);
		message->getUInt32(data->pipeType);
		message->getUInt32(data->objectClass);
		message->getUInt32(data->objectType);
		message->getUInt32(data->objectId);
		message->getUInt32(data->priority);
		message->getUInt32(data->userId);
		pipeList.push_back(data);
	}
}

Physics2DSyncEvent::~Physics2DSyncEvent()
{
	int i;
	for (i=0; i < (int)pipeList.size(); i++)
		delete pipeList[i];
	pipeList.clear();
} // Physics2DSyncEvent

void Physics2DSyncEvent::encode(NetMessage* message)
{
	int i;
	TransformationPipeIdData* data;

	message->putUInt32(pipeList.size());
	for (i=0; i < (int)pipeList.size(); i++)
	{
		data = pipeList[i];
		message->putUInt32(data->srcId);
		message->putUInt32(data->dstId);
		message->putUInt32(data->pipeType);
		message->putUInt32(data->objectClass);
		message->putUInt32(data->objectType);
		message->putUInt32(data->objectId);
		message->putUInt32(data->priority);
		message->putUInt32(data->userId);
	} // for
} // encode

void Physics2DSyncEvent::execute()
{
// 	printd("JUHU the sync-event is here!\n");
	int i;
	User* user;
	bool fromNetwork;
	TransformationPipeIdData* data;
//	unsigned localUserId = UserDatabase::getLocalUserId();

	for (i=0; i < (int)pipeList.size(); i++)
	{
		data = pipeList[i];
 		user = UserDatabase::getUserById(data->userId);
		fromNetwork = true;
		TransformationManager::openPipe(data->srcId, data->dstId, data->pipeType, data->objectClass, data->objectType, data->objectId, data->priority, fromNetwork, user);
	} // for
} // execute
