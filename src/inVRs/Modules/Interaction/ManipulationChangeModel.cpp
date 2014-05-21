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

#include "ManipulationChangeModel.h"
#include "ButtonPressManipulationChangeModel.h"
#include <inVRs/SystemCore/ArgumentVector.h>
#include <inVRs/SystemCore/DebugOutput.h>

ManipulationChangeModel* DefaultManipulationChangeModelsFactory::create(std::string className,
		void* args) {
	if (className != "ButtonPressManipulationChangeModel" &&
			className != "ButtonPressStateTransitionModel") {
		return NULL;
	} // if
	//TODO: automatically convert className in config file
	if (className == "ButtonPressStateTransitionModel") {
		printd(WARNING,
				"DefaultManipulationChangeModelsFactory::create(): Deprecated model ButtonPressStateTransitionModel found in Interaction configuration file! Please use ButtonPressManipulationChangeModel instead!\n");
	} // if

	int buttonIndex = 0;
	int buttonValue = 1;
	ArgumentVector* argVec = (ArgumentVector*)args;

	if (!argVec)
		return new ButtonPressManipulationChangeModel();

	if (argVec->keyExists("buttonIndex"))
		argVec->get("buttonIndex", buttonIndex);
	if (argVec->keyExists("buttonValue"))
		argVec->get("buttonValue", buttonValue);
	return new ButtonPressManipulationChangeModel(buttonIndex, buttonValue);
} // create
