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


#include <inVRs/SystemCore/MessageFunctions.h>
#include <inVRs/SystemCore/EventManager/EventManager.h>

#include "ActionEvent.h"
#include "SyncDActionsEvent.h"
#include "UndoDActionEvent.h"
#include "ExecuteDActionEvent.h"
#include "InsertLatticeDActionEvent.h"
#include "RemoveLatticeDActionEvent.h"
#include "BendDeformActionEvent.h"
#include "TwistDeformActionEvent.h"
#include "TaperDeformActionEvent.h"
#include "GlobalDeformActionEvent.h"
#include "SetPointDeformActionEvent.h"

#include "../Deformation.h"
#include "../InVRsDeformActionManager.h"

ActionEvent::ActionEvent(string actionEventName)
: Event(DEFORMATION_MODULE_ID, DEFORMATION_MODULE_ID, actionEventName)
{
}

// virtual
ActionEvent::~ActionEvent()
{
}

void ActionEvent::registerActionEventFactories()
{
	EventManager::registerEventFactory("SyncDActionsEvent",
		new SyncDActionsEvent::Factory());

	EventManager::registerEventFactory("UndoDActionEvent",
		new UndoDActionEvent::Factory());

	EventManager::registerEventFactory("InsertLatticeDActionEvent",
		new InsertLatticeDActionEvent::Factory());

	EventManager::registerEventFactory("ExecuteDActionEvent",
		new ExecuteDActionEvent::Factory());

	EventManager::registerEventFactory("RemoveLatticeDActionEvent",
		new RemoveLatticeDActionEvent::Factory());

	EventManager::registerEventFactory("BendDeformActionEvent",
		new BendDeformActionEvent::Factory());

	EventManager::registerEventFactory("TwistDeformActionEvent",
		new TwistDeformActionEvent::Factory());

	EventManager::registerEventFactory("TaperDeformActionEvent",
		new TaperDeformActionEvent::Factory());

	EventManager::registerEventFactory("GlobalDeformActionEvent",
		new GlobalDeformActionEvent::Factory());

	EventManager::registerEventFactory("SetPointDeformActionEvent",
		new SetPointDeformActionEvent::Factory());
}
