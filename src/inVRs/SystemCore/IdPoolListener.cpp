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

#include "IdPoolListener.h"

#include <assert.h>

#include "DebugOutput.h"
#include "UserDatabase/UserDatabase.h"
#include "EventManager/EventManager.h"

//Event* IdPoolListenerEventFactory::create(unsigned eventId)
//{
//// 	printd("IdPoolListenerEventFactory::decode(): decoding event of type %u for %u\n", id, type);
//	switch(eventId)
//	{
//		case IdPoolListenerFreeIdxRequestType:
//			return new IdPoolListenerFreeIndexRequestEvent();
//
//		case IdPoolListenerVetoRequestType:
//			return new IdPoolListenerVetoRequestEvent();
//
//		case IdPoolListenerFinalizeAllocationType:
//			return new IdPoolListenerFinalizeAllocationEvent();
//	}
//
//	return NULL;
//}

IdPoolListenerFreeIndexRequestEvent::IdPoolListenerFreeIndexRequestEvent(unsigned srcModuleId, std::string poolName) :
	RequestEvent(srcModuleId, SYSTEM_CORE_ID, "IdPoolListenerFreeIndexRequestEvent") {
	this->poolName = poolName;
}

IdPoolListenerFreeIndexRequestEvent::IdPoolListenerFreeIndexRequestEvent() :
	RequestEvent() {

}

void IdPoolListenerFreeIndexRequestEvent::encode(NetMessage* message) {
	RequestEvent::encode(message);
	message->putString(poolName);
}


void IdPoolListenerFreeIndexRequestEvent::decode(NetMessage* message) {
	RequestEvent::decode(message);
	message->getString(poolName);
}

unsigned IdPoolListenerFreeIndexRequestEvent::generateResponse() {
	IdPool* pool;
	unsigned result;

	pool = localIdPoolManager.getPoolByName(poolName);
	if (!pool) {
		printd(ERROR, "IdPoolListenerFreeIndexRequestEvent::generateResponse(): couldn't find any pool with name '%s'\n", poolName.c_str());
		return 0; // report a free idx of 0
	}

	result = pool->getUnallocatedSubPoolIdx();
	printd(INFO, "IdPoolListenerFreeIndexRequestEvent::generateResponse(): responding with %u\n", result);

	return result;
}

/**
 *
 */
IdPoolListenerVetoRequestEvent::IdPoolListenerVetoRequestEvent(unsigned srcModuleId, std::string poolName, std::string subPoolName, unsigned startIdx, unsigned size) :
	RequestEvent(srcModuleId, SYSTEM_CORE_ID, "IdPoolListenerVetoRequestEvent") {
	this->poolName = poolName;
	this->subPoolName = subPoolName;
	this->startIdx = startIdx;
	this->size = size;
}

IdPoolListenerVetoRequestEvent::IdPoolListenerVetoRequestEvent() :
	RequestEvent() {

}


void IdPoolListenerVetoRequestEvent::encode(NetMessage* message) {
	RequestEvent::encode(message);
	message->putUInt32(startIdx);
	message->putUInt32(size);
	message->putString(poolName);
	message->putString(subPoolName);
}

void IdPoolListenerVetoRequestEvent::decode(NetMessage* message) {
	RequestEvent::decode(message);
	message->getUInt32(startIdx);
	message->getUInt32(size);
	message->getString(poolName);
	message->getString(subPoolName);
}


unsigned IdPoolListenerVetoRequestEvent::generateResponse() {
	IdPool *pool, *subPool;

	pool = localIdPoolManager.getPoolByName(poolName);
	if (!pool) {
		printd(ERROR, "IdPoolListenerVetoRequestEvent::generateResponse(): couldn't find any pool with name '%s'\n", poolName.c_str());
		return RESPONSE_DENIED;
	}

	subPool = pool->allocSubPool(startIdx, size);
	if (!subPool) {
		printd(WARNING, "IdPoolListenerVetoRequestEvent::generateResponse(): allocation of subPool at %u with size %u failed\n", startIdx, size);
		return RESPONSE_DENIED;
	}

	printd(INFO, "IdPoolListenerVetoRequestEvent::generateResponse(): allocation starting at %u with size %u is ok for me\n", startIdx, size);

	localIdPoolManager.registerIdPool(subPoolName, subPool);
	return RESPONSE_OK;
}


/**
 *
 */
IdPoolListenerFinalizeAllocationEvent::IdPoolListenerFinalizeAllocationEvent(unsigned srcModuleId, std::string poolName, std::string subPoolName, unsigned bKeep) :
	Event(srcModuleId, SYSTEM_CORE_ID, "IdPoolListenerFinalizeAllocationEvent") {
	this->poolName = poolName;
	this->subPoolName = subPoolName;
	this->bKeep = bKeep;
}

IdPoolListenerFinalizeAllocationEvent::IdPoolListenerFinalizeAllocationEvent() :
	Event() {

}

void IdPoolListenerFinalizeAllocationEvent::encode(NetMessage* message) {
	message->putUInt32(bKeep);
	message->putString(poolName);
	message->putString(subPoolName);
}

void IdPoolListenerFinalizeAllocationEvent::decode(NetMessage* message) {
	message->getUInt32(bKeep);
	message->getString(poolName);
	message->getString(subPoolName);
}

void IdPoolListenerFinalizeAllocationEvent::execute() {
	IdPool* subPool;
	IdPool* pool;

	pool = localIdPoolManager.getPoolByName(poolName);
	assert(pool);
	subPool = localIdPoolManager.getPoolByName(subPoolName);

	printd("IdPoolListenerFinalizeAllocationEvent::execute(): got %u for pool %s\n", bKeep, subPoolName.c_str());

	if (bKeep) {
		if (!subPool) {
			printd(ERROR, "IdPoolListenerFinalizeAllocationEvent::execute(): subPool %s not found on this machine!!\n", subPoolName.c_str());
		}
	} else {
		if (!pool->freeSubPool(subPool)) {
			printd(ERROR, "IdPoolListenerFinalizeAllocationEvent::execute(): failed to deallocate subPool with name %s\n", subPoolName.c_str());
		}
	}
}

/**
 *
 */
IdPoolListener::IdPoolListener() {
	state = INTERNALSTATE_IDLE;
	localPool = NULL;
	localSubPool = NULL;
	size = 0;
}

IdPoolListener::~IdPoolListener() {

}

IdPoolListener::STATE IdPoolListener::getState() {
	switch (state) {
	case INTERNALSTATE_IDLE:
		return STATE_IDLE;

	case INTERNALSTATE_ISSUED_IDXREQ:
	case INTERNALSTATE_ISSUED_VETOREQ:
		return STATE_ISSUED;

	case INTERNALSTATE_FINISHED:
		return STATE_FINISHED;

	default:
		assert(false);
	}

	return STATE_FINISHED; // should never be reached
}

IdPool* IdPoolListener::getResult() {
	if (state != INTERNALSTATE_FINISHED) {
		printd(WARNING, "IdPoolListener::getResult(): requested result for an unfinished request!\n");
		return NULL; // localPool might have a value other than NULL during an request
	}
	return localSubPool;
}

void IdPoolListener::requestAllocation(std::string name, unsigned size, unsigned eventPipeModuleId) {
	RequestEvent* evt;

	if ((state != INTERNALSTATE_IDLE) && (state != INTERNALSTATE_FINISHED)) {
		printd(ERROR, "IdPoolListener::requestAllocation(): a request is currently processed\n");
		return;
	}

	localPool = localIdPoolManager.getPoolByName(name);
	if (!localPool) {
		printd(WARNING, "IdPoolListener::requestAllocation(): no pool with name '%s' found!\n", name.c_str());
		state = INTERNALSTATE_FINISHED;
		localSubPool = NULL;
		return;
	}

	poolName = name;
	localSubPool = NULL;
	evt = new IdPoolListenerFreeIndexRequestEvent(eventPipeModuleId, name);
	printd("IdPoolListener::requestAllocation(): starting a request for %s\n", name.c_str());
	requestIdxListener.startRequest(evt);
	state = INTERNALSTATE_ISSUED_IDXREQ;
	this->size = size;
	this->eventPipeModuleId = eventPipeModuleId;
	// 	delete evt;
}

void IdPoolListener::handleIncomingEvent(Event* incomingEvent) {
	char buffer[0x10000];
	unsigned maxfreeidx;
	RequestEvent* evt;
	Event* allocFinalizeEvt;
	std::vector<RequestListener::RESPONSE>* responses;
	int i;

	switch (state) {
	case INTERNALSTATE_ISSUED_IDXREQ:
		requestIdxListener.handleIncomingEvent(incomingEvent);
		if (requestIdxListener.getCurrentRequestState() == REQUESTSTATE_FINISHED) {
			// 				printd("IdPoolListener::handleIncomingEvent(): changing into INTERNALSTATE_ISSUED_VETOREQ\n");

			// check responses and write to maxfreeidx
			maxfreeidx = localPool->getUnallocatedSubPoolIdx();
			responses = requestIdxListener.getResponseVector();
			for (i = 0; i < (int)responses->size(); i++) {
				if ((*responses)[i].responseVal > maxfreeidx)
					maxfreeidx = (*responses)[i].responseVal;
			}

			// generate subPoolName if there is enough space:
			sprintf(buffer, "%s_sub_%u_%p_%u_%u", poolName.c_str(), UserDatabase::getLocalUserId(), this, maxfreeidx, size);
			subPoolName = std::string(buffer);

			localSubPool = localPool->allocSubPool(maxfreeidx, size);
			if (!localSubPool) {
				printd(WARNING, "IdPoolListener::handleIncomingEvent(): failed to allocate subpool on local machine: startIdx=%u, size=%u\n", maxfreeidx, size);

				// tell other about local failure
				allocFinalizeEvt = new IdPoolListenerFinalizeAllocationEvent(eventPipeModuleId, poolName, subPoolName, 0);
				EventManager::sendEvent(allocFinalizeEvt, EventManager::EXECUTE_REMOTE);
				// 					delete allocFinalizeEvt;

				// go into finished state
				state = INTERNALSTATE_FINISHED;
				break;
			} else {
				// state transition
				evt = new IdPoolListenerVetoRequestEvent(eventPipeModuleId, poolName, subPoolName, maxfreeidx, size);
				state = INTERNALSTATE_ISSUED_VETOREQ;
				vetoListener.startRequest(evt);
				// 					delete evt;
			}
		}
		break;

	case INTERNALSTATE_ISSUED_VETOREQ:
		vetoListener.handleIncomingEvent(incomingEvent);
		if (vetoListener.getCurrentRequestState() == REQUESTSTATE_FINISHED) {
			// 				printd("IdPoolListener::handleIncomingEvent(): changing into INTERNALSTATE_FINISHED\n");

			// check responses:
			responses = requestIdxListener.getResponseVector();
			for (i = 0; i < (int)responses->size(); i++) {
				if ((*responses)[i].responseVal == RESPONSE_DENIED)
					break;
			}
			if (i < (int)responses->size()) {
				// denied
				allocFinalizeEvt = new IdPoolListenerFinalizeAllocationEvent(eventPipeModuleId, poolName, subPoolName, 0);

				// immediately remove pool from localPool
				if (!localPool->freeSubPool(localSubPool)) {
					printd(ERROR, "IdPoolListener::handleIncomingEvent(): internal error, failed to deallocate my own pool :-(\n");
					return;
				}
			} else {
				// ok
				allocFinalizeEvt = new IdPoolListenerFinalizeAllocationEvent(eventPipeModuleId, poolName, subPoolName, 1);
			}
			EventManager::sendEvent(allocFinalizeEvt, EventManager::EXECUTE_REMOTE);
			// 				delete allocFinalizeEvt;
			state = INTERNALSTATE_FINISHED;

			// send reply event wheter subpool with name subPoolName should be kept or discarded
			// hint: sent it to the transformation manager, it will allways be executed there (pool is retrieved by name, do you remember ;-)
		}
		break;

	case INTERNALSTATE_IDLE:
		printd(WARNING, "IdPoolListener::handleIncomingEvent(): you shouldn't call me because im idle!\n");
		break;

	case INTERNALSTATE_FINISHED:
		printd(WARNING, "IdPoolListener::handleIncomingEvent(): im already finished!\n");
		break;

	default:
		printd(WARNING, "IdPoolListener::handleIncomingEvent(): found unhandled INTERNALSTATE %u!\n", state);
		break;
	}
}

