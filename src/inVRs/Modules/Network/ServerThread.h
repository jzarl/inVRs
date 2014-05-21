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

#ifndef _SERVERTHREAD_H
#define _SERVERTHREAD_H

// fix problems with Visual Studio 2008
#ifdef WIN32
#include <stdio.h>
#endif
#include <OpenSG/OSGStreamSocket.h>

#include "Network.h"


/**
 * This class listens for incoming connections and manages the
 * connection of new clients in an existing network.
 */
class ServerThread {
public:
	/**
	 * This method initializes the ServerThread. It opens a TCP socket
	 * with the port passed as parameter and starts listening for connections.
	 * @param serverport port for incoming connections
	 * @return true, if initialization succeeded
	 * @author rlander
	 * @author hbress
	 */
	static bool init(int serverport);

	/**
	 * This is the main method of the ServerThread. It waits for incoming connections,
	 * calls the handShake method for each new connection and adds the new connections
	 * to the socketList if everything worked. It runs until the kill method is called,
	 * then it enters the cleanupBarrier and waits for the other Threads (SendRecvThread
	 * and Main-thread) to shutdown.
	 * When the first client tries to connect the local IP-Address is taken from the
	 * opened socket and stored in the NetworkIdentification myId in class Network.
	 * @param dummy just a dummy parameter for Thread construction
	 * @author rlander
	 * @author hbress
	 */
	static void run(void* serverport);

	/**
	 * This method tells the ServerThread to leave it's loop and enter the
	 * cleanupBarrier.
	 */
	static void kill();

protected:
	/**
	 * This method makes the handshake between client and server to check,
	 * if the client is allowed or is able to connect to the network.
	 * For this it first receives a message from the client in which the
	 * client's NetworkIdentification and the connection type (normal or quick)
	 * is stored. With the ID the method first checks if the client is already
	 * connected to the local machine. If so, the connection is refused.
	 * The method then distinguishes between a quickConnect or a normal
	 * connection request.
	 * If the connection type is a quickConnect, the method adds the client
	 * to the socketList and sends a message with a connectionRequestOkTag back
	 * to the client.
	 * If it is a normal connection the method checks, if every client in the
	 * net is ready for a new connection (or if another client is already
	 * connecting) by calling the checkConnection method from class Network.
	 * If everyone in the net is ready for a new connection, the method sends
	 * a message with the local NetworkIdentification and a list of IDs of all other
	 * members in the net. The method then waits for a response message from
	 * the client saying that the client is done and then sends a message to
	 * all other members in the network that the connection request is finished.
	 * @param otherID pointer to the NetworkIdentification-Object for creation of a new
	 *	socketListEntry
	 * @param tag type of Message (quickConnectTag or normalMessageTag)
	 * @return true, if connection of client worked
	 * @author rlander
	 * @author hbress
	 */
	static bool handShake(OSG::StreamSocket* socket, UserNetworkIdentification* otherID, OSG::UInt32& tag);
	static bool handleNormalConnect(OSG::StreamSocket* socket, NetMessage& message);
	static bool handleQuickConnect(OSG::StreamSocket* socket, NetMessage& message,
			UserNetworkIdentification* otherID);

	static volatile bool shutdown;
	static int port;
	static OSG::StreamSocket client;
	static Network* internalNetwork;

	friend class Network;
};

#endif
