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

#ifndef _2DPHYSICSEVENTS_H
#define _2DPHYSICSEVENTS_H

#include <inVRs/SystemCore/DataTypes.h>
#include <inVRs/SystemCore/EventManager/Event.h>
#include <inVRs/SystemCore/EventManager/EventFactory.h>

class Physics2DSyncEvent : public Event
{
public:
	Physics2DSyncEvent(void* dummy);
	virtual ~Physics2DSyncEvent();
	Physics2DSyncEvent();
	
	typedef EventFactory<Physics2DSyncEvent> Factory;

protected:
	struct TransformationPipeIdData
	{
		unsigned srcId;
		unsigned dstId;
		unsigned pipeType;
		unsigned objectClass;
		unsigned objectType;
		unsigned objectId;
		unsigned priority;
		unsigned userId;	
	}; // TransformationPipeIdData
	std::vector<TransformationPipeIdData*> pipeList;

	virtual void decode(NetMessage* message);
	virtual void encode(NetMessage* message);
	virtual void execute();
// 	virtual std::string toString();

}; // Physics2DSyncEvent

//class Physics2DSyncEventFactory : public EventFactory
//{
//public:
//	virtual Event* create(unsigned eventId);
//}; // Physics2DSyncEventFactory

#endif // _2DPHYSICSEVENTS_H
