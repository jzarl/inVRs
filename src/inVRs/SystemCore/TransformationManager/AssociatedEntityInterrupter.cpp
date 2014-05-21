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

#include "AssociatedEntityInterrupter.h"
#include "TransformationManager.h"
#include "../UserDatabase/UserDatabase.h"

bool AssociatedEntityInterrupter::interrupt(TransformationData* resultLastStage, TransformationPipe* currentPipe)
{
	unsigned short objectType;
	unsigned short objectId;
	unsigned entityId;
	unsigned temp, tempObjectType, tempObjectId;
	bool tempBool;
	uint64_t pipeId = currentPipe->getPipeId();
	User* user = UserDatabase::getLocalUser();

	TransformationManager::unpackPipeId(pipeId, &temp, &temp, &temp, &temp, &tempObjectType, &tempObjectId, &tempBool);
	objectType = (unsigned short)tempObjectType;
	objectId = (unsigned short)tempObjectId;
	entityId = join(objectType, objectId);

	if (user->getAssociatedEntity(entityId))
		return false;

	return true;
} // interrupt

AssociatedEntityInterrupterFactory::AssociatedEntityInterrupterFactory()
{
	className = "AssociatedEntityInterrupter";
}

TransformationModifier* AssociatedEntityInterrupterFactory::createInternal(ArgumentVector* args)
{
	return new AssociatedEntityInterrupter;
} // create

bool AssociatedEntityInterrupterFactory::needSingleton()
{
	return true;
}

