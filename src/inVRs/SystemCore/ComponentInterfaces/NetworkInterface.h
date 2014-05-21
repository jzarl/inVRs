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

#ifndef _NETWORKINTERFACE_H
#define _NETWORKINTERFACE_H

#include <string>
#include <vector>

#include "ModuleInterface.h"
#include "../NetMessage.h"
#include "../DataTypes.h"

//struct SocketListEntry;
class TransformationPipe;

/**
 * Holds all for us relevant information of a connection
 * portTCP: server tcp port
 * portUDP: receiving udp port
 */
struct NetworkAddress
{
	unsigned ipAddress;
	unsigned short portUDP;
	unsigned short portTCP;
};

/**
 * A client is identified by its ip address, ports
 * and its processId (in case multiple instances are running on one machine).
 * IP-address and ports should be enough for a unique identification, but if
 * some parts of the network are removed
 * we are still on the save side with the processId
 */
struct NetworkIdentification
{
	NetworkAddress address;
	unsigned processId;
};

typedef void (*CLIENTDISCONNECTEDCALLBACK)(unsigned userId, void* whateveryouwant);

class INVRS_SYSTEMCORE_API NetworkInterface : public ModuleInterface
{
public:

	// Methods derived from ModuleInterface:
	virtual std::string getName() = 0;
	virtual void cleanup() = 0;

	/**
	 * \todo SYNCHRONISATION IS HAPPENING AT CONNECT
	 * !!! REMEMBER THE TRANSMISSION OF PIPE SETUP !!!
	 * Helmut: should consider doing that somewhere else (i.e.: TransformationManager::runFirst()), is that really needed for all applications?
	 */

	/**
	 * establishes a connection to a network
	 * @param nodeName node where the local process should connect to, the content of nodeName is implementation dependent
	 * @return true if connection has been established
	 */
	virtual bool connect(std::string nodeName)=0;

	virtual int sizeRecvList(uint8_t channelId=0) = 0;
	virtual NetMessage* peek(uint8_t channelId=0) = 0;
	virtual NetMessage* pop(uint8_t channelId=0) = 0;
	virtual void popAll(uint8_t channelId, std::vector<NetMessage*>* dst) = 0;

	virtual void sendMessageTCP(NetMessage* msg, uint8_t channelId=0) = 0;
	virtual void sendMessageUDP(NetMessage* msg, uint8_t channelId=0) = 0;
	virtual void sendMessageTCPTo(NetMessage* msg, uint8_t channelId, unsigned userId) = 0;
	virtual void sendMessageUDPTo(NetMessage* msg, uint8_t channelId, unsigned userId) = 0;
	virtual NetworkIdentification getLocalIdentification() = 0;

	/**
	 * DEPRECATED. Needed by the Transformation Manager during initialization (runFirst()).
	 * I hope we can get rid of it soon!
	 * @return returns the number of users in the network (not including the local user)
	 */
	virtual int getNumberOfParticipants() = 0;

	/**
	 * a NetMessage with the following layout will be transmitted:
	 *
	 *	struct MSGCONTENT
	 * 	{
	 *		uint32_t pipeOwnerUserId;
	 *		uint64_t pipeId;
	 *		uint32_t[14] contentOfTrans; // see addTransformationToBinaryMsg() (SystemCore/DataTypes.cpp)
	 *	};
	 *
	 * The message is expected to be addressed to the channel TRANSFORMATION_MANAGER_ID
	 *
	 * @param useTCP is intended to serve as hint
	 */
	virtual void sendTransformation(TransformationData& trans, TransformationPipe* pipe, bool useTCP) = 0;

	/**
	 * Visibility of Event is encoded in Event directly (visibilityLevel member)
	 * For now the EventManager sets a key "destinationUserId" to tell the network module if the event is addressed for a specific user
	 * The EventManager polls incomming events from the channel EVENT_MANAGER_ID
	 */
	virtual void sendEvent(Event* event) = 0;

	/**
	 * blocks current thread until all messages are sent
	 */
	virtual void flush() = 0;

	void registerConnectionClosedCallback(CLIENTDISCONNECTEDCALLBACK callback, unsigned userId, void* whateveryouwant);

	// some helpers:
	static std::string ipAddressToString(uint32_t ip);
	static uint32_t ipAddressFromString(std::string ip);

	// DEPRECATED: only to maintain compatibility with the old network interface
	static void addNetworkIdentificationToBinaryMessage(NetworkIdentification* netId, NetMessage* dst);
	static NetworkIdentification readNetworkIdentificationFrom(NetMessage* src);

protected:

	// TODO: need to specify thread here!!!!
	virtual void onConnectionClosed(unsigned userId);

	std::vector<CLIENTDISCONNECTEDCALLBACK> onDisconectCallbacks;
	std::vector<void*> onDisconectCallbackArgs;
};

#endif
