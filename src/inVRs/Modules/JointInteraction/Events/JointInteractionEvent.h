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

#ifndef _JOINTINTERACTIONEVENT_H
#define _JOINTINTERACTIONEVENT_H

#include <inVRs/SystemCore/EventManager/Event.h>
#include <inVRs/SystemCore/EventManager/EventFactory.h>
#include <inVRs/SystemCore/WorldDatabase/Entity.h>

class JointInteractionEvent : public Event
{
public:
	JointInteractionEvent();
	JointInteractionEvent(unsigned userId, std::vector<unsigned> entityIds, bool bBegin);

	virtual void encode(NetMessage* message);
	virtual void decode(NetMessage* message);

protected:
	unsigned userId;
	std::vector<unsigned> entityIds;
}; // JointInteractionEvent

class JointInteractionBeginEvent : public JointInteractionEvent
{
public:
	JointInteractionBeginEvent();
	JointInteractionBeginEvent(unsigned userId, std::vector<unsigned> entityIds);

	typedef EventFactory<JointInteractionBeginEvent> Factory;

	virtual void execute();
	virtual std::string toString();
}; // BeginJointInteractionEvent

class JointInteractionEndEvent : public JointInteractionEvent
{
public:
	JointInteractionEndEvent();
	JointInteractionEndEvent(unsigned userId, std::vector<unsigned> entityIds);

	typedef EventFactory<JointInteractionEndEvent> Factory;

	virtual void addJointID(int jointID);
	virtual void addFloat(float value);
	virtual float getFloat();
	virtual void addEntityId(unsigned entityId);
	virtual void addTransformationData(TransformationData trans);

	virtual void encode(NetMessage* message);
	virtual void decode(NetMessage* message);
	virtual void execute();
	virtual std::string toString();
	

protected:
	std::vector<int> jointIDs;
	std::vector<float> floatValues;
// 	std::vector<unsigned> entityIds;
	std::vector<TransformationData> entityTransformations;
	int floatIdx;

}; // EndJointInteractionEvent


#endif // _JOINTINTERACTIONEVENT_H
