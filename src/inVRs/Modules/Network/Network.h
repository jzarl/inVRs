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

#ifndef _NETWORK_H
#define _NETWORK_H

// fix problems with Visual Studio 2008
#ifdef WIN32
#include <stdio.h>
#endif

#include <deque>
#include <string>
#include <vector>
#include <map>

#include <OpenSG/OSGStreamSocket.h>
#include <OpenSG/OSGDgramSocket.h>
#include <OpenSG/OSGLock.h>
#include <OpenSG/OSGThreadManager.h>
#include <OpenSG/OSGThread.h>

#include <inVRs/SystemCore/Platform.h>
#include <inVRs/SystemCore/NetMessage.h>
#include <inVRs/SystemCore/SyncPipe.h>
#include <inVRs/SystemCore/ComponentInterfaces/NetworkInterface.h>
#include <inVRs/SystemCore/XmlConfigurationLoader.h>
#include <inVRs/Modules/Network/NetworkSharedLibraryExports.h>

class SendReceiveThread;

class SendListEntry {
public:

	SendListEntry();
	virtual ~SendListEntry();

	bool removeFromDestination(NetworkIdentification* id); // returns false if id is not in destinationList
	void setDestination(std::vector<NetworkIdentification>* newDestinationList);

	bool isMsgFor(NetworkIdentification* id);

	NetMessage* msg;
	std::vector<NetworkIdentification> destinationList; // also referred to as reference counter
};

struct UserNetworkIdentification {
	NetworkIdentification netId;
	unsigned userId;
};

/**
 * A collection of all data belonging to a connection.
 * nextMsg is maintained by the SendRecvThread, it is used
 * to navigate in sendListTCP member of the Network.
 * prioritizedMsg is used to override SendRecvThread's message sending
 * behaviour, prioritizedMsgs are processed immedeately.
 */
struct SocketListEntry {
	OSG::StreamSocket* socketTCP;
	SendListEntry* nextMsg;
	UserNetworkIdentification id;
	NetMessage* prioritizedMsg;
};

class INVRS_NETWORK_API Network : public NetworkInterface {
public:
	/**
	 * Constructor initializes Network Module.
	 */
	Network();

	/**
	 * Empty destructor
	 */
	virtual ~Network();

	/**
	 * Loads the configuration of the module from the passed config-file.
	 * It currently reads the ports for UDP and TCP from the file and calls the
	 * init-method.
	 */
	virtual bool loadConfig(std::string configFile);

	/**
	 * Cleans up the network module.
	 * runs flush implicitily!
	 * message which are still in the queue will not be discarded!
	 */
	virtual void cleanup();

	/**
	 * This method establishes the connection to a network. For this it first
	 * checks, if there is already a connection to another computer, and returns
	 * false if so, because joining 2 subnets is not supported. It then tries to
	 * open a socket to the computer with the passed IP-address and returns false
	 * if the socket-connection couldn't be established. The method then reads the
	 * local IP-address from the open socket and stores it in the local
	 * NetworkIdentification myId in class Network. Afterwards the
	 * handShake-method is called and if the handShake worked fine the connection
	 * is entered in the socketList.
	 * @param nodeName <ip>:<port> of machine to connect to
	 * @return true, if connection worked
	 * @author rlander
	 * @author hbress
	 */
	virtual bool connect(std::string nodeName);

	/**
	 * This method returns the number of messages waiting in the
	 * receiveList.
	 * @return number of messages waiting in receiveList
	 * @author rlander
	 * @author hbress
	 */
	virtual int sizeRecvList(uint8_t channelId = 0);

	/**
	 * This method returns a pointer to the first Message in the
	 * receiveList.
	 * @return pointer to first message in receiveList
	 * @author hbress
	 * @author rlander
	 */
	virtual NetMessage* peek(uint8_t channelId = 0);

	/**
	 * This method pops the first message from the receiveList.
	 * @return first message in receiveList
	 * @author hbress
	 * @author rlander
	 */
	virtual NetMessage* pop(uint8_t channelId = 0);
	virtual void popAll(uint8_t channelId, std::vector<NetMessage*>* dst);

	virtual void sendMessageTCP(NetMessage* msg, uint8_t channelId = 0);
	virtual void sendMessageUDP(NetMessage* msg, uint8_t channelId = 0);
	virtual void sendMessageTCPTo(NetMessage* msg, uint8_t channelId, unsigned userId);
	virtual void sendMessageUDPTo(NetMessage* msg, uint8_t channelId, unsigned userId);
	virtual void sendTransformation(TransformationData& trans, TransformationPipe* pipe,
			bool useTCP);

	/**
	 * encodes a Event into NetMessage and transmitts it via TCP
	 * checks event->visibilityLevel for int "destinationUserId".
	 * If not set the event will be broadcasted otherwise it will be send to
	 * userid referenced to by that key
	 */
	virtual void sendEvent(Event* event);

	virtual void flush();

	/**
	 * Returns the name of the module.
	 */
	virtual std::string getName();

	virtual int getNumberOfParticipants();
	virtual NetworkIdentification getLocalIdentification();

protected:

	/**
	 * Initializes the network module.
	 * @return true on success, false on failure (most likely because tcp port
	 * 			is already in use)
	 * @param portTCP tcp server port for incoming tcp connections
	 * @param portUDP port which receives udp messages
	 */
	bool init(uint16_t portTCP, uint16_t portUDP, std::string ipAddress = "");

	static void sendNetMessage(OSG::Socket* socket, NetMessage* msg);
	static void receiveNetMessage(OSG::Socket* socket, NetMessage* dst);

	// both udp methods require that the msg has set the readpointer to 0!!
	static void sendNetMessageTo(OSG::DgramSocket* socket, OSG::SocketAddress dst, NetMessage* msg);
	static void receiveNetMessageFrom(OSG::DgramSocket* socket, OSG::SocketAddress* src,
			NetMessage* dst);

	/**
	 * this method is used internally to handle all sendMessageXXX() tasks
	 * @param destinationList NULL means broadcast to all available connections
	 */
	void sendMessageToGroup(NetMessage* msg, uint8_t channelId,
			std::vector<NetworkIdentification>* destinationList, bool useTCP);
	/**
	 * This method is for connecting to all other members of the network when the
	 * connection to the first member is established. While connecting to the
	 * network the connection partner sends back a list of all other members in
	 * the network (see handShake). To avoid checking, if the new client is
	 * allowed to connect (since this was already done by the ServerThread of the
	 * first partner) this message just establishes the connection without a long
	 * handShake. For this it tries to open a socket to the computer with the
	 * passed NetworkIdentification and then calls the handShake-method with a
	 * parameter set for the quick-connection. If the handShake worked the new
	 * connection is added to the socketList.
	 * @param id NetworkIdentification of the partner to connect to
	 * @return true, if quick-connection worked
	 * @author rlander
	 * @author hbress
	 */
	bool quickConnect(UserNetworkIdentification* id);

	/**
	 * This method makes the handshake between the local computer and the parnter
	 * to check, if the local computer is allowed or is able to connect to the
	 * network. For this it first sends the type of the connection (quick or
	 * normal). Then the method distinguishes between a quick-connection or a
	 * normal connection. If the connection type is a quick connection, the method
	 * receives an answer from the server if the connection is ok and returns true
	 * if so. If it is a normal connection the method receives a message from the
	 * partner and first decodes the message-tag to see if the connection is
	 * allowed. If the connection is allowed the method decodes the partners ID
	 * and the IDs of all members connected to the partner from the message. It
	 * then tries to establish a quick-connection to all connected members of the
	 * client. If everything went fine the method sends back a message to it's
	 * partner that the connection is done.
	 * @param socket the socket to the partner to connect with
	 * @param otherID the partner's ID decoded from the first message of the partner
	 * @param quick for distinction beween quick-connect and normal connect
	 * @return true, if the connection is established
	 * @author rlander
	 * @author hbress
	 */
	bool handShake(OSG::StreamSocket *socket, UserNetworkIdentification* id, bool quickConnect = false);
	bool handleNormalConnect(OSG::StreamSocket* socket, UserNetworkIdentification* otherID);
	bool handleQuickConnect(OSG::StreamSocket* socket);

	void encodeId(NetMessage* msg, UserNetworkIdentification* id, bool putFirst = false);
	void decodeId(UserNetworkIdentification* id, NetMessage* msg);
	int decodeMessageToIDs(NetMessage* msg, UserNetworkIdentification** ids);

  /**
	 * This method checks, if the a client is allowed to connect.
	 * For this it first checks locally, if already another client tries to
	 * connect to the network. If not, it sends a prioritizedMsg to
	 * all partners with the connectionRequestTag and waits for the answers
	 * of all partners. If every partner accepts the connection request
	 * the message returns true.
	 * @returns true, if the client is allowed to connect
	 * @author rlander
	 * @author hbress
	 */
	bool checkConnection();

	/**
	 * This method sends a message to all partners for finalizing the
	 * connection request of a client, so that everyone can accept new
	 * connections of other clients again.
	 * @author rlander
	 * @author hbress
	 */
	void connectionDone();

	/**
	 * This method checks, if a client with the passed NetworkIdentification is
	 * already connected to the local machine.
	 * @param id NetworkIdentification for check
	 * @return true, if a client with matching ID is already connected
	 * @author rlander
	 * @author hbress
	 */
	bool checkClientConnected(NetworkIdentification *id);

	/**
	 * This method sends a prioritizedMsg, which is needed e.g. for connections.
	 * A prioritized message bypasses the message queues.
	 * @param message message to send
	 * @return number of clients to which the message will be sent
	 * @author rlander
	 * @author hbress
	 */
	int sendPrioritizedMessage(NetMessage* msg);

	/**
	 * This method adds the IDs of every connected client to the passed
	 * message.
	 * @param msg message, where to put the IDs to
	 * @author rlander
	 * @author hbress
	 */
	void addClientsToMessage(NetMessage *msg);
	std::string dumpNetworkIdentification(NetworkIdentification* netId);
	void dumpConnection(SocketListEntry* con);

	/**
	 * Polls killedSocketsPipe and invokes onConnectionClosed() for each lost
	 * connection
	 * \todo implement this
	 */
	void handleLostConnections();

	/**
	 * This method checks, if two NetworkIdentifications are equal
	 * @param i1,i2 NetworkIdentifications to compare
	 * @return true, if ipAddress, TCP-port and processId are the same
	 * @author rlander
	 * @author hbress
	 */
	static bool IDequalsID(NetworkIdentification* i1, NetworkIdentification* i2);

	/**
	 * \todo check if myId should be locked because of multiple threads
	 */
	void setLocalIPAddress(OSG::StreamSocket* socket);

	// only for debugging: checks if nextMessage points to something in the sendlist
	// used in SendRecvThread
	void checkSocketAndSendListConsistency(SocketListEntry* sockets, int entries);

	void splitDestination(std::string destination, std::string& ipAddress, int& port);

	bool alreadyConnected();

	bool initializeConnectionRequest();
	void finalizeConnectionRequest();

	/**
	 * \todo check if we don't get threading-problems since this method is called from ServerThread too!!!
	 */
	void addConnectionToNetwork(OSG::StreamSocket* socket, UserNetworkIdentification& otherID);

	std::map<uint32_t, NetworkIdentification> mapUserToNetworkId;

	std::deque<SendListEntry*> sendListTCP;
	// contains a reference counted ordered set of messages which have to
	// be delivered via tcp. That's the message queue the SendRecvThread is
	// working with

	std::deque<NetMessage*>* recvList[256];
	// list of incoming binary messages (UDP and TCP)

	std::vector<SocketListEntry*> socketList;
	// list of all active connections

	std::deque<SendListEntry*> sendListUDP;
	// message queue for outgoing udp messages

	bool connectionLocalAllowed;
	bool connectionGlobalAllowed;
	bool isInitialized;
	bool connectCalled;
#if OSG_MAJOR_VERSION >= 2
	OSG::LockRefPtr socketListLock;
	OSG::LockRefPtr connectionDisconnectionLock;
	OSG::LockRefPtr recvListLock;
	OSG::LockRefPtr sendListLock;		
#else //OpenSG1:
	OSG::Lock* socketListLock;
	OSG::Lock* connectionDisconnectionLock;
	OSG::Lock* recvListLock;
	OSG::Lock* sendListLock;
#endif
	OSG::DgramSocket socketUDP;
	NetworkIdentification myId;
#if OSG_MAJOR_VERSION >= 2
	OSG::ThreadRefPtr sendRecvThread;
	OSG::ThreadRefPtr serverThread;		
#else //OpenSG1:
	OSG::Thread* sendRecvThread;
	OSG::Thread* serverThread;
#endif
	SocketListEntry* newSocketListEntry;
	SendReceiveThread* sendRecvObj;
	SyncPipe<NetworkIdentification*> killedSocketsPipe; // filled by SendRecv Thread

	static const uint32_t normalMsgTag;
	// 	static const uint32_t disconnectTag;
	static const uint32_t connectionRequestTag;
	static const uint32_t connectionRequestOkTag;
	static const uint32_t connectionRequestDenyTag;
	static const uint32_t quickConnectTag;
	static const uint32_t connectionRequestDoneTag;
	static const uint32_t quickConnectFailedTag;

	friend class ServerThread;
	friend class SendReceiveThread;
	friend class SendListEntry;
	static const uint32_t quickConnectOkTag;

//*****************************************************************************
// Configuration loading
//*****************************************************************************
private:
	static XmlConfigurationLoader xmlConfigLoader;

	// Although the converter does not change anything it is used to add the
	// initial version number 1.0a4 (so that all configuration files have a
	// version then)
	class ConverterToV1_0a4 : public XmlConfigurationConverter {
	public:
		ConverterToV1_0a4();
		virtual bool convert(XmlDocument* document, const Version& version, std::string configFile);
	}; // ConverterToV1_0a4

};

#endif /*_NETWORK_H*/
