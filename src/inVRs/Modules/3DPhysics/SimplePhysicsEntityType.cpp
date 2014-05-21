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

#include "SimplePhysicsEntityType.h"

#include <assert.h>
#include "Physics.h"
#include "SimplePhysicsEntity.h"
#include "OopsObjectID.h"
#include <inVRs/SystemCore/DebugOutput.h>
#include <inVRs/SystemCore/SystemCore.h>

using namespace oops;

//*****************//
// PUBLIC METHODS: //
//*****************//

SimplePhysicsEntityType::SimplePhysicsEntityType(unsigned short entType) : EntityType(entType)
{
	this->rigidBody = NULL;
} // SimplePhysicsEntityType

SimplePhysicsEntityType::~SimplePhysicsEntityType()
{

} // ~SimplePhysicsEntityType

RigidBody* SimplePhysicsEntityType::getRigidBody() {
	return rigidBody;
} // getRigidBody

void SimplePhysicsEntityType::setRigidBody(RigidBody* rigidBody)
{
	this->rigidBody = rigidBody;
} // setRigidBody


//**********************************************//
// PROTECTED METHODS INHERITED FROM: EntityType //
//**********************************************//

Entity* SimplePhysicsEntityType::createInstanceAtLoadTime(unsigned short idOfEnv, unsigned short idInEnv)
{
	unsigned short entId;
	assert(loadTimeEntityIdPool);
	entId = (unsigned short)loadTimeEntityIdPool->allocEntry();
	return createInstanceUnchecked(entId, idOfEnv, idInEnv);
} // createInstanceAtLoadTime

Entity* SimplePhysicsEntityType::createInstanceUnchecked(unsigned short instId, unsigned short idOfEnv, unsigned short idInEnv)
{
	RigidBody* newRigidBody;
	OopsObjectID rigidBodyID;
	SimplePhysicsEntity* result;

	printd(INFO, "SimplePhysicsEntityType::createInstanceUnchecked(): building new SimplePhysicsEntity!\n");
	result = new SimplePhysicsEntity(idInEnv, idOfEnv, instId, this);

	if (rigidBody != NULL)
	{
		newRigidBody = rigidBody->clone();
		rigidBodyID = OopsObjectID(false, SimplePhysicsEntity::SIMPLEPHYSICSENTITY_CLASSID, 0, result->getTypeBasedId());
		newRigidBody->setID(rigidBodyID.get());
		result->setRigidBody(newRigidBody);
	} // if
	instances.push_back(result);
	printd(INFO, "SimplePhysicsEntityType::createInstanceUnchecked(): DONE!\n");
	return result;
} // createInstanceUnchecked
