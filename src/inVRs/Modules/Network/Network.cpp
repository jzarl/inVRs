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

#include <stdio.h>
#include <stdlib.h>
#ifndef WIN32
#include <unistd.h>
#include <sys/types.h>
#else
#include <Winsock2.h>
#endif
#include <assert.h>

#include "Network.h"
#include "ServerThread.h"
#include "SendReceiveThread.h"
#include <inVRs/SystemCore/Configuration.h>
#include <inVRs/SystemCore/DebugOutput.h>
#include <inVRs/SystemCore/Platform.h>
#include <inVRs/SystemCore/UserDatabase/UserDatabase.h>
#include <inVRs/SystemCore/UtilityFunctions.h>
#include <inVRs/SystemCore/XMLTools.h>
#include <inVRs/SystemCore/SystemCore.h>
#include <inVRs/SystemCore/TransformationManager/TransformationManager.h>
#include <inVRs/SystemCore/ComponentInterfaces/module_core_api.h>

// disable deprecation warning for std::auto_ptr when std::unique_ptr is not available.
#ifndef HAS_CXX11_UNIQUE_PTR
// Only do this on GCC versions that support it:
# if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 402
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
#endif

const uint32_t Network::normalMsgTag = 0;
const uint32_t Network::connectionRequestTag = 1;
const uint32_t Network::connectionRequestOkTag = 2;
const uint32_t Network::connectionRequestDenyTag = 3;
const uint32_t Network::quickConnectTag = 4;
const uint32_t Network::connectionRequestDoneTag = 5;
const uint32_t Network::quickConnectFailedTag = 6;
const uint32_t Network::quickConnectOkTag = 7;

XmlConfigurationLoader Network::xmlConfigLoader;

//using namespace irr;
//using namespace io;
OSG_USING_NAMESPACE

SendListEntry::SendListEntry() {
	msg = NULL;
}

SendListEntry::~SendListEntry() {
	if (msg)
		delete msg;
}

bool SendListEntry::removeFromDestination(NetworkIdentification* id) {
	int i;

	for (i = 0; i < (int)destinationList.size(); i++)
		if (Network::IDequalsID(id, &destinationList[i])) {
			destinationList.erase(destinationList.begin() + i);
			return true;
		}

	return false;
}

void SendListEntry::setDestination(std::vector<NetworkIdentification>* newDestinationList) {
	int i;
	destinationList.clear();
	for (i = 0; i < (int)newDestinationList->size(); i++)
		destinationList.push_back((*newDestinationList)[i]);
}

bool SendListEntry::isMsgFor(NetworkIdentification* id) {
	int i;

	for (i = 0; i < (int)destinationList.size(); i++)
		if (Network::IDequalsID(id, &destinationList[i]))
			return true;

	return false;
}

Network::Network() {
	socketListLock = NULL;
	connectionDisconnectionLock = NULL;
	sendListLock = NULL;
	recvListLock = NULL;

	sendRecvObj = NULL;
	sendRecvThread = NULL;
	serverThread = NULL;
	newSocketListEntry = NULL;
	isInitialized = false;
	connectCalled = false;
	xmlConfigLoader.registerConverter(new ConverterToV1_0a4);
} // Network

Network::~Network() {
} // ~Network

bool Network::loadConfig(std::string configFile) {

	std::string configFileConcatenatedPath = configFile;
	if (!Configuration::containsPath("NetworkModuleConfiguration") &&
			Configuration::containsPath("NetworkConfiguration")) {
		printd(WARNING,
				"Network::loadConfig(): Deprecated path entry NetworkConfiguration found in general configuration file! Use NetworkModuleConfiguration instead!\n");
		configFileConcatenatedPath = getConcatenatedPath(configFile, "NetworkConfiguration");
	} else {
		configFileConcatenatedPath = getConcatenatedPath(configFile, "NetworkModuleConfiguration");
	} // else

	// load xml document via xmlConfigLoader
	std::auto_ptr<const XmlDocument> document(
			xmlConfigLoader.loadConfiguration(configFileConcatenatedPath));
	if (!document.get()) {
		printd(ERROR,
				"Network::loadConfig(): error at loading of configuration file %s!\n",
				configFile.c_str());
		return false;
	} // if

	bool success = true;

	if (!document->hasAttribute("network.ports.TCP")
			|| !document->hasAttribute("network.ports.UDP")) {
		printd(ERROR,
				"Network::loadConfig(): missing TCP or UDP attribute in <ports> element found! Please fix your Network module configuration file!\n");
		return false;
	} // if

	std::string ipAddress = "";
	int portTCP = document->getAttributeValueAsInt("network.ports.TCP");
	int portUDP = document->getAttributeValueAsInt("network.ports.UDP");
	if (document->hasAttribute("network.localIP.value")) {
		ipAddress = document->getAttributeValue("network.localIP.value");
	} // if

	if (ipAddress.length() > 0) {
		success = this->init(portTCP, portUDP, ipAddress) && success;
	} // if
	else {
		success = this->init(portTCP, portUDP) && success;
	} // else

	return success;
} // loadConfig

void Network::cleanup() {
	if (!isInitialized)
		return;

	int i, j, size;

	this->flush();

	// CLEANUP
	ServerThread::kill();
	sendRecvObj->kill();

#if OSG_MAJOR_VERSION >= 2
	// Enter Barrier
	BarrierRefPtr cleanupBarrier = OSG::dynamic_pointer_cast<OSG::Barrier> (ThreadManager::the()->getBarrier(
				"cleanupBarrier",false));		
#else //OpenSG1:
	// Enter Barrier
	Barrier* cleanupBarrier = dynamic_cast<Barrier*> (ThreadManager::the()->getBarrier(
				"cleanupBarrier"));
#endif
	printd(INFO, "Network::cleanup(): entering cleanupBarrier with counter 3!\n");
	cleanupBarrier->enter(3);
	printd(INFO, "Network::cleanup(): leaving barrier!\n");

	SocketListEntry* entry;
	if (socketList.size() > 0) {
		if (socketList[0]->prioritizedMsg != NULL)
			delete socketList[0]->prioritizedMsg;
		size = socketList.size();
		for (i = 0; i < size; i++) {
			entry = socketList[i];
			entry->socketTCP->close();
			delete entry->socketTCP;
			delete entry;
		} // for
		socketList.clear();
	} // if

	SendListEntry *sEntry;
	size = sendListTCP.size();
	for (i = 0; i < size; i++) {
		sEntry = sendListTCP[0];
		delete sEntry->msg;
		sEntry->msg = NULL;
		sendListTCP.erase(sendListTCP.begin());
		delete sEntry;
	} // for

	size = sendListUDP.size();
	for (i = 0; i < size; i++) {
		sEntry = sendListUDP[0];
		delete sEntry->msg;
		sEntry->msg = NULL;
		sendListTCP.erase(sendListUDP.begin());
		delete sEntry;
	} // for

	NetMessage * msg;
#if OSG_MAJOR_VERSION >= 2
	recvListLock->acquire();
#else //OpenSG1:
	recvListLock->aquire();
#endif
	for (i = 0; i < 256; i++) {
		if (recvList[i] != NULL) {
			size = recvList[i]->size();
			for (j = 0; j < size; j++) {
				msg = (*recvList[i])[0];
				recvList[i]->erase(recvList[i]->begin());
				delete msg;
			} // for
			delete recvList[i];
			recvList[i] = NULL;
		}
	}
	recvListLock->release();

	delete sendRecvObj;

	socketUDP.close();

	printd(INFO, "Network::cleanup(): goodbye!\n");
} // cleanup

bool Network::connect(std::string nodeName) {

	int port;
	std::string ipAddress;
	SocketAddress address;
	UserNetworkIdentification otherID;

	if (!isInitialized)
		return false;

	splitDestination(nodeName, ipAddress, port);

	printd(INFO, "Network::connect(): connecting to %s:%u\n", ipAddress.c_str(), port);

	// check if someone already connected
	if (alreadyConnected()) {
		printd(INFO, "Network::connect(): Connection already established! Ignoring!\n");
		return false;
	} // if

	// initialize connection
	if (!initializeConnectionRequest()) {
		printd(INFO,
				"Network::connect(): Can't initialize connection because someone else is connecting currently!\n");
		return false;
	} // if

	// Try to open socket to communication partner
	try {
		address = SocketAddress(ipAddress.c_str(), port);
	} // try
	catch (...) {
		printd(ERROR, "Network::connect: ERROR: couldn't find server %s!\n", ipAddress.c_str());
		finalizeConnectionRequest();
		return false;
	} // catch

	printd(INFO, "Network::connect(): create new Socket\n");
	StreamSocket* socket = new StreamSocket();
	printd(INFO, "Network::connect(): open Socket\n");

	try {
		socket->open();
		printd(INFO, "Network::connect(): trying to connect to server ...\n");
		socket->connect(address);
		printd(INFO, "Network::connect(): connection established\n");
	} // try
	catch (...) {
		printd(ERROR, "Network::connect(): Error at connection attempt!\n");
		try {
			socket->close();
		} catch (...) {
		}
		finalizeConnectionRequest();
		return false;
	} // catch

	// saving local IP address
	if (myId.address.ipAddress == 0x7F000001)
		setLocalIPAddress(socket);

	// calling handShake
	printd(INFO, "Network::connect(): calling handShake!\n");
	if (!handShake(socket, &otherID)) {
		printd(ERROR, "Network::connect(): handShake FAILED!\n");
		try {
			socket->close();
		} catch (...) {
		}
		finalizeConnectionRequest();
		return false;
	} // if

	addConnectionToNetwork(socket, otherID);

	finalizeConnectionRequest();

	// used in connectionInitiated()
	connectCalled = true;

	return true;
} // connect

int Network::sizeRecvList(uint8_t channelId) {
	int result = 0;
	if (!isInitialized)
		return 0;
#if OSG_MAJOR_VERSION >= 2
	recvListLock->acquire();
#else //OpenSG1:
	recvListLock->aquire();
#endif
	if (recvList[channelId] != NULL)
		result = recvList[channelId]->size();
	recvListLock->release();
	return result;
}

NetMessage* Network::peek(uint8_t channelId) {
	NetMessage* ret = NULL;
	std::deque<NetMessage*>* list;
	if (!isInitialized)
		return NULL;
#if OSG_MAJOR_VERSION >= 2
	recvListLock->acquire();
#else //OpenSG1:
	recvListLock->aquire();
#endif
	list = recvList[channelId];
	if (list != NULL) {
		if (!list->empty())
			ret = (*list)[0];
	}
	recvListLock->release();
	return ret;
}

NetMessage* Network::pop(uint8_t channelId) {
	NetMessage* ret = NULL;
	std::deque<NetMessage*>* list;
	if (!isInitialized)
		return NULL;
#if OSG_MAJOR_VERSION >= 2
	recvListLock->acquire();
#else //OpenSG1:
	recvListLock->aquire();
#endif
	list = recvList[channelId];
	if (list != NULL) {

		if (!list->empty()) {
			ret = list->front();
			list->pop_front();
		}
	}
	recvListLock->release();
	return ret;
}

void Network::popAll(uint8_t channelId, std::vector<NetMessage*>* dst) {
	NetMessage* ret = NULL;
	std::deque<NetMessage*>* list;
	if (!isInitialized || (dst == NULL))
		return;
#if OSG_MAJOR_VERSION >= 2
	recvListLock->acquire();
#else //OpenSG1:
	recvListLock->aquire();
#endif
	list = recvList[channelId];
	if (list != NULL) {
		while (!list->empty()) {
			ret = list->front();
			list->pop_front();
			dst->push_back(ret);
		}
	}
	recvListLock->release();
}

void Network::sendMessageTCP(NetMessage* msg, uint8_t channelId) {
	sendMessageToGroup(msg, channelId, NULL, true);
}

void Network::sendMessageUDP(NetMessage* msg, uint8_t channelId) {
	sendMessageToGroup(msg, channelId, NULL, false);
}

void Network::sendMessageTCPTo(NetMessage* msg, uint8_t channelId, unsigned userId) {
	//	NetworkIdentification netId;
	std::vector<NetworkIdentification> netIdsList;
	std::map<uint32_t, NetworkIdentification>::iterator it = mapUserToNetworkId.find(userId);
	if (it == mapUserToNetworkId.end()) {
		printd(WARNING, "Network::sendMessageTCPTo(): cannot deliver messsage to user %u\n", userId);
		return;
	}
	//	netId = mapUserToNetworkId[userId];
	//	netIdsList.push_back(netId);
	netIdsList.push_back(it->second);
	sendMessageToGroup(msg, channelId, &netIdsList, true);
}

void Network::sendMessageUDPTo(NetMessage* msg, uint8_t channelId, unsigned userId) {
	NetworkIdentification netId;
	std::vector<NetworkIdentification> netIdsList;
	std::map<uint32_t, NetworkIdentification>::iterator it = mapUserToNetworkId.find(userId);
	if (it == mapUserToNetworkId.end()) {
		printd(WARNING, "Network::sendMessageUDPTo(): cannot deliver messsage to user %u\n", userId);
		return;
	}
	netId = mapUserToNetworkId[userId];
	netIdsList.push_back(netId);
	sendMessageToGroup(msg, channelId, &netIdsList, false);
}

void Network::sendTransformation(TransformationData& trans, TransformationPipe* pipe, bool useTCP) {
	NetMessage netMsg;

	netMsg.putUInt32((unsigned)pipe->getOwner()->getId()); // cast into 32-bit just to maintain compatibility with old code
	netMsg.putUInt64(pipe->getPipeId());
	addTransformationToBinaryMsg(&trans, &netMsg);

	sendMessageToGroup(&netMsg, TRANSFORMATION_MANAGER_ID, NULL, useTCP); // broadcast
}

void Network::sendEvent(Event* event) {
	NetMessage * msg;
	unsigned int dst = 0;

	msg = event->completeEncode();
	if (event->visibilityLevel.keyExists("destinationUserId")) {
		event->visibilityLevel.get("destinationUserId", dst);
	}
	if (dst == 0) {
		//		printd(INFO, "Network::sendEvent(): broadcasting event\n");
		sendMessageToGroup(msg, EVENT_MANAGER_ID, NULL, true); // broadcast over tcp
	} else {
		uint32_t dstUserId = (uint32_t)dst;
		//		printd(INFO, "Network::sendEvent(): sending event to user %u\n", dstUserId);
		sendMessageTCPTo(msg, EVENT_MANAGER_ID, dstUserId);
	}
	delete msg;
}

void Network::flush() {
	bool doSleep;

	if (!isInitialized)
		return;

	do {
#if OSG_MAJOR_VERSION >= 2
		sendListLock->acquire();
#else //OpenSG1:
		sendListLock->aquire();
#endif
		if ((sendListTCP.size() > 0) || (sendListUDP.size() > 0)) {
			printd(INFO, "Network::flush(): sendListTCP: %u, sendListUDP: %u entries\n",
					sendListTCP.size(), sendListUDP.size());
			doSleep = true;
		} else {
			doSleep = false;
		}
		sendListLock->release();

		// TODO: shouldn't this be 50000 microseconds?
		if (doSleep)
			usleep(5000000); // 50 ms
	} while (doSleep);
}

std::string Network::getName() {
	return "Network";
} // getName

int Network::getNumberOfParticipants() {
	int socketListSize;

	if (!socketListLock)
		return 0;

#if OSG_MAJOR_VERSION >= 2
	socketListLock->acquire();
#else //OpenSG1:
	socketListLock->aquire();
#endif
	socketListSize = socketList.size();
	socketListLock->release();
	return socketListSize;
}

NetworkIdentification Network::getLocalIdentification() {
	return myId;
}

bool Network::init(uint16_t portTCP, uint16_t portUDP, std::string ipAddress) {
	int i;
	User * localUser;

	if (isInitialized) {
		printd(ERROR, "Network::init(): called a second time!\n");
		return false;
	} // if

	// initialize lists for incoming Messages
	for (i = 0; i < 256; i++)
		recvList[i] = NULL;

	// initialize variables for connection requests
	connectionLocalAllowed = true;
	connectionGlobalAllowed = true;

#if OSG_MAJOR_VERSION >= 2
	// initialize Locks for communication with other Threads
	sendListLock = OSG::dynamic_pointer_cast<OSG::Lock> (ThreadManager::the()->getLock("sendListLock",false));
	recvListLock = OSG::dynamic_pointer_cast<OSG::Lock> (ThreadManager::the()->getLock("recvListLock",false));
	socketListLock = OSG::dynamic_pointer_cast<OSG::Lock>(ThreadManager::the()->getLock("socketListLock",false));
	connectionDisconnectionLock = OSG::dynamic_pointer_cast<OSG::Lock>(ThreadManager::the()->getLock(
			"connectionDisconnectionLock",false));	
#else //OpenSG1:
	// initialize Locks for communication with other Threads
	sendListLock = dynamic_cast<Lock*> (ThreadManager::the()->getLock("sendListLock"));
	recvListLock = dynamic_cast<Lock*> (ThreadManager::the()->getLock("recvListLock"));
	socketListLock = dynamic_cast<Lock*> (ThreadManager::the()->getLock("socketListLock"));
	connectionDisconnectionLock = dynamic_cast<Lock*> (ThreadManager::the()->getLock(
			"connectionDisconnectionLock"));
#endif

	// initialize local NetworkIdentification
	myId.address.portUDP = portUDP;
	myId.address.portTCP = portTCP;
	// ServerThread sets IP-Address from server socket:
	if (ipAddress.size() == 0)
		myId.address.ipAddress = 0x7F000001;
	else
		myId.address.ipAddress = ipAddressFromString(ipAddress);

#ifndef WIN32
	myId.processId = (UInt32)getpid(); // Linux + Irix
#else
	myId.processId = (UInt32)GetCurrentProcessId();
#endif

	// store NetworkIdentification in User
	localUser = UserDatabase::getLocalUser();
	if (localUser)
		localUser->setNetworkId(&myId);

	// open socket for UDP
	socketUDP.open();
	socketUDP.bind(SocketAddress(SocketAddress::ANY, portUDP));

	// initialize and start ServerThread
	ServerThread::internalNetwork = this;
	if (!ServerThread::init(portTCP)) {
		socketUDP.close();
		return false;
	} // if

	printd(INFO, "Network::init(): Try to start ServerThread!\n");
#if OSG_MAJOR_VERSION >= 2
	serverThread = OSG::dynamic_pointer_cast<OSG::Thread> (ThreadManager::the()->getThread("ServerThread",false));	
#else //OpenSG1:
	serverThread = dynamic_cast<Thread *> (ThreadManager::the()->getThread("ServerThread"));
#endif
	serverThread->runFunction(ServerThread::run, 0, NULL);

	// create and start SendReceiveThread
	sendRecvObj = new SendReceiveThread();

	printd(INFO, "Network::init(): Try to start SendRecvThread!\n");
	SendReceiveThread::internalNetwork = this;
#if OSG_MAJOR_VERSION >= 2
	sendRecvThread = OSG::dynamic_pointer_cast<OSG::Thread> (ThreadManager::the()->getThread("SendRecvThread",false));	
#else //OpenSG1:
	sendRecvThread = dynamic_cast<Thread *> (ThreadManager::the()->getThread("SendRecvThread"));
#endif
	sendRecvThread->runFunction(SendReceiveThread::run, 0, sendRecvObj);

	isInitialized = true;
	return true;
} // init

void Network::sendNetMessage(OSG::Socket* socket, NetMessage* msg) {
	unsigned msgSize = msg->getBufferSize();

	//TODO: Check why we have to create a copy here! There has to be some problem
	//      when modifiing the original message!!!

	NetMessage* msgCopy = new NetMessage(msg);
	msgCopy->reset();
	msgCopy->putFirstUInt32(msgSize);
	socket->send(msgCopy->getBufferPointer(), msgSize + 4);
	delete msgCopy;
} // sendNetMessage

void Network::receiveNetMessage(OSG::Socket* socket, NetMessage* dst) {
	unsigned msgSize, msgSizeNet;
	uint8_t* buffer;
	socket->recv(&msgSizeNet, 4);
	msgSize = ntohl(msgSizeNet);
	buffer = dst->allocateAtEnd(msgSize);
	socket->recv(buffer, msgSize);
}

void Network::sendNetMessageTo(OSG::DgramSocket* socket, OSG::SocketAddress dst, NetMessage* msg) {
	unsigned msgSize = msg->getBufferSize();

	//TODO: Check why we have to create a copy here! There has to be some problem
	//      when modifiing the original message!!!

	NetMessage* msgCopy = new NetMessage(msg);
	msgCopy->reset();
	msgCopy->putFirstUInt32(msgSize);
	socket->sendTo(msgCopy->getBufferPointer(), msgSize + 4, dst);
	delete msgCopy;
} // sendNetMessageTo

void Network::receiveNetMessageFrom(OSG::DgramSocket* socket, OSG::SocketAddress* src,
		NetMessage* dst) {
	unsigned msgSize, msgSizeNet;
	uint8_t* buffer;
	socket->peek(&msgSizeNet, 4);
	msgSize = ntohl(msgSizeNet);
	buffer = dst->allocateAtFront(msgSize + 4);
#ifdef OPENSG_DGRAMSOCKET_HAVE_RECVFROMRAW_FUNCTION
	socket->recvFromRaw(buffer, msgSize + 4, *src);
#else
	socket->recvFrom(buffer, msgSize + 4, *src);
#endif
	// 	dst->dump(stderr);
	if (!dst->removeFromFront(4))
		assert(false);
}

void Network::sendMessageToGroup(NetMessage* msg, uint8_t channelId, std::vector<
		NetworkIdentification>* destinationList, bool useTCP) {
	//TODO: fix dirty hack!!!
	std::vector<NetworkIdentification>* tempNetIdsList = new std::vector<NetworkIdentification>;
	int socketListSize = 0;
	int i;
	NetMessage* copy;
	SendListEntry* sendListEntry;

	if (!isInitialized)
		return;

	if (destinationList == NULL) {
		// broadcast, need to check current socket set:
#if OSG_MAJOR_VERSION >= 2
		socketListLock->acquire();
#else //OpenSG1:
		socketListLock->aquire();
#endif
		socketListSize = socketList.size();
		for (i = 0; i < socketListSize; i++) {
			tempNetIdsList->push_back(socketList[i]->id.netId);
		}
		destinationList = tempNetIdsList;
		socketListLock->release();
	}

	copy = new NetMessage(msg);
	copy->putFirstUInt8(channelId);
	copy->putFirstUInt32(normalMsgTag);
	sendListEntry = new SendListEntry();
	sendListEntry->msg = copy;
	sendListEntry->setDestination(destinationList);

	//	printd(INFO, "DestinationListSize = %i\n", destinationList->size());

#if OSG_MAJOR_VERSION >= 2
	sendListLock->acquire();
#else //OpenSG1:
	sendListLock->aquire();
#endif
	if (useTCP)
		sendListTCP.push_back(sendListEntry);
	else
		sendListUDP.push_back(sendListEntry);
	sendListLock->release();
}

bool Network::quickConnect(UserNetworkIdentification* id) {
	SocketAddress address;
	std::string ipString;

	ipString = ipAddressToString(id->netId.address.ipAddress);
	printd(INFO, "Network::quickConnect(): trying to quickConnect to %s\n", ipString.c_str());

	try {
		address = SocketAddress(ipString.c_str(), id->netId.address.portTCP);
	} catch (SocketException &e) {
		printd(ERROR, "Network::quickConnect: ERROR: couldn't find server %s!\n", ipString.c_str());
		return false;
	}

	printd(INFO, "Network::quickConnect(): create new Socket\n");
	StreamSocket* socket = new StreamSocket();

	try {
		printd(INFO, "Network::quickConnect(): open Socket\n");
		socket->open();
		printd(INFO, "Network::quickConnect(): trying to connect to server ...\n");
		socket->connect(address);
		printd(INFO, "Network::quickConnect(): connection established, calling handShake\n");
	} // try
	catch (...) {
		printd(ERROR, "Network::quickConnect(): connection failed!\n");
		try {
			socket->close();
		} catch (...) {
		}
		return false;
	}
	// catch

	if (!handShake(socket, id, true)) {
		printd(ERROR, "Network::quickConnect(): handShake FAILED!\n");
		try {
			socket->close();
		} catch (...) {
		}
		return false;
	} // if

	printd(INFO, "Network::quickConnect(): creating new SocketListEntry!\n");

	addConnectionToNetwork(socket, *id);

	return true;
} // quickConnect

bool Network::handShake(StreamSocket *socket, UserNetworkIdentification* otherID, bool quick) {

	bool success;
	UInt32 tag;
	NetMessage message;
	UserNetworkIdentification myIdWithUser;

	if (quick)
		tag = quickConnectTag;
	else
		tag = normalMsgTag;

	printd(INFO, "Network::handShake(): sending message with myId to partner (tag = %s)!\n", (tag
			== normalMsgTag ? "normalMsgTag" : "quickConnectTag"));

	// send Message with myId to partner
	message.putUInt32(tag);
	myIdWithUser.netId = myId;
	myIdWithUser.userId = UserDatabase::getLocalUserId();
	encodeId(&message, &myIdWithUser);

	try {
		sendNetMessage(socket, &message);
	} // try
	catch (...) {
		printd(ERROR, "Network::handShake(): error at sending Message!\n");
		return false;
	} // catch

	if (!quick) {
		success = handleNormalConnect(socket, otherID);
	} // if
	else {
		success = handleQuickConnect(socket);
	} // else

	return success;
} // handShake

bool Network::handleNormalConnect(StreamSocket* socket, UserNetworkIdentification* otherID) {

	int i;
	int size;
	UInt32 tag;
	NetMessage message;
	UserNetworkIdentification *ids;

	printd(INFO, "Network::handleNormalConnect(): receiving Message with OK/Deny and other's ID!\n");

	message.clear();

	// Receive server's answer and hopefully it's ID and the ID's of all other members in the net
	try {
		receiveNetMessage(socket, &message);
	} catch (...) {
		printd(ERROR, "Network::handleNormalConnect(): error at receiving message!\n");
		return false;
	} // try

	message.getUInt32(tag);

	// Check if connection is valid
	if (tag == connectionRequestDenyTag) {
		printd(WARNING, "Network::handleNormalConnect(): partner DENIED connection!\n");
		return false;
	} // it

	printd(INFO, "Network::handleNormalConnect(): decoding other's ID!\n");

	decodeId(otherID, &message);

	printd(INFO, "Network::handleNormalConnect(): server has ip %s and userId %u\n",
			ipAddressToString(otherID->netId.address.ipAddress).c_str(), otherID->userId);

	size = decodeMessageToIDs(&message, &ids);

	printd(INFO, "Network::handleNormalConnect(): partner has %i connections!\n", size);

	// Try to quick-connect to server's partners
	bool success = true;
	for (i = 0; i < size; i++) {
		// Quick connect
		printd(INFO, "Network::handleNormalConnect(): quickConnect to Parnter with ip %s!\n",
				ipAddressToString(ids[i].netId.address.ipAddress).c_str());
		success = success && quickConnect(&(ids[i]));
	} // for

	// Send back server if all connections worked
	message.clear();
	message.putUInt32(success ? quickConnectOkTag : quickConnectFailedTag);
	try {
		sendNetMessage(socket, &message);
	} // try
	catch (...) {
		printd(ERROR,
				"Network::handleNormalConnect(): error at sending quickConnectTag to server\n");
		return false;
	} // catch

	if (!success) {
		printd(ERROR,
				"Network::handleNormalConnect(): connection failed, closing existing connections ...\n");
		sleep(2);
#if OSG_MAJOR_VERSION >= 2
		socketListLock->acquire();
#else //OpenSG1:
		socketListLock->aquire();
#endif
		while (!socketList.empty()) {
			SocketListEntry* entry;
			entry = socketList[0];
			entry->socketTCP->close();
			delete entry->socketTCP;
			socketList.erase(socketList.begin());
			delete entry;
		} // while
		socketListLock->release();
		mapUserToNetworkId.clear();
		return false;
	} // if
	return true;
} // handleNormalConnect

bool Network::handleQuickConnect(StreamSocket* socket) {
	NetMessage message;
	bool success;

	message.clear();
	try {
		receiveNetMessage(socket, &message);
		if (message.getUInt32() != connectionRequestOkTag) {
			printd(ERROR, "Network::handShake(): internal error: quick-connect approach failed!\n");
			success = false;
		} // if
		else
			success = true;
	} catch (...) {
		printd(ERROR, "Network::handShake(): error at receiving quick-connect Message!\n");
		success = false;
	} // catch

	return success;
} // handleQuickConnect

void Network::encodeId(NetMessage* msg, UserNetworkIdentification* id, bool putFirst) {
	if (!putFirst) {
		msg->putUInt32(id->netId.address.ipAddress);
		msg->putUInt16(id->netId.address.portUDP);
		msg->putUInt16(id->netId.address.portTCP);
		msg->putUInt32(id->netId.processId);
		msg->putUInt32(id->userId);
	} // if
	else {
		// reverted order since data is always inserted at front
		msg->putFirstUInt32(id->userId);
		msg->putFirstUInt32(id->netId.processId);
		msg->putFirstUInt16(id->netId.address.portTCP);
		msg->putFirstUInt16(id->netId.address.portUDP);
		msg->putFirstUInt32(id->netId.address.ipAddress);
	} // else
} // encodeId

void Network::decodeId(UserNetworkIdentification* id, NetMessage* msg) {
	printd(INFO, "Network::decodeId(): entering method\n");
	msg->getUInt32(id->netId.address.ipAddress);
	id->netId.address.portUDP = msg->getUInt16();
	id->netId.address.portTCP = msg->getUInt16();
	msg->getUInt32(id->netId.processId);
	id->userId = msg->getUInt32();
	printd(INFO, "Network::decodeId(): leaving method\n");
} /// decodeId

int Network::decodeMessageToIDs(NetMessage* msg, UserNetworkIdentification** ids) {
	UserNetworkIdentification id;
	UInt8 size;
	size = msg->getUInt8();
	printd(INFO, "Network::decodeMessageToIDs(): allocating array with size %d\n", (UInt32)size);
	if (size > 0)
		*ids = new UserNetworkIdentification[size];
	else
		*ids = NULL;

	printd(INFO, "Network::decodeMessageToIDs(): entering loop ...\n");
	for (int i = 0; i < size; i++) {
		decodeId(&id, msg);
		(*ids)[i] = id;
	} // for
	return size;
}

bool Network::checkConnection() {

	int socketListSize;

#if OSG_MAJOR_VERSION >= 2
	connectionDisconnectionLock->acquire();
#else //OpenSG1:
	connectionDisconnectionLock->aquire();
#endif
	if (connectionLocalAllowed == false) {
		connectionDisconnectionLock->release();
		return false;
	} // if
	connectionLocalAllowed = false;
	connectionDisconnectionLock->release();

#if OSG_MAJOR_VERSION >= 2
	socketListLock->acquire();
#else //OpenSG1:
	socketListLock->aquire();
#endif
	socketListSize = socketList.size();
	socketListLock->release();

	if (socketListSize == 0) {
		return true;
	} // if

	NetMessage* message = new NetMessage();

	// submit Connection-Request to all connected partners
	message->putUInt32(connectionRequestTag);

	sendPrioritizedMessage(message);

#if OSG_MAJOR_VERSION >= 2
	BarrierRefPtr connectionBarrier = OSG::dynamic_pointer_cast<OSG::Barrier> (ThreadManager::the()->getBarrier(
				"connectionBarrier",false));		
#else //OpenSG1:
	Barrier* connectionBarrier = dynamic_cast<Barrier*> (ThreadManager::the()->getBarrier(
				"connectionBarrier"));
#endif
	printd(INFO, "Network::checkConnection(): entering connectionBarrier with counter 2!\n");
	connectionBarrier->enter(2);
	printd(INFO, "Network::checkConnection(): got every response!\n");

	bool globalOK = false;

#if OSG_MAJOR_VERSION >= 2
	connectionDisconnectionLock->acquire();
#else //OpenSG1:
	connectionDisconnectionLock->aquire();
#endif
	globalOK = connectionGlobalAllowed;
	connectionDisconnectionLock->release();

	if (!globalOK) {
		connectionDone();
		return false;
	} // else

	return true;
} // checkConnection

void Network::connectionDone() {
	int socketListSize;
	printd(INFO, "Network::connectionDone(): Trying to get socketListLock!\n");
#if OSG_MAJOR_VERSION >= 2
	socketListLock->acquire();
#else //OpenSG1:
	socketListLock->aquire();
#endif
	socketListSize = socketList.size();
	socketListLock->release();
	printd(INFO, "Network::connectionDone(): freed socketListLock socketListLock!\n");

	// Broadcast to everyone, that connection is Done!!!
	NetMessage* doneMessage = new NetMessage();
	doneMessage->putUInt32(connectionRequestDoneTag);
	sendPrioritizedMessage(doneMessage);

	if (socketListSize != 0) {
#if OSG_MAJOR_VERSION >= 2
		BarrierRefPtr connectionBarrier = OSG::dynamic_pointer_cast<OSG::Barrier> (ThreadManager::the()->getBarrier(
				"connectionBarrier",false));		
#else //OpenSG1:
		Barrier* connectionBarrier = dynamic_cast<Barrier*> (ThreadManager::the()->getBarrier(
					"connectionBarrier"));
#endif
		printd(INFO, "Network::connectionDone(): entering connectionBarrier with counter 2!\n");
		connectionBarrier->enter(2);
		printd(INFO, "Network::connectionDone(): got every response!\n");
	} // if

#if OSG_MAJOR_VERSION >= 2
	connectionDisconnectionLock->acquire();
#else //OpenSG1:
	connectionDisconnectionLock->aquire();
#endif
	connectionLocalAllowed = true;
	connectionDisconnectionLock->release();
} // connectionDone

bool Network::checkClientConnected(NetworkIdentification *id) {
#if OSG_MAJOR_VERSION >= 2
	socketListLock->acquire();
#else //OpenSG1:
	socketListLock->aquire();
#endif
	for (int i = 0; i < (int)socketList.size(); i++) {
		if (IDequalsID(id, &(socketList[i]->id.netId))) {
			socketListLock->release();
			return true;
		} // if
	} // for
	socketListLock->release();
	return false;
}

int Network::sendPrioritizedMessage(NetMessage* message) {
	int socketListSize = 0;

#if OSG_MAJOR_VERSION >= 2
	socketListLock->acquire();
#else //OpenSG1:
	socketListLock->aquire();
#endif
	socketListSize = socketList.size();
	for (int i = 0; i < socketListSize; i++) {
		if (socketList[i]->prioritizedMsg != NULL)
			assert(false);
		socketList[i]->prioritizedMsg = message;
	} // for
	socketListLock->release();
	return socketListSize;
}

void Network::addClientsToMessage(NetMessage *msg) {
	UserNetworkIdentification *id;
#if OSG_MAJOR_VERSION >= 2
	socketListLock->acquire();
#else //OpenSG1:
	socketListLock->aquire();
#endif
	UInt8 size = socketList.size();
	msg->putUInt8(size);
	for (int i = 0; i < size; i++) {
		id = &(socketList[i]->id);
		encodeId(msg, id);
	} // for
	socketListLock->release();
}

std::string Network::dumpNetworkIdentification(NetworkIdentification* netId) {
	char buff2[1024];

	sprintf(buff2, "%s: UDP: %u TCP: %u PID: %u\n",
			ipAddressToString(netId->address.ipAddress).c_str(), netId->address.portUDP,
			netId->address.portTCP, netId->processId);
	return std::string(buff2);
}

void Network::dumpConnection(SocketListEntry* con) {
	// did nothing and therefore does nothing :-)
}

void Network::handleLostConnections() {

}

bool Network::IDequalsID(NetworkIdentification* i1, NetworkIdentification* i2) {
	if (i1->processId != i2->processId)
		return false;
	if (i1->address.ipAddress != i2->address.ipAddress)
		return false;
	//	if (i1->address.portUDP != i2->address.portUDP)
	//		return false;
	if (i1->address.portTCP != i2->address.portTCP)
		return false;
	return true;
} // IDequalsID

void Network::setLocalIPAddress(StreamSocket* socket) {
	UInt32 ip;
	std::string myIP;
	//	User* localUser;

	// get the local IP-address from the incomming socket
	myIP = socket->getAddress().getHost();
	printd(INFO, "Network::setLocalIPAddress(): found local IP-Address = %s\n", myIP.c_str());

	ip = ipAddressFromString(myIP);
	myId.address.ipAddress = ip;

	// REMOVED UPDATE SINCE MYID-POINTER IN USER IS STILL VALID!!!
	// update Network-identification in local user
	//	localUser = UserDatabase::getLocalUser();
	//	if (localUser)
	//		localUser->setNetworkId(&internalNetwork->myId);
} // setLocalIPAddress

void Network::checkSocketAndSendListConsistency(SocketListEntry* sockets, int entries) {
	int i, j;

	for (i = 0; i < entries; i++) {
		if (!sockets[i].nextMsg)
			continue;
		for (j = 0; j < (int)sendListTCP.size(); j++)
			if (sendListTCP[j] == sockets[i].nextMsg)
				break;

		if (j == (int)sendListTCP.size()) {
			printd(
					ERROR,
					"Network::checkSocketAndSendListConsistency(): internal error, sockets next message is not what it pretends to be. Most likely it has been delete'd already!!\n");
			assert(false);
		}

		if (!sockets[i].nextMsg->isMsgFor(&sockets[i].id.netId)) {
			printd(
					ERROR,
					"Network::checkSocketAndSendListConsistency(): internal error, message is being sended to the wrong destination!\n");
			assert(false);
		}
	}
}

void Network::splitDestination(std::string destination, std::string& ipAddress, int& port) {
	int portPos = destination.find(':', 0);
	port = atoi(destination.substr(portPos + 1, destination.size()).c_str());
	ipAddress = destination.substr(0, portPos);
} // splitDestination

bool Network::alreadyConnected() {
	int socketListSize;
#if OSG_MAJOR_VERSION >= 2
	socketListLock->acquire();
#else //OpenSG1:
	socketListLock->aquire();
#endif
	socketListSize = socketList.size();
	socketListLock->release();

	return (socketListSize > 0);
} // connectionAlreadyEstablised

bool Network::initializeConnectionRequest() {
	bool allowed = true;
#if OSG_MAJOR_VERSION >= 2
	connectionDisconnectionLock->acquire();
#else //OpenSG1:
	connectionDisconnectionLock->aquire();
#endif
	if (connectionLocalAllowed)
		connectionLocalAllowed = false;
	else
		allowed = false;
	connectionDisconnectionLock->release();
	return allowed;
} // initializeConnectionRequest

void Network::finalizeConnectionRequest() {
#if OSG_MAJOR_VERSION >= 2
	connectionDisconnectionLock->acquire();
#else //OpenSG1:
	connectionDisconnectionLock->aquire();
#endif
	connectionLocalAllowed = true;
	connectionDisconnectionLock->release();
} // finalizeConnectionRequest

void Network::addConnectionToNetwork(StreamSocket* socket, UserNetworkIdentification& otherID) {
#if OSG_MAJOR_VERSION >= 2
	socketListLock->acquire();
#else //OpenSG1:
	socketListLock->aquire();
#endif
	// save connection in socketList
	printd(INFO, "Network::addConnectionToNetwork(): creating new SocketListEntry!\n");
	SocketListEntry* entry = new SocketListEntry();
	entry->socketTCP = socket;
	entry->id = otherID;
	entry->nextMsg = NULL;
	entry->prioritizedMsg = NULL;
	socketList.push_back(entry);

	// add id to user2id map:
	mapUserToNetworkId[otherID.userId] = otherID.netId;

	socketListLock->release();
} // addConnectionToNetwork

//*****************************************************************************
// Configuration loading
//*****************************************************************************
Network::ConverterToV1_0a4::ConverterToV1_0a4() :
	XmlConfigurationConverter("0.0", "1.0a4") {

} // ConverterToV1_0a4

bool Network::ConverterToV1_0a4::convert(XmlDocument* document, const Version& version,
		std::string configFile) {
	bool success = true;

	// add the link to the dtd and the version number
	updateDocument(document, XmlConfigurationLoader::XmlDtdUrl + "network_v1.0a4.dtd",
			destinationVersion, "network");

	return success;
} // convert


MAKEMODULEPLUGIN(Network, SystemCore)

