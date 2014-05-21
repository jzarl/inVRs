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

#ifndef _SIMPLEPHYSICSENTITYTYPE_H
#define _SIMPLEPHYSICSENTITYTYPE_H

#include <inVRs/SystemCore/WorldDatabase/EntityType.h>
#ifdef INVRS_BUILD_TIME
#include "oops/RigidBody.h"
#else
#include <inVRs/Modules/3DPhysics/oops/RigidBody.h>
#endif

class Physics;
class SimplePhysicsEntity;
class SimplePhysicsEntityTypeFactory;

//using namespace oops;

/** EntityType-class for SimplePhysicsEntity.
 * This class is the EntityType managing the creation and destruction of
 * SimplePhysicsEntities.
 */
class SimplePhysicsEntityType : public EntityType
{
public:

//*****************//
// PUBLIC METHODS: //
//*****************//

	/** Constructor initializes the EntityType.
	 */
	SimplePhysicsEntityType(unsigned short entType);

	/** Empty Destructor
	 */
	virtual ~SimplePhysicsEntityType();

	/**
	 */
	oops::RigidBody* getRigidBody();

	/**
	 */
	void setRigidBody(oops::RigidBody* rigidBody);

protected:
	friend class SimplePhysicsEntity;
	friend class LinkedPhysicsEntityType;
	friend class SimplePhysicsEntityTypeFactory;

//**********************************************//
// PROTECTED METHODS INHERITED FROM: EntityType //
//**********************************************//

	/** Creates a new instance of itself at load time.
	 * The method creates a new SimplePhysicsEntityEntity instance where the
	 * instanceId is requested from the loadTimeEntityIdPool. The physical
	 * representation is cloned and stored in the new SimplePhysicsEntity.
	 * @param idOfEnv ID of the destination Environment
	 * @param idInEnv ID of the Entity in the Environment
	 * @return new Entity instance of the current EntityType
	 */
	virtual Entity* createInstanceAtLoadTime(unsigned short idOfEnv, unsigned short idInEnv);

	/** Creates a new instance of itself with the passed IDs.
	 * The method creates a new Entity instance with the passed IDs. The
	 * physical representation is cloned and stored in the new
	 * SimplePhysicsEntity.
	 * @param instId ID of the Entity instance
	 * @param idOfEnv ID of the destination Environment
	 * @param idInEnv ID of the Entity in the Environment
	 * @return new Entity instance of the current EntityType
	 */
	virtual Entity* createInstanceUnchecked(unsigned short instId, unsigned short idOfEnv, unsigned short idInEnv);

//********************//
// PROTECTED MEMBERS: //
//********************//

	/// Physical representation used for Entities
	oops::RigidBody* rigidBody;

};
#endif // _SIMPLEPHYSICSENTITYTYPE_H
