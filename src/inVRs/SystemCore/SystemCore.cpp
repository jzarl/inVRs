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

#include "SystemCore.h"

#include <assert.h>
#include <memory>

#include <gmtl/QuatOps.h>
#include <gmtl/Matrix.h>
#include <gmtl/VecOps.h>
#include <gmtl/AxisAngle.h>
#include <gmtl/MatrixOps.h>
#include <gmtl/Generate.h>

#include "IdPoolListener.h"
#include "Timer.h"
#include "Configuration.h"
#include "DebugOutput.h"
#include "WorldDatabase/WorldDatabase.h"
#include "XMLTools.h"
#include "UtilityFunctions.h"
#include "SystemCoreEvents.h"
#include "UserDatabase/UserDatabaseEvents.h"
#include "EventManager/EventManager.h"
#include "TransformationManager/TransformationManager.h"
#include "../InputInterface/ControllerManagerInterface.h"

#include "../InputInterface/InputInterface.h"
#include "../OutputInterface/OutputInterface.h"
#include "ComponentInterfaces/ModuleInterface.h"

#ifndef WIN32
#include <dlfcn.h> /* for dlopen, dlsym, etc. */
#endif

// disable deprecation warning for std::auto_ptr when std::unique_ptr is not available.
#ifndef HAS_CXX11_UNIQUE_PTR
// Only do this on GCC versions that support it:
# if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 402
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
#endif

std::map<std::string, ModuleInterface*>	SystemCore::moduleMap;
SceneGraphInterface* 					SystemCore::sgInt = NULL;
std::list<plugin_t>						SystemCore::pluginlist;
std::vector<moduleInitCallback>			SystemCore::moduleCallbacks;
std::vector<coreComponentInitCallback>	SystemCore::coreComponentCallbacks;
EventPipe*								SystemCore::eventPipe;
bool 									SystemCore::initialized = false;
XmlConfigurationLoader 					SystemCore::systemCoreXmlConfigLoader;
XmlConfigurationLoader					SystemCore::moduleXmlConfigLoader;

#ifdef WIN32
std::vector<HMODULE> SystemCore::dllsPreloadList;
#endif

void SystemCore::init() {
	if (initialized) {
		return;
	}
	printd(INFO, "SystemCore::init(): initializing WorldDatabase!\n");
	WorldDatabase::init();
	printd(INFO, "SystemCore::init(): initializing UserDatabase!\n");
	UserDatabase::init();
	printd(INFO, "SystemCore::init(): initializing EventManager!\n");
	EventManager::init();
	EventManager::registerEventFactory("SystemCoreRequestSyncEvent",
			new SystemCoreRequestSyncEvent::Factory());
	printd(INFO, "SystemCore::init(): initializing RequestListener!\n");
	RequestListener::init();
	printd(INFO, "SystemCore::init(): initializing TransformationManager!\n");
	TransformationManager::init();
	printd(INFO, "SystemCore::init(): initializing configuration file loader!\n");
	systemCoreXmlConfigLoader.registerConverter(new SystemCoreConverterToV1_0a4);
	moduleXmlConfigLoader.registerConverter(new ModuleConverterToV1_0a4);
	printd(INFO, "SystemCore::init(): initialization finished!\n");
	initialized = true;
} // init

bool SystemCore::configure(std::string systemCoreConfigFile) {
	return SystemCore::configure(systemCoreConfigFile, "", "", "");
}

bool SystemCore::configure(std::string systemCoreConfigFile, std::string modulesConfigFile) {
	return SystemCore::configure(systemCoreConfigFile, modulesConfigFile, "", "");
}

bool SystemCore::configure(std::string systemCoreConfigFile, std::string outputInterfaceConfigFile,
		std::string inputInterfaceConfigFile, std::string modulesConfigFile) {

	std::vector<ModulesConfigData> modulesConfigurations;
	bool success = true;

	printd(INFO, "SystemCore::configure(): initializing SystemCore (and core components)!\n");
	init();

	if (inputInterfaceConfigFile.size() > 0) {
		printd(INFO, "SystemCore::configure(): initializing InputInterface!\n");
		success = InputInterface::init(inputInterfaceConfigFile);
		if (!success) {
			printd(ERROR, "SystemCore::configure(): failed to initialize InputInterface with config file %s!\n",
					inputInterfaceConfigFile.c_str());
			return false;
		} // if
	} // if

	if (outputInterfaceConfigFile.size() > 0) {
		printd(INFO, "SystemCore::configure(): initializing OutputInterface!\n");
		success = OutputInterface::init(outputInterfaceConfigFile);
		if (!success) {
			printd(ERROR, "SystemCore::configure(): failed to initialize OutputInterface with config file %s!\n",
					outputInterfaceConfigFile.c_str());
			return false;
		} // if
	} // if

	printd(INFO, "SystemCore::configure(): initializing Modules!\n");
	success = initModules(modulesConfigFile, modulesConfigurations);
	if (!success) {
		printd(ERROR, "SystemCore::configure(): failed to initialize Modules with config file %s!\n",
				modulesConfigFile.c_str());
		return false;
	} // if

	if (inputInterfaceConfigFile.size() > 0) {
		printd(INFO, "SystemCore::configure(): configuring InputInterface!\n");
		success = InputInterface::configure(inputInterfaceConfigFile);
		if (!success) {
			printd(ERROR,
					"SystemCore::configure(): Error during configuration of InputInterface!\n");
			return false;
		} // if
	} // if

	if (outputInterfaceConfigFile.size() > 0) {
		printd(INFO, "SystemCore::configure(): configuring OutputInterface!\n");
		success = OutputInterface::configure(outputInterfaceConfigFile);
		if (!success) {
			printd(ERROR,
					"SystemCore::configure(): Error during configuration of OutputInterface!\n");
			return false;
		} // if
	} // if

	if (modulesConfigFile.size() > 0) {
		printd(INFO, "SystemCore::configure(): configuring Modules before CoreComponents!\n");
		success = configureModules(modulesConfigurations,
				ModuleInterface::CONFIGURE_BEFORE_CORECOMPONENTS);
		if (!success) {
			printd(ERROR,
					"SystemCore::configure(): Error during configuration of Modules!\n");
			return false;
		} // if
	} // if

	//TODO: check for correct order:
	//      previous implementation initialized UserDatabase and TransformationManager first, then
	//      the interfaces and Modules and at last the WorldDatabase and the EventManager!!!
	if (systemCoreConfigFile.size() > 0) {
		printd(INFO, "SystemCore::configure(): configuring SystemCore!\n");
		success = configureCoreComponents(systemCoreConfigFile);
		if (!success) {
			printd(ERROR,
					"SystemCore::configure(): Error during configuration of SystemCore components!\n");
			return false;
		} // if
	} // if

	if (modulesConfigFile.size() > 0) {
		printd(INFO, "SystemCore::configure(): configuring Modules after CoreComponents!\n");
		success = configureModules(modulesConfigurations,
				ModuleInterface::CONFIGURE_AFTER_CORECOMPONENTS);
		if (!success) {
			printd(ERROR,
					"SystemCore::configure(): Error during configuration of Modules!\n");
			return false;
		} // if
	} // if

	sendCoreComponentCallback(EVENTMANAGER);
	EventManager::start();
	printd(INFO, "SystemCore::configure(): leaving method\n");



	return true;
} // configure

void SystemCore::cleanup() {
	if (!initialized)
		return;

	std::map<std::string, ModuleInterface*>::reverse_iterator it;

	// clean up modules
	for (it = moduleMap.rbegin(); it != moduleMap.rend(); ++it) {
		it->second->cleanup();
	} // for

	TransformationManager::cleanup();
	UserDatabase::cleanup();
	WorldDatabase::cleanup();
	EventManager::stop();
	EventManager::cleanup();
	RequestListener::cleanup();

	InputInterface::cleanup();
	OutputInterface::cleanup();

	cleanupPlugins();

} // cleanup

bool SystemCore::registerModuleInterface(ModuleInterface* modIntf) {
	std::string moduleName = modIntf->getName();
	if (moduleMap.find(moduleName) != moduleMap.end())
		return false;

	moduleMap[moduleName] = modIntf;
	return true;
} // registerModuleInterface

bool SystemCore::unregisterModuleInterface(ModuleInterface* modIntf) {
	std::string moduleName = modIntf->getName();
	if (moduleMap.find(moduleName) == moduleMap.end()) {
		printd(WARNING, "SystemCore::unregisterModuleInterface(): could not find registered Module with name %s\n", moduleName.c_str());
		return false;
	} // if

	moduleMap.erase(moduleName);
	return true;
} // unregisterModuleInterface

void SystemCore::registerModuleInitCallback(moduleInitCallback function) {
	moduleCallbacks.push_back(function);
} // registerModuleInitCallback

void SystemCore::registerCoreComponentInitCallback(coreComponentInitCallback function) {
	coreComponentCallbacks.push_back(function);
}

void SystemCore::synchronize() {
	SystemCoreRequestSyncEvent* requestSyncEvent;
	UserDatabaseAddUserEvent* addUserEvent;
	NetworkInterface* network = NULL;
	User* localUser = UserDatabase::getLocalUser();
	int i, j;

	std::map<std::string, ModuleInterface*>::iterator it;

	eventPipe = EventManager::getPipe(SYSTEM_CORE_ID);

	printd(INFO, "SystemCore::synchronize(): initializing system ...\n");

	if (!isModuleLoaded("Network")) {
		printd(INFO, "SystemCore::synchronize(): No network module found!\n");
		return;
	} // if

	network = (NetworkInterface*)getModuleByName("Network");
	const std::vector<EntityType*>& entityTypeList =
		WorldDatabase::getEntityTypeList();
	const std::vector<Environment*>& environmentList =
		WorldDatabase::getEnvironmentList();

	printd(INFO, "SystemCore::synchronize(): sending UserDatabaseAddUserEvent to all connected users!\n");

	addUserEvent = new UserDatabaseAddUserEvent(localUser);
	EventManager::sendEvent(addUserEvent, EventManager::EXECUTE_REMOTE);
	// 		delete addUserEvent;

	// block until we received a add user event from all other users
	int waitFor = 0;
	if (eventPipe && (network->getNumberOfParticipants() > 0)) {
		int maxwaits;
		int recvd;
		waitFor = network->getNumberOfParticipants();
		maxwaits = 1000;
		Event* event;
		for (i = 0; i < maxwaits; i++) {
			recvd = 0;
			for (j = 0; j < eventPipe->size(); j++) {
				event = eventPipe->at(j);
				assert(event);
				if (event->getEventName() == "UserDatabaseAddUserEvent")
					recvd++;
			} // for
			if (recvd >= waitFor) {
				// got an addUserEvent from all other users
				// addUserEvents need to be processed first
				// therefore empty eventQ right after running runFirst!!!!
				// as for now they are removed from the eventQ and executed
				break;
			}
			printd(INFO, "SystemCore::synchronize(): waiting for other users ...\n");
			usleep(100000);
		} // for

		// execute the addUserEvents:
		for (j = eventPipe->size() - 1; j >= 0; j--) {
			event = eventPipe->at(j);
			if (event->getEventName() == "UserDatabaseAddUserEvent") {
				event->execute();
				eventPipe->erase(j);
				delete event;
			} // if
		} // for

		// now lets allocate the poolid
		std::vector<IdPoolListener*> idPoolListenerEnvIdList;
		std::vector<IdPoolListener*> idPoolListenerInstIdList;
		IdPoolListener* idPoolListener;
		std::string poolName;
		IdPoolListener::STATE state;
		IdPool* result;

		j = eventPipe->size();
		for (i = 0; i < (int)entityTypeList.size(); i++) {
			poolName = (entityTypeList)[i]->getGlobalEntityIdPoolName();
			assert(poolName.size()>0);
			idPoolListener = new IdPoolListener;
			printd("SystemCore::synchronize(): issuing request for pool %s\n", poolName.c_str());
			idPoolListener->requestAllocation(poolName, 4096, SYSTEM_CORE_ID);
			idPoolListenerInstIdList.push_back(idPoolListener);
		}
		for (i = 0; i < (int)environmentList.size(); i++) {
			poolName = (environmentList)[i]->getGlobalEnvironmentIdPoolName();
			idPoolListener = new IdPoolListener;
			printd("SystemCore::synchronize(): issuing request for pool %s\n", poolName.c_str());
			idPoolListener->requestAllocation(poolName, 4096, SYSTEM_CORE_ID);
			idPoolListenerEnvIdList.push_back(idPoolListener);
		}

		//		int startIndex = j;
		do {
			// pass all incoming Events to idPoolListeners
			for (; j<eventPipe->size(); j++) {
				event = eventPipe->at(j);
				for (i=0; i<(int)idPoolListenerInstIdList.size(); i++) {
					idPoolListener = idPoolListenerInstIdList[i];
					if (idPoolListener->getState() != IdPoolListener::STATE_FINISHED) {
						idPoolListener->handleIncomingEvent(event);
					} // if
				} // for
				for (i=0; i<(int)idPoolListenerEnvIdList.size(); i++) {
					idPoolListener = idPoolListenerEnvIdList[i];
					if (idPoolListener->getState() != IdPoolListener::STATE_FINISHED) {
						idPoolListener->handleIncomingEvent(event);
					} // if
				} // for
			} // for

			waitFor = idPoolListenerInstIdList.size() + idPoolListenerEnvIdList.size();
			for (i=0; i<(int)idPoolListenerInstIdList.size(); i++) {
				idPoolListener = idPoolListenerInstIdList[i];
				state = idPoolListener->getState();
				if (state == IdPoolListener::STATE_FINISHED) {
					//					printd("SystemCore::synchronize(): have result for entitytype %d\n", i);
					result = idPoolListener->getResult();
					assert(result);
					waitFor--;
				} // if
			} // for
			for (i=0; i<(int)idPoolListenerEnvIdList.size(); i++) {
				idPoolListener = idPoolListenerEnvIdList[i];
				state = idPoolListener->getState();
				if (state == IdPoolListener::STATE_FINISHED) {
					//					printd("SystemCore::synchronize(): have result for environment %d\n", i);
					result = idPoolListener->getResult();
					assert(result);
					waitFor--;
				} // if
			} // for
			if (waitFor > 0) {
				usleep(1000);
			} // if
		} while (waitFor > 0);

		//		printd(INFO, "SystemCore::synchronize(): number of received Events: %i\n", j - startIndex);
		//		printd(INFO, "SystemCore::synchronize(): number of expected Events: multiple of %i\n", (idPoolListenerInstIdList.size() + idPoolListenerEnvIdList.size()));

		printd("SystemCore::synchronize(): got all results!\n");

		for (i = 0; i < (int)entityTypeList.size(); i++) {
			result = idPoolListenerInstIdList[i]->getResult();
			assert(result);
			(entityTypeList)[i]->setLocalEntityIdPool(result);
			delete idPoolListenerInstIdList[i];
		}
		for (i = 0; i < (int)environmentList.size(); i++) {
			result = idPoolListenerEnvIdList[i]->getResult();
			assert(result);
			(environmentList)[i]->setLocalEnvironmentIdPool(result);
			delete idPoolListenerEnvIdList[i];
		}

	} else {
		// allocate local id pools for entity creation in case we are the server
		IdPool *pool, *subPool;
		std::string poolName;
		for (i = 0; i < (int)entityTypeList.size(); i++) {
			poolName = (entityTypeList)[i]->getGlobalEntityIdPoolName();
			assert(poolName.size()> 0);
			pool = localIdPoolManager.getPoolByName(poolName);
			assert(pool);
			subPool = pool->allocSubPool(4096);
			assert(subPool);
			(entityTypeList)[i]->setLocalEntityIdPool(subPool);
		}
	}

	User* aRandomRemoteUser;
	aRandomRemoteUser = UserDatabase::getRemoteUserByIndex(0);
	if (aRandomRemoteUser != NULL) {
		printd(INFO, "SystemCore::synchronize(): syncing with user %u\n", aRandomRemoteUser->getId());
		requestSyncEvent = new SystemCoreRequestSyncEvent();
		EventManager::sendEventTo(requestSyncEvent, aRandomRemoteUser->getId());
		// 			delete requestSyncEvent;
	} else {
		printd(INFO, "SystemCore::synchronize(): nobody here to sync with\n");
	}

	printd(INFO, "SystemCore::synchronize(): leaving runFirst()\n");
} // init


void SystemCore::step() {
	std::deque<Event*>* eventRecvQ = NULL;
	Event* event;

	eventRecvQ = eventPipe->makeCopyAndClear();
	for (int i = 0; i < (int)eventRecvQ->size(); i++) {
		event = (*eventRecvQ)[i];
		event->execute();
		delete event;
	} // for
	delete eventRecvQ;
} // step

	bool SystemCore::isModuleLoaded(std::string name) {
		if (moduleMap.find(name) != moduleMap.end())
			return true;
		return false;
	} // isModuleLoaded

const std::map<std::string, ModuleInterface*>& SystemCore::getModuleMap() {
	return moduleMap;
} // getModuleMap

ModuleInterface* SystemCore::getModuleByName(std::string name, std::string fileName) {
	std::string pluginFileName;

	//printd(INFO, "moduleMapsize: %d\n",  moduleMap.size());

	if (moduleMap.find(name) != moduleMap.end())
		return moduleMap[name];

	if (fileName.length() > 0)
		pluginFileName = fileName;
	else
		pluginFileName = getLibraryFileName(name);

	printd(INFO, "SystemCore::getModuleByName() plugin file name is \"%s\"\n", pluginFileName.c_str());

	if (!loadPlugin(pluginFileName)) {
		printd(ERROR, "SystemCore::getModuleByName(): could not load plugin \"%s\"\n", pluginFileName.c_str());
		return NULL;
	} // if

	if (moduleMap.find(name) != moduleMap.end())
		return moduleMap[name];
	else
		return NULL;
} // getModuleByName

bool SystemCore::parseModuleConfig(std::string modulesConfigFile,
		std::vector<ModulesConfigData>& modulesConfigurations) {

	std::string configFileConcatenatedPath = getConcatenatedPath(modulesConfigFile, "ModulesConfiguration");

	// load xml document via xmlConfigLoader
	std::auto_ptr<const XmlDocument> document(
			moduleXmlConfigLoader.loadConfiguration(configFileConcatenatedPath));
	if (!document.get()) {
		printd(ERROR,
				"SystemCore::parseModuleConfig(): error at loading of configuration file %s!\n",
				modulesConfigFile.c_str());
		return false;
	} // if

	bool success = true;

	std::vector<const XmlElement*> elements = document->getElements("modules.module");
	std::vector<const XmlElement*>::iterator elementIt;
	const XmlElement* moduleElement;
	for (elementIt = elements.begin(); elementIt != elements.end(); ++elementIt) {
		moduleElement = *elementIt;
		ModulesConfigData data;
		if (!moduleElement->hasAttribute("name")
				|| !moduleElement->hasAttribute("configFile")) {
			printd(ERROR,
					"SystemCore::parseModuleConfig(): <module> element without attribute 'name' or 'configFile' found! Please fix your Modules configuration!\n");
			success = false;
			continue;
		} // if
		data.moduleName = moduleElement->getAttributeValue("name");
		data.moduleConfigPath = moduleElement->getAttributeValue("configFile");

		if (moduleElement->hasAttribute("libraryName")) {
			std::string libraryName = moduleElement->getAttributeValue("libraryName");
			data.moduleLibPath = getLibraryFileName(libraryName);
		} // else if
		modulesConfigurations.push_back(data);
	} // for

	return success;
} // parseModuleConfig

bool SystemCore::initializeModules(std::vector<ModulesConfigData>& modulesConfigurations) {

	std::vector<ModulesConfigData>::iterator it;
	ModuleInterface* module;
	bool success = true;

	for (it = modulesConfigurations.begin(); it != modulesConfigurations.end(); ++it) {
		ModulesConfigData& data = *it;

		if (data.moduleLibPath.length() > 0)
			module = getModuleByName(data.moduleName, data.moduleLibPath);
		else
			module = getModuleByName(data.moduleName);

		if (!module) {
			printd(ERROR, "SystemCore::parseModuleConfig(): could not load Module %s!\n", data.moduleName.c_str());
			success = false;
		}// if
	} // for
	return success;
} // initializeModules

bool SystemCore::configureModules(std::vector<ModulesConfigData>& modulesConfigurations,
		ModuleInterface::CONFIGURATION_ORDER configurationOrder) {

	std::vector<ModulesConfigData>::iterator it;
	ModuleInterface* module;
	bool success = true;

	for (it = modulesConfigurations.begin(); it != modulesConfigurations.end(); ++it) {
		ModulesConfigData& data = *it;

		module = getModuleByName(data.moduleName);

		if (module && module->getConfigurationOrder() == configurationOrder) {
			for (std::vector<moduleInitCallback>::iterator it1 =
					moduleCallbacks.begin(); it1 != moduleCallbacks.end(); ++it1) {
				(*it1)(module);
			} // for

			if (!module->loadConfig(data.moduleConfigPath)) {
				printd(ERROR, "SystemCore::configureModules(): failed to load config for module %s!\n", module->getName().c_str());
				success = false;
			} // if

			//TODO: Filtering for Module name does not allow multiple/different implementations
			if (module->getName() == "Network") {
				EventManager::registerNetwork((NetworkInterface*)module);
			} // if
		} // if
		else if (!module) {
			printd(ERROR, "SystemCore::configureModules(): could not configure Module %s!\n", data.moduleName.c_str());
			success = false;
		} // if
	} // for

	return success;
} // configureModules

void SystemCore::sendCoreComponentCallback(CoreComponents comp) {
	for (std::vector<coreComponentInitCallback>::iterator it1 =
			coreComponentCallbacks.begin(); it1 != coreComponentCallbacks.end(); ++it1) {
		(*it1)(comp);
	} // for
}

bool SystemCore::initModules(std::string modulesConfigFile,
		std::vector<ModulesConfigData>& modulesConfigurations) {
	bool success;
	if (modulesConfigFile.size() > 0) {
		success = parseModuleConfig(modulesConfigFile, modulesConfigurations);
		if (!success) {
			printd(ERROR,
					"SystemCore::initModules(): Error during parsing of Module configuration file %s!\n",
					modulesConfigFile.c_str());
			return false;
		} // if
		success = initializeModules(modulesConfigurations);
		if (!success) {
			printd(ERROR, "SystemCore::initModules(): Error during initialization of Modules!\n");
			return false;
		} // if
	} // if
	return true;
} // initModules

bool SystemCore::configureCoreComponents(std::string configFile) {
	bool success;

	std::string configFileConcatenatedPath = getConcatenatedPath(configFile, "SystemCoreConfiguration");

	// load xml document via xmlConfigLoader
	std::auto_ptr<const XmlDocument> document(
			systemCoreXmlConfigLoader.loadConfiguration(configFileConcatenatedPath));
	if (!document.get()) {
		printd(ERROR,
				"SystemCore::configureCoreComponents(): error at loading of configuration file %s!\n",
				configFile.c_str());
		return false;
	} // if

	std::string worldDatabaseFile = document->getAttributeValue("systemCore.worldDatabase.configFile");
	std::string userDatabaseFile = document->getAttributeValue("systemCore.userDatabase.configFile");
	std::string transformationManagerFile = document->getAttributeValue("systemCore.transformationManager.configFile");
	std::string eventManagerFile = document->getAttributeValue("systemCore.eventManager.configFile");

	printd(INFO, "SystemCore::configureCoreComponents(): configuring UserDatabase!\n");
	sendCoreComponentCallback(USERDATABASE);
	if (userDatabaseFile.size() != 0) {
		success = UserDatabase::getLocalUser()->loadConfig(userDatabaseFile);
		if (!success) {
			printd(ERROR, "SystemCore::configureCoreComponents(): Error during configuration of UserDatabase!\n");
			return false;
		} // if
	} // if
	else {
		printd(ERROR, "SystemCore::configureCoreComponents(): missing config-file for UserDatabase!\n");
		return false;
	} // else

	printd(INFO, "SystemCore::configureCoreComponents(): configuring TransformationManager!\n");
	sendCoreComponentCallback(TRANSFORMATIONMANAGER);
	if (transformationManagerFile.size() != 0) {
		success = TransformationManager::loadConfig(transformationManagerFile);
		if (!success) {
			printd(ERROR,
					"SystemCore::configureCoreComponents(): Error during configuration of TransformationManager!\n");
			return false;
		} // if
	} // if
	else {
		printd(ERROR, "SystemCore::configureCoreComponents(): missing config-file for TransformationManager!\n");
		return false;
	} // else

	printd(INFO, "SystemCore::configureCoreComponents(): configuring WorldDatabase!\n");
	sendCoreComponentCallback(WORLDDATABASE);
	if (worldDatabaseFile.size() != 0) {
		success = WorldDatabase::loadWorldDatabase(worldDatabaseFile);
		if (!success) {
			printd(ERROR,
					"SystemCore::configureCoreComponents(): Error during configuration of WorldDatabase!\n");
			return false;
		} // if
	} // if
	else {
		printd(ERROR, "SystemCore::configureCoreComponents(): missing config-file for WorldDatabase!\n");
		return false;
	} // else

	printd(INFO, "SystemCore::configureCoreComponents(): configuring EventManager!\n");
	sendCoreComponentCallback(EVENTMANAGER);
	if (eventManagerFile.size() != 0) {
		success = EventManager::loadConfig(eventManagerFile);
		if (!success) {
			printd(ERROR,
					"SystemCore::configureCoreComponents(): Error during configuration of EventManager!\n");
			return false;
		} // if
	} // if

	return true;
} // configureSystemCore

#ifdef WIN32
// Helmut: DLL entry point for system core dll
BOOL APIENTRY DllMain( HMODULE hModule,
		DWORD ul_reason_for_call,
		LPVOID lpReserved
		)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}

typedef FARPROC dsosymbol_t;
#else
typedef void* dsosymbol_t;
#endif

/**
 * Internal wrapper for dlopen/LoadLibrary.
 */
plugin_t openDSO(std::string filename)
{
	plugin_t plugin;
#ifdef WIN32
	// Helmut: '/' must be converted into '\' (claimed by the msdn docu)
	size_t pos = filename.find_first_of("/");
	while (pos!=std::string::npos)
	{
		filename[pos]='\\';
		pos=filename.find_first_of("/",pos+1);
	}
	plugin = LoadLibraryA(filename.c_str());
#else
	plugin = dlopen(filename.c_str(), RTLD_NOW);
#endif
	return plugin;
}

/**
 * Internal wrapper for dlclose/FreeLibrary.
 */
bool freeDSO(plugin_t &plugin) {
#if WIN32
	return FreeLibrary(plugin);
#else
	return (0 == dlclose(plugin));
#endif
}

/**
 * Internal wrapper for dlerror/GetLastError.
 * Returns a message describing the most recent error state.
 */
std::string getErrortext()
{
	std::string errortext;
#if WIN32
	DWORD errorType = GetLastError();
	char errorMsg[1024] = "";
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, errorType, LANG_SYSTEM_DEFAULT, errorMsg, 1024, 0);
	errortext = errorMsg;
#else
	errortext = dlerror();
#endif
	return errortext;
}

/**
 * Internal wrapper for dlsym/GetProcAddress.
 *
 * Note on Linux: since a symbol can be null, the correct way
 * to test the result would be to call dlerror() and see if an error is set.
 * Since we don't use null symbols in the plugins, testing for null is safe for us.
 */
dsosymbol_t getDSOSymbol( plugin_t &plugin, const std::string &symbolname )
{
#if WIN32
	return GetProcAddress(plugin,symbolname.c_str());
#else
	return dlsym(plugin,symbolname.c_str());
#endif
}

bool SystemCore::loadPlugin(std::string filename) {
	plugin_t plugin;
	registerModule_t register_f;
	modulePluginVersion_t modulePluginVersion_f;

	// try "Plugins" path first:
	printd(INFO, "SystemCore::loadPlugin(): Trying to load plugin %s\n", filename.c_str());
	plugin = openDSO(getConcatenatedPath(filename, "Plugins"));
	if (plugin == 0) {
		// try search by soname:
		printd(INFO, "SystemCore::loadPlugin(): Trying to load plugin %s\n", filename.c_str());
		plugin = openDSO(filename);
		if (plugin == 0)
		{
			std::string error = getErrortext();
			printd(ERROR, "SystemCore::loadPlugin(): Couldn't open plugin file \"%s\": %s!\n",
					filename.c_str(), error.c_str());
			return false;
		}
	}

	/*  check, if the version of the plugin matches the one of the ModuleInterface: */
	modulePluginVersion_f = (modulePluginVersion_t) getDSOSymbol(plugin, "modulePluginVersion");
	// If the function address is valid, call the function.
	if (modulePluginVersion_f == 0)
	{
		std::string error = getErrortext();
		printd(ERROR,
				"SystemCore::loadPlugin(): Couldn't find function modulePluginVersion: %s\n", error.c_str());
		freeDSO(plugin);
		return false;
	}
	if (strcmp(modulePluginVersion_f(), MODULEPLUGINVERSION) != 0)
	{
		printd( ERROR, "SystemCore::loadPlugin(): Version of plugin \"%s\" doesn't match!\n",
				filename.c_str());
		freeDSO(plugin);
		return false;
	} // if

	register_f = (registerModule_t) getDSOSymbol(plugin, "registerModule");
	if (register_f == 0)
	{
		std::string error = getErrortext();
		printd(ERROR, "SystemCore::loadPlugin(): Couldn't find function registerModule: %s\n",error.c_str());
		freeDSO(plugin);
		return false;
	} // if
	if (!(register_f)())
	{
		printd(ERROR, "SystemCore::loadPlugin(): Couldn't register plugin!\n");
		freeDSO(plugin);
		return false;
	} // if

	pluginlist.insert(pluginlist.end(), plugin);
	return true; //ok
} // loadPlugin
#ifdef WIN32

void SystemCore::addPreloadModule(HMODULE dllModuleHandle)
{
	dllsPreloadList.push_back(dllModuleHandle);
}

void SystemCore::initPreloadModules()
{
	int i;

	HMODULE plugin;
	registerModule_t register_f;
	modulePluginVersion_t modulePluginVersion_f;

	printd("SystemCore::initPreloadModules(): have %d modules to preload!\n", dllsPreloadList.size());
	for(i=0;i<dllsPreloadList.size();i++)
	{
		plugin = dllsPreloadList[i];

		modulePluginVersion_f = (modulePluginVersion_t)
			GetProcAddress(plugin, "modulePluginVersion");
		// If the function address is valid, call the function.
		if(modulePluginVersion_f == 0)
		{
			printd(ERROR, "SystemCore::initPreloadModules(): Couldn't find function modulePluginVersion!\n");
			continue;
		}
		register_f = (registerModule_t)
			GetProcAddress(plugin, "registerModule");
		if (register_f == 0)
		{
			printd(ERROR, "SystemCore::initPreloadModules(): Couldn't find function registerModule!\n");
			continue;
		} // if
		if (!(register_f)() )
		{
			printd(ERROR, "SystemCore::initPreloadModules(): Couldn't register plugin!\n");
			continue;
		}
	}
}

#endif // WIN32

std::string SystemCore::getLibraryFileName(std::string libraryName) {
	std::string libraryFileName;

#ifndef WIN32
#  if defined(MACOS_X) || defined(__APPLE_CC__)
	libraryFileName = "libinVRs" + libraryName + ".dylib";
#  else
	libraryFileName = "libinVRs" + libraryName + ".so";
#  endif
#else
	libraryFileName = "inVRs" + libraryName + ".dll";
#endif

	return libraryFileName;
} // getModuleWithLibraryName

void SystemCore::cleanupPlugins() {
	plugin_t plugin;
	registerModule_t unregister_f;
	const char *error;
	std::list<plugin_t>::reverse_iterator it = pluginlist.rbegin();

	printd(INFO, "SystemCore::cleanup(): releasing plugins...\n");

	while (it != pluginlist.rend()) {
		plugin = *it;

		unregister_f
			= (unregisterModule_t)getDSOSymbol(plugin, "unregisterModule");
		if (unregister_f == 0)
		{
			std::string error = getErrortext();
			printd(ERROR, "SystemCore::cleanup(): Couldn't find function unregisterModule: %s\n", error.c_str());
		} else if (!unregister_f())
			printd(ERROR, "SystemCore::cleanup(): Couldn't deregister plugin!\n");

		if (!freeDSO(plugin)) {
			printd(ERROR, "SystemCore::cleanup(): Couldn't close dynamic library!\n");
		}
		++it;
	} // while
	pluginlist.clear();
} // cleanupPlugins

bool SystemCore::ModuleLoadAccessor::loadModule(std::string name, std::string libraryName) {
	std::string pluginFileName;

	if (libraryName.length() > 0)
		pluginFileName = getLibraryFileName(libraryName);
	else
		pluginFileName = getLibraryFileName(name);

	printd(INFO, "SystemCore::ModuleLoadAccessor::loadModule() plugin file name is \"%s\"\n", pluginFileName.c_str());

	if (!loadPlugin(pluginFileName)) {
		printd(ERROR, "SystemCore::ModuleLoadAccessor::loadModule() could not load plugin \"%s\"\n", pluginFileName.c_str());
		return false;
	} // if

	return true;
} // loadModule

//*****************************************************************************
// Configuration loading
//*****************************************************************************
SystemCore::SystemCoreConverterToV1_0a4::SystemCoreConverterToV1_0a4() :
	XmlConfigurationConverter("0.0", "1.0a4") {
	} // ConverterToV1_0a4

bool SystemCore::SystemCoreConverterToV1_0a4::convert(XmlDocument* document, const Version& version,
		std::string configFile) {
	bool success = true;

	updateDocument(document, XmlConfigurationLoader::XmlDtdUrl + "systemCore_v1.0a4.dtd",
			destinationVersion, "systemCore");

	document->renameAttributes("systemCore.worldDatabase.config", "configFile");
	document->renameAttributes("systemCore.userDatabase.config", "configFile");
	document->renameAttributes("systemCore.transformationManager.config", "configFile");

	return success;
} // convert

SystemCore::ModuleConverterToV1_0a4::ModuleConverterToV1_0a4() :
	XmlConfigurationConverter("0.0", "1.0a4") {

	} // ModuleConverterToV1_0a4

bool SystemCore::ModuleConverterToV1_0a4::convert(XmlDocument* document, const Version& version,
		std::string configFile) {
	bool success = true;

	updateDocument(document, XmlConfigurationLoader::XmlDtdUrl + "modules_v1.0a4.dtd",
			destinationVersion, "modules");

	document->renameAttributes("modules.module.cfgPath", "configFile");
	document->renameAttributes("modules.module.libName", "libraryName");

	return success;
} // convert

