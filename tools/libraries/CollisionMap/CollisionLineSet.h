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

#ifndef _COLLISIONLINESET_H
#define _COLLISIONLINESET_H

#include <vector>

#include "CollisionObject.h"
#include <inVRs/SystemCore/ClassFactory.h>

class CollisionCircle;

/******************************************************************************
 * @class CollisionLineSet
 * @brief Collision class for 2D-Shapes described by a set of lines.
 *
 * This class is used for collision shapes which consist of a set of lines.
 * The used lines have to be stored in a VRML-file which is read by the
 * constructor of the class.
 * Implemented collision-methods:
 *     CollisionLineSet - CollisionCircle
 *
 * TODO: The class has no method for a collision test with another
 *       CollisionLineSet implemented. This still has to be done!!!
 */
class CollisionLineSet : public CollisionObject {
public:

	/// Datatype for a single collision line
	struct CollisionLine {
		gmtl::Vec2f startPoint;
		gmtl::Vec2f endPoint;
	}; // CollisionLine

	/** Constructor to copy an existing <code>CollisionLine</code> set.
	 * This constructor takes an array of <code>CollisionLine*</code>s and copies
	 * them for internal use.
	 * @param lines An array of <code>CollisionLine</code>s
	 */
	CollisionLineSet(const std::vector<CollisionLineSet::CollisionLine *> lines);

	/** Empty destructor.
	 */
	virtual ~CollisionLineSet();

	/** Get a vector containing all collision lines.
	 */
	const std::vector<CollisionLineSet::CollisionLine *> &getCollisionLines() const;

	/** Returns the name of the CollisionObject (="CollisionLineSet").
	 * The method returns the name of the CollisionObject. This name is
	 * normally equal to the classname of the CollisionObject. It is used in
	 * the <code>checkCollision</code> methods to identify the type of
	 * CollisionObject and to find the correct method for the collision test
	 * between two objects.
	 * @return Type of the CollisionObject (returns "CollisionLineSet").
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

	/** Searches for collisions between two CollisionLineSets.
	 * The method is called from the checkCollisionInternal method and does the
	 * collision test between this CollisionLineSet and the passed one.
	 * Note: if possible, the opponent should be the CollisionLineSet with more lines.
	 * @param opponent CollisionLineSet to check the collision with
	 * @param dst Destination vector where to write the collision information
	 * @param changeOrder Defines order of CollisionObjects in CollisionData
	 * @return Destination vector (same as second parameter)
	 */
	std::vector<CollisionData*>& checkCollisionWithLineSet(CollisionLineSet* opponent, std::vector<
			CollisionData*>& dst, bool changeOrder);

	/** Searches for collisions between the CollisionLineSet and the passed
	 * CollisionCircle.
	 * The method is called from the checkCollisionInternal method and does the
	 * collision test between the CollisionLineSet and the passed
	 * CollisionCircle. It therefore calls for each collision line the
	 * <code>checkCollisionWithCircle</code> method.
	 * @param opponent CollisionCircle to check the collision with
	 * @param dst Destination vector where to write the collision information to
	 * @param changeOrder Defines order of CollisionObjects in CollisionData
	 * @return Destination vector (same as second parameter)
	 */
	std::vector<CollisionData*>& checkCollisionWithCircle(CollisionCircle* opponent, std::vector<
			CollisionData*>& dst, bool changeOrder);

	/** Searches for a collision between a single line and the passed
	 * CollisionCircle.
	 * The checks if the passed CollisionLine collides with the passed
	 * CollisionCircle. If so a CollisionData object is created and returned to
	 * the caller (which is the <code>checkCollisionWithCircle</code> method).
	 * @param result CollisionData where the collision info is stored if found
	 * @param line CollisionLine which should be checked for collision
	 * @param circle CollisionCircle which should be checked for collision
	 * @param changeOrder Defines order of CollisionObjects in CollisionData
	 * @return true if a collision was found, false otherwise
	 */
	bool checkCollisionWithCircle(CollisionData* &result, CollisionLine* line,
			CollisionCircle* circle, bool changeOrder);

	/// List of all CollisionLines defining the shape of the CollisionObject
	std::vector<CollisionLine*> collisionLines;

}; // CollisionLineSet


typedef ClassFactory<CollisionLineSet,std::string> CollisionLineSetFactory;

#endif // _COLLISIONLINESET_H
