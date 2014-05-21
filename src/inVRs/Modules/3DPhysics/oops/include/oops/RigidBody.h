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

#ifndef _RIGIDBODY_H
#define _RIGIDBODY_H

#include <inVRs/SystemCore/DataTypes.h>
#include <ode/ode.h>
#include "Interfaces/TransformationWriterInterface.h"
#include "Interfaces/RendererInterface.h"
#include "Interfaces/SimulationObjectInterface.h"
#include "Interfaces/CollisionListenerInterface.h"
#include "FrictionValues.h"

// Object Oriented Physics Simulation
namespace oops
{

class Simulation;
class Geometry;
class ArticulatedBody;
class SimulationObjectInputListener;

//*****************************************************************************

/**
 *
 */
class RigidBody : public SimulationObjectInterface
{
public:

//*****************//
// PUBLIC METHODS: //
//*****************//

	RigidBody(Geometry* geometry);
	RigidBody(Geometry* geometry,
		TransformationWriterInterface* transformWriter);
	RigidBody(Geometry* geometry, RendererInterface* renderer);
	RigidBody(Geometry* geometry,
		TransformationWriterInterface* transformWriter,
		RendererInterface* renderer);
	virtual ~RigidBody();
	virtual RigidBody* clone();

	virtual void addCollisionListener(CollisionListenerInterface* listener);
	virtual bool removeCollisionListener(CollisionListenerInterface* listener);
	virtual void setTransformationWriter(TransformationWriterInterface* writer);
	virtual void setRenderer(RendererInterface* renderer);
	virtual void setOffsetTransformation(TransformationData& offset);
	virtual void setUserData(void* data);

	virtual void finalizeIntialization();

	virtual bool isActive();
	virtual bool isVisible();
	virtual bool isFixed();
	virtual TransformationData& getTransformation(TransformationData& dst);
	virtual const TransformationData& getTransformation();
	virtual const TransformationData& getOffsetTransformation();
	virtual TransformationWriterInterface* getTransformationWriter();
	virtual float getMass();
	virtual gmtl::Matrix33f getInertiaTensor();
	virtual gmtl::Vec3f& getStaticForce(gmtl::Vec3f& dst);
	virtual const gmtl::Vec3f& getStaticForce();
	virtual gmtl::Vec3f& getStaticTorque(gmtl::Vec3f& dst);
	virtual const gmtl::Vec3f& getStaticTorque();
	virtual gmtl::Vec3f& getLinearVelocity(gmtl::Vec3f& dst);
	virtual gmtl::Vec3f& getAngularVelocity(gmtl::Vec3f& dst);
	virtual gmtl::Vec3f& getForce(gmtl::Vec3f& dst);
	virtual gmtl::Vec3f& getTorque(gmtl::Vec3f& dst);
	virtual gmtl::Vec3f& getBodyPositionInWorld(gmtl::Vec3f& dst, gmtl::Vec3f& src);
	virtual gmtl::Vec3f& getWorldPositionInBody(gmtl::Vec3f& dst, gmtl::Vec3f& src);
	virtual gmtl::Vec3f& getPointVelocity(gmtl::Vec3f& dst, gmtl::Vec3f& position, bool relative = false);
	virtual gmtl::Vec3f& convertWorldToBody(gmtl::Vec3f& src, gmtl::Vec3f& dst);
	virtual gmtl::Vec3f& convertBodyToWorld(gmtl::Vec3f& src, gmtl::Vec3f& dst);
	virtual bool getGravityMode();
	virtual void* getUserData();

	virtual Geometry* getGeometry();
	virtual dBodyID getODEBody();
	virtual dGeomID getODEGeometry();

	virtual std::string getType();

	virtual void collideWithRigidBody(RigidBody* obj);
	virtual void dontCollideWithRigidBody(RigidBody* obj);

	static void setInputListener(SimulationObjectInputListener* listener);
	static SimulationObjectInputListener* getInputListener();

//********************************//
// PUBLIC SYNCHRONIZABLE METHODS: //
//********************************//

	virtual void setActive(bool active);
	virtual void setVisible(bool visible);
	virtual void setTransformation(TransformationData& trans, bool forwardToWriter = false);
	virtual void setMass(float mass);
	virtual void addForce(gmtl::Vec3f& force, bool relative = false);
	virtual void addTorque(gmtl::Vec3f& torque, bool relative = false);
	virtual void addForceAtPosition(gmtl::Vec3f& force, gmtl::Vec3f& position, bool relativeForce = false, bool relativePosition = false);
	virtual void setForce(gmtl::Vec3f& force);
	virtual void setTorque(gmtl::Vec3f& torque);
	virtual void setStaticForce(gmtl::Vec3f& force);
	virtual void setStaticTorque(gmtl::Vec3f& torque);
	virtual void setLinearVelocity(gmtl::Vec3f velocity, bool relative = false);
	virtual void setAngularVelocity(gmtl::Vec3f velocity, bool relative = false);
	virtual void setFixed(bool fixed); //, dWorldID world = 0);
	virtual void setGravityMode(bool onOff);
	virtual void setActive_unchecked(bool active);
	virtual void setVisible_unchecked(bool visible);
	virtual void setTransformation_unchecked(TransformationData& trans, bool forwardToWriter = false);
	virtual void setMass_unchecked(float mass);
	virtual void addForce_unchecked(gmtl::Vec3f& force, bool relative = false);
	virtual void addTorque_unchecked(gmtl::Vec3f& torque, bool relative = false);
	virtual void addForceAtPosition_unchecked(gmtl::Vec3f& force, gmtl::Vec3f& position, bool relativeForce = false, bool relativePosition = false);
	virtual void setForce_unchecked(gmtl::Vec3f& force);
	virtual void setTorque_unchecked(gmtl::Vec3f& torque);
	virtual void setStaticForce_unchecked(gmtl::Vec3f& force);
	virtual void setStaticTorque_unchecked(gmtl::Vec3f& torque);
	virtual void setLinearVelocity_unchecked(gmtl::Vec3f velocity, bool relative = false);
	virtual void setAngularVelocity_unchecked(gmtl::Vec3f velocity, bool relative = false);
	virtual void setFixed_unchecked(bool fixed); //, dWorldID world = 0);
	virtual void setGravityMode_unchecked(bool onOff);
	virtual void setColliding(bool doesCollide);

	virtual void updateVisualRepresentation();

protected:

	friend class Simulation;
	friend class ArticulatedBody;

//********************//
// PROTECTED METHODS: //
//********************//

	virtual void initialize(Geometry* geometry,
		TransformationWriterInterface* transformWriter,
		RendererInterface* renderer);
	virtual void updateTransformationData();
	virtual void setODETransformation();
	virtual void setODEMass();

// Methods derived from SimulationObject
	virtual void build(dWorldID world, dSpaceID space = 0);
	virtual void destroy();

	virtual void applyForce();
	virtual void updateTransformation();
	virtual bool canCollideWith(RigidBody* obj);
	virtual bool render();

// Methods called by other RigidBody
	virtual void collideWithRigidBodyInternal(RigidBody* obj);
	virtual void dontCollideWithRigidBodyInternal(RigidBody* obj);

	virtual void collisionOccured(RigidBody* opponent, std::vector<ContactData>& contacts);

//********************//
// PROTECTED MEMBERS: //
//********************//

	std::string className;

	bool built;
	bool active;
	bool visible;
	bool fixed;
	bool staticForceSet;
	bool ignoreTransformation;
	bool gravityMode;
	bool colliding;

	float mass;
	Geometry* geometry;
	RendererInterface* renderer;

	TransformationData transformation;
	TransformationData offsetTransformation;
	TransformationWriterInterface* transformWriter;

	gmtl::Vec3f staticForce, staticTorque;
	gmtl::Vec3f initialLinVel, initialAngVel;

	dWorldID world;
	dSpaceID space;
	dBodyID body;

	std::vector<RigidBody*> collisionList;
	std::vector<RigidBody*> ignoreCollisionList;
	void* userData;

	std::vector<CollisionListenerInterface*> collisionListener;

	static SimulationObjectInputListener* inputListener;

}; // RigidBody

//*****************************************************************************

} // oops

#endif //_RIGIDBODY_H
