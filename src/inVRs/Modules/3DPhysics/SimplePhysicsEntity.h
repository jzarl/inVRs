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

#ifndef _SIMPLEPHYSICSENTITY_H
#define _SIMPLEPHYSICSENTITY_H

#include "PhysicsEntity.h"
#ifdef INVRS_BUILD_TIME
#include "oops/RigidBody.h"
#else
#include <inVRs/Modules/3DPhysics/oops/RigidBody.h>
#endif

//using namespace oops;

/** Entity with single RigidBody as physical representation.
 * This class can be used for Entities which can be represented by one single
 * RigidBody.
 */
class SimplePhysicsEntity : public PhysicsEntity
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
	SimplePhysicsEntity(unsigned short id, unsigned short environmentId,
		unsigned short instanceId, EntityType *type);

	/** Destructor removes Object from Physics Module.
	 */
	virtual ~SimplePhysicsEntity();

	/** Adds a force to the RigidBody.
	 * The method just calls the addForce-method on the contained RigidBody.
	 * The distribution from this input to other hosts is done by the
	 * SynchronisationModel which has a Listener on such method calls.
	 * @param force Force that should be added
	 * @param relative indicating if force is relative to RigidBody
	 */
	virtual void addForce(gmtl::Vec3f& force, bool relative = false);

	/** Returns the RigidBody used by the Entity
	 * @return RigidBody of the Entity
	 */
	virtual oops::RigidBody* getRigidBody();

//************************************************//
// PUBLIC METHODS INNHERITED FROM: PhysicsEntity: //
//************************************************//

	virtual void getRigidBodies(std::vector<oops::RigidBody*>& rigidBodyList);
	virtual void getJoints(std::vector<oops::Joint*>& jointList);
	virtual PhysicsObjectID getPhysicsObjectID();

//*****************************************//
// PUBLIC METHODS INNHERITED FROM: Entity: //
//*****************************************//

	virtual void setEnvironment(Environment* env);

	virtual void setWorldTransformation(TransformationData trans);

//*****************//
// PUBLIC MEMBERS: //
//*****************//

	static const unsigned SIMPLEPHYSICSENTITY_CLASSID;

//****************************//
// DEPRECATED PUBLIC METHODS: //
//****************************//

// 	DECLARE_SERVER_METHOD2(addForce, gmtl::Vec3f&, bool)(gmtl::Vec3f& force,
// 		bool relative = false);
// 	DECLARE_SERVER_METHOD2(addTorque, gmtl::Vec3f&, bool)(gmtl::Vec3f& torque,
// 		bool relative = false);
// 	DECLARE_SERVER_METHOD2(setLinearVelocity, gmtl::Vec3f&, bool)(
// 		gmtl::Vec3f& velocity, bool relative = false);
// 	DECLARE_SERVER_METHOD2(setAngularVelocity, gmtl::Vec3f&, bool)(
// 		gmtl::Vec3f& angularVelocity, bool relative = false);
// 	DECLARE_SERVER_METHOD4(addForceAtPosition, gmtl::Vec3f&, gmtl::Vec3f&,
// 		bool, bool)(gmtl::Vec3f& force, gmtl::Vec3f& position,
// 		bool relativeForce = false, bool relativePosition = false);

// 	virtual void setActive(bool active);
// 	virtual bool isActive();

// 	virtual void update();

protected:

	friend class SimplePhysicsEntityController;
	friend class SimplePhysicsEntityType;
	friend class ArticulatedPhysicsEntity;
	friend class CursorSpringConnector;

//********************//
// PROTECTED METHODS: //
//********************//

	/** Sets the RigidBody.
	 * The method sets the RigidBody to the PhysicsEntity. It is called by the
	 * SimplePhysicsEntityType-class in the createInstance method.
	 * @param obj RigidBody representing the SimplePhysicsEntity
	 * @param useBodyTransformation define if RigidBody transformation should be
	 * 								applied to Entity
	 */
	virtual void setRigidBody(oops::RigidBody* obj, bool useBodyTransformation = false);

	/** Sets the initial Transformation of the RigidBody.
	 * The method sets the Transformation of the RigidBody. It does so by
	 * taking the Transformation of the SimplePhysicsEntity in world
	 * coordinates and multiplying the inverse offset transformation stored
	 * in the RigidBody.
	 * @return true if the RigidBody-Transformation could be set
	 */
	virtual bool calculatePhysicsTransformation();

//***************************************************//
// PROTECTED METHODS INNHERITED FROM: PhysicsEntity: //
//***************************************************//

//	virtual bool handleMessage(unsigned function, NetMessage* msg);
//	virtual void synchroniseAllToDRClient(NetMessage* msg);
//	virtual void handleSyncAllToDRClientMsg(NetMessage* msg);
//
//	virtual void synchroniseObjectTransformation(NetMessage* msg);
//	virtual void handleSyncObjectTransformationMsg(NetMessage* msg);
//
//	virtual std::vector<RigidBody*>& getRigidBodiesForSync(std::vector<RigidBody*> &rigidBodyList);

//*******************************//
// DEPRECATED PROTECTED METHODS: //
//*******************************//

//	/** Adds force to RigidBody on server side.
//	 * This method is the server side of the addForce method. It adds the force
//	 * to the RigidBody.
//	 * @param force force to add to RigidBody
//	 * @param relative indicates if force is relative to RigidBody
//	 */
//	virtual void addForce_SERVER(gmtl::Vec3f& force, bool relative);

//********************//
// PROTECTED MEMBERS: //
//********************//

	/// Function calls which have to be distributed to server
//	enum REMOTE_FUNCTIONS { FUNCTION_UNKNOWN = 0, FUNCTION_addForce = 1};
// 	enum SERVER_FUNCTIONS { FUNCTION_UNKNOWN, FUNCTION_addForce,
// 		FUNCTION_addTorque, FUNCTION_setLinearVelocity,
// 		FUNCTION_setAngularVelocity, FUNCTION_addForceAtPosition};

	/// indicating if the object is already added to the simulation
	bool addedToSimulation;
	/// Indicating if the RigidBody is already initialized with the Entity-transformation
	bool rigidBodyInitialized;
	/// Pointer to the physical representation of the Entity
	oops::RigidBody* rigidBody;
	/// ID of the PhysicsObject
	PhysicsObjectID physicsObjectID;
};

#endif // _SIMPLEPHYSICSENTITY_H
