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

#ifndef _MESSAGEFUNCTIONS_H
#define _MESSAGEFUNCTIONS_H

#include <assert.h>

#include <gmtl/VecOps.h>

#include "DataTypes.h"

#include "NetMessage.h"

namespace msgFunctions {
/**
 * DataType: unsigned int
 */
inline void encode(const unsigned u, NetMessage* msg) {
	msg->putUInt32(u);
} // encode

inline void decode(unsigned& u, NetMessage* msg) {
	u = msg->getUInt32();
} // decode

/**
 * DataType: int
 */
inline void encode(const int i, NetMessage* msg) {
	msg->putInt32(i);
} // encode

inline void decode(int& i, NetMessage* msg) {
	i = msg->getInt32();
} // decode

/**
 * DataType: unsigned short
 */
inline void encode(unsigned short s, NetMessage* msg) {
	msg->putUInt16(s);
} // encode

inline void decode(unsigned short& s, NetMessage* msg) {
	s = msg->getUInt16();
} // decode

/**
 * DataType: short
 */
inline void encode(short s, NetMessage* msg) {
	msg->putInt16(s);
} // encode

inline void decode(short& s, NetMessage* msg) {
	s = msg->getInt16();
} // decode

/**
 * DataType: bool
 */
inline void encode(bool b, NetMessage* msg) {
	if (b)
		msg->putUInt8(1);
	else
		msg->putUInt8(0);
} // encode

inline void decode(bool& b, NetMessage* msg) {
	unsigned u;
	u = msg->getUInt8();
	assert(u == 0 || u == 1);
	if (u)
		b = true;
	else
		b = false;
} // decode

/**
 * DataType: float
 */
inline void encode(float f, NetMessage* msg) {
	msg->putReal32(f);
} // encode

inline void decode(float& f, NetMessage* msg) {
	f = msg->getReal32();
} // decode

/**
 * DataType: double
 */
inline void encode(double d, NetMessage* msg) {
	msg->putUInt64(*((uint64_t*)&d));
} // encode

inline void decode(double& d, NetMessage* msg) {
	uint64_t u = msg->getUInt64();
	d = *((double*)&u);
} // decode


/**
 * DataType: uint64_t
 */
inline void encode(uint64_t u, NetMessage* msg) {
	msg->putUInt64(u);
} // encode

inline void decode(uint64_t& u, NetMessage* msg) {
	u = msg->getUInt64();
} // decode

/**
 * DataType: string
 */
inline void encode(std::string s, NetMessage* msg) {
	msg->putString(s);
} // encode

inline void decode(std::string& s, NetMessage* msg) {
	s = msg->getString();
} // decode

/**
 * DataType: Vec3f
 */
inline void encode(gmtl::Vec3f v, NetMessage* msg) {
	encode(v[0], msg);
	encode(v[1], msg);
	encode(v[2], msg);
} // encode

inline void decode(gmtl::Vec3f& v, NetMessage* msg) {
	decode(v[0], msg);
	decode(v[1], msg);
	decode(v[2], msg);
} // decode

/**
 * DataType: gmtl::Quatf
 */
inline void encode(gmtl::Quatf q, NetMessage* msg) {
	encode(q[0], msg);
	encode(q[1], msg);
	encode(q[2], msg);
	encode(q[3], msg);
} // encode

inline void decode(gmtl::Quatf& q, NetMessage* msg) {
	decode(q[0], msg);
	decode(q[1], msg);
	decode(q[2], msg);
	decode(q[3], msg);
} // decode

/**
 * DataType: TransformationData
 */
inline void encode(TransformationData t, NetMessage* msg) {
	bool ignoreScale = false;
	encode(t.position, msg);
	encode(t.orientation, msg);
	if (t.scale == gmtl::Vec3f(1, 1, 1)) {
		ignoreScale = true;
		encode(ignoreScale, msg);
	} // if
	else {
		encode(ignoreScale, msg);
		encode(t.scale, msg);
		encode(t.scaleOrientation, msg);
	} // else
} // encode

inline void decode(TransformationData& t, NetMessage* msg) {
	bool ignoreScale;
	decode(t.position, msg);
	decode(t.orientation, msg);
	decode(ignoreScale, msg);
	if (!ignoreScale) {
		decode(t.scale, msg);
		decode(t.scaleOrientation, msg);
	} // if
	else {
		t.scale = gmtl::Vec3f(1, 1, 1);
		t.scaleOrientation = gmtl::QUAT_IDENTITYF;
	} // else
} // decode

/**
 * DataType: SensorData
 */
inline void encode(SensorData s, NetMessage* msg) {
	encode(s.position, msg);
	encode(s.orientation, msg);
} // encode

inline void decode(SensorData& s, NetMessage* msg) {
	decode(s.position, msg);
	decode(s.orientation, msg);
} // decode

}// msgFunctions

#endif // _MESSAGEFUNCTIONS_H
