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

#ifndef _NAVIGATIONEVENTS_H
#define _NAVIGATIONEVENTS_H

#include "NavigationSharedLibraryExports.h"
#include <inVRs/SystemCore/EventManager/Event.h>
#include <inVRs/SystemCore/EventManager/EventFactory.h>

class INVRS_NAVIGATION_API NavigationPauseEvent : public Event {
public:

	NavigationPauseEvent(unsigned srcModuleId); // constructor for sender
	NavigationPauseEvent(); // constructor for factory

	typedef EventFactory<NavigationPauseEvent> Factory;

	virtual void execute();

}; // NavigationPauseNavigationEvent

class INVRS_NAVIGATION_API NavigationResumeEvent : public Event {
public:

	NavigationResumeEvent(unsigned srcModuleId);
	NavigationResumeEvent();

	typedef EventFactory<NavigationResumeEvent> Factory;

	virtual void execute();
};

//class NavigationEventsFactory : public EventFactory {
//public:
//	virtual Event* create(unsigned eventId);
//}; // NavigationPauseEventFactory

#endif // _NAVIGATIONPAUSEEVENT_H
