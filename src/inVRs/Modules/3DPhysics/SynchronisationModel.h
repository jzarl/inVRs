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

#ifndef _SYNCHRONISATIONMODEL_H
#define _SYNCHRONISATIONMODEL_H

#include "PhysicsObjectInterface.h"
#include "UserData.h"
#include "OopsObjectID.h"

#include <OpenSG/OSGThreadManager.h>
#include <OpenSG/OSGThread.h>

#include <inVRs/SystemCore/ComponentInterfaces/NetworkInterface.h>

class PhysicsObjectManager;
class Physics;
/** Class capable of synchronising server data with other servers or clients
 * This class is used to synchronise data from a server to the connected
 * clients and to other servers. The class therefore will be called from the
 * Physics Module before a simulation step takes place and after a simulation
 * step occured. The class can then decide whether to send data to other users
 * or not.
 */
class SynchronisationModel
{
public:

//******************//
// PUBLIC METHODS: *//
//******************//

	/** Constructor zeroes objectManager-pointer.
	 */
	SynchronisationModel();

	/** Empty destructor.
	 */
	virtual ~SynchronisationModel();

protected:

	friend class Physics;

//*********************//
// PROTECTED METHODS: *//
//*********************//

	/** Synchronises data before a simulation step.
	 * The method is called from the Physics Module before a simulation step
	 * takes place.
	 */
	virtual void synchroniseBeforeStep() = 0;

	/** Synchronises data after a simulation step.
	 * The method is called from the Physics Module after a simulation step
	 * takes place.
	 */
	virtual void synchroniseAfterStep() = 0;

	/** Stores a pointer to the PhysicsObjectManager.
	 * The method is called in the Physics::loadConfig method and stores a local
	 * pointer to the used PhysicsObjectManager.
	 * @param objectManager PhysicsObjectManager from the Physics Module.
	 */
	virtual void setPhysicsObjectManager(PhysicsObjectManager* objectManager);

	/** Handles the incoming network messages.
	 * The method is called before each simulation step. It should receive all
	 * incoming messages from the Network Module and execute them.
	 */
	virtual void handleMessages();

	/** Handles the synchronisation message
	 */
	virtual void handleSyncMessage(NetMessage* msg) = 0;
	
	/** Handles the client input messages
	 */
	virtual void handleClientInputMessage(NetMessage* msg) = 0;

//	/** DEPRECATED: Helper method for derived SynchronisationModels to access protected
//	 * methods from PhysicsObjects.
//	 */
//	void synchroniseAllToClient(PhysicsObjectInterface* obj, NetMessage* msg);
//
//	/** DEPRECATED: Helper method for derived SynchronisationModels to access protected
//	 * methods from PhysicsObjects.
//	 */
//	void synchroniseAllToDRClient(PhysicsObjectInterface* obj, NetMessage* msg);
//
//	/** DEPRECATED: Helper method for derived SynchronisationModels to access protected
//	 * methods from PhysicsObjects.
//	 */
//	void synchroniseAllToServer(PhysicsObjectInterface* obj, NetMessage* msg);
//
//	/** DEPRECATED: Helper method for derived SynchronisationModels to access protected
//	 * methods from PhysicsObjects.
//	 */
//	void synchroniseChangesToClient(PhysicsObjectInterface* obj, NetMessage* msg);
//
//	/** DEPRECATED: Helper method for derived SynchronisationModels to access protected
//	 * methods from PhysicsObjects.
//	 */
//	void synchroniseChangesToDRClient(PhysicsObjectInterface* obj, NetMessage* msg);
//
//	/** DEPRECATED: Helper method for derived SynchronisationModels to access protected
//	 * methods from PhysicsObjects.
//	 */
//	void synchroniseChangesToServer(PhysicsObjectInterface* obj, NetMessage* msg);
//
//	/** Helper method for derived SynchronisationModels to access protected
//	 * methods from PhysicsObjects.
//	 */
//	void synchroniseObjectTransformation(PhysicsObjectInterface* obj, NetMessage* msg);
//
//	/** Helper method for derived SynchronisationModels to access protected
//	 * methods from PhysicsObjects.
//	 */
//	void getRigidBodiesForSync(PhysicsObjectInterface* obj, std::vector<RigidBody*>& rigidBodyList);
	
//	/** Stores all connected users in the passes vector.
//	 * The method copies the vector of connected users from the Physics module
//	 * to the passed vector.
//	 * @param dst vector where the connected userIds should be stored in
//	 */
//	void getConnectedUsers(std::vector<unsigned>& dst);
//
//	/** Returns the UserData of the user with the passed Id.
//	 * @param userId Id of the user
//	 * @return UserData of the user with the passed Id
//	 */
//	UserData* getUserData(unsigned userId);
	
	/** Returns if the current SynchronisationModel needs physics calculations
	 * or not.
	 * @return true if the Physics-engine should do calculations
	 */
	virtual bool needPhysicsCalculation();

	/** Helper-method to access the method in the PhysicsObjectManager
	 */ 
	void getLocalSimulatedRigidBodies(PhysicsObjectManager* manager, std::vector<oops::RigidBody*>& dst);

	/** Helper-method to access the method in the PhysicsObjectManager
	 */ 
	void getRemoteSimulatedRigidBodies(PhysicsObjectManager* manager, std::vector<oops::RigidBody*>& dst);

	/** Helper-method to access the method in the PhysicsObjectManager
	 */ 
	void getLocalSimulatedJoints(PhysicsObjectManager* manager, std::vector<oops::Joint*>& dst);

	/** Helper-method to access the method in the PhysicsObjectManager
	 */ 
	void getRemoteSimulatedJoints(PhysicsObjectManager* manager, std::vector<oops::Joint*>& dst);

	/** Helper-method to access the method in the PhysicsObjectManager
	 */ 
	oops::RigidBody* getRigidBodyById(PhysicsObjectManager* manager, OopsObjectID id);
	
	/** Helper-method to access the method in the PhysicsObjectManager
	 */ 
	oops::Joint* getJointById(PhysicsObjectManager* manager, OopsObjectID id);

	/** Helper-method to set the simulationTime of the Physics module
	 */
	void setSimulationTime(unsigned simulationTime);
	
//*********************//
// PROTECTED MEMBERS: *//
//*********************//

	/// Pointer to Physics
	Physics* physics;
	
	/// Pointer to the used PhysicsObjectManager
	PhysicsObjectManager* objectManager;

	/// Pointer to the Network module
	NetworkInterface* network;

//	/// Lock for access to Network messages
//	OSG::Lock* messageListLock;
	/// List of incoming messages
	std::vector<NetMessage*> messageList;
	
}; // SynchronisationModel

#endif // _SYNCHRONISATIONMODEL_H
