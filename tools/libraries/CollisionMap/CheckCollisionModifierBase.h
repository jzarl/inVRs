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

/*---------------------------------------------------------------------------*\
 *                                Changes                                    *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
\*---------------------------------------------------------------------------*/

#ifndef _CHECKCOLLISIONMODIFIERBASE_H
#define _CHECKCOLLISIONMODIFIERBASE_H

#include "CollisionMap.h"
#include "CollisionMapExports.h"
#include <inVRs/SystemCore/TransformationManager/TransformationModifierFactory.h>

/******************************************************************************
 *
 */
class CheckCollisionModifierBase : public TransformationModifier {
public:
	CheckCollisionModifierBase(CollisionMap* collisionMap);

	~CheckCollisionModifierBase();

	virtual TransformationData execute(TransformationData* resultLastStage,
			TransformationPipe* currentPipe);

protected:
	CollisionMap* collisionMap;
	std::map<unsigned, TransformationData*> lastUserTransformation;
	int maxRepeats;
};

/******************************************************************************
 *
 */
class CheckCollisionModifierFactoryBase : public TransformationModifierFactory {
public:
	CheckCollisionModifierFactoryBase(CollisionLineSetFactory* collisionLineSetFactory);
	~CheckCollisionModifierFactoryBase();

protected:
	bool needInstanceForEachPipe();
	virtual TransformationModifier* createInternal(ArgumentVector* args = NULL);

private:
	CollisionLineSetFactory* collisionLineSetFactory;
}; //CheckCollisionModifierFactory

#endif
