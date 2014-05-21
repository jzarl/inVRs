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

#ifndef _SIMULATION_H
#define _SIMULATION_H

#include <ode/ode.h>
#include <time.h>
#include <map>
#include <vector>
#include <string>
#include <gmtl/Math.h>
#include <gmtl/Vec.h>

#ifndef WIN32
  #include <stdint.h>
#else
  typedef unsigned __int64  uint64_t;
#endif

class NetMessage;

// Object Oriented Physics Simulation
namespace oops
{

class RigidBody;
class Geometry;
class Joint;
class ArticulatedBody;
class SimulationObjectInputListener;
class CollisionListenerInterface;

//******************************************************************************

struct ContactData
{
	RigidBody *body1, *body2;
	gmtl::Vec3f position, normal;
	float penetrationDepth;
}; // ContactData

//******************************************************************************

enum JOINT_METHODTYPE {

};

class MethodParameter {
public:
	virtual ~MethodParameter() {};
	virtual void encode(NetMessage* msg) = 0;
}; // MethodParameter

//******************************************************************************

class Simulation
{
public:
	// Enumeration used to distinguish between different stepping functions
	enum STEPFUNCTION
	{
		STEPFUNCTION_STEP,
		STEPFUNCTION_QUICKSTEP,
		STEPFUNCTION_STEPFAST1
	}; // STEPFUNCTION

public:
	Simulation();
	~Simulation();

	// setter for simulation specific data
	void activateCollisionDetection();
	void deactivateCollisionDetection();
	void setGlobalERP(float erp);
	void setGlobalCFM(float cfm);
	void setGravity(gmtl::Vec3f gravity);
	void setStepFunction(STEPFUNCTION function, unsigned numberOfSteps = 1);
	void setStepSize(float stepSize);
	void setGlobalAutoDisable(bool active);
	void setAutoDisableLinearThreshold(float threshold);
	void setAutoDisableAngularThreshold(float threshold);
	void setAutoDisableTime(float time);

	// method to activate or deactivate rendering of objects
	void setObjectsVisible(bool visible);

	// getter for simulation specific data
	const dWorldID getODEWorld();
	const dSpaceID getODESpace();
	float getGlobalERP();
	float getGlobalCFM();
	gmtl::Vec3f getGravity();
	float getStepSize();

	// methods to add or remove SimulationObjects from the Simulation
	bool addRigidBody(RigidBody* obj);
	bool addJoint(Joint* joint);
	bool addArticulatedBody(ArticulatedBody* obj);
	bool removeRigidBody(RigidBody* obj);
	bool removeRigidBodyWithID(uint64_t id);
	bool removeJoint(Joint* joint);
	bool removeJointWithID(uint64_t id);
	bool removeArticulatedBody(ArticulatedBody* obj,
		bool leaveRigidBodies = false);
	bool removeArticulatedBodyWithID(uint64_t id, bool leaveRigidBodies=false);

	void registerCollisionListener(CollisionListenerInterface* listener);

	// public collision detection and simulation methods
	void checkCollisionWithWorld(RigidBody* body,
		std::vector<ContactData*>& contacts);
	void simulate(float dt);
	void step(float dt);

	void renderObjects();

	// getter for registered SimulationObjects
	RigidBody* getRigidBodyByID(uint64_t id);
	RigidBody* getRigidBodyByName(std::string name);
	Joint* getJointByID(uint64_t id);
	Joint* getJointByName(std::string name);
	ArticulatedBody* getArticulatedBodyByID(uint64_t id);
	ArticulatedBody* getArticulatedBodyByName(std::string name);

protected:
	// internal collision detection methods
	static void checkCollisionHelp(void* data, dGeomID o1, dGeomID o2);
	static void checkCollisionWithWorldHelp(void* data, dGeomID o1, dGeomID o2);
	void checkCollisionWithWorld(dGeomID o1, dGeomID o2,
		std::vector<ContactData*>* contacts);
	void checkCollision(void* data, dGeomID o1, dGeomID o2);

private:
	// simulation specific members
	dWorldID									world;
	dSpaceID									space;
	dJointGroupID								contactGroup;
	STEPFUNCTION								function;
	// number of steps if STEPFAST1 is used as stepping function
	unsigned									numberOfSteps;
	float										timestep;
	gmtl::Vec3f									gravity;
	bool										doCollisionDetection;
	bool										objectsVisible;

	// lists and maps storing SimulationObjects
	std::vector<RigidBody*>						rigidBodyList;
	std::vector<Joint*>							jointList;
	std::vector<ArticulatedBody*>				articulatedBodyList;
	std::map<uint64_t, RigidBody*>				rigidBodyMap;
	std::map<uint64_t, Joint*>					jointMap;
	std::map<uint64_t, ArticulatedBody*>		articulatedBodyMap;
	std::vector<CollisionListenerInterface*>	collisionListenerList;

	// internal methods used by simulation loop
	void applyObjectForces();
	void updatePosAndRot();

	bool jointConnectionAvoidsCollision(RigidBody* body1, RigidBody* body2);
}; // Simulation

//*****************************************************************************

} // oops

#endif // _SIMULATION_H
