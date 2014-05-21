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

#include <iostream>
#include <sstream>

#include "Event.h"
#include "../UserDatabase/UserDatabase.h"
#include "../Timer.h"

Event::Event() {
	evt_eventId = 0;
	evt_srcModuleId = 0;
	evt_dstModuleId = 0;
	evt_eventName = "";
	evt_timestamp = 0;
	evt_sequenceNumber = 0;
}

Event::Event(unsigned srcModuleId, unsigned dstModuleId, std::string eventName) {
	this->evt_srcModuleId = srcModuleId;
	this->evt_dstModuleId = dstModuleId;
	this->evt_eventName = eventName;
	this->evt_timestamp = (unsigned)timer.getTimeInMilliseconds();
	evt_userId = UserDatabase::getLocalUserId();
	evt_sequenceNumber = generateSequenceNumber();
} // Event

Event::~Event() {

} // ~Event

NetMessage* Event::completeEncode() {
	NetMessage* ret;

	if (evt_eventName.size() == 0 || evt_eventId == 0) {
		printd(ERROR, "Event::completeEncode(): UNINITIALIZED EVENT FOUND!\n");
		return NULL;
	} // if

	ret = new NetMessage;

	ret->putUInt32(evt_eventId);
	ret->putUInt32(evt_srcModuleId);
	ret->putUInt32(evt_dstModuleId);
	ret->putUInt32(evt_sequenceNumber);
	ret->putUInt32(evt_timestamp);
	ret->putUInt32(evt_userId);

	encode(ret);

	return ret;
}

void Event::completeDecode(NetMessage* message, std::string eventName) {
	this->evt_eventName = eventName;
	message->getUInt32(evt_srcModuleId);
	message->getUInt32(evt_dstModuleId);
	message->getUInt32(evt_sequenceNumber);
	message->getUInt32(evt_timestamp);
	message->getUInt32(evt_userId);

	decode(message);
}

void Event::encode(NetMessage* message) {

} // encode

void Event::decode(NetMessage* message) {

}

std::string Event::toString() {
	std::stringstream ss(std::stringstream::in | std::stringstream::out);

	ss << "EVENT: " << evt_eventName << std::endl;
	ss << "timestamp = " << evt_timestamp << "\tsrcModuleId =  " << evt_srcModuleId
			<< "\tdstModuleId = " << evt_dstModuleId << std::endl;
	return ss.str();
}

void Event::setSequenceNumber(unsigned sequenceNumber) {
	this->evt_sequenceNumber = sequenceNumber;
}

std::string Event::getEventName() {
	return evt_eventName;
} // getEventName

unsigned Event::getUserId() {
	return evt_userId;
}

unsigned Event::getSrcModuleId() {
	return evt_srcModuleId;
} // getModuleId

unsigned Event::getDstModuleId() {
	return evt_dstModuleId;
}

unsigned Event::generateSequenceNumber() {
	return 0; // TODO implement this properly
}

void Event::setEventId(unsigned id) {
	if (id == 0)
		printd(ERROR, "Event::setEventId(): illegal ID 0 found for event %s!\n",
				evt_eventName.c_str());
	this->evt_eventId = id;
} // setEventId

void Event::setEventName(std::string newName) {
	evt_eventName = newName;
}

