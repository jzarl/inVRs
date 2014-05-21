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

/*
 * CreateLinkedPhysicsEntityEvent.h
 *
 *  Created on: Dec 17, 2008
 *      Author: rlander
 */

#ifndef CREATELINKEDPHYSICSENTITYEVENT_H_
#define CREATELINKEDPHYSICSENTITYEVENT_H_

#include <inVRs/SystemCore/EventManager/EventFactory.h>
#include <inVRs/SystemCore/EventManager/Event.h>
#include <inVRs/SystemCore/DataTypes.h>
#include <inVRs/SystemCore/WorldDatabase/WorldDatabaseEvents.h>

class LinkedPhysicsEntityType;

/**
 *
 */
class CreateLinkedPhysicsEntityEvent : public WorldDatabaseCreateEntityEvent {
public:

	struct EntityCreationData {
		unsigned environmentBasedId;
		unsigned typeBasedId;
		TransformationData offset;
	}; // EntityIds

	/** Factory-class for CreateLinkedPhysicsEntityEvent
	 */
	typedef EventFactory<CreateLinkedPhysicsEntityEvent> Factory;

	/*******************/
	/* PUBLIC METHODS: */
	/*******************/

	/** Public constructor
	 */
	CreateLinkedPhysicsEntityEvent(EntityCreationData linkedEntityData,
			std::vector<EntityCreationData>& subEntityData,
			TransformationData initialTrans, unsigned srcModuleId);

	/**
	 *
	 */
	virtual ~CreateLinkedPhysicsEntityEvent();

	/****************************************/
	/* PUBLIC METHODS INHERITED FROM: Event */
	/****************************************/

	/**
	 *
	 */
	virtual void encode(NetMessage* message);

	/**
	 *
	 */
	virtual void decode(NetMessage* message);

	/**
	 *
	 */
	virtual void execute();

protected:

	/**
	 *
	 */
	Entity* createEntity(EntityCreationData& entityCreationData,
			std::vector<Entity*>* subEntityList = NULL);

private:
	friend class EventFactory<CreateLinkedPhysicsEntityEvent>;

	/********************/
	/* PRIVATE METHODS: */
	/********************/

	/** Constructor called by Factory class
	 */
	CreateLinkedPhysicsEntityEvent();

	/********************/
	/* PRIVATE MEMBERS: */
	/********************/

	///
	TransformationData initialTrans;

	///
	EntityCreationData linkedEntityData;
	///
	std::vector<EntityCreationData> subEntityData;
}; // CreateLinkedPhysicsEntityEvent

#endif /* CREATELINKEDPHYSICSENTITYEVENT_H_ */
