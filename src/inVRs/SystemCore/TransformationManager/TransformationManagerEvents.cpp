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

#include "TransformationManagerEvents.h"

#include <string.h>
#include <sstream>
#include <assert.h>

#include "TransformationManager.h"
#include "../UserDatabase/User.h"
#include "../UserDatabase/UserDatabase.h"
#include "../DebugOutput.h"
#include "../SystemCore.h"
#include "../ComponentInterfaces/NetworkInterface.h"
#include "../WorldDatabase/WorldDatabaseEvents.h"
#include "../MessageFunctions.h"

TransformationManagerOpenPipeEvent::TransformationManagerOpenPipeEvent() :
	Event(TRANSFORMATION_MANAGER_ID, TRANSFORMATION_MANAGER_ID,
			"TransformationManagerOpenPipeEvent") {
}

TransformationManagerOpenPipeEvent::TransformationManagerOpenPipeEvent(unsigned srcId,
		unsigned dstId, unsigned pipeType, unsigned objectClass, unsigned objectType,
		unsigned objectId, unsigned priority, unsigned userId, bool isMTPipe) :
	Event(TRANSFORMATION_MANAGER_ID, TRANSFORMATION_MANAGER_ID,
			"TransformationManagerOpenPipeEvent") {
	addPipe(srcId, dstId, pipeType, objectClass, objectType, objectId, priority, userId, isMTPipe);
} // TransformationManagerOpenPipeEvent

TransformationManagerOpenPipeEvent::~TransformationManagerOpenPipeEvent() {
	int i;
	for (i = 0; i < (int)pipeList.size(); i++)
		delete pipeList[i];
	pipeList.clear();
} // TransformationManagerOpenPipeEvent

void TransformationManagerOpenPipeEvent::addPipe(unsigned srcId, unsigned dstId, unsigned pipeType,
		unsigned objectClass, unsigned objectType, unsigned objectId, unsigned priority,
		unsigned userId, bool isMTPipe) {
	TransformationPipeIdData* data = new TransformationPipeIdData;
	data->srcId = srcId;
	data->dstId = dstId;
	data->pipeType = pipeType;
	data->objectClass = objectClass;
	data->objectType = objectType;
	data->objectId = objectId;
	data->priority = priority;
	data->userId = userId;
	data->isMTPipe = isMTPipe;
	pipeList.push_back(data);
} // addPipe

void TransformationManagerOpenPipeEvent::addPipe(TransformationPipe* pipe, unsigned priority) {
	bool fromNetwork;
	TransformationPipeIdData* data = new TransformationPipeIdData;

	data->userId = pipe->getOwner()->getId();
	TransformationManager::unpackPipeId(pipe->getPipeId(), &(data->srcId), &(data->dstId),
			&(data->pipeType), &(data->objectClass), &(data->objectType), &(data->objectId),
			&fromNetwork);
	data->priority = priority;
	pipeList.push_back(data);
} // addPipe

void TransformationManagerOpenPipeEvent::encode(NetMessage* message) {
	int i;
	TransformationPipeIdData* data;

	message->putUInt32(pipeList.size());
	for (i = 0; i < (int)pipeList.size(); i++) {
		data = pipeList[i];
		message->putUInt32(data->srcId);
		message->putUInt32(data->dstId);
		message->putUInt32(data->pipeType);
		message->putUInt32(data->objectClass);
		message->putUInt32(data->objectType);
		message->putUInt32(data->objectId);
		message->putUInt32(data->priority);
		message->putUInt32(data->userId);
		msgFunctions::encode(data->isMTPipe, message);
	} // for
}

void TransformationManagerOpenPipeEvent::decode(NetMessage* message) {
	int i;
	unsigned numberOfPipes;
	TransformationPipeIdData* data;
	message->getUInt32(numberOfPipes);

	for (i = 0; i < (int)numberOfPipes; i++) {
		data = new TransformationPipeIdData;
		message->getUInt32(data->srcId);
		message->getUInt32(data->dstId);
		message->getUInt32(data->pipeType);
		message->getUInt32(data->objectClass);
		message->getUInt32(data->objectType);
		message->getUInt32(data->objectId);
		message->getUInt32(data->priority);
		message->getUInt32(data->userId);
		msgFunctions::decode(data->isMTPipe, message);
		pipeList.push_back(data);
	}
}

void TransformationManagerOpenPipeEvent::execute() {
	int i;
	User* user;
	bool fromNetwork;
	TransformationPipeIdData* data;
	unsigned localUserId = UserDatabase::getLocalUserId();

	for (i = 0; i < (int)pipeList.size(); i++) {
		data = pipeList[i];
		user = UserDatabase::getUserById(data->userId);

		// TODO: following was commented out! Check if there exist pipes where the owner is localUser
		//       but the pipe is a remote Pipe!!!
		if (data->userId == localUserId)
			fromNetwork = false;
		else
			fromNetwork = true;
		//		fromNetwork = true;

		if (data->isMTPipe)
			TransformationManager::openMTPipe(data->srcId, data->dstId, data->pipeType,
					data->objectClass, data->objectType, data->objectId, data->priority,
					fromNetwork, user);
		else
			TransformationManager::openPipe(data->srcId, data->dstId, data->pipeType,
					data->objectClass, data->objectType, data->objectId, data->priority,
					fromNetwork, user);
	} // for
} // execute

std::string TransformationManagerOpenPipeEvent::toString() {
	int i;
	TransformationPipeIdData* data;
	std::stringstream ss;

	// TODO: check if every data is initialized!!!

	ss << "EVENT: TransformationManagerOpenPipeEvent\n";
	ss << "timestamp = " << evt_timestamp << std::endl;
	for (i = 0; i < (int)pipeList.size(); i++) {
		data = pipeList[i];
		ss << "\tuserId = " << data->userId << "\n\tsrcId = " << data->srcId << "\n\tdstId = "
				<< data->dstId << std::endl;
		ss << "\tpipeType = " << data->pipeType << "\n\tobjectClass = " << data->objectClass
				<< "\n\tobjectType = " << data->objectType << std::endl;
		ss << "\tobjectId = " << data->objectId << "\n\tpriority = " << data->priority << std::endl;
	} // for

	return ss.str();
} // toString

TransformationManagerClosePipeEvent::TransformationManagerClosePipeEvent() :
	Event(TRANSFORMATION_MANAGER_ID, TRANSFORMATION_MANAGER_ID,
			"TransformationManagerClosePipeEvent") {
} // TransformationManagerClosePipeEvent

TransformationManagerClosePipeEvent::TransformationManagerClosePipeEvent(TransformationPipe* pipe) :
	Event(TRANSFORMATION_MANAGER_ID, TRANSFORMATION_MANAGER_ID,
			"TransformationManagerClosePipeEvent") {
	addPipe(pipe);
} // TransformationManagerClosePipeEvent

TransformationManagerClosePipeEvent::~TransformationManagerClosePipeEvent() {
	int i;
	for (i = 0; i < (int)pipeList.size(); i++)
		delete pipeList[i];
	pipeList.clear();
} // TransformationManagerClosePipeEvent

void TransformationManagerClosePipeEvent::addPipe(unsigned srcId, unsigned dstId,
		unsigned pipeType, unsigned objectClass, unsigned objectType, unsigned objectId,
		unsigned userId) {
	TransformationPipeIdData* data = new TransformationPipeIdData;
	data->srcId = srcId;
	data->dstId = dstId;
	data->pipeType = pipeType;
	data->objectClass = objectClass;
	data->objectType = objectType;
	data->objectId = objectId;
	data->userId = userId;
	pipeList.push_back(data);
} // addPipe

void TransformationManagerClosePipeEvent::addPipe(TransformationPipe* pipe) {
	bool fromNetwork;
	TransformationPipeIdData* data = new TransformationPipeIdData;

	data->userId = pipe->getOwner()->getId();
	TransformationManager::unpackPipeId(pipe->getPipeId(), &(data->srcId), &(data->dstId),
			&(data->pipeType), &(data->objectClass), &(data->objectType), &(data->objectId),
			&fromNetwork);
	pipeList.push_back(data);
} // addPipe

void TransformationManagerClosePipeEvent::encode(NetMessage* message) {
	int i;
	TransformationPipeIdData* data;

	printd(INFO, "TransformationManagerClosePipeEvent::encode(): BEGIN!\n");

	message->putUInt32(pipeList.size());
	for (i = 0; i < (int)pipeList.size(); i++) {
		data = pipeList[i];
		message->putUInt32(data->srcId);
		message->putUInt32(data->dstId);
		message->putUInt32(data->pipeType);
		message->putUInt32(data->objectClass);
		message->putUInt32(data->objectType);
		message->putUInt32(data->objectId);
		message->putUInt32(data->userId);
	} // for
}

void TransformationManagerClosePipeEvent::decode(NetMessage* message) {
	int i;
	unsigned numberOfPipes;
	TransformationPipeIdData* data;
	message->getUInt32(numberOfPipes);

	for (i = 0; i < (int)numberOfPipes; i++) {
		data = new TransformationPipeIdData;
		message->getUInt32(data->srcId);
		message->getUInt32(data->dstId);
		message->getUInt32(data->pipeType);
		message->getUInt32(data->objectClass);
		message->getUInt32(data->objectType);
		message->getUInt32(data->objectId);
		message->getUInt32(data->userId);
		pipeList.push_back(data);
	}
}

void TransformationManagerClosePipeEvent::execute() {
	int i;
	User* user;
	unsigned priority = 0;
	bool fromNetwork = true;
	TransformationPipeIdData* data;
	unsigned localUserId = UserDatabase::getLocalUserId();

	printd(INFO, "TransformationManagerClosePipeEvent::execute(): BEGIN!\n");

	for (i = 0; i < (int)pipeList.size(); i++) {
		data = pipeList[i];
		user = UserDatabase::getUserById(data->userId);

		// TODO: following was commented out! Check if there exist pipes where the owner is localUser
		//       but the pipe is a remote Pipe!!!
		if (data->userId == localUserId)
			fromNetwork = false;
		else
			fromNetwork = true;
		//		fromNetwork = true;

		TransformationPipe* pipe = TransformationManager::getPipe(data->srcId, data->dstId,
				data->pipeType, data->objectClass, data->objectType, data->objectId, priority,
				fromNetwork, user);

		if (pipe)
			TransformationManager::closePipe(pipe);
		else {
			uint64_t pipeId = TransformationManager::packPipeId(data->srcId, data->dstId,
					data->pipeType, data->objectClass, data->objectType, data->objectId,
					fromNetwork);
			printd(
					ERROR,
					"TransformationManagerClosePipeEvent::execute(): ERROR: could not close pipe with ID %s from user with ID %u\n",
					getUInt64AsString(pipeId).c_str(), data->userId);
		} // else
	} // for

	printd(INFO, "TransformationManagerClosePipeEvent::execute(): DONE!\n");

} // execute

std::string TransformationManagerClosePipeEvent::toString() {
	int i;
	TransformationPipeIdData* data;
	std::stringstream ss(std::stringstream::in | std::stringstream::out);

	ss << "EVENT: TransformationManagerClosePipeEvent\n";
	ss << "timestamp = " << evt_timestamp << std::endl;
	for (i = 0; i < (int)pipeList.size(); i++) {
		data = pipeList[i];
		ss << "\tuserId = " << data->userId << "\n\tsrcId = " << data->srcId << "\n\tdstId = "
				<< data->dstId << std::endl;
		ss << "\tpipeType = " << data->pipeType << "\n\tobjectClass = " << data->objectClass
				<< "\n\tobjectType = " << data->objectType << std::endl;
		ss << "\tobjectId = " << data->objectId << std::endl;
	} // for

	return ss.str();
} // toString
