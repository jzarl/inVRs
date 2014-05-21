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

#include "OopsObjectID.h"

OopsObjectID::OopsObjectID() {
	isJoint = false;
	physicsObjectClass = 0;
	instanceID = 0;
	objectID = 0;
} // OopsObjectID

OopsObjectID::OopsObjectID(uint64_t id) {
	*this = id;
	//	this->isJoint = id >> 63;
	//	this->physicsObjectClass = (id >> 55) & 0xFF; 
	//	this->instanceID = (id >> 39) & 0xFFFF;
	//	this->objectID = id & 0xFFFFFFFF;
} // OopsObjectID

OopsObjectID::OopsObjectID(bool isJoint, uint8_t physicsObjectClass, uint16_t instanceID, uint32_t objectID) {
	this->isJoint = isJoint;
	this->physicsObjectClass = physicsObjectClass;
	this->instanceID = instanceID;
	this->objectID = objectID;
} // OopsObjectID

OopsObjectID& OopsObjectID::operator=(OopsObjectID id) {
	this->isJoint = id.isJoint;
	this->physicsObjectClass = id.physicsObjectClass;
	this->instanceID = id.instanceID;
	this->objectID = id.objectID;
	return *this;
} // operator=

OopsObjectID& OopsObjectID::operator=(uint64_t id) {
	this->isJoint = id >> 63;
	this->physicsObjectClass = (id >> 55) & 0xFF;
	this->instanceID = (id >> 39) & 0xFFFF;
	this->objectID = id & 0xFFFFFFFF;
	return *this;
} // operator=

bool OopsObjectID::operator==(OopsObjectID id) {
	if (this->isJoint == id.isJoint &&
			this->physicsObjectClass == id.physicsObjectClass &&
			this->instanceID == id.instanceID &&
			this->objectID == id.objectID)
		return true;

	return false;
} // operator==

bool OopsObjectID::operator==(uint64_t id) {
	return (*this == OopsObjectID(id));
} // operator==

bool OopsObjectID::operator!=(OopsObjectID id) {
	return !(*this == id);
} // operator!=

bool OopsObjectID::operator!=(uint64_t id) {
	return (*this != OopsObjectID(id));
} // operator!=

bool OopsObjectID::operator<(OopsObjectID id) {
	if (this->isJoint < id.isJoint)
		return true;
	else if (this->isJoint > id.isJoint)
		return false;
	
	if (this->physicsObjectClass < id.physicsObjectClass)
		return true;
	else if (this->physicsObjectClass > id.physicsObjectClass)
		return false;
	
	if (this->instanceID < id.instanceID)
		return true;
	else if (this->instanceID > id.instanceID)
		return false;
	
	if (this->objectID < id.objectID)
		return true;
	else
		return false;
} // operator<

bool OopsObjectID::operator<(uint64_t id) {
	return (*this < OopsObjectID(id));
} // operator<

bool OopsObjectID::operator<=(OopsObjectID id) {
	if (this->isJoint < id.isJoint)
		return true;
	else if (this->isJoint > id.isJoint)
		return false;
	
	if (this->physicsObjectClass < id.physicsObjectClass)
		return true;
	else if (this->physicsObjectClass > id.physicsObjectClass)
		return false;
	
	if (this->instanceID < id.instanceID)
		return true;
	else if (this->instanceID > id.instanceID)
		return false;
	
	if (this->objectID <= id.objectID)
		return true;
	else
		return false;
} // operator<

bool OopsObjectID::operator<=(uint64_t id) {
	return (*this <= OopsObjectID(id));
} // operator<


bool OopsObjectID::operator>(OopsObjectID id) {
	if (this->isJoint > id.isJoint)
		return true;
	else if (this->isJoint < id.isJoint)
		return false;
	
	if (this->physicsObjectClass > id.physicsObjectClass)
		return true;
	else if (this->physicsObjectClass < id.physicsObjectClass)
		return false;
	
	if (this->instanceID > id.instanceID)
		return true;
	else if (this->instanceID < id.instanceID)
		return false;
	
	if (this->objectID > id.objectID)
		return true;
	else
		return false;
} // operator>

bool OopsObjectID::operator>(uint64_t id) {
	return (*this > OopsObjectID(id));
} // operator>=

bool OopsObjectID::operator>=(OopsObjectID id) {
	if (this->isJoint > id.isJoint)
		return true;
	else if (this->isJoint < id.isJoint)
		return false;
	
	if (this->physicsObjectClass > id.physicsObjectClass)
		return true;
	else if (this->physicsObjectClass < id.physicsObjectClass)
		return false;
	
	if (this->instanceID > id.instanceID)
		return true;
	else if (this->instanceID < id.instanceID)
		return false;
	
	if (this->objectID >= id.objectID)
		return true;
	else
		return false;
} // operator>=

bool OopsObjectID::operator>=(uint64_t id) {
	return (*this >= OopsObjectID(id));
} // operator>

uint64_t OopsObjectID::get() {
	uint64_t result = 0;
	result |= ((uint64_t)(this->isJoint ? 1 : 0)) << 63;
	result |= ((uint64_t)this->physicsObjectClass) << 55;
	result |= ((uint64_t)this->instanceID) << 39;
	result |= ((uint64_t)this->objectID);
	return result;
} // get

bool OopsObjectID::getIsJoint() {
	return isJoint;
} // getIsJoint

uint8_t OopsObjectID::getPhysicsObjectClass() {
	return physicsObjectClass;
} // getPhysicsObjectClass

uint16_t OopsObjectID::getInstanceID() {
	return instanceID;
} // getInstanceID

uint32_t OopsObjectID::getObjectID() {
	return objectID;
} // getObjectID
