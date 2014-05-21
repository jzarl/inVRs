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

#ifndef _CREATECURSORSPRINGCONNECTOREVENT_H
#define _CREATECURSORSPRINGCONNECTOREVENT_H

#include "PhysicsEntity.h"

#include <inVRs/SystemCore/UserDatabase/User.h>
#include <inVRs/SystemCore/EventManager/Event.h>
#include <inVRs/SystemCore/EventManager/EventFactory.h>

/** Event for creating a CursorSpringConnector.
 * This event is sent when a user tries to add a CursorSpringConnector to the
 * System. It is executed on every participating host.
 */
class CreateCursorSpringConnectorEvent : public Event {
public:

	//*****************//
	// PUBLIC METHODS: //
	//*****************//

	/** Default Constructor needed for factory.
	 */
	CreateCursorSpringConnectorEvent();

	/** Constructor
	 *
	 */
	CreateCursorSpringConnectorEvent(PhysicsEntity* entity, User* user);

	/** Empty Destructor
	 */
	virtual ~CreateCursorSpringConnectorEvent();

	typedef EventFactory<CreateCursorSpringConnectorEvent> Factory;

	//**************************************//
	// PUBLIC METHODS INHERITED FROM: Event //
	//**************************************//

	virtual void encode(NetMessage* message);
	virtual void decode(NetMessage* message);
	virtual void execute();
	virtual std::string toString();

protected:

	//********************//
	// PROTECTED MEMBERS: //
	//********************//

	/// ID of the user interacting with the entity.
	unsigned userID;
	/// ID of the entity to interact with.
	unsigned entityID;

}; // CreateCursorSpringConnectorEvent

#endif // _CREATECURSORSPRINGCONNECTOREVENT_H
