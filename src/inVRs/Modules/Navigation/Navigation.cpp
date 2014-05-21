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

#include "Navigation.h"

#include <deque>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <gmtl/QuatOps.h>
#include <gmtl/AxisAngle.h>
#include <gmtl/Generate.h>
#include <gmtl/MatrixOps.h>

#include "NavigationEvents.h"
#include "AllInOneTranslationModel.h"
#include "AllInOneTranslationButtonModel.h"
#include <inVRs/SystemCore/Configuration.h>
#include <inVRs/InputInterface/ControllerManagerInterface.h>
#include <inVRs/SystemCore/UserDatabase/User.h>
#include <inVRs/SystemCore/UserDatabase/UserDatabase.h>
#include <inVRs/SystemCore/DebugOutput.h>
#include <inVRs/SystemCore/ModuleIds.h>
#include <inVRs/SystemCore/XMLTools.h>
#include <inVRs/SystemCore/SystemCore.h>
#include <inVRs/SystemCore/UtilityFunctions.h>
#include <inVRs/InputInterface/InputInterface.h>
#include <inVRs/SystemCore/ComponentInterfaces/module_core_api.h>

// disable deprecation warning for std::auto_ptr when std::unique_ptr is not available.
#ifndef HAS_CXX11_UNIQUE_PTR
// Only do this on GCC versions that support it:
# if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 402
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
#endif

XmlConfigurationLoader Navigation::xmlConfigLoader;

Navigation::Navigation() {
	int i;
	controller = NULL;
	speedModel = NULL;
	translationModel = NULL;
	orientationModel = NULL;
	incomingEvents = NULL;
	suspendNavigation = false;
	// 	localUser = NULL;

	navProvidedSpeedModelFactories.push_back(new DefaultSpeedModelFactory);
	navProvidedOrientationModelFactories.push_back(new DefaultOrientationModelFactory);
	navProvidedTranslationModelFactories.push_back(new DefaultTranslationModelFactory);
	navProvidedTranslationModelFactories.push_back(new AllInOneTranslationModelFactory);
	navProvidedTranslationModelFactories.push_back(new AllInOneTranslationButtonModelFactory);

	for (i = 0; i < (int)navProvidedSpeedModelFactories.size(); i++)
		speedModelFactories.push_back(navProvidedSpeedModelFactories[i]);

	for (i = 0; i < (int)navProvidedOrientationModelFactories.size(); i++)
		orientationModelFactories.push_back(navProvidedOrientationModelFactories[i]);

	for (i = 0; i < (int)navProvidedTranslationModelFactories.size(); i++)
		translationModelFactories.push_back(navProvidedTranslationModelFactories[i]);

	incomingEvents = EventManager::getPipe(NAVIGATION_MODULE_ID);
	EventManager::registerEventFactory("NavigationPauseEvent", new NavigationPauseEvent::Factory());
	EventManager::registerEventFactory("NavigationResumeEvent",
			new NavigationResumeEvent::Factory());

	xmlConfigLoader.registerConverter(new ConverterToV1_0a4);
} // Navigation

Navigation::~Navigation() {
	// IS DONE VIA PLUGIN-concept
	//	SystemCore::unregisterModuleInterface(this);
} // ~Navigation

bool Navigation::loadConfig(std::string configFile) {

	std::string configFileConcatenatedPath = configFile;
	if (!Configuration::containsPath("NavigationModuleConfiguration") &&
			Configuration::containsPath("NavigationConfiguration")) {
		printd(WARNING,
				"Navigation::loadConfig(): Deprecated path entry NavigationConfiguration found in general configuration file! Use NavigationModuleConfiguration instead!\n");
		configFileConcatenatedPath = getConcatenatedPath(configFile, "NavigationConfiguration");
	} else {
		configFileConcatenatedPath = getConcatenatedPath(configFile, "NavigationModuleConfiguration");
	} // else

	// load xml document via xmlConfigLoader
	std::auto_ptr<const XmlDocument> document(
			xmlConfigLoader.loadConfiguration(configFileConcatenatedPath));
	if (!document.get()) {
		printd(ERROR,
				"Navigation::loadConfig(): error at loading of configuration file %s!\n",
				configFile.c_str());
		return false;
	} // if

	bool success = true;

	// get elements for all three models
	const XmlElement* translationModelElement = document->getElement("navigation.translationModel");
	const XmlElement* orientationModelElement = document->getElement("navigation.orientationModel");
	const XmlElement* speedModelElement = document->getElement("navigation.speedModel");

	// delete old models (if used)
	if (translationModel) {
		delete translationModel;
		translationModel = NULL;
	} // if
	if (orientationModel) {
		delete orientationModel;
		orientationModel = NULL;
	} // if
	if (speedModel) {
		delete speedModel;
		speedModel = NULL;
	} // if

	// load translationModel
	std::string type;
	if (translationModelElement) {
		std::auto_ptr<ArgumentVector> arguments;
		type = translationModelElement->getAttributeValue("type");
		if (translationModelElement->hasSubElement("arguments")) {
			arguments.reset(readArgumentsFromXmlElement(translationModelElement));
		} // if
		printd(INFO, "Navigation::loadConfig(): trying to load translation model %s\n",
				type.c_str());
		translationModel = getTranslationModel(type, arguments.get());
		if (translationModel) {
			printd(INFO, "Navigation::loadConfig(): successfully loaded translation model %s\n",
					type.c_str());
		} else {
			printd(ERROR,
					"Navigation::loadConfig(): failed to load translation model %s\n",
					type.c_str());
			success = false;
		} // else
	} // if
	else {
		printd(WARNING,
				"Navigation::loadConfig(): no TranslationModel set in Navigation module configuration! TranslationModel will be ignored!\n");
	} // else

	// load orientationModel
	if (orientationModelElement) {
		std::auto_ptr<ArgumentVector> arguments;
		type = orientationModelElement->getAttributeValue("type");
		float angle = orientationModelElement->getAttributeValueAsFloat("angle");
		if (orientationModelElement->hasSubElement("arguments")) {
			arguments.reset(readArgumentsFromXmlElement(orientationModelElement));
		} // if
		printd(INFO, "Navigation::loadConfig(): trying to load orientation model %s\n",
				type.c_str());
		orientationModel = getOrientationModel(type, arguments.get());
		if (orientationModel) {
			orientationModel->setAngle(angle);
			printd(INFO, "Navigation::loadConfig(): successfully loaded orientation model %s\n",
					type.c_str());
		} else {
			printd(ERROR,
					"Navigation::loadConfig(): failed to load orientation model %s\n",
					type.c_str());
			success = false;
		} // else
	} // if
	else {
		printd(WARNING,
				"Navigation::loadConfig(): no OrientationModel set in Navigation module configuration! OrientationModel will be ignored!\n");
	} // else

	// load speedModel
	if (speedModelElement) {
		std::auto_ptr<ArgumentVector> arguments;
		type = speedModelElement->getAttributeValue("type");
		float speed = speedModelElement->getAttributeValueAsFloat("speed");
		if (speedModelElement->hasSubElement("arguments")) {
			arguments.reset(readArgumentsFromXmlElement(speedModelElement));
		} // if
		printd(INFO, "Navigation::loadConfig(): trying to load speed model %s\n",
				type.c_str());
		speedModel = getSpeedModel(type, arguments.get());
		if (speedModel) {
			speedModel->setSpeed(speed);
			printd(INFO, "Navigation::loadConfig(): successfully loaded speed model %s\n",
					type.c_str());
		} else {
			printd(ERROR,
					"Navigation::loadConfig(): failed to load speed model %s\n",
					type.c_str());
			success = false;
		} // else
	} // if
	else {
		printd(WARNING,
				"Navigation::loadConfig(): no SpeedModel set in Navigation module configuration! SpeedModel will be ignored!\n");
	} // else

	return success;
} // loadConfig

void Navigation::cleanup() {
	int i;
	SpeedModelFactory* speedModelFactory;
	OrientationModelFactory* orientationModelFactory;
	TranslationModelFactory* translationModelFactory;

	for (i = navProvidedSpeedModelFactories.size() - 1; i >= 0; i--) {
		speedModelFactory = navProvidedSpeedModelFactories[i];
		navProvidedSpeedModelFactories.pop_back();
		delete speedModelFactory;
	} // for

	for (i = navProvidedOrientationModelFactories.size() - 1; i >= 0; i--) {
		orientationModelFactory = navProvidedOrientationModelFactories[i];
		navProvidedOrientationModelFactories.pop_back();
		delete orientationModelFactory;
	} // for

	for (i = navProvidedTranslationModelFactories.size() - 1; i >= 0; i--) {
		translationModelFactory = navProvidedTranslationModelFactories[i];
		navProvidedTranslationModelFactories.pop_back();
		delete translationModelFactory;
	} // for
	speedModelFactories.clear();
	orientationModelFactories.clear();
	translationModelFactories.clear();

	if (translationModel)
		delete translationModel;
	if (orientationModel)
		delete orientationModel;
	if (speedModel)
		delete speedModel;
} // cleanup

std::string Navigation::getName() {
	return "Navigation";
} // getName

NavigationInfo Navigation::navigate(float dt) {
	NavigationInfo ret;

	gmtl::Quatf orientation = gmtl::QUAT_IDENTITYF;
	gmtl::Vec3f position(0, 0, 0);
	ret.translation = position;
	ret.rotation = orientation;
	float speed = 1.0f;
	Event* evt = NULL;
	std::deque<Event*>* incomingEventsLocal = NULL;
	// 	User* localUser;

	ControllerManagerInterface* contIntf;

	// execute events first:
	if (incomingEvents) {
		incomingEventsLocal = incomingEvents->makeCopyAndClear();
		while (!incomingEventsLocal->empty()) {
			evt = (*incomingEventsLocal)[0];
			incomingEventsLocal->pop_front();
			evt->execute();
			delete evt;
		} // while
		delete incomingEventsLocal;
	} // if
	else
		printd(ERROR, "Navigation::navigate(): event pipe not initialized!\n");

	if (suspendNavigation) {
		ret.translation = gmtl::Vec3f(0, 0, 0);
		ret.rotation = gmtl::QUAT_IDENTITYF;
		return ret;
	} // if

	// in case the controller was initialized after creation of Navigation object
	if (controller == NULL) {
		contIntf = (ControllerManagerInterface*)(InputInterface::getModuleByName("ControllerManager"));
		if (!contIntf) {
			printd(ERROR, "Navigation::navigate(): cannot find a suitable ControllerManager\n");
			return ret;
		} // if

		controller = contIntf->getController();
	} // if

	if (orientationModel)
		orientationModel->getOrientation(controller, orientation, dt);

	if (translationModel)
		translationModel->getTranslation(controller, orientation, position, dt);

	if (speedModel)
		speed = speedModel->getSpeed(controller, orientation, position, dt);

	ret.rotation = orientation;
	ret.translation = position * speed;

	return ret;
} // navigate

void Navigation::registerSpeedModelFactory(SpeedModelFactory* factory) {
	speedModelFactories.push_back(factory);
} // registerSpeedModelFactory

void Navigation::registerOrientationModelFactory(OrientationModelFactory* factory) {
	orientationModelFactories.push_back(factory);
} // registerOrientationModelFactory

void Navigation::registerTranslationModelFactory(TranslationModelFactory* factory) {
	translationModelFactories.push_back(factory);
} // registerTranslationModelFactory

SpeedModel* Navigation::getSpeedModel(std::string name, ArgumentVector* argVec) {
	int i;
	SpeedModel* ret;

	for (i = 0; i < (int)speedModelFactories.size(); i++) {
		ret = speedModelFactories[i]->create(name, argVec);
		if (ret)
			return ret;
	} // for

	return NULL;
} // getSpeedModel

OrientationModel* Navigation::getOrientationModel(std::string name, ArgumentVector* argVec) {
	int i;
	OrientationModel* ret;

	for (i = 0; i < (int)orientationModelFactories.size(); i++) {
		ret = orientationModelFactories[i]->create(name, argVec);
		if (ret)
			return ret;
	} // for

	return NULL;
} // getOrientationModel

TranslationModel* Navigation::getTranslationModel(std::string name, ArgumentVector* argVec) {
	int i;
	TranslationModel* ret;

	for (i = 0; i < (int)translationModelFactories.size(); i++) {
		ret = translationModelFactories[i]->create(name, argVec);
		if (ret)
			return ret;
	} // for

	return NULL;
} // getTranslationModel

//*****************************************************************************
// Configuration loading
//*****************************************************************************
Navigation::ConverterToV1_0a4::ConverterToV1_0a4() :
	XmlConfigurationConverter("0.0", "1.0a4") {

} // ConverterToV1_0a4

bool Navigation::ConverterToV1_0a4::convert(XmlDocument* document, const Version& version,
		std::string configFile) {
	bool success = true;

	updateDocument(document, XmlConfigurationLoader::XmlDtdUrl + "navigation_v1.0a4.dtd",
			destinationVersion, "navigation");

	// remove unnecessary <models> element
	XmlElement* rootElement = document->getRootElement();
	XmlElement* models = document->getElement("navigation.models");
	if (!models) {
		printd(ERROR,
				"Navigation::ConverterToV1_0a4::convert(): missing element <models>! Please fix your Navigation module configuration!\n");
		return false;
	} // if
	// detach models element from root
	rootElement->removeSubElement(models);
	std::vector<XmlElement*> subElements = models->getAllSubElements();
	std::vector<XmlElement*>::iterator it;
	XmlElement* subElement;
	// move all subelements of models to root
	for (it = subElements.begin(); it != subElements.end(); ++it) {
		subElement = *it;
		models->removeSubElement(subElement);
		rootElement->addSubElement(subElement);
		// update argument vector (if available)
		if (subElement->hasSubElement("arguments")) {
			XmlElement* argumentElement = subElement->getSubElement("arguments");
			XmlArgumentVectorLoader::get().updateXmlElement(argumentElement, version,
					destinationVersion, configFile);
		} // if
	} // for
	delete models;

	// rename element names
	document->renameElements("navigation.translationmodel", "translationModel");
	document->renameElements("navigation.orientationmodel", "orientationModel");
	document->renameElements("navigation.speedmodel", "speedModel");

	// rename attribute names
	document->renameAttributes("navigation.translationModel.name", "type");
	document->renameAttributes("navigation.orientationModel.name", "type");
	document->renameAttributes("navigation.speedModel.name", "type");

	// update deprecated model names
	document->replaceAttributeValues("navigation.orientationModel.type",
			"OrientationRelativeSensorFlyModel", "OrientationSensorFlyModel");
	document->replaceAttributeValues("navigation.orientationModel.type",
			"OrientationAbsoluteButtonModel", "OrientationButtonModel");
	document->replaceAttributeValues("navigation.orientationModel.type",
			"OrientationAbsoluteAxisModel", "OrientationSingleAxisModel");
	document->replaceAttributeValues("navigation.orientationModel.type",
			"OrientationAbsoluteSingleAxisModel", "OrientationSingleAxisModel");
	document->replaceAttributeValues("navigation.orientationModel.type",
			"OrientationAbsoluteDualAxisModel", "OrientationDualAxisModel");
	document->replaceAttributeValues("navigation.orientationModel.type",
			"OrientationAbsoluteSensorModel", "OrientationSensorModel");
	document->replaceAttributeValues("navigation.speedModel.type",
			"SpeedAbsoluteAxisModel", "SpeedAxisModel");
	document->replaceAttributeValues("navigation.speedModel.type",
			"SpeedAbsoluteButtonModel", "SpeedButtonModel");
	document->replaceAttributeValues("navigation.speedModel.type",
			"SpeedAbsoluteMultiButtonModel", "SpeedMultiButtonModel");
	document->replaceAttributeValues("navigation.speedModel.type",
			"SpeedAbsolute2AxisModel", "SpeedDualAxisModel");

	// rename Idx entries to Index
	document->replaceAttributeValues("navigation.speedModel.arguments.arg.key",
			"axisIdx", "axisIndex");
	document->replaceAttributeValues("navigation.speedModel.arguments.arg.key",
			"accelButtonIdx", "accelButtonIndex");
	document->replaceAttributeValues("navigation.speedModel.arguments.arg.key",
			"decelButtonIdx", "decelButtonIndex");
	document->replaceAttributeValues("navigation.speedModel.arguments.arg.key",
			"axis1Idx", "axis1Index");
	document->replaceAttributeValues("navigation.speedModel.arguments.arg.key",
			"axis2Idx", "axis2Index");

	document->replaceAttributeValues("navigation.translationModel.arguments.arg.key",
			"sensorIdx", "sensorIndex");
	document->replaceAttributeValues("navigation.translationModel.arguments.arg.key",
			"frontIdx", "frontIndex");
	document->replaceAttributeValues("navigation.translationModel.arguments.arg.key",
			"backIdx", "backIndex");
	document->replaceAttributeValues("navigation.translationModel.arguments.arg.key",
			"leftIdx", "leftIndex");
	document->replaceAttributeValues("navigation.translationModel.arguments.arg.key",
			"rightIdx", "rightIndex");
	document->replaceAttributeValues("navigation.translationModel.arguments.arg.key",
			"leftRightIdx", "leftRightIndex");
	document->replaceAttributeValues("navigation.translationModel.arguments.arg.key",
			"frontBackIdx", "frontBackIndex");

	document->replaceAttributeValues("navigation.orientationModel.arguments.arg.key",
			"sensorIdx", "sensorIndex");
	document->replaceAttributeValues("navigation.orientationModel.arguments.arg.key",
			"buttonIdx", "buttonIndex");
	document->replaceAttributeValues("navigation.orientationModel.arguments.arg.key",
			"leftIdx", "leftIndex");
	document->replaceAttributeValues("navigation.orientationModel.arguments.arg.key",
			"rightIdx", "rightIndex");
	document->replaceAttributeValues("navigation.orientationModel.arguments.arg.key",
			"upIdx", "upIndex");
	document->replaceAttributeValues("navigation.orientationModel.arguments.arg.key",
			"downIdx", "downIndex");
	document->replaceAttributeValues("navigation.orientationModel.arguments.arg.key",
			"cwIdx", "clockwiseIndex");
	document->replaceAttributeValues("navigation.orientationModel.arguments.arg.key",
			"ccwIdx", "counterclockwiseIndex");
	document->replaceAttributeValues("navigation.orientationModel.arguments.arg.key",
			"axisIdx", "axisIndex");
	document->replaceAttributeValues("navigation.orientationModel.arguments.arg.key",
			"xAxisIdx", "xAxisIndex");
	document->replaceAttributeValues("navigation.orientationModel.arguments.arg.key",
			"yAxisIdx", "yAxisIndex");


	return success;
} // convert

MAKEMODULEPLUGIN(Navigation, SystemCore)
