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

#ifndef _PHYSICSOBJECTINTERFACE_H
#define _PHYSICSOBJECTINTERFACE_H

#include "PhysicsObjectID.h"
#include "PhysicsObjectInterface.h"

#ifdef INVRS_BUILD_TIME
#include "oops/Simulation.h"
#include "oops/RigidBody.h"
#else
#include <inVRs/Modules/3DPhysics/oops/Simulation.h>
#include <inVRs/Modules/3DPhysics/oops/RigidBody.h>
#endif
#include <inVRs/SystemCore/NetMessage.h>

class PhysicsObjectManager;

//using namespace oops;

/** Interface for all simulated objects of the Physics Module
 * This interface is used for the simulated objects of the Physics Module.
 * Every object which should be simulated has to implement this interface.
 */
class PhysicsObjectInterface
{
public:

//******************//
// PUBLIC METHODS: *//
//******************//

	/** Empty destructor.
	 */
	virtual ~PhysicsObjectInterface(){};

	/** Fills the passed list with all RigidBodies it contains. It is mainly
	 * used by the PhysicsObjectManager to add the RigidBodies to the simulation.
	 * @param rigidBodyList List which is filled with contained RigidBodies
	 */
	virtual void getRigidBodies(std::vector<oops::RigidBody*>& rigidBodyList) = 0;

	/** Fills the passed list with all Joints it contains. It is mainly
	 * used by the PhysicsObjectManager to add the Joints to the simulation.
	 * @param jointList List which is filled with contained Joints
	 */
	virtual void getJoints(std::vector<oops::Joint*>& jointList) = 0;
	
// DEPRECATED:	
//	/** Adds the PhysicsObject to the passed simulation instance.
//	 * @param simulation oops::Simulation class instance
//	 * @return true if the object could be added, false otherwise
//	 */
//	virtual bool addToSimulation(oops::Simulation* simulation) = 0;
//
//	/** Removes the PhysicsObject from the passed simulation instance.
//	 * @param simulation oops::Simulation class instance
//	 * @return true if the object could be removed, false otherwise
//	 */
//	virtual bool removeFromSimulation(oops::Simulation* simulation) = 0;

	/** Returns the ID of the PhysicsObject.
	 * The method has to return the unique PhysicsObjectID of the PhysicsObject.
	 * The programmer has to ensure that this PhysicsObjectID is set properly
	 * and that it is unique.
	 * @return ID of the PhysicsObject
	 */
	virtual PhysicsObjectID getPhysicsObjectID() = 0;

// DEPRECATED:
//	/** Transforms the incoming Message into a message call.
//	 * The method is called from the PhysicsObjectManager. It has to decode
//	 * the passed Message and call the method call encoded in it.
//	 * @param msg Message containing a method call
//	 * @return true if the encoded method could be called, false otherwise
//	 */
//	virtual bool handleMessage(NetMessage* msg) = 0;

	/** Sets the PhysicsObjectManager responsible for the object.
	 * The method has to store the PhysicsObjectManager in order to call the
	 * handleClientInput method if a change in the physical state of the
	 * PhysicsObject should take place (e.g. via a user input)
	 * @param objectManager PhysicsObjectManager responsible for the object
	 */
	virtual void setPhysicsObjectManager(PhysicsObjectManager* objectManager) = 0;

protected:

	friend class SynchronisationModel;

//*********************//
// PROTECTED METHODS: *//
//*********************//

//	/** DEPRECATED: Synchronises the whole physical state to normal clients.
//	 * @param msg Message where to add the synchronisation data
//	 */
//	virtual void synchroniseAllToClient(NetMessage* msg) = 0;
//
//	/** DEPRECATED: Synchronises the whole physical state to DeadReckoning clients.
//	 * @param msg Message where to add the synchronisation data
//	 */
//	virtual void synchroniseAllToDRClient(NetMessage* msg) = 0;
//
//	/** DEPRECATED: Synchronises the whole physical state to servers.
//	 * @param msg Message where to add the synchronisation data
//	 */
//	virtual void synchroniseAllToServer(NetMessage* msg) = 0;
//
//	/** DEPRECATED: Synchronises the changes of the physical state to normal clients.
//	 * @param msg Message where to add the synchronisation data
//	 */
//	virtual void synchroniseChangesToClient(NetMessage* msg) = 0;
//
//	/** DEPRECATED: Synchronises the changes of the physical state to DeadReckoning clients.
//	 * @param msg Message where to add the synchronisation data
//	 */
//	virtual void synchroniseChangesToDRClient(NetMessage* msg) = 0;
//
//	/** DEPRECATED: Synchronises the changes of the physical state to servers.
//	 * @param msg Message where to add the synchronisation data
//	 */
//	virtual void synchroniseChangesToServer(NetMessage* msg) = 0;
//
//	/** DEPRECATED: Encodes the transformation of the physics object to the message for
//	 * synchronisation.
//	 * @param msg Message where to add eht synchronisation data
//	 */
//	virtual void synchroniseObjectTransformation(NetMessage* msg) = 0;

// DEPRECATED: DONE VIA GETRIGIDBODIES
//	/** Adds all RigidBodies to the passed vector for synchronisation purposes 
//	 * @param rigidBodyList list where the RigidBodies should be stored to
//	 */
//	virtual std::vector<RigidBody*>& getRigidBodiesForSync(std::vector<RigidBody*> &rigidBodyList) = 0;
	
}; // PhysicsObject

#endif // _PHYSICSOBJECTINTERFACE_H
