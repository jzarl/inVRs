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

#include "CollisionObject.h"

#include <inVRs/SystemCore/DebugOutput.h>

CollisionObject::CollisionObject() {
	transformation = identityTransformation();
} // CollisionObject

CollisionObject::~CollisionObject() {

} // ~CollisionObject

std::vector<CollisionData*>& CollisionObject::checkCollision(CollisionObject* opponent,
		std::vector<CollisionData*>& dst) {
	if (canCheckCollision(opponent))
		return checkCollisionInternal(opponent, dst, false);

	if (!opponent->canCheckCollision(this)) {
		printd(
				ERROR,
				"CollisionObject::checkCollision(): Could not check collision between CollisionObjects %s and %s! No implementation found!\n",
				this->getType().c_str(), opponent->getType().c_str());
		return dst;
	} // if

	return opponent->checkCollisionInternal(this, dst, true);
} // checkCollision

void CollisionObject::setTransformation(TransformationData trans) {
	this->transformation = trans;
} // setTransformation

TransformationData& CollisionObject::getTransformation() {
	return transformation;
} // getTransformation
