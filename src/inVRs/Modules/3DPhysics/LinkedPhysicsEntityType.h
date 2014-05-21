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

/*
 * LinkedPhysicsEntityType.h
 *
 *  Created on: Dec 16, 2008
 *      Author: rlander
 */

#ifndef LINKEDPHYSICSENTITYTYPE_H_
#define LINKEDPHYSICSENTITYTYPE_H_

#include <inVRs/SystemCore/WorldDatabase/EntityType.h>
#ifdef INVRS_BUILD_TIME
#include "oops/Joints.h"
#else
#include <inVRs/Modules/3DPhysics/oops/Joints.h>
#endif

class LinkedPhysicsEntity;
class SimplePhysicsEntityType;

//using namespace oops;

/**
 *
 */
class LinkedPhysicsEntityType : public EntityType {
public:

	struct SubEntityType {
		SimplePhysicsEntityType* type;
		TransformationData offset;
	};

	//*****************//
	// PUBLIC METHODS: //
	//*****************//

	/**
	 *
	 */
	LinkedPhysicsEntityType(unsigned short id);

	/**
	 *
	 */
	virtual ~LinkedPhysicsEntityType();

	/**
	 *
	 */
	void addSubEntity(SimplePhysicsEntityType* entityType, const TransformationData& offset);

	/**
	 *
	 */
	void addJoint(oops::Joint* joint, int entityIndex1, int entityIndex2);

	/**
	 *
	 */
	const std::vector<SubEntityType>& getSubEntityTypes();

protected:

	struct JointMapping {
		oops::Joint* joint;
		unsigned entityIndex1;
		unsigned entityIndex2;
	}; // JointMapping

	//**********************************************//
	// PROTECTED METHODS INHERITED FROM: EntityType //
	//**********************************************//

	/** Creates a new instance of itself at load time.
	 * The method creates a new LinkedPhysicsEntity instance where
	 * the instanceId is requested from the loadTimeEntityIdPool.
	 * @param idOfEnv ID of the destination Environment
	 * @param idInEnv ID of the Entity in the Environment
	 * @return new Entity instance of the current EntityType
	 */
	virtual Entity* createInstanceAtLoadTime(unsigned short idOfEnv, unsigned short idInEnv);

	/** Creates a new instance of itself with the passed IDs.
	 * The method creates a new Entity instance with the passed IDs.
	 * @param instId ID of the Entity instance
	 * @param idOfEnv ID of the destination Environment
	 * @param idInEnv ID of the Entity in the Environment
	 * @return new Entity instance of the current EntityType
	 */
	virtual Entity* createInstanceUnchecked(unsigned short instId, unsigned short idOfEnv,
			unsigned short idInEnv);

	/** Creates the EntityCreationEvent for a new instance.
	 */
	virtual Event* createEntityCreationEvent(Environment* env, unsigned environmentBasedId,
			TransformationData trans);

	//********************//
	// PROTECTED MEMBERS: //
	//********************//

	/// list of all members of the LinkedPhysicsEntity
	std::vector<SubEntityType> subEntityList;
	/// list of all defined Joints
	std::vector<JointMapping> jointList;

}; // LinkedPhysicsEntityType

#endif /* LINKEDPHYSICSENTITYTYPE_H_ */
