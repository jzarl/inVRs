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

#ifndef _PHYSICSOBJECTMANAGER_H
#define _PHYSICSOBJECTMANAGER_H

// for OSG_MAJOR_VERSION:
#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGLock.h>
#include "SimulationStepListenerInterface.h"
#include "PhysicsObjectInterface.h"
#include "OopsObjectID.h"
#ifdef INVRS_BUILD_TIME
#include "oops/Simulation.h"
#else
#include <inVRs/Modules/3DPhysics/oops/Simulation.h>
#endif

class Physics;

//using namespace oops;

/** Manager class for simulated objects in Physics Module.
 * This class is used as a template for Manager classes which handle the
 * PhysicsObjects used in the Physics Module. The class therefore has to store
 * all objects which are relevant for the local User. It also has the function
 * to add or remove objects from the simulation. Better Management classes can
 * also allow to migrate objects to other servers.
 */
class PhysicsObjectManager
{
public:

//******************//
// PUBLIC METHODS: *//
//******************//

	/** Constructor initializes members.
	 * The constructor initializes the members to NULL. The real initialization
	 * is done in the <code>init</code> method.
	 */
	PhysicsObjectManager();

	/** Empty destructor.
	 * The destructor has no work yet.
	 */
	virtual ~PhysicsObjectManager();

//	/** Handles the method call to a PhysicsObject.
//	 * The method is called by the PhysicsObjects when a client input occurs.
//	 * A client input is a method call on a PhysicsObject which has to be
//	 * distributed to the responsible server(s). The PhysicsObject encodes its
//	 * method call by itself and passes it to this method.
//	 * @param msg Encoded method call
//	 * @param dst ID of the PhysicsObject which was called
//	 */
//	virtual void handleClientInput(NetMessage* msg, PhysicsObjectID dst) = 0;

	/** DEPRECATED: WILL BE REMOVED SOON!!! 
	 * Creates a list of all PhysicsObjects which are locally simulated.
	 * The method adds all PhysicsObjects for which the current machine is the
	 * server to the passed vector.
	 * @param dst List where to add all locally simulated objects.
	 */
	virtual void getLocalSimulatedObjects(std::vector<PhysicsObjectInterface*>& dst);

	/** DEPRECATED: WILL BE REMOVED SOON!!! 
	 * Creates a list of all PhysicsObjects which are NOT simulated locally.
	 * The method adds all PhysicsObjects for which any remote machine is the
	 * server to the passed vector.
	 * @param dst List where to add all remote simulated objects.
	 */
	virtual void getRemoteSimulatedObjects(std::vector<PhysicsObjectInterface*>& dst);
	
	/** Returns the PhysicsObject with the passed ID
	 * 
	 */
	virtual PhysicsObjectInterface* getObjectById(PhysicsObjectID id);
	
	virtual bool isSimulatingRigidBody(OopsObjectID rigidBodyID) = 0;

	virtual bool isSimulatingJoint(OopsObjectID jointID) = 0;

protected:

	friend class Physics;
	friend class SynchronisationModel;
	friend class SynchronisePhysicsEvent;

//*********************//
// PROTECTED METHODS: *//
//*********************//

	virtual void getLocalSimulatedRigidBodies(std::vector<oops::RigidBody*>& dst) = 0;
	
	virtual void getLocalSimulatedJoints(std::vector<oops::Joint*>& dst) = 0;

	virtual void getRemoteSimulatedRigidBodies(std::vector<oops::RigidBody*>& dst) = 0;
	
	virtual void getRemoteSimulatedJoints(std::vector<oops::Joint*>& dst) = 0;

	virtual oops::RigidBody* getRigidBodyById(OopsObjectID id);

	virtual oops::Joint* getJointById(OopsObjectID id);

	/** Initializes the PhysicsObjectManager.
	 * The method is called from the Physics Module and initializes the pointer
	 * to the Module. This is needed to get access to the oops::Simulation
	 * class or to the current simulation time.
	 * @param physics Pointer to the Physics Module
	 */
	virtual void init(Physics* physics);

	/** Returns the Simulation class instance from the Physics Module.
	 * The method returns the Simulation class instance used by the
	 * Physics Module.
	 * @return Pointer to the used Simulation instance
	 */
	virtual oops::Simulation* getSimulation();

	/** Returns the current simulation time.
	 * The method returns the current simulation time from the Physics Module.
	 * @return current simulation time
	 */
	virtual unsigned int getSimulationTime();

	/** Adds a PhysicsObject to the Simulation.
	 * The method adds the passed object to a list of objects which should be
	 * added to the Simulation. The entries of this list are then added to the
	 * Simulation before the next simulation step occurs.
	 * @param object Object to add to simulation.
	 * @return true if the object could be added, false otherwise
	 */
	virtual bool addPhysicsObject(PhysicsObjectInterface* object);
	
	/** Adds a PhysicsObject to the Simulation.
	 * The method adds the passed object to the simulation and to the list of
	 * simulated objects.
	 * @param object Object to add to simulation.
	 * @return true if the object could be added, false otherwise
	 */
	virtual bool addPhysicsObjectInternal(PhysicsObjectInterface* object);

	/** Removes a PhysicsObject from the Simulation.
	 * The method removes the passed object from the list of simulated objects.
	 * @param object Object to remove from simulation.
	 * @return true if the object could be removed, false otherwise
	 */
	virtual bool removePhysicsObject(PhysicsObjectInterface* object);

	/** Removes a PhysicsObject from the Simulation.
	 * The method removes the object with the passed ID from the list of
	 * simulated objects.
	 * @param physicsObjectID ID of the object to remove
	 * @return object which was removed, NULL if not found
	 */
	virtual PhysicsObjectInterface* removePhysicsObject(PhysicsObjectID physicsObjectID);

// DEPRECATED: NOW DONE IN SYNCHRONISATIONMODEL
	/** Handles the incoming network messages.
	 * The method is called before each simulation step. It should receive all
	 * incoming messages from the Network Module and execute them.
	 */
//	virtual void handleMessages() = 0;

	/** Handles a simulation step.
	 * The method is called after the <code>handleMessages</code> method and
	 * before the <code>Physics::handleSimulationStepListener</code> method.
	 * It is used to add PhysicsObjects to the Simulation. The added objects
	 * are then already simulated in this step the first time. The method is
	 * equal to the one of a SimulationStepListenerInterface but it has to
	 * to called before the <code>Physics::handleSimulationStepListener</code>
	 * method is executed to allow PhysicsObjects which implement this
	 * interface to be notified in the same simulation step the first time.
	 * @param dt Timestep of the current simulation step
	 */
	virtual void step(float dt) = 0;

	/** Creates a SynchronisationMessage for the current PhysicsObjectManager.
	 * The method returns a message containing information for the
	 * initial synchronisation of the PhysicsObjectManager. It is called from
	 * the Physics-Module in the <code>Physics::createSyncEvent</code> method.
	 * The returned message is then appended to the PhysicsSynchronisationEvent
	 * and sent to the new connected User.
	 * @return Synchronisation message for new User.
	 */
	virtual NetMessage* createSynchronisationMessage() = 0;

	/** Handles a received SynchronisationMessage.
	 * The method is called by the Physics-Module when a
	 * PhysicsSynchronisationEvent is received. The passed method contains
	 * information used to synchronise the current PhysicsObjectManager. The
	 * message is encoded by the <code>createSynchronisationMessage</code>
	 * method.
	 * @param msg Synchronisation message for PhysicsObjectManager
	 */
	virtual void handleSynchronisationMessage(NetMessage* msg) = 0;

	/** Returns if the local machine has to do physics calculations.
	 */
	virtual bool isServer() = 0;
	
//*********************//
// PROTECTED MEMBERS: *//
//*********************//
	
	/// Pointer to the physics Module
	Physics* physics;
	/// Lock for list of objects to add
#if OSG_MAJOR_VERSION >= 2
	osg::LockRefPtr addObjectListLock;
#else
	osg::Lock* addObjectListLock;
#endif
	/// List of all objects in the simulation
	std::vector<PhysicsObjectInterface*> objectList;
	std::vector<oops::RigidBody*> rigidBodyList;
	std::vector<oops::Joint*> jointList;
	/// List of objects to add
	std::vector<PhysicsObjectInterface*> addObjectList;
	
}; // PhysicsObjectManager

#endif // _PHYSICSOBJECTMANAGER_H
