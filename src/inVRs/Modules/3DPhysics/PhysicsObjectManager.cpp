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

#include "PhysicsObjectManager.h"

#include <assert.h>
#include <OpenSG/OSGLock.h>
#include <OpenSG/OSGThreadManager.h>
#include <OpenSG/OSGThread.h>

#include "Physics.h"

OSG_USING_NAMESPACE
using namespace oops;

//******************//
// PUBLIC METHODS: *//
//******************//

PhysicsObjectManager::PhysicsObjectManager()
{
	physics = NULL;
	addObjectListLock = NULL;
} // PhysicsObjectManager

PhysicsObjectManager::~PhysicsObjectManager()
{

} // ~PhysicsObjectManager

void PhysicsObjectManager::getLocalSimulatedObjects(std::vector<PhysicsObjectInterface*>& dst) {
	printd(ERROR, "PhysicsObjectManager::getLocalSimulatedObjects(): DEPRECATED!!!\n");
} // getLocalSimulatedObjects

void PhysicsObjectManager::getRemoteSimulatedObjects(std::vector<PhysicsObjectInterface*>& dst) {
	printd(ERROR, "PhysicsObjectManager::getRemoteSimulatedObjects(): DEPRECATED!!!\n");
} // getRemoteSimulatedObjects

PhysicsObjectInterface* PhysicsObjectManager::getObjectById(PhysicsObjectID id) {
	std::vector<PhysicsObjectInterface*>::iterator it;

// TODO: for faster access better use a map or something similar
	for (it = objectList.begin(); it != objectList.end(); ++it) {
		if ((*it)->getPhysicsObjectID() == id)
			return *it;
	} // for

//	printd(ERROR, "PhysicsObjectManager::getObjectById(): ObjectListSize = %i, addObjectListSize = %i\n",
//			objectList.size(), addObjectList.size());
	return NULL;
} // getObjectById


//*********************//
// PROTECTED METHODS: *//
//*********************//

RigidBody* PhysicsObjectManager::getRigidBodyById(OopsObjectID id) {
	std::vector<RigidBody*>::iterator it;

// TODO: for faster access better use a map or something similar
	for (it = rigidBodyList.begin(); it != rigidBodyList.end(); ++it) {
		if (id == (*it)->getID())
			return *it;
	} // for

//	printd(ERROR, "PhysicsObjectManager::getRigidBodyById(): rigidBodyListSize = %i, addObjectListSize = %i\n",
//			rigidBodyList.size(), addObjectList.size());
	return NULL;
} // getRigidBodyById

Joint* PhysicsObjectManager::getJointById(OopsObjectID id) {
	std::vector<Joint*>::iterator it;

// TODO: for faster access better use a map or something similar
	for (it = jointList.begin(); it != jointList.end(); ++it) {
		if (id == (*it)->getID())
			return *it;
	} // for

//	printd(ERROR, "PhysicsObjectManager::getJointById(): jointListSize = %i, addObjectListSize = %i\n",
//			jointList.size(), addObjectList.size());
	return NULL;
} // getJointById

void PhysicsObjectManager::init(Physics* physics)
{
	this->physics = physics;
#if OPENSG_THREADMANAGER_GETLOCK_HAVE_BGLOBAL
	// get a non-global lock:
	addObjectListLock = ThreadManager::the()->getLock("PhysicsObjectManager::addObjectListLock", false);
#else
	addObjectListLock = dynamic_cast<Lock*>(ThreadManager::the()->getLock("PhysicsObjectManager::addObjectListLock"));
#endif
	assert(addObjectListLock);
} // init

Simulation* PhysicsObjectManager::getSimulation()
{
	return physics->simulation;
} // getSimulation

unsigned int PhysicsObjectManager::getSimulationTime()
{
	return physics->simulationTime;
} // getSimulationTime

bool PhysicsObjectManager::addPhysicsObject(PhysicsObjectInterface* object)
{
#if OSG_MAJOR_VERSION >=2
	addObjectListLock->acquire();
#else
	addObjectListLock->aquire();
#endif
		addObjectList.push_back(object);
	addObjectListLock->release();
	object->setPhysicsObjectManager(this);
	return true;
} // addPhysicsObject

bool PhysicsObjectManager::addPhysicsObjectInternal(PhysicsObjectInterface* object) {
	bool result = true;
	std::vector<RigidBody*> rigidBodyList;
	std::vector<RigidBody*>::iterator rbIt;
	std::vector<Joint*> jointList;
	std::vector<Joint*>::iterator jIt;

	object->getRigidBodies(rigidBodyList);
	object->getJoints(jointList);

	for (rbIt = rigidBodyList.begin(); rbIt != rigidBodyList.end(); ++rbIt) {
		if (physics->simulation->addRigidBody(*rbIt))
			this->rigidBodyList.push_back(*rbIt);
		else
			result = false;
	} // for

	for (jIt = jointList.begin(); jIt != jointList.end(); ++jIt) {
		if (physics->simulation->addJoint(*jIt))
			this->jointList.push_back(*jIt);
		else
			result = false;
	} // for

	return result;
} // addPhysicsObjectInternal

bool PhysicsObjectManager::removePhysicsObject(PhysicsObjectInterface* object)
{
	int i;
	bool result = true;
	int size = objectList.size();
	std::vector<RigidBody*> rigidBodyList;
	std::vector<RigidBody*>::iterator rbIt, rbIt2;
	std::vector<Joint*> jointList;
	std::vector<Joint*>::iterator jIt, jIt2;

	for (i=0; i < size; i++)
	{
		if (objectList[i] == object)
			break;
	} // for

	if (i == size) {
		printd(WARNING, "PhysicsObjectManager::removePhysicsObjects(): passed object not found in objectlist!\n");
		return false;
	} // if

	object->getRigidBodies(rigidBodyList);
	object->getJoints(jointList);

	for (jIt = jointList.begin(); jIt != jointList.end(); ++jIt) {
		if (physics->simulation->removeJoint(*jIt)) {
			for (jIt2 = this->jointList.begin(); jIt2 != this->jointList.end(); ++jIt2) {
				if (*jIt2 == *jIt) {
					this->jointList.erase(jIt2);
					break;
				} // for
			} // for
		} // if
		else
			result = false;
	} // for

	for (rbIt = rigidBodyList.begin(); rbIt != rigidBodyList.end(); ++rbIt) {
		if (physics->simulation->removeRigidBody(*rbIt)) {
			for (rbIt2 = this->rigidBodyList.begin(); rbIt2 != this->rigidBodyList.end(); ++rbIt2) {
				if (*rbIt2 == *rbIt) {
					this->rigidBodyList.erase(rbIt2);
					break;
				} // if
			} // for
		} // if
		else
			result = false;
	} // for
	objectList.erase(objectList.begin() + i);

	return result;
} // removePhysicsObject

PhysicsObjectInterface* PhysicsObjectManager::removePhysicsObject(PhysicsObjectID physicsObjectID)
{
	int i;
	int size = objectList.size();
	PhysicsObjectInterface* result = NULL;
	std::vector<RigidBody*> rigidBodyList;
	std::vector<RigidBody*>::iterator rbIt;
	std::vector<Joint*> jointList;
	std::vector<Joint*>::iterator jIt;

	for (i=0; i < size; i++)
	{
		if (objectList[i]->getPhysicsObjectID() == physicsObjectID)
			break;
	} // for
	if (i == size)
		return NULL;

	result = objectList[i];
	if (!removePhysicsObject(result))
		printd(ERROR, "PhysicsObjectManager::removePhysicsObject(): Could not remove PhysicsObject!\n");

	return result;
} // removePhysicsObject
