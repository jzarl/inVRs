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

#include "PhysicsHomerManipulationActionModel.h"

#include <assert.h>

#include <inVRs/SystemCore/UserDatabase/UserDatabase.h>
#include <inVRs/SystemCore/TransformationManager/TransformationManager.h>
#include <inVRs/Modules/Interaction/HomerCursorModel.h>
#include "CreateCursorSpringConnectorEvent.h"
#include "DeleteCursorSpringConnectorEvent.h"

PhysicsHomerManipulationActionModel::PhysicsHomerManipulationActionModel() {
	localUser = UserDatabase::getLocalUser();
}//PhysicsHomerManipulationActionModel

PhysicsHomerManipulationActionModel::~PhysicsHomerManipulationActionModel() {

}//~PhysicsHomerManipulationActionModel

void PhysicsHomerManipulationActionModel::enter(std::vector<PICKEDENTITY>* entities) {
	int i;
	cursorModel = localUser->getCursorTransformationModel();

	if (cursorModel->getName() != "HomerCursorModel") {
		printd(WARNING,
				"PhysicsHomerManipulationActionModel: Wrong CursorTransformationModel used. Use HomerCursorModel!\n");
		return;
	} // if

	assert(physicsEntities.size() == 0);
	PhysicsEntity* physicsEntity;
	for (i=0; i < (int)entities->size(); i++)
	{
		physicsEntity = dynamic_cast<PhysicsEntity*>(entities->at(i).entity);
		if (physicsEntity)
			physicsEntities.push_back(physicsEntity);
		else
			printd(WARNING, "PhysicsHomerManipulationActionModel::enter(): manipulation of non-physics entity not supported by this manipulation model!\n");
	} // for

	((HomerCursorModel*)cursorModel)->startForwardAnimation(entities);
	forwardAnimationRunning = true;
}//enter

void PhysicsHomerManipulationActionModel::exit() {
	if (cursorModel->getName() != "HomerCursorModel") {
		printd(WARNING,
				"PhysicsHomerManipulationActionModel: Wrong CursorTransformationModel used. Use HomerCursorModel!");
		return;
	}
	((HomerCursorModel*)cursorModel)->startBackwardAnimation();

	for (int i=0; i < (int)physicsEntities.size(); i++)
	{
		DeleteCursorSpringConnectorEvent* event =
			new DeleteCursorSpringConnectorEvent(localUser->getId());
		EventManager::sendEvent(event, EventManager::EXECUTE_GLOBAL);
	} // for
	physicsEntities.clear();
}//exit

void PhysicsHomerManipulationActionModel::action(std::vector<PICKEDENTITY>* addedEntities, std::vector<
		PICKEDENTITY>* removedEntities, float dt) {
	if (cursorModel->getName() != "HomerCursorModel") {
		return;
	} // if

	if (forwardAnimationRunning && !((HomerCursorModel*)cursorModel)->isAnimating()) {
		for (int i=0; i < (int)physicsEntities.size(); i++)
		{
			CreateCursorSpringConnectorEvent* event = new CreateCursorSpringConnectorEvent(physicsEntities[i], localUser);
			EventManager::sendEvent(event, EventManager::EXECUTE_GLOBAL);
		} // for
		forwardAnimationRunning = false;
	} // if
}//action


StateActionModel* PhysicsHomerManipulationActionModelFactory::create(std::string className, void* args) {
	if (className != "PhysicsHomerManipulationActionModel")
		return NULL;

	return new PhysicsHomerManipulationActionModel;
} // create
