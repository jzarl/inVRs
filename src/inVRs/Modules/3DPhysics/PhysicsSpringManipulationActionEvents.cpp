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

#include "PhysicsSpringManipulationActionEvents.h"

#include "PhysicsSpringManipulationActionModel.h"

#include "../Interaction/Interaction.h"
#include <inVRs/SystemCore/MessageFunctions.h>
#include <inVRs/SystemCore/SystemCore.h>
#include <inVRs/SystemCore/UserDatabase/UserDatabase.h>


PhysicsSpringManipulationActionEvent::PhysicsSpringManipulationActionEvent()
: Event()
{

} // PhysicsSpringManipulationActionEvent

PhysicsSpringManipulationActionEvent::PhysicsSpringManipulationActionEvent(std::vector<PhysicsEntity*>* physicsEntities, std::vector<Entity*>* nonPhysicsEntities, bool beginManipulation)
: Event(INTERACTION_MODULE_ID, INTERACTION_MODULE_ID, beginManipulation ? "PhysicsBeginSpringManipulationActionEvent" : "PhysicsEndSpringManipulationActionEvent")
{
	int i;
	for (i=0; i < (int)physicsEntities->size(); i++)
		physicsEntityIds.push_back(physicsEntities->at(i)->getTypeBasedId());

	for (i=0; i < (int)nonPhysicsEntities->size(); i++)
		nonPhysicsEntityIds.push_back(nonPhysicsEntities->at(i)->getTypeBasedId());

	this->beginManipulation = beginManipulation;
	this->userId = UserDatabase::getLocalUserId();
} // PhysicsSpringManipulationActionEvent

PhysicsSpringManipulationActionEvent::~PhysicsSpringManipulationActionEvent()
{

} // ~PhysicsSpringManipulationActionEvent

//**************************************//
// PUBLIC METHODS INHERITED FROM: Event //
//**************************************//

void PhysicsSpringManipulationActionEvent::encode(NetMessage* message)
{
	int i;

	msgFunctions::encode(beginManipulation, message);
	msgFunctions::encode(userId, message);

	msgFunctions::encode((unsigned)physicsEntityIds.size(), message);
	for (i=0; i < (int)physicsEntityIds.size(); i++)
		msgFunctions::encode(physicsEntityIds[i], message);

	msgFunctions::encode((unsigned)nonPhysicsEntityIds.size(), message);
	for (i=0; i < (int)nonPhysicsEntityIds.size(); i++)
		msgFunctions::encode(nonPhysicsEntityIds[i], message);
} // encode

void PhysicsSpringManipulationActionEvent::decode(NetMessage* message)
{
	int i;
	unsigned size;
	unsigned entityId;

	msgFunctions::decode(beginManipulation, message);
	msgFunctions::decode(userId, message);

	msgFunctions::decode(size, message);
	for (i=0; i < (int)size; i++)
	{
		msgFunctions::decode(entityId, message);
		physicsEntityIds.push_back(entityId);
	} // for

	msgFunctions::decode(size, message);
	for (i=0; i < (int)size; i++)
	{
		msgFunctions::decode(entityId, message);
		nonPhysicsEntityIds.push_back(entityId);
	} // for
} // decode

//*****************//
// PUBLIC METHODS: //
//*****************//

PhysicsBeginSpringManipulationActionEvent::PhysicsBeginSpringManipulationActionEvent()
{

} // PhysicsBeginSpringManipulationActionEvent

PhysicsBeginSpringManipulationActionEvent::PhysicsBeginSpringManipulationActionEvent(std::vector<PhysicsEntity*>* physicsEntities, std::vector<Entity*>* nonPhysicsEntities)
: PhysicsSpringManipulationActionEvent(physicsEntities, nonPhysicsEntities, true)
{

} // PhysicsBeginSpringManipulationActionEvent

PhysicsBeginSpringManipulationActionEvent::~PhysicsBeginSpringManipulationActionEvent()
{

}

//*********************************************************************//
// PUBLIC METHODS INHERITED FROM: PhysicsSpringManipulationActionEvent //
//*********************************************************************//

void PhysicsBeginSpringManipulationActionEvent::execute()
{
	int i;
	unsigned short type, id;
	User* user;
	bool localPipe = false;
	PhysicsSpringManipulationActionModel* manipulationModel = NULL;
	TransformationPipe* manipulationPipe;

	user = UserDatabase::getUserById(userId);
	assert(user);
	if (userId == UserDatabase::getLocalUserId())
		localPipe = true;

	if (localPipe)
	{
		Interaction* interactionModule = (Interaction*)SystemCore::getModuleByName("Interaction");
		assert(interactionModule);
		manipulationModel = dynamic_cast<PhysicsSpringManipulationActionModel*>(interactionModule->getManipulationActionModel());
	} // if


/*  DEPRECATED: PIPE FOR PHYSICSENTITY IS MANAGED BY PHYSICS MODULE
	for (i=0; i < (int)physicsEntityIds.size(); i++)
	{
		split(physicsEntityIds[i], type, id);

	// 	printd("Opening Pipe with type %i and id %i\n", type, id);
// 		manipulationPipe = TransformationManager::openPipe(INTERACTION_MODULE_ID, WORLD_DATABASE_ID, 2, 0, type, id, 0, !localPipe, user);

// 		if (manipulationModel)
// 			manipulationModel->setPhysicsEntityPipe(physicsEntityIds[i], manipulationPipe);
	} // for
*/

	for (i=0; i < (int)nonPhysicsEntityIds.size(); i++)
	{
		split(nonPhysicsEntityIds[i], type, id);

	// 	printd("Opening Pipe with type %i and id %i\n", type, id);
		manipulationPipe = TransformationManager::openPipe(INTERACTION_MODULE_ID, WORLD_DATABASE_ID, 1, 0, type, id, 0, !localPipe, user);

		if (manipulationModel)
			manipulationModel->setNonPhysicsEntityPipe(nonPhysicsEntityIds[i], manipulationPipe);
	} // for
} // execute

//*****************//
// PUBLIC METHODS: //
//*****************//

PhysicsEndSpringManipulationActionEvent::PhysicsEndSpringManipulationActionEvent()
{

} // PhysicsEndSpringManipulationActionEvent

PhysicsEndSpringManipulationActionEvent::PhysicsEndSpringManipulationActionEvent(std::vector<PhysicsEntity*>* physicsEntities, std::vector<Entity*>* nonPhysicsEntities)
: PhysicsSpringManipulationActionEvent(physicsEntities, nonPhysicsEntities, false)
{

} // PhysicsEndSpringManipulationActionEvent

PhysicsEndSpringManipulationActionEvent::~PhysicsEndSpringManipulationActionEvent()
{

} // PhysicsEndSpringManipulationActionEvent

//*********************************************************************//
// PUBLIC METHODS INHERITED FROM: PhysicsSpringManipulationActionEvent //
//*********************************************************************//

void PhysicsEndSpringManipulationActionEvent::execute()
{
	int i;
	unsigned short type, id;
	User* user;
	bool localPipe = false;
	TransformationPipe* manipulationPipe;
	PhysicsSpringManipulationActionModel* manipulationModel = NULL;

	user = UserDatabase::getUserById(userId);
	assert(user);
	if (userId == UserDatabase::getLocalUserId())
		localPipe = true;

	if (localPipe)
	{
		Interaction* interactionModule = (Interaction*)SystemCore::getModuleByName("Interaction");
		assert(interactionModule);
		manipulationModel = dynamic_cast<PhysicsSpringManipulationActionModel*>(interactionModule->getManipulationActionModel());
	} // if

/* DEPRECATED: PIPE FOR PHYSICSENTITY IS MANAGED BY PHYSICS MODULE
	for (i=0; i < (int)physicsEntityIds.size(); i++)
	{
		split(physicsEntityIds[i], type, id);

		manipulationPipe = TransformationManager::getPipe(INTERACTION_MODULE_ID, WORLD_DATABASE_ID, 2, 0, type, id, 0, !localPipe, user);

		if (!manipulationPipe)
			printd(WARNING, "PhysicsEndSpringManipulationActionEvent::execute(): could not find manipulation Pipe for user with ID %u\n", userId);
		else
			TransformationManager::closePipe(manipulationPipe);

		if (manipulationModel)
			manipulationModel->clearPhysicsEntityPipe(physicsEntityIds[i]);
	} // for
*/

	for (i=0; i < (int)nonPhysicsEntityIds.size(); i++)
	{
		split(nonPhysicsEntityIds[i], type, id);

		manipulationPipe = TransformationManager::getPipe(INTERACTION_MODULE_ID, WORLD_DATABASE_ID, 1, 0, type, id, 0, !localPipe, user);

		if (!manipulationPipe)
			printd(WARNING, "PhysicsEndSpringManipulationActionEvent::execute(): could not find manipulation Pipe for user with ID %u\n", userId);
		else
			TransformationManager::closePipe(manipulationPipe);

		if (manipulationModel)
			manipulationModel->clearNonPhysicsEntityPipe(nonPhysicsEntityIds[i]);
	} // for
} // execute
