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

#include "WorldDatabaseEvents.h"

#include <assert.h>

// TODO: fix dirty hack for interaction-notification when entity is destroyed
#include "../ComponentInterfaces/InteractionInterface.h"
#include "../SystemCore.h"
#include "../EventManager/EventManager.h"
#include "../MessageFunctions.h"
#include "../../OutputInterface/OutputInterface.h"

WorldDatabaseSyncEvent::WorldDatabaseSyncEvent() : Event() {
}

WorldDatabaseSyncEvent::WorldDatabaseSyncEvent(unsigned srcModuleId) :
	Event(srcModuleId, TRANSFORMATION_MANAGER_ID, "WorldDatabaseSyncEvent") {
	int i, j;

	Entity* ent;
	Environment* env;
	ENTITYINFO entInf;
	ENTITYTYPEINFO entTypeInf;

	const std::vector<Environment*>& envList = WorldDatabase::getEnvironmentList();
	const std::vector<EntityType*>& entTypeList = WorldDatabase::getEntityTypeList();

	for (i = 0; i < (int)envList.size(); i++) {
		env = (envList)[i];
		const std::vector<Entity*>& entityList = env->getEntityList();
		for (j = 0; j < (int)entityList.size(); j++) {
			ent = (entityList)[j];
			entInf.typeBasedId = ent->getTypeBasedId();
			entInf.transf = ent->getEnvironmentTransformation();
			// 			entInf.currentEnv = env->getId();
			entInf.envBasedId = ent->getEnvironmentBasedId();
			entInf.currentEnv = ent->getEnvironment()->getId();
			entityInfoList.push_back(entInf);
		}
	}

	for (i = 0; i < (int)entTypeList.size(); i++) {
		entTypeInf.typeId = (entTypeList)[i]->getId();
		// 		entTypeInf.allocationPoolFreeIdx = (*entTypeList)[i]->getIdAllocationPoolFreeIdx();
		entityTypeInfoList.push_back(entTypeInf);
	}
}

void WorldDatabaseSyncEvent::encode(NetMessage* message) {
	int i;

	message->putUInt32(entityTypeInfoList.size());
	for (i = 0; i < (int)entityTypeInfoList.size(); i++) {
		message->putUInt32(entityTypeInfoList[i].typeId);
		// 		ret->putUInt32(entityTypeInfoList[i].allocationPoolFreeIdx);
	}

	message->putUInt32(entityInfoList.size());
	for (i = 0; i < (int)entityInfoList.size(); i++) {
		message->putUInt32(entityInfoList[i].typeBasedId);
		message->putUInt32(entityInfoList[i].envBasedId);
		message->putUInt32(entityInfoList[i].currentEnv);
		addTransformationToBinaryMsg(&entityInfoList[i].transf, message);
		// 		printd("WorldDatabaseSyncEvent::encode(): transf of entity %u\n", entityInfoList[i].typeBasedId);
		// 		dumpTransformation(entityInfoList[i].transf);
	}
}

void WorldDatabaseSyncEvent::decode(NetMessage* message) {
	uint32_t i, maxi;
	ENTITYINFO entInf;
	ENTITYTYPEINFO entTypeInf;

	message->getUInt32(maxi);
	for (i = 0; i < maxi; i++) {
		message->getUInt32(entTypeInf.typeId);
		// 		msg->getUInt32(entTypeInf.allocationPoolFreeIdx);
		entityTypeInfoList.push_back(entTypeInf);
	}

	message->getUInt32(maxi);
	for (i = 0; i < maxi; i++) {
		message->getUInt32(entInf.typeBasedId);
		message->getUInt32(entInf.envBasedId);
		message->getUInt32(entInf.currentEnv);
		entInf.transf = readTransformationFrom(message);
		entityInfoList.push_back(entInf);
	}
}

void WorldDatabaseSyncEvent::execute() {
	int i;
	Entity* ent;
	EntityType* entType;
	ENTITYINFO entInf;
	unsigned short entTypeId, entEnvId;
	unsigned short entInstId, entInEnvId;
	Environment* env;

	unsigned entityId, entityEnvironmentId;
	const std::vector<Environment*> environmentList = WorldDatabase::getEnvironmentList();
	std::vector<Environment*>::const_iterator envIt;
	std::vector<Entity*>::const_iterator entIt;
	std::vector<Entity*> entitiesToRemove;

	printd("WorldDatabaseSyncEvent::execute(): im running!!!\n");

	for (envIt = environmentList.begin(); envIt != environmentList.end(); ++envIt) {
		const std::vector<Entity*> entityList = (*envIt)->getEntityList();
		for (entIt = entityList.begin(); entIt != entityList.end(); ++entIt) {
			entityId = (*entIt)->getTypeBasedId();
			entityEnvironmentId = (*entIt)->getEnvironmentBasedId();
			for (i = 0; i < (int)entityInfoList.size(); i++) {
				if (entityInfoList[i].typeBasedId == entityId && entityInfoList[i].envBasedId
						== entityEnvironmentId)
					break;
			} // for
			if (i == (int)entityInfoList.size()) {
				entitiesToRemove.push_back(*entIt);
			} // if
		} // for
		for (i = 0; i < (int)entitiesToRemove.size(); i++) {
			split(entitiesToRemove[i]->getEnvironmentBasedId(), entEnvId, entInEnvId);
			printd(
					"WorldDatabaseSyncEvent::execute(): deleting removed entity with ID (env/ent) %u %u",
					entEnvId, entInEnvId);
			(*envIt)->removeEntity(entitiesToRemove[i]);
			delete (entitiesToRemove[i]);
		} // for
		entitiesToRemove.clear();
	} // for

	for (i = 0; i < (int)entityInfoList.size(); i++) {
		entInf = entityInfoList[i];
		printd("WorldDatabaseSyncEvent::execute(): syncing entity %u\n", entInf.typeBasedId);
		ent = WorldDatabase::getEntityWithTypeInstanceId(entInf.typeBasedId);
		split(entInf.typeBasedId, entTypeId, entInstId);
		split(entInf.envBasedId, entEnvId, entInEnvId);
		if (!ent) {
			// 			entType = WorldDatabase::getEntityTypeWithId(
			printd(
					"WorldDatabaseSyncEvent::execute(): entity of type id %u and instance id %u does not exist!\n",
					entTypeId, entInstId);
			entType = WorldDatabase::getEntityTypeWithId(entTypeId);
			if (!entType) {
				printd(
						ERROR,
						"WorldDatabaseSyncEvent::execute(): entity creation failed, couldn't find entity type with id %u\n",
						entTypeId);
			} else {
				env = WorldDatabase::getEnvironmentWithId(entInf.currentEnv);
				assert(env);
				ent = entType->createInstanceUnchecked(entInstId, entEnvId, entInEnvId);
				if (!env->addNewEntity(ent))
					printd(
							ERROR,
							"WorldDatabaseSyncEvent::execute(): Could not add Entity with ID %u to Environment %u\n",
							entInEnvId, entEnvId);
				ent->setEnvironmentTransformation(entInf.transf);
			}
		} else {

			printd("WorldDatabaseSyncEvent::execute(): updating entity %u\n", entInf.typeBasedId);
			// 			dumpTransformation(entInf.transf);
			if (entInf.currentEnv != ent->getEnvironment()->getId()) {
				env = WorldDatabase::getEnvironmentWithId(entInf.currentEnv);
				ent->changeEnvironment(env);
			}
			ent->setEnvironmentTransformation(entInf.transf);
		}
	}
}


WorldDatabaseCreateEntityEvent::WorldDatabaseCreateEntityEvent(unsigned typeBasedId,
		unsigned environmentBasedId, TransformationData initialTrans, unsigned srcModuleId) :
	Event(srcModuleId, TRANSFORMATION_MANAGER_ID, "WorldDatabaseCreateEntityEvent") {
	this->initialTransf = initialTrans;
	this->entTypBasedId = typeBasedId;
	this->envBasedId = environmentBasedId;
}

WorldDatabaseCreateEntityEvent::WorldDatabaseCreateEntityEvent() :	Event() {

}

WorldDatabaseCreateEntityEvent::WorldDatabaseCreateEntityEvent(unsigned srcModuleId,
		unsigned dstModuleId, std::string eventName) :
	Event(srcModuleId, dstModuleId, eventName) {

} // WorldDatabaseCreateEntityEvent

WorldDatabaseCreateEntityEvent::~WorldDatabaseCreateEntityEvent() {

}

void WorldDatabaseCreateEntityEvent::encode(NetMessage* message) {
	addTransformationToBinaryMsg(&initialTransf, message);
	message->putUInt32(entTypBasedId);
	message->putUInt32(envBasedId);
}

void WorldDatabaseCreateEntityEvent::decode(NetMessage* message) {
	initialTransf = readTransformationFrom(message);
	message->getUInt32(entTypBasedId);
	message->getUInt32(envBasedId);
}

void WorldDatabaseCreateEntityEvent::execute() {
	Environment* env;
	EntityType* entType;
	Entity* entity;
	unsigned short envId;
	unsigned short entInEnvId;
	unsigned short entTypeId;
	unsigned short entInstId;

	split(envBasedId, envId, entInEnvId);
	split(entTypBasedId, entTypeId, entInstId);

	env = WorldDatabase::getEnvironmentWithId(envId);
	if (!env) {
		printd(ERROR,
				"WorldDatabaseCreateEntityEvent::execute(): unable to get environment for env %u ent id %u\n",
				(unsigned)envId, (unsigned)entInEnvId);
		return;
	}

	entType = WorldDatabase::getEntityTypeWithId(entTypeId);
	if (!entType) {
		printd(ERROR, "WorldDatabaseCreateEntityEvent::execute(): invalid entity type\n");
		return;
	}

	entity = entType->createInstanceUnchecked(entInstId, envId, entInEnvId);
	if (!entity) {
		printd(ERROR,
				"WorldDatabaseCreateEntityEvent::execute(): failed to create entity with instance id %u\n",
				entTypBasedId);
		return;
	}
	entity->setEnvironmentTransformation(initialTransf);

	if (!env->addNewEntity(entity)) {
		printd("WorldDatabaseCreateEntityEvent::execute(): failed to add entity to environment\n");
		return;
	} // if
} // execute


Entity* WorldDatabaseCreateEntityEvent::createEntityTypeInstance(EntityType* type,
		unsigned short instId, unsigned short idOfEnv, unsigned short idInEnv) {

	assert(type);
	return type->createInstanceUnchecked(instId, idOfEnv, idInEnv);
} // createEntityTypeInstance

bool WorldDatabaseCreateEntityEvent::addNewEntityToEnvironment(Environment* environment,
		Entity* entity) {

	assert(environment);
	assert(entity);
	return environment->addNewEntity(entity);
} // addNewEntityToEnvironment


WorldDatabaseDestroyEntityEvent::WorldDatabaseDestroyEntityEvent(Entity* ent, unsigned srcModuleId) :
	Event(srcModuleId, TRANSFORMATION_MANAGER_ID, "WorldDatabaseDestroyEntityEvent") {
	entTypBasedId = ent->getTypeBasedId();
	envBasedId = ent->getEnvironmentBasedId();
}

WorldDatabaseDestroyEntityEvent::WorldDatabaseDestroyEntityEvent() {

}

void WorldDatabaseDestroyEntityEvent::encode(NetMessage* message) {
	message->putUInt32(entTypBasedId);
	message->putUInt32(envBasedId);
}

void WorldDatabaseDestroyEntityEvent::decode(NetMessage* message) {
	message->getUInt32(entTypBasedId);
	message->getUInt32(envBasedId);
}

void WorldDatabaseDestroyEntityEvent::execute() {
	Environment* environment;
	Entity* entity;
	unsigned short environmentId;
	unsigned short entityId;

	split(envBasedId, environmentId, entityId);

	environment = WorldDatabase::getEnvironmentWithId(environmentId);
	if (!environment) {
		printd(
				ERROR,
				"WorldDatabaseDestroyEntityEvent::execute(): unable to get environment with ID %u\n",
				environmentId);
		return;
	} // if
	entity = environment->getEntityByEnvironmentBasedId(envBasedId);
	if (!entity) {
		printd(
				ERROR,
				"WorldDatabaseDestroyEntityEvent::execute(): could not find entity with EnvironmentBasedId %u/%u\n",
				environmentId, entityId);
		return;
	} // if

	//TODO: DIRTY HACK TO GET INTERACTION RUNNING!!!
	InteractionInterface* interaction =
			dynamic_cast<InteractionInterface*> (SystemCore::getModuleByName("Interaction"));
	//	Interaction* interaction = dynamic_cast<Interaction*>(SystemCore::getModuleByName("Interaction"));
	if (interaction)
		interaction->destroyEntityNotification(entity);

	environment = entity->getEnvironment();
	if (!environment->removeEntity(entity)) {
		printd(ERROR, "WorldDatabaseDestroyEntityEvent::execute(): failed to remove entity!\n");
		return;
	} // if
	delete entity;
} // execute


WorldDatabaseReloadEnvironmentsEvent::WorldDatabaseReloadEnvironmentsEvent(unsigned srcModuleId) :
	Event(srcModuleId, TRANSFORMATION_MANAGER_ID, "WorldDatabaseReloadEnvironmentsEvent") {
}

WorldDatabaseReloadEnvironmentsEvent::WorldDatabaseReloadEnvironmentsEvent() :
	Event() {
}

WorldDatabaseReloadEnvironmentsEvent::~WorldDatabaseReloadEnvironmentsEvent() {
}

void WorldDatabaseReloadEnvironmentsEvent::encode(NetMessage* message) {
}

void WorldDatabaseReloadEnvironmentsEvent::decode(NetMessage* message) {
}

void WorldDatabaseReloadEnvironmentsEvent::execute() {
	WorldDatabase::executeEnvironmentReload();
} // execute


WorldDatabaseCreateEnvironmentEvent::WorldDatabaseCreateEnvironmentEvent(unsigned short id,
		int xPosition, int zPosition, int xSize, int zSize, unsigned srcModuleId)
: Event(srcModuleId, TRANSFORMATION_MANAGER_ID, "WorldDatabaseCreateEnvironmentEvent")
{
	printd("WorldDatabaseCreateEnvironmentEvent::constructor\n");
	this->id = id;
	this->xPosition = xPosition;
	this->zPosition = zPosition;
	this->xSize = xSize;
	this->zSize = zSize;
} // WorldDatabaseCreateEnvironmentEvent

WorldDatabaseCreateEnvironmentEvent::WorldDatabaseCreateEnvironmentEvent()
: Event()
{
	printd("WorldDatabaseCreateEnvironmentEvent::constructor\n");
}

WorldDatabaseCreateEnvironmentEvent::~WorldDatabaseCreateEnvironmentEvent()
{}

void WorldDatabaseCreateEnvironmentEvent::encode(NetMessage* message)
{
	printd("WorldDatabaseCreateEnvironmentEvent::encode\n");
	msgFunctions::encode(id, message);
	msgFunctions::encode(xPosition, message);
	msgFunctions::encode(zPosition, message);
	msgFunctions::encode(xSize, message);
	msgFunctions::encode(zSize, message);
}

void WorldDatabaseCreateEnvironmentEvent::decode(NetMessage* message)
{
	printd("WorldDatabaseCreateEnvironmentEvent::decode\n");
	msgFunctions::decode(id, message);
	msgFunctions::decode(xPosition, message);
	msgFunctions::decode(zPosition, message);
	msgFunctions::decode(xSize, message);
	msgFunctions::decode(zSize, message);
}

void WorldDatabaseCreateEnvironmentEvent::execute()
{
	printd("WorldDatabaseCreateEnvironmentEvent::execute\n");
	Environment* environment = new Environment(WorldDatabase::getXSpacing(), WorldDatabase::getZSpacing(), xSize, zSize, id);
	SceneGraphInterface* sgIF = OutputInterface::getSceneGraphInterface();
	if (sgIF) {
		sgIF->attachEnvironment(environment);
	}
	environment->setXPosition(xPosition);
	environment->setZPosition(zPosition);
	WorldDatabase::addNewEnvironment(environment);
}
