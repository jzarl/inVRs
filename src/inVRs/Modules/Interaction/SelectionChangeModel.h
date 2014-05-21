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

#ifndef _SELECTIONCHANGEMODEL_H
#define _SELECTIONCHANGEMODEL_H

#include <vector>

#include "InteractionSharedLibraryExports.h"
#include <inVRs/SystemCore/ClassFactory.h>
#include <inVRs/SystemCore/WorldDatabase/Entity.h>
#include <inVRs/SystemCore/DataTypes.h>

// pickingOffset is relative to the point where the entity has been grabbed
// in most cases that point would be the cursor transformation (in matrix notation: matrix form entity space into cursor space)
struct PICKEDENTITY {
	Entity* entity;
	TransformationData pickingOffset;
};

class INVRS_INTERACTION_API SelectionChangeModel {
public:

	virtual ~SelectionChangeModel();

	// entitiesToSelect: output parameter, will contain entities which should be added
	// entitiesToUnselect: entities which have been unselected by a previouse unselect() call
	virtual void select(std::vector<PICKEDENTITY>* selectedEntities,
			std::vector<PICKEDENTITY>* entitiesToSelect,
			std::vector<PICKEDENTITY>* entitiesUnselected) = 0;
	virtual void unselect(std::vector<PICKEDENTITY>* selectedEntities,
			std::vector<PICKEDENTITY>* entitiesToUnselect) = 0;

	virtual bool changeState() = 0;
	virtual std::string getName() = 0;

	// some useful utility functions:
	static int getIdxOfEntity(std::vector<PICKEDENTITY>* entityList, Entity* ent);
	static bool contains(std::vector<PICKEDENTITY>* entityList, Entity* ent);
	static bool removeEntityFromList(std::vector<PICKEDENTITY>* entityList, Entity* ent);

};

class SelectionChangeModelFactory : public ClassFactory<SelectionChangeModel> {
public:

	SelectionChangeModel* create(std::string className, void* args = NULL) = 0;
};

class DefaultSelectionChangeModelsFactory : public SelectionChangeModelFactory {
public:

	SelectionChangeModel* create(std::string className, void* args = NULL);
};

#endif
