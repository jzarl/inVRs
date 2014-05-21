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

#ifndef _SINGLESERVERPHYSICSOBJECTMANAGER_H
#define _SINGLESERVERPHYSICSOBJECTMANAGER_H

#include "PhysicsObjectManager.h"

#include <inVRs/SystemCore/UserDatabase/User.h>
#include <inVRs/SystemCore/ComponentInterfaces/NetworkInterface.h>

/** PhysicsObjectManager for a simulation where only one machine acts as
 * Physics server.
 */
class SingleServerPhysicsObjectManager : public PhysicsObjectManager
{
public:

//*****************//
// PUBLIC METHODS: //
//*****************//

	/** Constructor initializes lock.
	 */
	SingleServerPhysicsObjectManager();

	/** Empty Destructor.
	 */
	virtual ~SingleServerPhysicsObjectManager();

	/** Sets the server mode.
	 * The method is called by the SingleServerPhysicsObjectManagerFactory and
	 * defines if the local machine acts as Physics Server or not.
	 * @param server indicating if local machine is physics server
	 */
	virtual void setServerMode(std::string mode);

//******************************************************//
// PUBLIC METHODS INHERITED FROM: PhysicsObjectManager: //
//******************************************************//

//	virtual void handleClientInput(NetMessage* msg, PhysicsObjectID dst);
//	virtual void getLocalSimulatedObjects(std::vector<PhysicsObjectInterface*>& dst);
//	virtual void getRemoteSimulatedObjects(std::vector<PhysicsObjectInterface*>& dst);
	
	virtual bool isSimulatingRigidBody(OopsObjectID rigidBodyID);

	virtual bool isSimulatingJoint(OopsObjectID jointID);


protected:

//*********************************************************//
// PROTECTED METHODS INHERITED FROM: PhysicsObjectManager: //
//*********************************************************//

	virtual void getLocalSimulatedRigidBodies(std::vector<oops::RigidBody*>& dst);
	virtual void getLocalSimulatedJoints(std::vector<oops::Joint*>& dst);
	virtual void getRemoteSimulatedRigidBodies(std::vector<oops::RigidBody*>& dst);
	virtual void getRemoteSimulatedJoints(std::vector<oops::Joint*>& dst);

	virtual void init(Physics* physics);
//	virtual void handleMessages();
	virtual void step(float dt);
	virtual NetMessage* createSynchronisationMessage();
	virtual void handleSynchronisationMessage(NetMessage* msg);

	virtual bool isServer();
	
//********************//
// PROTECTED MEMBERS: //
//********************//

	enum SERVERMODE	{ 
		SERVERMODE_UNDEFINED,
		SERVERMODE_SERVER,
		SERVERMODE_DRCLIENT,
		SERVERMODE_CLIENT
	};

	/// Variable indicating if local User is server
	SERVERMODE serverMode;
	/// User who is physics server
	User* server;
	/// Pointer to the Network Module
	NetworkInterface* network;

	osg::Lock* messageListLock;
	/// List of incoming messages
	std::vector<NetMessage*> messageList;
}; // SingleServerPhysicsObjectManager

#endif // _SINGLESERVERPHYSICSOBJECTMANAGER_H
