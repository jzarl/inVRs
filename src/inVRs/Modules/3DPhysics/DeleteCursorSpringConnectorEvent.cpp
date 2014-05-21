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

#include "DeleteCursorSpringConnectorEvent.h"

#include <assert.h>

#include "Physics.h"
#include "CursorSpringConnector.h"
#include "SimplePhysicsEntity.h"
#include "ArticulatedPhysicsEntity.h"

#include <inVRs/SystemCore/SystemCore.h>
#include <inVRs/SystemCore/UserDatabase/UserDatabase.h>
#include <inVRs/SystemCore/WorldDatabase/WorldDatabase.h>

//*****************//
// PUBLIC METHODS: //
//*****************//

DeleteCursorSpringConnectorEvent::DeleteCursorSpringConnectorEvent()
: Event()
{

} // DeleteCursorSpringConnectorEvent

DeleteCursorSpringConnectorEvent::DeleteCursorSpringConnectorEvent(unsigned userID)
: Event(INTERACTION_MODULE_ID, INTERACTION_MODULE_ID, "DeleteCursorSpringConnectorEvent") 
{
	this->userID = userID;
} // DeleteCursorSpringConnectorEvent

DeleteCursorSpringConnectorEvent::~DeleteCursorSpringConnectorEvent()
{

} // ~DeleteCursorSpringConnectorEvent

//**************************************//
// PUBLIC METHODS INHERITED FROM: Event //
//**************************************//

void DeleteCursorSpringConnectorEvent::encode(NetMessage* message)
{
	message->putUInt32(userID);
} // encode

void DeleteCursorSpringConnectorEvent::decode(NetMessage* message)
{
	userID = message->getUInt32();
} // decode

void DeleteCursorSpringConnectorEvent::execute()
{
	Physics* physicsModule;
	PhysicsObjectID physicsID = PhysicsObjectID(CursorSpringConnector::CURSORSPRINGCONNECTOR_CLASSID, userID);
	
	physicsModule = (Physics*)SystemCore::getModuleByName("Physics");
	assert(physicsModule);
	CursorSpringConnector* connector = (CursorSpringConnector*)physicsModule->removePhysicsObject(physicsID);

	if (connector)
		delete connector;
	else
		printd(ERROR, "DeleteCursorSpringConnectorEvent::execute(): could not remove CursorSpringConnector from Simulation!\n");
	
} // execute

std::string DeleteCursorSpringConnectorEvent::toString()
{
// TODO:
	return Event::toString();
} // toString
