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

#include "PhysicsInputSynchronisationModelFactory.h"

#include "PhysicsInputSynchronisationModel.h"
#include <inVRs/SystemCore/ArgumentVector.h>
#include <inVRs/SystemCore/DebugOutput.h>

//************************************************************//
// PUBLIC METHODS DERIVED FROM: SynchronisationModelFactory: *//
//************************************************************//

SynchronisationModel* PhysicsInputSynchronisationModelFactory::create(std::string className, void* args)
{
	if (className != "PhysicsInputSynchronisationModel")
		return NULL;

	ArgumentVector* argVec = (ArgumentVector*)args;
	float updateInterval;
	float inputUpdateInterval;
	
	if (!argVec || !argVec->keyExists("updateInterval"))
		return new PhysicsInputSynchronisationModel;
	
	argVec->get("updateInterval", updateInterval);
	
	if (updateInterval <= 0) {
		printd(ERROR, "PhysicsInputSynchronisationModelFactory::create(): invalid update inverval %f found (must be positive)!\n", updateInterval);
		return new PhysicsInputSynchronisationModel;
	} // if
	
	if (!argVec->keyExists("inputUpdateInterval"))
		return new PhysicsInputSynchronisationModel(updateInterval);
	
	argVec->get("inputUpdateInterval", inputUpdateInterval);
	if (inputUpdateInterval < 0) {
		printd(ERROR, "PhysicsInputSynchronisationModelFactory::create(): invalid input update inverval %f found (must be positive)!\n", inputUpdateInterval);
		return new PhysicsInputSynchronisationModel(updateInterval);
	} // if
	
	return new PhysicsInputSynchronisationModel(updateInterval, inputUpdateInterval);
} // create
