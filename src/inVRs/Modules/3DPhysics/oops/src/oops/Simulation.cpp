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

#include <assert.h>
#include <gmtl/Xforms.h>

//#include <OpenSG/OSGQuaternion.h>
//#include <OpenSG/OSGMatrix.h>
//#include <OpenSG/OSGSimpleSceneManager.h>
#include "oops/Simulation.h"
#include "oops/RigidBody.h"
#include "oops/Geometries.h"
#include "oops/Joints.h"
#include "oops/Interfaces/TransformationWriterInterface.h"
#include "oops/FrictionValues.h"
#include "oops/ArticulatedBody.h"
#include "oops/OopsMath.h"
#include "oops/JointFactories.h"
#include "oops/GeometryFactories.h"
//#include "oops/Interfaces/RigidBodyFactory.h"
#include "oops/Interfaces/CollisionListenerInterface.h"

// Object Oriented Physics Simulation
namespace oops
{

//******************************************************************************
// Begin Simulation

Simulation::Simulation()
{
	dRandSetSeed(0xDEADBEEF);
	world = dWorldCreate();
	space = dHashSpaceCreate(0);
	dWorldSetGravity(world, 0, 0, 0);
	contactGroup = dJointGroupCreate(0);
 	timestep = 0.01;
	function = STEPFUNCTION_STEP;
	numberOfSteps = 1;
	doCollisionDetection = true;
	objectsVisible = false;
} // Simulation

Simulation::~Simulation()
{
	fprintf(stderr, "Simulation destructor!\n");
	dJointGroupDestroy(contactGroup);
	dSpaceDestroy(space);
	dWorldDestroy(world);

	// TODO: check if all RigidBodies and ArticulatedBodies are freed too

	dCloseODE();
} // ~Simulation

//*****************************************
// internal methods used by simulation loop
//*****************************************
void Simulation::applyObjectForces()
{
	int i;
	for (i=0; i < (int)rigidBodyList.size(); i++)
		rigidBodyList[i]->applyForce();
} // applyObjectForces

void Simulation::updatePosAndRot()
{
	int i;
	for (i=0; i < (int)rigidBodyList.size(); i++)
		rigidBodyList[i]->updateTransformation();
} // updatePosAndRot

//************************************
// setter for simulation specific data
//************************************

void Simulation::activateCollisionDetection()
{
	doCollisionDetection = true;
} // activateCollisionDetection

void Simulation::deactivateCollisionDetection()
{
	doCollisionDetection = false;
} // deactivateCollisionDetection

void Simulation::setGlobalERP(float erp)
{
	dWorldSetERP(world, erp);
} // setGlobalERP

void Simulation::setGlobalCFM(float cfm)
{
	dWorldSetCFM(world, cfm);
} // setGlobalCFM

void Simulation::setGravity(gmtl::Vec3f gravity)
{
	int i;

	for (i=0; i < (int)rigidBodyList.size(); i++)
		rigidBodyList[i]->setActive(true);

	dWorldSetGravity(world, gravity[0], gravity[1], gravity[2]);
} // setGravity

void Simulation::setStepFunction(STEPFUNCTION function, unsigned numberOfSteps)
{
	this->function = function;
	this->numberOfSteps = numberOfSteps;
} // setStepFunction

void Simulation::setStepSize(float stepSize)
{
	Simulation::timestep = stepSize;
} // setStepSize

void Simulation::setGlobalAutoDisable(bool active)
{
	dWorldSetAutoDisableFlag(world, active);
} // setGlobalAutoDisable

void Simulation::setAutoDisableLinearThreshold(float threshold)
{
	dWorldSetAutoDisableLinearThreshold(world, threshold);
} // setAutoDisableLinearThreshold

void Simulation::setAutoDisableAngularThreshold(float threshold)
{
	dWorldSetAutoDisableAngularThreshold(world, threshold);
} // setAutoDisableAngularThreshold

void Simulation::setAutoDisableTime(float time)
{
	dWorldSetAutoDisableTime(world, time);
} // setAutoDisableTime

//******************************************************
// method to activate or deactivate rendering of objects
//******************************************************
void Simulation::setObjectsVisible(bool visible)
{
	int i;
	for (i=0; i < (int)rigidBodyList.size(); i++)
		rigidBodyList[i]->setVisible(visible);
	objectsVisible = visible;
} // setObjectsVisible

//***************************************************************
// methods to add or remove SimulationObjects from the Simulation
//***************************************************************
bool Simulation::addRigidBody(RigidBody *obj)
{
	uint64_t id = obj->getID();
	if (rigidBodyMap[id] != NULL)
	{
		printf(
			"Simulation::addRigidBody(): WARNING: Object with ID %llu already registered! IGNORING new object!\n",
			(long long unsigned)id);
		return false;
	} // if
	obj->build(world, space);
 	rigidBodyMap[id]=obj;
 	rigidBodyList.push_back(obj);
	obj->addedToSimulation = true;
	if (objectsVisible)
		obj->setVisible(true);
	return true;
} // addRigidBody

bool Simulation::addJoint(Joint* joint)
{
	uint64_t id = joint->getID();
	if (jointMap[id] != NULL)
	{
		printf("Simulation::addJoint(): ERROR: Joint with ID %llu already \
				registered!\n", (long long unsigned)id);
		return false;
	} // if
	joint->build(world);
	jointMap[id] = joint;
	jointList.push_back(joint);
	joint->addedToSimulation = true;
	return true;
} // addJoint

bool Simulation::addArticulatedBody(ArticulatedBody* obj)
{
	int i;
	bool error = false;
	RigidBody* body;
	Joint* joint;
	uint32_t temp;
	uint32_t rigidBodyID;
	uint32_t jointID;
	uint64_t articulatedBodyID = obj->getID();

	if (articulatedBodyMap[articulatedBodyID] != NULL)
	{
		printf("Simulation::addArticulatedBody(): ERROR: ArticulatedBody with \
				ID %llu already registered!\n", (long long unsigned)articulatedBodyID);
		return false;
	} // if
	for (i=0; i < (int)obj->rigidBodyList.size(); i++)
	{
		body = obj->rigidBodyList[i];
		if (!addRigidBody(body))
		{
			split(body->getID(), temp, rigidBodyID);
			printf("Simulation::addArticulatedBody(): ERROR: RigidBody with \
					local ID %u is already registered in ArticulatedBody with \
					ID %llu!\n", rigidBodyID, (long long unsigned)articulatedBodyID);
			error = true;
		} // if
	} // for
	for (i=0; i < (int)obj->jointList.size(); i++)
	{
		joint = obj->jointList[i];
		if (!addJoint(joint))
		{
			split(joint->getID(), temp, jointID);
			printf("Simulation::addArticulatedBody(): ERROR: Joint with local \
					ID %u is already registered in ArticulatedBody with ID \
					%llu!\n", jointID, (long long unsigned)articulatedBodyID);
			error = true;
		} // if
	} // for
	articulatedBodyMap[articulatedBodyID] = obj;
	articulatedBodyList.push_back(obj);
	obj->addedToSimulation = true;
	return !error;
} // addArticulatedBody

bool Simulation::removeRigidBody(RigidBody* obj)
{
	int i;
	int size = rigidBodyList.size();
	if (rigidBodyMap[obj->getID()] == NULL)
	{
		printf("Simulation::removeRigidBody(): ERROR: RigidBody with ID %llu \
				not found!\n", (long long unsigned)obj->getID());
		return false;
	} // if
	for (i=0; i < size; i++)
	{
		if (rigidBodyList[i] == obj)
		{
			rigidBodyList.erase(rigidBodyList.begin()+i);
			break;
		} // if
	} // for
	if (i == size)
	{
		printf("Simulation::removeRigidBody(): ERROR: RigidBody with ID %llu \
				not found in rigidBodyList!\n", (long long unsigned)obj->getID());
		return false;
	} // if

	rigidBodyMap.erase(obj->getID());
	obj->destroy();
	obj->addedToSimulation = false;
	return true;
} // removeRigidBody

bool Simulation::removeRigidBodyWithID(uint64_t id)
{
	RigidBody* obj = rigidBodyMap[id];
	if (!obj)
	{
		printf("Simulation::removeRigidBodyWithID(): ERROR: RigidBody with ID \
				%llu not found in rigidBodyMap!\n", (long long unsigned)id);
		return false;
	} // if
	return removeRigidBody(obj);
} // removeRigidBodyWithID

bool Simulation::removeJoint(Joint* joint)
{
	int i;
	int size = jointList.size();
	if (jointMap[joint->getID()] == NULL)
	{
		printf("Simulation::removeJoint(): ERROR: Joint with ID %llu not \
				found!\n", (long long unsigned)joint->getID());
		return false;
	} // if
	for (i=0; i < size; i++)
	{
		if (jointList[i] == joint)
		{
			jointList.erase(jointList.begin()+i);
			break;
		} // if
	} // for
	if (i == size)
	{
		printf("Simulation::removeJoint(): ERROR: Joint with ID %llu not found \
				in jointList!\n", (long long unsigned)joint->getID());
		return false;
	} // if

	jointMap.erase(joint->getID());
	joint->destroy();
	joint->addedToSimulation = false;
	return true;
} // removeJoint

bool Simulation::removeJointWithID(uint64_t id)
{
	Joint* joint = jointMap[id];
	if (!joint)
		return false;

	return removeJoint(joint);
} // removeJoint

bool Simulation::removeArticulatedBody(ArticulatedBody* obj,
	bool leaveRigidBodies)
{
	int i;
	uint64_t id = obj->getID();
	int size = rigidBodyList.size();
	if (articulatedBodyMap[id] == NULL)
	{
		printf("Simulation::removeArticulatedBody(): ERROR: ArticulatedBody \
				with ID %llu not found!\n", (long long unsigned)id);
		return false;
	} // if
	for (i=0; i < size; i++)
	{
		if (articulatedBodyList[i] == obj)
		{
			articulatedBodyList.erase(articulatedBodyList.begin()+i);
			break;
		} // if
	} // for
	if (i == size)
	{
		printf("Simulation::removeArticulatedBody(): ERROR: ArticulatedBody \
				with ID %llu not found in articulatedBodyList!\n", (long long unsigned)id);
		return false;
	} // if

	articulatedBodyMap.erase(id);
	if (!leaveRigidBodies)
	{
		bool success;
		for (i=0; i < (int)obj->jointList.size(); i++) {
			success = removeJoint(obj->jointList[i]);
			assert(success);
		} // for
		for (i=0; i < (int)obj->rigidBodyList.size(); i++) {
			success = removeRigidBody(obj->rigidBodyList[i]);
			assert(success);
		} // for
	} // if
	obj->addedToSimulation = false;
	return true;
} // removeArticulatedBody

bool Simulation::removeArticulatedBodyWithID(uint64_t id, bool leaveRigidBodies)
{
	ArticulatedBody* obj = articulatedBodyMap[id];
	if (!obj)
	{
		printf("Simulation::removeArticulatedBodyWithID(): ERROR: \
				ArticulatedBody with ID %llu not found in \
				articulatedBodyMap!\n", (long long unsigned)id);
		return false;
	} // if
	return removeArticulatedBody(obj);
} // removeArticulatedBodyWithID

void Simulation::registerCollisionListener(CollisionListenerInterface* listener)
{
	collisionListenerList.push_back(listener);
} // registerCollisionListener

//**************************************************
// public collision detection and simulation methods
//**************************************************
void Simulation::checkCollisionHelp(void* data, dGeomID o1, dGeomID o2)
{
	Simulation *sim = (Simulation*)data;
	sim->checkCollision(data, o1, o2);
} // checkCollisionHelp

void Simulation::checkCollision(void* data, dGeomID o1, dGeomID o2)
{
	RigidBody* body1 = NULL;
	RigidBody* body2 = NULL;

	if (dGeomIsSpace(o1) || dGeomIsSpace(o2))
	{
		// colliding a space with something
		dSpaceCollide2(o1, o2, data, Simulation::checkCollisionHelp);
		// collide all geoms internal to the space(s)
		if (dGeomIsSpace(o1))
			dSpaceCollide((dSpaceID)o1, data, Simulation::checkCollisionHelp);
		if (dGeomIsSpace (o2))
			dSpaceCollide((dSpaceID)o2, data, Simulation::checkCollisionHelp);
	} // if

	dBodyID b1 = dGeomGetBody(o1);
   	dBodyID b2 = dGeomGetBody(o2);

	Geometry* geom1 = (Geometry*)dGeomGetData(o1);
	Geometry* geom2 = (Geometry*)dGeomGetData(o2);

	bool surfaceParamSet = false;

   // exit without doing anything if the two bodies are connected by a joint
	if (b1 == b2)
		return;
// TODO: maybe we should make this option configureable
// 	if (b1 && b2 && dAreConnectedExcluding (b1,b2,dJointTypeContact))
// 		return;

	if (geom1)
		body1 = geom1->getOwner();
	if (geom2)
		body2 = geom2->getOwner();

	if (body1 && body2)
	{
		if (!body1->canCollideWith(body2) || !body2->canCollideWith(body1))
			return;
		if (jointConnectionAvoidsCollision(body1, body2))
			return;
	} // if

	dContact *contacts = new dContact[10];

	int numContacts = dCollide(o1, o2, 10, &contacts[0].geom, sizeof(dContact));

	// Begin Create contact joint for every collision point
	for(int i = 0; i < numContacts; ++i)
	 {
		if (geom1)
			surfaceParamSet = geom1->setContactParam(contacts[i], geom2, true);

		if (!surfaceParamSet && geom2)
			surfaceParamSet = geom2->setContactParam(contacts[i], geom1, false);

		if (!surfaceParamSet)
		{
			contacts[i].surface.mode = dContactApprox1;
			if (geom1 && geom2)
				contacts[i].surface.mu = getFriction(geom1->getMaterial(),
					geom2->getMaterial());
			else if (geom1)
				contacts[i].surface.mu = getFriction(geom1->getMaterial(),
					materials::UNDEFINED);
			else
				contacts[i].surface.mu = getFriction(geom2->getMaterial(),
					materials::UNDEFINED);
		} // if
		dJointID c = dJointCreateContact(world, contactGroup, &contacts[i]);

		dJointAttach(c, b1, b2);
// 		dJointAttach(c, dGeomGetBody(contacts[i].geom.g1),
//			dGeomGetBody(contacts[i].geom.g2));
//		printf("Collision!\n");
	}

	if (numContacts > 0)
	{
		// Create list of contacts for callbacks
		dReal *pos, *normal;
		std::vector<ContactData> contactList;
		for(int i = 0; i < numContacts; ++i)
		{
			ContactData contact;
			pos = contacts[i].geom.pos;
			normal = contacts[i].geom.normal;
			contact.position = gmtl::Vec3f(pos[0], pos[1], pos[2]);
			contact.normal = gmtl::Vec3f(normal[0], normal[1], normal[2]);
			contact.penetrationDepth = contacts[i].geom.depth;
			contact.body1 = NULL;
			contact.body2 = NULL;
			if (contacts[i].geom.g1 == o1)
			{
				if (b1)
					contact.body1 = (RigidBody*)dBodyGetData(b1);
				if (b2)
					contact.body2 = (RigidBody*)dBodyGetData(b2);
			} // if
			else
			{
				if (b2)
					contact.body1 = (RigidBody*)dBodyGetData(b2);
				if (b1)
					contact.body2 = (RigidBody*)dBodyGetData(b1);
			} // else
			contactList.push_back(contact);
		} // for

		for (int i=0; i < (int)collisionListenerList.size(); i++)
			collisionListenerList[i]->notifyCollision(body1, body2, contactList);
		if (body1)
			body1->collisionOccured(body2, contactList);
		if (body2)
			body2->collisionOccured(body1, contactList);
	} // if

	 // End Create contact joint for everycollision point
	delete[] contacts;
} // checkCollision

void Simulation::checkCollisionWithWorldHelp(void* data, dGeomID o1, dGeomID o2)
{
	void** passedData = (void**)data;
	Simulation* sim = (Simulation*)(passedData[0]);
	std::vector<ContactData*>* contacts =
		(std::vector<ContactData*>*)(passedData[1]);
	if (dGeomIsSpace(o1) || dGeomIsSpace(o2))
		dSpaceCollide2(o1, o2, data, Simulation::checkCollisionWithWorldHelp);
	else
		sim->checkCollisionWithWorld(o1, o2, contacts);
} // checkCollisionWithWorldHelp

void Simulation::checkCollisionWithWorld(dGeomID o1, dGeomID o2,
	std::vector<ContactData*>* contacts)
{
	dReal *pos, *normal;
	ContactData* contact;
	dBodyID b1 = dGeomGetBody(o1);
	dBodyID b2 = dGeomGetBody(o2);

   // exit without doing anything if the two bodies are connected by a joint
	if (o1 == o2)
		return;
	if (b1 && b2 && dAreConnectedExcluding (b1,b2,dJointTypeContact))
		return;

	dContact *odeContacts = new dContact[10];

	int numContacts = dCollide(o1, o2, 10, &odeContacts[0].geom,
		sizeof(dContact));

	// Add new entry in contacts-list for every recognized contact
	for(int i = 0; i < numContacts; ++i)
	{
		contact = new ContactData;
		pos = odeContacts[i].geom.pos;
		normal = odeContacts[i].geom.normal;
		contact->position = gmtl::Vec3f(pos[0], pos[1], pos[2]);
		contact->normal = gmtl::Vec3f(normal[0], normal[1], normal[2]);
		contact->penetrationDepth = odeContacts[i].geom.depth;
		contact->body1 = NULL;
		contact->body2 = NULL;
		if (odeContacts[i].geom.g1 == o1)
		{
			if (b1)
				contact->body1 = (RigidBody*)dBodyGetData(b1);
			if (b2)
				contact->body2 = (RigidBody*)dBodyGetData(b2);
		} // if
		else
		{
			if (b2)
				contact->body1 = (RigidBody*)dBodyGetData(b2);
			if (b1)
				contact->body2 = (RigidBody*)dBodyGetData(b1);
		} // else
		contacts->push_back(contact);
	} // for
} // checkCollisionWithWorld

void Simulation::checkCollisionWithWorld(RigidBody* body,
	std::vector<ContactData*>& contacts)
{
	void* data[2];
	data[0] = this;
	data[1] = &contacts;
	dGeomID geom = body->getODEGeometry();

	if (geom)
		dSpaceCollide2(geom, (dGeomID)space, data, Simulation::checkCollisionWithWorldHelp);
	else
		printf("WARNING, Simulation::checkCollisionWithWorld(): no Geometry to \
				RigidBody found!\n");
} // checkCollisionWithWorld

void Simulation::simulate(float dt)
{
// 	static char buffer[128];
// 	static int simsteps = 0;
// 	static float lastTime = 0;
	static float oldStepTime = 0;
	static float currentTime = 0;
	int loopCount = 0;

	currentTime = currentTime + dt;

/*	if ((currentTime-oldStepTime) < timestep)
		printf("Simulation::simulate(): Timestep %f is higher than dt %f\n", timestep, (currentTime - oldStepTime));*/
	while((currentTime-oldStepTime) >= timestep)
	{
		step(timestep);

		oldStepTime += timestep;
		loopCount++;
		if (loopCount > 5)
			break;
	} // while

	if (loopCount > 1)
		printf("WARNING: found %i simulation loops per frame!!!\n", loopCount);

	renderObjects();

/*
    if((currentTime - lastTime) >= 1)
    {
            sprintf(buffer, "FPS = %d", simsteps);
            printf("%s\n", buffer);
            simsteps = 0;
            lastTime = currentTime;
    }
    simsteps++;
*/
} // simulate

void Simulation::step(float dt)
{
	//Apply forces
	applyObjectForces();

	//Adjust joint parameters
	// NOT IMPLEMENTED YET

	// Begin Call collision detection
	if (doCollisionDetection)
		dSpaceCollide(space, this, checkCollisionHelp);
	// End Call collision detection

	// Begin Take simulation step
	if (function == STEPFUNCTION_STEP)
		dWorldStep(world, dt);
	else if (function == STEPFUNCTION_QUICKSTEP)
		dWorldQuickStep(world, dt);
	else if (function == STEPFUNCTION_STEPFAST1)
		dWorldStepFast1(world, dt, numberOfSteps);
	else
		printf("Simulation::step(): ERROR: unknown step-function with id %u",
			(unsigned)function);
	// End Take simulation step

	// Begin Remove all joints in contact group
	if (doCollisionDetection)
		dJointGroupEmpty(contactGroup);
	// End Remove all joints in contact group

	// Apply Object Pos and Rot to OSG-Object
	updatePosAndRot();
} // step

void Simulation::renderObjects()
{
	int i;
	for (i=0; i < (int)rigidBodyList.size(); i++)
		rigidBodyList[i]->render();
} // renderObjects

//************************************
// getter for simulation specific data
//************************************
const dWorldID Simulation::getODEWorld()
{
	return world;
} // getODEWorld

const dSpaceID Simulation::getODESpace()
{
	return space;
} // getODESpace

float Simulation::getGlobalERP()
{
	return (float)dWorldGetERP(world);
} // getGlobalERP

float Simulation::getGlobalCFM()
{
	return (float)dWorldGetCFM(world);
} // getGlobalCFM

gmtl::Vec3f Simulation::getGravity()
{
	dVector3 gravity;
	dWorldGetGravity(world, gravity);
	return gmtl::Vec3f(gravity[0], gravity[1], gravity[2]);
} // getGravity

float Simulation::getStepSize()
{
	return timestep;
} // getStepSize

//****************************************
// getter for registered SimulationObjects
//****************************************
RigidBody* Simulation::getRigidBodyByID(uint64_t id)
{
	return rigidBodyMap[id];
} // getRigidBodyByID

RigidBody* Simulation::getRigidBodyByName(std::string name)
{
	int i;
	RigidBody* result = NULL;
	for (i=0; i < (int)rigidBodyList.size(); i++)
	{
		if (rigidBodyList[i]->getName() == name)
		{
			result = rigidBodyList[i];
			break;
		} // if
	} // for
	return result;
} // getRigidBodyByName

Joint* Simulation::getJointByID(uint64_t id)
{
	return jointMap[id];
} // getJointByID

Joint* Simulation::getJointByName(std::string name)
{
	int i;
	Joint* result = NULL;
	for (i=0; i < (int)jointList.size(); i++)
	{
		if (name == jointList[i]->getName())
		{
			result = jointList[i];
			break;
		} // if
	} // for
	return result;
} // getJointByName

ArticulatedBody* Simulation::getArticulatedBodyByID(uint64_t id)
{
	return articulatedBodyMap[id];
} // getArticulatedBodyByID

ArticulatedBody* Simulation::getArticulatedBodyByName(std::string name)
{
	int i;
	ArticulatedBody* result = NULL;
	for (i=0; i < (int)articulatedBodyList.size(); i++)
	{
		if (articulatedBodyList[i]->getName() == name)
		{
			result = articulatedBodyList[i];
			break;
		} // if
	} // for
	return result;
} // getArticulatedBodyByName

bool Simulation::jointConnectionAvoidsCollision(RigidBody* body1, RigidBody* body2) {
	bool result = false;

	if (!body1 || !body2)
		return false;

	dBodyID b1 = body1->getODEBody();
	dBodyID b2 = body2->getODEBody();

	if (!b1 && !b2)
		return false;

	if (!b1) {
		dBodyID temp = b1;
		b1 = b2;
		b2 = temp;
	} // if

	Joint* joint = NULL;
	RigidBody* otherBody;
	dJointID odeJoint;
	int numJoints = dBodyGetNumJoints(b1);

	// Iterate over all Joints which are connected to Body 1
	for (int i=0; i < numJoints; i++) {
		odeJoint = dBodyGetJoint(b1, i);
		if (!odeJoint) {
			continue;
		} // if
		joint = (Joint*)dJointGetData(odeJoint);
		// ignore not user-defined joints (like contact joint)
		if (!joint) {
			continue;
		} // if

		// get second RigidBody the joint is connected with
		if (joint->getObject1() == body1) {
			otherBody = joint->getObject2();
		} // if
		else {
			otherBody = joint->getObject1();
		} // else

		// check if the second body is equal to the passed Body 2
		if (otherBody == body2) {
			// Get result if Joint avoids collision and combine this with other results:
			// Needed when more than one joint is connecting both objects, as soon as one
			// Joint avoid the collision the collision occurs!
			result = joint->doesAvoidCollision() || result;
		} // if
	} // for

	return result;
} // jointConnectionAvoidsCollision


} // oops
