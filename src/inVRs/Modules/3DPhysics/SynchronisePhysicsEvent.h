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

#ifndef _SYNCHRONISEPHYSICSEVENT_H
#define _SYNCHRONISEPHYSICSEVENT_H

#include "UserData.h"

#include <inVRs/SystemCore/EventManager/Event.h>
#include <inVRs/SystemCore/EventManager/EventFactory.h>

class TransformationPipe;

/**
 *
 */
class SynchronisePhysicsEvent : public Event
{
public:

//*****************//
// PUBLIC METHODS: //
//*****************//

	/**
	 *
	 */
	SynchronisePhysicsEvent();

	/**
	 *
	 */
// 	SynchronisePhysicsEvent(ExtendedBinaryMessage* msg, UInt32 sequenceNumber, unsigned id);

	/**
	 *
	 */
	virtual ~SynchronisePhysicsEvent();


	typedef EventFactory<SynchronisePhysicsEvent> Factory;


	/** Adds the synchronisation message of the PhysicsObjectManager.
	 *
	 */
	void addPhysicsObjectManagerMsg(NetMessage* msg);

//**************************************//
// PUBLIC METHODS INHERITED FROM: Event //
//**************************************//

	virtual void encode(NetMessage* message);
	virtual void decode(NetMessage* message);
	virtual void execute();
	virtual std::string toString();

//****************************//
// DEPRECATED PUBLIC METHODS: //
//****************************//

// 	void addPipe(TransformationPipe* pipe);

protected:

//********************//
// PROTECTED MEMBERS: //
//********************//

	/// synchronisation message of PhysicsObjectManager
	NetMessage* objectManagerMsg;
	/// map containing userData of all known users
//	std::map<unsigned, UserData*> userDataMap;

	/// time when the server started the simulation
	double simulationStartTime;
	/// simulation time when Event was created
	unsigned simulationTime;
	/// number of seconds between two simulation steps
	float stepSize;

//********************************//
// DEPTRECATED PROTECTED MEMBERS: //
//********************************//

// 	struct PipeData
// 	{
// 		UInt64 pipeId;
// 		unsigned priority;
// 		unsigned userId;
// 	}; // pipeData
//
// 	/// Opened Pipes
// 	std::vector<PipeData*> pipeList;
}; // SynchronisePhysicsEvent

#endif //_SYNCHRONISEPHYSICSEVENT_H
