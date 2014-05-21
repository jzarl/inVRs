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

#include "AccelerationSynchronisationModelFactory.h"

#include "AccelerationSynchronisationModel.h"

//************************************************************//
// PUBLIC METHODS DERIVED FROM: SynchronisationModelFactory: *//
//************************************************************//

SynchronisationModel* AccelerationSynchronisationModelFactory::create(std::string className, void* args)
{
	if (className != "AccelerationSynchronisationModel")
		return NULL;

	float linearThreshold = 0;
	float angularThreshold = 0;
	float convergenceTime = -1;
	int convergenceAlgorithm = -1;
	
	ArgumentVector* arguments = (ArgumentVector*)args;
	
	if (arguments->keyExists("linearThreshold"))
		arguments->get("linearThreshold", linearThreshold);
	else
		printd(WARNING, "AccelerationSynchronisationModelFactory::create(): no linear Threshold found in xml-file!!!\n");
	
	if (arguments->keyExists("angularThreshold"))
		arguments->get("angularThreshold", angularThreshold);
	else
		printd(WARNING, "AccelerationSynchronisationModelFactory::create(): no angular Threshold found in xml-file!!!\n");

	if (arguments->keyExists("convergenceAlgorithm"))
		arguments->get("convergenceAlgorithm", convergenceAlgorithm);
	
	if (arguments->keyExists("convergenceTime"))
		arguments->get("convergenceTime", convergenceTime);
	
	if (convergenceAlgorithm < 0)
		return new AccelerationSynchronisationModel(linearThreshold, angularThreshold);
	if (convergenceTime < 0)
		return new AccelerationSynchronisationModel(linearThreshold, angularThreshold, convergenceAlgorithm);
	return new AccelerationSynchronisationModel(linearThreshold, angularThreshold, convergenceAlgorithm, convergenceTime);
} // create
