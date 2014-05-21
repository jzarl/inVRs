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

#include "SystemCoreEvents.h"

#include <sstream>
#include <assert.h>

#include "SystemCore.h"
#include "EventManager/EventManager.h"
#include "ComponentInterfaces/NetworkInterface.h"
#include "UserDatabase/UserDatabase.h"
#include "WorldDatabase/WorldDatabaseEvents.h"

SystemCoreRequestSyncEvent::SystemCoreRequestSyncEvent() :
	Event(SYSTEM_CORE_ID, SYSTEM_CORE_ID, "SystemCoreRequestSyncEvent") {
	requestUserId = UserDatabase::getLocalUserId(); // everything will be overwritten when event is beeing deserialized during decode()
} // SystemCoreRequestSyncEvent

void SystemCoreRequestSyncEvent::encode(NetMessage* message) {
	printd(INFO, "SystemCoreRequestSyncEvent::encode(): encoding userId %u\n",
			requestUserId);
	message->putUInt32(requestUserId);
} // encode

void SystemCoreRequestSyncEvent::decode(NetMessage* message) {
	message->getUInt32(requestUserId);
	printd(INFO, "SystemCoreRequestSyncEvent::decode(): decoded userId %u\n",
			requestUserId);
} // decode

void SystemCoreRequestSyncEvent::execute() {
	NetworkInterface* netInt;
	std::map<std::string, ModuleInterface*>::const_iterator it;
	Event* syncEvent;

	printd(INFO,
			"SystemCoreRequestSyncEvent::execute(): running syncEvent for user %u ...\n",
			requestUserId);

	netInt = (NetworkInterface*)SystemCore::getModuleByName("Network");
	assert(netInt); // is not intended to be executed locally

	const std::map<std::string, ModuleInterface*>& moduleMap = SystemCore::getModuleMap();

	// sync events from non-modules:
	syncEvent = new WorldDatabaseSyncEvent(TRANSFORMATION_MANAGER_ID);
	EventManager::sendEventTo(syncEvent, requestUserId);

	for (it = moduleMap.begin(); it != moduleMap.end(); ++it) {
		syncEvent = it->second->createSyncEvent();
		if (syncEvent) {
			printd(INFO,
					"SystemCoreRequestSyncEvent::execute(): encoding and sending syncEvent from module %s!\n",
					it->second->getName().c_str());
			EventManager::sendEventTo(syncEvent, requestUserId);
		} // if
	} // for
	printd(INFO, "SystemCoreRequestSyncEvent::execute(): DONE!\n");
} // execute

std::string SystemCoreRequestSyncEvent::toString() {
	std::stringstream ss(std::stringstream::in | std::stringstream::out);

	ss << "EVENT: SystemCoreRequestSyncEvent\n";
	ss << "timestamp = " << evt_timestamp << std::endl;
	return ss.str();
} // toString

