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

#include "Simulation2D.h"

#include <inVRs/SystemCore/DebugOutput.h>
#include "CollisionMap/CollisionMap.h"
#include "HeightMap/HeightMapManager.h"
#include <inVRs/SystemCore/TransformationManager/TransformationManager.h>
#include <inVRs/SystemCore/XMLTools.h>
#include <inVRs/SystemCore/UtilityFunctions.h>
#include "2DPhysicsEvents.h"
#include "Physics2DEntityType.h"
#include <gmtl/Generate.h>
#include <assert.h>
#include <inVRs/SystemCore/SystemCore.h>
#include <inVRs/SystemCore/ComponentInterfaces/module_core_api.h>

Simulation2D::Simulation2D()
{
	gravity = gmtl::Vec3f(0,0,0);
	up = gmtl::Vec3f(0,0,0);
//	SystemCore::registerModuleInterface(this);
	EventManager::registerEventFactory("Physics2DSyncEvent", new Physics2DSyncEvent::Factory());
	WorldDatabase::registerEntityTypeFactory(new Physics2DEntityTypeFactory);
	collMap = NULL;
} // Simulation2D

Simulation2D::~Simulation2D()
{
//	SystemCore::unregisterModuleInterface(this);
} // ~Simulation2D

std::string Simulation2D::getName()
{
	return "Simulation2D";
} // getName

bool Simulation2D::loadConfig(std::string configFile)
{
	IrrXMLReader* xml;

	configFile = getConcatenatedPath(configFile, "Simulation2DConfiguration");
//	configFile = Configuration::getPath("Simulation2DConfiguration")+configFile;
	if (!fileExists(configFile))
	{
		printd(ERROR, "Simulation2D::loadConfig(): ERROR: could not find config-file %s\n", configFile.c_str());
		return false;
	} // if

	xml = createIrrXMLReader(configFile.c_str());

	printd(INFO, "Simulation2D::loadConfig(): loading from file: %s\n", configFile.c_str());
	while (xml && xml->read())
	{
		switch (xml->getNodeType())
		{
			case EXN_ELEMENT:
				if (!strcmp("simulation2D", xml->getNodeName()))
				{
					// Root node
				} // if
				else if (!strcmp("up", xml->getNodeName()))
				{
					up[0] = xml->getAttributeValueAsFloat("xAxis");
					up[1] = xml->getAttributeValueAsFloat("yAxis");
					up[2] = xml->getAttributeValueAsFloat("zAxis");
					gmtl::normalize(up);
				} // else if
				else if (!strcmp("gravity", xml->getNodeName()))
				{
					gravity[0] = xml->getAttributeValueAsFloat("xValue");
					gravity[1] = xml->getAttributeValueAsFloat("yValue");
					gravity[2] = xml->getAttributeValueAsFloat("zValue");
				} // else if
				else
				{
					printd(WARNING, "JointInteraction::readConfig(): WARNING: unknown element with name %s found!\n", xml->getNodeName());
				} // else
				break;
			default:
				break;
		} // switch
	} // while
	delete xml;
	return true;
} // loadConfig

void Simulation2D::cleanup()
{
	int i;
	for (i=0; i<(int)objects.size(); i++)
	{
		if (objects[i])
			delete objects[i];
	} // for
	objects.clear();
} // cleanup ~Simulation2D

void Simulation2D::addObject(RigidBody* obj)
{
	objects.push_back(obj);
//	Vec2f p = Vec2f(obj->x[0], obj->x[2]);
} // addCollCircle

void Simulation2D::removeObject(RigidBody* obj)
{
	int i;
	for(i=0;i<(int)objects.size();i++)
	{
		if(objects[i] == obj)
		{
			objects.erase(objects.begin()+i);
		}
	}
}

RigidBody* Simulation2D::getObject(unsigned id)
{
	if (id >= objects.size())
		return NULL;
	return objects[id];
} // getObject

void Simulation2D::setCollisionMap(CollisionMap* collMap)
{
	this->collMap = collMap;
} // setCollisionMap

bool Simulation2D::checkCollision()
{
	int i,j, k;
	RigidBody *obj1, *obj2;
	std::vector<CollisionData*> collisionList;
	Contact *ct;
	Environment *env;
	Tile *t;
	bool isFixed1, isFixed2;
	gmtl::Vec3f position;
	TransformationData trans;

// TODO: write RigidBody-Transformation to shape!!!

	for (i=0; i < (int)objects.size(); i++)
	{
		obj1 = objects[i];
		assert(obj1);
		isFixed1 = obj1->isFixed();
		for (j=i+1; j < (int)objects.size(); j++)
		{
			obj2 = objects[j];
			assert(obj2);
			isFixed2 = obj2->isFixed();
			if (isFixed1 && isFixed2)
				continue;
			if (!couldCollide(obj1, obj2))
				continue;

			obj1->getShape()->checkCollision(obj2->getShape(), collisionList);
			for (k=0; k < (int)collisionList.size(); k++)
			{
				ct = new Contact;
				ct->data = collisionList[k];
				ct->obj1 = obj1;
				ct->obj2 = obj2;
				ct->epsilon = 0.25;
				contactList.push_back(ct);
			} // for
			collisionList.clear();
		} // for

		if (!isFixed1)
		{
			env = WorldDatabase::getEnvironmentAtWorldPosition(obj1->x[0], obj1->x[2]);
			if (!env)
				continue;
			t = env->getTileAtWorldPosition(obj1->x[0], obj1->x[2]);
			if (!t)
				continue;

			if(!collMap)
				continue;
			CollisionObject* tileCollMap = collMap->getTileCollisionMap(t->getId());
			if (!tileCollMap)
				continue;

			position = env->getWorldPositionOfTile(t);
			tileCollMap->getTransformation().position = position;
			obj1->getShape()->checkCollision(tileCollMap, collisionList);

			for (k=0; k < (int)collisionList.size(); k++)
			{
				ct = new Contact;
				ct->data = collisionList[k];
				ct->obj1 = obj1;
				ct->obj2 = NULL;
				ct->epsilon = 0.25;
				contactList.push_back(ct);
			} // for
			collisionList.clear();
		} // if
	} // for
	return (contactList.size() > 0);
} // checkCollision

bool Simulation2D::couldCollide(RigidBody* obj1, RigidBody* obj2)
{
	float radius1, radius2;
	gmtl::Vec2f pos1, pos2;
	gmtl::Vec2f distance;
	radius1 = obj1->getBoundingCircleRadius();
	radius2 = obj2->getBoundingCircleRadius();
	if (radius1 < 0 || radius2 < 0)
		return true;

	pos1 = obj1->get2DPos();
	pos2 = obj2->get2DPos();
	distance = pos1 - pos2;
	if (gmtl::length(distance) > radius1 + radius2)
		return false;

	return true;
} // couldCollide

void Simulation2D::step(float dt)
{
	int i;
	Contact* ct;
	RigidBody* obj;
	float massInv;
	gmtl::Vec2f position;
	gmtl::Vec3f v_dt, force_dt, surfacenormal;
	gmtl::Vec3f angV_dt, torque_dt;
	gmtl::Vec3f axis;
	gmtl::AxisAnglef deltaAngle;
	gmtl::Quatf deltaRot;
	float height, impulse;
	TransformationPipe* pipe;
	Environment* env;
	Entity* entity;
	Tile* tle;
	gmtl::Vec3f tlePos;
	TransformationData trans;
	gmtl::Matrix44f rotationMatrix;
	CollisionObject* objectShape;
	HeightMapInterface* h;

	// Collision-response
	if ((int)contactList.size() != 0)
	{
		for (i=0; i < (int)contactList.size(); i++)
		{
			ct = contactList[i];
			collisionResponse(ct);
			delete ct->data;
			delete ct;
		} // for
		contactList.clear();
	} // if

	// Integration step
	for (i=0; i < (int)objects.size(); i++)
	{
		h = NULL;
		obj = (RigidBody*)(objects[i]);
		if (obj->isFixed())
			continue;

		massInv = obj->getMassInv();

		position[0] = obj->x[0];
		position[1] = obj->x[2];

		env = WorldDatabase::getEnvironmentAtWorldPosition(position[0], position[1]);
		if (env)
		{
			tle = env->getTileAtWorldPosition(position[0], position[1]);
			if (tle)
			{
				tlePos = env->getWorldPositionOfTile(tle);
				h = HeightMapManager::getHeightMapOfTile(tle->getId());
			} // if
		} // if

		v_dt = obj->v * dt;
		force_dt = obj->force * dt;
		force_dt = force_dt + gravity*(dt/massInv);

		angV_dt = obj->angV * dt;
		torque_dt = obj->torque * dt;

// TODO: check if force reset works here!!!
		obj->force = obj->constantForce;
		obj->torque = obj->constantTorque;

		obj->x += v_dt;
		obj->v += force_dt * massInv;
		axis = angV_dt;
		gmtl::normalize(axis);
		deltaAngle = gmtl::AxisAnglef(gmtl::length(angV_dt), axis);
		gmtl::set(deltaRot, deltaAngle);
		obj->setOrientation(obj->orientation * deltaRot);
		obj->angV += torque_dt * massInv;

		if (h)
		{
			gmtl::Vec3f tempVec;
			tempVec = h->getNormal(obj->x[0] - tlePos[0], obj->x[2] - tlePos[2]);
			surfacenormal[0] = tempVec[0];
			surfacenormal[1] = tempVec[1];
			surfacenormal[2] = tempVec[2];
		} // if
		else
			surfacenormal = up;

		obj->setUp(surfacenormal);

		if (h)
			height = h->getHeight(obj->x[0] - tlePos[0], obj->x[2] - tlePos[2]);
		else
			height = 0;

		// Remove speed in negative up-direction if interpenetrating with ground
		if(obj->x[1]<=height)
		{
			impulse = gmtl::dot(surfacenormal,obj->v);
			if(impulse < 0)
				obj->v = obj->v - impulse*surfacenormal;
			obj->x[1] = height;
		} // if

		entity = obj->entity;
		if (!entity)
			continue;

		pipe = transformationPipes[obj];
		if (!pipe)
		{
			unsigned typeBasedId;
			unsigned short type, id;
			typeBasedId = entity->getTypeBasedId();
			split(typeBasedId, type, id);
			pipe = TransformationManager::openPipe(PHYSICS_MODULE_ID, WORLD_DATABASE_ID, 0, 0, type, id, 0x0E000000, false);
			transformationPipes[obj] = pipe;
		} // if
		trans = identityTransformation();
		trans.position = obj->x;
		trans.orientation = obj->orientation;
 		pipe->push_back(trans);

		objectShape = obj->getShape();
		assert(objectShape);
		objectShape->setTransformation(trans);
	} // for

// Friction simulation!!!
// TODO: implement Friction simulation

// // Bumper simulation!!!
// TODO: implement in Application

// Collision check
	checkCollision();
} // step

void Simulation2D::collisionResponse(Contact* ct)
{
	CollisionCallbackData* collData;
// TODO: check for correctness
	gmtl::Vec3f vec1;
	gmtl::Vec3f vec2;
	gmtl::Vec3f vObj, up, normal1, normal2;
	float impulse, upLen, vRel1, vRel2, massInv1, massInv2;
	gmtl::Vec3f response1, response2;
	RigidBody* obj1 = NULL;
	RigidBody* obj2 = NULL;
	float epsilon = ct->epsilon;			// Daempfung: 0 = keine Abprallung, 1 = 100Prozent Abprallung

	if (!ct->obj1 && !ct->obj2)
		return;

	if (ct->obj1->isFixed() && (!ct->obj2 || ct->obj2->isFixed())) return;

	if (ct->obj1->isFixed())
	{
		obj1 = ct->obj2;
		vec1 = gmtl::Vec3f(ct->data->normal2[0], 0, ct->data->normal2[1]);
		obj2 = ct->obj1;
		vec2 = gmtl::Vec3f(ct->data->normal1[0], 0, ct->data->normal1[1]);
	} // if
	else					// Kollision Scooter-fixes Objekt
	{
		obj1 = ct->obj1;
		vec1 = gmtl::Vec3f(ct->data->normal1[0], 0, ct->data->normal1[1]);
		obj2 = ct->obj2;
		vec2 = gmtl::Vec3f(ct->data->normal2[0], 0, ct->data->normal2[1]);
	} // else if

	massInv1 = obj1->getMassInv();
	vObj = obj1->getCollisionV(vec1);		// Velocity of object
// 	printd("vObj = %f %f %f\n", vObj[0], vObj[1], vObj[2]);
	up = obj1->up;					// UP-Vector of object
	upLen = gmtl::dot(vec1, up) * -1;		// UP-Part of vector to collision point
	normal1 = vec1 + upLen*up;			// vector to collision point in objects plane
	gmtl::normalize(normal1);			//
	vRel1 = gmtl::dot(normal1,vObj);		// Relative velocity of object to collision point in objects plane
// 	printd("vRel1 = %f\n", vRel1);

	if (obj2 && !obj2->isFixed())
	{
		massInv2 = obj2->getMassInv();
		vObj = obj2->getCollisionV(vec2);
		up = obj2->up;
		upLen = gmtl::dot(vec2, up) * -1;	// Anteil lokale y-Achse
		normal2 = vec2 + upLen*up;
		gmtl::normalize(normal2);
		vRel2 = gmtl::dot(normal2,vObj);
	} // if
	else
	{
		massInv2 = 0;
		vRel2 = 0;
	} // else

	if (vRel1+vRel2 < 0) return;		// Objekte bewegen sich bereits auseinander

	if (obj1 && obj1->notifyAtCollision)
	{
		collData = new CollisionCallbackData;
		collData->obj1 = obj1;
		collData->obj2 = obj2;
		collData->data = ct->data;
		collData->vRel1 = vRel1;
		collData->vRel2 = vRel2;
		obj1->collisionCallback(collData);
		delete collData;
	}
	if (obj2 && obj2->notifyAtCollision)
	{
		collData = new CollisionCallbackData;
		collData->obj1 = obj1;
		collData->obj2 = obj2;
		collData->data = ct->data;
		collData->vRel1 = vRel1;
		collData->vRel2 = vRel2;
		obj1->collisionCallback(collData);
		delete collData;
	} // if

	impulse = (-(1+epsilon)*(vRel1 + vRel2))*(1/(massInv1 + massInv2));

	response1 = (impulse*massInv1)*normal1;	// Anteil in Ebene
	obj1->v += response1;

	if (obj2 && !obj2->isFixed())
	{
		response2 = (impulse*massInv2)*normal2;
		obj2->v += response2;
	} // if
// Uncomment the following for collision debug-output
/*	else
		response2 = gmtl::Vec3f(0,0,0);
	float oldSpeed = vRel1 + vRel2;
	float newSpeed = -impulse*massInv1-vRel1 - impulse*massInv2-vRel2;
	printd(INFO, "Relative velocities (eps=%f): before = %f / after = %f (should be %f)\n", epsilon, oldSpeed, newSpeed, oldSpeed * epsilon);
*/
} // collisionResponse

Event* Simulation2D::createSyncEvent()
{
	return new Physics2DSyncEvent();
} // createSyncEvent

std::vector<RigidBody*>* Simulation2D::getObjectList()
{
	return &objects;
}

MAKEMODULEPLUGIN(Simulation2D, SystemCore)

