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

#include "HomerCursorModel.h"

#include <sstream>

#include <inVRs/SystemCore/WorldDatabase/WorldDatabase.h>
#include <inVRs/SystemCore/Timer.h>
#include "SelectionChangeModel.h"

std::string HomerCursorModel::getName() {
	return "HomerCursorModel";
}

TransformationData HomerCursorModel::generateCursorTransformation(
		TransformationData userTransformation, User* user) {
//	TransformationData result;
	if (forwardAnimating) {
		cursorTrans = forwardAnimation(userTransformation, user);
//		result = forwardAnimation(userTransformation, user);
	} // if
	else if (backwardAnimating) {
		cursorTrans = backwardAnimation(userTransformation, user);
//		result = backwardAnimation(userTransformation, user);
	} // else if
	else {
//		result = getNotAnimatedCursorTrans(userTransformation, user);
//		if (picked) {
//			result.position += result.orientation*gmtl::Vec3f(0,0,-1) * dist;
//		}
//		cursorTrans.position = result.position;
		cursorTrans = getNotAnimatedCursorTrans(userTransformation, user);
		if (picked) {
			cursorTrans.position += cursorTrans.orientation*gmtl::Vec3f(0,0,-1) * dist;
		}

	} // else

	//dumpTransformation(cursorTrans);

	return cursorTrans;
}

void HomerCursorModel::setCursorRange(float range) {
	this->range = range;
}

//TODO make range for moving arm (range for trackedHandTransformation)

void HomerCursorModel::startForwardAnimation(std::vector<PICKEDENTITY>* entities) {
	PICKEDENTITY ent;
	if (entities->size() != 0)
		ent = entities->at(0);
	entity = ent.entity;
	Interaction * interaction = (Interaction*)SystemCore::getModuleByName("Interaction");

	if (!interaction) {
		printd(ERROR, "HomerCursorModel: Interaction is null");
		return;
	}

	SelectionChangeModel* selectionChangeModel = interaction->getSelectionChangeModel();

	rayCastSelectionChangeModel = dynamic_cast<RayCastSelectionChangeModel *>(selectionChangeModel);
	if (!rayCastSelectionChangeModel) {
		printd(ERROR, "HomerCursorModel: Wrong selectionChangeModel used. Use RayCastSelectionChangeModel!\n");
		return;
	}

//	userHandTrans = localUser->getWorldHandTransformation();
//	cursorTrans = localUser->getCursorTransformation();
//	TransformationData navigatedTransformation = localUser->getNavigatedTransformation();
//	invert(navigatedTransformation);
// TODO: maybe invert!!!
//	multiply(initialCursorOffset, navigatedTransformation, cursorTrans);

//	dist = rayCastSelectionChangeModel->getRayDistance();
//	direction = rayCastSelectionChangeModel->getRayDirection();

	rayDestTrans = identityTransformation();
	rayDestTrans.position = rayCastSelectionChangeModel->getRayDestination();

//	i = 0;
	forwardAnimating = true;

	lastTime = timer.getTime();

	// if(debug)
	//{
	// WorldDatabase::createEntity(14,0,rayDestTrans,NULL);
	// }
}

void HomerCursorModel::startBackwardAnimation() {
//	TransformationData userHandTransformation = localUser->getUserHandTransformation();
//	TransformationData userTransformation = localUser->getNavigatedTransformation();
//	userHandTransformation.position += cursorOffset;
//	multiply(userHandTrans, userTransformation, userHandTransformation);

//	userHandTrans = localUser->getWorldHandTransformation();
//	gmtl::Vec3f distVec = cursorTrans.position - userHandTrans.position;
//	direction = distVec / gmtl::length(distVec);
	backwardAnimating = true;
	lastTime = timer.getTime();
//	i = dist;
}


bool HomerCursorModel::isAnimating() {
	return forwardAnimating || backwardAnimating;
}

bool HomerCursorModel::isForwardAnimating() {
	return forwardAnimating;
}

bool HomerCursorModel::isBackwardAnimating() {
	return backwardAnimating;
}

/*
HomerCursorModel::HomerCursorModel() {
	animationSpeed = 2;
	forwardAnimating = false;
	backwardAnimating = false;
	dist = 0;
	direction = gmtl::Vec3f(0, 0, 0);
	picked = false;
	localUser = UserDatabase::getLocalUser();
	forwardDistanceThreshold = 0;
	backwardDistanceThreshold = 0;
}
*/

HomerCursorModel::HomerCursorModel(ArgumentVector* args) {
	forwardAnimating = false;
	backwardAnimating = false;
	forwardDistanceThreshold = 0.1f;
	backwardDistanceThreshold = 0.1f;
	dist = 0;
	direction = gmtl::Vec3f(0, 0, 0);
//	localUser = UserDatabase::getLocalUser();
	std::string offsetString;
	cursorOffset = gmtl::Vec3f(0, 0, 0);
	float offsetX = 0;
	float offsetY = 0;
	float offsetZ = 0;
	picked = false;

	this->args = args;

	if (args && args->keyExists("animationSpeed"))
		args->get("animationSpeed", animationSpeed);
	else
		animationSpeed = 1;

	if (args && args->keyExists("forwardThreshold"))
		args->get("forwardThreshold", forwardDistanceThreshold);
	if (args && args->keyExists("backwardThreshold"))
		args->get("backwardThreshold", backwardDistanceThreshold);

	if (args && args->keyExists("cursorOffset")) {
		args->get("cursorOffset", offsetString);
		std::istringstream iss(offsetString);

		iss >> offsetX >> offsetY >> offsetZ;
		cursorOffset[0] = offsetX;
		cursorOffset[1] = offsetY;
		cursorOffset[2] = offsetZ;
	} // if

}


TransformationData HomerCursorModel::forwardAnimation(TransformationData userTransformation,
		User* user) {

	TransformationData result = cursorTrans;
	gmtl::Vec3f distVec = rayDestTrans.position - cursorTrans.position;
	dist = gmtl::length(distVec);
	direction = distVec;
	gmtl::normalize(direction);

	double currentTime = timer.getTime();
	float dt = currentTime - lastTime;
	float movement = animationSpeed * dt;

	if (dist > forwardDistanceThreshold && movement < dist) {
		result.position = cursorTrans.position + direction * movement;
//		i += animationSpeed;
	} else {
		result.position = rayDestTrans.position;
		forwardAnimating = false;
		picked = true;
		distVec = result.position - getNotAnimatedCursorTrans(userTransformation, user).position;
//		distVec = localUser->getUserHandTransformation().position - cursorTrans.position;
		dist = gmtl::length(distVec);
//		direction = distVec / dist;
	}

	lastTime = currentTime;
	return result;
}

TransformationData HomerCursorModel::backwardAnimation(TransformationData userTransformation,
		User* user) {

	TransformationData result = cursorTrans;
	TransformationData targetTrans = getNotAnimatedCursorTrans(userTransformation, user);

//	userHandTrans = localUser->getWorldHandTransformation();
	gmtl::Vec3f distVec = targetTrans.position - cursorTrans.position;
	dist = gmtl::length(distVec);
	//	gmtl::Vec3f oldDirection = direction;
	direction = distVec;
	gmtl::normalize(direction);

	double currentTime = timer.getTime();
	float dt = currentTime - lastTime;
	float movement = animationSpeed * dt;

	//TODO same thing as in forwardAnimation
	if (dist > backwardDistanceThreshold && movement < dist) {
		result.position += direction * movement;
//		i -= animationSpeed;
	} else {
		backwardAnimating = false;
		picked = false;
	}

	lastTime = currentTime;
	return result;
}

TransformationData HomerCursorModel::getNotAnimatedCursorTrans(
		TransformationData userTransformation, User* user) {

	TransformationData result;
	TransformationData userHandTransformation = user->getUserHandTransformation();
	userHandTransformation.position += cursorOffset;
	multiply(result, userTransformation, userHandTransformation);
	return result;
} // getNotAnimatedCursorTrans


CursorTransformationModel* HomerCursorModelFactory::create(std::string className, void* args) {
	if (className != "HomerCursorModel")
		return NULL;

	if (!args)
		return new HomerCursorModel();

	return new HomerCursorModel((ArgumentVector*)args);

}

