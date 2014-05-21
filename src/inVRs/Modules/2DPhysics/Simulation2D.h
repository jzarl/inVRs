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

/*---------------------------------------------------------------------------*\
 *                                Changes                                    *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
\*---------------------------------------------------------------------------*/

#ifndef _SIMULATION_H_
#define _SIMULATION_H_

#include <gmtl/VecOps.h>
#include <vector>
#include <inVRs/SystemCore/WorldDatabase/WorldDatabase.h>
#include <inVRs/SystemCore/WorldDatabase/Environment.h>
#include <inVRs/SystemCore/WorldDatabase/Tile.h>
#include "RigidBody.h"
#include "HeightMap/HeightMap.h"
#include "2DPhysicsSharedLibraryExports.h"
#include <inVRs/SystemCore/ComponentInterfaces/ModuleInterface.h>
#include "CollisionMap/CollisionMap.h"

struct CollisionData;
class TransformationPipe;

struct CollisionCallbackData
{
	RigidBody *obj1, *obj2;
	CollisionData* data;
	float vRel1, vRel2;
}; // CollisionCallbackData

struct Contact
{
	CollisionData* data;
	RigidBody *obj1, *obj2;
	float epsilon;
}; // Contact

class INVRS_2DPHYSICS_API Simulation2D : public ModuleInterface
{
private:
// 	static std::vector<CollisionPrimitive*> collList;
	std::vector<RigidBody*> objects;
	std::vector<Contact*> contactList;
	std::map<RigidBody*, TransformationPipe*> transformationPipes;
	gmtl::Vec3f up;
	gmtl::Vec3f gravity;

	void collisionResponse(Contact* ct);
	CollisionMap* collMap;
// 	static void readCollMap(NodePtr n);

	friend class Physics2DSyncEvent;

// 	static Circle2D* scooter2D;

public:
	Simulation2D();
	virtual ~Simulation2D();

	virtual bool loadConfig(std::string configFile);
	virtual void cleanup();
	virtual std::string getName();
	virtual Event* createSyncEvent();

// 	static void init(RigidBody* car, HeightMap* h);
// 	static void init(RigidBody* car);

	bool checkCollision();
	bool couldCollide(RigidBody* obj1, RigidBody* obj2);
	void setCollisionMap(CollisionMap* collMap);
// 	static RigidBody* myScooter;
	
// 	static HeightMap* currentHeightMap;
// 	static std::vector<Line2D*>* localCollisionMap;
// 	static bool TestLineWithDisc(Line2D* l, Circle2D* c);
// 	static bool TestDiscWithDisc(Circle2D* c1, Circle2D* c2);
	
// 	static void addCollLine(gmtl::Vec2f p, gmtl::Vec2f q);
// 	static void clearCollLines();
// 	static void addCollCircle(gmtl::Vec2f p, float rad, bool fixed = true, gmtl::Vec3f up = gmtl::Vec3f(0, 1, 0));
	void addObject(RigidBody* obj);
	void removeObject(RigidBody* obj);
	RigidBody* getObject(unsigned id);
	std::vector<RigidBody*>* getObjectList();
// 	static void loadCollMap(char* map);

	void step(float dt);

// Methods for Event-Handling
// 	static void addContact(Vec2f myVec, Vec2f otherVec, Real32 v1, Real32 v2);
// 	static void setOtherBumper(bool bActive);
// 	static void setOtherRubberBand(bool bActive);
// 	static void addImpulse(gmtl::Vec3f impulse);

};

#endif //_SIMULATION_H_

