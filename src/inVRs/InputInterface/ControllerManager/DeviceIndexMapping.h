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

#ifndef DEVICEINDEXMAPPING_H_
#define DEVICEINDEXMAPPING_H_

#include "ControllerManagerSharedLibraryExports.h"

/******************************************************************************
 * Stores information about indices of device and index in device for a
 * button/axis/sensor.
 */
class CONTROLLERMANAGER_API DeviceIndexMapping {
public:
	/**
	 * Constructor for DeviceIndexMapping
	 */
	explicit DeviceIndexMapping(int deviceIndex, int indexInDevice);

	/**
	 * Comparison operator
	 */
	bool operator==(const DeviceIndexMapping& rhs) const;

	/**
	 * Sets the index of the device
	 */
	void setDeviceIndex(int deviceIndex);

	/**
	 * Sets the index of the button/axis/sensor in the device
	 */
	void setIndexInDevice(int indexInDevice);

	/**
	 * Returns the index of the corresponding device
	 */
	int getDeviceIndex() const;

	/**
	 * Returns the index of the button/axis/sensor in the device
	 */
	int getIndexInDevice() const;

private:
	int deviceIndex; // Index of button/axis/sensor in device
	int indexInDevice; // Index of device (in vector devices)
}; // DeviceIndexMapping

extern const DeviceIndexMapping UndefinedMapping; // = DeviceIndexMapping(-1, -1);

#endif /* DEVICEINDEXMAPPING_H_ */
