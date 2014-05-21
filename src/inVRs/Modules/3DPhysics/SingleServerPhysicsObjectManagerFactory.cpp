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

#include "SingleServerPhysicsObjectManagerFactory.h"

#include <inVRs/SystemCore/XMLTools.h>
#include <inVRs/SystemCore/DebugOutput.h>

PhysicsObjectManager* SingleServerPhysicsObjectManagerFactory::create(std::string className,
		ArgumentVector* args)
{
	if (className != "SingleServerPhysicsObjectManager")
		return NULL;

	SingleServerPhysicsObjectManager* result = NULL;
	std::string serverMode;

	if (args && args->keyExists("serverMode")) {
		args->get("serverMode", serverMode);
		result = new SingleServerPhysicsObjectManager();
		result->setServerMode(serverMode);
	} // if
	else {
		printd(ERROR,
				"SingleServerPhysicsObjectManagerFactory::create(): COULD NOT FIND ENTRY serverMode IN CONFIG-FILE!\n");
	} // if

	return result;
} // create
