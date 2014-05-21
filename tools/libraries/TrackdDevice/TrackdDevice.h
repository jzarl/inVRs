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

#ifndef _TRACKDDEVICE_H
#define _TRACKDDEVICE_H

#include <inVRs/InputInterface/ControllerManager/InputDevice.h>

#ifdef WIN32
# ifdef inVRsTrackdDevice_EXPORTS
#  define INVRS_TRACKDDEVICE_API __declspec(dllexport)
# else
#  define INVRS_TRACKDDEVICE_API __declspec(dllimport)
# endif
#else
# define INVRS_TRACKDDEVICE_API
#endif


class TrackerReader;
class ControllerReader;

/******************************************************************************
 * InputDevice class for reading values from the Trackd library.
 */
class INVRS_TRACKDDEVICE_API TrackdDevice : public InputDevice {
public:
	/**
	 * Constructor
	 */
	TrackdDevice(int trackerKey, int controllerKey);

	/**
	 * Destructor
	 */
	virtual ~TrackdDevice();

	/**
	 * Returns the number of buttons provided by the input device
	 */
	int getNumberOfButtons();

	/**
	 * Returns the number of axes provided by the input device
	 */
	int getNumberOfAxes();

	/**
	 * Returns the number of sensors provided by the input device
	 */
	int getNumberOfSensors();

	/**
	 * Returns the value of the button with the passed index
	 */
	int getButtonValue(int idx);

	/**
	 * Returns the value of the axis with the passed index
	 */
	float getAxisValue(int idx);

	/**
	 * Returns the value of the sensor with the passed index
	 */
	SensorData getSensorValue(int idx);

	/**
	 * Updates the values of the TrackdDevice
	 */
	void update();

	/**
	 * Returns if the TrackdDevice was successfully initialized
	 */
	bool isInitialized() const;

private:

	/**
	 * Initializes the device
	 */
	void initializeDevice();

	std::vector<int> buttonValues;
	std::vector<float> axisValues;
	std::vector<SensorData> sensorValues;

	/// defines if
	bool initialized;
	/// member for reading the tracker data
	TrackerReader* tracker;
	/// member for reading the controller data
	ControllerReader* controller;
	/// Trackd-keys for tracker and controller
	int trackerKey, controllerKey;
}; // TrackdDevice

/******************************************************************************
 * Factory class for the TrackdDevice
 */
class INVRS_TRACKDDEVICE_API TrackdDeviceFactory : public InputDeviceFactory {
public:

	/**
	 * Destructor
	 */
	virtual ~TrackdDeviceFactory() {}

	/**
	 * Creates a new TrackdDevice if the passed className matches
	 */
	virtual InputDevice* create(std::string className, ArgumentVector* args = NULL);
}; // TrackdDeviceFactory

#endif
