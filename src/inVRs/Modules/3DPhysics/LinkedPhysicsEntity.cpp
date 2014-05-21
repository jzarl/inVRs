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
 * LinkedPhysicsEntity.cpp
 *
 *  Created on: Dec 16, 2008
 *      Author: rlander
 */

#include "LinkedPhysicsEntity.h"

#include <assert.h>

#include <inVRs/SystemCore/SystemCore.h>
#include "Physics.h"
#include "SimplePhysicsEntity.h"

const unsigned LinkedPhysicsEntity::LINKEDPHYSICSENTITY_CLASSID = 4;

LinkedPhysicsEntity::LinkedPhysicsEntity(unsigned short id, unsigned short environmentId, unsigned short instanceId, EntityType *type) :
	PhysicsEntity(id, environmentId, instanceId, type) {

	addedToSimulation = false;
	isInitialized = false;
	initializedJoints = false;

	// typeBasedId is set in Entity-Constructor which is called from
	// the PhysicsEntity-Constructor
	physicsObjectID = PhysicsObjectID(LINKEDPHYSICSENTITY_CLASSID, typeBasedId);
} // LinkedPhysicsEntity

LinkedPhysicsEntity::~LinkedPhysicsEntity() {
	// TODO Auto-generated destructor stub
}

//************************************************//
// PUBLIC METHODS INNHERITED FROM: PhysicsEntity: //
//************************************************//

void LinkedPhysicsEntity::getRigidBodies(std::vector<oops::RigidBody*>& rigidBodyList) {
	// No need to synchronize RigidBodies here since this is done for each SimplePhysicsEntity separately
} // getRigidBodies

void LinkedPhysicsEntity::getJoints(std::vector<oops::Joint*>& jointList) {

	int i = jointList.size();
	printd(INFO, "LinkedPhysicsEntity::getJoints(): found %i joints to add!\n", i);
	std::vector<oops::Joint*>::iterator it;

	for (it = this->jointList.begin(); it != this->jointList.end(); ++it) {
		jointList.push_back(*it);
	} // for
} // getJoints

PhysicsObjectID LinkedPhysicsEntity::getPhysicsObjectID() {
	return physicsObjectID;
} // getPhysicsObjectID

//*****************************************//
// PUBLIC METHODS INNHERITED FROM: Entity: //
//*****************************************//

void LinkedPhysicsEntity::setEnvironment(Environment* env) {
	Entity::setEnvironment(env);
	if (!addedToSimulation) {
		initializeJoints();
		Physics* physicsModule = (Physics*)SystemCore::getModuleByName("Physics");
		assert(physicsModule);
		physicsModule->addPhysicsObject(this);
		addedToSimulation = true;
	} // if
} // setEnvironment

void LinkedPhysicsEntity::addJoint(oops::Joint* joint, int entityIdx1, int entityIdx2) {

	assert(!initializedJoints);
	JointWrapper wrapper;
	wrapper.joint = joint;
	wrapper.subEntityIndex1 = entityIdx1;
	wrapper.subEntityIndex2 = entityIdx2;
	jointDefinitionList.push_back(wrapper);
} // addJoint

void LinkedPhysicsEntity::initializeJoints() {

	assert(!initializedJoints);
	std::vector<JointWrapper>::iterator it;

	for (it = jointDefinitionList.begin(); it != jointDefinitionList.end(); ++it) {

		if (it->subEntityIndex1 < 0 || it->subEntityIndex1 >= (int)subEntityList.size() ||
				it->subEntityIndex2 >= (int)subEntityList.size()) {
			printd(ERROR,
					"LinkedPhysicsEntity::initializeJoints(): entity Index %i or %i out of range: Maximum Index = %i!\n",
					it->subEntityIndex1, it->subEntityIndex2, subEntityList.size() - 1);
			delete it->joint;
			continue;
		} // if

		SimplePhysicsEntity* subEntity1 = subEntityList.at(it->subEntityIndex1);
		SimplePhysicsEntity* subEntity2 = NULL;
		if (it->subEntityIndex2 >= 0) {
			subEntity2 = subEntityList.at(it->subEntityIndex2);
		} // if

		it->joint->setObject1(subEntity1->getRigidBody());
		if (subEntity2) {
			it->joint->setObject2(subEntity2->getRigidBody());
		} // if

		jointList.push_back(it->joint);
	} // if

	jointDefinitionList.clear();
	initializedJoints = true;
} // addJoint

void LinkedPhysicsEntity::setSubEntities(std::vector<Entity*> subEntityList) {

	std::vector<Entity*>::iterator it;
	SimplePhysicsEntity* subEntity;

	for (it = subEntityList.begin(); it != subEntityList.end(); ++it) {
		subEntity = dynamic_cast<SimplePhysicsEntity*>(*it);
		assert(subEntity);
		this->subEntityList.push_back(subEntity);
	} // for
} // setSubEntities

