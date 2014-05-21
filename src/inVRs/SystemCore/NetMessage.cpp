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

#include "NetMessage.h"

#include <assert.h>
#include <memory.h>

#ifndef WIN32
#include <netinet/in.h>
#else
#include <Winsock2.h>
#endif


#include "DebugOutput.h"

const unsigned NetMessage::APPENDED_MESSAGE = 0xFFFFFFFF;

NetMessage::NetMessage() {
	mem = NULL;
	memSize = 0;
	readOffset = 0;
}

NetMessage::NetMessage(NetMessage* src) {
	unsigned i;
	unsigned srcBufferSize;
	uint8_t* srcBuffer;

	mem = NULL;
	memSize = 0;
	readOffset = 0;

	srcBufferSize = src->getBufferSize();
	srcBuffer = src->getBufferPointer();

	for (i = 0; i < srcBufferSize; i++) {
		putUInt8(srcBuffer[i]);
	}

	readOffset = src->readOffset; // see doc for OSG::BinaryMessage
}

NetMessage::~NetMessage() {
	if (mem)
		delete[] mem;
}

void NetMessage::putUInt8(uint8_t value) {
	if (!mem)
		allocInitial();

	if (freePlus == 0) {
		allocMore();
	}

	beginPlus[nextPlus] = value;
	nextPlus++;
	freePlus--;
}

void NetMessage::putFirstUInt8(uint8_t value) {
	if (!mem)
		allocInitial();

	if (freeMinus == 0) {
		allocMore();
	}

	beginMinus[-(int)nextMinus] = value;
	nextMinus++;
	freeMinus--;

	// here NetMessage differs from the ExtendedBinaryMessage:
	// readOffset HAS TO BE INCREMENTED otherwise we might end up reading data twice or even worse data from another type
	if (readOffset > 0)
		readOffset++;
}

void NetMessage::putUInt16(uint16_t value) {
	uint16_t net;
	uint8_t* dummy;
	net = htons(value);
	dummy = (uint8_t*)&net;
	putUInt8(dummy[0]);
	putUInt8(dummy[1]);
}

void NetMessage::putFirstUInt16(uint16_t value) {
	uint16_t net;
	uint8_t* dummy;
	net = htons(value);
	dummy = (uint8_t*)&net;
	putFirstUInt8(dummy[1]);
	putFirstUInt8(dummy[0]);
}

void NetMessage::putUInt32(uint32_t value) {
	uint32_t net;
	uint8_t* dummy;
	net = htonl(value);
	dummy = (uint8_t*)&net;
	putUInt8(dummy[0]);
	putUInt8(dummy[1]);
	putUInt8(dummy[2]);
	putUInt8(dummy[3]);
}

void NetMessage::putFirstUInt32(uint32_t value) {
	uint32_t net;
	uint8_t* dummy;
	net = htonl(value);
	dummy = (uint8_t*)&net;
	putFirstUInt8(dummy[3]);
	putFirstUInt8(dummy[2]);
	putFirstUInt8(dummy[1]);
	putFirstUInt8(dummy[0]);
}

void NetMessage::putString(const std::string &value) {
	unsigned strSize = value.size();
	unsigned i;
	putUInt32(strSize);
	for (i = 0; i < strSize; i++) {
		putUInt8(value.c_str()[i]);
	}
}

void NetMessage::putUInt64(uint64_t value) {
	uint32_t upperPart = value >> 32;
	uint32_t lowerPart = value & 0xFFFFFFFF;
	putUInt32(upperPart);
	putUInt32(lowerPart);
}

void NetMessage::putReal32(float value) {
	// is a float also affected by endianess? OpenSG means yes!
	putUInt32(*((uint32_t*)&value));
}

void NetMessage::putInt32(int value) {
	putUInt32(*((uint32_t*)&value));
}

void NetMessage::putInt16(short value) {
	putUInt16(*((uint16_t*)&value));
}

uint8_t NetMessage::getUInt8() {
	uint8_t ret;
	if (!mem)
		return 0;
	assert(readOffset < getBufferSize());
	ret = *(getBufferPointer() + readOffset);
	readOffset++;
	return ret;
}

uint16_t NetMessage::getUInt16() {
	uint16_t net;
	uint8_t* dummy;
	dummy = (uint8_t*)&net;
	dummy[0] = getUInt8();
	dummy[1] = getUInt8();
	return ntohs(net);
}

uint32_t NetMessage::getUInt32() {
	uint32_t net;
	uint8_t* dummy;
	dummy = (uint8_t*)&net;
	dummy[0] = getUInt8();
	dummy[1] = getUInt8();
	dummy[2] = getUInt8();
	dummy[3] = getUInt8();
	return ntohl(net);
}

std::string NetMessage::getString() {
	unsigned strSize = getUInt32();
	unsigned i;
	char cstr[2];
	cstr[1] = '\0';
	std::string ret;
	for (i = 0; i < strSize; i++) {
		cstr[0] = getUInt8();
		ret.append(cstr);
	}
	return ret;
}

uint64_t NetMessage::getUInt64() {
	uint32_t upperPart;
	uint32_t lowerPart;
	uint64_t result;
	upperPart = getUInt32();
	lowerPart = getUInt32();
	result = upperPart;
	result = result << 32;
	result |= lowerPart;
	return result;
}

float NetMessage::getReal32() {
	uint32_t dummy = getUInt32();
	return *((float*)&dummy);
}

int NetMessage::getInt32() {
	uint32_t dummy = getUInt32();
	return *((int*)&dummy);
}

short NetMessage::getInt16() {
	uint16_t dummy = getUInt16();
	return *((short*)&dummy);
}

void NetMessage::getUInt8(uint8_t& dst) {
	dst = getUInt8();
}

void NetMessage::getUInt32(uint32_t& dst) {
	dst = getUInt32();
}

void NetMessage::getReal32(float& dst) {
	dst = getReal32();
}

void NetMessage::getString(std::string& dst) {
	dst = getString();
}

void NetMessage::appendMessage(NetMessage* src) {
	unsigned i;
	unsigned srcBufferSize;
	uint8_t* srcBuffer;

	srcBufferSize = src->getBufferSize();
	srcBuffer = src->getBufferPointer();

	putUInt32(APPENDED_MESSAGE);
	putUInt32(srcBufferSize);

	// do not use getUint8() here -> that would affect the readOffset!!!
	for (i = 0; i < srcBufferSize; i++) {
		putUInt8(srcBuffer[i]);
	}
}

NetMessage* NetMessage::detachMessage() {
	unsigned int i, size, type;
	NetMessage* ret;

	type = getUInt32();
	assert(type == APPENDED_MESSAGE);

	ret = new NetMessage();

	size = getUInt32();
	for (i = 0; i < size; i++) {
		ret->putUInt8(getUInt8());
	}

	return ret;
}

void NetMessage::clear() {
	if (!mem)
		return;

	beginPlus = mem + memSize / 2;
	beginMinus = beginPlus - 1;
	freePlus = memSize / 2;
	freeMinus = memSize / 2;
	nextPlus = 0;
	nextMinus = 0;
	readOffset = 0;
} // clear

void NetMessage::reset() {
	readOffset = 0;
}

bool NetMessage::finished() {
	return readOffset >= getBufferSize();
}

uint8_t* NetMessage::getBufferPointer() {
	if (!mem)
		return NULL;

	return &beginMinus[-(int)nextMinus + 1]; // +1: see initialAlloc()
}

unsigned NetMessage::getBufferSize() {
	return nextMinus + nextPlus;
}

uint8_t* NetMessage::allocateAtFront(unsigned size) {
	uint8_t* ret;

	if (!mem)
		allocInitial();

	while (freeMinus < (int)size) {
		allocMore();
	}

	ret = &beginMinus[-(int)((nextMinus + size - 1))];
	nextMinus += size;
	freeMinus -= size;

	return ret;
}

uint8_t* NetMessage::allocateAtEnd(unsigned size) {
	uint8_t* ret;

	if (!mem)
		allocInitial();

	while (freePlus < (int)size) {
		allocMore();
	}

	ret = beginPlus + nextPlus;

	nextPlus += size;
	freePlus -= size;

	return ret;
}

bool NetMessage::removeFromFront(unsigned nBytes) {
	if (readOffset > 0)
		return false;

	if (nextMinus > nBytes) {
		nextMinus -= nBytes;
		return true;
	}

	// TODO: implement that case
	// a memcopy is inevitable, so hopefully we never run into that situation :-(
	assert(false);
	return true;
}

unsigned NetMessage::getReadPointerOffset() {
	return readOffset;
}

void NetMessage::dump(FILE* stream) {
	unsigned i, buffSize;

	buffSize = getBufferSize();
	fprintf(stream, "NetMessage::dump(): size is %u, content:\n", buffSize);
	for (i = 0; i < buffSize; i++) {
		fprintf(stream, "%.2X", *(getBufferPointer() + i));
	}
	fprintf(stream, "\n");
	for (i = 0; i < buffSize; i++) {
		fprintf(stream, "%c", *(getBufferPointer() + i));
	}
	fprintf(stream, "\n");
}

void NetMessage::allocInitial() {
	assert(mem == NULL);

	memSize = 256;
	mem = new uint8_t[memSize];

	beginPlus = mem + memSize / 2;
	beginMinus = beginPlus - 1;
	freePlus = memSize / 2;
	freeMinus = memSize / 2;
	nextPlus = 0;
	nextMinus = 0;
	readOffset = 0;
}

void NetMessage::allocMore() {
	uint8_t* newMem;
	unsigned newMemSize;
	uint8_t* newBeginPlus;
	uint8_t* newBeginMinus;
	int newFreePlus;
	int newFreeMinus;

	newMemSize = memSize * 2;
	newMem = new uint8_t[newMemSize];

	newBeginPlus = newMem + newMemSize / 2;
	newBeginMinus = newBeginPlus - 1;
	newFreePlus = newMemSize / 2 - nextPlus;
	newFreeMinus = newMemSize / 2 - nextMinus;
	memcpy(newBeginMinus - nextMinus + 1, getBufferPointer(), getBufferSize());

	delete[] mem;
	mem = newMem;
	memSize = newMemSize;
	beginPlus = newBeginPlus;
	beginMinus = newBeginMinus;
	freePlus = newFreePlus;
	freeMinus = newFreeMinus;
}

INVRS_SYSTEMCORE_API void addTransformationToBinaryMsg(TransformationData* data, NetMessage* dst) {
	int i;
	uint32_t temp[64];

	for (i = 0; i < 3; i++) {
		dst->putUInt32(*((uint32_t*)&data->position[i]));
		dst->putUInt32(*((uint32_t*)&data->scale[i]));
	}

	data->orientation.get(*((float*)(&temp[0])), *((float*)(&temp[1])), *((float*)(&temp[2])),
			*((float*)(&temp[3])));

	for (i = 0; i < 4; i++)
		dst->putUInt32(temp[i]);

	data->scaleOrientation.get(*((float*)(&temp[0])), *((float*)(&temp[1])), *((float*)(&temp[2])),
			*((float*)(&temp[3])));

	for (i = 0; i < 4; i++) {
		dst->putUInt32(temp[i]);
	}
}

INVRS_SYSTEMCORE_API TransformationData readTransformationFrom(NetMessage* src) {
	// TODO: testing if everything works without the following variable!!!!
	// 	static unsigned int foo=0;
	uint32_t temp[16];
	int i;
	TransformationData ret;

	for (i = 0; i < 6; i++)
		src->getUInt32(temp[i]);

	ret.position = gmtl::Vec3f(*((float*)&temp[0]), *((float*)&temp[2]), *((float*)&temp[4]));
	ret.scale = gmtl::Vec3f(*((float*)&temp[1]), *((float*)&temp[3]), *((float*)&temp[5]));

	for (i = 0; i < 4; i++)
		src->getUInt32(temp[i]);

	// 	if ( foo % 500 == 0)
	// 		printf("temp: %u, %u, %u, %u\n",temp[0],temp[1],temp[2],temp[3]);

	for (i = 0; i < 4; i++)
		ret.orientation[i] = *((float*)&temp[i]);

	// 	if ( foo == 0)
	// 		printf("temp: %u, %u, %u, %u\n",temp[0],temp[1],temp[2],temp[3]);

	for (i = 0; i < 4; i++)
		src->getUInt32(temp[i + 4]);

	// 	if ( foo % 500 == 0)
	// 		printf("temp: %u, %u, %u, %u\n",temp[0],temp[1],temp[2],temp[3]);
	// 	foo++;

	// we had problems with gcc 4.1.1 when using a already initialized index of temp (behaviour depends on optimization flags, maybe a compiler bug???)
	for (i = 0; i < 4; i++)
		ret.scaleOrientation[i] = *((float*)&temp[i + 4]);

	return ret;
}
