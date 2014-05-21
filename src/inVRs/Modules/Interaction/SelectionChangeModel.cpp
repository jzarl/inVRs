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

#include "SelectionChangeModel.h"

#include "RayCastSelectionChangeModel.h"
#include "VirtualHandSelectionChangeModel.h"
#include "LimitRayCastSelectionChangeModel.h"

SelectionChangeModel::~SelectionChangeModel() {

}

int SelectionChangeModel::getIdxOfEntity(std::vector<PICKEDENTITY>* entityList, Entity* ent) {
	int i;
	for (i = 0; i < (int)entityList->size(); i++) {
		if ((*entityList)[i].entity == ent)
			return i;
	}
	return -1;
}

bool SelectionChangeModel::contains(std::vector<PICKEDENTITY>* entityList, Entity* ent) {
	return getIdxOfEntity(entityList, ent) >= 0;
}

bool SelectionChangeModel::removeEntityFromList(std::vector<PICKEDENTITY>* entityList, Entity* ent) {
	int idx = getIdxOfEntity(entityList, ent);
	if (idx >= 0) {
		entityList->erase(entityList->begin() + idx);
		return true;
	}
	return false;
}

SelectionChangeModel* DefaultSelectionChangeModelsFactory::create(std::string className, void* args) {
	if (className == "RayCastSelectionChangeModel") {
		bool usePickingOffset;
		ArgumentVector* argVec = (ArgumentVector*)args;

		if (argVec && argVec->keyExists("usePickingOffset")) {
			argVec->get("usePickingOffset", usePickingOffset);
			return new RayCastSelectionChangeModel(usePickingOffset);
		} // if
		return new RayCastSelectionChangeModel();
	}

	if (className == "VirtualHandSelectionChangeModel") {
		float distance;
		ArgumentVector* argVec = (ArgumentVector*)args;

		if (argVec && argVec->keyExists("distanceThreshold")) {
			argVec->get("distanceThreshold", distance);
			return new VirtualHandSelectionChangeModel(distance);
		} // if

		return new VirtualHandSelectionChangeModel();
	} // if

	if (className == "LimitRayCastSelectionChangeModel") {

		ArgumentVector* argVec = (ArgumentVector*)args;
		float rayDistanceThreshold = 10000;

		if (argVec && argVec->keyExists("rayDistanceThreshold")) {
			argVec->get("rayDistanceThreshold", rayDistanceThreshold);
		} // if
		return new LimitRayCastSelectionChangeModel(rayDistanceThreshold);
	}

	return NULL;
}

