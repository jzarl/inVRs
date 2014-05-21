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

#include <assert.h>

#include "RequestListener.h"
#include "DebugOutput.h"
#include "SystemCore.h"
#include "UserDatabase/UserDatabase.h"
#include "EventManager/EventManager.h"
#include "Timer.h"

unsigned RequestListener::nextRequestId = 0;

RequestEvent::RequestEvent(unsigned srcModuleId, unsigned dstModuleId, std::string eventName) :
	Event(srcModuleId, dstModuleId, eventName) {
	requestId = 0;
	userIdRequest = 0;
}

RequestEvent::RequestEvent() :
	Event() {

}

RequestEvent::~RequestEvent() {

}

void RequestEvent::encode(NetMessage* message) {
	message->putUInt32(requestId);
	message->putUInt32(userIdRequest);
}

void RequestEvent::decode(NetMessage* message) {
	message->getUInt32(requestId);
	message->getUInt32(userIdRequest);
}

void RequestEvent::execute() {
	Event* responseEvent;
	unsigned response;

	response = this->generateResponse();
	responseEvent = this->createResponseEvent(response);
	if (!responseEvent) {
		printd(
				ERROR,
				"RequestEvent::execute(): failed to create event for responding, my event id is %u\n",
				evt_eventId);
		return;
	}

	EventManager::sendEvent(responseEvent, EventManager::EXECUTE_REMOTE);
} // execute

void RequestEvent::setCompleteRequestId(unsigned requestId, unsigned userId) {
	this->requestId = requestId;
	this->userIdRequest = userId;
}

Event* RequestEvent::createResponseEvent(unsigned response) {
	return new RequestResponseEvent(evt_dstModuleId, response, requestId, userIdRequest, evt_srcModuleId);
}


RequestResponseEvent::RequestResponseEvent(unsigned srcModuleId, unsigned response,
		unsigned requestId, unsigned userIdRequest, unsigned srcReqModuleId) :
	Event(srcModuleId, srcReqModuleId, "RequestResponseEvent") {
	this->response = response;
	this->requestId = requestId;
	this->userIdRequest = userIdRequest;
	userIdResponder = UserDatabase::getLocalUserId();
}

RequestResponseEvent::RequestResponseEvent() :
	Event() {

}

RequestResponseEvent::~RequestResponseEvent() {

}

void RequestResponseEvent::encode(NetMessage* message) {
	message->putUInt32(response);
	message->putUInt32(requestId);
	message->putUInt32(userIdRequest);
	message->putUInt32(userIdResponder);
}

void RequestResponseEvent::decode(NetMessage* message) {
	message->getUInt32(response);
	message->getUInt32(requestId);
	message->getUInt32(userIdRequest);
	message->getUInt32(userIdResponder);
}

void RequestResponseEvent::execute() {
	// only interesting for derived classes
}

unsigned RequestResponseEvent::getResponse() {
	return response;
}

unsigned RequestResponseEvent::getRequestId() {
	return requestId;
}

unsigned RequestResponseEvent::getUserIdResponder() {
	return userIdResponder;
}

unsigned RequestResponseEvent::getUserIdRequest() {
	return userIdRequest;
}

/**
 * Request Listener impl
 */
RequestListener::RequestListener() {
	state = REQUESTSTATE_NOREQUEST;
	// 	localUser = NULL; // defer initialization until the first request is issued
	network = NULL;
} // RequestListener

void RequestListener::init() {
	EventManager::registerEventFactory("RequestResponseEvent", new RequestResponseEvent::Factory());
} // init

void RequestListener::cleanup() {
}

void RequestListener::handleIncomingEvent(Event* incomingEvent) {
	RequestResponseEvent* respEvent;
	RESPONSE resp;
	int i;

	if (checkTimeout())
		return;

	if ((incomingEvent->getEventName() != "RequestEvent") && (incomingEvent->getEventName()
			!= "RequestResponseEvent")) {
		// not from our type
		printd(INFO, "RequestListener::handleIncomingEvent(): found unknown event %s! IGNORING\n",
				incomingEvent->getEventName().c_str());
		return;
	}

	respEvent = (RequestResponseEvent*)incomingEvent;

	if (respEvent->getUserIdRequest() != UserDatabase::getLocalUserId()) {
		// response event for a RequestListener residing on another machine
		// 		printd("RequestResponseEvent::handleIncomingEvent(): this response is for another machine\n");
		return;
	}

	if (respEvent->getRequestId() != requestId) {
		// response for another RequestListener on local machine
		// 		printd("RequestResponseEvent::handleIncomingEvent(): this response is not for me\n");
		return;
	}

	assert(state != REQUESTSTATE_NOREQUEST);
	if (state == REQUESTSTATE_FINISHED) {
		printd(
				"RequestResponseEvent::handleIncomingEvent(): got an event from a previous request!\n");
		return;
	}

	resp.responseVal = respEvent->getResponse();
	resp.responseUserId = respEvent->getUserIdResponder();
	for (i = 0; i < (int)recvdResponsesList.size(); i++) {
		if (recvdResponsesList[i].responseUserId == resp.responseUserId) {
			printd(
					WARNING,
					"RequestResponseEvent::handleIncomingEvent(): got a second response from user %d\n",
					resp.responseUserId);
			return;
		}
	}

	recvdResponsesList.push_back(resp);

	// 	printd("RequestResponseEvent::handleIncomingEvent(): got response %u from %u\n", resp.responseVal, resp.responseUserId);

	if (recvdResponsesList.size() == waitingFor) {
		// all responses arrived
		// go into REQUESTSTATE_FINISHED state
		state = REQUESTSTATE_FINISHED;
		// 		printd("RequestResponseEvent::handleIncomingEvent(): completed request at %d\n", timer.getTimeInMilliseconds());
	}
}

void RequestListener::startRequest(RequestEvent* evt) {
	if ((state != REQUESTSTATE_NOREQUEST) && (state != REQUESTSTATE_FINISHED)) {
		printd(WARNING,
				"RequestListener::startRequest(): cannot process more than on request simultanousely\n");
		return;
	}

	// 	if(!localUser)
	// 	{
	// 		localUser = UserDatabase::getLocalUser();
	// 		assert(localUser);
	// 		network = (NetworkInterface*)SystemCore::getModuleByName("Network");
	// 	}

	if (!network)
		network = (NetworkInterface*)SystemCore::getModuleByName("Network");

	waitingFor = 0;
	if (network)
		waitingFor = network->getNumberOfParticipants();
	if (waitingFor == 0) {
		printd(
				INFO,
				"RequestListener::startRequest(): no other users are connected, entering FINISHED state immediately\n");
		state = REQUESTSTATE_FINISHED;
		return;
	}
	recvdResponsesList.clear();
	startTimeStamp = timer.getTimeInMilliseconds();
	// 	printd("RequestListener::startRequest(): starting request at %d, waiting for %u responses\n", startTimeStamp, waitingFor);

	evt->setCompleteRequestId(nextRequestId, UserDatabase::getLocalUserId());
	requestId = nextRequestId;
	nextRequestId++;

	EventManager::sendEvent(evt, EventManager::EXECUTE_REMOTE);

	state = REQUESTSTATE_ISSUED;
}


bool RequestListener::checkTimeout() {
	double now;

	if (state == REQUESTSTATE_ISSUED) {
		now = timer.getTimeInMilliseconds();
		if (now - startTimeStamp > 50000) {
			printd("RequestResponseEvent::checkTimeout(): timeout, got %u replies already\n",
					recvdResponsesList.size());
			state = REQUESTSTATE_FINISHED;
			return true;
		}
	}
	return false;
}

REQUESTSTATE RequestListener::getCurrentRequestState() {
	return state;
}

std::vector<RequestListener::RESPONSE>* RequestListener::getResponseVector() {
	return &recvdResponsesList;
}

