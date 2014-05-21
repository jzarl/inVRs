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

#include "ArticulatedPhysicsEntityType.h"

#include <assert.h>
#include <vector>

#include "ArticulatedPhysicsEntity.h"
#include "Physics.h"
#include "OopsObjectID.h"
#include <inVRs/SystemCore/DebugOutput.h>
#include <inVRs/SystemCore/SystemCore.h>

using namespace oops;

//*****************//
// PUBLIC METHODS: //
//*****************//

ArticulatedPhysicsEntityType::ArticulatedPhysicsEntityType(int entType) : EntityType(entType)
{
	this->articulatedBody = NULL;
} // ArticulatedPhysicsEntityType

ArticulatedPhysicsEntityType::~ArticulatedPhysicsEntityType()
{

} // ~ArticulatedPhysicsEntityType

//********************//
// PROTECTED METHODS: //
//********************//

void ArticulatedPhysicsEntityType::setRigidBodyMapping(unsigned rigidBodyId, unsigned succeedingEntity)
{
	rigidBodyMapping[rigidBodyId] = succeedingEntity;
} // setRigidBodyMapping

void ArticulatedPhysicsEntityType::initializeInstance(ArticulatedPhysicsEntity* entity)
{
	int i;
	unsigned rigidBodyIndex, succeedingIndex;
	std::map<unsigned,unsigned>::iterator it;
	std::vector<RigidBody*> rigidBodyList;
	std::vector<Joint*> jointList;
	OopsObjectID id;

	// TODO: remove deprecated ID-assignment!!!
	ArticulatedBody* newArticulatedBody = articulatedBody->clone();
	newArticulatedBody->setID(entity->getTypeBasedId());
	entity->setArticulatedBody(newArticulatedBody);
	instances.push_back(entity);

	for (it=rigidBodyMapping.begin(); it != rigidBodyMapping.end(); ++it)
	{
		rigidBodyIndex = (*it).first;
		succeedingIndex = (*it).second;
		entity->setRigidBodyMapping(rigidBodyIndex, succeedingIndex);
	} // for
	
	newArticulatedBody->getRigidBodies(rigidBodyList);
	newArticulatedBody->getJoints(jointList);
	for (i=0; i < (int)rigidBodyList.size(); i++) {
		id = OopsObjectID(false, ArticulatedPhysicsEntity::ARTICULATEDPHYSICSENTITY_CLASSID, i, entity->getTypeBasedId());
		rigidBodyList[i]->setID(id.get());
	} // for
	if (i == 0)
		printd(ERROR, "ArticulatedPhysicsEntity::initializeInstance(): NO RIGIDBODY FOUND FOR ARTICULATEDBODY!!!");
	for (i=0; i < (int)jointList.size(); i++) {
		id = OopsObjectID(true, ArticulatedPhysicsEntity::ARTICULATEDPHYSICSENTITY_CLASSID, i, entity->getTypeBasedId());
		jointList[i]->setID(id.get());
	} // for
} // initializeInstance

//**********************************************//
// PROTECTED METHODS INHERITED FROM: EntityType //
//**********************************************//

Entity* ArticulatedPhysicsEntityType::createInstanceAtLoadTime(unsigned short idOfEnv, unsigned short idInEnv)
{
	unsigned short entId;
	assert(loadTimeEntityIdPool);
	entId = (unsigned short)loadTimeEntityIdPool->allocEntry();
	return createInstanceUnchecked(entId, idOfEnv, idInEnv);
} // createInstanceAtLoadTime

Entity* ArticulatedPhysicsEntityType::createInstanceUnchecked(unsigned short instId, unsigned short idOfEnv, unsigned short idInEnv)
{
	printd(INFO, "ArticulatedPhysicsEntityType::createInstanceUnchecked(): building new PhysicsEntity!\n");
	ArticulatedPhysicsEntity* ret = new ArticulatedPhysicsEntity(idInEnv, idOfEnv, instId, this);
	initializeInstance(ret);
	printd(INFO, "ArticulatedPhysicsEntityType::createInstanceUnchecked(): DONE!\n");
	return ret;
} // createInstanceUnchecked
