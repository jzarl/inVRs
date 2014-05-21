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

#include "VrpnDevice.h"

#include <assert.h>

#include <inVRs/SystemCore/DebugOutput.h>

VrpnDevice::VrpnDevice(std::string deviceId, unsigned numSensors, unsigned numButtons,
		unsigned numAxes) :
	initialized(false),
	deviceId(deviceId),
	tracker(NULL),
	button(NULL),
	analog(NULL) {

	initializeDevice(numSensors, numButtons, numAxes);
} // VrpnDevice

VrpnDevice::~VrpnDevice() {
	if (tracker)
		delete tracker;
	if (button)
		delete button;
	if (analog)
		delete analog;
} // ~VrpnDevice

int VrpnDevice::getNumberOfButtons() {
	return buttonValues.size();
} // getNumberOfButtons

int VrpnDevice::getNumberOfAxes() {
	return axisValues.size();
} // getNumberOfAxes

int VrpnDevice::getNumberOfSensors() {
	return sensorValues.size();
} // getNumberOfSensors

int VrpnDevice::getButtonValue(int idx) {
	int result = 0;
	if (idx >= 0 && idx < (int)buttonValues.size()) {
		result = buttonValues[idx];
	} // if
	else {
		printd(WARNING,
				"VrpnDevice::getButtonValue(): invalid button index %i passed (device has %i buttons)!\n",
				idx, buttonValues.size());
	} // if
	return result;
} // getButtonValue

float VrpnDevice::getAxisValue(int idx) {
	float result = 0;
	if (idx >= 0 && idx < (int)axisValues.size()) {
		result = axisValues[idx];
	} // if
	else {
		printd(WARNING,
				"VrpnDevice::getAxisValue(): invalid axis index %i passed (device has %i axes)!\n",
				idx, axisValues.size());
	} // if
	return result;
} // getAxisValue

SensorData VrpnDevice::getSensorValue(int idx) {
	SensorData result = IdentitySensorData;
	if (idx >= 0 && idx < (int)sensorValues.size()) {
		result = sensorValues[idx];
	} // if
	else {
		printd(WARNING,
				"VrpnDevice::getSensorValue(): invalid sensor index %i passed (device has %i sensors)!\n",
				idx, sensorValues.size());
	} // if
	return result;
} // getSensorValue

void VrpnDevice::update() {
	if (tracker)
		tracker->mainloop();
	if (button)
		button->mainloop();
	if (analog)
		analog->mainloop();
} // update

bool VrpnDevice::isInitialized() const {
	return initialized;
} // isInitialized

void VRPN_CALLBACK VrpnDevice::trackerPosQuatCallback(void *userdata,
		const vrpn_TRACKERCB trackerData) {
	VrpnDevice* instance = (VrpnDevice*)userdata;
	if (instance) {
		instance->updateTracker(trackerData);
	} else {
		printd(WARNING,
				"VrpnDevice::trackerPosQuatCallback(): callback for unknown VRPN-device found!\n");
	} // else
} // trackerPosQuatCallback

void VRPN_CALLBACK VrpnDevice::buttonCallback(void *userdata, const vrpn_BUTTONCB buttonData) {
	VrpnDevice* instance = (VrpnDevice*)userdata;
	if (instance) {
		instance->updateButton(buttonData);
	} else {
		printd(WARNING,
				"VrpnDevice::buttonCallback(): callback for unknown VRPN-device found!\n");
	} // else
} // buttonCallback

void VRPN_CALLBACK VrpnDevice::analogCallback(void *userdata, const vrpn_ANALOGCB analogData) {
	VrpnDevice* instance = (VrpnDevice*)userdata;
	if (instance) {
		instance->updateAnalog(analogData);
	} else {
		printd(WARNING,
				"VrpnDevice::analogCallback(): callback for unknown VRPN-device found!\n");
	} // else
} // analogCallback

void VrpnDevice::initializeDevice(unsigned numSensors, unsigned numButtons, unsigned numAxes) {
	tracker = new vrpn_Tracker_Remote(deviceId.c_str());
	button = new vrpn_Button_Remote(deviceId.c_str());
	analog = new vrpn_Analog_Remote(deviceId.c_str());

	sensorValues.resize(numSensors);
	for (int i=0; i < (int)numSensors; i++) {
		sensorValues[i] = IdentitySensorData;
	} // for

	buttonValues.resize(numButtons);
	for (int i=0; i < (int)numButtons; i++) {
		buttonValues[i] = 0;
	} // for

	axisValues.resize(numAxes);
	for (int i=0; i < (int)numAxes; i++) {
		axisValues[i] = 0;
	} // for
	printd(INFO, "VrpnDevice has %d sensors, %d buttons, %d axes\n", numSensors, numButtons, numAxes);

	if (tracker) {
		tracker->register_change_handler(this, &VrpnDevice::trackerPosQuatCallback);
	} else {
		printd(WARNING, "VrpnDevice::initializeDevice(): unable to open vrpn_Tracker!\n");
	} // else

	if (button) {
		button->register_change_handler(this, &VrpnDevice::buttonCallback);
	} else {
		printd(WARNING, "VrpnDevice::initializeDevice(): unable to open vrpn_Button!\n");
	} // else

	if (analog) {
		analog->register_change_handler(this, &VrpnDevice::analogCallback);
	} else {
		printd(WARNING, "VrpnDevice::initializeDevice(): unable to open vrpn_Analog!\n");
	} // else

	if (!analog && !button && !tracker)
		initialized = false;
	else
		initialized = true;
} // initializeDevice

void VrpnDevice::updateTracker(const vrpn_TRACKERCB trackerData) {
	int sensorIndex = trackerData.sensor;

	acquireControllerLock();
	if (sensorIndex >= 0 && sensorIndex < (int)sensorValues.size()) {
		sensorValues[sensorIndex].position = gmtl::Vec3f(trackerData.pos[0], trackerData.pos[1],
				trackerData.pos[2]);
		sensorValues[sensorIndex].orientation = gmtl::Quatf(trackerData.quat[0], trackerData.quat[1],
				trackerData.quat[2], trackerData.quat[3]);
	} // if
	else if (sensorCallbackWarnings.find(sensorIndex) == sensorCallbackWarnings.end()) {
		printd(WARNING,
				"VrpnDevice::updateTracker(): invalid tracker with index %i found (valid: 0 <= index < %i). Further warnings for this sensor will be omitted.\n",
				sensorIndex, sensorValues.size());
		sensorCallbackWarnings.insert(sensorIndex);
	} // else
	releaseControllerLock();
} // updateTracker

void VrpnDevice::updateButton(const vrpn_BUTTONCB buttonData) {
	int buttonIndex = buttonData.button;
	int buttonValue = buttonData.state ? 1 : 0;
	bool change = false;

	acquireControllerLock();
	if (buttonIndex < (int)buttonValues.size()) {
		if (buttonValues[buttonIndex] != buttonValue) {
			buttonValues[buttonIndex] = buttonValue;
			change = true;
		} // if
	} // if
	else if (buttonCallbackWarnings.find(buttonIndex) == buttonCallbackWarnings.end()){
		printd(WARNING,
				"VrpnDevice::updateButton(): invalid button with index %i found - device is configured for only %i buttons! Further warnings for this button will be omitted!\n",
				buttonIndex, buttonValues.size());
		buttonCallbackWarnings.insert(buttonIndex);
	} // else
	releaseControllerLock();

	if (change)
		sendButtonChangeNotification(buttonIndex, buttonValue);

	if (buttonIndex < (int)buttonValues.size()) {
		printd(INFO, "VrpnDevice::updateButton(): updated value of button %i: %i!\n", buttonIndex,
				buttonData.state);
	} // if
} // updateButton

void VrpnDevice::updateAnalog(const vrpn_ANALOGCB analogData) {
	int numAxes = analogData.num_channel;

	acquireControllerLock();
	for (int i=0; i < numAxes; i++) {
		if (i < (int)axisValues.size()) {
			axisValues[i] = analogData.channel[i];
			printd(INFO, "\taxis %i: %f\n", i, axisValues[i]);
		} // else if
		else if (axisCallbackWarnings.find(i) == axisCallbackWarnings.end()){
			printd(WARNING,
					"VrpnDevice::updateAnalog(): invalid axis with index %i found - device is configured for only %i axes! Further warnings for this axis will be omitted!\n",
					i, axisValues.size());
			axisCallbackWarnings.insert(i);
		} // else
	} // for
	releaseControllerLock();
} // updateAnalog

InputDevice* VrpnDeviceFactory::create(std::string className, ArgumentVector* args) {
	if (className != "VrpnDevice")
		return NULL;

	if (!args || !args->keyExists("deviceID")) {
		printd(ERROR,
				"VrpnDeviceFactory::create(): missing argument entry deviceID! Cannot create Device!\n");
		return NULL;
	} // if

	std::string deviceId;
	unsigned numSensors = 0;
	unsigned numButtons = 0;
	unsigned numAxes = 0;
	args->get("deviceID", deviceId);
	if (args->keyExists("numSensors"))
		args->get("numSensors", numSensors);
	if (args->keyExists("numButtons"))
		args->get("numButtons", numButtons);
	if (args->keyExists("numAxes"))
		args->get("numAxes", numAxes);

	VrpnDevice* device = new VrpnDevice(deviceId, numSensors, numButtons, numAxes);

	// check if device could be initialized and return null if not!
	if (!device->isInitialized()) {
		printd(ERROR,
				"VrpnDeviceFactory::create(): unable to initialize VRPN device with ID %s\n",
				deviceId.c_str());
		delete device;
		device = NULL;
	} // if

	return device;
} // create

