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

#ifndef _CHANGEDSYNCHRONISATIONMODEL_H
#define _CHANGEDSYNCHRONISATIONMODEL_H

#include "SynchronisationModel.h"
#include "Physics.h"

#include "MessageSizeCounter.h"

#include <inVRs/SystemCore/ComponentInterfaces/NetworkInterface.h>
#include <inVRs/SystemCore/UserDatabase/User.h>

/** SynchronisationModel which synchronises every step only changed data.
 * After each simulation step the transformations of the rigid bodies which have
 * changed are distributed.
 */
class ChangedSynchronisationModel : public SynchronisationModel
{

public:

//*****************//
// PUBLIC METHODS: //
//*****************//

	/** Constructor initializes network-pointer.
	 */
	ChangedSynchronisationModel();

	/** Empty destructor.
	 */
	virtual ~ChangedSynchronisationModel();

//*******************************************************//
// PUBLIC METHODS INHERITED FROM: SynchronisationModel: *//
//*******************************************************//

	virtual void synchroniseBeforeStep();
	virtual void synchroniseAfterStep();

protected:

	static const unsigned CHANGEDSYNCHRONISATION_MESSAGEID;

//********************//
// PROTECTED METHODS: //
//********************//

//**********************************************************//
// PROTECTED METHODS INHERITED FROM: SynchronisationModel: *//
//**********************************************************//

//	virtual void handleMessages();
	virtual void handleSyncMessage(NetMessage* msg);
	virtual void handleClientInputMessage(NetMessage* msg);

//********************//
// PROTECTED MEMBERS: //
//********************//

	/// Counter for logging the amount of bytes transmitted for synchronisation
	MessageSizeCounter* counter;

	std::map<uint64_t, int> inactiveRepeatSend;
}; // ChangedSynchronisationModel

#endif // _CHANGEDSYNCHRONISATIONMODEL_H
