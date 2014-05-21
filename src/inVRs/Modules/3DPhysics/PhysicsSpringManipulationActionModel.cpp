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

#include "PhysicsSpringManipulationActionModel.h"

#include <assert.h>

#include <inVRs/SystemCore/ArgumentVector.h>
#include "CursorSpringConnector.h"
#include "CreateCursorSpringConnectorEvent.h"
#include "DeleteCursorSpringConnectorEvent.h"
#include "PhysicsSpringManipulationActionEvents.h"

//*****************//
// PUBLIC METHODS: //
//*****************//

PhysicsSpringManipulationActionModel::PhysicsSpringManipulationActionModel()
{
	localUser = UserDatabase::getLocalUser();
} // PhysicsSpringManipulationActionModel

PhysicsSpringManipulationActionModel::~PhysicsSpringManipulationActionModel()
{

} // ~PhysicsSpringManipulationActionModel

//***********************************************//
// PUBLIC METHODS DERIVED FROM: StateActionModel //
//***********************************************//

void PhysicsSpringManipulationActionModel::enter(std::vector<PICKEDENTITY>* entities)
{
	int i;
	PhysicsEntity* physicsEntity;

	assert(physicsEntities.size() == 0 && nonPhysicsEntities.size() == 0);

	for (i=0; i < (int)entities->size(); i++)
	{
		physicsEntity = dynamic_cast<PhysicsEntity*>(entities->at(i).entity);
		if (physicsEntity)
			physicsEntities.push_back(physicsEntity);
		else
			nonPhysicsEntities.push_back(entities->at(i).entity);
	} // for

	PhysicsBeginSpringManipulationActionEvent* event = new PhysicsBeginSpringManipulationActionEvent(&physicsEntities, &nonPhysicsEntities);

	for (i=0; i < (int)physicsEntities.size(); i++)
	{
		CreateCursorSpringConnectorEvent* event = new CreateCursorSpringConnectorEvent(physicsEntities[i], localUser);
		EventManager::sendEvent(event, EventManager::EXECUTE_GLOBAL);
	} // for

	EventManager::sendEvent(event, EventManager::EXECUTE_GLOBAL);
} // enter

void PhysicsSpringManipulationActionModel::exit()
{
	int i;

	for (i=0; i < (int)physicsEntities.size(); i++)
	{
		DeleteCursorSpringConnectorEvent* event = new DeleteCursorSpringConnectorEvent(localUser->getId());
		EventManager::sendEvent(event, EventManager::EXECUTE_GLOBAL);
	} // for

	PhysicsEndSpringManipulationActionEvent* event = new PhysicsEndSpringManipulationActionEvent(&physicsEntities, &nonPhysicsEntities);
	EventManager::sendEvent(event, EventManager::EXECUTE_GLOBAL);

	physicsEntities.clear();
	nonPhysicsEntities.clear();
} // exit

void PhysicsSpringManipulationActionModel::action(std::vector<PICKEDENTITY>* addedEntities, std::vector<PICKEDENTITY>* removedEntities, float dt)
{
	std::map<unsigned, TransformationPipe*>::iterator it;
	TransformationData cursorTrans = localUser->getCursorTransformation();
	
	for (it = nonPhysicsEntityPipes.begin(); it != nonPhysicsEntityPipes.end(); ++it)
	{
		it->second->push_back(cursorTrans);
	} // for
} // PhysicsSpringManipulationActionModel

//********************//
// PROTECTED METHODS: //
//********************//

/* DEPRECATED: PIPE FOR PHYSICSENTITY IS MANAGED BY PHYSICS MODULE
void PhysicsSpringManipulationActionModel::setPhysicsEntityPipe(unsigned entityId, TransformationPipe* pipe)
{
	physicsEntityPipes[entityId] = pipe;
} // setPhysicsEntityPipe

void PhysicsSpringManipulationActionModel::clearPhysicsEntityPipe(unsigned entityId)
{
	if (!physicsEntityPipes.erase(entityId))
		printd(ERROR, "PhysicsSpringManipulationActionModel::clearPhysicsEntityPipe(): INTERNAL ERROR: Pipe not found!\n");
} // clearPhysicsEntityPipe
*/

void PhysicsSpringManipulationActionModel::setNonPhysicsEntityPipe(unsigned entityId, TransformationPipe* pipe)
{
	nonPhysicsEntityPipes[entityId] = pipe;
} // setManipulationPipe

void PhysicsSpringManipulationActionModel::clearNonPhysicsEntityPipe(unsigned entityId)
{
	if (!nonPhysicsEntityPipes.erase(entityId))
		printd(ERROR, "PhysicsSpringManipulationActionModel::clearNonPhysicsEntityPipe(): INTERNAL ERROR: Pipe not found!\n");
} // clearManipulationPipe

//*****************//
// PUBLIC METHODS: //
//*****************//

StateActionModel* PhysicsSpringManipulationActionModelFactory::create(std::string className, void* args)
{
	if (className != "PhysicsSpringManipulationActionModel")
		return NULL;

	float linearSpringConst, linearDampingConst;
	float angularSpringConst, angularDampingConst;
	ArgumentVector* argVec = (ArgumentVector*)args;
	
	if (argVec) {
		if (argVec->keyExists("linearSpringConstant") && argVec->keyExists("linearDampingConstant")) {
			argVec->get("linearSpringConstant", linearSpringConst);
			argVec->get("linearDampingConstant", linearDampingConst);
			CursorSpringConnector::setLinearConstants(linearSpringConst, linearDampingConst);
		} // if
		if (argVec->keyExists("angularSpringConstant") && argVec->keyExists("angularDampingConstant")) {
			argVec->get("angularSpringConstant", angularSpringConst);
			argVec->get("angularDampingConstant", angularDampingConst);
			CursorSpringConnector::setAngularConstants(angularSpringConst, angularDampingConst);
		} // if
	} // if
	
	return new PhysicsSpringManipulationActionModel;
} // create
