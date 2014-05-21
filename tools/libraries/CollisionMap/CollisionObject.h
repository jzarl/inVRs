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

#ifndef _COLLISIONOBJECT_H
#define _COLLISIONOBJECT_H

#include <string>
#include <vector>

#include <inVRs/SystemCore/DataTypes.h>


class CollisionObject;

/******************************************************************************
 * @class CollisionData
 * @brief Structure where to store the Contact information.
 *
 * This struct is used to store the information about each contact point found
 * during the collision detection. It contains the CollisionObject which are
 * colliding, the normals which are the normalized 2D vectors from the center
 * of the CollisionObjects to the collision point and the position of the
 * collision point in 2D coordinates.
 */
struct CollisionData {
	CollisionObject *object1, *object2;
	gmtl::Vec2f normal1, normal2; // Vector in direction of Collision Point
	gmtl::Vec2f collisionPoint;
	// 	gmtl::Vec2f normal;
}; // CollisionData

/******************************************************************************
 * @class CollisionObject
 * @brief Abstract class for Objects used for 2D collision detection.
 *
 * This class is used as a base class for all collision shapes used for the
 * 2D collision detection. It contains methods to update the Transformation of
 * the collision shape and to check if a collision with other CollisionObjects
 * occur. The collision detection itself is mainly calculated in 2D space with
 * some exceptions like the CollisionCircle which additionally takes the up-
 * vector to correct the 2D-shape of itself to an Ellipse.
 */
class CollisionObject {
public:

	/** Constructor initializes transformation of CollisionObject.
	 */
	CollisionObject();

	/** Empty destructor.
	 */
	virtual ~CollisionObject();

	/** Checks if the Object collides with another CollisionObject.
	 * The method checks if it collides with the passed CollisionObject. It
	 * gets a reference to a vector of CollisionData elements as parameter
	 * where it stores the calculated collision information if one ore more
	 * collision points are found.
	 * @param opponent CollisionObject with which a collision should be checked
	 * @param dst Destination vector where all Collisions are stored to
	 * @return Destination vector (same as second parameter)
	 */
	std::vector<CollisionData*>& checkCollision(CollisionObject* opponent, std::vector<
			CollisionData*>& dst);

	/** Sets the Transformation of the CollisionObject.
	 * The method sets the Transformation of the Object. This Transformation
	 * will be used boy the <code>checkCollision</code> methods to determine
	 * if one CollisionObject collides with another one.
	 * @param trans Transformation of the CollisionObject
	 */
	virtual void setTransformation(TransformationData trans);

	/** Returns the Transformation of the CollisionObject.
	 * The method returns a the TransformationData of the Object. This
	 * Transformation can be used by the <code>checkCollision</code>
	 * methods to determine if one CollisionObject collides with another one.
	 * @return Transformation of CollisionObject
	 */
	virtual TransformationData& getTransformation();

	/** Returns the name of the CollisionObject.
	 * The method returns the name of the CollisionObject. This name is
	 * normally equal to the classname of the CollisionObject. It is used in
	 * the <code>checkCollision</code> methods to identify the type of
	 * CollisionObject and to find the correct method for the collision test
	 * between two objects.
	 * @return Type (=name) of the CollisionObject.
	 */
	virtual std::string getType() = 0;

protected:

	/** Checks if a collision-method is implemented for a collision check
	 * between the current CollisionObject and the passed one. Internally
	 * it checks the name of the passed CollisionObject and returns if a method
	 * exists locally to check for collisions.
	 * @param opponent CollisionObject for which a collision-method is searched
	 * @return true if a collision-method is implemented in this class, false
	 *         otherwise
	 */
	virtual bool canCheckCollision(CollisionObject* opponent) = 0;

	/** Calculates the collisions between the passed CollisionObject and itself.
	 * This method is called internally to calculate the collisions between the
	 * passed Object and the current Object. For each contact point found a own
	 * CollisionData-entry is placed into the passed destination vector. The
	 * parameter changeOrder defines in which order the CollisionObjects are
	 * stored in the CollisionData elements. The CollisionObject on which the
	 * <code>checkCollision</code> method is called is always the first
	 * CollisionObject in the CollisionData element.
	 * @param opponent CollisionObject to check the collision with
	 * @param dst Destination vector where to write the collision information to
	 * @param changeOrder Defines order of CollisionObjects in CollisionData
	 * @return Destination vector (same as second parameter)
	 */
	virtual std::vector<CollisionData*>& checkCollisionInternal(CollisionObject* opponent,
			std::vector<CollisionData*>& dst, bool changeOrder) = 0;

	/// The Transformation of the CollisionObject
	TransformationData transformation;

}; // CollisionObject

#endif // _COLLISIONOBJECT_H
