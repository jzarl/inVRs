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

#include "DeviceIndexMapping.h"

const DeviceIndexMapping UndefinedMapping = DeviceIndexMapping(-1, -1);

DeviceIndexMapping::DeviceIndexMapping(int deviceIndex, int indexInDevice) :
	deviceIndex(deviceIndex),
	indexInDevice(indexInDevice)
{}

bool DeviceIndexMapping::operator==(const DeviceIndexMapping& rhs) const {
	return (deviceIndex == rhs.deviceIndex &&
			indexInDevice == rhs.indexInDevice);
} // operator==

void DeviceIndexMapping::setDeviceIndex(int deviceIndex) {
	this->deviceIndex = deviceIndex;
} // setDeviceIndex

void DeviceIndexMapping::setIndexInDevice(int indexInDevice) {
	this->indexInDevice = indexInDevice;
} // setIndexInDevice

int DeviceIndexMapping::getDeviceIndex() const {
	return deviceIndex;
} // getDeviceIndex

int DeviceIndexMapping::getIndexInDevice() const {
	return indexInDevice;
} // getIndexInDevice
