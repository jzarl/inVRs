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

#include "ServerThread.h"

#include <assert.h>

#include <OpenSG/OSGThreadManager.h>
#include <OpenSG/OSGThread.h>

#include <inVRs/SystemCore/DebugOutput.h>
#include <inVRs/SystemCore/Platform.h>
#include <inVRs/SystemCore/UserDatabase/UserDatabase.h>
#include "Network.h"

OSG_USING_NAMESPACE

bool ServerThread::init(int serverport) {
	int retries = 0;
	port = serverport;
	bool failed = true;

	// repeat initialisation multiple times if it failed (socket already in use)
	while (failed && retries < 5) {
		try {
			// open socket for incoming connections
			client.open();
			//			client.setReusePort(true);
			client.bind(SocketAddress(SocketAddress::ANY, port));
			printd(INFO, "ServerThread::init(): listening...\n");
			client.listen();
			failed = false;
		} // try
		catch (SocketException &e) {
			printd(ERROR, "ServerThread::init(): network error: %s! Retrying!!!\n", e.what());
			retries++;
			client.close();
			if (retries < 5)
				sleep(1);
		} // catch
		catch (...) {
			printd(ERROR, "ServerThread::init(): unknown error thrown! Retrying!!!\n");
			retries++;
			client.close();
			if (retries < 5)
				sleep(1);
		}
		// catch
	} // while
	if (failed)
		return false;

	return true;
} // init

void ServerThread::run(void* dummy) {
	UInt32 tag;
	UserNetworkIdentification otherID;
	StreamSocket * socket;

	printd(INFO, "ServerThread::run(): waiting for client...\n");
	while (!shutdown) {
		try {
			socket = NULL;
			//ExtendedBinaryMessage message;

			// wait for incomming messages
			if (!client.waitReadable(5.0))
				continue;

			// open new socket for incomming communication
			socket = new StreamSocket(client.accept());
			printd(INFO, "ServerThread::run(): Client connected!\n");

			// update the local IP-address if not set yet
			if (internalNetwork->myId.address.ipAddress == 0x7F000001)
				internalNetwork->setLocalIPAddress(socket);

			printd(INFO, "ServerThread::run(): calling handShake!\n");

			// try to do handShake with other side
			if (!handShake(socket, &otherID, tag)) {
				printd(ERROR, "ServerThread::run(): handShake FAILED!\n");
				socket->close();
				continue;
			} // if


			internalNetwork->addConnectionToNetwork(socket, otherID);

			//			if (tag != internalNetwork->quickConnectTag)
			//			{
			//				printd(INFO, "ServerThread::run(): creating new SocketListEntry!\n");
			//				SocketListEntry* entry = new SocketListEntry();
			//				entry->socketTCP = socket;
			//				entry->id = otherID;
			//				entry->nextMsg = NULL;
			//				entry->prioritizedMsg = NULL;
			//
			//				internalNetwork->socketListLock->aquire();
			//					internalNetwork->socketList.push_back(entry);
			//				internalNetwork->socketListLock->release();
			//			} // if
		} catch (SocketException &e) {
			printd(ERROR, "ServerThread::run(): network error: %s\n", e.what());
			sleep(1);
		}
	}
	client.close();

#if OSG_MAJOR_VERSION >= 2
	// Enter Barrier
	BarrierRefPtr cleanupBarrier = OSG::dynamic_pointer_cast<OSG::Barrier> (ThreadManager::the()->getBarrier(
				"cleanupBarrier",false));		
#else //OpenSG1:
	// Enter Barrier
	Barrier* cleanupBarrier = dynamic_cast<Barrier*> (ThreadManager::the()->getBarrier(
				"cleanupBarrier"));
#endif
	printd(INFO, "ServerThread::run(): entering cleanupBarrier with counter 3!\n");
	cleanupBarrier->enter(3);

	printd(INFO, "ServerThread::run(): goodby!\n");
}

void ServerThread::kill() {
	shutdown = true;
} // kill

bool ServerThread::handShake(StreamSocket* socket, UserNetworkIdentification* otherID, UInt32& tag) {

	NetMessage message;
	bool success;
	UInt32 tempTag;

	printd(INFO, "ServerThread::handShake(): receiving ID of client!\n");

	// ID exchange
	try {
		Network::receiveNetMessage(socket, &message);
		message.getUInt32(tempTag);
		tag = tempTag;
		internalNetwork->decodeId(otherID, &message);
	} // try
	catch (...) {
		printd(ERROR, "ServerThread::handShake(): error at receiving client's ID!\n");
		return false;
	} // catch

	printd(INFO, "ServerThread::handShake(): client has ip %s and userId %u\n",
			internalNetwork->ipAddressToString(otherID->netId.address.ipAddress).c_str(),
			otherID->userId);

	// Check if client is already connected, send connectionDenied-Message if so
	if (internalNetwork->checkClientConnected(&otherID->netId)) {
		printd(WARNING, "ServerThread::handShake(): Client already connected!\n");
		message.clear();
		message.putUInt32(internalNetwork->connectionRequestDenyTag);
		try {
			Network::sendNetMessage(socket, &message);
		} catch (...) {
		}
		return false;
	} // if

	if (tag != internalNetwork->quickConnectTag)
		success = handleNormalConnect(socket, message);
	else
		success = handleQuickConnect(socket, message, otherID);

	if (!success)
		return false;

	//	internalNetwork->mapUserToNetworkId[otherID->userId] = otherID->netId;
	return true;
} // handShake

bool ServerThread::handleNormalConnect(StreamSocket* socket, NetMessage& message) {

	UInt32 localTag;
	UserNetworkIdentification myUserNetId;

	printd(INFO, "ServerThread::handleNormalConnect(): check, if connection is allowed!\n");

	// Check, if client is allowed to connect, send connectionDenied-Message if not
	if (!internalNetwork->checkConnection()) {
		printd(INFO, "ServerThread::handleNormalConnect(): Connection NOT allowed!\n");
		message.putUInt32(internalNetwork->connectionRequestDenyTag);
		try {
			Network::sendNetMessage(socket, &message);
			//socket->send(message);
		} catch (...) {
		}
		return false;
	} // if

	printd(INFO, "ServerThread::handleNormalConnect(): connection IS allowed!\n");

	printd(INFO, "ServerThread::handleNormalConnect(): sending connectionRequestOkTag to client!\n");

	// Send connectionOK-Message to client and attach list of all known connected clients to it
	message.clear();
	message.putUInt32(internalNetwork->connectionRequestOkTag);
	myUserNetId.netId = internalNetwork->myId;
	myUserNetId.userId = UserDatabase::getLocalUserId();
	internalNetwork->encodeId(&message, &myUserNetId);
	internalNetwork->addClientsToMessage(&message);

	try {
		Network::sendNetMessage(socket, &message);
		printd(INFO, "ServerThread::handleNormalConnect(): message sent!\n");
		printd(INFO,
				"ServerThread::handleNormalConnect(): waiting for client until it's done with it's quickConnect!\n");

		message.clear();
		Network::receiveNetMessage(socket, &message);
	} // try
	catch (...) {
		printd(ERROR,
				"ServerThread::handleNormalConnect(): Catched an Exception, closing connection!\n");
		internalNetwork->connectionDone();
		return false;
	} // catch

	message.getUInt32(localTag);
	if (localTag == internalNetwork->quickConnectFailedTag)
		printd(INFO, "ServerThread::handleNormalConnect(): quickConnect at client FAILED!\n");
	else if (localTag == internalNetwork->quickConnectOkTag)
		printd(INFO, "ServerThread::handleNormalConnect(): quickConnect at client WORKED!\n");
	else {
		printd(
				ERROR,
				"ServerThread::handleNormalConnect(): internal error: unknown message tag received! Connection failed!\n");
		return false;
	} // else

	printd(INFO, "ServerThread::handleNormalConnect(): calling connectionDone!\n");
	internalNetwork->connectionDone();
	return true;
} // handleNormalConnect

bool ServerThread::handleQuickConnect(StreamSocket* socket, NetMessage& message,
		UserNetworkIdentification* otherID) {
	printd(INFO,
			"ServerThread::handleQuickConnect(): creating new SocketListEntry for newSocketListEntry!\n");

	assert(internalNetwork->newSocketListEntry == NULL);

//	 TODO: problem is that newSocketListEntry is never added to Network because it has not step method!!!
//	 * The solution presented below is NOT THREADSAFE and may fail!!!!!!
//	 internalNetwork->socketListLock->aquire();
//	 while (internalNetwork->newSocketListEntry != NULL) {
//	 internalNetwork->socketListLock->release();
//	 printd(WARNING, "ServerThread::handShake(): waiting for main thread to allow network connection!\n");
//	 sleep(1);
//	 internalNetwork->socketListLock->aquire();
//	 } // while
//	 internalNetwork->newSocketListEntry = new SocketListEntry();
//	 internalNetwork->newSocketListEntry->socketTCP = socket;
//	 internalNetwork->newSocketListEntry->id = *otherID;
//	 internalNetwork->newSocketListEntry->nextMsg = NULL;
//	 internalNetwork->newSocketListEntry->prioritizedMsg = NULL;
//	 internalNetwork->socketListLock->release();
//			SocketListEntry* entry = new SocketListEntry();
//	 entry->socketTCP = socket;
//	 entry->id = *otherID;
//	 entry->nextMsg = NULL;
//	 entry->prioritizedMsg = NULL;
//
//	 internalNetwork->socketListLock->aquire();
//	 internalNetwork->socketList.push_back(entry);
//	 internalNetwork->socketListLock->release();
	message.clear();
	message.putUInt32(internalNetwork->connectionRequestOkTag);
	Network::sendNetMessage(socket, &message);

	return true;
} // handleQuickConnect

volatile bool ServerThread::shutdown = false;
int ServerThread::port;
StreamSocket ServerThread::client;
Network* ServerThread::internalNetwork = NULL;

