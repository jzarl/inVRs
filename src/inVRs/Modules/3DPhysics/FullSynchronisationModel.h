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

#ifndef _FULLSYNCHRONISATIONMODEL_H
#define _FULLSYNCHRONISATIONMODEL_H

#include "SynchronisationModel.h"
#include "Physics.h"

#include "MessageSizeCounter.h"

#include <inVRs/SystemCore/ComponentInterfaces/NetworkInterface.h>
#include <inVRs/SystemCore/UserDatabase/User.h>
#ifdef INVRS_BUILD_TIME
#include "oops/SimulationObjectInputListener.h"
#else
#include <inVRs/Modules/3DPhysics/oops/SimulationObjectInputListener.h>
#endif

#include <OpenSG/OSGThreadManager.h>
#include <OpenSG/OSGThread.h>

/** SynchronisationModel which synchronises every step everything.
 * This kind of SynchronisationModel is the easiest and most unefficient way
 * to synchronise physics data. After each simulation step the whole physical
 * state is distributed to all servers and clients.
 */
class FullSynchronisationModel : public SynchronisationModel, oops::SimulationObjectInputListener
{

public:

//*****************//
// PUBLIC METHODS: //
//*****************//

	/** Constructor initializes MessageCounter
	 */
	FullSynchronisationModel();

	/** Empty destructor.
	 */
	virtual ~FullSynchronisationModel();

//*******************************************************//
// PUBLIC METHODS INHERITED FROM: SynchronisationModel: *//
//*******************************************************//

	virtual void synchroniseBeforeStep();
	virtual void synchroniseAfterStep();

protected:

	static const unsigned FULLSYNCHRONISATION_MESSAGEID;
	
//**********************************************************//
// PROTECTED METHODS INHERITED FROM: SynchronisationModel: *//
//**********************************************************//
	
//	virtual void handleMessages();
	virtual void handleSyncMessage(NetMessage* msg);
	virtual void handleClientInputMessage(NetMessage* msg);

//*******************************************************************//
// PROTECTED METHODS INHERITED FROM: SimulationObjectInputListener: *//
//*******************************************************************//

	virtual void handleRigidBodyInput(oops::RigidBody* rigidBody, oops::RIGIDBODY_METHODTYPE method, oops::MethodParameter* parameter);

//********************//
// PROTECTED MEMBERS: //
//********************//

	/// Counter for logging the amount of bytes transmitted for synchronisation
	MessageSizeCounter* counter;
}; // FullSynchronisationModel

#endif // _FULLSYNCHRONISATIONMODEL_H
