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

#ifndef INPUTDEVICEBASE_H_
#define INPUTDEVICEBASE_H_

#include "InputDevice.h"

/******************************************************************************
 * This class is the abstract base class for the implemented InputDevices
 * which are provided by the inVRs framework. It provides data structures for
 * storing button, axis and sensor data and implements the common getter
 * methods.
 *
 * @author rlander
 */
class CONTROLLERMANAGER_API InputDeviceBase : public InputDevice {
public:

	/**
	 * Constructor
	 */
	InputDeviceBase();

	/**
	 * Destructor
	 */
	virtual ~InputDeviceBase();

	/**
	 * Returns the number of buttons provided by the input device
	 */
	virtual int getNumberOfButtons();

	/**
	 * Returns the number of axes provided by the input device
	 */
	virtual int getNumberOfAxes();

	/**
	 * Returns the number of sensors provided by the input device
	 */
	virtual int getNumberOfSensors();

	/**
	 * Returns the value of the button with the passed index
	 */
	virtual int getButtonValue(int idx);

	/**
	 * Returns the value of the axis with the passed index
	 */
	virtual float getAxisValue(int idx);

	/**
	 * Returns the value of the sensor with the passed index
	 */
	virtual SensorData getSensorValue(int idx);

	/**
	 * Empty implementation of the update method.
	 */
	virtual void update();

protected:
	int nButtons;				// number of buttons
	int nAxes;					// number of axes
	int nSensors;				// number of sensors
	int *buttonState;			// values of the provided buttons
	float *axisState;			// values of the provided axes
	SensorData* sensorState;	// values of the provided sensors
};

#endif /* INPUTDEVICEBASE_H_ */
