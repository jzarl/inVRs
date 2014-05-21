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

#ifndef _WORLDDATABASEEVENTS_H
#define _WORLDDATABASEEVENTS_H

#include "WorldDatabase.h"
#include "../DataTypes.h"
#include "../EventManager/Event.h"
#include "../EventManager/EventFactory.h"

/******************************************************************************
 *
 */
class INVRS_SYSTEMCORE_API WorldDatabaseSyncEvent : public Event {
public:

	WorldDatabaseSyncEvent();
	WorldDatabaseSyncEvent(unsigned srcModuleId);

	typedef EventFactory<WorldDatabaseSyncEvent> Factory;

	virtual void encode(NetMessage* message);
	virtual void decode(NetMessage* message);
	virtual void execute();

protected:

	struct ENTITYINFO {
		unsigned typeBasedId;
		unsigned envBasedId;
		unsigned currentEnv;
		TransformationData transf;
	};

	struct ENTITYTYPEINFO {
		unsigned typeId;
		// 		unsigned allocationPoolFreeIdx;
	};

	std::vector<ENTITYINFO> entityInfoList;
	std::vector<ENTITYTYPEINFO> entityTypeInfoList;

	// 	virtual std::string toString();
};

/******************************************************************************
 *
 */
class INVRS_SYSTEMCORE_API WorldDatabaseCreateEntityEvent : public Event {
public:

	WorldDatabaseCreateEntityEvent(unsigned typeBasedId, unsigned environmentBasedId,
			TransformationData initialTrans, unsigned srcModuleId);
	virtual ~WorldDatabaseCreateEntityEvent();

	typedef EventFactory<WorldDatabaseCreateEntityEvent> Factory;

	virtual void encode(NetMessage* message);
	virtual void decode(NetMessage* message);
	virtual void execute();

protected:
	friend class EventFactory<WorldDatabaseCreateEntityEvent> ;

	WorldDatabaseCreateEntityEvent(); // constructor for factory
	WorldDatabaseCreateEntityEvent(unsigned srcModuleId, unsigned dstModuleId,
			std::string eventName); // constructor for derived classes
	Entity* createEntityTypeInstance(EntityType* type, unsigned short instId,
			unsigned short idOfEnv, unsigned short idInEnv);
	bool addNewEntityToEnvironment(Environment* environment, Entity* entity);

	TransformationData initialTransf;
	unsigned entTypBasedId;
	unsigned envBasedId;
};

/******************************************************************************
 *
 */
class INVRS_SYSTEMCORE_API WorldDatabaseDestroyEntityEvent : public Event {
public:
	WorldDatabaseDestroyEntityEvent(Entity* ent, unsigned srcModuleId); // constructor for application
	WorldDatabaseDestroyEntityEvent();

	typedef EventFactory<WorldDatabaseDestroyEntityEvent> Factory;

	virtual void encode(NetMessage* message);
	virtual void decode(NetMessage* message);
	virtual void execute();

protected:

	unsigned entTypBasedId;
	unsigned envBasedId;
};

/******************************************************************************
 *
 */
class INVRS_SYSTEMCORE_API WorldDatabaseReloadEnvironmentsEvent : public Event {
public:
	WorldDatabaseReloadEnvironmentsEvent(unsigned srcModuleId); // constructor for application
	WorldDatabaseReloadEnvironmentsEvent(); // constructor for factory
	virtual ~WorldDatabaseReloadEnvironmentsEvent();

	typedef EventFactory<WorldDatabaseReloadEnvironmentsEvent> Factory;

	virtual void encode(NetMessage* message);
	virtual void decode(NetMessage* message);
	virtual void execute();
};


class INVRS_SYSTEMCORE_API WorldDatabaseCreateEnvironmentEvent : public Event {
public:
	WorldDatabaseCreateEnvironmentEvent(unsigned short id, int xPosition, int zPosition, int xSize, int zSize, unsigned srcModuleId); // constructor for application
	WorldDatabaseCreateEnvironmentEvent(); // constructor for factory
	virtual ~WorldDatabaseCreateEnvironmentEvent();

	typedef EventFactory<WorldDatabaseCreateEnvironmentEvent> Factory;

	virtual void encode(NetMessage* message);
	virtual void decode(NetMessage* message);
	virtual void execute();

private:
	unsigned short id;
	int xSize, zSize, xPosition, zPosition;
};

//class WorldDatabaseEventsFactory : public EventFactory
//{
//public:
//	virtual Event* create(unsigned eventId);
//};

#endif
