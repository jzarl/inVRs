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

#include "NetworkInterface.h"

#include <assert.h>

#include "../DebugOutput.h"


void NetworkInterface::registerConnectionClosedCallback(CLIENTDISCONNECTEDCALLBACK callback,
		unsigned userId, void* whateveryouwant) {
	onDisconectCallbacks.push_back(callback);
	onDisconectCallbackArgs.push_back(whateveryouwant);
}

void NetworkInterface::onConnectionClosed(unsigned userId) {
	int i;

	printd(INFO, "NetworkInterface::onConnectionClosed(): lost connection to user %u\n", userId);

	for (i = 0; i < (int)onDisconectCallbacks.size(); i++) {
		onDisconectCallbacks[i](userId, onDisconectCallbackArgs[i]);
	}
}

std::string NetworkInterface::ipAddressToString(uint32_t ip) {
	unsigned char bytes[4];
	char buffer[64];

//	bytes[0] = (unsigned char)ip;
//	bytes[1] = (unsigned char)(ip >> 8);
//	bytes[2] = (unsigned char)(ip >> 16);
//	bytes[3] = (unsigned char)(ip >> 24);
	// Downcast to unsigned char with 0xFF because of errors on windows systems
	bytes[0] = 0xFF & ip;
	bytes[1] = 0xFF & (ip >> 8);
	bytes[2] = 0xFF & (ip >> 16);
	bytes[3] = 0xFF & (ip >> 24);

	sprintf(buffer, "%u.%u.%u.%u", bytes[3], bytes[2], bytes[1], bytes[0]);
	return std::string(buffer);
}

uint32_t NetworkInterface::ipAddressFromString(std::string ip) {
	int pos = 0;
	uint32_t dst;
	int nextPos = ip.find('.');

	dst = atoi(ip.substr(pos, nextPos).c_str()) << 24;

	pos = nextPos + 1;
	nextPos = ip.find('.', pos);
	dst += atoi(ip.substr(pos, nextPos).c_str()) << 16;

	pos = nextPos + 1;
	nextPos = ip.find('.', pos);
	dst += atoi(ip.substr(pos, nextPos).c_str()) << 8;

	pos = nextPos + 1;
	//	nextPos = ip.size();
	dst += atoi((ip.c_str()) + pos);

	return dst;
}

void NetworkInterface::addNetworkIdentificationToBinaryMessage(NetworkIdentification* netId,
		NetMessage* dst) {
	assert(dst);
	dst->putUInt32(netId->address.ipAddress);
	dst->putUInt16(netId->address.portTCP);
	dst->putUInt16(netId->address.portUDP);
	dst->putUInt32(netId->processId);
}

NetworkIdentification NetworkInterface::readNetworkIdentificationFrom(NetMessage* src) {
	NetworkIdentification ret;
	assert(src);
	ret.address.ipAddress = src->getUInt32();
	ret.address.portTCP = src->getUInt16();
	ret.address.portUDP = src->getUInt16();
	ret.processId = src->getUInt32();
	return ret;
}
