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

#ifndef _CHANGEDUSERDATAEVENT_H
#define _CHANGEDUSERDATAEVENT_H

#include <inVRs/SystemCore/EventManager/Event.h>
#include <inVRs/SystemCore/EventManager/EventFactory.h>

#include "UserData.h"

/** Event for changes on local UserData.
 * This event is sent out when the user changes information of his local
 * UserData. It is sent to all other participants to notify them about the
 * changes.
 */
class ChangedUserDataEvent : public Event
{
public:

//*****************//
// PUBLIC METHODS: //
//*****************//

	/** Empty constructor for factory.
	 */
	ChangedUserDataEvent();

	/** Constructor gets userId from UserDatabase and stores UserData.
	 * @param data New UserData object.
	 */
	ChangedUserDataEvent(UserData* data);

	/** Empty destructor.
	 */
	virtual ~ChangedUserDataEvent();
	
	typedef EventFactory<ChangedUserDataEvent> Factory;

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

	/// ID of the user who changed his UserData
	unsigned userId;
	/// New version of UserData
	UserData* data;

}; // ChangedUserDataEvent

#endif // _CHANGEDUSERDATAEVENT_H
