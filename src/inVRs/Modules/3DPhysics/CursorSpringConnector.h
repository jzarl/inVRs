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

#ifndef _CURSORSPRINGCONNECTOR_H
#define _CURSORSPRINGCONNECTOR_H

#include <OpenSG/OSGThreadManager.h>
#include <OpenSG/OSGThread.h>
#include <OpenSG/OSGLock.h>

#include "PhysicsEntity.h"
#include "PhysicsObjectInterface.h"
#include "SystemThreadListenerInterface.h"
#include "SimulationStepListenerInterface.h"
#ifdef INVRS_BUILD_TIME
#include "oops/Joints.h"
#else
#include <inVRs/Modules/3DPhysics/oops/Joints.h>
#endif

#include <inVRs/SystemCore/UserDatabase/User.h>

/** Class to connect a PhysicsEntity to the User's Cursor.
 * This class is used for Natural Interaction when a PhysicsEntity should be
 * manipulated with the Users's cursor. It tries to update the position of the
 * Entity so that it matches to the one of the Cursor.
 * Available modes:
 * Velocity Mode: sets object-velocities directly
 *     - also called GOD-MODE: mass does not affect manipulation
 *     - very stable
 *     - problems with tires of corvette
 *
 * Spring Mode: simulates a spring between cursor and RigidBody
 *     - different manipulation depending of object mass
 *     - hard to configure to work with all objects properly
 *     - problems with objects which are connected to other objects via joints
 *     - good for use with simple RigidBodies
 *
 * Force Mode: calculates force and torque needed to reach velocities needed to
 *             fix transformation.
 *     - TODO
 */
class CursorSpringConnector : public PhysicsObjectInterface, public SystemThreadListenerInterface, public SimulationStepListenerInterface
{
public:

//*****************//
// PUBLIC METHODS: //
//*****************//

	/** Constructor.
	 * @param entity Entity which should be manipulated
	 * @param user owner of the cursor
	 */
	CursorSpringConnector(PhysicsEntity* entity, User* user);

	/** Empty Destructor.
	 */
	virtual ~CursorSpringConnector();

	static void setLinearConstants(float spring, float damping);

	static void setAngularConstants(float spring, float damping);

//*******************************************************//
// PUBLIC METHODS INHERITED FROM: PhysicsObjectInterface //
//*******************************************************//

	virtual void getRigidBodies(std::vector<oops::RigidBody*>& rigidBodyList);
	virtual void getJoints(std::vector<oops::Joint*>& jointList);
//	virtual bool addToSimulation(oops::Simulation* simulation);
//	virtual bool removeFromSimulation(oops::Simulation* simulation);
	virtual PhysicsObjectID getPhysicsObjectID();
	virtual bool handleMessage(NetMessage* msg);
	virtual void setPhysicsObjectManager(PhysicsObjectManager* objectManager);

//*****************//
// PUBLIC MEMBERS: //
//*****************//

	/// Possible modes for connection
	enum CONNECTION_MODE {
		VELOCITY_MODE,
		SPRING_MODE,
		FORCETORQUE_MODE,
		JOINT_MODE
	};
	/// ID for CursorSpringConnector-class
	static const unsigned CURSORSPRINGCONNECTOR_CLASSID;

protected:

//********************//
// PROTECTED METHODS: //
//********************//

	/** Executes the manipulation in Velocity Mode
	 * @param dt timestep since last call
	 * @param cursorTrans transformation of the Cursor
	 * @param bodyTrans transformation of the RigidBody
	 */
	virtual void executeVelocityMode(float dt, TransformationData cursorTrans, TransformationData bodyTrans);

	/** Executes the manipulation in Spring Mode
	 * @param dt timestep since last call
	 * @param cursorTrans transformation of the Cursor
	 * @param bodyTrans transformation of the RigidBody
	 */
	virtual void executeSpringMode(float dt, TransformationData cursorTrans, TransformationData bodyTrans);

	/** Executes the manipulation in Force Mode
	 * @param dt timestep since last call
	 * @param cursorTrans transformation of the Cursor
	 * @param bodyTrans transformation of the RigidBody
	 */
	virtual void executeForceTorqueMode(float dt, TransformationData cursorTrans, TransformationData bodyTrans);

	/** Executes the manipulation in Joint Mode
	 * @param dt timestep since last call
	 * @param cursorTrans transformation of the Cursor
	 * @param bodyTrans transformation of the RigidBody
	 */
	virtual void executeJointMode(float dt, TransformationData cursorTrans, TransformationData bodyTrans);

	virtual bool initialize();

//**********************************************************//
// PROTECTED METHODS INHERITED FROM: PhysicsObjectInterface //
//**********************************************************//

//	virtual void synchroniseAllToClient(NetMessage* msg);
//	virtual void synchroniseAllToDRClient(NetMessage* msg);
//	virtual void synchroniseAllToServer(NetMessage* msg);
//	virtual void synchroniseChangesToClient(NetMessage* msg);
//	virtual void synchroniseChangesToDRClient(NetMessage* msg);
//	virtual void synchroniseChangesToServer(NetMessage* msg);
//
//	virtual void synchroniseObjectTransformation(NetMessage* msg);
//	virtual void handleSyncObjectTransformationMsg(NetMessage* msg);
//
//	virtual std::vector<RigidBody*>& getRigidBodiesForSync(std::vector<RigidBody*> &rigidBodyList);

//*****************************************************************//
// PROTECTED METHODS INHERITED FROM: SystemThreadListenerInterface //
//*****************************************************************//

	virtual void systemUpdate(float dt);

//*******************************************************************//
// PROTECTED METHODS INHERITED FROM: SimulationStepListenerInterface //
//*******************************************************************//

	virtual void step(float dt, unsigned simulationTime);

//********************//
// PROTECTED MEMBERS: //
//********************//
	/// Spring constants
	static float linearSpringConstant, angularSpringConstant;
	/// Damping constants
	static float linearDampingConstant, angularDampingConstant;
	/// Thresholds where connection will break
	static float linearThreshold, angularThreshold;

	/// Indicating if the CursorSpringConnector is already initialized
	bool isInitialized;
	/// ID of the CursorSpringConnector
	PhysicsObjectID physicsObjectID;
	/// mode which is used for connection
	CONNECTION_MODE connectionMode;

	/// Pointer to the used PhysicsObjectManager
	PhysicsObjectManager* objectManager;
	/// Owner of the Cursor used for manipulation
	User* user;
	/// PhysicsEntity which should be manipulated
	PhysicsEntity* entity;
	/// RigidBody of PhysicsEntity
	oops::RigidBody* rigidBody;
	/// ODE-RigidBody for JointMode
	oops::RigidBody* cursorBody;
//	dBodyID cursorBody;
//	/// ODE-Joint for JointMode
//	dJointID connectionJoint;
	oops::SpringDamperJoint* connectionJoint;

	/// Offset from Cursor to RigidBody transformation
	TransformationData cursorToBodyOffset;
	/// Offset from RigidBody to Cursor transformation
	TransformationData bodyToCursorOffset;
	/// Cursor transformation from last simulation step
	TransformationData oldCursorTransformation;
	/// Current Cursor transformation
	TransformationData cursorTransformation;
	/// Lock for updating Cursor transformation
#if OSG_MAJOR_VERSION >= 2
	OSG::LockRefPtr cursorTransformationLock;
#else
	OSG::Lock* cursorTransformationLock;
#endif

	uint64_t rigidBodyId;

}; // CursorSpringConnector

#endif // _CURSORSPRINGCONNECTOR_H
