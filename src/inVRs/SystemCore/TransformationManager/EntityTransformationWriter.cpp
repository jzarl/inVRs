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

#include "EntityTransformationWriter.h"

#include "TransformationManager.h"
#include "../WorldDatabase/WorldDatabase.h"
#include "../WorldDatabase/Environment.h"
#include "../DebugOutput.h"

TransformationData EntityTransformationWriter::execute(TransformationData* resultLastStage,
		TransformationPipe* currentPipe) {
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

	TransformationManager::unpackPipeId(pipeId, &temp, &temp, &temp, &temp, &tempObjectType,
			&tempObjectId, &tempBool);
	objectType = (unsigned short)tempObjectType;
	objectId = (unsigned short)tempObjectId;

	Entity* ent = WorldDatabase::getEntityWithTypeInstanceId(objectType, objectId);
	// 	printf("Writing Transformation to Entity with name %s\n", ent->getEntityType()->getName().c_str());

	if (!ent)
		printd( WARNING,
				"EntityTransformationWriter::execute(): couldn't find any entity with typeid %u and instanceid %u\n",
				objectType, objectId);
	else {
		// TODO: should we really take the scale-value from the Entity???
		// 		result.scale = ent->getWorldTransformation().scale;
		ent->setWorldTransformation(result);

		currentEnv = ent->getEnvironment();
		newEnv = WorldDatabase::getEnvironmentAtWorldPosition(result.position[0],
				result.position[2]);
		if (newEnv && newEnv != currentEnv)
			ent->changeEnvironment(newEnv);

		// TODO: shouldn't this be done on other place?
		// 		ent->update();
	} // else
	return result;
} // execute

EntityTransformationWriterFactory::EntityTransformationWriterFactory() {
	className = "EntityTransformationWriter";
}

TransformationModifier* EntityTransformationWriterFactory::createInternal(ArgumentVector* args) {
	return new EntityTransformationWriter;
} // create

bool EntityTransformationWriterFactory::needSingleton() {
	return true;
}

