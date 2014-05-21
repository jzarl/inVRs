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

#ifndef _SENDRECEIVETHREAD_H
#define _SENDRECEIVETHREAD_H

// fix problems with Visual Studio 2008
#ifdef WIN32
#include <stdio.h>
#endif

#include <vector>
#include <deque>

#include <OpenSG/OSGThreadManager.h>
#include <OpenSG/OSGSocket.h>
#include <OpenSG/OSGStreamSocket.h>
#include <OpenSG/OSGSocketAddress.h>
#include <OpenSG/OSGBinaryMessage.h>
#include <OpenSG/OSGSocketSelection.h>

#include "Network.h"


/******************************************************************************
 * This class is responsible for delivering all messages
 * (of Network::rawSendListTCP and Network::sendListUDP)
 * to all connected clients and for receiving messages (from all of them).
 * There is only one instance of this class. Its involved into the connection
 * and disconnection procedures (which are quite complicated, handled in the
 * respondToConnectionRequest() and respondToConnectionRequestDone() methods).
 * Additionally messages 'prioritized' (in sense of bypassing the the traditional
 * rawSendListTCP and sendListTCP) can be set, to let the SendRecvThread invoke
 * a topology regarding part of the network protocoll, see handlePrioritizedMsgs()
 * for details (in fact we have only one case so far, which occures whenever someone
 * tries to enter the network by asking our server, see handleConnectionRequestPrioritizedMsg()).
 * Allmost everything is done in the run() method. kill() sets the shutdown member to true and
 * cleanup() does nothing.
 */
class SendReceiveThread {

public:

	SendReceiveThread();

	void kill();
	
	/**
	 * This method contains the main loop of this thread. It uses a socket selection
	 * to avoid busy waiting or blocking. Whenever a prioritizedMsg is found, it invokes
	 * handlePrioritizedMsgs(). All received messages are handled by receiveMessage().
	 * In order to minimize synchronization efforts it makes copy of the working set of
	 * the connections at the beginning of each iteration (thats the localCopy local variable).
	 * Additionally the rawSendListTCP is appended to the sendListTCP.
	 * This method is also responsible for dealing with lost connections, which are handled in
	 * the killedSocket() method.
	 * @param dummy has type SendReceiveThread, its an instance created by the caller
	 */
	static void run(void* dummy);
	/**
	 * \todo 2009-02-06 ZaJ: I guess this method hasn't been implemented.
	 */
	void dump();
	
	/**
	 * \todo 2009-02-06 ZaJ: I guess this method hasn't been implemented.
	 */
	static void cleanup();

protected:

	/**
	 * This method creates a copy of Networks socketList.
	 * In addition it removes dead sockets from it.
	 * Assumes socketListLock is hold
	 */
	int createLocalCopy(SocketListEntry** asArray);
	
	/**
	 * In case a prioritizedMsg is assigned to a SocketListEntry (by now that means each SocketListEntry
	 * has the same prioritizedMsg) we typically have to stick to a certain order of send and recv steps.
	 * However because blocking operations can be avoided by introducing special
	 * message tags and working together with receiveMessage() we tried to go this way, so expect some
	 * complicated pieces of code ;-)
	 */
	void handlePrioritizedMsgs(SocketListEntry* connections, int entries);
	
	/**
	 * Here the nextMsg members of the internalNetwork->socketList are updated.
	 * We don't use nextMsg members anywhere else but now we have it already and using this
	 * method makes the behaviour of the localCopy more intuitive.
	 * Assumes socketListLock is hold.
	 */
	void adjustNextMsgPointers(SocketListEntry* connections, int entries);
	
	/**
	 * this method searches the sendListTCP for a specific message and returns it index on success
	 * and -1 otherwise.
	 * this method should also work in case multiple instances of the same message (pointers) are in the queue
	 * which is impossible with the current implementation
	 */
	int findIndexOfMsg(NetMessage* msg);
	
	/**
	 * Basically this method simply puts all messages into the recvList (TCP and UDP).
	 * Beside this and as mentioned in the description for handlePrioritizedMsgs() it is
	 * also involved in more complicated procedures (like connection requests).
	 */
	void receiveMessage(NetMessage* msg, SocketListEntry* recvdBy);
	
	/**
	 * This method manages decreasing of the reference counters of a SendListEntry and also releases
	 * its resources if necessary.
	 */
	void decreaseReferenceCounter(SendListEntry* sendListEntry, int idxInSendList,
			NetworkIdentification connectionCausedDec);

	void updateSendListTCP(SocketListEntry* connections, int entries);
	void updateSendListUDP(SendListEntry* &nextUDPMsg, int connections);

	void checkForPrioritizedMessages(SocketListEntry* socketListCopy, int entries);

	void initializeSocketSelection(OSG::SocketSelection& sel, SendListEntry* nextUDPMsg,
			SocketListEntry* socketListCopy, int entries);

	void sendUDPMessage(SendListEntry* &nextUDPMsg, SocketListEntry* socketListCopy, int entries);
	void receiveUDPMessage();

	/**
	 * This method waits for the answers of all partners to the previous sent
	 * connectionRequest. It does this by iterating over all sockets and waiting
	 * for a message with a connectionRequestOkTag or a connectionRequestDenyTag.
	 * If a message with another Tag is received at a socket, the receiveMessage
	 * method is called and the method waits again for the answer. When all answers
	 * arrived, the method checks if every partner accepts the connection request
	 * and sets the connectionGlobalAllowed-member in the Network according
	 * to it. Afterwards it enters a connectionBarrier for synchronisation with the
	 * ServerThread.
	 * @param connections list of socketListEntrys from which an answer is expected
	 * @param entries the number of entries in connections
	 *
	 * @author hbress
	 */
	void handleConnectionRequestPrioritizedMsg(SocketListEntry* connections, int entries);
	/**
	 * This method sends back an answer to a received connectionRequest-message.
	 * If another client is already trying to connect a message with a connectionRequestDenyTag
	 * will be sent back, otherwise a message with a connectionRequestOkTag will be sent.
	 * @param msg pointer to received connectionRequest-message
	 * @param recvdBy SocketListEntry from partner who sent connectionRequest
	 * @author hbress
	 */
	void respondToConnectionRequest(NetMessage* msg, SocketListEntry* recvdBy);

	/**
	 * This method finalizes the connectionRequest. For this it pushes back the socketListEntry
	 * of the new client into the socketList and sets the connectionLocalAllowed-member to true again.
	 * @param msg pointer to received connectionRequest-message
	 * @param recvdBy SocketListEntry from partner who sent connectionRequest
	 * @author hbress
	 */
	void respondToConnectionRequestDone(NetMessage* msg, SocketListEntry* recvdBy);

	/**
	 * Here we try to close a lost socket in a safe way and to reduce the ref counters of
	 * all the messages it would have send.
	 */
	void killedSocket(SocketListEntry* connections, int entries, int idx);
	
	/**
	 * Assumes sendListLock is hold and connection is from the localCopy list
	 */
	SendListEntry* findNextMessageForMe(SocketListEntry* connection, int startIdx);

	volatile bool shutdown;
	static Network* internalNetwork;

	friend class Network;
};

#endif /*_SENDRECEIVETHREAD_H*/
