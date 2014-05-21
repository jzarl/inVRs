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

#ifndef _ACCELERATIONSYNCHRONISATIONMODEL_H
#define _ACCELERATIONSYNCHRONISATIONMODEL_H

#include "VelocitySynchronisationModel.h"

/** SynchronisationModel which synchronises every step only changed data.
 * After each simulation step the transformations of the rigid bodies which have
 * changed are distributed.
 */
class AccelerationSynchronisationModel : public VelocitySynchronisationModel
{

public:

//*****************//
// PUBLIC METHODS: //
//*****************//

	/** Constructor initializes network-pointer.
	 */
	AccelerationSynchronisationModel(float linearThreshold, float angularThreshold,
			int convergenceAlgorithm = 0, float convergenceTime = 0);

	/** Empty destructor.
	 */
	virtual ~AccelerationSynchronisationModel();

//*******************************************************//
// PUBLIC METHODS INHERITED FROM: SynchronisationModel: *//
//*******************************************************//

	virtual void synchroniseBeforeStep();

protected:
	
	static const unsigned ACCELERATIONSYNCHRONISATION_MESSAGEID;

	struct ExtendedRigidBodyState {
		unsigned simulationTime;
		gmtl::Vec3f position;
		gmtl::Quatf orientation;
		gmtl::Vec3f linearVel;
		gmtl::Vec3f angularVel;
		gmtl::Vec3f acceleration;
	};
	
//********************//
// PROTECTED METHODS: //
//********************//

	/**
	 */
	virtual bool calculatePrediction(gmtl::Vec3f& newPos, gmtl::Quatf& newOri, 
			oops::RigidBody* rigidBody, unsigned simulationTime);

	/**
	 */
	virtual bool calculatePredictionFromState(gmtl::Vec3f& newPos, gmtl::Quatf& newOri, 
			ExtendedRigidBodyState* rigidBodyState, unsigned simulationTime);

	/**
	 */
	virtual void initiateLinearConvergence(oops::RigidBody* rigidBody, ExtendedRigidBodyState* oldState,
			ExtendedRigidBodyState* newState, unsigned simulationTime);

	/**
	 */
	virtual void updatePredictionState(oops::RigidBody* rigidBody, unsigned simulationTime,
			bool isInactive = false);
	
	/**
	 */
	virtual void encodePredictionState(oops::RigidBody* rigidBody, NetMessage* msg);
	
//**********************************************************//
// PROTECTED METHODS INHERITED FROM: SynchronisationModel: *//
//**********************************************************//
	
//	virtual void handleMessages();
	virtual void handleSyncMessage(NetMessage* msg);
	virtual void handleClientInputMessage(NetMessage* msg);
	
//********************//
// PROTECTED MEMBERS: //
//********************//

	std::map<uint64_t, ExtendedRigidBodyState*> lastExtendedStateMap;
	std::map<uint64_t, gmtl::Vec3f> lastVelocity;

}; // AccelerationSynchronisationModel

#endif // _ACCELERATIONSYNCHRONISATIONMODEL_H
