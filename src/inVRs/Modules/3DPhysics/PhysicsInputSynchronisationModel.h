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

#ifndef _PHYSICSINPUTSYNCHRONISATIONMODEL_H_
#define _PHYSICSINPUTSYNCHRONISATIONMODEL_H_

#include "SynchronisationModel.h"
#ifdef INVRS_BUILD_TIME
#include "oops/SimulationObjectInputListener.h"
#else
#include <inVRs/Modules/3DPhysics/oops/SimulationObjectInputListener.h>
#endif

class MessageSizeCounter;

/** SynchronisationModel which does client-side physics calculations.
 */
class PhysicsInputSynchronisationModel : public SynchronisationModel, oops::SimulationObjectInputListener {
	
public:

//*****************//
// PUBLIC METHODS: //
//*****************//

	/** Constructor initializes MessageCounter
	 */
	PhysicsInputSynchronisationModel(float updateInterval = 1, float inputUpdateInterval = 0);

	/** Empty destructor.
	 */
	virtual ~PhysicsInputSynchronisationModel();

//*******************************************************//
// PUBLIC METHODS INHERITED FROM: SynchronisationModel: *//
//*******************************************************//

	virtual void synchroniseBeforeStep();
	virtual void synchroniseAfterStep();

protected:
	
	static const unsigned PHYSICSINPUTSYNCHRONISATION_MESSAGEID;

	struct RigidBodyState {
		uint64_t rigidBodyID;
		gmtl::Vec3f position;
		gmtl::Quatf orientation;
		gmtl::Vec3f linearVel;
		gmtl::Vec3f angularVel;
	};
	
	struct SyncData {
		unsigned simulationTime;
		uint64_t randomSeed;
		std::vector<RigidBodyState*> states;   
	};

	virtual void handleBufferedSyncMessages();
	
	virtual void applySyncData(SyncData* nextSyncData, unsigned localSimulationTime);

//**********************************************************//
// PROTECTED METHODS INHERITED FROM: SynchronisationModel: *//
//**********************************************************//
	
	virtual void handleSyncMessage(NetMessage* msg);
	virtual void handleClientInputMessage(NetMessage* msg);
	virtual bool needPhysicsCalculation();

//*******************************************************************//
// PROTECTED METHODS INHERITED FROM: SimulationObjectInputListener: *//
//*******************************************************************//

	virtual void handleRigidBodyInput(oops::RigidBody* rigidBody, oops::RIGIDBODY_METHODTYPE method, oops::MethodParameter* parameter);

//********************//
// PROTECTED MEMBERS: //
//********************//

	/// Counter for logging the amount of bytes transmitted for synchronisation
	MessageSizeCounter* counter;

	std::vector<SyncData*> immediateUpdates;
	std::map<unsigned, SyncData*> storedUpdates;
	unsigned lastUpdateTime;
	
	/// timer until next synchronisation messages has to be sent
	float timeUntilNextUpdate;
	
	/// timer until next frequent synchronisation message for changed RBs has to be sent
	float timeUntilFrequentUpdate;
	
	/// Number of seconds between two updates 
	float updateInterval;
	
	/// Number of seconds between two updates when input occured
	float inputUpdateInterval;

	/// time between two simulation steps
	float simulationStepSize;

	/// list of all rigidBodies which have been modified in the last simulation step
	std::vector<oops::RigidBody*> modifiedRigidBodies; 
	
}; // PhysicsInputSynchronisationModel

#endif /*_PHYSICSINPUTSYNCHRONISATIONMODEL_H_*/
