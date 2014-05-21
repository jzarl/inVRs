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

#ifndef _COLLISIONCIRCLE_H
#define _COLLISIONCIRCLE_H

#include "CollisionObject.h"

/******************************************************************************
 * @class CollisionCircle
 * @brief Collision class for 2D-Shapes described by a circle.
 *
 * This class is used for circular collision shapes. Additional to the
 * 2D collision test this class also transforms the shape of the Circle
 * according to the orientation of the collision shape (up-vector) to
 * form an ellipse. This is useful for simulating objects with circular
 * shape which move over a nonplanar ground and adjust their orientation
 * to the normal vector of the ground. (e.g. steep curve in a racing game).
 * If this behaviour is not desired and the shape should always be a circle
 * independent of the orientation then simply pass an identity-orientation
 * in the setTransformation method.
 * Implemented collision-methods:
 *     CollisionCircle - CollisionCircle
 */
class CollisionCircle : public CollisionObject {
public:

	/** Constructor initializes radius.
	 * @param radius Radius of the CollisionCircle
	 */
	CollisionCircle(float radius);

	/** Empty destructor.
	 */
	virtual ~CollisionCircle();

	/** Sets the radius of the CollisionCircle.
	 * The method sets the radius of the CollisionCircle. This can be used
	 * to dynamically change the radius of the CollisionCircle in runtime.
	 * @param radius Radius of the CollisionCircle.
	 */
	void setRadius(float radius);

	/** Returns the radius of the CollisionCircle.
	 * @return Radius of the CollisionCircle.
	 */
	float getRadius();

	/** Returns the name of the CollisionObject (="CollisionCircle").
	 * @see CollisionObject::getType()
	 * @return Type of the CollisionObject (="CollisionCircle").
	 */
	virtual std::string getType();

protected:
	/** Checks if a collision-method is implemented for a collision check
	 * between the current CollisionObject and the passed one. Internally
	 * it checks the name of the passed CollisionObject and returns if a method
	 * exists locally to check for collisions.
	 * @param opponent CollisionObject for which a collision-method is searched
	 * @return true if a collision-method is implemented in this class, false
	 *         otherwise
	 */
	virtual bool canCheckCollision(CollisionObject* opponent);

	/** Calculates the collisions between the passed CollisionObject and itself.
	 * @see CollisionObject::checkCollisionInternal()
	 * @param opponent CollisionObject to check the collision with
	 * @param dst Destination vector where to write the collision information to
	 * @param changeOrder Defines order of CollisionObjects in CollisionData
	 * @return Destination vector (same as second parameter)
	 */
	virtual std::vector<CollisionData*>& checkCollisionInternal(CollisionObject* opponent,
			std::vector<CollisionData*>& dst, bool changeOrder);

	/** Searches for collisions between the current CollisionCircle and the
	 * passed one.
	 * The method is called from the checkCollisionInternal method and does the
	 * collision test between the local CollisionCircle and the passed
	 * CollisionCircle.
	 * @param opponent CollisionCircle to check the collision with
	 * @param dst Destination vector where to write the collision information to
	 * @param changeOrder Defines order of CollisionObjects in CollisionData
	 * @return Destination vector (same as second parameter)
	 */
	std::vector<CollisionData*>& checkCollisionWithCircle(CollisionCircle* opponent, std::vector<
			CollisionData*>& dst, bool changeOrder);

	/// Radius of the Collision Circle
	float radius;

}; // CollisionCircle

#endif // _COLLISIONCIRCLE_H
