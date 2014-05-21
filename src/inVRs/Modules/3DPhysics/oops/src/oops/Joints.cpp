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

#include "oops/Joints.h"

#include <assert.h>
#include <limits>
#include "oops/Simulation.h"
#include "oops/RigidBody.h"
#include "oops/odeJoints/ODEJoints.h"
#include <gmtl/VecOps.h>
#include <inVRs/SystemCore/DebugOutput.h>

// Object Oriented Physics Simulation
namespace oops
{

//*****************************************************************************
// Begin Joint

Joint::Joint()
{
	joint = 0;
	obj1 = NULL;
	obj2 = NULL;
	avoidsCollision = false;
} // Joint

Joint::~Joint()
{
	if (joint != 0)
		dJointDestroy(joint);
} // Joint

void Joint::cloneData(Joint* dst)
{
	dst->id = this->id;
	dst->name = this->name;
	dst->avoidsCollision = avoidsCollision;
} // cloneData

void Joint::setObject1(RigidBody* obj1)
{
	this->obj1 = obj1;
} // setObject1

void Joint::setObject2(RigidBody* obj2)
{
	this->obj2 = obj2;
} // setObject2

RigidBody* Joint::getObject1()
{
	return obj1;
} // getObject1

RigidBody* Joint::getObject2()
{
	return obj2;
} // getObject2

dJointID Joint::getODEJoint()
{
	return joint;
} // getODEJoint

void Joint::setCollisionBehaviour(bool avoidsCollision) {
	this->avoidsCollision = avoidsCollision;
}

bool Joint::doesAvoidCollision() {
	return avoidsCollision;
} // doesAvoidCollision

void Joint::destroy()
{
	if (joint != 0)
	{
		dJointDestroy(joint);
		joint = 0;
	} // if
} // destroy

// End Joint
//*****************************************************************************

HingeJoint::HingeJoint(unsigned anchorObject, gmtl::Vec3f offset, gmtl::Vec3f axis)
{
	this->anchorObject = anchorObject;
	this->offset = offset;
	this->position = offset;
	this->axis = axis;
	maxAngleSet = false;
	minAngleSet = false;
	maxAngle = 0;
	minAngle = 0;
} // HingeJoint

HingeJoint::HingeJoint(unsigned id, RigidBody* obj1, RigidBody* obj2,
	gmtl::Vec3f position, gmtl::Vec3f axis)
{
	this->anchorObject = 0;
	this->id = id;
	this->obj1 = obj1;
	this->obj2 = obj2;
	this->position = position;
	this->axis = axis;
// TODO: check what happens with offset here!!!
	maxAngleSet = false;
	minAngleSet = false;
	maxAngle = 0;
	minAngle = 0;
} // HingeJoint

Joint* HingeJoint::clone()
{
	HingeJoint* result = new HingeJoint(anchorObject, offset, axis);
	Joint::cloneData(result);
	result->minAngleSet = minAngleSet;
	result->maxAngleSet = maxAngleSet;
	result->minAngle = minAngle;
	result->maxAngle = maxAngle;

	return result;
} // clone

void HingeJoint::build(dWorldID world)
{
	dBodyID body1=NULL;
	dBodyID body2=NULL;

	if (obj1 == NULL && obj2 == NULL)
		return;

	joint = dJointCreateHinge(world, NULL);

	if (obj1 != NULL)
		body1 = obj1->getODEBody();

	if (obj2 != NULL)
		body2 = obj2->getODEBody();

	if (anchorObject == 1 && obj1)
		position = offset + obj1->getTransformation().position;
	else if (anchorObject == 2 && obj2)
		position = offset + obj2->getTransformation().position;
	else
		printd(ERROR, "HingeJoint::build(): no anchorObject found!\n");

	dJointAttach(joint, body1, body2);
	dJointSetHingeAnchor(joint, position[0], position[1], position[2]);
	dJointSetHingeAxis(joint, axis[0], axis[1], axis[2]);
	if (minAngleSet)
		dJointSetHingeParam(joint, dParamLoStop, minAngle);
	if (maxAngleSet)
		dJointSetHingeParam(joint, dParamHiStop, maxAngle);

	dJointSetData(joint, this);
} // build

void HingeJoint::setODEParameter(int attrib, double value)
{
	dJointSetHingeParam(joint, attrib, value);
} // setAttrib

void HingeJoint::setMaxAngle(float angle) {
	maxAngle = angle;
	this->maxAngleSet = true;
} // setMaxAngle

void HingeJoint::setMinAngle(float angle) {
	minAngle = angle;
	this->minAngleSet = true;
} // setMinAngle

float HingeJoint::getMaxAngle() {
	if (maxAngleSet)
		return maxAngle;
	else
		return std::numeric_limits<float>::max();
} // getMaxAngle

float HingeJoint::getMinAngle() {
	if (minAngleSet)
		return minAngle;
	else
		return std::numeric_limits<float>::min();
} // getMinAngle

float HingeJoint::getAngle() {
	if (!joint)
		return 0;

	return dJointGetHingeAngle(joint);
} // getAngle

//*****************************************************************************

BallJoint::BallJoint(unsigned anchorObject, gmtl::Vec3f offset)
{
	this->anchorObject = anchorObject;
	this->offset = offset;
	this->position = offset;
} // BallJoint

BallJoint::BallJoint(unsigned id, RigidBody* obj1, RigidBody* obj2,
	gmtl::Vec3f position)
{
	this->anchorObject = 0;
	this->id = id;
	this->obj1 = obj1;
	this->obj2 = obj2;
	this->position = position;
// TODO: check what happens with offset here!!!
} // BallJoint

Joint* BallJoint::clone()
{
	BallJoint* result = new BallJoint(anchorObject, offset);
	Joint::cloneData(result);
	return result;
} // clone

void BallJoint::build(dWorldID world)
{
	dBodyID body1=NULL;
	dBodyID body2=NULL;

	if (obj1 == NULL && obj2 == NULL)
		return;

	joint = dJointCreateBall(world, NULL);

	if (obj1 != NULL)
		body1 = obj1->getODEBody();
	if (obj2 != NULL)
		body2 = obj2->getODEBody();

	if (anchorObject == 1 && obj1)
		position = offset + obj1->getTransformation().position;
	else if (anchorObject == 2 && obj2)
		position = offset + obj2->getTransformation().position;

	dJointAttach(joint, body1, body2);
	dJointSetBallAnchor(joint, position[0], position[1], position[2]);

	dJointSetData(joint, this);
} // build

void BallJoint::setODEParameter(int attrib, double value)
{
	// THIS JOINT DOES NOT SUPPORT PARAMETERS
	assert(false);
} // setAttrib

//*****************************************************************************

SliderJoint::SliderJoint(gmtl::Vec3f axis)
{
	this->axis = axis;
	this->minBoundarySet = false;
	this->maxBoundarySet = false;
	this->minBoundary = 0;
	this->maxBoundary = 0;
} // SliderJoint

SliderJoint::SliderJoint(unsigned id, RigidBody* obj1, RigidBody* obj2,
		gmtl::Vec3f axis)
{
	this->id = id;
	this->obj1 = obj1;
	this->obj2 = obj2;
	this->axis = axis;
        this->minBoundarySet = false;
        this->maxBoundarySet = false;
        this->minBoundary = 0;
        this->maxBoundary = 0;
} // SliderJoint

Joint* SliderJoint::clone()
{
	SliderJoint* result = new SliderJoint(axis);
	Joint::cloneData(result);
	result->minBoundarySet = minBoundarySet;
	result->maxBoundarySet = maxBoundarySet;
	result->minBoundary = minBoundary;
	result->maxBoundary = maxBoundary;
	return result;
} // clone

void SliderJoint::build(dWorldID world)
{
	dBodyID body1 = NULL;
	dBodyID body2 = NULL;

	if (obj1 == NULL && obj2 == NULL)
		return;

	joint = dJointCreateSlider(world, NULL);

	if (obj1 != NULL)
		body1 = obj1->getODEBody();
	if (obj2 != NULL)
		body2 = obj2->getODEBody();

	dJointAttach(joint, body1, body2);
	dJointSetSliderAxis(joint, axis[0], axis[1], axis[2]);

	if (minBoundarySet)
		dJointSetSliderParam(joint, dParamLoStop, minBoundary);
	if (maxBoundarySet)
		dJointSetSliderParam(joint, dParamHiStop, maxBoundary);

	dJointSetData(joint, this);
} // build

gmtl::Vec3f& SliderJoint::getAxis(gmtl::Vec3f& dst)
{
	dst = axis;
	return dst;
} // getAxis


const gmtl::Vec3f& SliderJoint::getAxis()
{
	return axis;
} // getAxis

float SliderJoint::getPosition()
{
	if (!joint)
		return 0;

	return dJointGetSliderPosition(joint);
} // getPosition

void SliderJoint::setODEParameter(int attrib, double value)
{
	dJointSetSliderParam(joint, attrib, value);
} // setAttrib

void SliderJoint::setMinBoundary(float min) {
	minBoundary = min;
	minBoundarySet = true;
} // setMinBoundary

void SliderJoint::setMaxBoundary(float max) {
	maxBoundary = max;
	maxBoundarySet = true;
} // setMaxBoundary

float SliderJoint::getMinBoundary() {
	if (minBoundarySet)
		return minBoundary;
	else
		return std::numeric_limits<float>::min();
} // setMinBoundary

float SliderJoint::getMaxBoundary() {
	if (maxBoundarySet)
		return maxBoundary;
	else
		return std::numeric_limits<float>::max();
} // setMaxBoundary


//*****************************************************************************

UniversalJoint::UniversalJoint(unsigned anchorObject, gmtl::Vec3f offset, gmtl::Vec3f axis1, gmtl::Vec3f axis2)
{
	this->anchorObject = anchorObject;
	this->offset = offset;
	this->position = offset;
	this->axis1 = axis1;
	this->axis2 = axis2;
} // UniversalJoint

UniversalJoint::UniversalJoint(unsigned id, RigidBody* obj1, RigidBody* obj2,
	gmtl::Vec3f position, gmtl::Vec3f axis1, gmtl::Vec3f axis2)
{
	this->anchorObject = 0;
	this->id = id;
	this->obj1 = obj1;
	this->obj2 = obj2;
	this->position = position;
	this->axis1 = axis1;
	this->axis2 = axis2;
} // UniversalJoint

Joint* UniversalJoint::clone()
{
	UniversalJoint* result = new UniversalJoint(anchorObject, offset, axis1, axis2);
	Joint::cloneData(result);
	return result;
} // clone

void UniversalJoint::build(dWorldID world)
{
	dBodyID body1 = NULL;
	dBodyID body2 = NULL;

	if (obj1 == NULL && obj2 == NULL)
		return;

	joint = dJointCreateUniversal(world, NULL);

	if (obj1 != NULL)
		body1 = obj1->getODEBody();
	if (obj2 != NULL)
		body2 = obj2->getODEBody();

	if (anchorObject == 1 && obj1)
		position = offset + obj1->getTransformation().position;
	else if (anchorObject == 2 && obj2)
		position = offset + obj2->getTransformation().position;

	dJointAttach(joint, body1, body2);
	dJointSetUniversalAnchor(joint, position[0], position[1], position[2]);
	dJointSetUniversalAxis1(joint, axis1[0], axis1[1], axis1[2]);
	dJointSetUniversalAxis2(joint, axis2[0], axis2[1], axis2[2]);

	dJointSetData(joint, this);
} // build

void UniversalJoint::setODEParameter(int attrib, double value)
{
	dJointSetUniversalParam(joint, attrib, value);
} // setAttrib

//*****************************************************************************

Hinge2Joint::Hinge2Joint(unsigned anchorObject, gmtl::Vec3f offset, gmtl::Vec3f axis1, gmtl::Vec3f axis2)
{
	this->anchorObject = anchorObject;
	this->offset = offset;
	this->position = offset;
	this->axis1 = axis1;
	this->axis2 = axis2;
} // Hinge2Joint

Hinge2Joint::Hinge2Joint(unsigned id, RigidBody* obj1, RigidBody* obj2,
	gmtl::Vec3f position, gmtl::Vec3f axis1, gmtl::Vec3f axis2)
{
	this->anchorObject = 0;
	this->id = id;
	this->obj1 = obj1;
	this->obj2 = obj2;
	this->position = position;
	this->axis1 = axis1;
	this->axis2 = axis2;
} // Hinge2Joint

Joint* Hinge2Joint::clone()
{
	Hinge2Joint* result = new Hinge2Joint(anchorObject, offset, axis1, axis2);
	Joint::cloneData(result);
	return result;
} // clone

void Hinge2Joint::build(dWorldID world)
{
	dBodyID body1=NULL;
	dBodyID body2=NULL;

	if (obj1 == NULL && obj2 == NULL)
		return;

	joint = dJointCreateHinge2(world, NULL);

	if (obj1 != NULL)
		body1 = obj1->getODEBody();
	if (obj2 != NULL)
		body2 = obj2->getODEBody();

	if (anchorObject == 1 && obj1)
		position = offset + obj1->getTransformation().position;
	else if (anchorObject == 2 && obj2)
		position = offset + obj2->getTransformation().position;

	dJointAttach(joint, body1, body2);
	dJointSetHinge2Anchor(joint, position[0], position[1], position[2]);
	dJointSetHinge2Axis1(joint, axis1[0], axis1[1], axis1[2]);
	dJointSetHinge2Axis2(joint, axis2[0], axis2[1], axis2[2]);

	dJointSetData(joint, this);
} // build

gmtl::Vec3f& Hinge2Joint::getAnchor1(gmtl::Vec3f& dst)
{
	assert(joint);
	dVector3 result;
	dJointGetHinge2Anchor(joint, result);
	dst = gmtl::Vec3f(result[0], result[1], result[2]);
	return dst;
} // getAnchor1

gmtl::Vec3f& Hinge2Joint::getAnchor2(gmtl::Vec3f& dst)
{
	assert(joint);
	dVector3 result;
	dJointGetHinge2Anchor2(joint, result);
	dst = gmtl::Vec3f(result[0], result[1], result[2]);
	return dst;
} // getAnchor2

void Hinge2Joint::setODEParameter(int attrib, double value)
{
	dJointSetHinge2Param(joint, attrib, value);
} // setAttrib

//*****************************************************************************

FixedJoint::FixedJoint()
{

} // FixedJoint

FixedJoint::FixedJoint(unsigned id, RigidBody* obj1, RigidBody* obj2)
{
	this->id = id;
	this->obj1 = obj1;
	this->obj2 = obj2;
} // FixedJoint

Joint* FixedJoint::clone()
{
	FixedJoint* result = new FixedJoint;
	Joint::cloneData(result);
	return result;
} // clone

void FixedJoint::build(dWorldID world)
{
	dBodyID body1=NULL;
	dBodyID body2=NULL;

	if (obj1 == NULL && obj2 == NULL)
		return;

	joint = dJointCreateFixed(world, NULL);

	if (obj1 != NULL)
		body1 = obj1->getODEBody();
	if (obj2 != NULL)
		body2 = obj2->getODEBody();

	dJointAttach(joint, body1, body2);
	dJointSetFixed(joint);

	dJointSetData(joint, this);
} // build

void FixedJoint::setODEParameter(int attrib, double value)
{
	// THIS JOINT DOES NOT SUPPORT PARAMETERS
	assert(false);
} // setAttrib

//*****************************************************************************

AMotorJoint::AMotorJoint(int axis1Obj, gmtl::Vec3f axis1)
{
	this->axis1Obj = axis1Obj;
	this->axis1 = axis1;
	this->numberOfAxis = 1;
} // AMotorJoint

AMotorJoint::AMotorJoint(int axis1Obj, gmtl::Vec3f axis1, int axis2Obj, gmtl::Vec3f axis2,
	int mode)
{
	this->axis1Obj = axis1Obj;
	this->axis1 = axis1;
	this->axis2Obj = axis2Obj;
	this->axis2 = axis2;
	this->numberOfAxis = 2;
	this->mode = mode;
} // AMotorJoint

AMotorJoint::AMotorJoint(int axis1Obj, gmtl::Vec3f axis1, int axis2Obj, gmtl::Vec3f axis2,
	int axis3Obj, gmtl::Vec3f axis3, int mode)
{
	this->axis1Obj = axis1Obj;
	this->axis1 = axis1;
	this->axis2Obj = axis2Obj;
	this->axis2 = axis2;
	this->axis3Obj = axis3Obj;
	this->axis3 = axis3;
	this->numberOfAxis = 3;
	this->mode = mode;
} // AMotorJoint

AMotorJoint::AMotorJoint(unsigned id, RigidBody* obj1, RigidBody* obj2,
	int axis1Obj, gmtl::Vec3f axis1)
{
	this->id = id;
	this->obj1 = obj1;
	this->obj2 = obj2;
	this->axis1Obj = axis1Obj;
	this->axis1 = axis1;
	this->numberOfAxis = 1;
} // AMotorJoint

AMotorJoint::AMotorJoint(unsigned id, RigidBody* obj1, RigidBody* obj2, int axis1Obj,
		gmtl::Vec3f axis1, int axis2Obj, gmtl::Vec3f axis2,
		int mode)
{
	this->id = id;
	this->obj1 = obj1;
	this->obj2 = obj2;
	this->axis1Obj = axis1Obj;
	this->axis1 = axis1;
	this->axis2Obj = axis2Obj;
	this->axis2 = axis2;
	this->numberOfAxis = 2;
	this->mode = mode;
} // AMotorJoint

AMotorJoint::AMotorJoint(unsigned id, RigidBody* obj1, RigidBody* obj2, int axis1Obj,
		gmtl::Vec3f axis1, int axis2Obj, gmtl::Vec3f axis2, int axis3Obj,
		gmtl::Vec3f axis3, int mode)
{
	this->id = id;
	this->obj1 = obj1;
	this->obj2 = obj2;
	this->axis1Obj = axis1Obj;
	this->axis1 = axis1;
	this->axis2Obj = axis2Obj;
	this->axis2 = axis2;
	this->axis3Obj = axis3Obj;
	this->axis3 = axis3;
	this->numberOfAxis = 3;
	this->mode = mode;
} // AMotorJoint

Joint* AMotorJoint::clone()
{
	AMotorJoint* result = NULL;
	if (numberOfAxis == 1)
		result = new AMotorJoint(axis1Obj, axis1);
	else if (numberOfAxis == 2)
		result = new AMotorJoint(axis1Obj, axis1, axis2Obj, axis2, mode);
	else if (numberOfAxis == 3)
		result = new AMotorJoint(axis1Obj, axis1, axis2Obj, axis2, axis3Obj, axis3, mode);
	else
		assert(false);

	Joint::cloneData(result);
	return result;
} // clone

void AMotorJoint::build(dWorldID world)
{
	dBodyID body1=NULL;
	dBodyID body2=NULL;

	if (obj1 == NULL && obj2 == NULL)
		return;

	joint = dJointCreateAMotor(world, NULL);

	if (obj1 != NULL)
		body1 = obj1->getODEBody();
	if (obj2 != NULL)
		body2 = obj2->getODEBody();

	dJointAttach(joint, body1, body2);
	if (numberOfAxis == 1)
	{
		dJointSetAMotorMode(joint, dAMotorUser);
		dJointSetAMotorNumAxes(joint, 1);
		dJointSetAMotorAxis(joint, 0, axis1Obj, axis1[0], axis1[1], axis1[2]);
	} else if (numberOfAxis == 2)
	{
		if (mode == dAMotorEuler && (axis1Obj != 1 || axis2Obj != 2))
			assert(false);
		if (mode != dAMotorEuler)
			mode = dAMotorUser;
		dJointSetAMotorMode(joint, mode);
		if (mode != dAMotorEuler)
			dJointSetAMotorNumAxes(joint, 2);
		else
			dJointSetAMotorNumAxes(joint, 3);
		dJointSetAMotorAxis(joint, 0, axis1Obj, axis1[0], axis1[1], axis1[2]);
		dJointSetAMotorAxis(joint, 1, axis2Obj, axis2[0], axis2[1], axis2[2]);
	} else //	if (numberOfAxis == 3)
	{
		if (mode == dAMotorEuler && (axis1Obj != 1 || axis2Obj != 2))
			assert(false);
		if (mode != dAMotorEuler)
			mode = dAMotorUser;

		dJointAttach(joint, body1, body2);
		dJointSetAMotorMode(joint, mode);
		dJointSetAMotorNumAxes(joint, 3);
		dJointSetAMotorAxis(joint, 0, axis1Obj, axis1[0], axis1[1], axis1[2]);
		dJointSetAMotorAxis(joint, 1, axis2Obj, axis2[0], axis2[1], axis2[2]);
		if (mode != dAMotorEuler)
			dJointSetAMotorAxis(joint, 2, axis3Obj, axis3[0], axis3[1], axis3[2]);
	} // else

	dJointSetData(joint, this);
} // build


void AMotorJoint::setODEParameter(int attrib, double value)
{
	dJointSetAMotorParam(joint, attrib, value);
} // setAttrib

//*****************************************************************************

SpringDamperJoint::SpringDamperJoint(unsigned anchorObject)
{
	this->anchorObject = anchorObject;
	linearSpringConstant = 1.0f;
	linearDampingConstant = 100.0f;
	angularSpringConstant = 0.01f;
	angularDampingConstant = 10.f;
} // SpringDamperJoint

SpringDamperJoint::SpringDamperJoint(unsigned id, RigidBody* obj1, RigidBody* obj2)
{
	this->anchorObject = 0;
	this->id = id;
	this->obj1 = obj1;
	this->obj2 = obj2;
        linearSpringConstant = 1.0f;
        linearDampingConstant = 100.0f;
        angularSpringConstant = 0.01f;
        angularDampingConstant = 10.f;
} // HingeJoint

Joint* SpringDamperJoint::clone()
{
	SpringDamperJoint* result = new SpringDamperJoint(anchorObject);
	Joint::cloneData(result);

	return result;
} // clone

void SpringDamperJoint::build(dWorldID world)
{
	dBodyID body1=NULL;
	dBodyID body2=NULL;

	if (obj1 == NULL && obj2 == NULL)
		return;

	joint = dJointCreateSpring(world, NULL);

	if (obj1 != NULL)
		body1 = obj1->getODEBody();

	if (obj2 != NULL)
		body2 = obj2->getODEBody();

	if (anchorObject == 1)
		dJointAttach(joint, body1, body2);
	else
		dJointAttach(joint, body2, body1);

	dJointSetSpring(joint);
	dJointSetSpringLinearConstants(joint, linearSpringConstant, linearDampingConstant);
	dJointSetSpringAngularConstants(joint, angularSpringConstant, angularDampingConstant);

	dJointSetData(joint, this);
} // build

void SpringDamperJoint::setLinearConstants(float springConstant, float dampingConstant) {
	linearSpringConstant = springConstant;
	linearDampingConstant = dampingConstant;

	if (joint)
		dJointSetSpringLinearConstants(joint, linearSpringConstant, linearDampingConstant);
} // setLinearConstants

void SpringDamperJoint::setAngularConstants(float springConstant, float dampingConstant) {
	angularSpringConstant = springConstant;
	angularDampingConstant = dampingConstant;

	if (joint)
		dJointSetSpringAngularConstants(joint, angularSpringConstant, angularDampingConstant);
} // setAngularConstants

void SpringDamperJoint::setODEParameter(int attrib, double value)
{
	fprintf(stderr, "SpringDamperJoint::setODEParameter(): NOT SUPPORTED BY SPRINGDAMPERJOINT!!!\n");
} // setAttrib

//*****************************************************************************

} // oops
