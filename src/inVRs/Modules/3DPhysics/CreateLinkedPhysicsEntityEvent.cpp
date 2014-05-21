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

/*
 * CreateLinkedPhysicsEntityEvent.cpp
 *
 *  Created on: Dec 17, 2008
 *      Author: rlander
 */

#include <assert.h>

#include "CreateLinkedPhysicsEntityEvent.h"
#include "LinkedPhysicsEntityType.h"
#include "LinkedPhysicsEntity.h"

#include <inVRs/SystemCore/WorldDatabase/WorldDatabase.h>
#include <inVRs/SystemCore/EventManager/Event.h>
#include <inVRs/SystemCore/MessageFunctions.h>

#include <OpenSG/OSGThreadManager.h>

OSG_USING_NAMESPACE

/*******************/
/* PUBLIC METHODS: */
/*******************/

CreateLinkedPhysicsEntityEvent::CreateLinkedPhysicsEntityEvent(EntityCreationData linkedEntityData,
		std::vector<EntityCreationData>& subEntityData, TransformationData initialTrans,
		unsigned srcModuleId) :
	WorldDatabaseCreateEntityEvent(srcModuleId, TRANSFORMATION_MANAGER_ID,
			"CreateLinkedPhysicsEntityEvent") {

	this->initialTrans = initialTrans;
	this->linkedEntityData = linkedEntityData;
	std::vector<EntityCreationData>::iterator it;
	for (it = subEntityData.begin(); it != subEntityData.end(); ++it) {
		this->subEntityData.push_back(*it);
	} // for
} // CreateLinkedPhysicsEntityEvent

CreateLinkedPhysicsEntityEvent::~CreateLinkedPhysicsEntityEvent() {

} // ~CreateLinkedPhysicsEntityEvent

/****************************************/
/* PUBLIC METHODS INHERITED FROM: Event */
/****************************************/

void CreateLinkedPhysicsEntityEvent::encode(NetMessage* message) {
	std::vector<EntityCreationData>::iterator it;

	msgFunctions::encode(initialTrans, message);
	msgFunctions::encode(linkedEntityData.environmentBasedId, message);
	msgFunctions::encode(linkedEntityData.typeBasedId, message);

	unsigned entityListSize = subEntityData.size();
	msgFunctions::encode(entityListSize, message);
	for (it = subEntityData.begin(); it != subEntityData.end(); ++it) {
		msgFunctions::encode((*it).environmentBasedId, message);
		msgFunctions::encode((*it).typeBasedId, message);
		msgFunctions::encode((*it).offset, message);
	} // for
} // encode

void CreateLinkedPhysicsEntityEvent::decode(NetMessage* message) {
	unsigned entityListSize;

	msgFunctions::decode(initialTrans, message);
	msgFunctions::decode(linkedEntityData.environmentBasedId, message);
	msgFunctions::decode(linkedEntityData.typeBasedId, message);

	msgFunctions::decode(entityListSize, message);
	for (unsigned i = 0; i < entityListSize; i++) {
		EntityCreationData data;
		msgFunctions::decode(data.environmentBasedId, message);
		msgFunctions::decode(data.typeBasedId, message);
		msgFunctions::decode(data.offset, message);
		subEntityData.push_back(data);
	} // for
} // decode

void CreateLinkedPhysicsEntityEvent::execute() {
	Entity* entity;

	std::vector<Entity*> entityList;

	// create all SubEntities
	std::vector<EntityCreationData>::iterator it;
	for (it = subEntityData.begin(); it != subEntityData.end(); ++it) {
		EntityCreationData& entityCreationData = *it;
		entity = createEntity(entityCreationData);
		entityList.push_back(entity);
	} // for

	createEntity(linkedEntityData, &entityList);
} // execute

Entity* CreateLinkedPhysicsEntityEvent::createEntity(EntityCreationData& entityCreationData,
		std::vector<Entity*>* subEntityList) {
	unsigned short environmentId;
	unsigned short idInEnvironment;
	unsigned short entityTypeId;
	unsigned short instanceId;

	Environment* environment;
	EntityType* entityType;
	Entity* entity;
	TransformationData finalTransformation;

	split(entityCreationData.environmentBasedId, environmentId, idInEnvironment);
	split(entityCreationData.typeBasedId, entityTypeId, instanceId);

	environment = WorldDatabase::getEnvironmentWithId(environmentId);
	if (!environment) {
		printd(
				ERROR,
				"CreateLinkedPhysicsEntityEvent::execute(): unable to get environment for env %u ent id %u\n",
				environmentId, idInEnvironment);
		return NULL;
	} // if
	entityType = WorldDatabase::getEntityTypeWithId(entityTypeId);
	if (!entityType) {
		printd(
				ERROR,
				"CreateLinkedPhysicsEntityEvent::execute(): unable to get entity type with id %u\n",
				entityTypeId);
		return NULL;
	} // if
	entity = createEntityTypeInstance(entityType, instanceId, environmentId, idInEnvironment);
	if (!entity) {
		printd(
				ERROR,
				"CreateLinkedPhysicsEntityEvent::execute(): failed to create entity with instance id %u\n",
				instanceId);
		return NULL;
	} // if

	multiply(finalTransformation, entityCreationData.offset, initialTrans);
	entity->setEnvironmentTransformation(finalTransformation);

	if (subEntityList != NULL) {
		LinkedPhysicsEntity* linkedEntity = dynamic_cast<LinkedPhysicsEntity*> (entity);
		assert(linkedEntity);
		linkedEntity->setSubEntities(*subEntityList);
		std::vector<Entity*>::iterator it = (*subEntityList).begin();

#if OSG_MAJOR_VERSION >= 2
		OSG::LockRefPtr simulationStepLock;
#else
		OSG::Lock* simulationStepLock;
#endif
#if OPENSG_THREADMANAGER_GETLOCK_HAVE_BGLOBAL
		simulationStepLock = ThreadManager::the()->getLock( "Physics::simulationStepLock",false);
#else
		simulationStepLock = dynamic_cast<Lock*>(ThreadManager::the()->getLock( "Physics::simulationStepLock"));
#endif

		assert(simulationStepLock);
#if OSG_MAJOR_VERSION >= 2
		simulationStepLock->acquire();
#else
		simulationStepLock->aquire();
#endif
			while (it != (*subEntityList).end()) {
				Entity* subEntity = *it;
				if (!addNewEntityToEnvironment(environment, subEntity)) {
					printd(ERROR,
							"CreateLinkedPhysicsEntityEvent::execute(): failed to add new sub-entity to environment!\n");
				} // if
				++it;
			} // while
			if (!addNewEntityToEnvironment(environment, entity)) {
				printd(ERROR,
						"CreateLinkedPhysicsEntityEvent::execute(): failed to add new LinkedPhysicsEntity to environment!\n");
				return NULL;
			} // if
		simulationStepLock->release();
	} // if

	return entity;
} // createEntity

/********************/
/* PRIVATE METHODS: */
/********************/

CreateLinkedPhysicsEntityEvent::CreateLinkedPhysicsEntityEvent() :
	WorldDatabaseCreateEntityEvent() {

} // CreateLinkedPhysicsEntityEvent
