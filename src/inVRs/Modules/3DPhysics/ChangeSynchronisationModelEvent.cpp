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

#include "ChangeSynchronisationModelEvent.h"

#include "Physics.h"

#include <inVRs/SystemCore/SystemCore.h>
#include <inVRs/SystemCore/MessageFunctions.h>

//*****************//
// PUBLIC METHODS: //
//*****************//

ChangeSynchronisationModelEvent::ChangeSynchronisationModelEvent()
: Event()
{
	this->modelName = "";
	this->args = NULL;
} // ChangeSynchronisationModelEvent

ChangeSynchronisationModelEvent::ChangeSynchronisationModelEvent(std::string modelName, ArgumentVector* args)
: Event(USER_DEFINED_ID, PHYSICS_MODULE_ID, "ChangeSynchronisationModelEvent") 
{
	this->modelName = modelName;
	this->args = args;
} // ChangeSynchronisationModelEvent

ChangeSynchronisationModelEvent::~ChangeSynchronisationModelEvent()
{
	if (args)
		delete args;
} // ~ChangeSynchronisationModelEvent

//**************************************//
// PUBLIC METHODS INHERITED FROM: Event //
//**************************************//

void ChangeSynchronisationModelEvent::encode(NetMessage* message)
{
	msgFunctions::encode(modelName, message);
	if (args) {
		msgFunctions::encode(true, message);
		args->addToBinaryMessage(message);
	}
	else
		msgFunctions::encode(false, message);	
} // encode

void ChangeSynchronisationModelEvent::decode(NetMessage* message)
{
	bool hasArguments = false;
	msgFunctions::decode(modelName, message);
	msgFunctions::decode(hasArguments, message);
	if (hasArguments) {
		args = ArgumentVector::readFromBinaryMessage(message);
	} // if
} // decode

void ChangeSynchronisationModelEvent::execute()
{
	Physics* physicsModule;

	physicsModule = (Physics*)SystemCore::getModuleByName("Physics");
	if (!physicsModule)
	{
		printd(ERROR, "ChangeSynchronisationModelEvent::execute(): ERROR: Could not get Physics Module! Ignoring Event!\n");
		return;
	} // if

	physicsModule->setSynchronisationModel(modelName, args);
} // execute

std::string ChangeSynchronisationModelEvent::toString()
{
// TODO:
	return Event::toString();
} // toString

