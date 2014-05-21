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

#include "RayCastSelectionChangeModel.h"

#include <assert.h>

#include <gmtl/QuatOps.h>
#include <gmtl/VecOps.h>
#include <gmtl/Generate.h>
#include <gmtl/Xforms.h>

#include <inVRs/SystemCore/UserDatabase/UserDatabase.h>
#include <inVRs/SystemCore/DataTypes.h>
#include <inVRs/SystemCore/WorldDatabase/WorldDatabase.h>
#include <inVRs/SystemCore/WorldDatabase/Environment.h>

RayCastSelectionChangeModel::RayCastSelectionChangeModel() {
	triggerSelecetionStateChange = false;
	triggerUnselectionStateChange = false;
	usePickingOffset = true;
	//visualisationCreated = true;
	//rayVisualisation = NULL;
	//localUser = UserDatabase::getLocalUser();

}

RayCastSelectionChangeModel::RayCastSelectionChangeModel(bool usePickOffset) {
	triggerSelecetionStateChange = false;
	triggerUnselectionStateChange = false;
	usePickingOffset = usePickOffset;
}

/*void RayCastSelectionChangeModel::entityCreated(Entity* ent)
 {
 rayVisualisation = ent;
 }*/

RayCastSelectionChangeModel::~RayCastSelectionChangeModel() {

}

void RayCastSelectionChangeModel::select(std::vector<PICKEDENTITY>* selectedEntities, std::vector<
		PICKEDENTITY>* entitiesToSelect, std::vector<PICKEDENTITY>* entitiesUnselected) {

	Entity* ent;
	PICKEDENTITY pickedEntity;

	assert(selectedEntities);
	assert(entitiesToSelect);
	assert(entitiesUnselected);
	assert(selectedEntities->size()<=1);

	entitiesToSelect->clear();

	ent = getEntityCursorIsPointingAt();

	// 	if(ent) fprintf(stderr, "RayCastSelectionChangeModel: pointing at %u\n", ent->getTypeBasedId());

	if (selectedEntities->size() == 0) {

		if (!ent)
			return; // nothing selected, stay in idle state


		// we are in idle state, switch to selected state
		if (usePickingOffset)
			pickedEntity.pickingOffset = pickingOffset;
		else
			pickedEntity.pickingOffset = identityTransformation();
		pickedEntity.entity = ent;
		entitiesToSelect->push_back(pickedEntity);
		triggerSelecetionStateChange = true;
		assert(entitiesUnselected->size() == 0); // unselect() should not write on selectedEntities
	} else {
		// already in selection state
		if ((*selectedEntities)[0].entity != ent) {
			assert(contains(entitiesUnselected, (*selectedEntities)[0].entity));

			if (ent != NULL) {
				if (usePickingOffset)
					pickedEntity.pickingOffset = pickingOffset;
				else
					pickedEntity.pickingOffset = identityTransformation();

				pickedEntity.entity = ent;
				entitiesToSelect->push_back(pickedEntity);
			}
		} else {
			// selected the same entity again
			if (usePickingOffset) {
				(*selectedEntities)[0].pickingOffset = pickingOffset;
			}
			removeEntityFromList(entitiesUnselected, ent);
		}
		triggerSelecetionStateChange = false;
	}

}

void RayCastSelectionChangeModel::unselect(std::vector<PICKEDENTITY>* selectedEntities,
		std::vector<PICKEDENTITY>* entitiesToUnselect) {
	Entity* ent;
	PICKEDENTITY pickedEntity;

	assert(selectedEntities);
	assert(entitiesToUnselect);
	assert(selectedEntities->size()<=1);

	entitiesToUnselect->clear();
	ent = getEntityCursorIsPointingAt();

	// 	if(ent) fprintf(stderr, "RayCastSelectionChangeModel: pointing at %u\n", ent->getTypeBasedId());

	if (selectedEntities->size() == 1) {
		if ((*selectedEntities)[0].entity == ent) {
			// selected entity remains selected
			triggerUnselectionStateChange = false;
		} else if (ent == NULL) {
			// remove selected entity from selection and trigger changeState
			entitiesToUnselect->push_back((*selectedEntities)[0]);
			triggerUnselectionStateChange = true;
		} else {
			// unselect entity previousely selected (no statechange will occure, if the same model is choosen for selecting)
			entitiesToUnselect->push_back((*selectedEntities)[0]);
			triggerUnselectionStateChange = true;
		}
	} else {
		// empty selection, suggest state change
		triggerUnselectionStateChange = true;
	}
}

bool RayCastSelectionChangeModel::changeState() {
	assert(!triggerSelecetionStateChange || !triggerUnselectionStateChange);
	return triggerSelecetionStateChange || triggerUnselectionStateChange;
}

std::string RayCastSelectionChangeModel::getName() {
	return "RayCastSelectionChangeModel";
}

float RayCastSelectionChangeModel::getRayDistance() {
	return rayDistance;
}

gmtl::Vec3f RayCastSelectionChangeModel::getRayDirection() {
	return rayDirection;
}

gmtl::Vec3f RayCastSelectionChangeModel::getRayDestination() {
	return rayDestination;
}

Entity* RayCastSelectionChangeModel::getEntityCursorIsPointingAt() {
	User* localUser;
	Environment* env;
	TransformationData cursorTransf;
	gmtl::Vec3f direction = gmtl::Vec3f(0, 0, -1);
	float d;
	Entity* ret = NULL;

	localUser = UserDatabase::getLocalUser();
	cursorTransf = localUser->getCursorTransformation();

	direction = cursorTransf.orientation * direction;
	env = WorldDatabase::getEnvironmentAtWorldPosition(cursorTransf.position[0],
			cursorTransf.position[2]);

	if (!env) {
//		printd(
//				INFO,
//				"RayCastSelectionChangeModel::getEntityCursorIsPointingAt(): couldn't find any environment at (%0.03f, %0.03f)\n",
//				cursorTransf.position[0], cursorTransf.position[1]);
		return NULL;
	}

	if (env->rayIntersect(cursorTransf.position, direction, &d, &ret)) {
		rayDirection = direction;
		rayDistance = d;
		if (usePickingOffset) {
			TransformationData entTrans = ret->getWorldTransformation();
			TransformationData rayCursorTrans = identityTransformation();
			rayCursorTrans.position = cursorTransf.position + direction * d;
			rayCursorTrans.orientation = cursorTransf.orientation;
			rayDestination = rayCursorTrans.position;
			entTrans.scale = gmtl::Vec3f(1, 1, 1);
			invert(rayCursorTrans);
			multiply(pickingOffset, rayCursorTrans, entTrans);
		}
		// 		printd("RayCastSelectionChangeModel::select(): found something\n");
	}

	return ret;
}


