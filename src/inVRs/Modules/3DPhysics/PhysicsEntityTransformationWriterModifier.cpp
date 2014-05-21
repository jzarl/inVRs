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

#include "PhysicsEntityTransformationWriterModifier.h"

#include "PhysicsEntity.h"

#include <inVRs/SystemCore/WorldDatabase/WorldDatabase.h>
#include <inVRs/SystemCore/TransformationManager/TransformationManager.h>

OSG_USING_NAMESPACE

//*******************************************************//
// PUBLIC METHODS INHERITED FROM: TransformationModifier //
//*******************************************************//

TransformationData PhysicsEntityTransformationWriterModifier::execute(TransformationData* resultLastStage, TransformationPipe* currentPipe)
{
	unsigned short objectType;
	unsigned short objectId;
	unsigned temp, tempObjectType, tempObjectId;
	bool tempBool;
	Environment *currentEnv, *newEnv;
	uint64_t pipeId = currentPipe->getPipeId();
	TransformationData result = *resultLastStage;

// TODO: document this problem
	if (currentPipe->size() == 0)
		return *resultLastStage;

	TransformationManager::unpackPipeId(pipeId, &temp, &temp, &temp, &temp, &tempObjectType, &tempObjectId, &tempBool);
	objectType = (unsigned short) tempObjectType;
	objectId = (unsigned short) tempObjectId;

	PhysicsEntity* ent = (PhysicsEntity*)WorldDatabase::getEntityWithTypeInstanceId(objectType, objectId);

	if (!ent)
		printd(WARNING, "PhysicsEntityTransformationWriter::execute(): couldn't find any entity with typeid %u and instanceid %u\n", objectType, objectId);
	else
	{
// TODO: should we really take the scale-value from the Entity???
		result.scale = ent->getWorldTransformation().scale;
		ent->setEntityWorldTransformation(result);

		currentEnv = ent->getEnvironment();
		newEnv = WorldDatabase::getEnvironmentAtWorldPosition(result.position[0], result.position[2]);
		if (newEnv && newEnv != currentEnv)
			ent->changeEnvironment(newEnv);

		// TODO: do we still have to do this here?
		ent->update();
	} // else
	return result;
} // execute

//============================================================================//
/* CLASS: PhysicsEntityTransformationWriterModifierFactory                    */

//**************************************************************//
// PUBLIC METHODS INHERITED FROM: TransformationModifierFactory //
//**************************************************************//

PhysicsEntityTransformationWriterModifierFactory::PhysicsEntityTransformationWriterModifierFactory()
{
	className = "PhysicsEntityTransformationWriterModifier";
}

TransformationModifier* PhysicsEntityTransformationWriterModifierFactory::createInternal(ArgumentVector* args)
{
	return new PhysicsEntityTransformationWriterModifier;
} // create

bool PhysicsEntityTransformationWriterModifierFactory::needSingleton()
{
	return true;
} // needSingleton

