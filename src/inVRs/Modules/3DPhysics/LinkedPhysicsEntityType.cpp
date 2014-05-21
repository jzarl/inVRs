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
 * LinkedPhysicsEntityType.cpp
 *
 *  Created on: Dec 16, 2008
 *      Author: rlander
 */
#include "LinkedPhysicsEntityType.h"

#include <assert.h>

#include "LinkedPhysicsEntity.h"
#include "SimplePhysicsEntityType.h"
#include "CreateLinkedPhysicsEntityEvent.h"
#include "OopsObjectID.h"
#include <inVRs/SystemCore/DebugOutput.h>
#include <inVRs/SystemCore/WorldDatabase/WorldDatabase.h>
#include <inVRs/SystemCore/UserDatabase/UserDatabase.h>

LinkedPhysicsEntityType::LinkedPhysicsEntityType(unsigned short id) :
	EntityType(id) {

} // LinkedPhysicsEntityType

LinkedPhysicsEntityType::~LinkedPhysicsEntityType() {

	// TODO: free Joints!!!

} // ~LinkedPhysicsEntityType

void LinkedPhysicsEntityType::addSubEntity(SimplePhysicsEntityType* entityType,
		const TransformationData& offset) {
	SubEntityType subEntityType;
	subEntityType.type = entityType;
	subEntityType.offset = offset;
	WorldDatabase::getPrivateAccessor().addEntityType(entityType);
	subEntityList.push_back(subEntityType);
} // addSubEntity

void LinkedPhysicsEntityType::addJoint(oops::Joint* joint, int entityIndex1, int entityIndex2) {
	JointMapping mapping;
	mapping.joint = joint;
	mapping.entityIndex1 = entityIndex1;
	mapping.entityIndex2 = entityIndex2;
	jointList.push_back(mapping);
} // addJoint

Entity* LinkedPhysicsEntityType::createInstanceAtLoadTime(unsigned short idOfEnv,
		unsigned short idInEnv) {
	//TODO: implement!!!
	assert(false);
	return NULL;
} // createInstanceAtLoadTime

Entity* LinkedPhysicsEntityType::createInstanceUnchecked(unsigned short instId,
		unsigned short idOfEnv, unsigned short idInEnv) {

	printd(INFO,
			"LinkedPhysicsEntityType::createInstanceUnchecked(): building new PhysicsEntity!\n");
	LinkedPhysicsEntity* ret = new LinkedPhysicsEntity(idInEnv, idOfEnv, instId, this);

	std::vector<JointMapping>::iterator jointIt;
	oops::Joint* joint;

	int index = 0;
	for (jointIt = jointList.begin(); jointIt != jointList.end(); ++jointIt) {
		JointMapping& mapping = *jointIt;
		joint = mapping.joint->clone();
		OopsObjectID jointId = OopsObjectID(true,
				LinkedPhysicsEntity::LINKEDPHYSICSENTITY_CLASSID, index, ret->getTypeBasedId());
		joint->setID(jointId.get());
		ret->addJoint(joint, mapping.entityIndex1, mapping.entityIndex2);
		index++;
	} // for

	instances.push_back(ret);

	printd(INFO, "LinkedPhysicsEntityType::createInstanceUnchecked(): DONE!\n");
	return ret;
} // createInstanceUnchecked

Event* LinkedPhysicsEntityType::createEntityCreationEvent(Environment* env,
		unsigned linkedEnvironmentBasedId, TransformationData trans) {

	CreateLinkedPhysicsEntityEvent::EntityCreationData entityCreationData;
	std::vector<CreateLinkedPhysicsEntityEvent::EntityCreationData> entityDataList;

	unsigned short environmentId = env->getId();
	unsigned short idInEnvironment;
	unsigned environmentBasedId;
	unsigned short entityTypeId;
	unsigned short instanceId;
	unsigned typeBasedId;

	// Create IDs for all Sub-Entities
	SubEntityType subEntityType;
	std::vector<SubEntityType>::iterator it;
	for (it = subEntityList.begin(); it != subEntityList.end(); ++it) {
		subEntityType = *it;

		idInEnvironment = getFreeIdInEnvironment(env);
		environmentBasedId = join(environmentId, idInEnvironment);
		entityTypeId = subEntityType.type->getId();
		instanceId = subEntityType.type->getFreeInstanceId();
		typeBasedId = join(entityTypeId, instanceId);

		entityCreationData.environmentBasedId = environmentBasedId;
		entityCreationData.typeBasedId = typeBasedId;
		entityCreationData.offset = subEntityType.offset;
		entityDataList.push_back(entityCreationData);
	} // for

	// Create IDs for LinkedPhysicsEntity itself
//	idInEnvironment = getFreeIdInEnvironment(env);
//	environmentBasedId = join(environmentId, idInEnvironment);
	entityTypeId = this->id;
	instanceId = getFreeInstanceId();
	typeBasedId = join(entityTypeId, instanceId);

	entityCreationData.environmentBasedId = linkedEnvironmentBasedId;
	entityCreationData.typeBasedId = typeBasedId;
	entityCreationData.offset = identityTransformation();

	CreateLinkedPhysicsEntityEvent* event = new CreateLinkedPhysicsEntityEvent(entityCreationData,
			entityDataList, trans, WORLD_DATABASE_ID);

	return event;
} // createEntityCreationEvent

const std::vector<LinkedPhysicsEntityType::SubEntityType>& LinkedPhysicsEntityType::getSubEntityTypes() {
	return subEntityList;
} // getSubEntityTypes
