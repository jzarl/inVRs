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

#include "TrackdDevice.h"

#include <trackdAPI_CC.h>

#include <gmtl/Matrix.h>
#include <gmtl/Generate.h>
#include <irrXML.h>

#include <inVRs/SystemCore/DebugOutput.h>

TrackdDevice::TrackdDevice(int trackerKey, int controllerKey) :
	initialized(false),
	tracker(NULL),
	controller(NULL),
	trackerKey(trackerKey),
	controllerKey(controllerKey) {
	initializeDevice();
} // TrackdDevice

TrackdDevice::~TrackdDevice() {
	if (tracker)
		delete tracker;
	tracker = NULL;
	if (controller)
		delete controller;
	controller = NULL;
} // ~TrackdDevice

int TrackdDevice::getNumberOfButtons() {
	return buttonValues.size();
} // getNumberOfButtons

int TrackdDevice::getNumberOfAxes() {
	return axisValues.size();
} // getNumberOfAxes

int TrackdDevice::getNumberOfSensors() {
	return sensorValues.size();
} // getNumberOfSensors

int TrackdDevice::getButtonValue(int idx) {
	int result = 0;
	if (idx >= 0 && idx < (int)buttonValues.size()) {
		result = buttonValues[idx];
	} // if
	else {
		printd(WARNING,
				"TrackdDevice::getButtonValue(): invalid button index %i passed (device has %i buttons)!\n",
				idx, buttonValues.size());
	} // if
	return result;
} // getButtonValue

float TrackdDevice::getAxisValue(int idx) {
	float result = 0;
	if (idx >= 0 && idx < (int)axisValues.size()) {
		result = axisValues[idx];
	} // if
	else {
		printd(WARNING,
				"TrackdDevice::getAxisValue(): invalid axis index %i passed (device has %i axes)!\n",
				idx, axisValues.size());
	} // if
	return result;
} // getAxisValue

SensorData TrackdDevice::getSensorValue(int idx) {
	SensorData result = IdentitySensorData;
	if (idx >= 0 && idx < (int)sensorValues.size()) {
		result = sensorValues[idx];
	} // if
	else {
		printd(WARNING,
				"TrackdDevice::getSensorValue(): invalid sensor index %i passed (device has %i sensors)!\n",
				idx, sensorValues.size());
	} // if
	return result;
} // getSensorValue

void TrackdDevice::update() {
	int numButtons = buttonValues.size();
	int numAxes = axisValues.size();
	int numSensors = sensorValues.size();
	int newButtonValue;
	float matrix[4][4];
	gmtl::Matrix44f gmtlMatrix;
	int i;

	for (i = 0; i < numButtons; i++) {
		newButtonValue = controller->trackdGetButton(i);
		if (buttonValues[i] != newButtonValue) {
			buttonValues[i] = newButtonValue;
			sendButtonChangeNotification(i, newButtonValue);
		} // if
	} // for
	for (i = 0; i < numAxes; i++) {
		axisValues[i] = controller->trackdGetValuator(i);
	} // for
	for (i = 0; i < numSensors; i++) {
		tracker->trackdGetMatrix(i, matrix);
		sensorValues[i].position[0] = matrix[3][0];
		sensorValues[i].position[1] = matrix[3][1];
		sensorValues[i].position[2] = matrix[3][2];
		matrix[3][0] = 0;
		matrix[3][1] = 0;
		matrix[3][2] = 0;
		gmtlMatrix.set(&matrix[0][0]);
		gmtl::set(sensorValues[i].orientation, gmtlMatrix);
	} // for
} // update

bool TrackdDevice::isInitialized() const {
	return initialized;
} // isInitialized

void TrackdDevice::initializeDevice() {
	int numButtons = 0;
	int numAxes = 0;
	int numSensors = 0;
	tracker = new TrackerReader(trackerKey);
	controller = new ControllerReader(controllerKey);
	if (tracker && controller) {
		numButtons = controller->trackdGetNumberOfButtons();
		numAxes = controller->trackdGetNumberOfValuators();
		numSensors = tracker->trackdGetNumberOfSensors();
		printd(INFO, "TrackdDevice::initializeDevice(): number of buttons = %i\n", numButtons);
		printd(INFO, "TrackdDevice::initializeDevice(): number of axes = %i\n", numAxes);
		printd(INFO, "TrackdDevice::initializeDevice(): number of sensors = %i\n", numSensors);
		buttonValues.resize(numButtons);
		axisValues.resize(numAxes);
		sensorValues.resize(numSensors);
		// Because SensorData is a struct we have to initialize it manually (to set the quaternions
		// correctly)
		std::vector<SensorData>::iterator it;
		for (it = sensorValues.begin(); it != sensorValues.end(); ++it) {
			*it = IdentitySensorData;
		} // for
		initialized = true;
	} else {
		if (!tracker) {
			printd(ERROR,
					"TrackdDevice::initializeDevice(): unable to create TrackerReader with key %i!\n",
					trackerKey);
		} // if
		if (!controller) {
			printd(ERROR,
					"TrackdDevice::initializeDevice(): unable to create ControllerReader with key %i!\n",
					controllerKey);
		} // if
		initialized = false;
	} // else
} // initializeDevice


InputDevice* TrackdDeviceFactory::create(std::string className, ArgumentVector* args) {
	if (className != "TrackdDevice") {
		return NULL;
	} // if

	if (!args || !args->keyExists("trackerKey") || !args->keyExists("controllerKey")) {
		printd(ERROR,
				"TrackdDeviceFactory::create(): missing argument entry trackerKey or controllerKey! Cannot create Device!\n");
		return NULL;
	} // if

	int trackerKey = -1;
	int controllerKey = -1;

	args->get("trackerKey", trackerKey);
	args->get("controllerKey", controllerKey);

	TrackdDevice* device = new TrackdDevice(trackerKey, controllerKey);

	// check if device could be initialized and return null if not!
	if (!device->isInitialized()) {
		delete device;
		device = NULL;
	} // if

	return device;
} // create
