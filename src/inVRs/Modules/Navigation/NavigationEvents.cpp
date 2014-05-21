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

#include <sstream>

#include "NavigationEvents.h"
#include <inVRs/SystemCore/UserDatabase/UserDatabase.h>
#include <inVRs/SystemCore/ComponentInterfaces/NavigationInterface.h>
#include <inVRs/SystemCore/DebugOutput.h>
#include <inVRs/SystemCore/EventManager/Event.h>
#include "Navigation.h"
#include <inVRs/SystemCore/SystemCore.h>

NavigationPauseEvent::NavigationPauseEvent(unsigned srcModuleId) :
	Event(srcModuleId, NAVIGATION_MODULE_ID, "NavigationPauseEvent") {

}

NavigationPauseEvent::NavigationPauseEvent() :
	Event() {

}

void NavigationPauseEvent::execute() {
	Navigation* nav = (Navigation*)SystemCore::getModuleByName("Navigation");
	if (!nav) {
		printd(ERROR, "NavigationPauseEvent::execute(): ERROR - Navigation module not found!\n");
		return;
	} // if
	nav->suspendNavigation = true;
} // execute

NavigationResumeEvent::NavigationResumeEvent(unsigned srcModuleId) :
	Event(srcModuleId, NAVIGATION_MODULE_ID, "NavigationResumeEvent") {

}

NavigationResumeEvent::NavigationResumeEvent() :
	Event() {

}

void NavigationResumeEvent::execute() {
	Navigation* nav = (Navigation*)SystemCore::getModuleByName("Navigation");
	if (!nav) {
		printd(ERROR, "NavigationPauseEvent::execute(): ERROR - Navigation module not found!\n");
		return;
	} // if
	nav->suspendNavigation = false;
}

