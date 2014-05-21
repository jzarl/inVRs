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

#include "SingleServerPhysicsObjectManager.h"

#include <assert.h>
#include <OpenSG/OSGThreadManager.h>
#include <OpenSG/OSGThread.h>

#include "Physics.h"
#include "PhysicsMessageFunctions.h"

#include <inVRs/SystemCore/SystemCore.h>
#include <inVRs/SystemCore/UserDatabase/UserDatabase.h>

OSG_USING_NAMESPACE
using namespace oops;

//*****************//
// PUBLIC METHODS: //
//*****************//

SingleServerPhysicsObjectManager::SingleServerPhysicsObjectManager()
{
//	messageListLock = dynamic_cast<Lock*>(ThreadManager::the()->getLock("SingleServerPhysicsObjectManager::messageListLock"));
//	assert(messageListLock);
	server = NULL;
	network = (NetworkInterface*)SystemCore::getModuleByName("Network");
	assert(network);
	serverMode = SERVERMODE_UNDEFINED;
} // SingleServerPhysicsObjectManager

SingleServerPhysicsObjectManager::~SingleServerPhysicsObjectManager()
{
} // ~SingleServerPhysicsObjectManager

void SingleServerPhysicsObjectManager::setServerMode(std::string mode)
{
	if (!strcasecmp(mode.c_str(), "server"))
		this->serverMode = SERVERMODE_SERVER;
	else if (!strcasecmp(mode.c_str(), "client"))
		this->serverMode = SERVERMODE_CLIENT;
	else if (!strcasecmp(mode.c_str(), "drclient"))
		this->serverMode = SERVERMODE_DRCLIENT;
	else
		printd(ERROR, "SingleServerPhysicsObjectManager::setServerMode(): undefined servermode \"%s\"\n", mode.c_str());

	if (serverMode == SERVERMODE_SERVER)
		this->server = UserDatabase::getLocalUser();
} // setServerMode

//******************************************************//
// PUBLIC METHODS INHERITED FROM: PhysicsObjectManager: //
//******************************************************//

//void SingleServerPhysicsObjectManager::handleClientInput(NetMessage* msg, PhysicsObjectID dst)
//{
//	std::vector<NetworkIdentification> serverHosts;
//
//	msgFunctions::encodeFirst(dst, msg);
//	if (serverMode == SERVERMODE_SERVER)
//	{
//		messageListLock->aquire();
//			messageList.push_back(msg);
//		messageListLock->release();
//	} // if
//	else
//	{
//		network->sendMessageTCPTo(msg, PHYSICS_MODULE_ID, server->getId());
//	} // else
//} // handleClientInput

//void SingleServerPhysicsObjectManager::getLocalSimulatedObjects(std::vector<PhysicsObjectInterface*>& dst)
//{
//	int i;
//
//	if (serverMode == SERVERMODE_SERVER)
//	{
//		for (i=0; i < (int)objectList.size(); i++)
//			dst.push_back(objectList[i]);
//	} // if
//} // getLocalSimulatedObjects

//void SingleServerPhysicsObjectManager::getRemoteSimulatedObjects(std::vector<PhysicsObjectInterface*>& dst) {
//	
//	int i;
//	
//	if (serverMode != SERVERMODE_SERVER) {
//		for (i=0; i < (int)objectList.size(); i++)
//			dst.push_back(objectList[i]);
//	} // if
//} // getRemoteSimulatedObjects

bool SingleServerPhysicsObjectManager::isSimulatingRigidBody(OopsObjectID rigidBodyID) {
	if (serverMode == SERVERMODE_SERVER)
		return true;
	return false;
} // isSimulatingRigidBody

bool SingleServerPhysicsObjectManager::isSimulatingJoint(OopsObjectID jointID) {
	if (serverMode == SERVERMODE_SERVER)
		return true;
	return false;
} // isSimulatingJoint


//*********************************************************//
// PROTECTED METHODS INHERITED FROM: PhysicsObjectManager: //
//*********************************************************//

void SingleServerPhysicsObjectManager::getLocalSimulatedRigidBodies(std::vector<RigidBody*>& dst) {
	int i;
	
	if (serverMode == SERVERMODE_SERVER) {
		for (i=0; i < (int)rigidBodyList.size(); i++)
			dst.push_back(rigidBodyList[i]);
	} // if
} // getLocalSimulatedRigidBodies

void SingleServerPhysicsObjectManager::getLocalSimulatedJoints(std::vector<Joint*>& dst) {
	int i;
	
	if (serverMode == SERVERMODE_SERVER) {
		for (i=0; i < (int)jointList.size(); i++)
			dst.push_back(jointList[i]);
	} // if
} // getLocalSimulatedJoints

void SingleServerPhysicsObjectManager::getRemoteSimulatedRigidBodies(std::vector<RigidBody*>& dst) {
	int i;
	
	if (serverMode != SERVERMODE_SERVER) {
		for (i=0; i < (int)rigidBodyList.size(); i++)
			dst.push_back(rigidBodyList[i]);
	} // if
} // getRemoteSimulatedRigidBodies

void SingleServerPhysicsObjectManager::getRemoteSimulatedJoints(std::vector<Joint*>& dst) {
	int i;
	
	if (serverMode != SERVERMODE_SERVER) {
		for (i=0; i < (int)jointList.size(); i++)
			dst.push_back(jointList[i]);
	} // if
} // getRemoteSimulatedJoints


void SingleServerPhysicsObjectManager::init(Physics* physics)
{
	PhysicsObjectManager::init(physics);


/* DEPRECATED: DECISION IS TAKEN BY SYNCHRONISATIONMODEL!!!
	if (serverMode == SERVERMODE_DRCLIENT)
	{
		assert(physics);
		Simulation* sim = physics->getSimulation();
		assert(sim);
// 		sim->deactivateCollisionDetection();
	} // else if
*/
} // init

/* DONE IN SYNCHRONISATIONMODEL NOW!!!
void SingleServerPhysicsObjectManager::handleMessages()
{
	int i, j;
	NetMessage* msg;
	PhysicsObjectInterface* object = NULL;
	PhysicsObjectID destinationObject;

	int messageCount = network->sizeRecvList(PHYSICS_MODULE_ID);

	unsigned currentSimulationTime, simulationTime;

	currentSimulationTime = physics->getSimulationTime();

	messageListLock->aquire();

		for (i=0; i < messageCount; i++)
			messageList.push_back(network->pop(PHYSICS_MODULE_ID));


		for (i=0; i < (int)messageList.size(); i++)
		{
			msg = messageList[i];
			msgFunctions::decode(simulationTime, msg);
//			printd(INFO, "\nSingleServerPhysicsObjectManager::handleMessages(): SIMULATION TIME DT: %u\n\n", currentSimulationTime - simulationTime);
			while (!msg->finished())
			{
				msgFunctions::decode(destinationObject, msg);
				// TODO: implement better solution!
				for (j=0; j < (int)objectList.size(); j++)
				{
					if (objectList[j]->getPhysicsObjectID() == destinationObject)
						break;
				} // for
				if (j < (int)objectList.size())
					object = objectList[j];
				else
				{
					printd(ERROR, "SingleServerPhysicsObjectManager::handleMessages(): ERROR: unknown PhysicsObject found!\n");
					break;
				} // else
				if (!object->handleMessage(msg))
					printd(ERROR, "SingleServerPhysicsObjectManager::handleMessages(): ERROR: could not handle message!\n");
			} // while
			delete msg;
		} // for
		messageList.clear();
	messageListLock->release();
} // handleMessages
*/

void SingleServerPhysicsObjectManager::step(float dt)
{
	int i;
#if OSG_MAJOR_VERSION >= 2
	addObjectListLock->acquire();
#else
	addObjectListLock->aquire();
#endif
		for (i=0; i < (int)addObjectList.size(); i++)
		{
//			if (serverMode == SERVERMODE_SERVER || serverMode == SERVERMODE_DRCLIENT)
//			{
			if (!addPhysicsObjectInternal(addObjectList[i]))
				printd(ERROR, "PhysicsObjectManager::step(): could not add PhysicsObject to Simulation!\n");
//			} // if
			objectList.push_back(addObjectList[i]);
		} // for
		addObjectList.clear();
	addObjectListLock->release();
} // step

NetMessage* SingleServerPhysicsObjectManager::createSynchronisationMessage()
{
	unsigned serverID;
	NetMessage* msg = new NetMessage;

	if (server)
		serverID = server->getId();
	else
		serverID = 0xFFFFFFFF;

	printd("SingleServerPhysicsObjectManager::createSynchronisationMessage(): adding serverID %u!\n", serverID);

	msg->putUInt32(serverID);
	return msg;	
} // createSynchronisationMessage

void SingleServerPhysicsObjectManager::handleSynchronisationMessage(NetMessage* msg)
{
	unsigned serverID = msg->getUInt32();

	printd("SingleServerPhysicsObjectManager::handleSynchronisationMessage(): found serverID %u!\n", serverID);

	if (serverID != 0xFFFFFFFF)
		server = UserDatabase::getUserById(serverID);
	else
		printd(WARNING, "SingleServerPhysicsObjectManager::handleSynchronisationMessage(): no server synchronised!\n");

	if (serverID != 0xFFFFFFFF && !server)
		printd(ERROR, "SingleServerPhysicsObjectManager::handleSynchronisationMessage(): found server with ID %u but no according user!\n", serverID);
} // handleSynchronisationMessage

bool SingleServerPhysicsObjectManager::isServer() {
	return (serverMode == SERVERMODE_SERVER);
} // isServer

