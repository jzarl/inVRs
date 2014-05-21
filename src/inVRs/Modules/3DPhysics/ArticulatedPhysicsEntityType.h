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

#ifndef _ARTICULATEDPHYSICSENTITYTYPE_H
#define _ARTICULATEDPHYSICSENTITYTYPE_H

#include <inVRs/SystemCore/WorldDatabase/EntityType.h>
#ifdef INVRS_BUILD_TIME
#include "oops/ArticulatedBody.h"
#else
#include <inVRs/Modules/3DPhysics/oops/ArticulatedBody.h>
#endif

class ArticulatedPhysicsEntity;

//using namespace oops;

/** EntityType-class for ArticulatedPhysicsEntity.
 * This class is the EntityType managing the creation and destruction of
 * ArticulatedPhysicsEntities.
 */
class ArticulatedPhysicsEntityType : public EntityType
{
public:

//*****************//
// PUBLIC METHODS: //
//*****************//

	/** Constructor initializes the EntityType.
	 */
	ArticulatedPhysicsEntityType(int entType);

	/** Empty Destructor
	 */
	virtual ~ArticulatedPhysicsEntityType();


protected:
	friend class ArticulatedPhysicsEntity;
	friend class ArticulatedPhysicsEntityTypeFactory;

//********************//
// PROTECTED METHODS: //
//********************//

	/** Sets the mapping of the RigidBodies inside the ArticulatedBody to the
	 * Entities in the world.
	 * The method sets the mapping of a RigidBody inside the ArticulatedBody to
	 * an Entity. Each ArticulatedBody consists of a number of RigidBodies.
	 * These RigidBodies have to be represented by SimplePhysicsEntities yet. 
	 * In the Environment config file the ArticulatedPhysicsEntity has to be
	 * followed by a number of SimplePhysicsEntities each for one RigidBody
	 * inside the ArticulatedBody. The passed indices define the index of the
	 * RigidBody inside the ArticulatedBody and the ID-offset of the
	 * matching SimplePhysicsEntity to the ArticulatedPhysicsEntity.
	 * @param rigidBodyIndex Index of the RigidBody inside the ArticulatedBody
	 * @param succeedingEntityIndex ID-offset between the
	 *								ArticulatedPhysicsEntity and the
	 * 								SimplePhysicsEntity
	 */
	virtual void setRigidBodyMapping(unsigned rigidBodyId, unsigned succeedingEntity);

	/** Initializes the passed Entity.
	 * The method allows to initialize the passed Entity. It is called from the
	 * <code>createInstanceAtLoadTime</code> and
	 * <code>createInstanceUnchecked</code> methods. This allows to easily
	 * initialize Entities which derive from ArticulatedPhysicsEntity.
	 * @param entity Entity which should be initialized
	 */
	virtual void initializeInstance(ArticulatedPhysicsEntity* entity);


//**********************************************//
// PROTECTED METHODS INHERITED FROM: EntityType //
//**********************************************//

	/** Creates a new instance of itself at load time.
	 * The method creates a new ArticulatedPhysicsEntityEntity instance where
	 * the instanceId is requested from the loadTimeEntityIdPool. The physical
	 * representation is cloned and stored in the new ArticulatedPhysicsEntity.
	 * @param idOfEnv ID of the destination Environment
	 * @param idInEnv ID of the Entity in the Environment
	 * @return new Entity instance of the current EntityType
	 */
	virtual Entity* createInstanceAtLoadTime(unsigned short idOfEnv, unsigned short idInEnv);

	/** Creates a new instance of itself with the passed IDs.
	 * The method creates a new Entity instance with the passed IDs. The
	 * physical representation is cloned and stored in the new
	 * ArticulatedPhysicsEntity.
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
	oops::ArticulatedBody* articulatedBody;
	/// Mapping from RigidBody-index to SimplePhysicsEntity-ID-offset
	std::map<unsigned, unsigned> rigidBodyMapping;

};
#endif // _ARTICULATEDPHYSICSENTITYTYPE_H
