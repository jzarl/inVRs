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

#include "ApplicationBase.h"

#include <assert.h>
#include <signal.h>

#include "DebugOutput.h"
#include "Configuration.h"
#include "Timer.h"
#include "WorldDatabase/WorldDatabase.h"
#include "UserDatabase/UserDatabase.h"
#include "TransformationManager/TransformationManager.h"
#include "../InputInterface/InputInterface.h"
#include "../OutputInterface/OutputInterface.h"

ApplicationBase* ApplicationBase::_instance = NULL;

ApplicationBase::ApplicationBase() :
sceneGraphInterface(NULL),
controllerManager(NULL),
networkModule(NULL),
navigationModule(NULL),
interactionModule(NULL),
localUser(NULL),
activeCamera(NULL),
_logFile(NULL),
_isCleanedUp(false),
_lastTime(0),
_frameDuration(0),
_timeToNextFrame(0)
{

} // ApplicationBase

ApplicationBase::~ApplicationBase() {
	if (!_isCleanedUp)
		printd(WARNING, "ApplicationBase::~ApplicationBase(): class destroyed before globalCleanup is called!\n");

	_instance = NULL;
} // ~ApplicationBase

// Overload in derived class if needed!!!
bool ApplicationBase::preInit(const CommandLineArgumentWrapper& args) {
	return true;
} // preInit

// Overload in derived class if needed!!!
void ApplicationBase::initCoreComponentCallback(CoreComponents comp) {

} // initCoreComponentCallback

// Overload in derived class if needed!!!
void ApplicationBase::initInputInterfaceCallback(ModuleInterface* moduleInterface) {

} // initInputInterfaceCallback

// Overload in derived class if needed!!!
void ApplicationBase::initOutputInterfaceCallback(ModuleInterface* moduleInterface) {

} // initOutputInterfaceCallback

// Overload in derived class if needed!!!
void ApplicationBase::initModuleCallback(ModuleInterface* module) {

} // initModuleCallback

// overload in derived class if needed
bool ApplicationBase::disableAutomaticModuleUpdate() {
	return false;
} // disableAutomaticModuleUpdate

// overload in derived class if needed
void ApplicationBase::manualModuleUpdate(float dt) {

} // manualModuleUpdate


bool ApplicationBase::start(int argc, char** argv) {
    signal (SIGABRT, ApplicationBase::_handleUnexpectedTermination);
    signal (SIGFPE, ApplicationBase::_handleUnexpectedTermination);
    signal (SIGILL, ApplicationBase::_handleUnexpectedTermination);
    signal (SIGSEGV, ApplicationBase::_handleUnexpectedTermination);

    CommandLineArgumentWrapper args(argc, argv);
 	if (!this->_init(args)) {
 		printd(ERROR, "ApplicationBase::start(): initialization of application failed!\n");
 		return false;
 	} // if

 	run();

 	return true;
} // start

void ApplicationBase::globalCleanup() {
	cleanup();
	SystemCore::cleanup();
	printd_finalize();
	if (_logFile)
		fclose(_logFile);
	_isCleanedUp = true;
} // globalCleanup

void ApplicationBase::globalUpdate() {
	_appProfiler.step("remaining display (GLUT)");
	_appProfiler.end();
	_appProfiler.start();
	if (_lastTime <= 0)
		_lastTime = timer.getTime();

	float currentTime = timer.getTime();
	float dt = currentTime - _lastTime;

	// Avoid negative or zero timesteps
	if (dt <= 0) {
		_lastTime = currentTime;
		_appProfiler.interrupt();
		return;
	} // if

	float secToNextFrame = _timeToNextFrame - dt;
	while (secToNextFrame > 0) {
		double usecToNextFrame = ((double)secToNextFrame)*1000000;
		usleep((unsigned int)usecToNextFrame);
		currentTime = timer.getTime();
		dt = currentTime - _lastTime;
		secToNextFrame = _timeToNextFrame - dt;
	} // while

	_timeToNextFrame -= dt;
	assert(_timeToNextFrame < 0);
	_lastTime = currentTime;
	_timeToNextFrame += _frameDuration;

	// avoid that system tries to catch up frames
	if (_timeToNextFrame < 0)
		_timeToNextFrame = 0;

	_appProfiler.step("ApplicationBase.MaxFrameRate calculations");
	// update SystemCore
	SystemCore::step();
	_appProfiler.step("SystemCore::step");

	if (controllerManager && controllerManager->getController()) {
		_updateController(dt);
	} // if
	_appProfiler.step("ApplicationBase::_updateController");

	if (disableAutomaticModuleUpdate()) {
		manualModuleUpdate(dt);
		_appProfiler.step("ApplicationBase::manualModuleUpdate");
	} // if
	else {
		_updateModules(dt);
	} // else

	// call application's display method
	display(dt);
	_appProfiler.step("user display");

	// execute remaining Transformation-pipes
	TransformationManager::step(dt);
	_appProfiler.step("TransformationManager::step(2/2)");

	// TODO: check if we have to do this somewhere sooner
	UserDatabase::updateCursors(dt);
	WorldDatabase::updateAvatars(dt);
	_appProfiler.step("WorldDatabase::updateAvatars");
} // globalUpdate

bool ApplicationBase::_init(const CommandLineArgumentWrapper& args) {

	if (_instance != NULL) {
		printd(ERROR,
				"ApplicationBase::_init(): Instance of ApplicationBase already existing!\n");
		return false;
	} // if
	_instance = this;

	if (!preInit(args)) {
		printd(ERROR, "ApplicationBase::_init(): preInit() failed!\n");
		return false;
	} // if

	std::string configFile = getConfigFile(args);
	if (!_loadConfiguration(configFile)) {
		printd(ERROR, "ApplicationBase::_init(): _loadConfiguration() failed!\n");
		return false;
	} // if

#ifdef WIN32
	SystemCore::initPreloadModules();
#endif

//	printd(INFO, "ApplicationBase::_init(): creating SceneGraphInterface for SystemCore!\n");
//	sceneGraphInterface = createSceneGraphInterface(args);
//	if (!sceneGraphInterface) {
//		printd(ERROR, "ApplicationBase::_init(): createSceneGraphInterface() failed!\n");
//		return false;
//	} // if
//	SystemCore::setSceneGraphInterface(sceneGraphInterface);
//
//	printd(INFO, "ApplicationBase::_init(): Initializing SystemCore!\n");
//	SystemCore::init();

	printd(INFO, "ApplicationBase::_init(): Registering Initialization-callbacks!\n");
	SystemCore::registerCoreComponentInitCallback(_initCoreComponent);
	InputInterface::registerModuleInitCallback(_initInputInterface);
	OutputInterface::registerModuleInitCallback(_initOutputInterface);
	SystemCore::registerModuleInitCallback(_initModule);

	printd(INFO, "ApplicationBase::_init(): Configuring SystemCore!\n");
	if (!_configureSystemCore()) {
		printd(ERROR,
				"ApplicationBase::_init(): _configureSystemCore() failed!\n");
		return false;
	} // if

	sceneGraphInterface = OutputInterface::getSceneGraphInterface();

	printd(INFO, "ApplicationBase::_init(): Reading local User and Camera object!\n");
	localUser = UserDatabase::getLocalUser();
	// must exist since UserDatabase was initialized in SystemCore::init
	assert(localUser);

	activeCamera = localUser->getCamera();
	// must exist since Camera was created when User was created
	assert(activeCamera);

	printd(INFO, "ApplicationBase::_init(): Calling init()!\n");
	if (!init(args)) {
		printd(ERROR, "ApplicationBase::_init(): Initialization of application (init()) failed!\n");
		return false;
	} // if

	printd(INFO, "ApplicationBase::_init(): Synchronizing SystemCore!\n");
	SystemCore::synchronize();

	printd(INFO, "ApplicationBase::_init(): Successfully finished initialization!\n");
	return true;
} // _init

bool ApplicationBase::_loadConfiguration(std::string configFile) {
	if (!Configuration::loadConfig(configFile)) {
		printd(ERROR, "ApplicationBase::_loadConfiguration(): Error loading config-file %s!\n",
				configFile.c_str());
		return false;
	} // if

	if (Configuration::contains("ApplicationBase.debugLevel")) {
		std::string debugLevel = Configuration::getString("ApplicationBase.debugLevel");
		if(!strcasecmp(debugLevel.c_str(), "info"))
			printd_severity(INFO);
		else if(!strcasecmp(debugLevel.c_str(), "warning"))
			printd_severity(WARNING);
		else if(!strcasecmp(debugLevel.c_str(), "error"))
			printd_severity(ERROR);
	} // if
	if (Configuration::contains("ApplicationBase.profilerLogFile")) {
		std::string profilerLogFile = Configuration::getString("ApplicationBase.profilerLogFile");
		_appProfiler.setLogFile(profilerLogFile);
	} // if

	if (Configuration::contains("ApplicationBase.useLogFile")) {
		std::string logFileName = Configuration::getString("ApplicationBase.useLogFile");
        printd(INFO,
        		"ApplicationBase::_loadConfiguration(): Debug output is written to file: %s!\n",
        		logFileName.c_str());
		_logFile = fopen(logFileName.c_str(), "a");
		printd_init(_logFile);
	} // if

	if (Configuration::contains("ApplicationBase.MaxFrameRate")) {
		_frameDuration = 1.0 / Configuration::getFloat("ApplicationBase.MaxFrameRate");
	} // if

	return true;
} // _loadConfiguration

bool ApplicationBase::_configureSystemCore() {
	if (!Configuration::contains("SystemCore.systemCoreConfiguration")) {
		printd(ERROR, "ApplicationBase::_configureSystemCore(): Entry for SystemCore configuration file missing in config file!\n");
		return false;
	} // if
	if (!Configuration::contains("Modules.modulesConfiguration")) {
		printd(ERROR, "ApplicationBase::_configureSystemCore(): Entry for Modules configuration missing in config file!\n");
		return false;
	} // if
	if (!Configuration::contains("Interfaces.inputInterfaceConfiguration")) {
		printd(ERROR, "ApplicationBase::_configureSystemCore(): Entry for InputInterface configuration missing in config file!\n");
		return false;
	} // if
	if (!Configuration::contains("Interfaces.outputInterfaceConfiguration")) {
		printd(ERROR, "ApplicationBase::_configureSystemCore(): Entry for OutputInterface configuration missing in config file!\n");
		return false;
	} // if

	std::string systemCoreConfigFile = Configuration::getString(
			"SystemCore.systemCoreConfiguration");
	std::string modulesConfigFile = Configuration::getString("Modules.modulesConfiguration");
	std::string inputInterfaceConfigFile = Configuration::getString(
			"Interfaces.inputInterfaceConfiguration");
	std::string outputInterfaceConfigFile = Configuration::getString(
			"Interfaces.outputInterfaceConfiguration");

	if (!SystemCore::configure(systemCoreConfigFile, outputInterfaceConfigFile,
			inputInterfaceConfigFile, modulesConfigFile)) {
		printd(ERROR, "ApplicationBase::_configureSystemCore(): failed to setup SystemCore!\n");
		return false;
	} // if

	if (InputInterface::isModuleLoaded("ControllerManager")) {
		controllerManager = (ControllerManagerInterface*)InputInterface::getModuleByName("ControllerManager");
	} else {
		printd(WARNING,
				"ApplicationBase::_configureSystemCore: No ControllerManager found: Input via inVRs controller deactivated!\n");
	} // else

	if (SystemCore::isModuleLoaded("Navigation")) {
		navigationModule = (NavigationInterface*)SystemCore::getModuleByName("Navigation");
	} else {
		printd(WARNING,	"ApplicationBase::_configureSystemCore: No Navigation module found!\n");
	} // else

	if (SystemCore::isModuleLoaded("Network")) {
		networkModule = (NetworkInterface*)SystemCore::getModuleByName("Network");
	} else {
		printd(WARNING,	"ApplicationBase::_configureSystemCore: No Network module found!\n");
	} // else

	if (SystemCore::isModuleLoaded("Interaction")) {
		interactionModule = (InteractionInterface*)SystemCore::getModuleByName("Interaction");
	} else {
		printd(WARNING,	"ApplicationBase::_configureSystemCore: No Interaction module found!\n");
	} // else

	return true;
} // _configureSystemCore

void ApplicationBase::_initCoreComponent(CoreComponents comp) {
	// must be true because registration was after setting instance member
	assert(_instance);
	_instance->initCoreComponentCallback(comp);
} // _initCoreComponent

void ApplicationBase::_initInputInterface(ModuleInterface* moduleInterface) {
	// must be true because registration was after setting instance member
	assert(_instance);
	_instance->initInputInterfaceCallback(moduleInterface);
} // _initInputInterface

void ApplicationBase::_initOutputInterface(ModuleInterface* moduleInterface) {
	// must be true because registration was after setting instance member
	assert(_instance);
	_instance->initOutputInterfaceCallback(moduleInterface);
} // _initOutputInterface

void ApplicationBase::_initModule(ModuleInterface* module) {
	// must be true because registration was after setting instance member
	assert(_instance);
	_instance->initModuleCallback(module);
} // _initModule

void ApplicationBase::_updateModules(float dt) {
	// update Navigation
	if (navigationModule) {
		navigationModule->update(dt);
	} // if
	_appProfiler.step("Navigation::update");

	// Make TransformationManager-step to calculate Tracking-Pipe and Navigation-Pipe
	TransformationManager::step(dt, 0x0D000000);
	_appProfiler.step("TransformationManager::step(1/2)");

	// update Interaction
	if (interactionModule) {
		interactionModule->update(dt);
	} // if
	_appProfiler.step("Interaction::step");

	const std::map<std::string, ModuleInterface*>& moduleMap = SystemCore::getModuleMap();
	std::map<std::string, ModuleInterface*>::const_iterator it;
	ModuleInterface* module;
	for (it = moduleMap.begin(); it != moduleMap.end(); ++it) {
		module = it->second;
		if (module != navigationModule && module != interactionModule) {
			module->update(dt);
			_appProfiler.step(module->getName() + "::update");
		} // if
	} // for
} // _updateModules

void ApplicationBase::_updateController(float dt) {
	// must exist because this is tested before method is called
	assert(controllerManager);

	ControllerInterface* controller = controllerManager->getController();
	// must exist because this is tested before method is called
	assert(controller);

	controller->update();

	int numSensors = controller->getNumberOfSensors();
	// put sensor-values into tracking pipes
	if (numSensors != 0) {
		// add new trackingPipes for sensors which are not available yet
		if (numSensors > (int)_trackingPipes.size()) {
			for (int i = _trackingPipes.size(); i < numSensors; i++) {
				_trackingPipes.push_back(TransformationManager::openPipe(INPUT_INTERFACE_ID,
						USER_DATABASE_ID, i, 0, 0, 0, 0xE0000000 + i, false, localUser));
			} // for
		} // if
		else if (numSensors < (int)_trackingPipes.size()) {
			for (int i = _trackingPipes.size()-1; i >= numSensors; i++) {
				if (_trackingPipes[i] != NULL) {
					TransformationManager::closePipe(_trackingPipes[i]);
				} // if
				_trackingPipes.erase(_trackingPipes.end());
			} // for
		} // else if

		// just to be sure
		assert((int)_trackingPipes.size() == numSensors);

		TransformationData sensorTransf;
		SensorData sensorData;
		for (int i = 0; i < numSensors; i++) {
			// ensure that there really exists a pipe (could be that no configuration was found)
			if (_trackingPipes[i] != NULL) {
				sensorData = controller->getSensorValue(i);
				convert(sensorData, sensorTransf);
				_trackingPipes[i]->push_back(sensorTransf);
			} // if
		} // for
	} // if
} // _updateController

void ApplicationBase::_handleUnexpectedTermination(int errorCode) {
	std::string errorCodeName = "UNKNOWN";
	if (errorCode == SIGABRT)
		errorCodeName = "SIGABRT";
	else if (errorCode == SIGFPE)
		errorCodeName = "SIGFPE";
	else if (errorCode == SIGILL)
		errorCodeName = "SIGILL";
	else if (errorCode == SIGSEGV)
		errorCodeName = "SIGSEGV";
	printd(ERROR, "ApplicationBase::handleUnexpectedTermination(): ERROR CODE %s!\n",
			errorCodeName.c_str());
	printStacktrace();
	printd_finalize();
	if (_instance->_logFile)
		fclose(_instance->_logFile);
	exit(1);
} // _handleUnexpectedTermination
