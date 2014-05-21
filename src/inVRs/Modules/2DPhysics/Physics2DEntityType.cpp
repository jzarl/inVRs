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

#include "Physics2DEntityType.h"
#include "Physics2DEntity.h"
#include <inVRs/SystemCore/DebugOutput.h>

#include <assert.h>

Physics2DEntityType::Physics2DEntityType(int type)
: EntityType(type)
{
// nothing to do here
}

Entity* Physics2DEntityType::createInstanceUnchecked(unsigned short instId, unsigned short idOfEnv, unsigned short idInEnv)
{
	Physics2DEntity* ret = new Physics2DEntity(idInEnv, idOfEnv, instId, this);
	instances.push_back(ret);
	return ret;
} // createInstanceUnchecked

Entity* Physics2DEntityType::createInstanceAtLoadTime(unsigned short idOfEnv, unsigned short idInEnv)
{
	unsigned short entId;

	assert(loadTimeEntityIdPool);
	entId = (unsigned short)loadTimeEntityIdPool->allocEntry();
	Physics2DEntity* ret = new Physics2DEntity(idInEnv, idOfEnv, entId, this);
	instances.push_back(ret);
	return ret;
} // createInstanceAtLoadTime

EntityType* Physics2DEntityTypeFactory::create(std::string className, void* args)
{
	if(className == "Physics2DEntityType")
		return new Physics2DEntityType((long)args);

	return NULL;
} // create
