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

#ifndef _STATEACTIONMODEL_H
#define _STATEACTIONMODEL_H

#include <inVRs/SystemCore/ClassFactory.h>
#include <inVRs/SystemCore/WorldDatabase/Entity.h>
#include "SelectionChangeModel.h"

/**
 * @class StateActionModel
 * @brief Abstract class for Action-Models in the states of the DFA.
 */
class INVRS_INTERACTION_API StateActionModel {
public:

	//*****************//
	// PUBLIC METHODS: //
	//*****************//

	/** Empty destructor.
	 */
	virtual ~StateActionModel() {};

	/** Called when the state is entered.
	 */
	virtual void enter(std::vector<PICKEDENTITY>* entities) = 0;

	/** Called when the state is left.
	 */
	virtual void exit() = 0;

	/** Called when the DFA is in the state where the Model is registered for.
	 */
	virtual void action(std::vector<PICKEDENTITY>* addedEntities,
			std::vector<PICKEDENTITY>* removedEntities, float dt) = 0;

}; // StateActionModel

typedef ClassFactory<StateActionModel> StateActionModelFactory;

// class DefaultStateActionModelFactory : public StateActionModelFactory
// {
// public
// 	StateActionModel* create(std::string className, void* args = NULL);
// };

#endif // _STATEACTIONMODEL_H
