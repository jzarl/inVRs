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

/*---------------------------------------------------------------------------*\
 *                                Changes                                    *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
\*---------------------------------------------------------------------------*/

#ifndef _EVENT_H
#define _EVENT_H

#include <map>

#include "../ModuleIds.h"
#include "../NetMessage.h"
#include "../ArgumentVector.h"


class INVRS_SYSTEMCORE_API Event {
public:
	Event(); /// empty constructor, for network deserialization

	/**
	 * Event constructor.
	 * @param srcModuleId the id (as defined in ModuleIds.h) of the Module of origin.
	 * @param dstModuleId the id (as defined in ModuleIds.h) of the recipient Module, where the Event is executed.
	 * @param eventName the (class-) name of the Event.
	 */
	Event(unsigned srcModuleId, unsigned dstModuleId, std::string eventName);

	virtual ~Event();

	/**
	 * Used for visibility management.
	 * Visibilty management heavily relies on a specific network module.
	 * The content of the visibilityLevel member will not be encoded!
	 * Arguments can be set at any time before the event has been transmitted (by the Event itself, the EventManager ...)
	 * @see Network::sendEvent()
	 */
	ArgumentVector visibilityLevel;

	/**
	 * Encodes the Event into a NetMessage.
	 * This (and only this) method is needed by the Network module to encode the Event.
	 */
	NetMessage* completeEncode();

	/**
	 * Overwrite the attributes of this Event using the values contained in a NetMessage.
	 * @param message the NetMessage used as a prototype for the Event.
	 * @param eventId the
	 */
	void completeDecode(NetMessage* message, std::string eventName);

	// overwrite these two methods:
	/// \todo should Event::decode() and Event::encode() be protected?
	/// \todo describe Event::decode() and Event::encode()
	virtual void encode(NetMessage* message); // append to message
	virtual void decode(NetMessage* message);

	/**
	 * Process the Event.
	 * \todo write more precise description
	 */
	virtual void execute() = 0;

	/**
	 * Return a string representation of the Event, containing all attributes.
	 */
	virtual std::string toString();

	/**
	 * Force a specific sequence number for the Event.
	 * Normally, each Event automatically gets a sequence number on creation.
	 * You can override that sequence number with this method.
	 * Note: The sequence number is not neccessarily unique.
	 */
	void setSequenceNumber(unsigned sequenceNumber);

	std::string getEventName();
	unsigned getUserId();
	unsigned getSrcModuleId();
	unsigned getDstModuleId();

protected:

	friend class EventManager;

	std::string evt_eventName; /// the type of the Event (= className)
	unsigned evt_eventId;
	unsigned evt_srcModuleId; /// id of module which puts event into event pipe (as defined in ModuleIds.h)
	unsigned evt_dstModuleId; /// id of module which runs execute() (as defined in ModuleIds.h)
	unsigned evt_sequenceNumber; /// only for booking
	unsigned evt_timestamp; /// when event has been created
	unsigned evt_userId; /// id of user who triggered the event

	/**
	 * Return a sequence number for an Event.
	 * Not neccessarily unique.
	 * Note: current implementation just returns 0.
	 */
	static unsigned generateSequenceNumber();

	/**
	 * Method for the EventManager to set the ID of the event for network transfer.
	 * The Eventmanager calls this method when the sendEvent or sendEventTo methods
	 * are called. It passes the calculated hash code for the event's name. This code
	 * is then encoded in the completeEncode method and used for identification of
	 * the event at decode time.
	 * @param id hash code of the Event's name
	 */
	void setEventId(unsigned id);

	void setEventName(std::string newName); // in order to overwrite the id set by previousely called constructor
};

#endif
