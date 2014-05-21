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

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <string.h>
#include <memory>
#include <vector>

#include <assert.h>

#include "ControllerManagerSharedLibraryExports.h"
#include "InputDevice.h"
#include "DeviceIndexMapping.h"
#include "ButtonCorrection.h"
#include "AxisCorrection.h"
#include "SensorCorrection.h"

#include "../ControllerInterface.h"

class AbstractControllerButtonChangeCB;

/******************************************************************************
 * abstract controller class. It provides an interface for accessing devices
 * which have buttons and axes and sensors. The following value ranges should
 * be provided by the controller (and therefore from its input devices):
 * button .. (0,1)
 * axis   .. [-1,+1]
 * sensor .. position: arbitrary vector
 *           orientation: arbitrary rotation quaternion
 * @author hbress
 * @author rlander
 */
class CONTROLLERMANAGER_API Controller : public ControllerInterface {
public:
	/**
	 * Destructor
	 */
	virtual ~Controller();

//********************************************
// Methods inherited from ControllerInterface:
//********************************************

	/**
	 * Returns the number of buttons provided by the controller
	 */
	int getNumberOfButtons();

	/**
	 * Returns the number of axes provided by the controller
	 */
	int getNumberOfAxes();

	/**
	 * Returns the number of sensors provided by the controller
	 */
	int getNumberOfSensors();

	/**
	 * Returns the value of the button with the passed index. If no button with
	 * the passed index exists the method returns 0
	 */
	int getButtonValue(int idx);

	/**
	 * Returns the value of the axis with the passed index. If no axis with the
	 * passed index exists the method returns 0
	 */
	float getAxisValue(int idx);

	/**
	 * Returns the value of the sensor with the passed index. If no sensor with
	 * the passed index exists the method returns the identity transformation
	 */
	SensorData getSensorValue(int idx);

	/**
	 * Sets the scale factor for sensor values (obtained from tracking systems)
	 * from physical units (e.g. centimeters) to world units (units used by the
	 * application).
	 */
	void setPhysicalToWorldScale(float scaleValue);

	/**
	 * Returns the scale factor for sensor values from physical units (e.g.
	 * centimeters) to world units (units used by the application).
	 */
	float getPhysicalToWorldScale();

	/**
	 * Calls the update method on all input devices. This method can be used
	 * by input devices which are based on polling mechanisms to update their
	 * internal state.
	 */
	void update();

	/**
	 * Registers a callback-function for changes on the controller buttons
	 */
	bool registerButtonChangeCallback(AbstractControllerButtonChangeCB* callback);

	/**
	 * Unregisters a callback-function for changes on the controller buttons
	 */
	bool unregisterButtonChangeCallback(AbstractControllerButtonChangeCB* callback);

	//*************
	// Own methods:
	//*************
	/**
	 * Returns the InputDevice with the passed index
	 */
	InputDevice* getInputDevice(unsigned int index);

	/**
	 * Sets the mapping for the controller button with the passed index to the
	 * according device button
	 */
	bool setButtonMapping(int buttonIndex, const DeviceIndexMapping& mapping);

	/**
	 * Sets the mapping for the controller axis with the passed index to the
	 * according device axis
	 */
	bool setAxisMapping(int axisIndex, const DeviceIndexMapping& mapping);

	/**
	 * Sets the mapping for the controller sensor with the passed index to the
	 * according device sensor
	 */
	bool setSensorMapping(int sensorIndex, const DeviceIndexMapping& mapping);

	/**
	 * Sets the correction for the button with the passed index
	 */
	bool setButtonCorrection(int buttonIndex, const ButtonCorrection& correction);

	/**
	 * Sets the correction for the axis with the passed index
	 */
	bool setAxisCorrection(int axisIndex, const AxisCorrection& correction);

	/**
	 * Sets the correction for the sensor with the passed index
	 */
	bool setSensorCorrection(int sensorIndex, const SensorCorrection& correction);

	/**
	 * Returns the mapping for the controller button with the passed index
	 */
	const DeviceIndexMapping& getButtonMapping(int buttonIndex) const;

	/**
	 * Returns the mapping for the controller axis with the passed index
	 */
	const DeviceIndexMapping& getAxisMapping(int axisIndex) const;

	/**
	 * Returns the mapping for the controller sensor with the passed index
	 */
	const DeviceIndexMapping& getSensorMapping(int sensorIndex) const;

	/**
	 * Returns the correction for the controller button with the passed index
	 */
	const ButtonCorrection& getButtonCorrection(int buttonIndex) const;

	/**
	 * Returns the correction for the controller axis with the passed index
	 */
	const AxisCorrection& getAxisCorrection(int axisIndex) const;

	/**
	 * Returns the correction for the controller sensor with the passed index
	 */
	const SensorCorrection& getSensorCorrection(int sensorIndex) const;

private:
	friend class ControllerManager;
	friend class InputDevice;

	typedef std::pair<int, int> ButtonState;

	/**
	 * Default constructor
	 */
	Controller();

	// Avoid copy assignment via assignment operator
	Controller& operator=(const Controller& src);

	// Avoid copy via copy constructor
	Controller(const Controller& src);

	/**
	 * Define the number of buttons, axes and sensors for the controller
	 */
	void init(int numButtons, int numAxes, int numSensors);

	/**
	 * Adds a new inputDevice to the controller
	 */
	void addDevice(InputDevice* dev);

	/**
	 * Swaps the data between the passed controller and the current one
	 */
	void swap(const std::auto_ptr<Controller>& rhs);

	/**
	 * Is called by InputDevice as soon as a button has changed its value. The
	 * method then checks if the button is part of the controller and if so it
	 * invokes all registered buttonChangeCallbacks.
	 */
	void deviceButtonChangeNotification(int deviceIndex, int buttonIndex, int newButtonValue);

	/**
	 * This method is called after the update method has finished in order to
	 * execute callbacks which had to be delayed because of deadlock prevention
	 */
	void executeDelayedCallbacks();

	/**
	 * Invokes all registered button change callbacks.
	 */
	void executeButtonChangeCallbacks(int buttonIndex, int newButtonValue);

	/**
	 * Returns the index of the passed device button in the controller. If the
	 * device button is not mapped to a controller button the method returns -1
	 */
	int findControllerButtonIndex(const DeviceIndexMapping& deviceButton);

	bool inUpdate;	// stores if the update-method is currently invoked
	int nButtons;	// the number of provided buttons
	int nAxes;		// the number of provided axes
	int nSensors;	// the number of provided sensors
	std::map<unsigned int, DeviceIndexMapping> buttonMapping;
	std::map<unsigned int, DeviceIndexMapping> axisMapping;
	std::map<unsigned int, DeviceIndexMapping> sensorMapping;
	std::map<unsigned int, ButtonCorrection> buttonCorrections;
	std::map<unsigned int, AxisCorrection> axisCorrections;
	std::map<unsigned int, SensorCorrection> sensorCorrections;
	float physicalToWorldScale;

	std::vector<InputDevice*> devices;	// list of all input devices
#if OSG_MAJOR_VERSION >= 2
	OSG::LockRefPtr inputDataLock;			// lock for input devices, mappings and corrections	
	OSG::LockRefPtr callbackLock;			// lock for all callbacks
#else //OpenSG1:
	OSG::Lock* inputDataLock;			// lock for input devices, mappings and corrections
	OSG::Lock* callbackLock;			// lock for all callbacks
#endif
	std::vector<AbstractControllerButtonChangeCB*> buttonChangeCallbacks;
	std::vector<ButtonState> delayedButtonChangeCallbacks;
}; // Controller

#endif
