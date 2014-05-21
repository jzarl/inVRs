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

#include "CollisionCircle.h"

#include <assert.h>

#include <gmtl/VecOps.h>

#include "CollisionMap.h"
#include <inVRs/SystemCore/DebugOutput.h>

CollisionCircle::CollisionCircle(float radius) {
	this->radius = radius;
} // CollisionCircle

CollisionCircle::~CollisionCircle() {

} // ~CollisionCircle

void CollisionCircle::setRadius(float radius) {
	this->radius = radius;
} // setRadius

float CollisionCircle::getRadius() {
	return radius;
} // getRadius

std::string CollisionCircle::getType() {
	return "CollisionCircle";
} // getType


bool CollisionCircle::canCheckCollision(CollisionObject* opponent) {
	if (opponent->getType() == "CollisionCircle")
		return true;
	return false;
} // canCheckCollision

std::vector<CollisionData*>& CollisionCircle::checkCollisionInternal(CollisionObject* opponent,
		std::vector<CollisionData*>& dst, bool changeOrder) {
	if (opponent->getType() == "CollisionCircle")
		return checkCollisionWithCircle((CollisionCircle*)opponent, dst, changeOrder);

	assert(false);
	return dst;
} // checkCollisionInternal

std::vector<CollisionData*>& CollisionCircle::checkCollisionWithCircle(CollisionCircle* opponent,
		std::vector<CollisionData*>& dst, bool changeOrder) {
	// TODO: Scale!!!
	float length;
	float penetrationDepth;
	CollisionData* result;
	gmtl::Vec2f center = gmtl::Vec2f(this->transformation.position[0],
			this->transformation.position[2]);
	gmtl::Vec3f distance = opponent->transformation.position - this->transformation.position;
	distance[1] = 0;
	length = gmtl::length(distance);

	penetrationDepth = opponent->radius + this->radius - length;
	// 	printd("DISTANCE = %f / depth = %f\n", length, penetrationDepth);

	if (penetrationDepth < 0)
		return dst;

	// 	printd("Found Circle Circle Collision!\n");
	result = new CollisionData;
	result->object1 = this;
	result->object2 = opponent;
	result->normal1 = gmtl::Vec2f(distance[0], distance[2]);
	gmtl::normalize(result->normal1);
	result->normal2 = result->normal1 * (-1.f);
	result->collisionPoint = center + result->normal1 * (this->radius - 0.5f * penetrationDepth);
	dst.push_back(result);
	return dst;
} // checkCollisionWithCircle
