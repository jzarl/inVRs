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

#include "CheckCollisionModifierBase.h"
#include "CollisionCircle.h"

#include <gmtl/AxisAngle.h>
#include <gmtl/MatrixOps.h>
#include <gmtl/Generate.h>
#include <gmtl/QuatOps.h>

#include <inVRs/SystemCore/Configuration.h>
#include <inVRs/SystemCore/DebugOutput.h>

CheckCollisionModifierBase::CheckCollisionModifierBase(CollisionMap* collisionMap) {
	printd(INFO, "CheckCollisionModifierBase(): Constructor init!\n");
	this->collisionMap = collisionMap;
	// No repeats by default!
	this->maxRepeats = 0;
	printd(INFO, "CheckCollisionModifierBase(): Constructor done!\n");
} // CheckCollisionModifierBase

CheckCollisionModifierBase::~CheckCollisionModifierBase() {
	std::map<unsigned, TransformationData*>::iterator it;
	for (it = lastUserTransformation.begin(); it != lastUserTransformation.end(); ++it)
		delete (*it).second;
	lastUserTransformation.clear();

	if (collisionMap)
		delete collisionMap;
} // ~CheckCollisionModifierBase

TransformationData CheckCollisionModifierBase::execute(TransformationData* resultLastStage,
		TransformationPipe* currentPipe) {
	bool didCorrection;
	gmtl::Vec3f movement;
	std::vector<CollisionData*> collisions;
	std::vector<CollisionData*>::iterator it;
	TransformationData currentTrans = *resultLastStage;
	TransformationData *lastResult;
	unsigned userId = currentPipe->getOwner()->getId();

	lastResult = lastUserTransformation[userId];
	if (!lastResult) {
		lastResult = new TransformationData;
		*lastResult = currentTrans;
		lastUserTransformation[userId] = lastResult;
		return currentTrans;
	} // if

	collisionMap->checkCollision(currentTrans, collisions);
	movement = currentTrans.position - lastResult->position;
	int repeatCounter = -1;

	do {
		didCorrection = false;
		for (it = collisions.begin(); it != collisions.end(); ++it) {
			gmtl::Vec3f normal3D = -1.f * gmtl::Vec3f((*it)->normal1[0], 0, (*it)->normal1[1]);
			gmtl::normalize(normal3D);
			// TODO: Check if this code only works for CIRCLES!!!
			float normalFraction = gmtl::dot(movement, normal3D);

			// otherwise we already move away from collision
			if (normalFraction < -0.0001) {
				gmtl::Vec3f movementCorrection = normal3D * (-normalFraction);
				currentTrans.position += movementCorrection;
				movement += movementCorrection;
				didCorrection = true;
			} // if
		} // for
		repeatCounter++;
	} while (didCorrection == true && repeatCounter < maxRepeats);
//	if (repeatCounter > 0)
//		printd(INFO, "CheckCollisionModifierBase::execute(): found %i repeats!\n", repeatCounter);

	for (it = collisions.begin(); it != collisions.end(); ++it) {
		delete (*it);
	} // for
	*lastResult = currentTrans;

	return currentTrans;
} // execute


CheckCollisionModifierFactoryBase::CheckCollisionModifierFactoryBase(CollisionLineSetFactory* collisionLineSetFactory):
	collisionLineSetFactory(collisionLineSetFactory) {
	className = "CheckCollisionModifier";
}

CheckCollisionModifierFactoryBase::~CheckCollisionModifierFactoryBase() {
	delete collisionLineSetFactory;
}

bool CheckCollisionModifierFactoryBase::needInstanceForEachPipe() {
	return true;
}

TransformationModifier* CheckCollisionModifierFactoryBase::createInternal(ArgumentVector* args) {
	float radius;
	std::string fileName;

	if (!args || !args->get("radius", radius) || !args->get("fileName", fileName)) {
		printd(
				ERROR,
				"CheckCollisionModifierBase::createInternal(): error within xml-Arguments! Expected the arguments \"radius\" and \"fileName\"\n");
		return NULL;
	} // if

	CollisionMap* collMap = new CollisionMap(new CollisionCircle(radius));
	collMap->loadCollisionLineSet(Configuration::getPath("CollisionMaps") + fileName,
			identityTransformation(), collisionLineSetFactory);

	return new CheckCollisionModifierBase(collMap);
} // create
