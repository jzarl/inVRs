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

#ifndef _TRANSFORMATIONMANAGEREVENTS_H
#define _TRANSFORMATIONMANAGEREVENTS_H

#include <string>

#include "../EventManager/EventFactory.h"
#include "../EventManager/Event.h"
#include "../UserDatabase/UserDatabase.h"
#include "TransformationPipe.h"

struct TransformationPipeIdData {
	unsigned srcId;
	unsigned dstId;
	unsigned pipeType;
	unsigned objectClass;
	unsigned objectType;
	unsigned objectId;
	unsigned priority;
	unsigned userId;
	bool isMTPipe;
}; // TransformationPipeIdData

class INVRS_SYSTEMCORE_API TransformationManagerOpenPipeEvent : public Event {
public:
	TransformationManagerOpenPipeEvent();
	TransformationManagerOpenPipeEvent(unsigned srcId, unsigned dstId, unsigned pipeType,
			unsigned objectClass, unsigned objectType, unsigned objectId, unsigned priority,
			unsigned userId, bool isMTPipe = false);
	TransformationManagerOpenPipeEvent(NetMessage* msg, uint32_t sequenceNumber, unsigned id);
	virtual ~TransformationManagerOpenPipeEvent();

	typedef EventFactory<TransformationManagerOpenPipeEvent> Factory;

	void addPipe(unsigned srcId, unsigned dstId, unsigned pipeType, unsigned objectClass,
			unsigned objectType, unsigned objectId, unsigned priority, unsigned userId,
			bool isMTPipe = false);
	void addPipe(TransformationPipe* pipe, unsigned priority);

	virtual void encode(NetMessage* message);
	virtual void decode(NetMessage* message);

	virtual void execute();
	virtual std::string toString();

protected:
	std::vector<TransformationPipeIdData*> pipeList;
}; // TransformationManagerOpenPipeEvent

class INVRS_SYSTEMCORE_API TransformationManagerClosePipeEvent : public Event {
public:
	TransformationManagerClosePipeEvent();
	TransformationManagerClosePipeEvent(TransformationPipe* pipe);
	TransformationManagerClosePipeEvent(NetMessage* msg, uint32_t sequenceNumber, unsigned id);
	virtual ~TransformationManagerClosePipeEvent();

	typedef EventFactory<TransformationManagerClosePipeEvent> Factory;

	void addPipe(unsigned srcId, unsigned dstId, unsigned pipeType, unsigned objectClass,
			unsigned objectType, unsigned objectId, unsigned userId);
	void addPipe(TransformationPipe* pipe);

	virtual void encode(NetMessage* message);
	virtual void decode(NetMessage* message);
	virtual void execute();
	virtual std::string toString();

protected:
	std::vector<TransformationPipeIdData*> pipeList;
}; // TransformationManagerClosePipeEvent

#endif
