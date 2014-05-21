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

#ifndef _ARTICULATEDPHYSICSENTITY_H
#define _ARTICULATEDPHYSICSENTITY_H

#include "PhysicsEntity.h"
#ifdef INVRS_BUILD_TIME
#include "oops/ArticulatedBody.h"
#else
#include <inVRs/Modules/3DPhysics/oops/ArticulatedBody.h>
#endif

//using namespace oops;

class TransformationPipe;

/** Entity with ArticulatedBody as physical representation.
 * This class can be used for Entities which can be represented by an
 * ArticulatedBody.
 */
class ArticulatedPhysicsEntity : public PhysicsEntity
{
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
	ArticulatedPhysicsEntity(unsigned short id, unsigned short environmentId,
		unsigned short instanceId, EntityType *type);

	/** Destructor removes Object from Physics Module.
	 */
	virtual ~ArticulatedPhysicsEntity();

	/** Adds a force at a passed position at the Physics Server.
	 * The method encodes the method call into a message and passes it to the
	 * PhysicsObjectManager which sends it to the Physics Server which
	 * simulates this Entity.
	 * @param force force to add
	 * @param position where to add the force
	 * @param relativeForce is the force oriented relative to the body
	 * @param relativePosition is the position relative to the body
	 */
	virtual void addForceAtPosition(gmtl::Vec3f& force, gmtl::Vec3f& position,
					bool relativeForce = false,	bool relativePosition = false);

//************************************************//
// PUBLIC METHODS INNHERITED FROM: PhysicsEntity: //
//************************************************//

	virtual void getRigidBodies(std::vector<oops::RigidBody*>& rigidBodyList);
	virtual void getJoints(std::vector<oops::Joint*>& jointList);
//	virtual bool addToSimulation(oops::Simulation* simulation);
//	virtual bool removeFromSimulation(oops::Simulation* simulation);
	virtual PhysicsObjectID getPhysicsObjectID();

//*****************************************//
// PUBLIC METHODS INNHERITED FROM: Entity: //
//*****************************************//

	virtual void setEnvironment(Environment* env);

//*****************//
// PUBLIC MEMBERS: //
//*****************//

	/// unique class-ID used for PhysicsObjectID
	static const unsigned ARTICULATEDPHYSICSENTITY_CLASSID;

//****************************//
// DEPRECATED PUBLIC METHODS: //
//****************************//

// 	virtual ArticulatedBody* getArticulatedBody();
//	virtual void setActive(bool active);
// 	virtual bool isActive();

protected:

	friend class ArticulatedPhysicsEntityController;
	friend class ArticulatedPhysicsEntityType;
	friend class CursorSpringConnector;

//********************//
// PROTECTED METHODS: //
//********************//

//	/** Adds the passed force at the passed position to the Physics Entity.
//	 * This method is the server part of the addForceAtPosition method. It is
//	 * only called on the machine which acts as Server for this PhysicsEntity.
//	 * @param force force to add
//	 * @param position where to add the force
//	 * @param relativeForce is the force oriented relative to the body
//	 * @param relativePosition is the position relative to the body
//	 */
//	virtual void addForceAtPosition_SERVER(gmtl::Vec3f& force,
//					gmtl::Vec3f& position, bool relativeForce,
//					bool relativePosition);

	/** Sets the ArticulatedBody.
	 * The method sets the ArticulatedBody to the PhysicsEntity. It is called
	 * by the ArticulatedPhysicsEntityType-class in the createInstance method.
	 * @param obj ArticulatedBody representing the ArticulatedPhysicsEntity
	 */
	virtual void setArticulatedBody(oops::ArticulatedBody* obj);

	/** Sets the initial Transformation of the ArticulatedBody.
	 * The method sets the TransformationPipe of the ArticulatedBody. It does
	 * so by taking the Transformation of the ArticulatedPhysicsEntity in world
	 * coordinates and multiplying the inverse offset transformation stored
	 * in the ArticulatedBody.
	 * @return true if the ArticulatedBody-Transformation could be set
	 */
	virtual bool initialize();

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
	virtual void setRigidBodyMapping(unsigned rigidBodyIndex, unsigned succeedingEntityIndex);

//***************************************************//
// PROTECTED METHODS INNHERITED FROM: PhysicsEntity: //
//***************************************************//

//	virtual bool handleMessage(unsigned function, NetMessage* msg);
//
//	virtual void handleSyncObjectTransformationMsg(NetMessage* msg);

//********************//
// PROTECTED MEMBERS: //
//********************//

//	/// Function calls which have to be distributed to server
//	enum REMOTE_FUNCTIONS { FUNCTION_UNKNOWN = 0,
//		FUNCTION_addForceAtPosition = 1};

	/// Indicating if the physical representation is already initialized
	bool isInitialized;
	/// Indicating if Entity is added to physics simulation
	bool addedToSimulation;
	/// Pointer to the physical representation of the Entity
	oops::ArticulatedBody* articulatedBody;
	/// Mapping from RigidBody-index to SimplePhysicsEntity-ID-offset
	std::map<unsigned, unsigned> rigidBodyMapping;

	/// ID of the PhysicsObject
	PhysicsObjectID physicsObjectID;
};
#endif // _ARTICULATEDPHYSICSENTITY_H
