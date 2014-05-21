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

#ifndef _CONTROLLERINTERFACE_H
#define _CONTROLLERINTERFACE_H

#include "InputInterfaceSharedLibraryExports.h"
#include "../SystemCore/DataTypes.h"

class AbstractControllerButtonChangeCB;

class INVRS_INPUTINTERFACE_API ControllerInterface {
public:
	virtual ~ControllerInterface();

	virtual int getNumberOfButtons() = 0;
	virtual int getNumberOfAxes() = 0;
	virtual int getNumberOfSensors() = 0;
	virtual int getButtonValue(int idx) = 0;
	virtual float getAxisValue(int idx) = 0;
	virtual SensorData getSensorValue(int idx) = 0;
	virtual void setPhysicalToWorldScale(float scaleValue) = 0;
	virtual float getPhysicalToWorldScale() = 0;
	virtual void update() = 0;

	/**
	 * Registers a callback-function for changes on the controller buttons
	 */
	virtual bool registerButtonChangeCallback(AbstractControllerButtonChangeCB* callback) = 0;

	/**
	 * Unregisters a callback-function for changes on the controller buttons
	 */
	virtual bool unregisterButtonChangeCallback(AbstractControllerButtonChangeCB* callback) = 0;

protected:
	/**
	 * Calls the passed callback function
	 */
	void call(AbstractControllerButtonChangeCB* callback, int buttonIndex, int newButtonValue);

	/**
	 * Cleans the controller pointer and deactivates the passed callback
	 * (should be called out of the destructor of the controller)
	 */
	void invalidateController(AbstractControllerButtonChangeCB* callback);

}; // ControllerInterfaace

#endif
