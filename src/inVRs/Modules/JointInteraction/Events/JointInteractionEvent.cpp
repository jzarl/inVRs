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

#include "JointInteractionEvent.h"

#include <assert.h>
#include <sstream>

#include "../JointInteraction.h"
#include <inVRs/SystemCore/TransformationManager/TransformationManager.h>
#include <inVRs/SystemCore/WorldDatabase/WorldDatabase.h>
#include <inVRs/SystemCore/UserDatabase/UserDatabase.h>
#include <inVRs/SystemCore/DebugOutput.h>
#include <inVRs/SystemCore/SystemCore.h>

JointInteractionEvent::JointInteractionEvent()
: Event()
{

} // JointInteractionEvent

void JointInteractionEvent::decode(NetMessage* message)
{
	int size, i;

	userId = message->getUInt32();
	size = (int)message->getUInt32();
	printd(INFO, "JointInteractionEvent(): found %u entities!\n", size);
	for (i=0; i < size; i++)
		entityIds.push_back(message->getUInt32());
}

JointInteractionEvent::JointInteractionEvent(unsigned userId, std::vector<unsigned> entityIds, bool bBegin)
: Event(JOINT_INTERACTION_MODULE_ID, JOINT_INTERACTION_MODULE_ID, bBegin ? "JointInteractionBeginEvent" : "JointInteractionEndEvent")
{
	this->userId = userId;
	this->entityIds = entityIds;
	printd(INFO, "JointInteractionEvent(): found %u entities!\n", entityIds.size());
} // JointInteractionEvent

void JointInteractionEvent::encode(NetMessage* message)
{
	int i;

	message->putUInt32(static_cast<uint32_t>(userId));
	message->putUInt32(entityIds.size());
	printd(INFO, "JointInteractionEvent::encode(): encoding %u entities!\n", entityIds.size());
	for (i=0; i < (int)entityIds.size(); i++)
		message->putUInt32(entityIds[i]);

} // encode

JointInteractionBeginEvent::JointInteractionBeginEvent()
: JointInteractionEvent()
{

} // BeginJointInteractionEvent

JointInteractionBeginEvent::JointInteractionBeginEvent(unsigned userId, std::vector<unsigned> entityIds)
: JointInteractionEvent(userId, entityIds, true)
{

} // BeginJointInteractionEvent

void JointInteractionBeginEvent::execute()
{
	printf("JointInteractionBeginEvent::execute(): BEGIN!\n");
	unsigned short type, id;
	std::vector<unsigned>::iterator it;
	bool localPipe;
	User* user = UserDatabase::getUserById(userId);
	TransformationPipe* pipe;
	JointInteraction* interaction = NULL;
	Entity* ent;

	if (UserDatabase::getLocalUser()->getId() == userId)
		localPipe = true;
	else
		localPipe = false;

	if (localPipe)
	{
		interaction = dynamic_cast<JointInteraction*>(SystemCore::getModuleByName("JointInteraction"));
		assert(interaction);
	} // if

	for (it = entityIds.begin(); it != entityIds.end(); ++it)
	{
		split((*it), type, id);
		pipe = TransformationManager::openPipe(JOINT_INTERACTION_MODULE_ID, WORLD_DATABASE_ID, 0, 0, type, id, 0, !localPipe, user);
		if (localPipe)
		{
			ent = WorldDatabase::getEntityWithTypeInstanceId(*it);
			if (!ent)
				printd(ERROR, "JointInteractionEvent::execute(): Can not open Pipe for non existing Entity!\n");
			else
				interaction->registerTransformationPipe(ent->getEnvironmentBasedId(), pipe);
		} // if
	} // for
	printf("JointInteractionBeginEvent::execute(): END!\n");
} // execute

std::string JointInteractionBeginEvent::toString()
{
	std::stringstream ss (Event::toString());
	std::vector<unsigned>::iterator it;

	ss << "\tuserId = " <<  userId << "\n\tentityIds = ";
	for (it = entityIds.begin(); it != entityIds.end(); ++it)
		ss << (*it) << " ";
	ss << std::endl;
	return ss.str();
} // toString

JointInteractionEndEvent::JointInteractionEndEvent()
: JointInteractionEvent()
{

} // EndJointInteractionEvent

void JointInteractionEndEvent::decode(NetMessage* message)
{
	int size, i;
	int jointId;
	float fValue;

	size = (int)message->getUInt32();
	printd(INFO, "JointInteractionEndEvent(): found %i joints!\n", size);
	for (i=0; i < size; i++)
	{
		jointId = message->getUInt32();
		jointIDs.push_back(jointId);
		printd(INFO, "\tJointID = %i\n", jointId);
	} // for

	size = (int)message->getUInt32();
	printd(INFO, "JointInteractionEndEvent(): found %i floatValues!\n", size);
	for (i=0; i < size; i++)
	{
		fValue = message->getReal32();
		floatValues.push_back(fValue);
		printd(INFO, "\tfloatValue = %f\n", fValue);
	} // for

	// EntityIds already in JointInteractionEvent-constructor decoded!
	size = (int)message->getUInt32();
	for (i=0; i < size; i++)
	{
		entityTransformations.push_back(readTransformationFrom(message));
	} // for

	floatIdx = 0;
}

JointInteractionEndEvent::JointInteractionEndEvent(unsigned userId, std::vector<unsigned> entityIds)
: JointInteractionEvent(userId, entityIds, false)
{

} // EndJointInteractionEvent

void JointInteractionEndEvent::encode(NetMessage* message)
{
	int i;

	message->putUInt32(jointIDs.size());
	printd(INFO, "JointInteractionEndEvent(): found %i joints!\n", jointIDs.size());
	for (i=0; i < (int)jointIDs.size(); i++)
	{
		message->putUInt32(jointIDs[i]);
		printd(INFO, "\tJointID = %i\n", (int)jointIDs[i]);
	} // for

	message->putUInt32(floatValues.size());
	printd(INFO, "JointInteractionEndEvent(): found %i floatValues!\n", floatValues.size());
	for (i=0; i < (int)floatValues.size(); i++)
	{
		message->putReal32(floatValues[i]);
		printd(INFO, "\tfloatValue = %f\n", floatValues[i]);
	} // for

	// EntityIds already in JointInteractionEvent encoded!
	message->putUInt32(entityTransformations.size());
	for (i=0; i < (int)entityTransformations.size(); i++)
	{
		addTransformationToBinaryMsg(&entityTransformations[i], message);
	} // for

} // encode

void JointInteractionEndEvent::execute()
{
	printf("JointInteractionEndEvent::execute(): BEGIN!\n");
	unsigned short type, id;
	std::vector<unsigned>::iterator it;
	std::vector<int>::iterator it_i;
	int jointID;
	Joint* joint;
	JointInteraction* jointInteraction;
	int i;
	unsigned entityId;
	Entity* entity;
	bool localPipe;
	User* user = UserDatabase::getUserById(userId);
	TransformationPipe* pipe;

	if (UserDatabase::getLocalUser()->getId() == userId)
		localPipe = true;
	else
		localPipe = false;

	for (it = entityIds.begin(); it != entityIds.end(); ++it)
	{
		split((*it), type, id);
		pipe = TransformationManager::getPipe(JOINT_INTERACTION_MODULE_ID, WORLD_DATABASE_ID, 0, 0, type, id, 0, !localPipe, user);
		TransformationManager::closePipe(pipe);
	} // for

	if (!localPipe)
	{
		jointInteraction = (JointInteraction*)SystemCore::getModuleByName("JointInteraction");
		assert(jointInteraction);

		for (it_i = jointIDs.begin(); it_i != jointIDs.end(); ++it_i)
		{
			jointID = (*it_i);
			joint = jointInteraction->getJointWithID(jointID);
			joint->setMemberValue("internalChanges", this);
		} // for

		printd(INFO, "JointInteractionEndEvent::execute(): applying TransformationData to %i Entities!\n", entityIds.size());

		for (i=0; i < (int)entityIds.size(); i++)
		{
			entityId = entityIds[i];
			entity = WorldDatabase::getEntityWithTypeInstanceId(entityId);
			assert(entity);
			entity->setEnvironmentTransformation(entityTransformations[i]);
// 			entity->update();
		} // for

	} // if

	printf("JointInteractionEndEvent::execute(): END!\n");
} // execute

void JointInteractionEndEvent::addJointID(int jointID)
{
	jointIDs.push_back(jointID);
} // addJointID

void JointInteractionEndEvent::addFloat(float value)
{
	floatValues.push_back(value);
}

float JointInteractionEndEvent::getFloat()
{
	assert(floatIdx < (int)floatValues.size());
	return floatValues[floatIdx++];
}

void JointInteractionEndEvent::addEntityId(unsigned entityId)
{
	printd(INFO, "JointInteractionEndEvent::addEntityId(): adding Entity with ID %u\n", entityId);
	entityIds.push_back(entityId);
} // addEntityId

void JointInteractionEndEvent::addTransformationData(TransformationData trans)
{
	entityTransformations.push_back(trans);
} // addTransformationData

std::string JointInteractionEndEvent::toString()
{
	std::stringstream ss (Event::toString());
	std::vector<unsigned>::iterator it1;
	std::vector<TransformationData>::iterator it2;
	std::vector<int>::iterator it3;;
	std::vector<float>::iterator it4;

	ss << "\tuserId = " <<  userId << "\n\tentityIds = ";
	for (it1 = entityIds.begin(); it1 != entityIds.end(); ++it1)
		ss << (*it1) << " ";
	ss << "\n\tentityTransformations = \n";
	for (it2 = entityTransformations.begin(); it2 != entityTransformations.end(); ++it2)
		ss << dumpTransformationToString((*it2), "\t\t") << std::endl;
	ss << "\tjointIDs = ";
	for (it3 = jointIDs.begin(); it3 != jointIDs.end(); ++it3)
		ss << (*it3) << " ";
	ss << "\n\tfloatValues = ";
	for (it4 = floatValues.begin(); it4 != floatValues.end(); ++it4)
		ss << (*it4) << " ";
	ss << std::endl;
	return ss.str();
} // toString
