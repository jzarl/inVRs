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

#ifndef _RAYCASTSELECTIONCHANGEMODEL_H
#define _RAYCASTSELECTIONCHANGEMODEL_H

#include <vector>

#include "SelectionChangeModel.h"
#include <inVRs/SystemCore/UserDatabase/User.h>

class INVRS_INTERACTION_API RayCastSelectionChangeModel : public SelectionChangeModel {
public:

	RayCastSelectionChangeModel();
	RayCastSelectionChangeModel(bool usePickOffset);
	virtual ~RayCastSelectionChangeModel();

	virtual void select(std::vector<PICKEDENTITY>* selectedEntities,
			std::vector<PICKEDENTITY>* entitiesToSelect,
			std::vector<PICKEDENTITY>* entitiesUnselected);
	virtual void unselect(std::vector<PICKEDENTITY>* selectedEntities,
			std::vector<PICKEDENTITY>* entitiesToUnselect);
	virtual bool changeState();
	virtual std::string getName();

	virtual float getRayDistance();
	virtual gmtl::Vec3f getRayDirection();
	virtual gmtl::Vec3f getRayDestination();

protected:

	Entity* getEntityCursorIsPointingAt();

	bool triggerSelecetionStateChange;
	bool triggerUnselectionStateChange;
	bool usePickingOffset;
	bool visualisationCreated;
	TransformationData pickingOffset;
	gmtl::Vec3f rayDirection;
	float rayDistance;
	gmtl::Vec3f rayDestination;
};

#endif
