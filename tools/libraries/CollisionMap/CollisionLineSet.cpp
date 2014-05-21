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

#include "CollisionLineSet.h"

#include <assert.h>

#include <gmtl/Generate.h>

#include "CollisionMap.h"
#include "CollisionCircle.h"
#include <inVRs/SystemCore/DataTypes.h>
#include <inVRs/SystemCore/DebugOutput.h>


CollisionLineSet::CollisionLineSet(const std::vector<CollisionLineSet::CollisionLine *> lines) {
	assert( ! lines.empty() );
	collisionLines = lines;
	printd(INFO, "CollisionLineSet(): Found %i collisionLines!\n", collisionLines.size());
} // CollisionLineSet

CollisionLineSet::~CollisionLineSet() {

} // CollisionLineSet

const std::vector<CollisionLineSet::CollisionLine *> & CollisionLineSet::getCollisionLines() const {
	return collisionLines;
}

std::string CollisionLineSet::getType() {
	return "CollisionLineSet";
} // getType

bool CollisionLineSet::canCheckCollision(CollisionObject* opponent) {
	if (opponent->getType() == "CollisionCircle" || opponent->getType() == "CollisionLineSet")
		return true;
	return false;
} // canCheckCollision

std::vector<CollisionData*>& CollisionLineSet::checkCollisionInternal(CollisionObject* opponent,
		std::vector<CollisionData*>& dst, bool changeOrder) {
	if (opponent->getType() == "CollisionCircle")
		return checkCollisionWithCircle((CollisionCircle*)opponent, dst, changeOrder);
	if (opponent->getType() == "CollisionLineSet")
		return checkCollisionWithLineSet((CollisionLineSet*)opponent, dst, changeOrder);

	assert(false);
	return dst;
} // checkCollisionInternal

/// \todo Test checkCollisionWithLineSet
std::vector<CollisionData*>& CollisionLineSet::checkCollisionWithLineSet(
		CollisionLineSet* opponent, std::vector<CollisionData*>& dst, bool changeOrder) {
	std::vector<CollisionLine*>::iterator it, opit;
	CollisionData* result = NULL;

	gmtl::Matrix44f transformation;
	gmtl::Point4f p0_tmp, p1_tmp;
	gmtl::Point4f vec0_tmp, vec1_tmp; //XXX should these be rotated or not?
	gmtl::Point2f p0, p1;
	gmtl::Point2f int0, int1; //points of intersection
	gmtl::Vec2f vec0, vec1;
	gmtl::Matrix<float, 2, 1> p1_minus_p0; //temporarily needed
	gmtl::Matrix<float, 2, 1> params; // the parameters to the two line-equations in vector-form
	gmtl::Matrix22f v0_v1;

	/* let both lines be of the form
	 * (1): p0 + s * vec0 = 0 and
	 * (2): p1 + t * vec1 = 0
	 *
	 * (1) - (2) yields:
	 * vec0 * s - vec1 * t = p1 - p0
	 *
	 * writing this as a matrix-system:
	 * ( vec0[0] , -vec1[0] )     ( s )    ( (p1-p0)[0] )
	 * ( vec0[1] , -vec1[1] )  *  ( t )  = ( (p1-p0)[1] )
	 *
	 *  ^^^^^^^^^^^^^^^^^^^         ^
	 *   this matrix is v0_v1       |this is params
	 *
	 * so we can write for short:
	 * v0_v1 * params = p1-p0
	 * i.e. params = invert(v0_v1) * (p1-p0)
	 *
	 * XXX what if line0 = line1 ?
	 */

	for (it = collisionLines.begin(); it != collisionLines.end(); ++it) {
		// build and transform own line (translate,scale,rotate):
		// the transformation has to be done in 3D:
		// N.B.: Point3f will be rotated, Vec3f not
		p1_tmp = gmtl::Point4f((*it)->startPoint[0], 0.0f, (*it)->startPoint[1], 1.0f);
		vec0_tmp = gmtl::Point4f((*it)->endPoint[0] - (*it)->startPoint[0], 0.0f,
				(*it)->endPoint[1] - (*it)->startPoint[1], 1.0f);
		transformationDataToMatrix(getTransformation(), transformation);
		p0_tmp = p0_tmp * transformation;
		vec0_tmp = vec0_tmp * transformation;
		// go back to 2D:
		p0 = gmtl::Point2f(p0_tmp[0], p0_tmp[2]);
		vec0 = gmtl::Point2f(vec0_tmp[0], vec0_tmp[2]);

		// cycle through opponent lines, identifying colliding lines
		// for each line, get transformed position/size/rotation and test for collision
		for (opit = opponent->collisionLines.begin(); opit != collisionLines.end(); opit++) {
			// if the above test didn't help, we go back to our normal test:
			p1_tmp = gmtl::Point4f((*opit)->startPoint[0], 0.0f, (*opit)->startPoint[1], 1.0f);
			vec1_tmp = gmtl::Point4f((*opit)->endPoint[0] - (*opit)->startPoint[0], 0.0f,
					(*opit)->endPoint[1] - (*opit)->startPoint[1], 1.0f);
			// transform opponent line (translate,scale,rotate):
			transformationDataToMatrix(opponent->getTransformation(), transformation);
			p1_tmp = p1_tmp * transformation;
			vec1_tmp = vec1_tmp * transformation;
			// go back to 2D:
			p1 = gmtl::Point2f(p1_tmp[0], p1_tmp[2]);
			vec1 = gmtl::Point2f(vec1_tmp[0], vec1_tmp[2]);

			// quick intersection test:
			// if the line in its entirety is left/right/above/below of the other line, it cannot intersect:
			if (gmtl::Math::Max(p0[0], (p0 + vec0)[0]) < gmtl::Math::Min(p1[0], (p1 + vec1)[0])
					|| gmtl::Math::Max(p1[0], (p1 + vec1)[0]) < gmtl::Math::Min(p0[0],
							(p0 + vec0)[0]) || gmtl::Math::Max(p0[1], (p0 + vec0)[1])
					< gmtl::Math::Min(p1[1], (p1 + vec1)[1]) || gmtl::Math::Max(p1[1],
					(p1 + vec1)[1]) < gmtl::Math::Min(p0[1], (p0 + vec0)[1])) {
				continue;
			}

			// intersect lines the "conventional" way:
			v0_v1.set(vec0[0], -vec1[0], vec0[1], -vec1[1]);
			gmtl::invert(v0_v1);
			// use (p1 - p0) as a 2x1 matrix
			p1_minus_p0[0][0] = (p1 - p0)[0];
			p1_minus_p0[1][0] = (p1 - p0)[1];
			// multiply (p1-p0) by v0_v1 from right
			gmtl::mult(params, v0_v1, p1_minus_p0);

			// compute point of intersection based on both line-equations:
			int0 = p0 + params[0][0] * vec0;
			int1 = p1 + params[1][0] * vec1;

			if (int0 == int1) //points match
			{
				//build result
				result = new CollisionData;
				result->object1 = this;
				result->object2 = opponent;
				result->normal1 = int0 - p0; // vector pointing from this.startPoint to collisionPoint
				gmtl::normalize(result->normal1);
				result->normal2 = result->normal1 * (-1.f);
				result->collisionPoint = int0;
				dst.push_back(result);
				result = NULL;
			}

		}

	} // for
	return dst;
}

std::vector<CollisionData*>& CollisionLineSet::checkCollisionWithCircle(CollisionCircle* opponent,
		std::vector<CollisionData*>& dst, bool changeOrder) {
	std::vector<CollisionLine*>::iterator it;
	CollisionData* result = NULL;

	for (it = collisionLines.begin(); it != collisionLines.end(); ++it) {
		if (checkCollisionWithCircle(result, *it, opponent, changeOrder)) {
			dst.push_back(result);
			result = NULL;
		} // if
	} // for
	return dst;
} // checkCollisionWithCircle

///\todo: CODE SHOULD CONTAIN MORE COMMENTS, USE BETTER VARIABLE NAMES AND ALL
///      IN ENGLISH IF POSSIBLE
bool CollisionLineSet::checkCollisionWithCircle(CollisionData* &result, CollisionLine* line,
		CollisionCircle* circle, bool changeOrder) {
	gmtl::Vec2f dist; // Vektor von Kreismittelpunkt zu Kollisionspunkt
	gmtl::Vec2f c, p, q; // c ... Circle-Center, p ... startPoint line, q ... endPoint line
	gmtl::Vec2f cp; // Vektor von Eckpunkt zu Kreis
	gmtl::Vec2f qp; // Vektor von Eckpunkt zu Eckpunkt
	gmtl::Vec2f R; // Projektion von Vektor zu Kreis auf Linie
	gmtl::Vec3f rotatedLine;
	gmtl::Vec3f scaledLine;
	gmtl::Quatf orientation;
	float radius;
	float penetrationDepth;
	int i;

	radius = circle->getRadius();
	c = gmtl::Vec2f(circle->getTransformation().position[0],
			circle->getTransformation().position[2]);

	scaledLine = gmtl::Vec3f(line->startPoint[0], 0, line->startPoint[1]);
	for (i = 0; i < 3; i++)
		scaledLine[i] = scaledLine[i] * transformation.scale[i];
	orientation = transformation.orientation;
	rotatedLine = orientation * scaledLine;
	p[0] = transformation.position[0] + rotatedLine[0];
	p[1] = transformation.position[2] + rotatedLine[2];

	scaledLine = gmtl::Vec3f(line->endPoint[0], 0, line->endPoint[1]);
	for (i = 0; i < 3; i++)
		scaledLine[i] = scaledLine[i] * transformation.scale[i];
	rotatedLine = transformation.orientation * scaledLine;
	q[0] = transformation.position[0] + rotatedLine[0];
	q[1] = transformation.position[2] + rotatedLine[2];

	cp = c - p; // Vektor von Eckpunkt zu Kreis
	qp = q - p; // Vektor von Eckpunkt zu Eckpunkt
	R = (gmtl::dot(cp, qp) / gmtl::dot(qp, qp)) * qp; // Projektion von Vektor zu Kreis auf Linie
	if (gmtl::length(R) > (gmtl::length(qp) + radius)) // Kollisionspunkt ausserhalb von Linie
		return false;
	if (gmtl::dot(R, qp) < 0 && gmtl::length(R) > (radius)) // Kollisionspunkt ausserhalb von Linie
		return false;
	if (gmtl::length(R) > gmtl::length(qp)) // Kollisionstest mit Endpunkt von Linie
	{
		dist = q - c;
		R = qp;
	} // if
	else if (gmtl::dot(R, qp) < 0) // Kollisionstest mit Anfangspunkt von Linie
	{
		dist = p - c;
		R = gmtl::Vec2f(0, 0);
	} // else if
	else // Kollisionstest mit irgendeinem Punkt auf Linie
	{
		dist = R - cp;
	} // else

	penetrationDepth = radius - gmtl::length(dist);
	if (penetrationDepth < 0) // Abbruch falls Abstand zu Kollisionspunkt groesser als Radius
		return false;

	// // Ergaenzung im 3dimensionalen Fall
	// 	Vec3f dist3 = Vec3f(dist[0], 0, dist[1]);	// Konvertierung in 3d-Vektor
	// 	Real32 len = (dist3 * c->up) * -1;			// Ermitteln von Normalanteil
	// 	Vec3f vPlane = dist3 + (len*c->up);			// Addition von Normalanteil um Vektor in Lage-Ebene zu bringen
	// 	vPlane.normalize();							// Normalisierter Vektor in Ebene
	// 	vPlane *= c->rad;							// Vektor auf Kreisradius verlaengern
	// 	Vec2f projV = Vec2f(vPlane[0], vPlane[2]);	// Entfernen von Hoehenwert -> Projektion auf 2D
	// 	Real32 projRad = projV.length();			// Ermitteln von projizierten Radius
	// // Ende Ergaenzung

	result = new CollisionData;
	result->object1 = this;
	result->object2 = circle;
	result->normal1 = dist;
	gmtl::normalize(result->normal1);
	result->normal2 = result->normal1 * (-1.f);
	result->collisionPoint = p + R + result->normal1 * (0.5f * penetrationDepth);

	return true;
} // checkCollisionWithCircle
