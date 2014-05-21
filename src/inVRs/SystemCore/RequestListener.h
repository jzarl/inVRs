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

#ifndef _REQUESTLISTENER_H
#define _REQUESTLISTENER_H

#include "DataTypes.h"
#include "EventManager/Event.h"
#include "EventManager/EventFactory.h"
#include "IdPool.h"
#include "ModuleIds.h"
#include "ComponentInterfaces/NetworkInterface.h"
#include "UserDatabase/UserDatabase.h"

#define RESPONSE_DENIED		0
#define RESPONSE_OK		1

enum REQUESTSTATE {
	REQUESTSTATE_NOREQUEST, REQUESTSTATE_ISSUED, REQUESTSTATE_FINISHED
};

/******************************************************************************
 * Abstract base Event for all Requests sent over the network. All requests
 * which have to be sent over the network should be inherited from this class.
 */
class INVRS_SYSTEMCORE_API RequestEvent : public Event {
public:

	/**
	 * srcModuleId ... module which is raising the request
	 * dstModuleId ... module which handles the request (= calls execute on all other connected machines)
	 */
	RequestEvent(unsigned srcModuleId, unsigned dstModuleId, std::string eventName);
	RequestEvent();
	virtual ~RequestEvent();

	virtual void encode(NetMessage* message);
	virtual void decode(NetMessage* message);
	virtual void execute();
	virtual void setCompleteRequestId(unsigned requestId, unsigned userId);
	virtual unsigned generateResponse() = 0;

protected:
	virtual Event* createResponseEvent(unsigned response);

	unsigned requestId;
	unsigned userIdRequest;
};

class INVRS_SYSTEMCORE_API RequestResponseEvent : public Event {
public:

	RequestResponseEvent(unsigned srcModuleId, unsigned response, unsigned requestId,
			unsigned userIdRequest, unsigned srcReqModuleId);
	RequestResponseEvent();
	virtual ~RequestResponseEvent();

	typedef EventFactory<RequestResponseEvent> Factory;

	virtual void encode(NetMessage* message);
	virtual void decode(NetMessage* message);
	virtual void execute();

	virtual unsigned getResponse();
	virtual unsigned getRequestId();
	virtual unsigned getUserIdResponder();
	virtual unsigned getUserIdRequest();

protected:
	unsigned response;
	unsigned requestId;
	unsigned userIdRequest;
	unsigned userIdResponder;
};

class INVRS_SYSTEMCORE_API RequestListener {
public:

	RequestListener();

	/** Empty destructor.
	 */
	virtual ~RequestListener() {}

	static void init();
	static void cleanup();

	struct RESPONSE {
		unsigned responseVal;
		unsigned responseUserId;
	};

	virtual void handleIncomingEvent(Event* incomingEvent);
	virtual void startRequest(RequestEvent* evt); // calls sendEvent() but does not delete it
	virtual bool checkTimeout();
	REQUESTSTATE getCurrentRequestState();
	std::vector<RESPONSE>* getResponseVector();

protected:

	double startTimeStamp;
	std::vector<RESPONSE> recvdResponsesList;
	unsigned waitingFor;
	unsigned requestId;
	static unsigned nextRequestId;
	REQUESTSTATE state;
	NetworkInterface* network;
	// 	User* localUser;
};

#endif
