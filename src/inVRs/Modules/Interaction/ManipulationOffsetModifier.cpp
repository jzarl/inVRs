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

#include "ManipulationOffsetModifier.h"

#include <inVRs/SystemCore/TransformationManager/TransformationManager.h>
#include <inVRs/SystemCore/WorldDatabase/WorldDatabase.h>
#include <inVRs/SystemCore/DebugOutput.h>

ManipulationOffsetModifier::ManipulationOffsetModifier() {
} // ManipulationOffsetModifier

TransformationData ManipulationOffsetModifier::execute(TransformationData* resultLastStage,
		TransformationPipe* currentPipe) {
	TransformationData result;
	unsigned temp;
	unsigned short objectType;
	unsigned short objectId;
	unsigned entityId;
	unsigned tempObjectType, tempObjectId;
	bool tempBool;
	uint64_t pipeId = currentPipe->getPipeId();

	if (currentPipe->size() == 0)
		return *resultLastStage;

	TransformationManager::unpackPipeId(pipeId, &temp, &temp, &temp, &temp, &tempObjectType,
			&tempObjectId, &tempBool);
	objectType = (unsigned short)tempObjectType;
	objectId = (unsigned short)tempObjectId;
	entityId = join(objectType, objectId);

	User* user = currentPipe->getOwner();
	TransformationData offset = user->getAssociatedEntityOffset(entityId);

	// TODO: Try to avoid this scale check, either by finding a solution to avoid negative scale in SingularValueDecomposition
	//	or by setting scale to (1,1,1)
	// 	assert((*resultLastStage).scale[0] == 1 && (*resultLastStage).scale[1] == 1 && (*resultLastStage).scale[2] == 1);

	multiply(result, *resultLastStage, offset);

	return result;
} // execute

ManipulationOffsetModifierFactory::ManipulationOffsetModifierFactory() {
	className = "ManipulationOffsetModifier";
}

TransformationModifier* ManipulationOffsetModifierFactory::createInternal(ArgumentVector* args) {
	return new ManipulationOffsetModifier;
} // create

bool ManipulationOffsetModifierFactory::needSingleton() {
	return true;
}

