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

#include "UserDatabase.h"
#include "../DebugOutput.h"
#include "VirtualHandCursorModel.h"
#include "GoGoCursorModel.h"
#include "HeadPositionUserTransformationModel.h"
#include "UserDatabaseEvents.h"
#include "SimpleCursorRepresentation.h"
#include "InteractionCursorRepresentation.h"
#include "../EventManager/EventManager.h"
#include <inVRs/OutputInterface/OutputInterface.h>
#include "../UtilityFunctions.h"

// disable deprecation warning for std::auto_ptr when std::unique_ptr is not available.
#ifndef HAS_CXX11_UNIQUE_PTR
// Only do this on GCC versions that support it:
# if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 402
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
#endif

User*										UserDatabase::localUser = NULL;
std::vector<User*>							UserDatabase::remoteUserList;
std::vector<CursorRepresentationFactory*>	UserDatabase::cursorRepresentationFactories;
std::vector<AbstractUserConnectCB*>			UserDatabase::userConnectCallbacks;
std::vector<AbstractUserDisconnectCB*>		UserDatabase::userDisconnectCallbacks;
XmlConfigurationLoader 						UserDatabase::cursorXmlConfigLoader;

void UserDatabase::init() {
	User::registerCursorTransformationModelFactory(new VirtualHandCursorModelFactory);
	User::registerCursorTransformationModelFactory(new GoGoCursorModelFactory);
	User::registerUserTransformationModelFactory(new HeadPositionUserTransformationModelFactory);

	EventManager::registerEventFactory("UserDatabaseAddUserEvent",
			new UserDatabaseAddUserEvent::Factory());
	EventManager::registerEventFactory("UserDatabaseRemoveUserEvent",
			new UserDatabaseRemoveUserEvent::Factory());
	EventManager::registerEventFactory("UserDatabasePickUpEntityEvent",
			new UserDatabasePickUpEntityEvent::Factory());
	EventManager::registerEventFactory("UserDatabaseDropEntityEvent",
			new UserDatabaseDropEntityEvent::Factory());

	registerCursorRepresentationFactory(new SimpleCursorRepresentationFactory());
	registerCursorRepresentationFactory(new InteractionCursorRepresentationFactory());

	localUser = new User();
} // init

void UserDatabase::cleanup() {
	delete localUser;
	User::cleanup();

	std::vector<AbstractUserConnectCB*>::iterator it1;
	std::vector<AbstractUserDisconnectCB*>::iterator it2;
	std::vector<CursorRepresentationFactory*>::iterator it3;

	for (it1 = userConnectCallbacks.begin(); it1 != userConnectCallbacks.end(); ++it1)
		delete *it1;
	userConnectCallbacks.clear();

	for (it2 = userDisconnectCallbacks.begin(); it2 != userDisconnectCallbacks.end(); ++it2)
		delete *it2;
	userDisconnectCallbacks.clear();

	for (it3 = cursorRepresentationFactories.begin(); it3 != cursorRepresentationFactories.end();
			++it3) {
		delete *it3;
	} // for
	cursorRepresentationFactories.clear();
} // cleanup

void UserDatabase::addRemoteUser(User* remoteUser) {
	int i;
	std::vector<AbstractUserConnectCB*>::iterator it;

	if (remoteUser->getId() == localUser->getId()) {
		printd(
				WARNING,
				"UserDatabase::addRemoteUser(): cannot add user with id %u because its identical to local user id\n",
				remoteUser->getId());
		return;
	} // if
	for (i = 0; i < (int)remoteUserList.size(); i++) {
		if (remoteUser->getId() == remoteUserList[i]->getId()) {
			printd(WARNING, "UserDatabase::addRemoteUser(): a user with id %u already exists\n",
					remoteUser->getId());
			return;
		} // if
	} // for
	remoteUserList.push_back(remoteUser);

	for (it = userConnectCallbacks.begin(); it != userConnectCallbacks.end(); ++it)
		(*it)->call(remoteUser);
} // addRemoteUser

void UserDatabase::removeRemoteUser(unsigned userId) {
	int i;
	User* user;
	std::vector<AbstractUserDisconnectCB*>::iterator it;

	for (i = 0; i < (int)remoteUserList.size(); i++) {
		if (userId == remoteUserList[i]->getId()) {
			printd(INFO, "UserDatabase::removeRemoteUser(): removing user %u\n", userId);
			user = remoteUserList[i];
			remoteUserList.erase(remoteUserList.begin() + i);

			for (it = userDisconnectCallbacks.begin(); it != userDisconnectCallbacks.end(); ++it)
				(*it)->call(user);

			delete user;
			printd(INFO, "UserDatabase::removeRemoteUser(): done!\n");
			return;
		} // if
	} // for
	printd(WARNING, "UserDatabase::removeRemoteUser(): couldn't find any user with id %u\n", userId);
} // removeRemoteUser

void UserDatabase::registerUserConnectCallback(AbstractUserConnectCB& callback) {
	userConnectCallbacks.push_back(callback.clone());
} // registerUserConectCallback

void UserDatabase::unregisterUserConnectCallback(AbstractUserConnectCB& callback) {
	bool success = false;
	std::vector<AbstractUserConnectCB*>::iterator it;

	for (it = userConnectCallbacks.begin(); it != userConnectCallbacks.end(); ++it) {
		if ((**it) == callback) {
			delete *it;
			userConnectCallbacks.erase(it);
			success = true;
			break;
		} // if
	} // for

	if (!success)
		printd(WARNING,
				"UserDatabase::unregisterUserConnectCallback(): passed callback not registered!\n");
} // unregisterUserConnectCallback

void UserDatabase::registerUserDisconnectCallback(AbstractUserDisconnectCB& callback) {
	userDisconnectCallbacks.push_back(callback.clone());
} // registerUserDisconnectCallback

void UserDatabase::unregisterUserDisconnectCallback(AbstractUserDisconnectCB& callback) {
	bool success = false;
	std::vector<AbstractUserDisconnectCB*>::iterator it;

	for (it = userDisconnectCallbacks.begin(); it != userDisconnectCallbacks.end(); ++it) {
		if ((**it) == callback) {
			delete *it;
			userDisconnectCallbacks.erase(it);
			success = true;
			break;
		} // if
	} // for

	if (!success)
		printd(WARNING,
				"UserDatabase::unregisterUserDisconnectCallback(): passed callback not registered!\n");
} // unregisterUserDisconnectCallback

void UserDatabase::registerCursorRepresentationFactory(CursorRepresentationFactory* factory) {
	cursorRepresentationFactories.push_back(factory);
} // registerCursorRepresentationFactory

void UserDatabase::updateCursors(float dt) {
	localUser->updateCursor(dt);
	std::vector<User*>::iterator it;
	for (it = remoteUserList.begin(); it != remoteUserList.end(); ++it) {
		(*it)->updateCursor(dt);
	} // while
} // updateCursors

unsigned UserDatabase::getLocalUserId() {
	if (!localUser) {
		printd(WARNING, "UserDatabase::getLocalUserId(): UserDatabase not initialized yet!\n");
		return 0;
	}
	return localUser->getId();
} // getLocalUserId


User* UserDatabase::getLocalUser() {
	return localUser;
} // getLocalUser

int UserDatabase::getNumberOfRemoteUsers() {
	return remoteUserList.size();
} // getNumberOfRemoteUsers

User* UserDatabase::getUserById(unsigned userId) {
	int i;
	if (localUser->getId() == userId)
		return localUser;

	for (i = 0; i < (int)remoteUserList.size(); i++) {
		if (remoteUserList[i]->getId() == userId)
			return remoteUserList[i];
	} // for

	printd(INFO, "UserDatabase::getUserById(): cannot find a user with id %u\n", userId);
	return NULL;
} // getUserById

User* UserDatabase::getRemoteUserByIndex(int idx) {
	if (idx >= (int)remoteUserList.size()) {
		printd(
				INFO,
				"UserDatabase::getRemoteUserByIndex(): list does not have an element with index %d\n",
				idx);
		return NULL;
	} // if
	return remoteUserList[idx];
} // getRemoteUserByIndex

CursorRepresentationInterface* UserDatabase::loadCursorRepresentation(std::string configFile) {

	SceneGraphInterface* sgIF = OutputInterface::getSceneGraphInterface();
	if (!sgIF) {
		printd(ERROR,
				"UserDatabase::loadCursorRepresentation(): ERROR obtaining SceneGraphInterface!\n");
		return NULL;
	} // if

	int i;
	std::string cursorType;
	CursorRepresentationInterface* result = NULL;

	std::string configFileConcatenatedPath = getConcatenatedPath(configFile, "CursorRepresentationConfiguration");
	printd(INFO, "UserDatabase::loadCursorRepresentation(): loading configuration %s\n",
			configFile.c_str());

	// load xml document via xmlConfigLoader
	std::auto_ptr<const XmlDocument> document(
			cursorXmlConfigLoader.loadConfiguration(configFileConcatenatedPath, false));
	if (!document.get()) {
		printd(ERROR,
				"UserDatabase::loadCursorRepresentation(): error at loading of configuration file %s!\n",
				configFile.c_str());
		return NULL;
	} // if

	cursorType = document->getRootElement()->getName();
	cursorType[0] = toupper(cursorType[0]);
	for (i = 0; i < (int)cursorRepresentationFactories.size(); i++) {
		// We have to pass the file instead of the document, because we may need to update the
		// configuration versions
		result = cursorRepresentationFactories[i]->create(cursorType, configFile);
		if (result)
			break;
	} // for

	return result;
} // loadCursorRepresentation

