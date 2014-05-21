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

#include "ChangedUserDataEvent.h"

#include "Physics.h"
#include "PhysicsMessageFunctions.h"

#include <inVRs/SystemCore/UserDatabase/UserDatabase.h>

//*****************//
// PUBLIC METHODS: //
//*****************//

ChangedUserDataEvent::ChangedUserDataEvent()
: Event(PHYSICS_MODULE_ID, PHYSICS_MODULE_ID, "ChangedUserDataEvent")
{
} // ChangedUserDataEvent

ChangedUserDataEvent::ChangedUserDataEvent(UserData* data)
: Event(PHYSICS_MODULE_ID, PHYSICS_MODULE_ID, "ChangedUserDataEvent")
{
	this->userId = UserDatabase::getLocalUserId();
	this->data = data;
} // ChangedUserDataEvent

ChangedUserDataEvent::~ChangedUserDataEvent()
{
} // ~ChangedUserDataEvent

//**************************************//
// PUBLIC METHODS INHERITED FROM: Event //
//**************************************//

void ChangedUserDataEvent::encode(NetMessage* message)
{
	msgFunctions::encode(userId, message);
	msgFunctions::encode(data, message);
} // encode

void ChangedUserDataEvent::decode(NetMessage* message)
{
	data = new UserData;
	msgFunctions::decode(userId, message);
	msgFunctions::decode(data, message);
} // decode

void ChangedUserDataEvent::execute()
{
	//TODO: event not needed any more
/*
	UserData* oldData;
	if (Physics::singleton->userDataMap.find(userId) != Physics::singleton->userDataMap.end())
	{
		oldData = Physics::singleton->userDataMap[userId];
		delete oldData;
	} // if
	Physics::singleton->userDataMap[userId] = data;

	printd(INFO, "ChangedUserDataEvent::execute(): updated UserData of user with ID %u\n", userId);
*/
} // execute

std::string ChangedUserDataEvent::toString()
{
// TODO:
	return "TODO: implement ChangedUserDataEvent!\n";
} // toString
