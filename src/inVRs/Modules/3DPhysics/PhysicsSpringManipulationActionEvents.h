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

#ifndef _PHYSICSSPRINGMANIPULATIONACTIONEVENTS_H
#define _PHYSICSSPRINGMANIPULATIONACTIONEVENTS_H

#include "PhysicsEntity.h"

#include <inVRs/SystemCore/EventManager/Event.h>
#include <inVRs/SystemCore/EventManager/EventFactory.h>

/** Events sent by the PhysicsSpringManipulationActionModel.
 * These events are sent by the PhysicsSpringManipulationActionModel when a
 * user starts to manipulate an Entity. Depending on the type of Entity
 * different TransformationPipes are created. When the Entity is a
 * PhysicsEntity then no Pipe is opened since the Physics modules manages the
 * TransformationPipes for all PhysicsEntities itself. When the Entity is not a
 * PhysicsEntity then a standard ManipulationPipe for Natural Interaction is
 * opened. This allows to manipulate PhysicsEntities and other Entities with the
 * same manipulation model.
 */
class PhysicsSpringManipulationActionEvent : public Event
{
public:

//*****************//
// PUBLIC METHODS: //
//*****************//

	/** Empty constructor.
	 */
	PhysicsSpringManipulationActionEvent();

	/** Constructor ... .
	 */
	PhysicsSpringManipulationActionEvent(std::vector<PhysicsEntity*>* physicsEntities, std::vector<Entity*>* nonPhysicsEntities, bool beginManipulation);

	/** Empty destructor.
	 */
	virtual ~PhysicsSpringManipulationActionEvent();

//**************************************//
// PUBLIC METHODS INHERITED FROM: Event //
//**************************************//
	
	virtual void encode(NetMessage* message);
	virtual void decode(NetMessage* message);
	virtual void execute() = 0;

protected:

//********************//
// PROTECTED MEMBERS: //
//********************//

	/// boolean value indicating if the manipulation should be started or ended
	bool beginManipulation;

	/// ID of the user sending this event
	unsigned userId;

	/// List of all PhysicsEntities which should be highlighted
	std::vector<unsigned> physicsEntityIds;
	/// List of all Non-PhysicsEntities which should be highlighted
	std::vector<unsigned> nonPhysicsEntityIds;

}; // PhysicsSpringManipulationActionEvent

class PhysicsBeginSpringManipulationActionEvent : public PhysicsSpringManipulationActionEvent
{
public:

//*****************//
// PUBLIC METHODS: //
//*****************//

	/** Empty constructor.
	 */
	PhysicsBeginSpringManipulationActionEvent();

	/** Dummy constructor calling parent constructor.
	 */
	PhysicsBeginSpringManipulationActionEvent(std::vector<PhysicsEntity*>* physicsEntities, std::vector<Entity*>* nonPhysicsEntities);

	/** Empty destructor.
	 */
	virtual ~PhysicsBeginSpringManipulationActionEvent();

	typedef EventFactory<PhysicsBeginSpringManipulationActionEvent> Factory;

//*********************************************************************//
// PUBLIC METHODS INHERITED FROM: PhysicsSpringManipulationActionEvent //
//*********************************************************************//

	virtual void execute();

}; // PhysicsBeginSpringManipulationActionEvent

class PhysicsEndSpringManipulationActionEvent : public PhysicsSpringManipulationActionEvent
{
public:

//*****************//
// PUBLIC METHODS: //
//*****************//

	/** Empty constructor.
	 */
	PhysicsEndSpringManipulationActionEvent();

	/** Dummy constructor calling parent constructor.
	 */
	PhysicsEndSpringManipulationActionEvent(std::vector<PhysicsEntity*>* physicsEntities, std::vector<Entity*>* nonPhysicsEntities);

	/** Empty destructor.
	 */
	virtual ~PhysicsEndSpringManipulationActionEvent();

	typedef EventFactory<PhysicsEndSpringManipulationActionEvent> Factory;
	
//*********************************************************************//
// PUBLIC METHODS INHERITED FROM: PhysicsSpringManipulationActionEvent //
//*********************************************************************//

	virtual void execute();

}; // PhysicsEndSpringManipulationActionEvent

#endif // _PHYSICSSPRINGMANIPULATIONACTIONEVENTS_H
