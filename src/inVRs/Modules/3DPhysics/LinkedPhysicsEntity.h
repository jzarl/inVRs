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
 * LinkedPhysicsEntity.h
 *
 *  Created on: Dec 16, 2008
 *      Author: rlander
 */

#ifndef LINKEDPHYSICSENTITY_H_
#define LINKEDPHYSICSENTITY_H_

#include "PhysicsEntity.h"

class SimplePhysicsEntity;

/**
 *
 */
class LinkedPhysicsEntity : public PhysicsEntity {
public:

	//*****************//
	// PUBLIC METHODS: //
	//*****************//

	/** Constructor initializes Entity.
	 * @param id ID of the Entity
	 * @param environmentId ID of the Environment where the Entity is created
	 * @param instanceId ID for the instance this Entity is from the EntityType
	 * @param type EntityType this Entity is an instance of
	 */
	LinkedPhysicsEntity(unsigned short id, unsigned short environmentId,
			unsigned short instanceId, EntityType *type);

	/** Destructor removes Object from Physics Module.
	 */
	virtual ~LinkedPhysicsEntity();

	//************************************************//
	// PUBLIC METHODS INNHERITED FROM: PhysicsEntity: //
	//************************************************//

	/**
	 *
	 */
	virtual void getRigidBodies(std::vector<oops::RigidBody*>& rigidBodyList);

	/**
	 *
	 */
	virtual void getJoints(std::vector<oops::Joint*>& jointList);

	/**
	 *
	 */
	virtual PhysicsObjectID getPhysicsObjectID();

	//*****************************************//
	// PUBLIC METHODS INNHERITED FROM: Entity: //
	//*****************************************//

	virtual void setEnvironment(Environment* env);

	//*****************//
	// PUBLIC MEMBERS: //
	//*****************//

	/// unique class-ID used for PhysicsObjectID
	static const unsigned LINKEDPHYSICSENTITY_CLASSID;

protected:

	friend class LinkedPhysicsEntityType;
	friend class CreateLinkedPhysicsEntityEvent;

	struct JointWrapper {
		oops::Joint* joint;
		int subEntityIndex1;
		int subEntityIndex2;
	}; // JointWrapper

	//********************//
	// PROTECTED METHODS: //
	//********************//

	/**
	 *
	 */
	void addJoint(oops::Joint* joint, int entityIdx1, int entityIdx2);

	/**
	 *
	 */
	void initializeJoints();

	/**
	 *
	 */
	void setSubEntities(std::vector<Entity*> subEntityList);

	//********************//
	// PROTECTED MEMBERS: //
	//********************//

	/// Indicating if the physical representation is already initialized
	bool isInitialized;
	/// Indicating if Entity is added to physics simulation
	bool addedToSimulation;
	/// indicates that joints were already initialized
	bool initializedJoints;

	/// List of predefined Joints (needed to store for mapping)
	std::vector<JointWrapper> jointDefinitionList;
	/// List of all sub entities
	std::vector<SimplePhysicsEntity*> subEntityList;
	/// List of all available Joints
	std::vector<oops::Joint*> jointList;

	/// ID of the PhysicsObject
	PhysicsObjectID physicsObjectID;

}; // LinkedPhysicsEntity

#endif /* LINKEDPHYSICSENTITY_H_ */
