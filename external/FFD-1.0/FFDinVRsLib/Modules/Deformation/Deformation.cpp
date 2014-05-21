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
 *                           Project: FFD                                    *
 *                                                                           *
 * The FFD library was developed during a practical at the Johannes Kepler   *
 * University, Linz in 2009 by Marlene Hochrieser                            *
 *                                                                           *
\*---------------------------------------------------------------------------*/


#include "Deformation.h"

#include "Events/ActionEvent.h"
#include <inVRs/SystemCore/EventManager/EventManager.h>
#include <inVRs/SystemCore/XMLTools.h>
#include <inVRs/SystemCore/Configuration.h>
#include <inVRs/SystemCore/SystemCore.h>
#include <inVRs/SystemCore/WorldDatabase/WorldDatabase.h>
#include <inVRs/SystemCore/ComponentInterfaces/module_core_api.h>

Deformation::Deformation()
{
    ActionEvent::registerActionEventFactories();
    incomingEvents = EventManager::getPipe(DEFORMATION_MODULE_ID);
}

Deformation::~Deformation()
{
}

bool Deformation::loadConfig(std::string configFile)
{
    std::string fullPath = "";

	if (Configuration::containsPath("DeformationConfiguration"))
		fullPath = Configuration::getPath("DeformationConfiguration");
	fullPath += configFile;

	if (!fileExists(fullPath))
	{
		printd(ERROR, "Deformation::loadConfig(): cannot read from file %s\n", fullPath.c_str());
		return false;
	}
    return true;
}

void Deformation::cleanup()
{
}

std::string Deformation::getName()
{
    return "Deformation";
}

Event* Deformation::createSyncEvent()
{
    printd(INFO, "Deformation::createSyncEvent()\n");
    return new SyncDActionsEvent();
}

void Deformation::step(float dt)
{
	if (!incomingEvents) {
		incomingEvents = EventManager::getPipe(DEFORMATION_MODULE_ID);
		if (!incomingEvents) {
			printd(ERROR, "Deformation::step(): failed to fetch my event queue!\n");
			return;
		}
	}

    Event* evt;
	if (incomingEvents) {
		while (incomingEvents->size() > 0) {
			evt = incomingEvents->pop_front();
			evt->execute();
			delete evt;
		}
	}
}

MAKEMODULEPLUGIN(Deformation, SystemCore)
