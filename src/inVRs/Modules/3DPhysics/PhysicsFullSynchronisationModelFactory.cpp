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

#include "PhysicsFullSynchronisationModelFactory.h"

#include "PhysicsFullSynchronisationModel.h"
#include <inVRs/SystemCore/ArgumentVector.h>
#include <inVRs/SystemCore/DebugOutput.h>

//************************************************************//
// PUBLIC METHODS DERIVED FROM: SynchronisationModelFactory: *//
//************************************************************//

SynchronisationModel* PhysicsFullSynchronisationModelFactory::create(std::string className, void* args)
{
	if (className != "PhysicsFullSynchronisationModel")
		return NULL;

	int updateInterval = -1;
	int convergenceAlgorithm = -1;
	float convergenceTime = -1;

	ArgumentVector* argVec = (ArgumentVector*)args;
	
	if (!argVec)
		return new PhysicsFullSynchronisationModel;
	
	if (argVec->keyExists("updateInterval"))
		argVec->get("updateInterval", updateInterval);
	else
		printd(WARNING, "PhysicsFullSynchronisationModelFactory::create(): no updateInterval found in xml-file!!!\n");

	if (argVec->keyExists("convergenceAlgorithm"))
		argVec->get("convergenceAlgorithm", convergenceAlgorithm);
	
	if (argVec->keyExists("convergenceTime"))
		argVec->get("convergenceTime", convergenceTime);

	if (updateInterval <= 0) {
		printd(ERROR, "PhysicsFullSynchronisationModelFactory::create(): invalid update inverval %i found (must be positive)!\n", updateInterval);
		return new PhysicsFullSynchronisationModel;
	} // if

	if (convergenceAlgorithm < 0)
		return new PhysicsFullSynchronisationModel(updateInterval);
	if (convergenceTime < 0)
		return new PhysicsFullSynchronisationModel(updateInterval, convergenceAlgorithm);

	return new PhysicsFullSynchronisationModel(updateInterval, convergenceAlgorithm, convergenceTime);
} // create
