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

#include "VirtualHandManipulationActionModel.h"

#include <assert.h>

#include "InteractionEvents.h"

#include <inVRs/SystemCore/UserDatabase/UserDatabase.h>
#include <inVRs/SystemCore/TransformationManager/TransformationManager.h>

//*****************//
// PUBLIC METHODS: //
//*****************//

VirtualHandManipulationActionModel::VirtualHandManipulationActionModel() {
	localUser = UserDatabase::getLocalUser();
} // VirtualHandManipulationActionModel

VirtualHandManipulationActionModel::~VirtualHandManipulationActionModel() {

} // ~VirtualHandManipulationActionModel

//***********************************************//
// PUBLIC METHODS DERIVED FROM: StateActionModel //
//***********************************************//

void VirtualHandManipulationActionModel::enter(std::vector<PICKEDENTITY>* entities) {
	int i;

	assert(manipulatingEntities.size() == 0);

	InteractionBeginVirtualHandManipulationActionEvent* event =
			new InteractionBeginVirtualHandManipulationActionEvent(entities);
	EventManager::sendEvent(event, EventManager::EXECUTE_GLOBAL);

	for (i = 0; i < (int)entities->size(); i++)
		manipulatingEntities.push_back(entities->at(i));
} // enter

void VirtualHandManipulationActionModel::exit() {
	InteractionEndVirtualHandManipulationActionEvent* event =
			new InteractionEndVirtualHandManipulationActionEvent(&manipulatingEntities);
	EventManager::sendEvent(event, EventManager::EXECUTE_GLOBAL);

	manipulatingEntities.clear();
} // exit

void VirtualHandManipulationActionModel::action(std::vector<PICKEDENTITY>* addedEntities,
		std::vector<PICKEDENTITY>* removedEntities, float dt) {
	std::map<unsigned, TransformationPipe*>::iterator it;
	TransformationData cursorTrans = localUser->getCursorTransformation();

	for (it = manipulationPipes.begin(); it != manipulationPipes.end(); ++it) {
		it->second->push_back(cursorTrans);
	} // for
} // action

//********************//
// PROTECTED METHODS: //
//********************//

void VirtualHandManipulationActionModel::setManipulationPipe(unsigned entityId,
		TransformationPipe* pipe) {
	manipulationPipes[entityId] = pipe;
} // setManipulationPipe

void VirtualHandManipulationActionModel::clearManipulationPipe(unsigned entityId) {
	if (!manipulationPipes.erase(entityId))
		printd(ERROR,
				"VirtualHandManipulationModel::clearManipulationPipe(): INTERNAL ERROR: Pipe not found!\n");
} // clearManipulationPipe

StateActionModel* VirtualHandManipulationActionModelFactory::create(std::string className,
		void* args) {
	if (className != "VirtualHandManipulationActionModel")
		return NULL;

	return new VirtualHandManipulationActionModel;
} // create
