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

#include "PhysicsObjectID.h"

#define UNDEFINED_ID 0xFFFFFFFF

//******************//
// PUBLIC METHODS: *//
//******************//

PhysicsObjectID::PhysicsObjectID() : typeID(UNDEFINED_ID), instanceID(UNDEFINED_ID)
{
} // PhysicsObjectID

PhysicsObjectID::PhysicsObjectID(unsigned int classTypeID, unsigned int classInstanceID)
: typeID(classTypeID), instanceID(classInstanceID)
{
} // PhysicsObjectID

unsigned int PhysicsObjectID::getTypeID()
{
	return typeID;
} // getTypeID

unsigned int PhysicsObjectID::getInstanceID()
{
	return instanceID;
} // getInstanceID

PhysicsObjectID& PhysicsObjectID::operator=(PhysicsObjectID id)
{
	this->typeID = id.typeID;
	this->instanceID = id.instanceID;
	return *this;
} // operator=

bool PhysicsObjectID::operator==(PhysicsObjectID& id)
{
	if (this->typeID == id.typeID && this->instanceID == id.instanceID)
		return true;
	return false;
} // operator==

bool PhysicsObjectID::operator<(PhysicsObjectID& id)
{
	if (this->typeID < id.typeID)
		return true;
	if (this->typeID > id.typeID)
		return false;
	if (this->instanceID < id.instanceID)
		return true;
	return false;
} // operator<
 
bool PhysicsObjectID::operator>(PhysicsObjectID& id)
{
	if (this->typeID > id.typeID)
		return true;
	if (this->typeID < id.typeID)
		return false;
	if (this->instanceID > id.instanceID)
		return true;
	return false;
} // operator>
