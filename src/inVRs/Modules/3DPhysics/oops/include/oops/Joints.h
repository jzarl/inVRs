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

#ifndef _JOINTS_H_
#define _JOINTS_H_

#include <ode/ode.h>
#include "gmtl/Vec.h"
#include <string>
#include "Interfaces/SimulationObjectInterface.h"

// Object Oriented Physics Simulation
namespace oops
{

class RigidBody;

//*****************************************************************************

class Joint : public SimulationObjectInterface
{
public:
	Joint();
	virtual ~Joint();

	virtual Joint* clone() = 0;

	dJointID getODEJoint();

// TODO: maybe hide the following three setter (protected)
	virtual void setObject1(RigidBody* obj1);
	virtual void setObject2(RigidBody* obj2);

	virtual RigidBody* getObject1();
	virtual RigidBody* getObject2();

	virtual void setODEParameter(int attrib, double value) = 0;

	void setCollisionBehaviour(bool avoidsCollision);

protected:
	friend class Simulation;
	dJointID joint;
	RigidBody *obj1, *obj2;
	bool avoidsCollision;

	virtual void cloneData(Joint* dst);

	virtual bool doesAvoidCollision();

// Method derived from SimulationObjectInterface
	virtual void build(dWorldID world) = 0;
	virtual void destroy();
}; // Joints

//*****************************************************************************

class HingeJoint : public Joint
{
public:
	HingeJoint(unsigned anchorObject, gmtl::Vec3f offset, gmtl::Vec3f axis);
	HingeJoint(unsigned id, RigidBody* obj1, RigidBody* obj2, gmtl::Vec3f position,
		gmtl::Vec3f axis);
	virtual Joint* clone();

	virtual void setODEParameter(int attrib, double value);
	virtual void build(dWorldID world);

	virtual void setMaxAngle(float angle);
	virtual void setMinAngle(float angle);
	float getMaxAngle();
	float getMinAngle();

	virtual float getAngle();

protected:
	unsigned anchorObject;
	gmtl::Vec3f offset;
	gmtl::Vec3f position, axis;
	bool maxAngleSet, minAngleSet;
	float maxAngle, minAngle;
}; // HingeJoint

//*****************************************************************************

class BallJoint : public Joint
{
protected:
	unsigned anchorObject;
	gmtl::Vec3f offset;
	gmtl::Vec3f position;

public:
	BallJoint(unsigned anchorObject, gmtl::Vec3f offset);
	BallJoint(unsigned id, RigidBody* obj1, RigidBody* obj2, gmtl::Vec3f position);
	virtual Joint* clone();

	virtual void setODEParameter(int attrib, double value);
	virtual void build(dWorldID world);
}; // BallJoint

//*****************************************************************************

class SliderJoint : public Joint
{
protected:
	gmtl::Vec3f axis;
	bool minBoundarySet, maxBoundarySet;
	float minBoundary, maxBoundary;

public:
	SliderJoint(gmtl::Vec3f axis);
	SliderJoint(unsigned id, RigidBody* obj1, RigidBody* obj2, gmtl::Vec3f axis);
	virtual Joint* clone();

	virtual void setODEParameter(int attrib, double value);
	virtual void build(dWorldID world);
	gmtl::Vec3f& getAxis(gmtl::Vec3f& dst);
	const gmtl::Vec3f& getAxis();
	float getPosition();

	virtual void setMinBoundary(float min);
	virtual void setMaxBoundary(float max);
	float getMinBoundary();
	float getMaxBoundary();

}; // SliderJoint

//*****************************************************************************

class UniversalJoint : public Joint
{
protected:
	unsigned anchorObject;
	gmtl::Vec3f offset;
	gmtl::Vec3f position, axis1, axis2;

public:
	UniversalJoint(unsigned anchorObject, gmtl::Vec3f offset, gmtl::Vec3f axis1, gmtl::Vec3f axis2);
	UniversalJoint(unsigned id, RigidBody* obj1, RigidBody* obj2, gmtl::Vec3f position,
		gmtl::Vec3f axis1, gmtl::Vec3f axis2);
	virtual Joint* clone();

	virtual void setODEParameter(int attrib, double value);
	virtual void build(dWorldID world);
}; //UniversalJoint

//*****************************************************************************

class Hinge2Joint : public Joint
{
protected:
	unsigned anchorObject;
	gmtl::Vec3f offset;
	gmtl::Vec3f position, axis1, axis2;

public:
	Hinge2Joint(unsigned anchorObject, gmtl::Vec3f offset, gmtl::Vec3f axis1, gmtl::Vec3f axis2);
	Hinge2Joint(unsigned id, RigidBody* obj1, RigidBody* obj2, gmtl::Vec3f position,
		gmtl::Vec3f axis1, gmtl::Vec3f axis2);
	virtual Joint* clone();

	virtual void setODEParameter(int attrib, double value);
	virtual void build(dWorldID world);
	gmtl::Vec3f& getAnchor1(gmtl::Vec3f& dst);
	gmtl::Vec3f& getAnchor2(gmtl::Vec3f& dst);
}; // Hinge2Joint

//*****************************************************************************

class FixedJoint : public Joint
{
public:
	FixedJoint();
	FixedJoint(unsigned id, RigidBody* obj1, RigidBody* obj2);
	virtual Joint* clone();

	virtual void setODEParameter(int attrib, double value);
	virtual void build(dWorldID world);
}; // FixedJoint

//*****************************************************************************

class AMotorJoint : public Joint
{
protected:
	int numberOfAxis;
	gmtl::Vec3f axis1, axis2, axis3;
	int axis1Obj, axis2Obj, axis3Obj;
	int mode;

public:
	AMotorJoint(int axis1Obj, gmtl::Vec3f axis1);
	AMotorJoint(int axis1Obj, gmtl::Vec3f axis1, int axis2Obj,
		gmtl::Vec3f axis2, int mode = dAMotorUser);
	AMotorJoint(int axis1Obj, gmtl::Vec3f axis1, int axis2Obj,
		gmtl::Vec3f axis2, int axis3Obj, gmtl::Vec3f axis3,
		int mode = dAMotorUser);

	AMotorJoint(unsigned id, RigidBody* obj1, RigidBody* obj2, int axis1Obj,
		gmtl::Vec3f axis1);
	AMotorJoint(unsigned id, RigidBody* obj1, RigidBody* obj2, int axis1Obj,
		gmtl::Vec3f axis1, int axis2Obj, gmtl::Vec3f axis2,
		int mode = dAMotorUser);
	AMotorJoint(unsigned id, RigidBody* obj1, RigidBody* obj2, int axis1Obj,
		gmtl::Vec3f axis1, int axis2Obj, gmtl::Vec3f axis2, int axis3Obj,
		gmtl::Vec3f axis3, int mode = dAMotorUser);
	virtual Joint* clone();

	virtual void setODEParameter(int attrib, double value);
	virtual void build(dWorldID world);
}; // AMotorJoint

//*****************************************************************************

class SpringDamperJoint : public Joint
{
protected:
	unsigned anchorObject;
	float linearSpringConstant, linearDampingConstant;
	float angularSpringConstant, angularDampingConstant;

public:
	SpringDamperJoint(unsigned anchorObject);
	SpringDamperJoint(unsigned id, RigidBody* obj1, RigidBody* obj2);
	virtual Joint* clone();

	virtual void setODEParameter(int attrib, double value);
	virtual void build(dWorldID world);

	virtual void setLinearConstants(float springConstant, float dampingConstant);
	virtual void setAngularConstants(float springConstant, float dampingConstant);
}; // SpringDamperJoint

//*****************************************************************************
} // oops

#endif //_JOINTS_H_
