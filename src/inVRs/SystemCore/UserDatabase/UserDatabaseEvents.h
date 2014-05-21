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

#ifndef _USERDATABASEEVENTS_H
#define _USERDATABASEEVENTS_H

#include "../DataTypes.h"
#include "../EventManager/Event.h"
#include "../EventManager/EventFactory.h"
#include "../WorldDatabase/WorldDatabase.h"
#include "../ComponentInterfaces/NetworkInterface.h"

/******************************************************************************
 * @class UserDatabasePickUpEntityEvent
 * @brief An event which is triggered once an entity is picked up.
 *
 */
class INVRS_SYSTEMCORE_API UserDatabasePickUpEntityEvent : public Event {
public:
	UserDatabasePickUpEntityEvent();
	UserDatabasePickUpEntityEvent(User* userPickingUp, Entity* entity, TransformationData offset);

	typedef EventFactory<UserDatabasePickUpEntityEvent> Factory;

	virtual void encode(NetMessage* message);
	virtual void decode(NetMessage* message);
	virtual void execute();

protected:
	unsigned userIdPickingUp;
	unsigned typeBasedEntityId;
	TransformationData offset;
};

/******************************************************************************
 * @class UserDatabaseDropEntityEvent
 * @brief An event which is triggered once an entity is dropped.
 *
 */
class INVRS_SYSTEMCORE_API UserDatabaseDropEntityEvent : public Event {
public:
	UserDatabaseDropEntityEvent();
	UserDatabaseDropEntityEvent(User* userDropingDown, Entity* entity);

	typedef EventFactory<UserDatabaseDropEntityEvent> Factory;

	virtual void encode(NetMessage* message);
	virtual void decode(NetMessage* message);
	virtual void execute();

protected:
	unsigned userIdDropingDown;
	unsigned typeBasedEntityId;
};

/******************************************************************************
 * @class UserDatabaseAddUserEvent
 * @brief An event which is triggered once a user is added to the UserDatabase.
 *
 */
class INVRS_SYSTEMCORE_API UserDatabaseAddUserEvent : public Event {
public:
	UserDatabaseAddUserEvent(User* user);
	UserDatabaseAddUserEvent();

	typedef EventFactory<UserDatabaseAddUserEvent> Factory;

	virtual void encode(NetMessage* message);
	virtual void decode(NetMessage* message);
	virtual void execute();
	virtual std::string toString();
	// 	unsigned getUserId(); // returns id of user who should be added

protected:
	std::string name;
	unsigned userId;
	NetworkIdentification networkId;
	std::string avatarCfgFilePath;
	std::string cursorModel;
	ArgumentVector* cursorModelArguments;
	std::string userTransformationModel;
	ArgumentVector* userTransformationModelArguments;
	unsigned numberOfSensors;
//	float physicalToWorldScale;
};

/******************************************************************************
 * @class UserDatabaseRemoveUserEvent
 * @brief An event which is triggered once a user is removed from the
 * UserDatabase.
 *
 */
class INVRS_SYSTEMCORE_API UserDatabaseRemoveUserEvent : public Event {
public:
	UserDatabaseRemoveUserEvent(User* user);
	UserDatabaseRemoveUserEvent();

	typedef EventFactory<UserDatabaseRemoveUserEvent> Factory;

	virtual void encode(NetMessage* message);
	virtual void decode(NetMessage* message);
	virtual void execute();
	virtual std::string toString();

private:
	std::string name;
	unsigned userId;
};

#endif
