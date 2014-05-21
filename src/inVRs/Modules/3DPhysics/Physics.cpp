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

#include "Physics.h"

#include <assert.h>
#include <OpenSG/OSGThread.h>
#include <OpenSG/OSGChangeList.h>
#include <OpenSG/OSGThreadManager.h>

#include "PhysicsEntity.h"
#include "ChangedUserDataEvent.h"
#include "StartSimulationEvent.h"
#include "SynchronisePhysicsEvent.h"
#include "CreateCursorSpringConnectorEvent.h"
#include "DeleteCursorSpringConnectorEvent.h"
#include "PhysicsSpringManipulationActionEvents.h"
#include "ChangeSynchronisationModelEvent.h"
#include "CreateLinkedPhysicsEntityEvent.h"

#include "FullSynchronisationModelFactory.h"
#include "ChangedSynchronisationModelFactory.h"
#include "VelocitySynchronisationModelFactory.h"
#include "AccelerationSynchronisationModelFactory.h"
#include "PhysicsFullSynchronisationModelFactory.h"
#include "PhysicsInputSynchronisationModelFactory.h"
#include "SingleServerPhysicsObjectManagerFactory.h"
#include "PhysicsEntityTransformationWriterModifier.h"

#include "SimplePhysicsEntityTypeFactory.h"
#include "ArticulatedPhysicsEntityTypeFactory.h"

#include <inVRs/SystemCore/Configuration.h>
#include <inVRs/SystemCore/XMLTools.h>
#include <inVRs/SystemCore/UserDatabase/UserDatabase.h>
#include <inVRs/SystemCore/WorldDatabase/WorldDatabase.h>
#include <inVRs/SystemCore/TransformationManager/TransformationManager.h>
#include <inVRs/SystemCore/SystemCore.h>
#include <inVRs/SystemCore/Timer.h>
#include <inVRs/SystemCore/UtilityFunctions.h>
#include <inVRs/SystemCore/ComponentInterfaces/module_core_api.h>

// disable deprecation warning for std::auto_ptr when std::unique_ptr is not available.
#ifndef HAS_CXX11_UNIQUE_PTR
// Only do this on GCC versions that support it:
# if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 402
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
#endif

OSG_USING_NAMESPACE

Physics*				Physics::singleton = NULL;
volatile bool			Physics::shutdown;
volatile bool			Physics::running;
volatile bool			Physics::waitForEvent;
volatile double			Physics::startTime;
inVRsUtilities::Timer	Physics::physicsTimer = inVRsUtilities::Timer(true);

XmlConfigurationLoader	Physics::xmlConfigLoader;

using namespace oops;

//******************//
// PUBLIC METHODS: *//
//******************//

Physics::Physics()
	: objectManager(NULL), synchronisationModel(NULL)
{
//	SystemCore::registerModuleInterface(this);

	simulation = new Simulation;
	xmlLoader = new oops::XMLLoader();

	objectManagerFactories.push_back(new SingleServerPhysicsObjectManagerFactory);
	synchronisationModelFactories.push_back(new FullSynchronisationModelFactory);
	synchronisationModelFactories.push_back(new ChangedSynchronisationModelFactory);
	synchronisationModelFactories.push_back(new VelocitySynchronisationModelFactory);
	synchronisationModelFactories.push_back(new AccelerationSynchronisationModelFactory);
	synchronisationModelFactories.push_back(new PhysicsFullSynchronisationModelFactory);
	synchronisationModelFactories.push_back(new PhysicsInputSynchronisationModelFactory);
	EventManager::registerEventFactory("SynchronisePhysicsEvent", new SynchronisePhysicsEvent::Factory());
	EventManager::registerEventFactory("CreateCursorSpringConnectorEvent", new CreateCursorSpringConnectorEvent::Factory());
	EventManager::registerEventFactory("DeleteCursorSpringConnectorEvent", new DeleteCursorSpringConnectorEvent::Factory());
	EventManager::registerEventFactory("PhysicsBeginSpringManipulationActionEvent", new PhysicsBeginSpringManipulationActionEvent::Factory());
	EventManager::registerEventFactory("PhysicsEndSpringManipulationActionEvent", new PhysicsEndSpringManipulationActionEvent::Factory());
	EventManager::registerEventFactory("ChangedUserDataEvent", new ChangedUserDataEvent::Factory());
	EventManager::registerEventFactory("StartSimulationEvent", new StartSimulationEvent::Factory());
	EventManager::registerEventFactory("ChangeSynchronisationModelEvent", new ChangeSynchronisationModelEvent::Factory());
	EventManager::registerEventFactory("CreateLinkedPhysicsEntityEvent", new CreateLinkedPhysicsEntityEvent::Factory());

	WorldDatabase::registerEntityTypeFactory(new SimplePhysicsEntityTypeFactory(xmlLoader));
	WorldDatabase::registerEntityTypeFactory(new ArticulatedPhysicsEntityTypeFactory(xmlLoader));

	TransformationManager::registerModifierFactory(new PhysicsEntityTransformationWriterModifierFactory);

	simulation->setGlobalAutoDisable(true);
	simulation->setAutoDisableLinearThreshold(0.2f);
	simulation->setAutoDisableAngularThreshold(0.2f);
	simulation->setAutoDisableTime(0.5f);
	stepSize = 0.01f;
	simulationTime = 0;

#ifdef OPENSG_THREADMANAGER_GETLOCK_HAVE_BGLOBAL
	simulationStepLock = ThreadManager::the()->getLock("Physics::simulationStepLock",false);
	simulationStepListenerLock = ThreadManager::the()->getLock("Physics::simulationStepListenerLock",false);
	systemThreadListenerLock = ThreadManager::the()->getLock("Physics::systemThreadListenerLock",false);
#else
	simulationStepLock = dynamic_cast<Lock*>(ThreadManager::the()->getLock("Physics::simulationStepLock"));
	simulationStepListenerLock = dynamic_cast<Lock*>(ThreadManager::the()->getLock("Physics::simulationStepListenerLock"));
	systemThreadListenerLock = dynamic_cast<Lock*>(ThreadManager::the()->getLock("Physics::systemThreadListenerLock"));
#endif

	if (!singleton)
	{
		singleton = this;
		running = false;
		shutdown = false;
	} // if
	else
	{
		printd(ERROR, "Physics::Physics(): more than one instance of class Physics not supported!\n");
		assert(false);
	} // else

	physicsThread = NULL;

	simLastTime = -1;
	simTimeToNextStep = stepSize;
	simFpsTime = -1;
	simFpsCounter = 0;

	waitForEvent = false;
	startTime = 0;

	alreadyCleanedUp = false;

	simulationStartTime = 0;

	xmlConfigLoader.registerConverter(new ConverterToV1_0a4);
} // Physics

Physics::~Physics()
{
	int i;
	for (i=0; i < (int)objectManagerFactories.size(); i++)
		delete objectManagerFactories[i];
	objectManagerFactories.clear();

	for (i=0; i < (int)synchronisationModelFactories.size(); i++)
		delete synchronisationModelFactories[i];
	synchronisationModelFactories.clear();

	if (singleton == this)
		singleton = NULL;

	delete simulation;

//	SystemCore::unregisterModuleInterface(this);
} // ~Physics

bool Physics::loadConfig(std::string configFile)
{
	std::string configFileConcatenatedPath = configFile;
	if (!Configuration::containsPath("PhysicsModuleConfiguration") &&
			Configuration::containsPath("PhysicsConfiguration")) {
		printd(WARNING,
				"Physics::loadConfig(): Deprecated path entry PhysicsConfiguration found in general configuration file! Use PhysicsModuleConfiguration instead!\n");
		configFileConcatenatedPath = getConcatenatedPath(configFile, "PhysicsConfiguration");
	} else {
		configFileConcatenatedPath = getConcatenatedPath(configFile, "PhysicsModuleConfiguration");
	} // else

	// load xml document via xmlConfigLoader
	std::auto_ptr<const XmlDocument> document(
			xmlConfigLoader.loadConfiguration(configFileConcatenatedPath));
	if (!document.get()) {
		printd(ERROR,
				"Physics::loadConfig(): error at loading of configuration file %s!\n",
				configFile.c_str());
		return false;
	} // if

	bool success = true;

	const XmlElement* simulationElement = document->getElement("physics.simulation");
	const XmlElement* objectManagerElement = document->getElement("physics.objectManager");
	const XmlElement* synchronisationModelElement = document->getElement("physics.synchronisationModel");

	if (!simulationElement || !objectManagerElement || !synchronisationModelElement) {
		printd(ERROR,
				"Physics::loadConfig(): missing element <simulation>, <objectManager> or <synchronisationModel> in <physics> element! Please fix your Physics Module configuration file!\n");
		return false;
	} // if

	// handle <simulation> element
	gmtl::Vec3f gravity;
	std::string stepFunction = "";

	const XmlElement* gravityElement = simulationElement->getSubElement("gravity");
	const XmlElement* stepSizeElement = simulationElement->getSubElement("stepSize");
	const XmlElement* stepFunctionElement = simulationElement->getSubElement("stepFunction");

	if (!gravityElement || !stepSizeElement) {
		printd(ERROR,
				"Physics::loadConfig(): missing element <gravityElement> or <stepSizeElement> in <simulation> element! Please fix your Physics Module configuration file!\n");
		return false;
	} // if
	gravity[0] = gravityElement->getAttributeValueAsFloat("x");
	gravity[1] = gravityElement->getAttributeValueAsFloat("y");
	gravity[2] = gravityElement->getAttributeValueAsFloat("z");
	simulation->setGravity(gravity);

	this->stepSize = stepSizeElement->getAttributeValueAsFloat("value");
	this->simTimeToNextStep = this->stepSize;
	simulation->setStepSize(this->stepSize);

	if (stepFunctionElement) {
		stepFunction = stepFunctionElement->getAttributeValue("name");
		if (stepFunction == "NORMAL" || stepFunction == "STEP") {
			simulation->setStepFunction(Simulation::STEPFUNCTION_STEP);
			printd(INFO, "Physics::loadConfig(): changing to stepping function STEP!\n");
		} // if
		else if (stepFunction == "QUICKSTEP") {
			simulation->setStepFunction(Simulation::STEPFUNCTION_QUICKSTEP);
			printd(INFO, "Physics::loadConfig(): changing to stepping function QUICKSTEP!\n");
		} // else if
		else if (stepFunction == "STEPFAST1") {
			printd(INFO, "Physics::loadConfig(): changing to stepping function STEPFAST1!\n");
			int numberOfSteps = stepFunctionElement->getAttributeValueAsInt("numberOfSteps");
			if (numberOfSteps > 0)
				simulation->setStepFunction(Simulation::STEPFUNCTION_STEPFAST1, numberOfSteps);
			else
				simulation->setStepFunction(Simulation::STEPFUNCTION_STEPFAST1);
		} // else if
		else {
			printd(WARNING, "Physics::loadConfig(): unknown stepFunction %s found! Using default function!\n", stepFunction.c_str());
		} // else
	} // if

	// handle <objectManager> element
	std::string className = objectManagerElement->getAttributeValue("type");
	ArgumentVector* arguments;
	if (objectManagerElement->hasSubElement("arguments")) {
		arguments = readArgumentsFromXmlElement(objectManagerElement);
	} // if
	else {
		arguments = NULL;
	} // else
	printd(INFO, "Physics::loadConfig(): trying to load objectManager %s!\n", className.c_str());
	this->objectManager = createPhysicsObjectManager(className, arguments);
	if (arguments) {
		delete arguments;
	} // if
	success = (this->objectManager != NULL) && success;

	// handle <synchronisationModel> element
	className = synchronisationModelElement->getAttributeValue("type");
	if (synchronisationModelElement->hasSubElement("arguments")) {
		arguments = readArgumentsFromXmlElement(
				synchronisationModelElement->getSubElement("arguments"));
	} // if
	else {
		arguments = NULL;
	} // else
	printd(INFO, "Physics::loadConfig(): trying to load synchronisationModel %s!\n",
			className.c_str());
	this->synchronisationModel = createSynchronisationModel(className, arguments);
	if (arguments) {
		delete arguments;
	} // if
	success = (this->synchronisationModel != NULL) && success;

	if (success) {
		this->synchronisationModel->setPhysicsObjectManager(this->objectManager);
		init();
	} // if

	return success;
/*
	bool parsingError = false;
	bool error = false;
	gmtl::Vec3f gravity;
	IrrXMLReader* xml;
	std::string stepFunction;
	std::string className;
	unsigned numberOfSteps;
	ArgumentVector* arguments;

	printd(INFO, "Physics::loadConfig(): loading Physics config!\n");

	configFile = Configuration::getPath("PhysicsConfiguration") + configFile;

	if(!fileExists(configFile))
	{
		printd(ERROR, "Physics::loadConfig(): cannot read from file %s\n", configFile.c_str());
		return false;
	} // if

	xml = createIrrXMLReader(configFile.c_str());

	if (!xml)
	{
		printd(ERROR, "Physics::loadConfig(): ERROR: could not find config-file %s\n", configFile.c_str());
		return false;
	} // if

	printd(INFO, "Physics::loadConfig(): loading file %s\n", configFile.c_str());

	while(xml && xml->read())
	{
		switch (xml -> getNodeType())
		{
			case EXN_TEXT:
				break;
			case EXN_ELEMENT:
				if (!strcmp("gravity", xml->getNodeName()))
				{
					gravity[0] = xml->getAttributeValueAsFloat("x");
					gravity[1] = xml->getAttributeValueAsFloat("y");
					gravity[2] = xml->getAttributeValueAsFloat("z");
					simulation->setGravity(gravity);
				} // if
				else if (!strcmp("stepSize", xml->getNodeName()))
				{
					this->stepSize = xml->getAttributeValueAsFloat("value");
					simulation->setStepSize(this->stepSize);
					simTimeToNextStep = this->stepSize;
				} // else if
				else if (!strcmp("stepFunction", xml->getNodeName()))
				{
					stepFunction = xml->getAttributeValue("name");
					if (stepFunction == "NORMAL" || stepFunction == "STEP")
					{
						simulation->setStepFunction(Simulation::STEPFUNCTION_STEP);
						printd(INFO, "Physics::loadConfig(): changing to stepping function STEP!\n");
					}
					else if (stepFunction == "QUICKSTEP")
					{
						simulation->setStepFunction(Simulation::STEPFUNCTION_QUICKSTEP);
						printd(INFO, "Physics::loadConfig(): changing to stepping function QUICKSTEP!\n");
					}
					else if (stepFunction == "STEPFAST1")
					{
						printd(INFO, "Physics::loadConfig(): changing to stepping function STEPFAST1!\n");
						numberOfSteps = xml->getAttributeValueAsInt("numberOfSteps");
						if (numberOfSteps > 0)
							simulation->setStepFunction(Simulation::STEPFUNCTION_STEPFAST1, numberOfSteps);
						else
							simulation->setStepFunction(Simulation::STEPFUNCTION_STEPFAST1);
					} // else if
					else
						printd(WARNING, "Physics::loadConfig(): unknown stepFunction %s found!\n", stepFunction.c_str());
				} // else if
				else if (!strcmp("userData", xml->getNodeName()))
				{
					parsingError |= parseUserData(xml);
				} // else if
				else if (!strcmp("objectManager", xml->getNodeName()))
				{
					className = xml->getAttributeValue("type");
					printd(INFO, "Physics::loadConfig(): trying to load objectManager %s!\n", className.c_str());
					objectManager = createPhysicsObjectManager(className, xml);
				} // else if
				else if (!strcmp("synchronisationModel", xml->getNodeName()))
				{
					className = xml->getAttributeValue("type");
					if (!xml->isEmptyElement())
						arguments = readArgumentsFromXML(xml);
					else
						arguments = NULL;
					synchronisationModel = createSynchronisationModel(className, arguments);
				} // else if
				break;
			case EXN_ELEMENT_END:
				break;
			default:
				break;
		} // switch
	} // while
	delete xml;

	if (!objectManager)
	{
		printd(ERROR, "Physics::loadConfig(): No objectManager configured!\n");
		error = true;
	} // if

	if (!synchronisationModel)
	{
		printd(ERROR, "Physics::loadConfig(): No synchronisationModel configured!\n");
		error = true;
	} // if

	if (userDataMap.find(UserDatabase::getLocalUserId()) == userDataMap.end())
	{
		printd(ERROR, "Physics::loadConfig(): No UserData configuration found in config file!\n");
		error = true;
	} // if

	if (!error)
		synchronisationModel->setPhysicsObjectManager(objectManager);

	init();

	return !error;
*/
} // loadConfig

void Physics::cleanup()
{
	printd("Physics::cleanup(): enteriing\n");
	if (objectManager)
		delete objectManager;
	objectManager = NULL;

	if (synchronisationModel)
		delete synchronisationModel;
	synchronisationModel = NULL;

	systemThreadListener.clear();
	simulationStepListener.clear();

	alreadyCleanedUp = true;
} // cleanup

std::string Physics::getName()
{
	return "Physics";
} // getName

Event* Physics::createSyncEvent()
{
	SynchronisePhysicsEvent* event = new SynchronisePhysicsEvent();
#if OSG_MAJOR_VERSION >= 2
	simulationStepLock->acquire();
#else
	simulationStepLock->aquire();
#endif
		event->addPhysicsObjectManagerMsg(objectManager->createSynchronisationMessage());
	simulationStepLock->release();
	return event;
} // createSyncEvent

void Physics::registerPhysicsObjectManagerFactory(PhysicsObjectManagerFactory* factory)
{
	objectManagerFactories.push_back(factory);
} // registerPhysicsObjectManagerFactory

void Physics::registerSynchronisationModelFactory(SynchronisationModelFactory* factory)
{
	synchronisationModelFactories.push_back(factory);
} // registerSynchronisationModelFactory

void Physics::init()
{
//	UserData* localUserData;
	incomingEvents = EventManager::getPipe(PHYSICS_MODULE_ID);
	objectManager->init(this);

	// inform all other users about local UserData!!!
//	localUserData = userDataMap[UserDatabase::getLocalUserId()];
//	assert(localUserData);
//	ChangedUserDataEvent* event = new ChangedUserDataEvent(localUserData);
//	EventManager::sendEvent(event, EventManager::EXECUTE_REMOTE);
// 	synchronisationModel->init(this);
} // init

void Physics::start()
{
	if (!physicsThread)
	{
#ifdef OPENSG_THREADMANAGER_GETLOCK_HAVE_BGLOBAL
		// getThread API has probably changed at the same time as getThread and other methods; no need for an extra platform check
#if OSG_MAJOR_VERSION >= 2
		physicsThread = dynamic_pointer_cast<Thread>(ThreadManager::the()->getThread("PhysicsThread", false));
#else
		physicsThread = dynamic_cast<Thread*>(ThreadManager::the()->getThread("PhysicsThread", false));
#endif
#else
		physicsThread = dynamic_cast<Thread*>(ThreadManager::the()->getThread("PhysicsThread"));
#endif
		physicsThread->runFunction(Physics::run, 1, NULL);
	} // if
	else
		printd(WARNING, "Physics::start(): Could not start Physics Thread because its already running!\n");
} // start

void Physics::startOnEvent()
{
	if (!physicsThread)
	{
		waitForEvent = true;
#ifdef OPENSG_THREADMANAGER_GETLOCK_HAVE_BGLOBAL
		// getThread API has probably changed at the same time as getThread and other methods; no need for an extra platform check
#if OSG_MAJOR_VERSION >= 2
		physicsThread = dynamic_pointer_cast<Thread>(ThreadManager::the()->getThread("PhysicsThread", false));
#else
		physicsThread = dynamic_cast<Thread*>(ThreadManager::the()->getThread("PhysicsThread", false));
#endif
#else
		physicsThread = dynamic_cast<Thread*>(ThreadManager::the()->getThread("PhysicsThread"));
#endif
		physicsThread->runFunction(Physics::run, 1, NULL);
	} // if
	else
		printd(WARNING, "Physics::startOnEvent(): Could not start Physics Thread because its already running!\n");
} // startOnEvent

void Physics::kill()
{
	shutdown = true;
	// check every 1ms if thread is killed
	do
	{
		usleep(1000);
	} while (running);
} // kill

void Physics::simulate()
{
	float dt;
	float currentTime;
	float stepsPerFrame;

	currentTime = physicsTimer.getTime();

	// initialize timer variables
	if (simLastTime < 0)
		simLastTime = currentTime;
	if (simFpsTime < 0)
		simFpsTime = currentTime;

	dt = currentTime - simLastTime;
	simTimeToNextStep -= dt;
	simFpsTime += dt;
	simLastTime = currentTime;
	if (simFpsTime >= 1)
	{
		printd(INFO, "Physics::simulate(): FPS = %u\n", simFpsCounter);
		simFpsCounter = 0;
		simFpsTime = 0;
	} // if
	stepsPerFrame = 0;
	while (simTimeToNextStep <= 0)
	{
		singleton->step();
		simTimeToNextStep += singleton->stepSize;
		simFpsCounter++;
		stepsPerFrame++;
	} // while
	if (stepsPerFrame > 10)
		printd(WARNING, "Physics::simulate(): encountered %u simulation steps per frame!\n", stepsPerFrame);
} // simulate

bool Physics::addPhysicsObject(PhysicsObjectInterface* object)
{
	return objectManager->addPhysicsObject(object);
} // addPhysicsObject

bool Physics::removePhysicsObject(PhysicsObjectInterface* object)
{
	if (alreadyCleanedUp) {
//		printd(INFO, "Physics::removePhysicsObject(): Physics module already cleaned up, object therefore alredy removed!\n");
		return true;
	} // if

	bool success;
#if OSG_MAJOR_VERSION >= 2
	simulationStepLock->acquire();
#else
	simulationStepLock->aquire();
#endif
		success = objectManager->removePhysicsObject(object);
	simulationStepLock->release();

	if (!success) {
		printd(WARNING, "Physics::removePhysicsObject(): Could not remove PhysicsObject from Simulation!\n");
	} // if

	return success;
} // removePhysicsObject

PhysicsObjectInterface* Physics::removePhysicsObject(PhysicsObjectID physicsObjectID)
{
	if (alreadyCleanedUp) {
		printd(WARNING, "Physics::removePhysicsObject(): Physics module already cleaned up!\n");
		return NULL;
	} // if

	PhysicsObjectInterface* result;
#if OSG_MAJOR_VERSION >= 2
	simulationStepLock->acquire();
#else
	simulationStepLock->aquire();
#endif
		result = objectManager->removePhysicsObject(physicsObjectID);
	simulationStepLock->release();

	if (!result) {
		printd(WARNING, "Physics::removePhysicsObject(): Could not remove PhysicsObject from Simulation!\n");
	} // if

	return result;
} // removePhysicsObject

bool Physics::addSystemThreadListener(SystemThreadListenerInterface* listener)
{
#if OSG_MAJOR_VERSION >= 2
	systemThreadListenerLock->acquire();
#else
	systemThreadListenerLock->aquire();
#endif
		systemThreadListener.push_back(listener);
	systemThreadListenerLock->release();
	return true;
} // addSystemThreadListener

bool Physics::removeSystemThreadListener(SystemThreadListenerInterface* listener)
{
	int i, size;
	bool removed;

	if (alreadyCleanedUp)
		return true;

#if OSG_MAJOR_VERSION >= 2
	systemThreadListenerLock->acquire();
#else
	systemThreadListenerLock->aquire();
#endif
		size = systemThreadListener.size();
		for (i=0; i < size; i++)
		{
			if (systemThreadListener[i] == listener)
				break;
		} // for
		if (i < size)
		{
			systemThreadListener.erase(systemThreadListener.begin() + i);
			removed = true;
		} // if
		else
			removed = false;
	systemThreadListenerLock->release();

	return removed;
} // removeSystemThreadListener

bool Physics::addSimulationStepListener(SimulationStepListenerInterface* listener)
{
#if OSG_MAJOR_VERSION >= 2
	simulationStepListenerLock->acquire();
#else
	simulationStepListenerLock->aquire();
#endif
		simulationStepListener.push_back(listener);
	simulationStepListenerLock->release();
	return true;
} // addSimulationStepListener

bool Physics::removeSimulationStepListener(SimulationStepListenerInterface* listener)
{
	int i, size;
	bool removed;

	if (alreadyCleanedUp)
		return true;

#if OSG_MAJOR_VERSION >= 2
	simulationStepListenerLock->acquire();
#else
	simulationStepListenerLock->aquire();
#endif
		size = simulationStepListener.size();
		for (i=0; i < size; i++)
		{
			if (simulationStepListener[i] == listener)
				break;
		} // for
		if (i < size)
		{
			simulationStepListener.erase(simulationStepListener.begin() + i);
			removed = true;
		} // if
		else
			removed = false;
	simulationStepListenerLock->release();

	return removed;
} // removeSimulationStepListener

void Physics::update(float dt)
{
	int i;

// 	connectedUserLock->aquire();
// 		connectedUsers.clear();
// 		connectedUsers.push_back(UserDatabase::getLocalUserId());
// 		for (i=0; i < (int)UserDatabase::getNumberOfRemoteUsers(); i++)
// 			connectedUsers.push_back(UserDatabase::getRemoteUserByIndex(i)->getId());
// 	connectedUserLock->release();

#if OSG_MAJOR_VERSION >= 2
	systemThreadListenerLock->acquire();
#else
	systemThreadListenerLock->aquire();
#endif

		for (i=0; i < (int)systemThreadListener.size(); i++)
			systemThreadListener[i]->systemUpdate(dt);

		simulation->renderObjects();
	systemThreadListenerLock->release();
} // update

Simulation* Physics::getSimulation()
{
	return simulation;
} // getSimulation

oops::XMLLoader* Physics::getXMLLoader() {
	return xmlLoader;
} // getXMLLoader

unsigned Physics::getSimulationTime()
{
	return simulationTime;
} // getSimulationTime

float Physics::getStepSize() {
	return stepSize;
} // getStepSize

bool Physics::isPhysicsServer() {
	return (objectManager->isServer());
} // doPhysicsCalculations

bool Physics::doesPhysicsCalculations() {
	return (objectManager->isServer() || synchronisationModel->needPhysicsCalculation());
} // doesPhysicsCalculations

//******************************//
//* DEPRECATED PUBLIC METHODS: *//
//******************************//

/*
void Physics::setTriangleMeshLoader(TriangleMeshLoader* loader)
{
	simulation->setTriangleMeshLoader(loader);
} // setTriangleMeshLoader

void Physics::setRendererFactory(RendererFactory* factory)
{
	simulation->setRendererFactory(factory);
} // setRendererFactory

void Physics::setGravity(gmtl::Vec3f gravity)
{
	simulation->setGravity(gravity);
} // setGravity

void Physics::setObjectsVisible(bool visible)
{
	simulation->setObjectsVisible(visible);
} // setObjectsVisible

void Physics::setServerMode(bool server)
{
	singleton->syncModel->setServerMode(server);
} // setServerMode

bool Physics::isServer()
{
	return singleton->syncModel->amIServer();
} // isServer

void Physics::syncToServer(ExtendedBinaryMessage* msg)
{
	singleton->syncModel->syncToServer(msg);
} // syncToServer

void Physics::getAvatarCollisions(PhysicsAvatar* avatar, std::vector<ContactData*>& contacts)
{
	assert(singleton);
	singleton->simulation->checkCollisionWithWorld(avatar->getPhysicsObject(), contacts);
} // getAvatarCollisions

void Physics::removePhysicsObject(PhysicsObject* object)
{
	int i;
	unsigned short envId, entId;
	unsigned id;
	for (i=0; i < objectList.size(); i++)
	{
		if (objectList[i] == object)
			break;
	} // for
	if (i < objectList.size())
		objectList.erase(objectList.begin()+i);
	else
	{
		id = object->getEnvironmentBasedId();
		split(id, envId, entId);
		printd(WARNING, "Physics::removePhysicsEntity(): PhysicsEntity with ID (Environment=%u ID=%u) not registered!\n", envId, entId);
	} // else

	if (singleton)  // && entity->physicsObject->isBuilt())
		object->removeFromSimulation(singleton->simulation);
} // removePhysicsObject

void Physics::handleMessages()
{
	int i;
	ExtendedBinaryMessage* msg;
	std::vector<ExtendedBinaryMessage*> msgList;

	if (!network)
		return;

	network->popAll(PHYSICS_MODULE_ID, &msgList);

	for (i=0; i < (int)msgList.size(); i++)
	{
		msg = msgList[i];
		syncModel->handleMessage(msg);
		delete msg;
	} // for
	msgList.clear();
} // handleMessages
*/

//*********************//
// PROTECTED METHODS: *//
//*********************//
/*
bool Physics::parseUserData(IrrXMLReader* xml)
{
	bool parsingError = false;
	bool finished = false;
	UserData* localUserData = NULL;
	unsigned localUserId = UserDatabase::getLocalUserId();

	if (userDataMap.find(localUserId) != userDataMap.end())
		localUserData = userDataMap[localUserId];
	else
	{
		localUserData = new UserData;
		userDataMap[localUserId] = localUserData;
	} // else

	while (!finished && xml && xml->read())
	{
		switch(xml->getNodeType())
		{
			case EXN_ELEMENT:
				if (!strcmp(xml->getNodeName(), "doDeadReckoning"))
				{
					convert(xml->getAttributeValue("value"), localUserData->doesDeadReckoning);
				} // if
				else if (!strcmp(xml->getNodeName(), "actAsServer"))
				{
					convert(xml->getAttributeValue("value"), localUserData->isServer);
				} // else if
				break;
			case EXN_ELEMENT_END:
				if (!strcmp(xml->getNodeName(), "userData"))
					finished = true;
				break;
			default:
				break;
		} // switch
	} // while
	if (!finished)
		printd(ERROR, "Physics::parseUserData(): could not find end-Tag </userData>!\n");
	if (parsingError)
		printd(ERROR, "Physics::parseUserData(): Error at parsing element <userData>!\n");

	return (finished && !parsingError);
} // parseUserData
*/

PhysicsObjectManager* Physics::createPhysicsObjectManager(std::string className,
		ArgumentVector* args) {
	int i;
	PhysicsObjectManager* result = NULL;
	for (i=0; i < (int)objectManagerFactories.size(); i++)
	{
		result = objectManagerFactories[i]->create(className, args);
		if (result)
			break;
	} // for
	return result;
} // createPhysicsObjectManager


SynchronisationModel* Physics::createSynchronisationModel(std::string className,
		ArgumentVector* args) {
	int i;
	SynchronisationModel* result = NULL;
	for (i=0; i < (int)synchronisationModelFactories.size(); i++)
	{
		result = synchronisationModelFactories[i]->create(className, args);
		if (result)
			break;
	} // for
	return result;
} // createSynchronisationModel

void Physics::run(void* args)
{
	float timeToNextStep = singleton->stepSize;
	float lastTime;
	float currentTime;
	float dt;
	unsigned fpsCounter = 0;
	float fpsTime = 0;
	unsigned stepsPerFrame;
	double startCurrentTime;
	int newCountdown, countdown;
	bool slept = false;
	float sleepCount = 0;

	if (waitForEvent) {
		while (startTime == 0) {
			usleep(500000);
		} // while
		printd(INFO, "Physics::run(): found startTime (%f)\n", startTime);
		countdown = 0;
		do {
			startCurrentTime = inVRsUtilities::Timer::getSystemTime();
// 			printd(INFO, "Physics::run(): currentTime = %f\n", startCurrentTime);
			newCountdown = (int)(startTime - startCurrentTime);
			if (newCountdown != countdown) {
				countdown = newCountdown;
				printd(INFO, "Physics::run(): COUNTDOWN: %i\n", countdown);
			} // if
		} while (startCurrentTime < (startTime - timeToNextStep));
		printd(INFO, "Physics::run(): COUNTDOWN: 0\n");
	} // if

	lastTime = physicsTimer.getTime();

	printd(INFO, "Physics::run(): checking for simulationStartTime!\n");

	if (singleton->simulationStartTime == 0) {
		printd(INFO, "Physics::run(): simulationStartTime NOT SET!!!!\n");
		singleton->simulationStartTime = physicsTimer.getSystemTime();
	} // if
	else {
		// USUALLY THE SYNCHRONISEPHYSICSEVENT IS RECEIVED AFTER THE FIRST SIMULATION STEP, SO THIS PATH WILL
		// MOST PROBABLY NEVER BE CALLED!!!
		printd(INFO, "Physics::run(): found simulationStartTime!\n");
		singleton->simulationTime = (unsigned)(((physicsTimer.getSystemTime() - singleton->simulationStartTime) /  singleton->stepSize)+0.5f);
	} // else

	running = true;
	// loop until kill-method is called
	while (!shutdown)
	{
		currentTime = physicsTimer.getTime();
		dt = currentTime - lastTime;
		timeToNextStep -= dt;
		fpsTime += dt;
		lastTime = currentTime;
		if (slept)
		{
//			if (timeToNextStep < 0)
//				printd("Overslept %f seconds!\n", -timeToNextStep);
//			else
//				printd("Still %f seconds after sleep!\n", timeToNextStep);
			sleepCount += dt;
			slept = false;
		} // if
		if (fpsTime >= 1)
		{
			singleton->simulationLoad = (fpsTime-sleepCount)*100;
			printd(INFO, "Physics::run(): FPS = %u / LOAD: %0.2f\%\t(simulationTime = %u)\n",
					fpsCounter, singleton->simulationLoad, singleton->simulationTime);
			fpsCounter = 0;
			fpsTime = 0;
			sleepCount = 0;
		} // if
		if (timeToNextStep > 0)
		{
			usleep((unsigned)(timeToNextStep * 1000000));
			slept = true;
			continue;
		} // if
		stepsPerFrame = 0;
		while (timeToNextStep <= 0)
		{
			singleton->step();
			timeToNextStep += singleton->stepSize;
			fpsCounter++;
			stepsPerFrame++;
		} // while
		if (stepsPerFrame > 10)
			printd(WARNING, "Physics::run(): encountered %u simulation steps per frame!\n", stepsPerFrame);
	} // while
	running = false;
} // run

void Physics::step()
{
#if OSG_MAJOR_VERSION >= 2
	simulationStepLock->acquire();
#else
	simulationStepLock->aquire();
#endif
		handleEvents();
		synchronisationModel->handleMessages();
		objectManager->step(stepSize);
		handleSimulationStepListener(stepSize);
		synchronisationModel->synchroniseBeforeStep();
		if (objectManager->isServer() || synchronisationModel->needPhysicsCalculation())
			simulation->step(stepSize);
		simulationTime++;
		synchronisationModel->synchroniseAfterStep();
	simulationStepLock->release();
} // step

void Physics::handleEvents()
{
	std::deque<Event*>* incomingEventsLocal;
	Event* evt;

	incomingEventsLocal = incomingEvents->makeCopyAndClear();
	while(!incomingEventsLocal->empty())
	{
		printd(INFO, "Physics::handleEvents(): found an Event!\n");
		evt = (*incomingEventsLocal)[0];
		incomingEventsLocal->pop_front();
		evt->execute();
		delete evt;
		printd(INFO, "Physics::handleEvents(): Event executed and deleted!\n");
	}
	delete incomingEventsLocal;
} // handleEvents

void Physics::handleSimulationStepListener(float dt)
{
	int i;

#if OSG_MAJOR_VERSION >= 2
	simulationStepListenerLock->acquire();
#else
	simulationStepListenerLock->aquire();
#endif
		for (i=0; i < (int)simulationStepListener.size(); i++)
			simulationStepListener[i]->step(dt, simulationTime);
	simulationStepListenerLock->release();
} // handleSimulationStepListener

void Physics::setStartTime(double time)
{
	startTime = time;
} // setStartTime

void Physics::setSynchronisationModel(std::string modelName, ArgumentVector* arguments) {
	// no need to aquire lock since method is only called from event and event
	// is executed in handleEvents method
	SynchronisationModel* newModel = createSynchronisationModel(modelName, arguments);
	if (!newModel)
		printd(WARNING, "Physics::setSynchronisationModel(): Could not create new SynchronisationModel %s, keeping old one!\n", modelName.c_str());
	else {
		delete synchronisationModel;
		newModel->setPhysicsObjectManager(objectManager);
		synchronisationModel = newModel;
		printd(INFO, "Physics::setSynchroniationModel(): Changed SynchroniationModel to %s!\n", modelName.c_str());
	} // else
} // setSynchronisationModel

//*****************************************************************************
// Configuration loading
//*****************************************************************************
Physics::ConverterToV1_0a4::ConverterToV1_0a4() :
	XmlConfigurationConverter("0.0", "1.0a4") {

} // ConverterToV1_0a4

bool Physics::ConverterToV1_0a4::convert(XmlDocument* document, const Version& version,
		std::string configFile) {
	bool success = true;

	updateDocument(document, XmlConfigurationLoader::XmlDtdUrl + "physics_v1.0a4.dtd",
			destinationVersion, "physics");

//TODO: update if neccessary!

	return success;
} // convert

MAKEMODULEPLUGIN(Physics, SystemCore)

