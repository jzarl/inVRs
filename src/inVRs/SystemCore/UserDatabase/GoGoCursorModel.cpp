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

#include "GoGoCursorModel.h"

TransformationData GoGoCursorModel::generateCursorTransformation(
		TransformationData userTransformation, User* user) {
	gmtl::Vec3f distVec; //distance vector between shoulder and hand in real world
	float dist;
	gmtl::Vec3f worldDistVector; //distance vector in world coordinates
	gmtl::Vec3f worldUnitVector; //unit vector of the distance vector between shoulder and hand in world coordinates

	//gmtl::Quatf navigatedOrientation = user->getNavigatedTransformation().orientation;

	TransformationData result;
	TransformationData userHandTransformation = user->getUserHandTransformation();
	TransformationData userTrackedHandTransformation = user->getTrackedHandTransformation();
	TransformationData userTrackedHeadTransformation = user->getTrackedHeadTransformation();
	TransformationData userWorldHeadTransformation = user->getWorldHeadTransformation();

	//calculate shoulder position
	//	userWorldHeadTransformation.position[1] -= headHeight*user->getPhysicalToWorldScale();

	worldDistVector = user->getWorldHandTransformation().position
			- userWorldHeadTransformation.position;
	worldUnitVector = worldDistVector / gmtl::length(worldDistVector);

	distVec = userTrackedHandTransformation.position - userTrackedHeadTransformation.position;
	dist = gmtl::length(distVec);

	multiply(result, userTransformation, userHandTransformation);

	if (dist > distanceThreshold) //perform quadratic cursor transformation
	{
		result.position += worldUnitVector * (dist - distanceThreshold)
				* (dist - distanceThreshold) * k;
	}

	return result;
}

std::string GoGoCursorModel::getName() {
	return "GoGoCursorModel";
}

GoGoCursorModel::GoGoCursorModel() {
	this->distanceThreshold = 20;
	this->k = 0.5;
}

GoGoCursorModel::GoGoCursorModel(ArgumentVector* args) {
	this->args = args;

	if (args->get("distanceThreshold", distanceThreshold))
		;

	else {
		distanceThreshold = 20;
	}

	if (args->get("k", k))
		;

	else {
		this->k = 0.5;
	}

	if (args->get("headHeigth", headHeight))
		;

	else {
		headHeight = 10;
	}
}

CursorTransformationModel* GoGoCursorModelFactory::create(std::string className, void* args) {
	if (className != "GoGoCursorModel")
		return NULL;
	if (!args) {
		return new GoGoCursorModel();
	}
	return new GoGoCursorModel((ArgumentVector*)args);
} // create
