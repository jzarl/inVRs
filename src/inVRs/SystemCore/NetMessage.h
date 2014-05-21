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

#ifndef NETMESSAGE_H_
#define NETMESSAGE_H_

#include <string>
#include <stdio.h>
#include "Platform.h"
#include "DataTypes.h"

//#include "ExtendedBinaryMessage.h"

/******************************************************************************
 * This class holds an ordered set of data (endianes independet).
 * Data can be added by putFirstXXX() or putXXX() at either the beginning or the end of the set.
 * Read access takes place sequentialy (starting from the beginning). When reading data the type has to be known in advance.
 * TODO tweak allocation strategy and/or make it tweakable
 */
class INVRS_SYSTEMCORE_API NetMessage {
public:
	NetMessage();

	/**
	 * copy constructor (sets also the read pointer to the read pointer of src)
	 */
	NetMessage(NetMessage* src);

	virtual ~NetMessage();

	/**
	 * methods for appending/prepending data to the network message:
	 * the readpointer will skip data added by putFirstXXX() unless it is still at the beginnig (or reset() has been invoked)!!!
	 */
	void putUInt8(uint8_t value);
	void putFirstUInt8(uint8_t value);
	void putUInt16(uint16_t value);
	void putFirstUInt16(uint16_t value);
	void putUInt32(uint32_t value);
	void putFirstUInt32(uint32_t value);
	void putString(const std::string &value);
	void putUInt64(uint64_t value);
	void putReal32(float value);
	void putInt32(int value);
	void putInt16(short value);

	/**
	 * methods for retrieving data from the network message.
	 * will increment the read pointer according to the size of the datatype
	 * for now, reading from beyond the buffer triggers an assert
	 */
	uint8_t getUInt8();
	uint16_t getUInt16();
	uint32_t getUInt32();
	std::string getString();
	uint64_t getUInt64();
	float getReal32();
	int getInt32();
	short getInt16();

	void getUInt8(uint8_t& dst);
	void getUInt32(uint32_t& dst);
	void getString(std::string& dst);
	void getReal32(float& dst);

	/**
	 * appends a message to the data set (can only be read by invoking detachMessage())
	 * @param src message which should be appended. src can be modified afterwards
	 */
	void appendMessage(NetMessage* src);

	/**
	 * reads a message which has been added previousely by appendMessage()
	 * @return pointer to message (must be delete'd manually)
	 */
	NetMessage* detachMessage();

	/**
	 * removes all data and resets the read pointer
	 */
	void clear();

	/**
	 * resets the read pointer
	 */
	void reset();

	/**
	 * @return returns true if the readpointer has reached the end of the data set
	 */
	bool finished();

	/**
	 * @return returns a pointer to the beginning of a byte array containing the whole data set
	 * the value might (and will!) change whenever putXXX() or appendMessage() is called
	 */
	uint8_t* getBufferPointer();

	/**
	 * @return returns the size of the data stream
	 */
	unsigned getBufferSize();

	// the follinwg methods have been added with the current network implenentation in mind, but could turn out as usefull also in other cases


	/**
	 * allocates an array of bytes at the end of the data set. This method is used in the network module in order to allow to recv() directly into the buffer.
	 * @param size specifies the size of the array
	 * @return returns a pointer pointing to the beginning of the array. The pointer is only valid before the next putXXX() call has occured.
	 */
	uint8_t* allocateAtEnd(unsigned size);

	/**
	 * works as the method above
	 */
	uint8_t* allocateAtFront(unsigned size);

	/**
	 * removes nBytes from the front of the data set
	 * works only if readpointer is set to zero
	 * @return returns true on success
	 */
	bool removeFromFront(unsigned nBytes);

	/**
	 * @returns the current offset of the read pointer
	 */
	unsigned getReadPointerOffset();

	void dump(FILE* stream);

protected:

	static const unsigned APPENDED_MESSAGE;

	void allocInitial();
	void allocMore();

	uint8_t* mem;
	unsigned memSize;

	uint8_t* beginPlus;
	uint8_t* beginMinus;
	int freePlus;
	int freeMinus;
	unsigned nextPlus;
	unsigned nextMinus;
	unsigned readOffset;
};

INVRS_SYSTEMCORE_API void addTransformationToBinaryMsg(TransformationData* data, NetMessage* dst);
INVRS_SYSTEMCORE_API TransformationData readTransformationFrom(NetMessage* src);
#endif /*NETWORKMESSAGE_H_*/
