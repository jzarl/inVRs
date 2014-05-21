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

/*---------------------------------------------------------------------------*\
 *                                Changes                                    *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 \*---------------------------------------------------------------------------*/

#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include <map>
#include <string>

#include <OpenSG/OSGThread.h>

#include "AbstractEventFactory.h"
#include "Event.h"
#include "../ModuleIds.h"
#include "../SyncPipe.h"
#include "../ComponentInterfaces/NetworkInterface.h"
#include "../XmlConfigurationLoader.h"

/**
 * A threadsafe version of std::deque.
 * The EventManager stores newly arrived Events in EventPipes, each module
 * gets its Events by accessing the proper EventPipe.
 */
typedef SyncPipe<Event*> EventPipe;

/******************************************************************************
 * The EventManager asynchronously sends outgoing Events and processes
 * incoming Events and stores them in EventPipes. It runs as a separate thread.
 * The Events can be local, remote, and global (global Events are processed both
 * locally and globally).
 * The source module of an Event uses the sendEvent() method.
 * The receiving module gets the EventPipe via the getPipe() method, and then
 * calls the Event::execute() method for each Event in the pipe.
 *
 * @see NetworkInterface
 * @see Event
 */
class INVRS_SYSTEMCORE_API EventManager {
public:
	enum EXECUTION_MODE {
		EXECUTE_LOCAL = 0, /// Only execute Event locally.
		EXECUTE_REMOTE = 1, /// Send Event to remote Users, but don't execute it locally.
		EXECUTE_GLOBAL = 2
	/// Send Event to both local and remote Users.
	};

	/**
	 * Initialise the EventManager.
	 * Must be called before any other EventManager method is used.
	 */
	static void init();

	/**
	 * Loads the configuration of the EventManager
	 */
	static bool loadConfig(std::string configFile);

	/**
	 * Free all EventFactories and Pipes, and flush the logfile.
	 */
	static void cleanup();

	/**
	 * Register a NetworkInterface for sending remote Events.
	 * A NetworkInterface can only be set before calling EventManager::start().
	 */
	static void registerNetwork(NetworkInterface* netInt);

	/**
	 * Register an EventFactory with a given name.
	 * There can only be one factory per name, so calling this method twice with
	 * the same name (and Factories != <code>NULL</code> will not accomplish anything.
	 * To deregister a Factory, call registerEventFactory with the desired name and
	 * <code>NULL</code> as Factory.
	 * @param name Specifies the name of the Event type the factory produces.
	 * @param evtFact A Factory for Events of the given name, or null.
	 */
	static void registerEventFactory(std::string name, AbstractEventFactory* evtFact);

	/**
	 * Start the EventManager.
	 * This method creates and starts a new Thread for the EventManager.
	 */
	static void start();

	/**
	 * Stop the EventManager.
	 */
	static void stop();

	/**
	 * Send an Event to the recipients indecated by the <code>mode</code> parameter.
	 * Note: Events are always deleted by the receiver, not the sender.
	 */
	static void sendEvent(Event* event, EXECUTION_MODE mode); // events are deleted by the receiver (also true for remote only events)


	/**
	 * Send an Event to the User with the specified userId.
	 * Note: Events are always deleted by the receiver, not the sender.
	 */
	static void sendEventTo(Event* event, unsigned userId);

	/**
	 * Activate logging for all sent and received Events.
	 * The logfile will only be written, when one of the methods cleanup(),
	 * deactivateLogging(), or dump() is called.
	 * @param logFile path to the logfile
	 * @param append sets append or overwrite mode.
	 */
	static void activateLogging(std::string logFile, bool append = false);

	/**
	 * Deactivate logging and flush the logfile.
	 */
	static void deactivateLogging();

	/**
	 * Flush the logfile.
	 */
	static void dump();

	/**
	 * Get the EventPipe for incoming Events for the given module.
	 * @param id numerical module id (as defined in ModuleIds.h)
	 */
	static EventPipe* getPipe(uint8_t id);

private:
	static bool initialized;
	static bool shutdown;
	static bool isRunning;
	static unsigned sequenceNumber;
#if OSG_MAJOR_VERSION >= 2
	static OSG::ThreadRefPtr eventRecvThread;
#else //OpenSG1:
	static OSG::Thread* eventRecvThread;
#endif
	static std::map<std::string, AbstractEventFactory*> eventFactoryMap;
	static std::map<unsigned, AbstractEventFactory*> hashEventFactoryMap;
	static std::map<std::string, unsigned> eventHashMap;
	static std::map<unsigned, std::string> idToEventName;
	static EventPipe* pipes[256];
	static NetworkInterface* networkController;
#if OSG_MAJOR_VERSION >= 2
	static OSG::LockRefPtr loggingLock;
#else //OpenSG1:
	static OSG::Lock* loggingLock;
#endif
	static std::vector<std::string> eventLogList;
	static bool isLogging;
	static bool createNewLogFile;
	static std::string logFile;

	static void putEventInPipe(Event* event);
	static Event* decode(NetMessage* msg);

	static void generateHashValues();

	/**
	 * Internal function to be called by the ThreadManager.
	 * Process events until EventManager::stop() is called.
	 */
	static void run(void* dummy);

//*****************************************************************************
// Configuration loading
//*****************************************************************************
private:
	static XmlConfigurationLoader xmlConfigLoader;

}; // EventManager

#endif // EVENTMANAGER_H
