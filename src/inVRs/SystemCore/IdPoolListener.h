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

#ifndef _IDPOOLLISTENER_H
#define _IDPOOLLISTENER_H

#include "RequestListener.h"
#include "DataTypes.h"
#include "EventManager/Event.h"
#include "EventManager/EventFactory.h"
#include "IdPool.h"
#include "IdPoolManager.h"
#include "ModuleIds.h"

class INVRS_SYSTEMCORE_API IdPoolListenerFreeIndexRequestEvent : public RequestEvent {
public:
	IdPoolListenerFreeIndexRequestEvent(unsigned srcModuleId, std::string poolName);
	IdPoolListenerFreeIndexRequestEvent();

	typedef EventFactory<IdPoolListenerFreeIndexRequestEvent> Factory;

	virtual void encode(NetMessage* message);
	virtual void decode(NetMessage* message);
	virtual unsigned generateResponse();

protected:

	std::string poolName;
};

class INVRS_SYSTEMCORE_API IdPoolListenerVetoRequestEvent : public RequestEvent {
public:

	IdPoolListenerVetoRequestEvent(unsigned srcModuleId, std::string poolName,
			std::string subPoolName, unsigned startIdx, unsigned size);
	IdPoolListenerVetoRequestEvent();

	typedef EventFactory<IdPoolListenerVetoRequestEvent> Factory;

	virtual void encode(NetMessage* message);
	virtual void decode(NetMessage* message);
	virtual unsigned generateResponse();

protected:

	unsigned startIdx;
	unsigned size;
	std::string poolName;
	std::string subPoolName;

};

class INVRS_SYSTEMCORE_API IdPoolListenerFinalizeAllocationEvent : public Event {
public:
	IdPoolListenerFinalizeAllocationEvent(unsigned srcModuleId, std::string poolName,
			std::string subPoolName, unsigned bKeep);
	IdPoolListenerFinalizeAllocationEvent();

	typedef EventFactory<IdPoolListenerFinalizeAllocationEvent> Factory;

	virtual void encode(NetMessage* message);
	virtual void decode(NetMessage* message);
	virtual void execute();

protected:

	std::string poolName;
	std::string subPoolName;
	unsigned bKeep;
};

class INVRS_SYSTEMCORE_API IdPoolListener {
public:

	enum STATE {
		STATE_IDLE, STATE_ISSUED, STATE_FINISHED
	};

	IdPoolListener();
	~IdPoolListener();

	void requestAllocation(std::string name, unsigned size, unsigned eventPipeModuleId);
	void handleIncomingEvent(Event* incomingEvent);
	STATE getState();
	IdPool* getResult();

protected:

	enum INTERNALSTATE {
		INTERNALSTATE_IDLE,
		INTERNALSTATE_ISSUED_IDXREQ,
		INTERNALSTATE_ISSUED_VETOREQ,
		INTERNALSTATE_FINISHED
	};

	RequestListener requestIdxListener;
	RequestListener vetoListener;
	INTERNALSTATE state;
	std::string poolName;
	std::string subPoolName;
	unsigned size;
	unsigned eventPipeModuleId;
	IdPool* localPool;
	IdPool* localSubPool;

};

#endif
