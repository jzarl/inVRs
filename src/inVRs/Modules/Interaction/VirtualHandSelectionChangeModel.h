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

#ifndef _VIRTUALHANDSELECTIONCHANGEMODEL_H
#define _VIRTUALHANDSELECTIONCHANGEMODEL_H

#include "SelectionChangeModel.h"

class INVRS_INTERACTION_API VirtualHandSelectionChangeModel : public SelectionChangeModel {
public:

	VirtualHandSelectionChangeModel(float distanceThreshold = 0);
	virtual ~VirtualHandSelectionChangeModel();

	virtual void select(std::vector<PICKEDENTITY>* selectedEntities,
			std::vector<PICKEDENTITY>* entitiesToSelect,
			std::vector<PICKEDENTITY>* entitiesUnselected);
	virtual void unselect(std::vector<PICKEDENTITY>* selectedEntities,
			std::vector<PICKEDENTITY>* entitiesToUnselect);
	virtual std::string getName();
	virtual bool changeState();

protected:

	PICKEDENTITY getEntityTouched();
	PICKEDENTITY getNearbyEntity();
	void generateEntityList(Environment* environment);

	bool triggerSelecetionStateChange;
	bool triggerUnselectionStateChange;

	float distanceThreshold;

	Environment* currentEnvironment;
	//	std::vector< Entity* > entityList;
};

#endif
