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

#include "OutputInterface.h"

#include <memory.h>
#include <cassert>

#include "inVRs/SystemCore/SystemCore.h"
#include "inVRs/SystemCore/UtilityFunctions.h"
#include "inVRs/SystemCore/DebugOutput.h"

// disable deprecation warning for std::auto_ptr when std::unique_ptr is not available.
#ifndef HAS_CXX11_UNIQUE_PTR
// Only do this on GCC versions that support it:
# if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 402
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
#endif

bool											OutputInterface::isInitialized = false;
bool											OutputInterface::isConfigured = false;
std::map<std::string, ModuleInterface*>			OutputInterface::moduleMap;
std::vector<OutputInterface::ModulesConfigData>	OutputInterface::modulesConfigurations;
std::vector<outputInterfaceCallback>			OutputInterface::moduleInitCallbacks;
SceneGraphInterface*							OutputInterface::sceneGraphInterface = NULL;

XmlConfigurationLoader							OutputInterface::xmlConfigLoader;

bool OutputInterface::init(std::string outputInterfaceConfigFile) {
	bool success;
	if (!xmlConfigLoader.hasConverters()) {
		printd(INFO, "OutputInterface::init(): initializing configuration file loader!\n");
		xmlConfigLoader.registerConverter(new ConverterToV1_0a4);
	} // if

	if (outputInterfaceConfigFile.size() == 0) {
		printd(WARNING, "OutputInterface::init(): empty config file passed!\n");
		return false;
	} // if

	std::string fullConfigFilePath = getConcatenatedPath(outputInterfaceConfigFile,
			"OutputInterfaceConfiguration");
	printd(INFO, "OutputInterface::init(): loading OutputInterface Module configuration!\n");
	success = parseOutputInterfaceConfig(fullConfigFilePath, modulesConfigurations);
	if (!success) {
		printd(ERROR,
				"OutputInterface::init(): Error during parsing of configuration file %s!\n",
				fullConfigFilePath.c_str());
		return false;
	} // if
	printd(INFO, "OutputInterface::init(): initializing OutputInterface modules!\n");
	success = initializeModules(modulesConfigurations);
	if (!success) {
		printd(ERROR,
				"OutputInterface::init(): Error during initialization of OutputInterface modules!\n");
		return false;
	} // if
	isInitialized = true;
	return true;
} // init

bool OutputInterface::configure(std::string outputInterfaceConfigFile) {
	bool success;
	if (isConfigured) {
		printd(WARNING,
				"OutputInterface::configure(): OutputInterface already configured! Call will be ignored!\n");
		return false;
	} // if
	if (!isInitialized) {
		success = init(outputInterfaceConfigFile);
		if (!success) {
			printd(ERROR,
					"OutputInterface::configure(): error initializing OutputInterface with config file %s!\n",
					outputInterfaceConfigFile.c_str());
			return false;
		} // if
	} // if
	assert(isInitialized);

	printd(INFO, "OutputInterface::configure(): configuring OutputInterface modules!\n");
	success = configureModules(modulesConfigurations);
	if (!success) {
		printd(ERROR,
				"OutputInterface::configure(): Error during configuration of OutputInterface modules!\n");
		return false;
	} // if

	isConfigured = true;
	return true;
} // configure


void OutputInterface::cleanup() {
	std::map<std::string, ModuleInterface*>::reverse_iterator it;

	// clean up modules
	for (it = moduleMap.rbegin(); it != moduleMap.rend(); ++it) {
		it->second->cleanup();
	} // for
} // cleanup

bool OutputInterface::registerModuleInterface(ModuleInterface* modIntf) {
	std::string moduleName = modIntf->getName();
	if (moduleMap.find(moduleName) != moduleMap.end()) {
		printd(WARNING,
				"OutputInterface::registerModuleInterface(): module with name %s already registered!\n",
				moduleName.c_str());
		return false;
	} // if

	moduleMap[moduleName] = modIntf;
	if (!sceneGraphInterface) {
		sceneGraphInterface = dynamic_cast<SceneGraphInterface*>(modIntf);
	} // if
	return true;
} // registerModuleInterface

bool OutputInterface::unregisterModuleInterface(ModuleInterface* modIntf) {
	std::string moduleName = modIntf->getName();
	if (moduleMap.find(moduleName) == moduleMap.end()) {
		printd(WARNING, "SystemCore::unregisterModuleInterface(): could not find registered Module with name %s\n", moduleName.c_str());
		return false;
	} // if

	moduleMap.erase(moduleName);
	return true;
} // unregisterModuleInterface

void OutputInterface::registerModuleInitCallback(outputInterfaceCallback function) {
	moduleInitCallbacks.push_back(function);
} // registerModuleInitCallback

ModuleInterface* OutputInterface::getModuleByName(std::string name, std::string fileName) {
	if (moduleMap.find(name) == moduleMap.end()) {
		bool success = SystemCore::ModuleLoadAccessor::loadModule(name, fileName);
		if (!success) {
			printd(ERROR, "OutputInterface::getModuleByName(): loading of Module %s failed!\n",
					name.c_str());
		} // if
	} // if

	if (moduleMap.find(name) != moduleMap.end())
		return moduleMap[name];
	else
		return NULL;
} // getModuleByName

bool OutputInterface::isModuleLoaded(std::string name) {
	if (moduleMap.find(name) != moduleMap.end())
		return true;
	return false;
} // isModuleLoaded

SceneGraphInterface* OutputInterface::getSceneGraphInterface() {
	return sceneGraphInterface;
} // getSceneGraphInterface

bool OutputInterface::parseOutputInterfaceConfig(std::string configFile,
		std::vector<ModulesConfigData>& modulesConfigurations) {
	// load xml document via xmlConfigLoader
	std::auto_ptr<const XmlDocument> document(
			xmlConfigLoader.loadConfiguration(configFile));
	if (!document.get()) {
		printd(ERROR,
				"OutputInterface::parseOutputInterfaceConfig(): error at loading of configuration file %s!\n",
				configFile.c_str());
		return false;
	} // if

	bool success = true;

	std::vector<const XmlElement*> elements = document->getElements("outputInterface.module");
	std::vector<const XmlElement*>::iterator elementIt;
	const XmlElement* moduleElement;
	for (elementIt = elements.begin(); elementIt != elements.end(); ++elementIt) {
		moduleElement = *elementIt;
		ModulesConfigData data;
		if (!moduleElement->hasAttribute("name")
				|| !moduleElement->hasAttribute("configFile")) {
			printd(ERROR,
					"OutputInterface::parseOutputInterfaceConfig(): <module> element without attribute 'name' or 'configFile' found! Please fix your Modules configuration!\n");
			success = false;
			continue;
		} // if
		data.moduleName = moduleElement->getAttributeValue("name");
		data.moduleConfigPath = moduleElement->getAttributeValue("configFile");

		if (moduleElement->hasAttribute("libName")) {
			data.moduleLibName = moduleElement->getAttributeValue("libName");
		} // else if
		modulesConfigurations.push_back(data);
	} // for

	return success;
} // parseModuleConfig

bool OutputInterface::initializeModules(std::vector<ModulesConfigData>& modulesConfigurations) {

	std::vector<ModulesConfigData>::iterator it;
	ModuleInterface* module;
	bool success = true;

	for (it = modulesConfigurations.begin(); it != modulesConfigurations.end(); ++it) {
		ModulesConfigData& data = *it;

		if (data.moduleLibName.length() > 0)
			module = getModuleByName(data.moduleName, data.moduleLibName);
		else
			module = getModuleByName(data.moduleName);

		if (!module) {
			printd(ERROR, "OutputInterface::initializeModules(): could not load Module %s!\n",
					data.moduleName.c_str());
			success = false;
		}// if
	} // for
	return success;
} // initializeModules

bool OutputInterface::configureModules(std::vector<ModulesConfigData>& modulesConfigurations) {

	std::vector<ModulesConfigData>::iterator it;
	ModuleInterface* module;
	bool success = true;

	for (it = modulesConfigurations.begin(); it != modulesConfigurations.end(); ++it) {
		ModulesConfigData& data = *it;
		module = getModuleByName(data.moduleName);
		if (module) {
			for (std::vector<moduleInitCallback>::iterator it1 =
					moduleInitCallbacks.begin(); it1 != moduleInitCallbacks.end(); ++it1) {
				(*it1)(module);
			} // for
			if (!module->loadConfig(data.moduleConfigPath)) {
				printd(ERROR, "OutputInterface::configureModules(): failed to load config for module %s!\n", module->getName().c_str());
				success = false;
			} // if
		} // if
		else {
			printd(ERROR, "OutputInterface::configureModules(): could not configure Module %s!\n", data.moduleName.c_str());
			success = false;
		} // if
	} // for

	return success;
} // configureModules

//*****************************************************************************
// Configuration loading
//*****************************************************************************
OutputInterface::ConverterToV1_0a4::ConverterToV1_0a4() :
	XmlConfigurationConverter("0.0", "1.0a4") {

} // ModuleConverterToV1_0a4

bool OutputInterface::ConverterToV1_0a4::convert(XmlDocument* document, const Version& version,
		std::string configFile) {
	bool success = true;

	updateDocument(document, XmlConfigurationLoader::XmlDtdUrl + "outputInterface_v1.0a4.dtd",
			destinationVersion, "outputInterface");

	document->renameAttributes("outputInterface.module.cfgPath", "configFile");
	document->renameAttributes("outputInterface.module.libName", "libraryName");

	return success;
} // convert

