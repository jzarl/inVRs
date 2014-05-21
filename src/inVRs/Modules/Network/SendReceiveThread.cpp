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

#include <string>
#include <string.h>
#include <assert.h>

#include "SendReceiveThread.h"
#include "Network.h"
#include <inVRs/SystemCore/EventManager/EventManager.h>
#include <inVRs/SystemCore/DebugOutput.h>
#include <inVRs/SystemCore/Platform.h>
#include <inVRs/SystemCore/UserDatabase/UserDatabase.h>
#include <inVRs/SystemCore/UserDatabase/UserDatabaseEvents.h>
#include <inVRs/SystemCore/ComponentInterfaces/NetworkInterface.h>

OSG_USING_NAMESPACE

SendReceiveThread::SendReceiveThread() {
	shutdown = false;
}

void SendReceiveThread::kill() {
	shutdown = true;
}

void SendReceiveThread::run(void* dummy) {
	SocketListEntry* socketListCopy = NULL;
	SendListEntry* nextUDPMsg = NULL;
	SendListEntry* nextTCPMsg;
	int socketListEntries = 0;
	int i;
	int idx;
	SocketSelection sel;
	SendReceiveThread* me = (SendReceiveThread*)dummy;
	// TODO: remove debug UserId
	//	unsigned debugLocalUserId = UserDatabase::getLocalUserId();
	// 	internalNetwork = dynamic_cast<Network*>(UserDatabase::getLocalUser()->getModuleByName("Network"));

	printd(INFO, "SendReceiveThread::run(): entering method\n");

	assert(internalNetwork->sendListLock != NULL);
	assert(internalNetwork->socketListLock != NULL);

	while (!me->shutdown) {
#if OSG_MAJOR_VERSION >= 2
		internalNetwork->sendListLock->acquire();
		internalNetwork->socketListLock->acquire();
#else //OpenSG1:
		internalNetwork->sendListLock->aquire();
		internalNetwork->socketListLock->aquire();
#endif

		// Setting nextMessagePointers in socketList of Network to next messages to send!!!
		if (socketListCopy != NULL) {
			me->adjustNextMsgPointers(socketListCopy, socketListEntries);
		} // if

		// create array of available Sockets
		socketListEntries = me->createLocalCopy(&socketListCopy);
		//		 		printd(INFO, "SendReceiveThread::run(): %i number of connections found!\n", localCopyEntries);

		// fills next messages into matching TCP-sockets in socketListCopy and grabs next UDP-message to send
		me->updateSendListTCP(socketListCopy, socketListEntries);
		me->updateSendListUDP(nextUDPMsg, socketListEntries);
		internalNetwork->socketListLock->release();
		internalNetwork->sendListLock->release();

		// Check if some prioritizedMessages have to be sent!!
		me->checkForPrioritizedMessages(socketListCopy, socketListEntries);

		//		printd(INFO, "SendReceiveThread::run(): FOUND %i socketListEntries!\n", socketListEntries);

		//printd("SendReceiveThread::run(): falling into sleep ...\n");
		//sleep(5);
		//printd("SendReceiveThread::run(): im awake\n");
		try {
			// now we are setting up a socket selection which includes
			// all sockets which have a message to deliver.
			me->initializeSocketSelection(sel, nextUDPMsg, socketListCopy, socketListEntries);

			// check if we are allowed to send a udp message if there is one available
			if (nextUDPMsg && sel.isSetWrite(internalNetwork->socketUDP))
				me->sendUDPMessage(nextUDPMsg, socketListCopy, socketListEntries);

			// watch out for incoming udp messages
			if (sel.isSetRead(internalNetwork->socketUDP))
				me->receiveUDPMessage();

			for (i = 0; i < socketListEntries; i++) {
				// check if there is any incoming tcp message
				if (socketListCopy[i].socketTCP && sel.isSetRead(*(socketListCopy[i].socketTCP))) {
					// 					printd("SendReceiveThread::run(): receiving a tcp message from connection %d\n", i);
					// we found out that a lost connection typically results in a successfull read operation with 0 bytes available for reading.
					if (!socketListCopy[i].socketTCP->getAvailable()) {
						printd(INFO, "SendReceiveThread::run(): socket died during read\n");
						me->killedSocket(socketListCopy, socketListEntries, i);
						continue;
					}
					internalNetwork->dumpConnection(&socketListCopy[i]);
					//localCopy[i].socketTCP->recv(recvMsg);
					//ExtendedBinaryMessage* copy = new ExtendedBinaryMessage(recvMsg);
					NetMessage* copy = new NetMessage();
					Network::receiveNetMessage(socketListCopy[i].socketTCP, copy);
					me->receiveMessage(copy, &socketListCopy[i]);
					//me->receiveMessage(copy, &localCopy[i]);
				}
				// check if socket is wraitable and whether we have something to deliver
				if (socketListCopy[i].socketTCP && socketListCopy[i].nextMsg && sel.isSetWrite(
						*(socketListCopy[i].socketTCP))) {
					// 					getMessageString(localCopy[i].nextMsg->msg);
					//					localCopy[i].socketTCP->send(*(localCopy[i].nextMsg->msg));
					// TODO: remove following three debug outputs!!!
					//					unsigned debugPort = socketListCopy[i].id.netId.address.portTCP;
					//					unsigned debugUserId = socketListCopy[i].id.userId;
					//					printd(INFO, "SendReceiveThread::run(): sending tcp-message to user %u at port %u\n", debugUserId, debugPort);
					Network::sendNetMessage(socketListCopy[i].socketTCP,
							socketListCopy[i].nextMsg->msg);
					// 					printd("SendReceiveThread::run(): sending a tcp message to connection %d\n", i);
#if OSG_MAJOR_VERSION >= 2
					internalNetwork->sendListLock->acquire();
#else //OpenSG1:
					internalNetwork->sendListLock->aquire();
#endif
					// find next message to send for this connection
					idx = me->findIndexOfMsg(socketListCopy[i].nextMsg->msg);
					// check if it was the last message (nextMsg was at the end of the queue)
					// 						if(idx<(int)internalNetwork->sendListTCP.size()-1)
					// 						{
					// 							// no, get next message
					// 							nextTCPMsg = internalNetwork->sendListTCP[idx+1];
					//
					// 						} else
					// 						{
					// 							// yes, new messages will be assigned during fillSendList
					// 							nextTCPMsg = NULL;
					// 						}

					// we have got an new method doing this now:
					nextTCPMsg = me->findNextMessageForMe(&socketListCopy[i], idx + 1);

					// in any case we have to decrease the reference counter of the sent message
					// and to delete it in case it goes to zero
					me->decreaseReferenceCounter(socketListCopy[i].nextMsg, idx,
							socketListCopy[i].id.netId);
					socketListCopy[i].nextMsg = nextTCPMsg;
					internalNetwork->sendListLock->release();
				}
			}
		} catch (SocketConnReset &e) {
			printd(ERROR, "SendReceiveThread::run(): lost connection to a socket, error: %s\n",
					e.what());
		} catch (SocketException &e) {
			printd(ERROR, "SendReceiveThread::run(): network error: %s\n", e.what());
		}
	}

#if OSG_MAJOR_VERSION >= 2
	BarrierRefPtr cleanupBarrier = dynamic_pointer_cast<OSG::Barrier> (ThreadManager::the()->getBarrier(
				"cleanupBarrier",false));	
#else //OpenSG1:
	Barrier* cleanupBarrier = dynamic_cast<Barrier*> (ThreadManager::the()->getBarrier(
				"cleanupBarrier"));
#endif
	printd(INFO, "SendReceiveThread::run(): entering cleanupBarrier ...\n");
	cleanupBarrier->enter(3);
	printd(INFO, "SendReceiveThread::run(): leaving method\n");
} // SendReceiveThread.run()

int SendReceiveThread::createLocalCopy(SocketListEntry** asArray) {
	int ret = internalNetwork->socketList.size();
	int i;
	if (*asArray != NULL)
		delete[] *asArray;
	*asArray = new SocketListEntry[ret];
	memset(*asArray, 0, sizeof(SocketListEntry*) * ret);
	for (i = 0; i < ret; i++) {
		// Removing SocketListEntry for died socket 
		if (internalNetwork->socketList[i]->socketTCP == NULL) {
			SocketListEntry* entry = internalNetwork->socketList[i];
			internalNetwork->socketList.erase(internalNetwork->socketList.begin() + i);
			delete entry;
			i--;
			ret--;
			continue;
		} // if
		(*asArray)[i] = *internalNetwork->socketList[i];
		if ((*asArray)[i].prioritizedMsg != NULL) // reseting of prioritizedMsg
			internalNetwork->socketList[i]->prioritizedMsg = NULL;
	}
	return ret;
}

void SendReceiveThread::handlePrioritizedMsgs(SocketListEntry* connections, int entries) {
	int i;
	UInt32 tag;

	printd(INFO, "SendReceiveThread::handlePrioritizedMsgs(): sending some prioritizedMsg\n");
	// first we send the prioritizedMsg to every other client

	// check the tag to decide what kind of response we have to expect
	connections[0].prioritizedMsg->getUInt32(tag);

	for (i = 0; i < entries; i++) {
		try {
			//connections[i].socketTCP->send(*(connections[i].prioritizedMsg));
			Network::sendNetMessage(connections[i].socketTCP, connections[i].prioritizedMsg);
		} catch (SocketException &e) {
			printd(ERROR, "SendReceiveThread::handlePrioritizedMsgs(): network error: %s\n",
					e.what());
		}
	}

	if (tag == internalNetwork->connectionRequestTag) {
		// a client wants to connect to our ServerThread
		// we wait until we get a OK or DENY from all our connections
		// see Handshake.cpp
		handleConnectionRequestPrioritizedMsg(connections, entries);
	} else if (tag == internalNetwork->connectionRequestDoneTag) {
		// thats the second part of a connection request
		// the only thing we have to do here is to wait until the ServerThread has completed
		// the handshake procedure with the client who tries to connect to us
#if OSG_MAJOR_VERSION >= 2
		BarrierRefPtr connectionBarrier = OSG::dynamic_pointer_cast<OSG::Barrier> (ThreadManager::the()->getBarrier(
					"connectionBarrier",false));	
#else //OpenSG1:
		Barrier* connectionBarrier = dynamic_cast<Barrier*> (ThreadManager::the()->getBarrier(
					"connectionBarrier"));
#endif
		printd(INFO,
				"SendReceiveThread::handlePrioritizedMsgs(): sended connectionRequestDoneTag\n");
		printd(INFO, "SendReceiveThread::handlePrioritizedMsgs(): entering connectionBarrier\n");
		connectionBarrier->enter(2);
		printd(INFO, "SendReceiveThread::handlePrioritizedMsgs(): connectionBarrier passed\n");
	} else {
		printd(WARNING,
				"SendReceiveThread::handlePrioritizedMsgs(): unknown tag %u encountered!\n", tag);
	}

	printd(INFO, "SendReceiveThread::handlePrioritizedMsgs(): deleting all prioritizedMsges\n");

	// there is only one instance of prioritizedMsg at the moment
	delete connections[0].prioritizedMsg;
	for (i = 0; i < entries; i++) {
		connections[i].prioritizedMsg = NULL;
	}

	printd(INFO, "SendReceiveThread::handlePrioritizedMsgs(): everything done!\n");
}

void SendReceiveThread::adjustNextMsgPointers(SocketListEntry* connections, int entries) {
	int i;

	for (i = 0; i < entries; i++) {
		internalNetwork->socketList[i]->nextMsg = connections[i].nextMsg;
	}
}

int SendReceiveThread::findIndexOfMsg(NetMessage* msg) {
	int i;
	SendListEntry* entry;

	for (i = 0; i < (int)internalNetwork->sendListTCP.size(); i++) {
		entry = internalNetwork->sendListTCP[i];
		if (entry->msg == msg)
			break;
	}
	if (i == (int)internalNetwork->sendListTCP.size()) {
		printd(
				WARNING,
				"SendReceiveThread::findIndexOfMsg(): message does not exist in sendList. check the ref counters!\n");
		return -1;
	}
	return i;
}

void SendReceiveThread::receiveMessage(NetMessage* msg, SocketListEntry* recvdBy) {
	uint32_t tag;
	uint8_t channelID;
	std::string ipAddressStr;

	// for debugging only:
	if (recvdBy)
		ipAddressStr = internalNetwork->ipAddressToString(recvdBy->id.netId.address.ipAddress);
	else
		ipAddressStr = "some udp source";

	msg->getUInt32(tag);

	// check tag to decide if it is a normal message
	if (tag == internalNetwork->connectionRequestTag) {
		// someone wants to connect to the network (NOT to us)
		// basically that means to tell the ServerThread that no connections are accepted until
		// the connection procedure is finished
		printd(INFO,
				"SendReceiveThread::receiveMessage(): received connectionRequestTag-msg from %s\n",
				ipAddressStr.c_str());
		assert(recvdBy != NULL);
		// do not accept special tags via udp
		respondToConnectionRequest(msg, recvdBy);
		return;
	} else if (tag == internalNetwork->connectionRequestDoneTag) {
		// connection procedure on some other machine has been completed
		printd(
				INFO,
				"SendReceiveThread::receiveMessage(): received connectionRequestTagDoneTag-msg from %s\n",
				ipAddressStr.c_str());
		respondToConnectionRequestDone(msg, recvdBy);
		return;
	}

	channelID = msg->getUInt8();

	//	printd("SendReceiveThread::receiveMessage(): received a normal message from %s with tag %u for channel %u\n", ipAddressStr.c_str(), tag, channelID);
#if OSG_MAJOR_VERSION >= 2
	internalNetwork->recvListLock->acquire();
#else //OpenSG1:
	internalNetwork->recvListLock->aquire();
#endif
	if (internalNetwork->recvList[channelID] == NULL)
		internalNetwork->recvList[channelID] = new std::deque<NetMessage*>;
	internalNetwork->recvList[channelID]->push_back(msg);
	internalNetwork->recvListLock->release();
}

void SendReceiveThread::decreaseReferenceCounter(SendListEntry* sendListEntry, int idxInSendList,
		NetworkIdentification connectionCausedDec) {
	if (sendListEntry == NULL) {
		printd(
				WARNING,
				"SendReceiveThread::decreaseReferenceCounter(): sendListEntry is NULL and idxInSendList is %u\n",
				idxInSendList);
		return;
	}
	// 	if(sendListEntry->refCounter <= 0)
	// 	{
	// 		printd(WARNING, "SendReceiveThread::decreaseReferenceCounter(): reference counter with index %u went to or below zero (%d)\n", idxInSendList, sendListEntry->refCounter);
	// 		return;
	// 	}
	// 	printd("SendReceiveThread::decreaseReferenceCounter(): reducing refCounter from %d to %d\n", sendListEntry->refCounter, sendListEntry->refCounter-1);

	if (!sendListEntry->removeFromDestination(&connectionCausedDec)) {
		printd(ERROR,
				"SendReceiveThread::decreaseReferenceCounter(): called on wrong connection!\n");
		return;
	}
	// 	sendListEntry->refCounter--;

	if (sendListEntry->destinationList.size() == 0) {
		// releasing memory and removing entry from sendListTCP (therefore we needed idxInSendList)
		// 		printd("SendReceiveThread::decreaseReferenceCounter(): refcounter == 0 -> deleting message ...\n");
		internalNetwork->sendListTCP.erase(internalNetwork->sendListTCP.begin() + idxInSendList);
		// 		printd("SendReceiveThread::decreaseReferenceCounter(): deleting binary msg ... ");
		// 		delete sendListEntry->msg;
		// 		sendListEntry->msg = NULL;
		// 		printd(" deleting SendListEntry ... ");
		delete sendListEntry;
		// 		printd(" done!\n");
	}
}

void SendReceiveThread::updateSendListTCP(SocketListEntry* connections, int entries) {
	int i, j, k;
	SendListEntry* sendListEntry;

	assert(entries == (int)internalNetwork->socketList.size());

	if (entries == 0) {
		// nobody is connected, remove all messages:
		for (i = 0; i < (int)internalNetwork->sendListTCP.size(); i++) {
			sendListEntry = internalNetwork->sendListTCP[i];
			delete sendListEntry;
		} // for
		internalNetwork->sendListTCP.clear();
		return;
	} // if

	// remove dead connections from reference counter list:
	for (i = 0; i < (int)internalNetwork->sendListTCP.size();) {
		sendListEntry = internalNetwork->sendListTCP[i];
		for (j = sendListEntry->destinationList.size() - 1; j >= 0; j--) {
			for (k = 0; k < entries; k++)
				if (Network::IDequalsID(&connections[k].id.netId,
						&sendListEntry->destinationList[j]))
					break;

			if (k == entries) {
				std::string connDump;
				connDump = internalNetwork->dumpNetworkIdentification(
						&sendListEntry->destinationList[j]);
				printd(INFO,
						"SendReceiveThread::fillSendList(): message for %s cannot be delivered\n",
						connDump.c_str());
				sendListEntry->destinationList.erase(sendListEntry->destinationList.begin() + j);
			} // if
		} // for

		// an orphaned message? If so, delete it!
		if (sendListEntry->destinationList.size() == 0) {
			printd(INFO, "SendReceiveThread::fillSendList(): message hasn't any destination!\n");
			internalNetwork->sendListTCP.erase(internalNetwork->sendListTCP.begin() + i);
			continue;
		} // if

		// check if there is a connection idle (where nextMsg == NULL)
		// in that case this connection gets some more work
		for (j = 0; j < entries; j++) {
			if ((connections[j].nextMsg == NULL) && (sendListEntry->isMsgFor(
					&connections[j].id.netId))) {
				// 				printd("SendReceiveThread::fillSendList(): adding new message to connection %d\n", j);
				connections[j].nextMsg = sendListEntry;
			} // if
		} // for

		i++;
	} // for

	// onyl for debugging, can be removed as soon as the new network has proven its stability :-)
	internalNetwork->checkSocketAndSendListConsistency(connections, entries);
}

void SendReceiveThread::updateSendListUDP(SendListEntry* &nextUDPMsg, int connections) {
	int i, size;
	SendListEntry* msg;

	if (connections == 0) // delete content of udp list if we are connectionless
	{
		size = internalNetwork->sendListUDP.size();
		for (i = 0; i < size; i++) {
			msg = internalNetwork->sendListUDP[0];
			internalNetwork->sendListUDP.erase(internalNetwork->sendListUDP.begin());
			delete msg;
		} // for
		//assert(nextUDPMsg != NULL);
		if (nextUDPMsg)
			delete nextUDPMsg;
		nextUDPMsg = NULL;
	} // if

	// we have only one udp socket, so we don't need a reference counter
	// mechanism. We simply keep the message in nextUDPMsg until the
	// socket selection allowes us to send it.
	// 				printd("SendReceiveThread::run(): found an outgoing udp message\n");
	if (!internalNetwork->sendListUDP.empty() && (nextUDPMsg == NULL)) {
		nextUDPMsg = internalNetwork->sendListUDP[0];
		internalNetwork->sendListUDP.pop_front();
	} // if

} // updateSendListUDP

void SendReceiveThread::checkForPrioritizedMessages(SocketListEntry* socketListCopy, int entries) {
	int i;
	bool prioritizedMsgFound = false;

	// 	printd("SendReceiveThread::checkForPrioritizedMessages(): checking prioritizedMsgs\n");
	for (i = 0; i < entries; i++) {
		if (socketListCopy[i].prioritizedMsg) {
			prioritizedMsgFound = true;
			break;
		} // if
	} // for

	if (prioritizedMsgFound) {
		printd(INFO, "checkForPrioritizedMessages::run(): got a prioritizedMsg!\n");
		assert(i==0);
		handlePrioritizedMsgs(socketListCopy, entries);
	} // if
} // checkForPrioritizedMessages

void SendReceiveThread::initializeSocketSelection(SocketSelection& sel, SendListEntry* nextUDPMsg,
		SocketListEntry* socketListCopy, int entries) {
	int i;

	sel.clear();
	// 			printd("SendReceiveThread::run(): assembling socket selection ..\n");
	sel.setRead(internalNetwork->socketUDP);
	if (nextUDPMsg != NULL)
		sel.setWrite(internalNetwork->socketUDP);

	for (i = 0; i < entries; i++) {
		try {
			sel.setRead(*(socketListCopy[i].socketTCP));
		} catch (...) {
			printd(INFO, "SendReceiveThread::run(): caught exception during selection assembly\n");
			killedSocket(socketListCopy, entries, i);
			//TODO: inform SystemCore that user with matching socket disconnected!!!
		} // catch

		if (socketListCopy[i].nextMsg != NULL)
			sel.setWrite(*(socketListCopy[i].socketTCP));
	} // for

	sel.select(0.01);
} // initializeSocketSelection

void SendReceiveThread::sendUDPMessage(SendListEntry* &nextUDPMsg, SocketListEntry* socketListCopy,
		int entries) {
	int i, j;
	//	std::string destinationIP;
	//	unsigned destinationPort;

	// 	printd("SendReceiveThread::sendUDPMessage(): sending udp message to %i host(s)\n", (int)nextUDPMsg->destinationList.size());
	for (i = 0; i < (int)nextUDPMsg->destinationList.size(); i++) {
		for (j = 0; j < entries; j++) {
			if (Network::IDequalsID(&socketListCopy[j].id.netId, &nextUDPMsg->destinationList[i]))
				break;
		} // for

		if (j == entries) {
			printd(WARNING,
					"SendReceiveThread::sendUDPMessage(): unknown udp destination encountered (lost connection?)!");
			continue;
		} // if

		//		destinationIP = NetworkInterface::ipAddressToString(socketListCopy[j].id.netId.address.ipAddress);
		//		destinationPort = socketListCopy[j].id.netId.address.portUDP;
		//		printd(INFO, "SendReceiveThread::sendUDPMessage(): sending message to %s:%u\n", destinationIP.c_str(), destinationPort);

		Network::sendNetMessageTo(&internalNetwork->socketUDP,
				OSG::SocketAddress(NetworkInterface::ipAddressToString(
						socketListCopy[j].id.netId.address.ipAddress).c_str(),
						socketListCopy[j].id.netId.address.portUDP), nextUDPMsg->msg);
	} // for

	delete nextUDPMsg; // will also delete msg member!
	nextUDPMsg = NULL;
} // sendUDPMessage

void SendReceiveThread::receiveUDPMessage() {
	SocketAddress dummySocketAddress;
	NetMessage* copy = new NetMessage();

	// 	printd("SendReceiveThread::receiveUDPMessage(): receiving a udp message\n");
	Network::receiveNetMessageFrom(&internalNetwork->socketUDP, &dummySocketAddress, copy);
	receiveMessage(copy, NULL);
} // receiveUDPMessage

void SendReceiveThread::handleConnectionRequestPrioritizedMsg(SocketListEntry* connections,
		int entries) {
	int i;
	UInt32 responseTag;
	NetMessage* response;
	NetMessage* newMessage;
	bool* connectionAllowedResponse = new bool[entries];
	bool connectionAllowed = true;

	response = new NetMessage();
	for (i = 0; i < entries; i++) {
		bool whatIWanted = false;
		do {
			response->clear();
			try {
				//connections[i].socketTCP->recv(*response);
				Network::receiveNetMessage(connections[i].socketTCP, response);
				response->dump(stderr);
				response->getUInt32(responseTag);
				if ((responseTag != internalNetwork->connectionRequestDenyTag) && (responseTag
						!= internalNetwork->connectionRequestOkTag)) {
					response->reset();
					newMessage = new NetMessage(response);
					printd(
							INFO,
							"SendRecvThread::handleConnectionRequestPrioritizedMsg(): got a nonexpected msg with tag %u while waiting for some specific msg at connection %d\n",
							responseTag, i);
					receiveMessage(newMessage, &connections[i]);
				} else
					whatIWanted = true;
			} catch (SocketException &e) {
				printd(
						ERROR,
						"SendRecvThread::handleConnectionRequestPrioritizedMsg(): network error: %s (occured during connection procedure)!\n",
						e.what());
				whatIWanted = true;
			}
		} while (!whatIWanted);
		printd(
				INFO,
				"SendRecvThread::handleConnectionRequestPrioritizedMsg(): got a response from %d: %u\n",
				i, responseTag);
		connectionAllowedResponse[i] = (responseTag == internalNetwork->connectionRequestOkTag);
		connectionAllowed = connectionAllowedResponse[i] && connectionAllowed;
	}

	printd(
			INFO,
			"SendRecvThread::handleConnectionRequestPrioritizedMsg(): setting connectionGlobalAllowed to %s\n",
			connectionAllowed ? "true" : "false");
#if OSG_MAJOR_VERSION >= 2
	internalNetwork->connectionDisconnectionLock->acquire();
#else //OpenSG1:
	internalNetwork->connectionDisconnectionLock->aquire();
#endif
	internalNetwork->connectionGlobalAllowed = connectionAllowed;
	internalNetwork->connectionDisconnectionLock->release();

#if OSG_MAJOR_VERSION >= 2
	BarrierRefPtr connectionBarrier = OSG::dynamic_pointer_cast<OSG::Barrier> (ThreadManager::the()->getBarrier(
				"connectionBarrier",false));		
#else //OpenSG1:
	Barrier* connectionBarrier = dynamic_cast<Barrier*> (ThreadManager::the()->getBarrier(
				"connectionBarrier"));
#endif

	printd(INFO,
			"SendRecvThread::handleConnectionRequestPrioritizedMsg(): entering barrier the first time\n");
	connectionBarrier->enter(2);

	// connection granted, server does some intialization work

	// 	printd("SendRecvThread::handleConnectionRequestPrioritizedMsg(): entering barrier the second time\n");
	// 	connectionBarrier->enter(2);
	//
	// 	response->clear();
	// 	response->putUInt32(internalNetwork->connectionRequestDoneTag);
	// 	for(i=0;i<entries;i++)
	// 	{
	// 		printd("SendRecvThread::handleConnectionRequestPrioritizedMsg(): sending connectionRequestDoneTag to connection %d\n", i);
	// 		try
	// 		{
	// 			connections[i].socketTCP->send(*response);
	// 		}
	// 		catch (SocketException &e)
	// 		{
	// 			printd("SendRecvThread::handleConnectionRequestPrioritizedMsg(): network error: %s\n", e.what());
	// 			printd("\tduring sending connectionRequestDoneTag to connection %d\n", i);
	// 		}
	// 	}

	printd(INFO, "SendRecvThread::handleConnectionRequestPrioritizedMsg(): everything done!\n");

	delete[] connectionAllowedResponse;
	printd(INFO,
			"SendRecvThread::handleConnectionRequestPrioritizedMsg(): connectionAllowedResponse freed!\n");
	delete response;
	printd(INFO,
			"SendRecvThread::handleConnectionRequestPrioritizedMsg(): response-message freed!\n");
}

void SendReceiveThread::respondToConnectionRequest(NetMessage* msg, SocketListEntry* recvdBy) {
	NetMessage* response;
	//	UInt32 tag;
	//char ipAddressStr[16];

	//internalNetwork->ipAddressToString(recvdBy->id.address.ipAddress, ipAddressStr);
	printd(INFO,
			"SendRecvThread::respondToConnectionRequest(): acquiring connectionDisconnectionLock ...\n");
#if OSG_MAJOR_VERSION >= 2
	internalNetwork->connectionDisconnectionLock->acquire();
#else //OpenSG1:
	internalNetwork->connectionDisconnectionLock->aquire();
#endif
	printd(INFO,
			"SendRecvThread::respondToConnectionRequest(): connectionDisconnectionLock acquired\n");
	response = new NetMessage();
	if (internalNetwork->connectionLocalAllowed)
		response->putUInt32(internalNetwork->connectionRequestOkTag);
	else
		response->putUInt32(internalNetwork->connectionRequestDenyTag);
	internalNetwork->connectionLocalAllowed = false;
	internalNetwork->connectionDisconnectionLock->release();
	try {
		Network::sendNetMessage(recvdBy->socketTCP, response);
		//			recvdBy->socketTCP->send(*response); // sending ok or deny
		// 			response->clear();
		// 			recvdBy->socketTCP->recv(*response); // waiting for connectionRequestDoneTag
		// 			response->getUInt32(tag);
		// 			if(tag != internalNetwork->connectionRequestDoneTag)
		// 			{
		// 				printd("SendRecvThread::respondToConnectionRequest(): received %u but expected connectionRequestDoneTag from %s\n", tag, ipAddressStr);
		// 				assert(false);
		// 			}
	} catch (SocketException &e) {
		printd(ERROR, "SendRecvThread::respondToConnectionRequest(): network error: %s\n", e.what());
	}
	// 		if(!internalNetwork->someoneIsConnecting)
	// 		{
	// 			internalNetwork->connectionLocalAllowed = true;
	// 		} else
	// 		{
	// 			printd("SendRecvThread::respondToConnectionRequest(): there's still someone connecting ..\n");
	// 		}
	delete response;
	// 	internalNetwork->connectionDisconnectionLock->release();
	printd(INFO,
			"SendRecvThread::respondToConnectionRequest(): connectionDisconnectionLock released\n");
}

void SendReceiveThread::respondToConnectionRequestDone(NetMessage* msg, SocketListEntry* recvdBy) {
	std::string ipAddressStr;

	printd(INFO, "SendRecvThread::respondToConnectionRequestDone(): acquiring socketListLock\n");
#if OSG_MAJOR_VERSION >= 2
	internalNetwork->socketListLock->acquire();
#else //OpenSG1:
	internalNetwork->socketListLock->aquire();
#endif
	printd(INFO, "SendRecvThread::respondToConnectionRequestDone(): socketListLock acquired\n");
	if (internalNetwork->newSocketListEntry) {
		internalNetwork->socketList.push_back(internalNetwork->newSocketListEntry);
		ipAddressStr = Network::ipAddressToString(
				internalNetwork->newSocketListEntry->id.netId.address.ipAddress);
		printd(
				INFO,
				"SendRecvThread::respondToConnectionRequestDone(): %s is joinig our connection list!\n",
				ipAddressStr.c_str());
		internalNetwork->newSocketListEntry = NULL;
	} else {
		printd(INFO,
				"SendRecvThread::respondToConnectionRequestDone(): newSocketListEntry is NULL\n");
	}
	internalNetwork->socketListLock->release();

	printd(INFO,
			"SendRecvThread::respondToConnectionRequestDone(): acquiring connectionDisconnectionLock ...\n");
#if OSG_MAJOR_VERSION >= 2
	internalNetwork->connectionDisconnectionLock->acquire();
#else //OpenSG1:
	internalNetwork->connectionDisconnectionLock->aquire();
#endif
	printd(INFO,
			"SendRecvThread::respondToConnectionRequestDone(): connectionDisconnectionLock acquired\n");
	internalNetwork->connectionLocalAllowed = true;
	internalNetwork->connectionDisconnectionLock->release();

}

void SendReceiveThread::killedSocket(SocketListEntry* connections, int entries, int idx) {
	int msgIdx, i;
	printd(INFO, "SendReceiveThread::killedSocket(): lost connection %d\n", idx);
#if OSG_MAJOR_VERSION >= 2
	internalNetwork->socketListLock->acquire();
#else //OpenSG1:
	internalNetwork->socketListLock->aquire();
#endif
	printd(INFO, "SendReceiveThread::killedSocket(): trying to close socket %d ...\n", idx);
	connections[idx].socketTCP->close();
	printd(INFO, "SendReceiveThread::killedSocket(): closing socket %d was successfull\n", idx);
	internalNetwork->socketList[idx]->socketTCP = NULL;
	connections[idx].socketTCP = NULL;
	internalNetwork->socketListLock->release();
#if OSG_MAJOR_VERSION >= 2
	internalNetwork->sendListLock->acquire();
#else //OpenSG1:
	internalNetwork->sendListLock->aquire();
#endif
	printd(INFO, "SendReceiveThread::killedSocket(): adjusting ref counters of messages ...\n");
	if (connections[idx].nextMsg != NULL) {
		msgIdx = findIndexOfMsg(connections[idx].nextMsg->msg);
		printd(INFO, "SendReceiveThread::killedSocket(): findIndexOfMsg() returned %d\n", msgIdx);
		for (i = internalNetwork->sendListTCP.size() - 1; i >= msgIdx; i--) {
			// 				printd("SendReceiveThread::killedSocket(): decreasing ref counter from msg %d\n", i);
			decreaseReferenceCounter(internalNetwork->sendListTCP[i], i, connections[idx].id.netId);
		}
		connections[idx].nextMsg = NULL;
		printd(INFO, "SendReceiveThread::killedSocket(): sendListTCP has %d elements\n",
				internalNetwork->sendListTCP.size());
	}
	internalNetwork->sendListLock->release();

	printd(INFO, "SendReceiveThread::killedSocket(): disconnected user has userId %u\n",
			connections[idx].id.userId);
	User* user = UserDatabase::getUserById(connections[idx].id.userId);
	if (user) {
		UserDatabaseRemoveUserEvent *removeUserEvent = new UserDatabaseRemoveUserEvent(user);
		EventManager::sendEvent(removeUserEvent, EventManager::EXECUTE_LOCAL);
	} // if
}

SendListEntry* SendReceiveThread::findNextMessageForMe(SocketListEntry* connection, int startIdx) {
	int i;
	// 	if(startIdx>=internalNetwork->sendListTCP.size())
	// 	{
	// 		return NULL;
	// 	}

	for (i = startIdx; i < (int)internalNetwork->sendListTCP.size(); i++) {
		if (internalNetwork->sendListTCP[i]->isMsgFor(&connection->id.netId)) {
			return internalNetwork->sendListTCP[i];
		}
	}

	return NULL;
}

Network* SendReceiveThread::internalNetwork = NULL;

