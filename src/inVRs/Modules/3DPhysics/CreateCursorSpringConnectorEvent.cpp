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

#include "CreateCursorSpringConnectorEvent.h"

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

CreateCursorSpringConnectorEvent::CreateCursorSpringConnectorEvent() :
	Event() {
	this->entityID = 0;
	this->userID = 0;
} // CreateCursorSpringConnectorEvent

CreateCursorSpringConnectorEvent::CreateCursorSpringConnectorEvent(PhysicsEntity* entity,
		User* user) :
	Event(INTERACTION_MODULE_ID, INTERACTION_MODULE_ID, "CreateCursorSpringConnectorEvent") {
	this->entityID = entity->getTypeBasedId();
	this->userID = user->getId();
} // CreateCursorSpringConnectorEvent

CreateCursorSpringConnectorEvent::~CreateCursorSpringConnectorEvent() {

} // ~CreateCursorSpringConnectorEvent

//**************************************//
// PUBLIC METHODS INHERITED FROM: Event //
//**************************************//

void CreateCursorSpringConnectorEvent::encode(NetMessage* message) {
	message->putUInt32(userID);
	message->putUInt32(entityID);
} // encode

void CreateCursorSpringConnectorEvent::decode(NetMessage* message) {
	userID = message->getUInt32();
	entityID = message->getUInt32();
} // decode

void CreateCursorSpringConnectorEvent::execute() {
	User* user;
	PhysicsEntity* entity;
	Physics* physicsModule;
	CursorSpringConnector* connector = NULL;

	entity = (PhysicsEntity*)WorldDatabase::getEntityWithTypeInstanceId(entityID);
	if (!entity) {
		unsigned short typeID, instanceID;
		split(entityID, typeID, instanceID);
		printd(
				ERROR,
				"CreateCursorSpringConnectorEvent::execute(): ERROR: Entity of type %u and instanceID %u could not be found! Ignoring Event!\n",
				typeID, instanceID);
		return;
	} // if

	user = UserDatabase::getUserById(userID);
	if (!user) {
		printd(
				ERROR,
				"CreateCursorSpringConnectorEvent::execute(): ERROR: Could not find user with ID %u! Ignoring Event!\n",
				userID);
		return;
	} // if

	physicsModule = (Physics*)SystemCore::getModuleByName("Physics");
	if (!physicsModule) {
		printd(
				ERROR,
				"CreateCursorSpringConnectorEvent::execute(): ERROR: Could not get Physics Module! Ignoring Event!\n");
		return;
	} // if

	connector = new CursorSpringConnector(entity, user);
	physicsModule->addPhysicsObject(connector);
} // execute

std::string CreateCursorSpringConnectorEvent::toString() {
	// TODO:
	return Event::toString();
} // CreateCursorSpringConnectorEvent
