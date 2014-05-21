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

#ifndef _PHYSICS2DENTITYTYPE_H
#define _PHYSICS2DENTITYTYPE_H

#include <inVRs/SystemCore/WorldDatabase/EntityType.h>
#include <inVRs/SystemCore/WorldDatabase/EntityTypeFactory.h>
#include "RigidBody.h"

class Physics2DEntity;

class Physics2DEntityType : public EntityType
{
protected:
	virtual Entity* createInstanceAtLoadTime(unsigned short idOfEnv, unsigned short idInEnv); //used by WorldDatabase during initialisation

public:
	Physics2DEntityType(int type);
	virtual ~Physics2DEntityType(){};
	virtual Entity* createInstanceUnchecked(unsigned short instId, unsigned short idOfEnv, unsigned short idInEnv);
};

class Physics2DEntityTypeFactory : public EntityTypeFactory
{
public:
	virtual EntityType* create(std::string className, void* args = NULL);
};

#endif // _PHYSICS2DENTITYTYPE_H
