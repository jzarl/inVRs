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

#ifndef _DELETECURSORSPRINGCONNECTOREVENT_H
#define _DELETECURSORSPRINGCONNECTOREVENT_H

#include "PhysicsEntity.h"

#include <inVRs/SystemCore/UserDatabase/User.h>
#include <inVRs/SystemCore/EventManager/Event.h>
#include <inVRs/SystemCore/EventManager/EventFactory.h>

/** Event for removing an existing CursorSpringConnector.
 * This Event is sent by a User who wants to stop the manipulation of a
 * PhysicsEntity. It is executed on every host.
 */
class DeleteCursorSpringConnectorEvent : public Event
{
public:

//*****************//
// PUBLIC METHODS: //
//*****************//

	/** Default Constructor needed for factory.
	 */
	DeleteCursorSpringConnectorEvent();

	/** Constructor initializing the Event.
	 * @param userID user who wants to stop the manipulation
	 */
	DeleteCursorSpringConnectorEvent(unsigned userID);

	/** Empty Destructor.
	 */
	virtual ~DeleteCursorSpringConnectorEvent();

	typedef EventFactory<DeleteCursorSpringConnectorEvent> Factory;

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

}; // DeleteCursorSpringConnectorEvent

#endif // _DELETECURSORSPRINGCONNECTOREVENT_H
