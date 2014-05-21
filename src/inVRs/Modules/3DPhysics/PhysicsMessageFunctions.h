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

#ifndef _PHYSICSMESSAGEFUNCTIONS_H
#define _PHYSICSMESSAGEFUNCTIONS_H

#include <assert.h>

#include <inVRs/SystemCore/MessageFunctions.h>

#include "PhysicsObjectID.h"
#include "OopsObjectID.h"
#include "UserData.h"

namespace msgFunctions
{
//***************************
// DataType: PhysicsObjectID
//***************************
	inline void encode(PhysicsObjectID id, NetMessage* msg)
	{
		msg->putUInt32(id.getTypeID());
		msg->putUInt32(id.getInstanceID());
	} // encode

	inline void encodeFirst(PhysicsObjectID id, NetMessage* msg)
	{
		msg->putFirstUInt32(id.getInstanceID());
		msg->putFirstUInt32(id.getTypeID());
	} // encodeFirst

	inline void decode(PhysicsObjectID& id, NetMessage* msg)
	{
		unsigned typeId = msg->getUInt32();
		unsigned instanceId = msg->getUInt32();
		id = PhysicsObjectID(typeId, instanceId);
	} // decode

//***********************
// DataType: OopsObjectID
//***********************
	inline void encode(OopsObjectID id, NetMessage* msg)
	{
		msg->putUInt64(id.get());
	} // encode

	inline void decode(OopsObjectID& id, NetMessage* msg)
	{
		id = OopsObjectID(msg->getUInt64());
	} // decode

//********************
// DataType: UserData
//********************
	inline void encode(UserData* userData, NetMessage* msg)
	{
		encode(userData->doesDeadReckoning, msg);
		encode(userData->isServer, msg);
	} // encode

	inline void decode(UserData* userData, NetMessage* msg)
	{
		decode(userData->doesDeadReckoning, msg);
		decode(userData->isServer, msg);
	} // decode

}; // msgFunctions

#endif // _PHYSICSMESSAGEFUNCTIONS_H
