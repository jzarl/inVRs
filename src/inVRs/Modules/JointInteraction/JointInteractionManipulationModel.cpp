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

#include "JointInteractionManipulationModel.h"

#include <assert.h>
#include <vector>

#include "JointInteraction.h"

#include <inVRs/SystemCore/UserDatabase/UserDatabase.h>
#include <inVRs/SystemCore/SystemCore.h>

//*****************//
// PUBLIC METHODS: //
//*****************//

JointInteractionManipulationModel::JointInteractionManipulationModel()
{
	localUser = UserDatabase::getLocalUser();
	jointInteraction = (JointInteraction*)SystemCore::getModuleByName("JointInteraction");
// 	manipulationPipe = NULL;
} // JointInteractionManipulationModel

//*************************************************//
// PUBLIC METHODS INHERITED FROM: StateActionModel //
//*************************************************//

void JointInteractionManipulationModel::enter(std::vector<PICKEDENTITY>* entities)
{
	Entity* entity;
	TransformationData cursorTrans, offset;

	assert(entities);
	entity = (entities->at(0)).entity;
	cursorTrans = localUser->getCursorTransformation();
	jointInteraction->update(cursorTrans);
	assert(localUser->getAssociatedEntity(entity->getTypeBasedId()));
	offset = localUser->getAssociatedEntityOffset(entity->getTypeBasedId());
	jointInteraction->grab(entity->getEnvironmentBasedId(), offset);
} // enter

void JointInteractionManipulationModel::exit()
{
	jointInteraction->ungrab();
// 	pipe = NULL;
} // exit

void JointInteractionManipulationModel::action(std::vector<PICKEDENTITY>* addedEntities, std::vector<PICKEDENTITY>* removedEntities, float dt)
{
	TransformationData cursorTrans = localUser->getCursorTransformation();
	TransformationData result = jointInteraction->update(cursorTrans);

/*	if (!jointInteraction->grabbing())
	{
		result = entity->getEnvironmentTransformation();
		result.scale = gmtl::Vec3f(1,1,1);
	} // if
	return result;*/
} // action

//********************//
// PROTECTED METHODS: //
//********************//

// void JointInteractionManipulationModel::registerManipulationPipe(TransformationPipe* pipe)
// {
// 	this->manipulationPipe = pipe;
// } // JointInteractionManipulationModel

/*
void JointInteractionManipulationModel::beginManipulation(Entity* entity, TransformationData offset)
{
	TransformationData cursorTrans = localUser->getCursorTransformation();
	jointInteraction->update(cursorTrans);
	jointInteraction->grab(entity->getEnvironmentBasedId(), offset);
} // beginManipulation

void JointInteractionManipulationModel::endManipulation(Entity* entity)
{
	jointInteraction->ungrab();
} // endManipulation

TransformationData JointInteractionManipulationModel::manipulate(Entity* entity, std::vector<BUTTONPRESSED*>* buttonPressed, gmtl::Vec3f position, gmtl::Quatf orientation)
{
	TransformationData cursorTrans = identityTransformation();
	cursorTrans.position = position;
	cursorTrans.orientation = orientation;
	TransformationData result = jointInteraction->update(cursorTrans);

// TODO: Avoid the possibility of being in Manipulation-State while JointInteraction is not!!!
	if (!jointInteraction->grabbing())
	{
		result = entity->getEnvironmentTransformation();
		result.scale = gmtl::Vec3f(1,1,1);
	} // if
	return result;
} // manipulate
*/
