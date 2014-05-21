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

#ifndef _RIGIDBODY_H_
#define _RIGIDBODY_H_

#include <gmtl/VecOps.h>
#include <gmtl/QuatOps.h>
#include <gmtl/MatrixOps.h>
#include <inVRs/SystemCore/DataTypes.h>


class CollisionObject;
struct CollisionCallbackData;
class Entity;

class RigidBody
{
protected:
	friend class Simulation2D;
	gmtl::Vec3f x;		// Position des Schwerpunkts
	gmtl::Vec3f v;		// Geschwindigkeit
	gmtl::Vec3f angV;
	gmtl::Vec3f constantForce;
	gmtl::Vec3f force; 	// Summe aller wirkenden Kraefte
	gmtl::Vec3f constantTorque;
	gmtl::Vec3f torque;
	float massInv;

	static const gmtl::Vec3f UP, FORWARD, RIGHT;
	gmtl::Quatf orientation;
	gmtl::Vec3f right;
	gmtl::Vec3f forward;
	gmtl::Vec3f up;
// 	float levitation;
// 	float pivotOffset;
	float boundingCircleRadius;
	CollisionObject* shape;
	bool movable;

	Entity* entity;
	bool notifyAtCollision;
	virtual void collisionCallback(CollisionCallbackData* collision) {};

public:
	RigidBody();
	virtual ~RigidBody();
	virtual gmtl::Vec3f getPos();
	virtual void setOrientation(gmtl::Quatf orientation);
	virtual void setRight(gmtl::Vec3f right);
	virtual void setForward(gmtl::Vec3f forward);
	virtual void setUp(gmtl::Vec3f up);
	virtual void setVel(gmtl::Vec3f vel);
	virtual void setAngularVel(gmtl::Vec3f angVel);
	virtual void setPos(gmtl::Vec3f pos);
	virtual void setMass(float mass);
	virtual void setShape(CollisionObject* shape);
	virtual void setConstantForce(gmtl::Vec3f force);
	virtual void setConstantTorque(gmtl::Vec3f torque);
	virtual void setBoundingCircleRadius(float radius);
	virtual void setMovable(bool movable);
	virtual void setEntity(Entity* entity);

	virtual void addForce(gmtl::Vec3f force);
	virtual void addTorque(gmtl::Vec3f torque);

	virtual gmtl::Quatf getOrientation();
	virtual gmtl::Vec3f getRight();
	virtual gmtl::Vec3f getForward();
	virtual gmtl::Vec3f getUp();
	virtual gmtl::Vec3f getVel();
	virtual gmtl::Vec3f getCollisionV(gmtl::Vec3f collDir);
	virtual gmtl::Vec3f getAngularVel();
	virtual float getMassInv();
	virtual gmtl::Vec2f get2DPos();
	virtual float getBoundingCircleRadius();
	virtual CollisionObject* getShape();
	virtual bool isFixed();
	virtual Entity* getEntity();
	virtual TransformationData getTransformation();
	// setzt forwart und right members von up ausgehend
	virtual void adjustOrientation();

	// liefert Model-zu-Welt Matrix zur Darstellung
	virtual void getModelToWorldMatrix(gmtl::Matrix44f* m);

	virtual void update(float dt) {};
}; // RigidBody

#endif //_RIGIDBODY_H_
