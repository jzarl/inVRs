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

#ifndef INPUTDEVICE_H
#define INPUTDEVICE_H

#include <OpenSG/OSGThreadManager.h>

#include "ControllerManagerSharedLibraryExports.h"

#include "../../SystemCore/ClassFactory.h"
#include "../../SystemCore/DataTypes.h"
#include "../../SystemCore/ArgumentVector.h"

class Controller;

/******************************************************************************
 * This class is the abstract base class for all input devices which can be
 * used in the Controller class. It provides a simple API which has to be
 * implemented by all inherited classes as well as implementations of methods
 * for thread safety and button change callbacks.
 * The input device can be composed of 0 or more buttons, axes and sensors.
 * The following value ranges should be provided by the input device:
 *
 * button .. (0,1)
 * axis   .. [-1,+1]
 * sensor .. position: arbitrary vector
 *           orientation: arbitrary rotation quaternion
 * @author hbress
 * @author rlander
 */
class CONTROLLERMANAGER_API InputDevice {
public:

	/**
	 * Constructor
	 */
	InputDevice();

	/**
	 * Destructor
	 */
	virtual ~InputDevice();

	/**
	 * Returns the number of buttons provided by the input device
	 */
	virtual int getNumberOfButtons() = 0;

	/**
	 * Returns the number of axes provided by the input device
	 */
	virtual int getNumberOfAxes() = 0;

	/**
	 * Returns the number of sensors provided by the input device
	 */
	virtual int getNumberOfSensors() = 0;

	/**
	 * Returns the value of the button with the passed index
	 */
	virtual int getButtonValue(int idx) = 0;

	/**
	 * Returns the value of the axis with the passed index
	 */
	virtual float getAxisValue(int idx) = 0;

	/**
	 * Returns the value of the sensor with the passed index
	 */
	virtual SensorData getSensorValue(int idx) = 0;

	/**
	 * Updates the data of the buttons/axes/sensors. This method is called
	 * once a frame. Take care to also call the sendButtonChangeNotification
	 * method if the state of a button has changed in order to get a correct
	 * callback behavior!
	 */
	virtual void update() = 0;

protected:

	/**
	 * Locks the controller. This method has to be called when the data for
	 * the buttons/axes/sensors is not updated in the update method. Take care
	 * that the lock is already automatically acquired in the update method, so
	 * don't try to call it from there or you may get deadlocks!
	 */
	void acquireControllerLock();

	/**
	 * Releases the controller lock.
	 * @see acquireControllerLock
	 */
	void releaseControllerLock();

	/**
	 * Notifies the controller that the state of a button has changed. This
	 * method has to be called whenever a value of a button has changed in
	 * order to allow the controller to send callbacks to all registered
	 * listeners.
	 */
	void sendButtonChangeNotification(int buttonIndex, int newButtonValue);

private:
	friend class Controller;

	/**
	 * Initializes the InputDevice. This method is called by the controller
	 * when the device is added.
	 */
	void initialize(int deviceIndex, Controller* controller, OSG::Lock* inputDataLock);

	Controller* controller;		// pointer to the controller (for callbacks)
	OSG::Lock* controllerLock;	// controller lock for thread safety
	int deviceIndex;			// index of the InputDevice in the controller
}; // InputDevice

/******************************************************************************
 * This class is the base class for the for InputDevice factories. For every
 * InputDevice a separate InputDeviceFactory has to be provided which can
 * create an instance of the InputDevice based on its name and the passed
 * ArgumentVector
 */
typedef ClassFactory<InputDevice, ArgumentVector*> InputDeviceFactory;

#endif
