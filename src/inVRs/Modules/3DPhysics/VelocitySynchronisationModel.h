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

#ifndef _VELOCITYSYNCHRONISATIONMODEL_H
#define _VELOCITYSYNCHRONISATIONMODEL_H

#include "SynchronisationModel.h"
#include "Physics.h"

#include "MessageSizeCounter.h"

#include <inVRs/SystemCore/ComponentInterfaces/NetworkInterface.h>
#include <inVRs/SystemCore/UserDatabase/User.h>
//#include <OpenSG/OSGThreadManager.h>
//#include <OpenSG/OSGThread.h>

/** SynchronisationModel which synchronises every step only changed data.
 * After each simulation step the transformations of the rigid bodies which have
 * changed are distributed.
 */
class VelocitySynchronisationModel : public SynchronisationModel
{

public:

//*****************//
// PUBLIC METHODS: //
//*****************//

	/** Constructor initializes network-pointer.
	 */
	VelocitySynchronisationModel(float linearThreshold, float angularThreshold,
			int convergenceAlgorithm = 0, float convergenceTime = 0);

	/** Empty destructor.
	 */
	virtual ~VelocitySynchronisationModel();

//*******************************************************//
// PUBLIC METHODS INHERITED FROM: SynchronisationModel: *//
//*******************************************************//

	virtual void synchroniseBeforeStep();
	virtual void synchroniseAfterStep();

protected:
	
	static const unsigned VELOCITYSYNCHRONISATION_MESSAGEID;

	struct RigidBodyState {
		unsigned simulationTime;
		gmtl::Vec3f position;
		gmtl::Quatf orientation;
		gmtl::Vec3f linearVel;
		gmtl::Vec3f angularVel;
	};
	
	struct LinearConvergenceState {
		unsigned startSimulationTime;
		gmtl::Vec3f startPoint;
		gmtl::Vec3f destPoint;
		gmtl::Quatf startOri;
		gmtl::Quatf destOri;
	};
	
	enum CONVERGENCE_ALGORITHM {
		SNAPPING = 0,
		LINEAR = 1,
		QUADRIC = 2
	};

	struct SyncState {
		uint64_t rigidBodyID;
		RigidBodyState state;
	};
	
	struct SyncData {
		unsigned simulationTime;
		std::vector<SyncState*> states;   
	};
	
//********************//
// PROTECTED METHODS: //
//********************//

	/** Sends last rigid body states of deactivated objects multiple times to 
	 * ensure the clients receive the correct values.
	 */
	void handleDeactivatedRigidBody(oops::RigidBody* rigidBody, 
			unsigned simulationTime, NetMessage* msg);
	
	/** Calculates the predicted transformation for the passed rigid body
	 *  and returns if the prediction is still valid or if an update message
	 *  has to be transmitted.
	 */
	virtual bool isPredictionStillValid(oops::RigidBody* rigidBody, 
			unsigned simulationTime);

	/**
	 */
	virtual bool calculatePrediction(gmtl::Vec3f& newPos, gmtl::Quatf& newOri, 
			oops::RigidBody* rigidBody, unsigned simulationTime);

	/**
	 */
	virtual bool calculatePredictionFromState(gmtl::Vec3f& newPos, gmtl::Quatf& newOri, 
			RigidBodyState* rigidBodyState, unsigned simulationTime);
	/**
	 */
	virtual bool calculatePredictionLinear(gmtl::Vec3f& newPos, gmtl::Quatf& newOri, 
			oops::RigidBody* rigidBody, unsigned simulationTime);
	
	/**
	 */
	virtual void initiateLinearConvergence(oops::RigidBody* rigidBody, RigidBodyState* oldState,
			RigidBodyState* newState, unsigned simulationTime);

	/**
	 */
	virtual void updatePredictionState(oops::RigidBody* rigidBody, unsigned simulationTime, bool isInactive=false);
	
	/**
	 */
	virtual void encodePredictionState(oops::RigidBody* rigidBody, NetMessage* msg);
	
	virtual void handleBufferedSyncMessages();
	
	virtual void applySyncData(SyncData* nextSyncData, unsigned localSimulationTime);
	
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

	std::vector<SyncData*> immediateUpdates;
	std::map<unsigned, SyncData*> storedUpdates;
	unsigned lastUpdateTime;
	
	std::map<uint64_t, int> inactiveRepeatSend;
	
	std::map<uint64_t, RigidBodyState*> lastStateMap;

	std::map<uint64_t, LinearConvergenceState*> linearConvMap;
	
	float simulationStepSize;
	
	float linearThreshold;
	
	float angularThreshold;
	
	float convergenceTime;
	
	int convergenceAlgorithm;
	
	unsigned usedSyncModel;
}; // VelocitySynchronisationModel

#endif // _VELOCITYSYNCHRONISATIONMODEL_H
