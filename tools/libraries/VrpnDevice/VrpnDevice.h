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
 *  Created on: Jul 14, 2009                                                 *
 *      Author: rlander                                                      *
 *                                                                           *
\*---------------------------------------------------------------------------*/

#ifndef VRPNDEVICE_H_
#define VRPNDEVICE_H_

#ifdef _WIN32 //winsock 1 vs. winsock 2
# include <winsock2.h>
#endif
#include <vrpn_Button.h>
#include <vrpn_Tracker.h>
#include <vrpn_Analog.h>
#include <set>

#include <inVRs/InputInterface/ControllerManager/InputDevice.h>

#ifdef WIN32
# ifdef inVRsVrpnDevice_EXPORTS
#  define INVRS_VRPNDEVICE_API __declspec(dllexport)
# else
#  define INVRS_VRPNDEVICE_API __declspec(dllimport)
# endif
#else
# define INVRS_VRPNDEVICE_API
#endif



/******************************************************************************
 * InputDevice class for reading values from the VRPN library.
 *
 * Note: up to r2240, the sensor enumeration differed between VRPN and the 
 *       inVRs VrpnDevice (inVRs sensor index was VRPN sensorIndex - 1).
 *       After that revision, both are the same.
 *       That means before you could not access sensor 0 of a vrpn device, now
 *       you can.
 */
class INVRS_VRPNDEVICE_API VrpnDevice : public InputDevice {
public:
	/**
	 * Constructor
	 */
	VrpnDevice(std::string deviceId, unsigned numSensors, unsigned numButtons, unsigned numAxes);

	/**
	 * Destructor
	 */
	virtual ~VrpnDevice();

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
	 * Updates the values of the VrpnDevice
	 */
	void update();

	/**
	 * Returns if the VrpnDevice was successfully initialized
	 */
	bool isInitialized() const;

	/**
	 * Callback method for the tracker
	 */
	static void VRPN_CALLBACK trackerPosQuatCallback(void *userdata,
			const vrpn_TRACKERCB trackerData);

	/**
	 * Callback method for the buttons
	 */
	static void VRPN_CALLBACK buttonCallback(void *userdata, const vrpn_BUTTONCB buttonData);

	/**
	 * Callback method for the analog input data
	 */
	static void VRPN_CALLBACK analogCallback(void *userdata, const vrpn_ANALOGCB analogData);

private:

	/**
	 * Initializes the device
	 */
	void initializeDevice(unsigned numSensors, unsigned numButtons, unsigned numAxes);

	/**
	 * Update tracker data
	 */
	void updateTracker(const vrpn_TRACKERCB trackerData);

	/**
	 * Update button data
	 */
	void updateButton(const vrpn_BUTTONCB buttonData);

	/**
	 * Update analog data
	 */
	void updateAnalog(const vrpn_ANALOGCB analogData);

	std::vector<int> buttonValues;
	std::vector<float> axisValues;
	std::vector<SensorData> sensorValues;
	std::set<int> buttonCallbackWarnings;
	std::set<int> axisCallbackWarnings;
	std::set<int> sensorCallbackWarnings;

	/// defines if
	bool initialized;
	// ID for the device
	std::string deviceId;
	/// member for reading the tracker data
	vrpn_Tracker_Remote* tracker;
	/// member for reading the button data
	vrpn_Button_Remote* button;
	/// member for reading the axis data
	vrpn_Analog_Remote* analog;
}; // VrpnDevice

/******************************************************************************
 * Factory class for the VrpnDevice
 */
class INVRS_VRPNDEVICE_API VrpnDeviceFactory : public InputDeviceFactory {
public:

	/**
	 * Destructor
	 */
	virtual ~VrpnDeviceFactory() {}

	/**
	 * Creates a new VrpnDevice if the passed className matches
	 */
	virtual InputDevice* create(std::string className, ArgumentVector* args = NULL);
}; // VrpnDeviceFactory


#endif /* VRPNDEVICE_H_ */
