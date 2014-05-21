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

#include <assert.h>

#include <set>
#include <memory>

#include <OpenSG/OSGLock.h>
#include <OpenSG/OSGThreadManager.h>

#include "EventManager.h"
#include "../DebugOutput.h"
#include "../UserDatabase/UserDatabase.h"
#include "../Platform.h"
#include "../IdPoolListener.h"
#include "../UtilityFunctions.h"

// disable deprecation warning for std::auto_ptr when std::unique_ptr is not available.
#ifndef HAS_CXX11_UNIQUE_PTR
// Only do this on GCC versions that support it:
# if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 402
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
#endif

OSG_USING_NAMESPACE

bool											EventManager::initialized = false;
bool											EventManager::isRunning = false;
bool											EventManager::isLogging = false;
bool											EventManager::createNewLogFile = true;
bool											EventManager::shutdown = false;
#if OSG_MAJOR_VERSION >= 2
ThreadRefPtr									EventManager::eventRecvThread = NULL;
#else //OpenSG1:
Thread* 										EventManager::eventRecvThread = NULL;
#endif
EventPipe*										EventManager::pipes[256];
unsigned										EventManager::sequenceNumber = 0;
NetworkInterface*								EventManager::networkController;
std::map<std::string, AbstractEventFactory*>	EventManager::eventFactoryMap;
std::map<unsigned, AbstractEventFactory*>		EventManager::hashEventFactoryMap;
std::map<std::string, unsigned>					EventManager::eventHashMap;
std::map<unsigned, std::string>					EventManager::idToEventName;
#if OSG_MAJOR_VERSION >= 2
	LockRefPtr									EventManager::loggingLock;
#else //OpenSG1:
Lock*											EventManager::loggingLock;
#endif
std::vector<std::string>						EventManager::eventLogList;
std::string										EventManager::logFile;

XmlConfigurationLoader 							EventManager::xmlConfigLoader;


void EventManager::init() {
	shutdown = false;
	int i;
	for (i = 0; i < 256; i++) {
		pipes[i] = NULL;
	} // for
	networkController = NULL;
	isRunning = false;
#if OSG_MAJOR_VERSION >= 2
	loggingLock = OSG::dynamic_pointer_cast<OSG::Lock> (ThreadManager::the()->getLock("loggingLock",false));
#else //OpenSG1:
	loggingLock = dynamic_cast<Lock*> (ThreadManager::the()->getLock("loggingLock"));
#endif
	isLogging = false;
	createNewLogFile = true;

	registerEventFactory("IdPoolListenerFreeIndexRequestEvent",
			new IdPoolListenerFreeIndexRequestEvent::Factory());
	registerEventFactory("IdPoolListenerVetoRequestEvent",
			new IdPoolListenerVetoRequestEvent::Factory());
	registerEventFactory("IdPoolListenerFinalizeAllocationEvent",
			new IdPoolListenerFinalizeAllocationEvent::Factory());

	initialized = true;
} // init

bool EventManager::loadConfig(std::string configFile) {
	bool success = true;

	if (!initialized) {
		printd(WARNING,
				"EventManager::loadConfig(): EventManager not initialized yet - initializing now!\n");
		init();
	} // if

	std::string configFileConcatenatedPath = getConcatenatedPath(configFile, "EventManagerConfiguration");
	printd(INFO, "EventManager::loadConfig(): loading configuration %s!\n", configFile.c_str());

	// load xml document via xmlConfigLoader
	std::auto_ptr<const XmlDocument> document(
			xmlConfigLoader.loadConfiguration(configFileConcatenatedPath));
	if (!document.get()) {
		printd(ERROR,
				"EventManager::loadConfig(): error at loading of configuration file %s!\n",
				configFile.c_str());
		return false;
	} // if

	return success;
} // loadConfig

void EventManager::cleanup() {
	int i;
	std::map<std::string, AbstractEventFactory*>::iterator it;
	std::set<AbstractEventFactory*> deletedFactories;

	if (isRunning) {
#if OSG_MAJOR_VERSION >= 2
		OSG::BarrierRefPtr eventmanagerCleanupBarrier = dynamic_pointer_cast<OSG::Barrier> (ThreadManager::the()->getBarrier(
					"eventmanagerCleanupBarrier",false));
#else //OpenSG1:
		Barrier* eventmanagerCleanupBarrier = dynamic_cast<Barrier*> (ThreadManager::the()->getBarrier(
					"eventmanagerCleanupBarrier"));
#endif
		printd(INFO, "EventManager::cleanup(): entering barrier ...\n");
		assert(eventmanagerCleanupBarrier);
		eventmanagerCleanupBarrier->enter(2);
		printd(INFO, "EventManager::cleanup(): barrier passed\n");
	} // if

	for (i = 0; i < 256; i++) {
		if (pipes[i])
			delete pipes[i];
	} // for
	// delete factories: store already freed pointers since factories can be registered multiple times!!!
	for (it = eventFactoryMap.begin(); it != eventFactoryMap.end(); ++it) {
		if (deletedFactories.find(it->second) == deletedFactories.end()) {
			printd(INFO, "EventManager::cleanup(): deleting factory for Event with Name %s\n",
					it->first.c_str());
			deletedFactories.insert(deletedFactories.end(), it->second);
			delete it->second;
		} // if
	} // for
	deletedFactories.clear();
	eventFactoryMap.clear();
	hashEventFactoryMap.clear();
	eventHashMap.clear();
	dump();
} // cleanup

void EventManager::registerNetwork(NetworkInterface* netInt) {
	if (isRunning) {
		printd(ERROR, "EventManager::registerNetwork(): eventmanager is already running\n");
		return;
	}

	networkController = netInt;
} // registerNetwork

void EventManager::registerEventFactory(std::string name, AbstractEventFactory* evtFact) {
	if (isRunning) {
		printd(ERROR,
				"EventManager::registerEventFactory(): Could not register Factory for Event %s! EventManager is already running\n",
				name.c_str());
	} // if
	else if (evtFact == NULL) {
		printd(ERROR,
				"EventManager::registerEventFactory(): No EventFactory passed for Event with name %s!\n",
				name.c_str());
	} // else if
	else if (eventFactoryMap.find(name) != eventFactoryMap.end()) {
		printd(WARNING,
				"EventManager::registerEventFactory(): Factory for Event %s already registered!\n",
				name.c_str());
	} // else if
	else {
		eventFactoryMap[name] = evtFact;
	} // else
} // registerEventFactory

void EventManager::start() {
	if (isRunning) {
		printd(WARNING,
				"EventManager::start(): EventManager was started already! Second start attempt will be ignored!\n");
		return;
	} // if
	generateHashValues();
#if OSG_MAJOR_VERSION >= 2
	eventRecvThread = dynamic_pointer_cast<OSG::Thread> (ThreadManager::the()->getThread("eventRecvThread",false));
#else //OpenSG1:
	eventRecvThread = dynamic_cast<Thread *> (ThreadManager::the()->getThread("eventRecvThread"));
#endif
	eventRecvThread->runFunction(EventManager::run, 0, NULL);
	isRunning = true;
}

void EventManager::stop() {
	shutdown = true;
} // stop

void EventManager::sendEvent(Event* event, EXECUTION_MODE mode) {

	if (!isRunning && mode != EXECUTE_LOCAL) {
		printd(ERROR,
				"EventManager::sendEvent(): EventManager was not started yet! EVENT WILL NOT BE SENT!\n");
		delete event;
		return;
	} // if

	// TODO: insert lock for sequenceNumber and eventHashMap here!!
	sequenceNumber++;
	event->setEventId(eventHashMap[event->getEventName()]);

	if (mode != EXECUTE_LOCAL) {
		if (networkController)
			networkController->sendEvent(event); //broadcast event
	}
	if (mode == EXECUTE_LOCAL || mode == EXECUTE_GLOBAL) {
		putEventInPipe(event);
		//		event->execute();
	} // if

	if (isLogging && (mode != EXECUTE_LOCAL)) {
#if OSG_MAJOR_VERSION >= 2
		loggingLock->acquire();
#else //OpenSG1:
		loggingLock->aquire();
#endif
		eventLogList.push_back(event->toString());
		loggingLock->release();
	} // if

	if (mode == EXECUTE_REMOTE)
		delete event;
} // sendEvent

void EventManager::sendEventTo(Event* event, unsigned userId) {
	User* user;

	if (!isRunning) {
		printd(ERROR,
				"EventManager::sendEventTo(): EventManager was not started yet! EVENT WILL NOT BE SENT!\n");
		delete event;
		return;
	} // if

	user = UserDatabase::getUserById(userId);
	if (user == NULL) {
		printd(WARNING, "EventManager::sendEventTo(): couldn't find user with id %u\n", userId);
		return;
	} // if

	// TODO: insert lock for sequenceNumber and eventHashMap here!!
	sequenceNumber++;
	event->setEventId(eventHashMap[event->getEventName()]);

	if (userId == UserDatabase::getLocalUserId())
		putEventInPipe(event);
	else {
		event->visibilityLevel.push_back("destinationUserId", userId);
		if (networkController)
			networkController->sendEvent(event);

		if (isLogging) {
#if OSG_MAJOR_VERSION >= 2
			loggingLock->acquire();
#else //OpenSG1:
			loggingLock->aquire();
#endif
			eventLogList.push_back(event->toString());
			loggingLock->release();
		} // if
		delete event;
	} // else
} // sendEventTo

void EventManager::activateLogging(std::string logFile, bool append) {
	EventManager::logFile = logFile;
	isLogging = true;
	createNewLogFile = !append;
} // activateLogging

void EventManager::deactivateLogging() {
	dump();
	isLogging = false;
} // deactivateLogging

void EventManager::dump() {
	if (!isLogging)
		return;

	FILE* file;
	std::vector<std::string>::iterator it;
	if (createNewLogFile)
		file = fopen(logFile.c_str(), "w");
	else
		file = fopen(logFile.c_str(), "a");
#if OSG_MAJOR_VERSION >= 2
	loggingLock->acquire();
#else //OpenSG1:
	loggingLock->aquire();
#endif
	for (it = eventLogList.begin(); it != eventLogList.end(); ++it)
		fprintf(file, "%s", (*it).c_str());
	eventLogList.clear();
	loggingLock->release();
	fclose(file);

	createNewLogFile = false;
} // dump

EventPipe* EventManager::getPipe(uint8_t id) {
	if (pipes[id] == NULL)
		pipes[id] = new EventPipe;
	return pipes[id];
} // getPipe

void EventManager::putEventInPipe(Event* event) {
	unsigned moduleId = event->getDstModuleId();
	if (pipes[moduleId] == NULL) {
		printd(INFO,
				"EventManager::putEventInPipe(): opening new EventPipe for Module with ID %u\n",
				moduleId);
		pipes[moduleId] = new EventPipe;
	} // if
	pipes[moduleId]->push_back(event);
} // putEventInPipe

Event* EventManager::decode(NetMessage* msg) {
	Event* ret = NULL;
	// 	UInt32 type;
	unsigned eventId;
	std::string eventName;
	// 	UInt32 sequence;
	AbstractEventFactory* evtFact;

	// 	msg->getUInt32(type);
	msg->getUInt32(eventId);
	eventName = idToEventName[eventId];
	// 	msg->getUInt32(sequence);

	evtFact = hashEventFactoryMap[eventId];
	if (evtFact) {
		ret = evtFact->create(eventName);
		assert(ret);
		ret->completeDecode(msg, eventName);
	} else {
		printd(ERROR, "EventManager::decode(): unknown event type with name %s and ID %u\n",
				eventName.c_str(), eventId);
		if (isLogging) {
#if OSG_MAJOR_VERSION >= 2
			loggingLock->acquire();
#else //OpenSG1:
			loggingLock->aquire();
#endif
			eventLogList.push_back("(EE) EVENT: unknown event type with name " + eventName + "\n");
			loggingLock->release();
		} // if
	} // else

	return ret;
} // decode

void EventManager::generateHashValues() {
	std::map<std::string, AbstractEventFactory*>::iterator it;
	unsigned hashValue;

	printd(INFO, "EventManager::generateHashValues(): generating hash values for Events!\n");

	for (it = eventFactoryMap.begin(); it != eventFactoryMap.end(); ++it) {
		hashValue = generateHashCode(it->first);
		if (hashEventFactoryMap.find(hashValue) != hashEventFactoryMap.end()) {
			do {
				printd(
						INFO,
						"EventManager::generateHashValues(): HASH COLLISION for event %s / value = %u!\n",
						it->first.c_str(), hashValue);
				hashValue++;
			} while (hashEventFactoryMap.find(hashValue) != hashEventFactoryMap.end());
		} // if
		printd(INFO, "EventManager::generateHashValues(): Unique hash value for %s is %u!\n",
				it->first.c_str(), hashValue);
		hashEventFactoryMap[hashValue] = it->second;
		eventHashMap[it->first] = hashValue;
		idToEventName[hashValue] = it->first;
	} // for
} // generateHashValues

void EventManager::run(void* dummy) {
	NetMessage* recvMsg;
	Event* recvEvent;

	if (networkController) {
		while (!shutdown) {

			while (networkController->sizeRecvList(EVENT_MANAGER_ID)) {
				// 				printd("EventManager::run(): received something\n");
				recvMsg = networkController->pop(EVENT_MANAGER_ID);
				recvEvent = decode(recvMsg);
				if (recvEvent) {
					putEventInPipe(recvEvent);
					if (isLogging) {
#if OSG_MAJOR_VERSION >= 2
						loggingLock->acquire();
#else //OpenSG1:
						loggingLock->aquire();
#endif
						eventLogList.push_back(recvEvent->toString());
						loggingLock->release();
					} // if
				} // if
				delete recvMsg;
			} // while
			usleep(1000);
		} // while
	} // if
	else
		printd(WARNING, "EventManager::run():  No network module found!\n");

#if OSG_MAJOR_VERSION >= 2
	BarrierRefPtr eventmanagerCleanupBarrier = dynamic_pointer_cast<OSG::Barrier> (ThreadManager::the()->getBarrier(
				"eventmanagerCleanupBarrier",false));
#else //OpenSG1:
	Barrier* eventmanagerCleanupBarrier = dynamic_cast<Barrier*> (ThreadManager::the()->getBarrier(
				"eventmanagerCleanupBarrier"));
#endif
	printd(INFO, "EventManager::run(): entering barrier ...\n");
	assert(eventmanagerCleanupBarrier);
	eventmanagerCleanupBarrier->enter(2);
	printd(INFO, "EventManager::run(): barrier passed\n");
	isRunning = false;
} // run
