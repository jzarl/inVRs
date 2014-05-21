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

#include "Controller.h"
#include "ScopedOSGLock.h"

#include <assert.h>

#include "../ControllerButtonChangeCB.h"

#include "../../SystemCore/DebugOutput.h"

// disable deprecation warning for std::auto_ptr when std::unique_ptr is not available.
#ifndef HAS_CXX11_UNIQUE_PTR
// Only do this on GCC versions that support it:
# if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 402
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
#endif


OSG_USING_NAMESPACE

Controller::Controller() :
	inUpdate(false),
	nButtons(0),
	nAxes(0),
	nSensors(0),
	physicalToWorldScale(1) {
#if OSG_MAJOR_VERSION >= 2
	inputDataLock = OSG::dynamic_pointer_cast<OSG::Lock> (ThreadManager::the()->getLock("Controller_inputDataLock",false));
	callbackLock = OSG::dynamic_pointer_cast<OSG::Lock> (ThreadManager::the()->getLock("Controller_callbackLock",false));
#else //OpenSG1:
	inputDataLock = dynamic_cast<Lock*> (ThreadManager::the()->getLock("Controller_inputDataLock"));
	callbackLock = dynamic_cast<Lock*> (ThreadManager::the()->getLock("Controller_callbackLock"));
#endif
} // Controller

Controller::~Controller() {
	std::vector<InputDevice*>::iterator it;
	for (it = devices.begin(); it != devices.end(); ++it)
		delete *it;

	std::vector<AbstractControllerButtonChangeCB*>::iterator itCB;
	for (itCB = buttonChangeCallbacks.begin(); itCB != buttonChangeCallbacks.end(); ++itCB) {
		ControllerInterface::invalidateController(*itCB);
//		(*itCB)->invalidateController();
	} // for
	buttonChangeCallbacks.clear();
} // ~Controller

int Controller::getNumberOfButtons() {
	ScopedOSGLock lock(inputDataLock);
	return nButtons;
} // getNumberOfButtons

int Controller::getNumberOfAxes() {
	ScopedOSGLock lock(inputDataLock);
	return nAxes;
} // getNumberOfAxes

int Controller::getNumberOfSensors() {
	ScopedOSGLock lock(inputDataLock);
	return nSensors;
} // getNumberOfSensors

int Controller::getButtonValue(int idx) {
	int value = 0;
	std::map<unsigned int, DeviceIndexMapping>::const_iterator it;
	ScopedOSGLock lock(inputDataLock);

	if (idx >= nButtons) {
		printd(ERROR,
				"Controller::getButtonValue(): button index %i out of range: controller has only %i buttons!\n",
				idx, nButtons);
	} // if
	else  if (idx < 0) {
		printd(ERROR,
				"Controller::getButtonValue(): invalid button index %i passed!\n", idx);
	} // else if
	else if ((it=buttonMapping.find(idx)) != buttonMapping.end()) {
		const DeviceIndexMapping& mapping = it->second;
		value = devices.at(mapping.getDeviceIndex())->getButtonValue(mapping.getIndexInDevice());
	} // else

	std::map<unsigned int, ButtonCorrection>::const_iterator it1;
	if ((it1 = buttonCorrections.find(idx)) != buttonCorrections.end()) {
		const ButtonCorrection& correction = it1->second;
		value = correction.correctButton(value);
	} // if

	return value;
} // getButtonValue

float Controller::getAxisValue(int idx) {
	float value = 0;
	std::map<unsigned int, DeviceIndexMapping>::const_iterator it;
	ScopedOSGLock lock(inputDataLock);

	if (idx >= nAxes) {
		printd(ERROR,
				"Controller::getAxisValue(): axis index %i out of range: controller has only %i axes!\n",
				idx, nAxes);
	} // if
	else  if (idx < 0) {
		printd(ERROR,
				"Controller::getAxisValue(): invalid axis index %i passed!\n", idx);
	} // else if
	else if ((it=axisMapping.find(idx)) != axisMapping.end()) {
		const DeviceIndexMapping& mapping = it->second;
		value = devices.at(mapping.getDeviceIndex())->getAxisValue(mapping.getIndexInDevice());
	} // else

	std::map<unsigned int, AxisCorrection>::const_iterator it1;
	if ((it1 = axisCorrections.find(idx)) != axisCorrections.end()) {
		const AxisCorrection& correction = it1->second;
		value = correction.correctAxis(value);
	} // if

	return value;
} // getAxisValue

SensorData Controller::getSensorValue(int idx) {
	SensorData value;
	value.orientation = gmtl::QUAT_IDENTITYF;
	value.position = gmtl::Vec3f(0.f, 0.f, 0.f);
	std::map<unsigned int, DeviceIndexMapping>::const_iterator it;
	ScopedOSGLock lock(inputDataLock);

	if (idx >= nSensors) {
		printd(ERROR,
				"Controller::getSensorValue(): sensor index %i out of range: controller has only %i sensors!\n",
				idx, nSensors);
	} // if
	else if (idx < 0) {
		printd(ERROR,
				"Controller::getSensorValue(): invalid sensor index %i passed!\n", idx);
	} // else if
	else if ((it=sensorMapping.find(idx)) != sensorMapping.end()) {
		const DeviceIndexMapping& mapping = it->second;
		value = devices.at(mapping.getDeviceIndex())->getSensorValue(mapping.getIndexInDevice());
	} // else if

	std::map<unsigned int, SensorCorrection>::const_iterator it1;
	if ((it1 = sensorCorrections.find(idx)) != sensorCorrections.end()) {
		const SensorCorrection& correction = it1->second;
		value = correction.correctSensor(value);
	} // if
	value.position *= physicalToWorldScale;

	return value;
} // getSensorValue

void Controller::setPhysicalToWorldScale(float scaleValue) {
	physicalToWorldScale = scaleValue;
} // setPhysicalToWorldScale

float Controller::getPhysicalToWorldScale() {
	return physicalToWorldScale;
} // getPhysicalToWorldScale

void Controller::update() {
	std::vector<InputDevice*>::iterator it;

#if OSG_MAJOR_VERSION >= 2
	inputDataLock->acquire();
#else //OpenSG1:
	inputDataLock->aquire();
#endif
	// access to bool should be atomic -> no locking!
	inUpdate = true;

	for (it = devices.begin(); it != devices.end(); ++it) {
		(*it)->update();
	} // for
	inUpdate = false;
	inputDataLock->release();

	executeDelayedCallbacks();
} // update

InputDevice* Controller::getInputDevice(unsigned index) {
	ScopedOSGLock lock(inputDataLock);

	if (devices.size() <= index) {
		printd(WARNING, "Controller::getInputDevice(): invalid device index!\n");
		return NULL;
	} // if

	return devices.at(index);
} // getInputDevice


bool Controller::registerButtonChangeCallback(AbstractControllerButtonChangeCB* callback) {
	ScopedOSGLock lock(callbackLock);

	if (!callback->isActive())
		return callback->activate();

	std::vector<AbstractControllerButtonChangeCB*>::iterator it;
	bool cbAlreadyRegistered = false;
	for (it = buttonChangeCallbacks.begin(); it != buttonChangeCallbacks.end(); ++it) {
		if ((*it) == callback) {
			cbAlreadyRegistered = true;
			break;
		} // if
	} // for

	if (!cbAlreadyRegistered) {
		buttonChangeCallbacks.push_back(callback);
	} else {
		printd(WARNING, "Controller::registerButtonChangeCallback(): Passed callback is already registered! Call will be ignored!\n");
	} // else
	return !cbAlreadyRegistered;
} // registerButtonChangeCallback

bool Controller::unregisterButtonChangeCallback(AbstractControllerButtonChangeCB* callback) {
	ScopedOSGLock lock(callbackLock);

	if (callback->isActive())
		return callback->deactivate();

	std::vector<AbstractControllerButtonChangeCB*>::iterator it;
	bool cbRegistered = false;
	for (it = buttonChangeCallbacks.begin(); it != buttonChangeCallbacks.end(); ++it) {
		if ((*it) == callback) {
			cbRegistered = true;
			buttonChangeCallbacks.erase(it);
			break;
		} // if
	} // for

	if (!cbRegistered) {
		printd(WARNING, "Controller::unregisterButtonChangeCallback(): Passed callback was not registered! Call will be ignored!\n");
	} // else

	return cbRegistered;
} // registerButtonChangeCallback

bool Controller::setButtonMapping(int buttonIndex, const DeviceIndexMapping& mapping) {
	bool success = false;
	ScopedOSGLock lock(inputDataLock);

	if (buttonIndex >= nButtons) {
		printd(ERROR,
				"Controller::setButtonMapping(): index %i for button out of range (controller only provides %i buttons)!\n",
				buttonIndex, nButtons);
	} // else if
	else if (buttonIndex < 0){
		printd(ERROR,
				"Controller::setButtonMapping(): invalid button index %i passed!\n", buttonIndex);
	} // else if
	else if (mapping.getDeviceIndex() >= (int)devices.size()) {
		printd(ERROR,
				"Controller::setButtonMapping(): index %i for device out of range (controller only contains %i devices at this time)!\n",
				mapping.getDeviceIndex(), devices.size());
	} // else if
	else if (mapping.getDeviceIndex() < 0) {
		printd(ERROR,
				"Controller::setButtonMapping(): invalid device index %i passed!\n",
				mapping.getDeviceIndex());
	} // else if
	else if (mapping.getIndexInDevice() >=
			devices.at(mapping.getDeviceIndex())->getNumberOfButtons()) {
		printd(ERROR,
				"Controller::setButtonMapping(): index %i for button in device with index %i out of range (device only contains %i buttons)!\n",
				mapping.getIndexInDevice(), mapping.getDeviceIndex(),
				devices.at(mapping.getDeviceIndex())->getNumberOfButtons());
	} // else if
	else if (mapping.getIndexInDevice() < 0) {
		printd(ERROR,
				"Controller::setButtonMapping(): invalid button index %i for device %i passed!\n",
				mapping.getIndexInDevice(), mapping.getDeviceIndex());
	} // else if
	else {
		if (buttonMapping.find(buttonIndex) != buttonMapping.end()) {
			printd(INFO,
					"Controller::setButtonMapping(): replacing mapping for button with index %i!\n",
					buttonIndex);
			buttonMapping.erase(buttonIndex);
		} // if
		buttonMapping.insert(std::make_pair(buttonIndex, mapping));
		success = true;
	} // if

	return success;
} // setButtonMapping

bool Controller::setAxisMapping(int axisIndex, const DeviceIndexMapping& mapping) {
	bool success = false;
	ScopedOSGLock lock(inputDataLock);

	if (axisIndex >= nAxes) {
		printd(ERROR,
				"Controller::setAxisMapping(): index %i for axis out of range (controller only provides %i axes)!\n",
				axisIndex, nAxes);
	} // else if
	else if (axisIndex < 0){
		printd(ERROR,
				"Controller::setAxisMapping(): invalid axis index %i passed!\n", axisIndex);
	} // else if
	else if (mapping.getDeviceIndex() >= (int)devices.size()) {
		printd(ERROR,
				"Controller::setAxisMapping(): index %i for device out of range (controller only contains %i devices at this time)!\n",
				mapping.getDeviceIndex(), devices.size());
	} // else if
	else if (mapping.getDeviceIndex() < 0) {
		printd(ERROR,
				"Controller::setAxisMapping(): invalid device index %i passed!\n",
				mapping.getDeviceIndex());
	} // else if
	else if (mapping.getIndexInDevice() >=
			devices.at(mapping.getDeviceIndex())->getNumberOfAxes()) {
		printd(ERROR,
				"Controller::setAxisMapping(): index %i for axis in device with index %i out of range (device only contains %i axes)!\n",
				mapping.getIndexInDevice(),
				devices.at(mapping.getDeviceIndex())->getNumberOfAxes());
	} // else if
	else if (mapping.getIndexInDevice() < 0) {
		printd(ERROR,
				"Controller::setAxisMapping(): invalid axis index %i for device %i passed!\n",
				mapping.getIndexInDevice(), mapping.getDeviceIndex());
	} // else if
	else {
		if (axisMapping.find(axisIndex) != axisMapping.end()) {
			printd(INFO,
					"Controller::setAxisMapping(): replacing mapping for axis with index %i!\n",
					axisIndex);
			axisMapping.erase(axisIndex);
		} // if
		axisMapping.insert(std::make_pair(axisIndex, mapping));
		success = true;
	} // if

	return success;
} // setAxisMapping

bool Controller::setSensorMapping(int sensorIndex, const DeviceIndexMapping& mapping) {
	bool success = false;
	ScopedOSGLock lock(inputDataLock);

	if (sensorIndex >= nSensors) {
		printd(ERROR,
				"Controller::setSensorMapping(): index %i for sensor out of range (controller only provides %i sensors)!\n",
				sensorIndex, nSensors);
	} // else if
	else if (sensorIndex < 0){
		printd(ERROR,
				"Controller::setSensorMapping(): invalid sensor index %i passed!\n", sensorIndex);
	} // else if
	else if (mapping.getDeviceIndex() >= (int)devices.size()) {
		printd(ERROR,
				"Controller::setSensorMapping(): index %i for device out of range (controller only contains %i devices at this time)!\n",
				mapping.getDeviceIndex(), devices.size());
	} // else if
	else if (mapping.getDeviceIndex() < 0) {
		printd(ERROR,
				"Controller::setSensorMapping(): invalid device index %i passed!\n",
				mapping.getDeviceIndex());
	} // else if
	else if (mapping.getIndexInDevice() >=
			devices.at(mapping.getDeviceIndex())->getNumberOfSensors()) {
		printd(ERROR,
				"Controller::setSensorMapping(): index %i for sensor in device with index %i out of range (device only contains %i sensors)!\n",
				mapping.getIndexInDevice(),
				devices.at(mapping.getDeviceIndex())->getNumberOfSensors());
	} // else if
	else if (mapping.getIndexInDevice() < 0) {
		printd(ERROR,
				"Controller::setSensorMapping(): invalid sensor index %i for device %i passed!\n",
				mapping.getIndexInDevice(), mapping.getDeviceIndex());
	} // else if
	else {
		if (sensorMapping.find(sensorIndex) != sensorMapping.end()) {
			printd(INFO,
					"Controller::setSensorMapping(): replacing mapping for sensor with index %i!\n",
					sensorIndex);
			sensorMapping.erase(sensorIndex);
		} // if
		sensorMapping.insert(std::make_pair(sensorIndex, mapping));
		success = true;
	} // if

	return success;
} // setSensorMapping

bool Controller::setButtonCorrection(int buttonIndex, const ButtonCorrection& correction) {
	bool success = false;
	ScopedOSGLock lock(inputDataLock);

	if (buttonIndex >= nButtons) {
		printd(ERROR,
				"Controller::setButtonCorrection(): index %i for button out of range (controller only provides %i buttons)!\n",
				buttonIndex, nButtons);
	} // else if
	else if (buttonIndex < 0){
		printd(ERROR,
				"Controller::setButtonCorrection(): invalid button index %i passed!\n", buttonIndex);
	} // else if
	else {
		if (buttonCorrections.find(buttonIndex) != buttonCorrections.end()) {
			printd(INFO,
					"Controller::setButtonCorrection(): replacing correction for button with index %i!\n",
					buttonIndex);
			buttonCorrections.erase(buttonIndex);
		} // if
		buttonCorrections.insert(std::make_pair(buttonIndex, correction));
		success = true;
	} // else

	return success;
} // setButtonCorrection

bool Controller::setAxisCorrection(int axisIndex, const AxisCorrection& correction) {
	bool success = false;
	ScopedOSGLock lock(inputDataLock);

	if (axisIndex >= nAxes) {
		printd(ERROR,
				"Controller::setAxisCorrection(): index %i for axis out of range (controller only provides %i axes)!\n",
				axisIndex, nAxes);
	} // else if
	else if (axisIndex < 0){
		printd(ERROR,
				"Controller::setAxisCorrection(): invalid axis index %i passed!\n", axisIndex);
	} // else if
	else {
		if (axisCorrections.find(axisIndex) != axisCorrections.end()) {
			printd(INFO,
					"Controller::setAxisCorrection(): replacing correction for axis with index %i!\n",
					axisIndex);
			axisCorrections.erase(axisIndex);
		} // if
		axisCorrections.insert(std::make_pair(axisIndex, correction));
		success = true;
	} // else

	return success;
} // setAxisCorrection

bool Controller::setSensorCorrection(int sensorIndex, const SensorCorrection& correction) {
	bool success = false;
	ScopedOSGLock lock(inputDataLock);

	if (sensorIndex >= nSensors) {
		printd(ERROR,
				"Controller::setSensorCorrection(): index %i for sensor out of range (controller only provides %i sensors)!\n",
				sensorIndex, nSensors);
	} // else if
	else if (sensorIndex < 0){
		printd(ERROR,
				"Controller::setSensorCorrection(): invalid sensor index %i passed!\n", sensorIndex);
	} // else if
	else {
		if (sensorCorrections.find(sensorIndex) != sensorCorrections.end()) {
			printd(INFO,
					"Controller::setSensorCorrection(): replacing correction for sensor with index %i!\n",
					sensorIndex);
			sensorCorrections.erase(sensorIndex);
		} // if
		sensorCorrections.insert(std::make_pair(sensorIndex, correction));
		success = true;
	} // else

	return success;
} // setSensorCorrection

const DeviceIndexMapping& Controller::getButtonMapping(int buttonIndex) const {
	ScopedOSGLock lock(inputDataLock);

	if (buttonIndex >= nButtons) {
		printd(WARNING, "Controller::getButtonMapping(): index %i for button out of range (controller only provides %i buttons)!\n",
				buttonIndex, nButtons);
		return UndefinedMapping;
	} // if
	if (buttonIndex < 0) {
		printd(WARNING,
				"Controller::getButtonMapping(): invalid index %i passed!\n", buttonIndex);
		return UndefinedMapping;
	} // if

	std::map<unsigned int, DeviceIndexMapping>::const_iterator it =
		buttonMapping.find(buttonIndex);

	if (it == buttonMapping.end()) {
		return UndefinedMapping;
	} // if

	return it->second;
} // getButtonMapping

const DeviceIndexMapping& Controller::getAxisMapping(int axisIndex) const {
	ScopedOSGLock lock(inputDataLock);

	if (axisIndex >= nAxes) {
		printd(WARNING, "Controller::getAxisMapping(): index %i for axis out of range (controller only provides %i axes)!\n",
				axisIndex, nAxes);
		return UndefinedMapping;
	} // if
	if (axisIndex < 0) {
		printd(WARNING,
				"Controller::getAxisMapping(): invalid index %i passed!\n", axisIndex);
		return UndefinedMapping;
	} // if

	std::map<unsigned int, DeviceIndexMapping>::const_iterator it =
		axisMapping.find(axisIndex);

	if (it == axisMapping.end()) {
		return UndefinedMapping;
	} // if

	return it->second;
} // getAxisMapping

const DeviceIndexMapping& Controller::getSensorMapping(int sensorIndex) const {
	ScopedOSGLock lock(inputDataLock);

	if (sensorIndex >= nSensors) {
		printd(WARNING, "Controller::getSensorMapping(): index %i for sensor out of range (controller only provides %i sensors)!\n",
				sensorIndex, nSensors);
		return UndefinedMapping;
	} // if
	if (sensorIndex < 0) {
		printd(WARNING,
				"Controller::getSensorMapping(): invalid index %i passed!\n", sensorIndex);
		return UndefinedMapping;
	} // if

	std::map<unsigned int, DeviceIndexMapping>::const_iterator it =
		sensorMapping.find(sensorIndex);

	if (it == sensorMapping.end()) {
		return UndefinedMapping;
	} // if

	return it->second;
} // getSensorMapping

const ButtonCorrection& Controller::getButtonCorrection(int buttonIndex) const {
	ScopedOSGLock lock(inputDataLock);

	if (buttonIndex >= nButtons) {
		printd(WARNING, "Controller::getButtonCorrection(): index %i for button out of range (controller only provides %i buttons)!\n",
				buttonIndex, nButtons);
		return DefaultButtonCorrection;
	} // if
	if (buttonIndex < 0) {
		printd(WARNING,
				"Controller::getButtonCorrection(): invalid index %i passed!\n", buttonIndex);
		return DefaultButtonCorrection;
	} // if

	std::map<unsigned int, ButtonCorrection>::const_iterator it =
		buttonCorrections.find(buttonIndex);

	if (it == buttonCorrections.end()) {
		return DefaultButtonCorrection;
	} // if

	return it->second;
} // getButtonCorrection

const AxisCorrection& Controller::getAxisCorrection(int axisIndex) const {
	ScopedOSGLock lock(inputDataLock);

	if (axisIndex >= nAxes) {
		printd(WARNING, "Controller::getAxisCorrection(): index %i for axis out of range (controller only provides %i axes)!\n",
				axisIndex, nAxes);
		return DefaultAxisCorrection;
	} // if
	if (axisIndex < 0) {
		printd(WARNING,
				"Controller::getAxisCorrection(): invalid index %i passed!\n", axisIndex);
		return DefaultAxisCorrection;
	} // if

	std::map<unsigned int, AxisCorrection>::const_iterator it =
		axisCorrections.find(axisIndex);

	if (it == axisCorrections.end()) {
		return DefaultAxisCorrection;
	} // if

	return it->second;
} // getAxisCorrection

const SensorCorrection& Controller::getSensorCorrection(int sensorIndex) const {
	ScopedOSGLock lock(inputDataLock);

	if (sensorIndex >= nSensors) {
		printd(WARNING, "Controller::getSensorCorrection(): index %i for sensor out of range (controller only provides %i sensors)!\n",
				sensorIndex, nSensors);
		return DefaultSensorCorrection;
	} // if
	if (sensorIndex < 0) {
		printd(WARNING,
				"Controller::getSensorCorrection(): invalid index %i passed!\n", sensorIndex);
		return DefaultSensorCorrection;
	} // if

	std::map<unsigned int, SensorCorrection>::const_iterator it =
		sensorCorrections.find(sensorIndex);

	if (it == sensorCorrections.end()) {
		return DefaultSensorCorrection;
	} // if

	return it->second;
} // getSensorCorrection

void Controller::init(int numButtons, int numAxes, int numSensors) {
	assert(nButtons == nAxes == nSensors == 0);
	this->nButtons = numButtons;
	this->nAxes = numAxes;
	this->nSensors = numSensors;
} // init

void Controller::addDevice(InputDevice* dev) {
	ScopedOSGLock lock(inputDataLock);
	int deviceIndex = devices.size();
	dev->initialize(deviceIndex, this, inputDataLock);
	devices.push_back(dev);
} // addDevice

void Controller::swap(const std::auto_ptr<Controller>& rhs) {
	ScopedOSGLock lock(inputDataLock);

	// Following data will not be swapped (because loading a different
	// configuration should not overwrite this data):
	// physicalToWorldScale
	// inputDataLock
	// callbackLock
	// buttonChangeCallbacks

	std::swap(this->nButtons, rhs->nButtons);
	std::swap(this->nAxes, rhs->nAxes);
	std::swap(this->nSensors, rhs->nSensors);
	std::swap(this->buttonMapping, rhs->buttonMapping);
	std::swap(this->axisMapping, rhs->axisMapping);
	std::swap(this->sensorMapping, rhs->sensorMapping);
	std::swap(this->buttonCorrections, rhs->buttonCorrections);
	std::swap(this->axisCorrections, rhs->axisCorrections);
	std::swap(this->sensorCorrections, rhs->sensorCorrections);
	std::swap(this->devices, rhs->devices);
	std::vector<InputDevice*>::iterator itDev;
	int index = 0;
	for (itDev = this->devices.begin(); itDev != this->devices.end(); ++itDev) {
		(*itDev)->initialize(index++, this, this->inputDataLock);
	} // for
	index = 0;
	for (itDev = rhs->devices.begin(); itDev != rhs->devices.end(); ++itDev) {
		(*itDev)->initialize(index++, rhs.get(), rhs->inputDataLock);
	} // for
} // swap

void Controller::deviceButtonChangeNotification(int deviceIndex, int buttonIndex, int newButtonValue) {
	DeviceIndexMapping deviceButton(deviceIndex, buttonIndex);
	int controllerButtonIndex = -1;

	if (inUpdate) {
		// no need to lock because lock is already obtained in update method!!!
		controllerButtonIndex = findControllerButtonIndex(deviceButton);
	}  // if
	else {
		// we have to lock the method call because it accesses the device
		// mappings which may change during runtime (CB could be triggered out
		// of different thread)!
		ScopedOSGLock lock(inputDataLock);
		controllerButtonIndex = findControllerButtonIndex(deviceButton);
	} // else

	if (controllerButtonIndex >= 0) {
		// fix the button value of a correction is registered
		std::map<unsigned int, ButtonCorrection>::const_iterator it;
		if ((it = buttonCorrections.find(controllerButtonIndex)) != buttonCorrections.end()) {
			const ButtonCorrection& correction = it->second;
			newButtonValue = correction.correctButton(newButtonValue);
		} // if

		if (inUpdate) {
			// if we are in update we have to delay the callbacks to avoid
			// deadlocks when a callback method wants to request other data
			// from the controller (like button/axis/sensor values or so)
			delayedButtonChangeCallbacks.push_back(
					ButtonState(controllerButtonIndex, newButtonValue));
		} // if
		else {
			executeButtonChangeCallbacks(controllerButtonIndex, newButtonValue);
		} // else
	} // if
} // deviceButtonChangeNotification

void Controller::executeDelayedCallbacks() {
	assert(!inUpdate);

	std::vector<ButtonState>::iterator itDCB;
	for (itDCB = delayedButtonChangeCallbacks.begin(); itDCB != delayedButtonChangeCallbacks.end(); ++itDCB) {
		executeButtonChangeCallbacks(itDCB->first, itDCB->second);
	} // for
	delayedButtonChangeCallbacks.clear();
} // executeDelayedCallbacks

void Controller::executeButtonChangeCallbacks(int buttonIndex, int newButtonValue) {
	std::vector<AbstractControllerButtonChangeCB*>::iterator itCB;
	ScopedOSGLock lock(callbackLock);

	for (itCB = buttonChangeCallbacks.begin(); itCB != buttonChangeCallbacks.end(); ++itCB) {
		ControllerInterface::call(*itCB, buttonIndex, newButtonValue);
	} // for
} // executeButtonChangeCallbacks

int Controller::findControllerButtonIndex(const DeviceIndexMapping& deviceButton) {
	//TODO: improve finding of controller button index (maybe by using maps)
	int result = -1;
	std::map<unsigned int, DeviceIndexMapping>::iterator itMap;
	for (itMap = buttonMapping.begin(); itMap != buttonMapping.end(); ++itMap) {
		if (itMap->second == deviceButton) {
			result = itMap->first;
			break;
		} // if
	} // for
	return result;
} // findControllerButtonIndex
