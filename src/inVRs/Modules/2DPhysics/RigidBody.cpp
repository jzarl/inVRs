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

#include "RigidBody.h"

#include "CollisionMap/CollisionObject.h"
#include <inVRs/SystemCore/WorldDatabase/Entity.h>
#include <inVRs/SystemCore/DebugOutput.h>
#include <gmtl/Generate.h>
#include <gmtl/VecOps.h>

#include <assert.h>

const gmtl::Vec3f RigidBody::RIGHT = gmtl::Vec3f(1, 0, 0);
const gmtl::Vec3f RigidBody::FORWARD = gmtl::Vec3f(0, 0, 1);
const gmtl::Vec3f RigidBody::UP = gmtl::Vec3f(0, 1, 0);

RigidBody::RigidBody()
{
	x = gmtl::Vec3f(0, 0, 0);
	v = gmtl::Vec3f(0, 0, 0);
	force = gmtl::Vec3f(0, 0, 0);
	torque = gmtl::Vec3f(0,0,0);
	constantForce = gmtl::Vec3f(0,0,0);
	constantTorque = gmtl::Vec3f(0,0,0);
//	levitation = 0;
	massInv = 1;
	orientation = gmtl::QUAT_IDENTITYF;
	right = RIGHT;
	forward = FORWARD;
	up = UP;
	shape = NULL;
	boundingCircleRadius = -1;
	entity = NULL;
	notifyAtCollision = false;
} // RigidBody

RigidBody::~RigidBody()
{
	// Nothing to do here
} // RigidBody

void RigidBody::setPos(gmtl::Vec3f pos)
{
	x = pos;
} // setPos

void RigidBody::setOrientation(gmtl::Quatf orientation)
{
	this->orientation = orientation;
	forward = orientation * FORWARD;
	up = orientation * UP;
	right = orientation * RIGHT;
} // setOrientation

void RigidBody::setForward(gmtl::Vec3f forward)
{
	assert(false);
	this->forward = forward;
} // setForward

void RigidBody::setUp(gmtl::Vec3f up)
{
	this->up = up;
	gmtl::normalize(this->up);
	adjustOrientation();
} // setUp

void RigidBody::setRight(gmtl::Vec3f right)
{
	assert(false);
	this->right = right;
} // setRight

void RigidBody::setVel(gmtl::Vec3f vel)
{
	this->v = vel;
} // setVel

void RigidBody::setAngularVel(gmtl::Vec3f angVel)
{
	this->angV = angVel;
} // setAngularVel

void RigidBody::setMass(float mass)
{
	massInv = 1 / mass;
} // setMass

void RigidBody::setShape(CollisionObject* shape)
{
	this->shape = shape;
} // setShape

void RigidBody::setConstantForce(gmtl::Vec3f force)
{
	this->force = this->force - this->constantForce + force;
	this->constantForce = force;
} // setConstantForce

void RigidBody::setConstantTorque(gmtl::Vec3f torque)
{
	this->torque = this->torque - this->constantTorque + torque;
	this->constantTorque = torque;
} // setConstantTorque

void RigidBody::setBoundingCircleRadius(float radius)
{
	this->boundingCircleRadius = radius;
} // setBoundingCircleRadius

void RigidBody::setMovable(bool movable)
{
	this->movable = movable;
} // setMovable

void RigidBody::setEntity(Entity* entity)
{
	this->entity = entity;
} // setEntity

void RigidBody::addForce(gmtl::Vec3f force)
{
	this->force += force;
} // addForce

void RigidBody::addTorque(gmtl::Vec3f torque)
{
	this->torque += torque;
} // addTorque

float RigidBody::getMassInv()
{
	return massInv;
} // getMassInv

// TODO: Check this method!!!
gmtl::Vec2f RigidBody::get2DPos()
{
// 	gmtl::Vec3f realPos = x + (levitation - pivotOffset)*up;
	gmtl::Vec3f realPos = x;
	return gmtl::Vec2f(realPos[0], realPos[2]);
} // get2DPos

gmtl::Vec3f RigidBody::getPos()
{
	return x;
} // getPos

gmtl::Vec3f RigidBody::getCollisionV(gmtl::Vec3f collDir)
{
	gmtl::Vec3f result = collDir;
	gmtl::normalize(result);
	float length = gmtl::dot(v, collDir);
	result *= length;
	return result;
} // getCollisionV

gmtl::Quatf RigidBody::getOrientation()
{
	return orientation;
} // getOrientation

gmtl::Vec3f RigidBody::getForward()
{
	return forward;
} // getForward

gmtl::Vec3f RigidBody::getUp()
{
	return up;
} // getUp

gmtl::Vec3f RigidBody::getRight()
{
	return right;
} // getRight

gmtl::Vec3f RigidBody::getVel()
{
	return v;
} // getVel

gmtl::Vec3f RigidBody::getAngularVel()
{
	return angV;
}

CollisionObject* RigidBody::getShape()
{
	return shape;
} // getShape

float RigidBody::getBoundingCircleRadius()
{
	return boundingCircleRadius;
} // getBoundingCircleRadius

bool RigidBody::isFixed()
{
	return !movable;
} // isMovable

Entity* RigidBody::getEntity()
{
	return entity;
} // getEntity

TransformationData RigidBody::getTransformation()
{
	TransformationData result = identityTransformation();
	result.position = x;
	result.orientation = orientation;
	return result;
} // getTransformation

void RigidBody::adjustOrientation()
{
	gmtl::Matrix44f m, m1;
	gmtl::cross(right, up, forward);	// calculate right vector
	gmtl::cross(forward, right, up);	// calculate front vector
	gmtl::normalize(right);
	gmtl::normalize(forward);
	gmtl::setDirCos(m, right, up, forward, RIGHT, UP, FORWARD);
	gmtl::set(orientation, m);
}

void RigidBody::getModelToWorldMatrix(gmtl::Matrix44f* m)
{
	gmtl::Matrix44f translation = gmtl::MAT_IDENTITY44F;

	gmtl::setTrans(translation, x);
	*m = gmtl::MAT_IDENTITY44F;
	gmtl::setAxes(*m, right, up, forward);
	gmtl::preMult(*m, translation);
/*	translation.setIdentity();
	translation.setTranslate(x);
	m->setIdentity();
	m->setValue(right, up, forward);
	m->multRight(translation);*/
} // getModelToWorldgmtl::Matrix44f
