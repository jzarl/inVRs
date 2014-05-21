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

#ifndef _OOPSOBJECTID_H
#define _OOPSOBJECTID_H

#include <inVRs/SystemCore/Platform.h>

/**
 */
class OopsObjectID {

public:
	OopsObjectID();
	OopsObjectID(uint64_t);
	OopsObjectID(bool isJoint, uint8_t physicsObjectClass, uint16_t instanceID, uint32_t objectID);

	OopsObjectID& operator=(OopsObjectID id);
	OopsObjectID& operator=(uint64_t src);
	bool operator==(OopsObjectID id);
	bool operator==(uint64_t id);
	bool operator!=(OopsObjectID id);
	bool operator!=(uint64_t id);
	bool operator<(OopsObjectID id);
	bool operator<(uint64_t id);
	bool operator<=(OopsObjectID id);
	bool operator<=(uint64_t id);
	bool operator>(OopsObjectID id);
	bool operator>(uint64_t id);
	bool operator>=(OopsObjectID id);
	bool operator>=(uint64_t id);
	
	uint64_t get();
	bool getIsJoint();
	uint8_t getPhysicsObjectClass();
	uint16_t getInstanceID();
	uint32_t getObjectID();
	
protected:
	bool isJoint;
	uint8_t physicsObjectClass;
	uint16_t instanceID;
	uint32_t objectID;
}; // OopsObjectID

#endif // _OOPSOBJECTID_H

