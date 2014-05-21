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

#ifndef MANIPULATIONCHANGEMODEL_H_
#define MANIPULATIONCHANGEMODEL_H_

#include "StateTransitionModel.h"

class ManipulationChangeModel : public StateTransitionModel {
public:

	/**
	 * Empty destructor
	 */
	virtual ~ManipulationChangeModel() {};

	virtual void enterTransitionSource() = 0;

}; // ManipulationChangeModel

typedef ClassFactory<ManipulationChangeModel> ManipulationChangeModelFactory;

class INVRS_INTERACTION_API DefaultManipulationChangeModelsFactory
	: public ManipulationChangeModelFactory {
public:
	ManipulationChangeModel* create(std::string className, void* args = NULL);
}; // DefaultManipulationChangeModelsFactory

#endif /* MANIPULATIONCHANGEMODEL_H_ */
