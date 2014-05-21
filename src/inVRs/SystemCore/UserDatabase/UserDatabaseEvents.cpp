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
#include <string.h>

#include <sstream>

#include "UserDatabaseEvents.h"
#include "User.h"
#include "UserDatabase.h"
#include "../TransformationManager/TransformationManager.h"
#include "../DebugOutput.h"
#include "../Platform.h"
#include "../MessageFunctions.h"
#include "../SystemCore.h"

#include "../../InputInterface/InputInterface.h"

UserDatabasePickUpEntityEvent::UserDatabasePickUpEntityEvent() :
	Event() {}

UserDatabasePickUpEntityEvent::UserDatabasePickUpEntityEvent(User* userPickingUp, Entity* entity,
		TransformationData offset) :
	Event(USER_DATABASE_ID, SYSTEM_CORE_ID, "UserDatabasePickUpEntityEvent") {
	userIdPickingUp = userPickingUp->getId();
	typeBasedEntityId = entity->getTypeBasedId();
	this->offset = offset;
}

void UserDatabasePickUpEntityEvent::encode(NetMessage* message) {
	message->putUInt32(userIdPickingUp);
	message->putUInt32(typeBasedEntityId);
	addTransformationToBinaryMsg(&offset, message);
}

void UserDatabasePickUpEntityEvent::decode(NetMessage* message) {
	message->getUInt32(userIdPickingUp);
	message->getUInt32(typeBasedEntityId);
	offset = readTransformationFrom(message);
}

void UserDatabasePickUpEntityEvent::execute() {
	User* userPickingUp;
	Entity* entity;

	userPickingUp = UserDatabase::getUserById(userIdPickingUp);
	entity = WorldDatabase::getEntityWithTypeInstanceId(typeBasedEntityId);
	if (!userPickingUp || !entity) {
		printd(ERROR,
				"UserDatabasePickUpEntityEvent::execute(): either entity or user could not be retrieved!\n");
		assert(false);
		return;
	}
	userPickingUp->pickUpEntity(entity, offset);
	printd(INFO, "UserDatabasePickUpEntityEvent::execute(): picked up entity!\n");
}

UserDatabaseDropEntityEvent::UserDatabaseDropEntityEvent() :
	Event() {}

UserDatabaseDropEntityEvent::UserDatabaseDropEntityEvent(User* userDropingDown, Entity* entity) :
	Event(USER_DATABASE_ID, SYSTEM_CORE_ID, "UserDatabaseDropEntityEvent") {
	userIdDropingDown = userDropingDown->getId();
	typeBasedEntityId = entity->getTypeBasedId();
}

void UserDatabaseDropEntityEvent::encode(NetMessage* message) {
	message->putUInt32(userIdDropingDown);
	message->putUInt32(typeBasedEntityId);
}

void UserDatabaseDropEntityEvent::decode(NetMessage* message) {
	message->getUInt32(userIdDropingDown);
	message->getUInt32(typeBasedEntityId);
}

void UserDatabaseDropEntityEvent::execute() {
	User* userDropingDown;

	userDropingDown = UserDatabase::getUserById(userIdDropingDown);
	assert(userDropingDown);
	if (!userDropingDown->dropEntity(typeBasedEntityId)) {
		printd(ERROR, "UserDatabaseDropEntityEvent::execute(): failed to drop entity!\n");
	}
	printd(INFO, "UserDatabaseDropEntityEvent::execute(): droped entity!\n");
}

UserDatabaseAddUserEvent::UserDatabaseAddUserEvent(User* user) :
	Event(SYSTEM_CORE_ID, SYSTEM_CORE_ID, "UserDatabaseAddUserEvent") {
	CursorTransformationModel* cursorTransformationModel;
	this->name = user->getName();
	this->userId = user->getId();
	this->networkId = user->getNetworkId();
	this->avatarCfgFilePath = user->getAvatarConfigFile();
	cursorTransformationModel = user->getCursorTransformationModel();
	if (cursorTransformationModel)
		cursorModel = cursorTransformationModel->getName();
	else {
		printd(ERROR,
				"UserDatabaseAddUserEvent::constructor(): COULD NOT FIND CURSORMODEL FOR USER WITH ID %u!\n",
				user->getId());
		cursorModel = "";
	} // else
	cursorModelArguments = user->getCursorTransformationModelArguments();

	if (user->getUserTransformationModel())
		userTransformationModel = user->getUserTransformationModel()->getName();
	else {
		printd(ERROR,
				"UserDatabaseAddUserEvent::constructor(): Unable to find UserTransformationModel for user with ID %u!\n",
				user->getId());
		userTransformationModel = "";
	} // else
	userTransformationModelArguments = user->getUserTransformationModelArguments();
//	this->physicalToWorldScale = user->getPhysicalToWorldScale();

	this->numberOfSensors = 0;
	if (user == UserDatabase::getLocalUser()) {
		int numSensors = -1;
		if (InputInterface::isModuleLoaded("ControllerManager")) {
			ControllerManagerInterface* controllerManager = (ControllerManagerInterface*)InputInterface::getModuleByName("ControllerManager");
			if (controllerManager) {
				ControllerInterface* controller = controllerManager->getController();
				if (controller) {
					numSensors = controller->getNumberOfSensors();
				} // if
			} // if
		} // if
		if (numSensors < 0)
			printd(WARNING, "UserDatabaseAddUserEvent::constructor(): controller for local user not found! Can not open Sensor-pipes remotely!\n");
		else
			numberOfSensors = (unsigned)numSensors;
	} // if
} // constructor

// unsigned TransformationManagerAddUserEvent::getUserId()
// {
// 	return userId;
// }

UserDatabaseAddUserEvent::UserDatabaseAddUserEvent() :
	Event() {
	cursorModelArguments = NULL;
	userTransformationModelArguments = NULL;
	numberOfSensors = 0;
}

void UserDatabaseAddUserEvent::decode(NetMessage* message) {
	bool cursorModelArgumentsSet = false;
	bool userTransformationModelArgumentsSet = false;
	message->getUInt32(userId);
	networkId = NetworkInterface::readNetworkIdentificationFrom(message);
	message->getString(name);
	message->getString(avatarCfgFilePath);
	message->getString(cursorModel);
//	message->getReal32(physicalToWorldScale);
	msgFunctions::decode(cursorModelArgumentsSet, message);
	if (cursorModelArgumentsSet)
		cursorModelArguments = ArgumentVector::readFromBinaryMessage(message);
	else
		cursorModelArguments = NULL;
	msgFunctions::decode(userTransformationModel, message);
	msgFunctions::decode(userTransformationModelArgumentsSet, message);
	if (userTransformationModelArgumentsSet)
		userTransformationModelArguments = ArgumentVector::readFromBinaryMessage(message);
	else
		userTransformationModelArguments = NULL;

	msgFunctions::decode(numberOfSensors, message);
} // decode

void UserDatabaseAddUserEvent::encode(NetMessage* message) {
	bool cursorModelArgumentsSet = (cursorModelArguments != NULL);
	bool userTransformationModelArgumentsSet = (userTransformationModelArguments != NULL);
	message->putUInt32(userId);
	NetworkInterface::addNetworkIdentificationToBinaryMessage(&networkId, message);
	message->putString(name);
	message->putString(avatarCfgFilePath);
	message->putString(cursorModel);
	msgFunctions::encode(cursorModelArgumentsSet, message);
	if (cursorModelArgumentsSet)
		cursorModelArguments->addToBinaryMessage(message);
	message->putString(userTransformationModel);
	msgFunctions::encode(userTransformationModelArgumentsSet, message);
	if (userTransformationModelArgumentsSet)
		userTransformationModelArguments->addToBinaryMessage(message);

	msgFunctions::encode(numberOfSensors, message);
} // encode

void UserDatabaseAddUserEvent::execute() {
	User *remoteUser, *localUser;
	UserDatabaseAddUserEvent* event;
	UserSetupData remoteUserData;

	localUser = UserDatabase::getLocalUser();
	if (localUser->getId() == userId) {
		printd(WARNING,
				"UserDatabaseAddUserEvent(): WARNING: remote user %s has same id (%d) as localUser %s\n",
				name.c_str(), userId, localUser->getName().c_str());
		return;
	} // if

	if (UserDatabase::getUserById(userId)) {
		printd(INFO,
				"UserDatabaseAddUserEvent(): user %s with id %u already connected - ignoring!\n",
				name.c_str(), userId);
		return;
	} // if

	printd(INFO, "UserDatabaseAddUserEvent(): adding new user %s with id %u and avatarFile %s\n",
			name.c_str(), userId, avatarCfgFilePath.c_str());

	remoteUserData.name = name;
	remoteUserData.id = userId;
	// 	remoteUserData.avatarFile = "tieAvatar.xml";
	remoteUserData.avatarFile = avatarCfgFilePath;
	remoteUserData.initialCursorModel = cursorModel;
	remoteUserData.cursorModelArguments = cursorModelArguments;
	remoteUserData.initialUserTransformationModel = userTransformationModel;
	remoteUserData.userTransformationModelArguments = userTransformationModelArguments;
	remoteUserData.networkId = networkId;
//	remoteUserData.physicalToWorldScale = physicalToWorldScale;
	remoteUser = new User(&remoteUserData);
	UserDatabase::addRemoteUser(remoteUser);

	// open Navigation pipe
	TransformationManager::openPipe(NAVIGATION_MODULE_ID, TRANSFORMATION_MANAGER_ID, 0, 0, 0, 0, 0,
		true, remoteUser);

	// open Tracking pipes
	for (int i=0; i < (int)numberOfSensors; i++) {
		TransformationManager::openPipe(INPUT_INTERFACE_ID,	USER_DATABASE_ID, i, 0, 0, 0,
				0xD0000000 + i, true, remoteUser);
	} // for
	// HACK: open Cursor pipe
//	TransformationManager::openPipe(TRANSFORMATION_MANAGER_ID, TRANSFORMATION_MANAGER_ID, 2, 0, 0,
//			0, 0, true, remoteUser);

	event = new UserDatabaseAddUserEvent(localUser);

	EventManager::sendEvent(event, EventManager::EXECUTE_REMOTE);
	// 	delete event;
} // execute

std::string UserDatabaseAddUserEvent::toString() {
	std::stringstream ss(Event::toString());

	ss << "\tname = " << name << "\n\tuserId = " << userId << "\n\tavatarCfgFilePath = "
			<< avatarCfgFilePath << std::endl;
//	ss << "\tphysicalToWorldScale = " << physicalToWorldScale << std::endl;
	return ss.str();
} // toString

UserDatabaseRemoveUserEvent::UserDatabaseRemoveUserEvent(User* user) :
	Event(SYSTEM_CORE_ID, SYSTEM_CORE_ID, "UserDatabaseRemoveUserEvent") {

	name = user->getName();
	userId = user->getId();
} // UserDatabaseRemoveUserEvent

UserDatabaseRemoveUserEvent::UserDatabaseRemoveUserEvent() :
Event(),
name(""),
userId(0)
{
}

void UserDatabaseRemoveUserEvent::encode(NetMessage* message) {
	msgFunctions::encode(name, message);
	msgFunctions::encode(userId, message);
} // encode

void UserDatabaseRemoveUserEvent::decode(NetMessage* message) {
	msgFunctions::decode(name, message);
	msgFunctions::decode(userId, message);
} // decode

void UserDatabaseRemoveUserEvent::execute() {
	User* remoteUser;

	printd(INFO, "UserDatabaseRemoveUserEvent(): removing user %s with id %u\n", name.c_str(),
			userId);

	remoteUser = UserDatabase::getUserById(userId);
	if (remoteUser) {
		TransformationManager::closeAllPipesFromUser(remoteUser);
		UserDatabase::removeRemoteUser(userId);
	} // if
} // esecute

std::string UserDatabaseRemoveUserEvent::toString() {
	std::stringstream ss(Event::toString());

	ss << "\tname = " << name << "\n\tuserId = " << userId << std::endl;
	return ss.str();
} // toString


