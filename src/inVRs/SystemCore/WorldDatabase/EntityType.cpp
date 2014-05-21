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

#include "EntityType.h"

#include <assert.h>
#include <sstream>

#include "Entity.h"
#include "WorldDatabaseEvents.h"
#include "../DebugOutput.h"
#include "../IdPoolManager.h"
#include "../SystemCore.h"
#include "../../OutputInterface/SceneGraphInterface.h"

EntityType::EntityType(unsigned short id) :
	globalEntityIdPool(0, 0xFFFF) {
	this->id = id;
	this->name = "";
	this->fixed = false;

	localEntityIdPool = NULL;
	loadTimeEntityIdPool = globalEntityIdPool.allocSubPool(0, 4096);

	if (!loadTimeEntityIdPool) {
		printd(ERROR, "EntityType::EntityType(): failed to allocate load time id pool \n");
	}

	modelTemplate = NULL;
} // EntityType

EntityType::~EntityType() {
	printd(
			INFO,
			"EntityType::~EntityType(): destructor: deleting entities of EntityType with ID %u ...\n",
			id);
	clearInstances();
	printd(
			INFO,
			"EntityType::~EntityType(): destructor: deleting entities of EntityType with ID %u ... done\n",
			id);
} // ~EntityType

void EntityType::dump() {
	printd(INFO, "EntityType::dump(): ID: %u\tNAME: %s\n", this->id, this->name.c_str());
} // dump

void EntityType::setName(std::string entName) {
	// register globalEntityIdPool on first call
	if (name.size() == 0) {
		std::ostringstream o;
		o << id;
		nameGlobalEntityIdPool = entName + "_globalEntityIdPool" + "_" + o.str();
		localIdPoolManager.registerIdPool(nameGlobalEntityIdPool, &globalEntityIdPool);
		printd(INFO, "EntityType::setName(): registered id pool %s\n",
				nameGlobalEntityIdPool.c_str());
	}
	name = entName;
} // setName

void EntityType::setFixed(bool fixed) {
	this->fixed = fixed;
} // setFixed

void EntityType::setModel(ModelInterface* model) {
	this->modelTemplate = model;
} // setModel

void EntityType::setXmlFilename(std::string filename) {
	this->xmlFilename = filename;
} // setXmlFile

unsigned short EntityType::getId() {
	return id;
} // getId

std::string EntityType::getName() {
	return name;
} // getName

unsigned EntityType::getNumberOfInstances() {
	return instances.size();
} // getNumberOfInstances

bool EntityType::isFixed() {
	return fixed;
} // isFixed

Entity* EntityType::getEntityByInstanceId(unsigned short instanceId) {
	int i;
	unsigned resultId = join(id, instanceId);
	for (i = 0; i < (int)instances.size(); i++) {
		if (instances[i]->getTypeBasedId() == resultId)
			return instances[i];
	} // for
	return NULL;
} // getEntityByInstanceId

Entity* EntityType::getEntityByEnvironmentBasedId(unsigned envBasedId) {
	int i;
	for (i = 0; i < (int)instances.size(); i++) {
		if (instances[i]->getEnvironmentBasedId() == envBasedId)
			return instances[i];
	} // for
	return NULL;
} // getEntityByEnvironmentBasedId

ModelInterface* EntityType::getModel() {
	return modelTemplate;
} // getModel

std::string EntityType::getXmlFilename(){
	return xmlFilename;
} // getXmlFile

const std::vector<Entity*>& EntityType::getInstanceList() {
	return instances;
}

Entity* EntityType::createInstance(unsigned short idOfEnv, unsigned short idInEnv) {
	unsigned short entId;

	entId = getFreeInstanceId();
	return createInstanceUnchecked(entId, idOfEnv, idInEnv);
} // createInstance

Entity* EntityType::createInstanceAtLoadTime(unsigned short idOfEnv, unsigned short idInEnv) {
	unsigned short entId;

	assert(loadTimeEntityIdPool);
	entId = (unsigned short)loadTimeEntityIdPool->allocEntry();
	return createInstanceUnchecked(entId, idOfEnv, idInEnv);
} // createInstanceAtLoadTime

void EntityType::removeInstance(Entity* entity) {
	// remove Entity of EntityTypeList
	std::vector<Entity*>::iterator it;
	bool entityRemoved = false;

	for (it = instances.begin(); it != instances.end(); ++it) {
		if (*it == entity) {
			instances.erase(it);
			if (localEntityIdPool != NULL &&
					entity->getInstanceId() >= localEntityIdPool->getMinIdx() &&
					entity->getInstanceId() <= localEntityIdPool->getMaxIdx()) {
				localEntityIdPool->freeEntry(entity->getInstanceId());
			} // if
			entityRemoved = true;
			break;
		} // if
	} // for

	if (!entityRemoved)
		printd(ERROR, "EntityType::removeInstance(): Could not delete Entity from instance-list!\n");
} // removeInstance

Entity* EntityType::createInstanceUnchecked(unsigned short instId, unsigned short idOfEnv,
		unsigned short idInEnv) {
	Entity* ret = new Entity(idInEnv, idOfEnv, instId, this);
	instances.push_back(ret);
	return ret;
} // createInstanceUnchecked

void EntityType::clearInstances() {
	int i;

	for (i = 0; i < (int)instances.size(); i++)
		delete instances[i];
	instances.clear();
} // clearInstances

Event* EntityType::createEntityCreationEvent(Environment* env, unsigned environmentBasedId,
		TransformationData trans) {

	//	unsigned short environmentId = env->getId();
	//	unsigned short idInEnvironment = getFreeIdInEnvironment(env);
	//	unsigned environmentBasedId = join(environmentId, idInEnvironment);

	unsigned short instanceId = getFreeInstanceId();
	unsigned typeBasedId = join(this->id, instanceId);

	Event* event = new WorldDatabaseCreateEntityEvent(typeBasedId, environmentBasedId, trans,
			WORLD_DATABASE_ID);

	return event;
} // createEntityCreationEvent

void EntityType::setModelTemplate(ModelInterface* modelTemplate) {
	this->modelTemplate = modelTemplate;
} // setModelTemplate

void EntityType::setLocalEntityIdPool(IdPool* newlocalPool) {
	localEntityIdPool = newlocalPool;
} // setLocalEntityIdPool

std::string EntityType::getGlobalEntityIdPoolName() {
	return nameGlobalEntityIdPool;
} // getGlobalEntityIdPoolName

unsigned short EntityType::getFreeInstanceId() {
	if (localEntityIdPool == NULL) {
		printd(INFO, "EntityType::getFreeInstanceId(): creating pool with 4096 entries ...\n");
		localEntityIdPool = globalEntityIdPool.allocSubPool(4096);
		assert(localEntityIdPool);
	} // if
	return (unsigned short)localEntityIdPool->allocEntry();
} // getFreeInstanceId

unsigned short EntityType::getFreeIdInEnvironment(Environment* env) {
	assert(env);
	return env->getFreeEntityId();
} // getFreeIdInEnvironment
