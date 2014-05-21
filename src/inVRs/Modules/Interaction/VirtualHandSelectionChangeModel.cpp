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

#include "VirtualHandSelectionChangeModel.h"

#include <assert.h>

#include <gmtl/QuatOps.h>
#include <gmtl/VecOps.h>
#include <gmtl/Generate.h>
#include <gmtl/Xforms.h>

#include <inVRs/SystemCore/UserDatabase/UserDatabase.h>
#include <inVRs/SystemCore/DataTypes.h>
#include <inVRs/SystemCore/WorldDatabase/WorldDatabase.h>

VirtualHandSelectionChangeModel::VirtualHandSelectionChangeModel(float distanceThreshold) {
	triggerSelecetionStateChange = false;
	triggerUnselectionStateChange = false;
	currentEnvironment = NULL;
	this->distanceThreshold = distanceThreshold;
}

VirtualHandSelectionChangeModel::~VirtualHandSelectionChangeModel() {

}

void VirtualHandSelectionChangeModel::select(std::vector<PICKEDENTITY>* selectedEntities,
		std::vector<PICKEDENTITY>* entitiesToSelect, std::vector<PICKEDENTITY>* entitiesUnselected) {
	PICKEDENTITY pickedEntity;

	assert(selectedEntities);
	assert(entitiesToSelect);
	assert(entitiesUnselected);
	assert(selectedEntities->size()<=1);

	entitiesToSelect->clear();
	triggerSelecetionStateChange = false;

	pickedEntity = getEntityTouched();

	if (pickedEntity.entity == NULL && distanceThreshold > 0)
		pickedEntity = getNearbyEntity();

	if (pickedEntity.entity == NULL)
		return;
	//	else
	//		printd("JUHUUUUUUU: picked an Entity!\n");

	if (selectedEntities->size() == 0) {
		// we are in idle state, switch to selected state
		entitiesToSelect->push_back(pickedEntity);
		triggerSelecetionStateChange = true;
		assert(entitiesUnselected->size() == 0); // unselect() should not write on selectedEntities
	} else {
		// already in selection state
		if ((*selectedEntities)[0].entity != pickedEntity.entity) {
			assert(contains(entitiesUnselected, (*selectedEntities)[0].entity));
			entitiesToSelect->push_back(pickedEntity);
		} else {
			// selected the same entity again
			removeEntityFromList(entitiesUnselected, pickedEntity.entity);

			// overwrite picking offset
			(*selectedEntities)[0].pickingOffset = pickedEntity.pickingOffset;
		}
		triggerSelecetionStateChange = false;
	}

	// 	dumpVec(pickedEntity.pickingOffset.position, "VirtualHandSelectionChangeModel: picking pos");
}

void VirtualHandSelectionChangeModel::unselect(std::vector<PICKEDENTITY>* selectedEntities,
		std::vector<PICKEDENTITY>* entitiesToUnselect) {
	PICKEDENTITY pickedEntity;

	assert(selectedEntities);
	assert(entitiesToUnselect);
	assert(selectedEntities->size()<=1);

	entitiesToUnselect->clear();
	pickedEntity = getEntityTouched();

	if (pickedEntity.entity == NULL && distanceThreshold > 0)
		pickedEntity = getNearbyEntity();

	if (selectedEntities->size() == 1) {
		if ((*selectedEntities)[0].entity == pickedEntity.entity) {
			// selected entity remains selected
			triggerUnselectionStateChange = false;
		} else if (pickedEntity.entity == NULL) {
			// remove selected entity from selection and trigger changeState
			entitiesToUnselect->push_back((*selectedEntities)[0]);
			triggerUnselectionStateChange = true;
		} else {
			// unselect entity previousely selected (no statechange will occure, if the same model is choosen during selecting)
			entitiesToUnselect->push_back((*selectedEntities)[0]);
			triggerUnselectionStateChange = true;
		}
	} else {
		// empty selection, suggest state change
		triggerUnselectionStateChange = true;
	}
}

std::string VirtualHandSelectionChangeModel::getName() {
	return "VirtualHandSelectionChangeModel";
}

bool VirtualHandSelectionChangeModel::changeState() {
	assert(!triggerSelecetionStateChange || !triggerUnselectionStateChange);
	return triggerSelecetionStateChange || triggerUnselectionStateChange;
}

PICKEDENTITY VirtualHandSelectionChangeModel::getEntityTouched() {
	int i;
	std::vector<Entity*>::iterator it;
	//	std::vector<Entity*>* possibleResults;
	Entity* entity;
	User* localUser;
	TransformationData cursorTransf;
	TransformationData pickingOffset;
	PICKEDENTITY ret;

	ret.entity = NULL;
	ret.pickingOffset = identityTransformation();
	localUser = UserDatabase::getLocalUser();
	cursorTransf = localUser->getCursorTransformation();

	// TODO: environment may differ between selection instance and unselection instance
	// using the same instance might help, but doesn't comply with our principles :-)
	Environment* environment = WorldDatabase::getEnvironmentAtWorldPosition(
			cursorTransf.position[0], cursorTransf.position[2]);
	if (environment != NULL)
		currentEnvironment = environment;

	//	possibleResults = NULL;
	//	if(currentEnvironment) possibleResults = currentEnvironment->getEntityList();
	//
	//	if(!possibleResults || (possibleResults->size() == 0))
	//	{
	//		return ret;
	//	}

	if (!currentEnvironment)
		return ret;

	const std::vector<Entity*>& possibleResults = currentEnvironment->getEntityList();
	if (possibleResults.size() == 0)
		return ret;

	entity = NULL;
	for (i = 0; i < (int)possibleResults.size(); i++) {
		if ((possibleResults)[i]->isFixed())
			continue;
		if (isInside(cursorTransf.position, (possibleResults)[i])) {
			entity = (possibleResults)[i];
			i++;
			break;
		}
	}

	if (!entity)
		return ret;

	for (; i < (int)possibleResults.size(); i++) {
		if ((possibleResults)[i]->isFixed() || !isInside(cursorTransf.position,
				(possibleResults)[i]))
			continue;
		entity = smallerBBox(entity, (possibleResults)[i]);
	} // for

	// TODO: check if position and orientation are in world or in environment coordinates
	TransformationData entityTrans = entity->getWorldTransformation();
	// 	entityTrans.scale = gmtl::Vec3f(1,1,1);
	TransformationData userTrans = identityTransformation();
	userTrans.position = cursorTransf.position;
	userTrans.orientation = cursorTransf.orientation;

	// 	invert(entityTrans);
	invert(userTrans);
	multiply(pickingOffset, userTrans, entityTrans);

	ret.pickingOffset = pickingOffset;
	ret.entity = entity;
	return ret;
}

PICKEDENTITY VirtualHandSelectionChangeModel::getNearbyEntity() {
	int i;
	std::vector<Entity*>::iterator it;
	//	std::vector<Entity*>* possibleResults;
	Entity* entity;
	User* localUser;
	TransformationData cursorTransf;
	TransformationData pickingOffset;
	PICKEDENTITY ret;
	float distance;
	float minDistance = distanceThreshold;

	//	printd(INFO, "VirtualHandSelectionChangeModel::getNearbyEntity(): called!\n");

	ret.entity = NULL;
	ret.pickingOffset = identityTransformation();
	localUser = UserDatabase::getLocalUser();
	cursorTransf = localUser->getCursorTransformation();

	// TODO: environment may differ between selection instance and unselection instance
	// using the same instance might help, but doesn't comply with our principles :-)
	Environment* environment = WorldDatabase::getEnvironmentAtWorldPosition(
			cursorTransf.position[0], cursorTransf.position[2]);
	if (environment != NULL)
		currentEnvironment = environment;

	//	possibleResults = NULL;
	//	if(currentEnvironment) possibleResults = currentEnvironment->getEntityList();
	//	if(!possibleResults || (possibleResults->size() == 0))
	//		return ret;

	if (!currentEnvironment)
		return ret;

	const std::vector<Entity*>& possibleResults = currentEnvironment->getEntityList();
	if (possibleResults.size() == 0)
		return ret;

	entity = NULL;
	for (i = 0; i < (int)possibleResults.size(); i++) {
		if ((possibleResults)[i]->isFixed())
			continue;
		distance = getMinDistanceToAABBox(cursorTransf.position, (possibleResults)[i]);
		//		printd(INFO, "Found minimum distance %f\n", distance);
		if (distance < distanceThreshold && distance < minDistance) {
			entity = (possibleResults)[i];
			minDistance = distance;
		} // if
	}

	if (!entity)
		return ret;

	TransformationData entityTrans = entity->getWorldTransformation();
	// 	entityTrans.scale = gmtl::Vec3f(1,1,1);
	TransformationData userTrans = identityTransformation();
	userTrans.position = cursorTransf.position;
	userTrans.orientation = cursorTransf.orientation;

	// 	invert(entityTrans);
	invert(userTrans);
	multiply(pickingOffset, userTrans, entityTrans);

	ret.pickingOffset = pickingOffset;
	ret.entity = entity;

	//	printd(INFO, "VirtualHandSelectionChangeModel::getNearbyEntity(): found entity at distance %f (threshold = %f)!\n", minDistance, distanceThreshold);

	return ret;
} // getNearbyEntity


