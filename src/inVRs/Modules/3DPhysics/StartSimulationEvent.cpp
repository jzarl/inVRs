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

#include "StartSimulationEvent.h"

#include "Physics.h"

#include <inVRs/SystemCore/MessageFunctions.h>
#include <inVRs/SystemCore/SystemCore.h>

//*****************//
// PUBLIC METHODS: //
//*****************//

StartSimulationEvent::StartSimulationEvent()
: Event(PHYSICS_MODULE_ID, TRANSFORMATION_MANAGER_ID, "StartSimulationEvent") 
{
#ifndef WIN32
	timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	gettimeofday(&tv, NULL);
	double currentTime = ((double)tv.tv_sec+(double)tv.tv_usec/1000000.0);
	startTime = ((double)tv.tv_sec+(double)tv.tv_usec/1000000.0) + 10.f;
	printd(INFO, "StartSimulationEvent(): Current Time = %f\n", currentTime);
	printd(INFO, "StartSimulationEvent(): Start Time = %f\n", startTime);
#else
	printd(ERROR, "StartSimulationEvent(): NOT IMPLEMENTED FOR WINDOWS SYSTEMS!n");
	startTime = timer.getTime();
#endif
} // StartSimulationEvent

StartSimulationEvent::~StartSimulationEvent()
{

} // ~StartSimulationEvent

//**************************************//
// PUBLIC METHODS INHERITED FROM: Event //
//**************************************//

void StartSimulationEvent::encode(NetMessage* message)
{
	printd(INFO, "StartSimulationEvent::encode(): BEGIN\n");

	printd(INFO, "StartSimulationEvent::encode(): encoding time = %f\n", startTime);
	msgFunctions::encode(startTime, message);

	printd(INFO, "StartSimulationEvent::encode(): DONE\n");
} // encode

void StartSimulationEvent::decode(NetMessage* message)
{
	msgFunctions::decode(startTime, message);
	printd(INFO, "StartSimulationEvent::decode(): decoding time = %f\n", startTime);
} // decode

void StartSimulationEvent::execute()
{
	printd(INFO, "StartSimulationEvent::execute(): BEGIN\n");

	Physics* physicsModule = (Physics*)SystemCore::getModuleByName("Physics");
	assert(physicsModule);
	physicsModule->setStartTime(startTime);
	printd(INFO, "StartSimulationEvent::execute(): setting time = %f\n", startTime);

	printd(INFO, "StartSimulationEvent::execute(): DONE\n");
} // execute

std::string StartSimulationEvent::toString()
{
// TODO:
	return Event::toString();
} // toString
