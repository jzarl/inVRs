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

#include "ControllerManager.h"

#include <assert.h>

#include "../InputInterface.h"
#include "InputDevice.h"
#ifndef DISABLE_GLUT_DEVICES
# include "GlutMouseDevice.h"
# include "GlutCharKeyboardDevice.h"
# include "GlutSensorEmulatorDevice.h"
#endif
#include "UdpDevice.h"
#include "StrangeDevice.h"

#include "../../SystemCore/XMLTools.h"
#include "../../SystemCore/Configuration.h"
#include "../../SystemCore/DebugOutput.h"
#include "../../SystemCore/SystemCore.h"
#include "../../SystemCore/UtilityFunctions.h"
#include <inVRs/SystemCore/ComponentInterfaces/module_core_api.h>

// disable deprecation warning for std::auto_ptr when std::unique_ptr is not available.
#ifndef HAS_CXX11_UNIQUE_PTR
// Only do this on GCC versions that support it:
# if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 402
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
#endif

using namespace std;

XmlConfigurationLoader	ControllerManager::xmlConfigLoader;

ControllerManager::ControllerManager() :
	isCleanedUp(false),
	controller(new Controller()) {
#ifndef DISABLE_GLUT_DEVICES
	factoryList.push_back(new GlutMouseDeviceFactory);
	factoryList.push_back(new GlutCharKeyboardDeviceFactory);
	factoryList.push_back(new GlutSensorEmulatorDeviceFactory);
#endif
	factoryList.push_back(new UdpDeviceFactory);
	factoryList.push_back(new StrangeDeviceFactory);
	xmlConfigLoader.registerConverter(new ConverterToV1_0a4);
} // ControllerManager

ControllerManager::~ControllerManager() {
	if (!isCleanedUp)
		cleanup();
} // ~ControllerManager

void ControllerManager::cleanup() {
	for (int i = 0; i < (int)factoryList.size(); i++)
		delete factoryList[i];
	factoryList.clear();

	isCleanedUp = true;
} // cleanup

bool ControllerManager::loadConfig(std::string configFile) {

	std::string configFileConcatenatedPath = configFile;
	if (!Configuration::containsPath("ControllerManagerConfiguration") &&
			Configuration::containsPath("ControllerConfiguration")) {
		printd(WARNING,
				"ControllerManager::loadConfig(): Deprecated path entry ControllerConfiguration found in general configuration file! Use ControllerManagerConfiguration instead!\n");
		configFileConcatenatedPath = getConcatenatedPath(configFile, "ControllerConfiguration");
	} else {
		configFileConcatenatedPath = getConcatenatedPath(configFile, "ControllerManagerConfiguration");
	} // else
	printd(INFO, "ControllerManager::loadConfig(): loading configuration %s!\n",
			configFile.c_str());

	// load xml document via xmlConfigLoader
	std::auto_ptr<const XmlDocument> document(
			xmlConfigLoader.loadConfiguration(configFileConcatenatedPath));
	if (!document.get()) {
		printd(ERROR,
				"ControllerManager::loadConfig(): error at loading of configuration file %s!\n",
				configFile.c_str());
		return false;
	} // if

	bool success = true;

	std::auto_ptr<Controller> newController(new Controller());

	// check if all necessary attributes are available
	if (!document->hasAttribute("controllerManager.controller.buttons") ||
			!document->hasAttribute("controllerManager.controller.axes") ||
			!document->hasAttribute("controllerManager.controller.sensors")) {
		printd(ERROR,
				"ControllerManager::loadConfig(): missing attributes buttons, axes or sensors in <controller> element! Please fix your ControllerManager configuration file!\n");
		return false;
	} // if

	// get number of buttons axes and sensors
	int numButtons = document->getAttributeValueAsInt("controllerManager.controller.buttons");
	int numAxes = document->getAttributeValueAsInt("controllerManager.controller.axes");
	int numSensors = document->getAttributeValueAsInt("controllerManager.controller.sensors");
	newController->init(numButtons, numAxes, numSensors);

	// parse all devices
	std::vector<const XmlElement*> deviceElements =
		document->getElements("controllerManager.controller.device");
	std::vector<const XmlElement*>::iterator deviceIt;
	int deviceIndex = 0;
	for (deviceIt = deviceElements.begin(); deviceIt != deviceElements.end(); ++deviceIt) {
		success = parseDeviceElement(*deviceIt, deviceIndex, newController.get()) && success;
		deviceIndex++;
	} // for

	// swap members of newController and controller so that controller is deleted by destructor
	// of auto_ptr instead of newController
	if (success) {
		controller->swap(newController);
	} // if

	return success;
} // loadConfig

std::string ControllerManager::getName() {
	return "ControllerManager";
} // getName

ControllerInterface* ControllerManager::getController() {
	return controller.get();
} // getController

void ControllerManager::registerInputDeviceFactory(InputDeviceFactory* factory) {
	factoryList.push_back(factory);
} // registerInputDeviceFactory

InputDevice* ControllerManager::getInputDevice(std::string deviceName, ArgumentVector* args) {
	int i;
	InputDevice* result = NULL;
	int size = factoryList.size();

	for (i = 0; i < size; i++) {
		result = factoryList[i]->create(deviceName, args);
		if (result)
			break;
	} // for
	return result;
} // getInputDevice

bool ControllerManager::parseDeviceElement(const XmlElement* deviceElement, int deviceIndex,
		Controller* newController) {

	bool success = true;

	std::string deviceType;
	std::vector<const XmlElement*> elements;
	std::vector<const XmlElement*>::iterator it;

	std::map<int, DeviceIndexMapping> buttonIndexMapping;
	std::map<int, ButtonCorrection> buttonCorrections;
	std::map<int, DeviceIndexMapping> axisIndexMapping;
	std::map<int, DeviceIndexMapping> sensorIndexMapping;
	std::map<int, AxisCorrection> axisCorrections;
	std::map<int, SensorCorrection> sensorCorrections;

	// create auto_ptr for arguments (which are deleted automatically after each loop iteration)
	std::auto_ptr<ArgumentVector> arguments;

	// check for type entry
	if (!deviceElement->hasAttribute("type")) {
		printd(ERROR,
				"ControllerManager::parseDeviceElement(): missing attribute type in <device> element found! Please fix your ControllerManager configuration file!\n");
		return false;;
	} // if
	deviceType = deviceElement->getAttributeValue("type");

	// read optional arguments
	if (deviceElement->hasSubElement("arguments")) {
		arguments.reset(readArgumentsFromXmlElement(deviceElement));
	} // if

	// create new InputDevice
	InputDevice* device = getInputDevice(deviceType, arguments.get());
	if (!device) {
		printd(ERROR,
				"ControllerManager::parseDeviceElement(): Unable to create device of type %s! Please check the ControllerManager configuration file!\n",
				deviceType.c_str());
		return false;
	} // if

	// parse all button elements
	elements = deviceElement->getSubElements("button");
	for (it = elements.begin(); it != elements.end(); ++it) {
		success = parseButtonElement(*it, deviceIndex, buttonIndexMapping, buttonCorrections)
				&& success;
	} // for

	// parse all axis elements
	elements = deviceElement->getSubElements("axis");
	for (it = elements.begin(); it != elements.end(); ++it) {
		success = parseAxisElement(*it, deviceIndex, axisIndexMapping, axisCorrections) && success;
	} // for

	// parse all sensor elements
	elements = deviceElement->getSubElements("sensor");
	for (it = elements.begin(); it != elements.end(); ++it) {
		success = parseSensorElement(*it, deviceIndex, sensorIndexMapping, sensorCorrections)
				&& success;
	} // for

	if (success) {
		newController->addDevice(device);
		std::map<int, DeviceIndexMapping>::const_iterator it;
		std::map<int, ButtonCorrection>::const_iterator itButton;
		std::map<int, AxisCorrection>::const_iterator itAxis;
		std::map<int, SensorCorrection>::const_iterator itSensor;
		for (it = buttonIndexMapping.begin(); it != buttonIndexMapping.end(); ++it) {
			success = newController->setButtonMapping(it->first, it->second) && success;
		} // for
		for (it = axisIndexMapping.begin(); it != axisIndexMapping.end(); ++it) {
			success = newController->setAxisMapping(it->first, it->second) && success;
		} // for
		for (it = sensorIndexMapping.begin(); it != sensorIndexMapping.end(); ++it) {
			success = newController->setSensorMapping(it->first, it->second) && success;
		} // for
		for (itButton = buttonCorrections.begin(); itButton != buttonCorrections.end();
				++itButton) {
			success = newController->setButtonCorrection(itButton->first, itButton->second)
					&& success;
		} // for
		for (itAxis = axisCorrections.begin(); itAxis != axisCorrections.end(); ++itAxis) {
			success = newController->setAxisCorrection(itAxis->first, itAxis->second)
					&& success;
		} // for
		for (itSensor = sensorCorrections.begin(); itSensor != sensorCorrections.end(); ++itSensor) {
			success = newController->setSensorCorrection(itSensor->first, itSensor->second)
					&& success;
		} // for
	} // if

	return success;
} // parseDeviceElement

bool ControllerManager::parseButtonElement(const XmlElement* buttonElement, int deviceIndex,
		std::map<int, DeviceIndexMapping>& buttonIndexMapping,
		std::map<int, ButtonCorrection>& buttonCorrections) {

	bool success = true;
	int indexInController = -1;

	success = parseDeviceIndexMapping(buttonElement, deviceIndex, buttonIndexMapping, "button",
			indexInController) && success;

	if (success && buttonElement->hasSubElement("buttonCorrection")) {
		assert(indexInController >= 0);
		std::string invertString = buttonElement->getAttributeValue("buttonCorrection.invert");
		bool invert;
		convert(invertString.c_str(), invert);
		ButtonCorrection correction = ButtonCorrection(invert);
		buttonCorrections.insert(std::make_pair(indexInController, correction));
	} // if

	return success;
} // parseButtonElement

bool ControllerManager::parseAxisElement(const XmlElement* axisElement, int deviceIndex,
		std::map<int, DeviceIndexMapping>& axisIndexMapping,
		std::map<int, AxisCorrection>& axisCorrections) {

	bool success = true;
	int indexInController = -1;

	success = parseDeviceIndexMapping(axisElement, deviceIndex, axisIndexMapping, "axis",
			indexInController) && success;

	if (success && axisElement->hasSubElement("axisCorrection")) {
		float offset = axisElement->getAttributeValueAsFloat("axisCorrection.offset");
		float scale = axisElement->getAttributeValueAsFloat("axisCorrection.scale");
		AxisCorrection correction = AxisCorrection(offset, scale);
		axisCorrections.insert(std::make_pair(indexInController, correction));
	} // if
	else if (success && axisElement->hasSubElement("axisValues")) {
		float minValue = axisElement->getAttributeValueAsFloat("axisValues.minValue");
		float maxValue = axisElement->getAttributeValueAsFloat("axisValues.maxValue");
		float offset = - (minValue + maxValue) / 2.0f;
		float scale = 2.0f / (maxValue - minValue);
		AxisCorrection correction = AxisCorrection(offset, scale);
		axisCorrections.insert(std::make_pair(indexInController, correction));
	} // if
	return success;
} // parseAxisElement

bool ControllerManager::parseSensorElement(const XmlElement* sensorElement, int deviceIndex,
		std::map<int, DeviceIndexMapping>& sensorIndexMapping,
		std::map<int, SensorCorrection>& sensorCorrections) {

	bool success = true;
	int indexInController = -1;

	success = parseDeviceIndexMapping(sensorElement, deviceIndex, sensorIndexMapping, "axis",
			indexInController) && success;

	bool foundCorrection = false;
	TransformationData coordinateCorrection = IdentityTransformation;
	TransformationData positionCorrection = IdentityTransformation;
	TransformationData orientationCorrection = IdentityTransformation;

	if (sensorElement->hasSubElement("coordinateSystemCorrection")) {
		const XmlElement* element = sensorElement->getSubElement("coordinateSystemCorrection");
		readTransformationDataFromXmlElement(coordinateCorrection, element);
		foundCorrection = true;
	} // if
	if (sensorElement->hasSubElement("positionCorrection")) {
		const XmlElement* element = sensorElement->getSubElement("positionCorrection");
		readTransformationDataFromXmlElement(positionCorrection, element);
		foundCorrection = true;
	} // if
	if (sensorElement->hasSubElement("orientationCorrection")) {
		const XmlElement* element = sensorElement->getSubElement("orientationCorrection");
		readTransformationDataFromXmlElement(orientationCorrection, element);
		foundCorrection = true;
	} // if

	if (foundCorrection) {
		SensorCorrection correction = SensorCorrection(coordinateCorrection, positionCorrection,
				orientationCorrection);
		sensorCorrections.insert(std::make_pair(indexInController, correction));
	} // if
	return success;
} // parseAxisElement

bool ControllerManager::parseDeviceIndexMapping(const XmlElement* element, int deviceIndex,
		std::map<int, DeviceIndexMapping>& indexMapping, std::string type, int& indexInController) {

	int indexInDevice;

	// check for required attributes
	if (!element->hasAttribute("deviceIndex") ||
			!element->hasAttribute("controllerIndex")) {
		printd(ERROR,
				"ControllerManager::parseDeviceIndexMapping(): missing attributes deviceIndex or controllerIndex in <%s> element! Please fix your ControllerManager configuration file!\n",
				type.c_str());
		return false;
	} // if

	// read indices and create DeviceIndexMapping entry
	indexInDevice = element->getAttributeValueAsInt("deviceIndex");
	indexInController = element->getAttributeValueAsInt("controllerIndex");
	DeviceIndexMapping buttonMapping = DeviceIndexMapping(deviceIndex, indexInDevice);
	if (indexMapping.find(indexInController) != indexMapping.end()) {
		printd(ERROR,
				"ControllerManager::parseDeviceIndexMapping(): Mapping for %s with controllerIndex %i occurs multiple times! Please fix your ControllerManager configuration!\n",
				type.c_str(), indexInController);
		return false;
	} // if
	indexMapping.insert(make_pair(indexInController, buttonMapping));

	return true;
} // parseDeviceIndexMapping

//*****************************************************************************
// Configuration loading
//*****************************************************************************
ControllerManager::ConverterToV1_0a4::ConverterToV1_0a4() :
	XmlConfigurationConverter("0.0", "1.0a4") {

} // ConverterToV1_0a4

bool ControllerManager::ConverterToV1_0a4::convert(XmlDocument* document, const Version& version,
		std::string configFile) {
	bool success = true;

	// obtain all needed elements
	XmlElement* rootElement = document->getElement("controller");
	XmlElement* dataElement = document->getElement("controller.data");
//	std::vector<XmlElement*> deviceElements = document->getElements("controller.device");
	// check for required elements
	if (!rootElement) {
		printd(ERROR,
				"ControllerManager::ConverterToV1_0a4::convert(): could not convert ControllerManager configuration file because of invalid root node <%s> (expected <controller>)! Please fix your configuration file!\n",
				document->getRootElement()->getName().c_str());
		return false;
	} // if
	if (!dataElement) {
		printd(ERROR,
				"ControllerManager::ConverterToV1_0a4::convert(): could not convert ControllerManager configuration file because of missing node <data>! Please fix your configuration file!\n",
				document->getRootElement()->getName().c_str());
		return false;
	} // if

	// move attributes from data element into controller element and remove data element
	XmlAttribute* buttonsAttribute = dataElement->removeAttribute("buttons");
	XmlAttribute* axesAttribute = dataElement->removeAttribute("axes");
	XmlAttribute* sensorsAttribute = dataElement->removeAttribute("sensors");
	if (!buttonsAttribute || !axesAttribute || !sensorsAttribute) {
		printd(ERROR,
				"ControllerManager::ConverterToV1_0a4::convert(): missing attribute buttons, axes or sensors in <data> element! Please fix your ControllerManager configuration file!\n",
				document->getRootElement()->getName().c_str());
		return false;
	} // if
	rootElement->addAttribute(buttonsAttribute);
	rootElement->addAttribute(axesAttribute);
	rootElement->addAttribute(sensorsAttribute);
	rootElement->removeSubElement(dataElement);
	delete dataElement;

	// replace old root element with new one
	XmlElement* newRootElement = new XmlElement("controllerManager");
	newRootElement->addSubElement(rootElement);
	document->replaceRootElement(newRootElement);

	// update argument elements (if existing)
	std::vector<XmlElement*> elements =
		document->getElements("controllerManager.controller.device.arguments");
	std::vector<XmlElement*>::iterator it;
	for (it = elements.begin(); it != elements.end(); ++it) {
		success = XmlArgumentVectorLoader::get().updateXmlElement(*it, version,	destinationVersion,
				configFile);
	} // for

	// update adjustment elements
	elements = document->getElements("controllerManager.controller.device.sensor.adjustment");
	for (it = elements.begin(); it != elements.end(); ++it) {
		success = XmlTransformationLoader::get().updateXmlElement(*it, version, destinationVersion,
				configFile);
	} // for

	// update device elements
	document->renameAttributes("controllerManager.controller.device.name", "type");
	document->renameAttributes("controllerManager.controller.device.button.devIdx", "deviceIndex");
	document->renameAttributes("controllerManager.controller.device.button.contIdx", "controllerIndex");
	document->renameAttributes("controllerManager.controller.device.axis.devIdx", "deviceIndex");
	document->renameAttributes("controllerManager.controller.device.axis.contIdx", "controllerIndex");
	document->renameAttributes("controllerManager.controller.device.axis.axisValues.min", "minValue");
	document->renameAttributes("controllerManager.controller.device.axis.axisValues.max", "maxValue");
	document->renameAttributes("controllerManager.controller.device.sensor.devIdx", "deviceIndex");
	document->renameAttributes("controllerManager.controller.device.sensor.contIdx", "controllerIndex");
	document->renameElements("controllerManager.controller.device.sensor.adjustment", "coordinateSystemCorrection");

	// update the dtd and version entries
	updateDocument(document, XmlConfigurationLoader::XmlDtdUrl + "controllerManager_v1.0a4.dtd",
			destinationVersion, "controllerManager");

	return success;
} // convert


MAKEMODULEPLUGIN(ControllerManager, InputInterface)

