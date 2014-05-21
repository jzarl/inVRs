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

#ifndef _PHYSICSFULLSYNCHRONISATIONMODEL_H_
#define _PHYSICSFULLSYNCHRONISATIONMODEL_H_

#include "SynchronisationModel.h"

class MessageSizeCounter;

/** SynchronisationModel which does client-side physics calculations.
 */
class PhysicsFullSynchronisationModel : public SynchronisationModel {
	
public:

//*****************//
// PUBLIC METHODS: //
//*****************//

	/** Constructor initializes MessageCounter
	 */
	PhysicsFullSynchronisationModel(unsigned updateInterval = 1, int convergenceAlgorithm = 0,
			float convergenceTime = 0);

	/** Empty destructor.
	 */
	virtual ~PhysicsFullSynchronisationModel();

//*******************************************************//
// PUBLIC METHODS INHERITED FROM: SynchronisationModel: *//
//*******************************************************//

	virtual void synchroniseBeforeStep();
	virtual void synchroniseAfterStep();

protected:
	
	static const unsigned PHYSICSFULLSYNCHRONISATION_MESSAGEID;

	struct RigidBodyState {
		unsigned simulationTime;
		gmtl::Vec3f position;
		gmtl::Quatf orientation;
		gmtl::Vec3f linearVel;
		gmtl::Vec3f angularVel;
	};
	
	enum CONVERGENCE_ALGORITHM {
		SNAPPING = 0,
		LINEAR = 1,
		QUADRIC = 2
	};

//********************//
// PROTECTED METHODS: //
//********************//

	virtual void handleServerSynchronisation();
	virtual void handleClientSynchronisation();

	/**
	 */
	virtual bool calculateLinearConvergence(gmtl::Vec3f& newPos, gmtl::Quatf& newOri, 
			oops::RigidBody* rigidBody, unsigned simulationTime);

	/**
	 */
	virtual bool calculateQuadricConvergence(gmtl::Vec3f& newPos, gmtl::Quatf& newOri, 
			oops::RigidBody* rigidBody, unsigned simulationTime);

	virtual void calculateFutureTransformation(TransformationData& result, 
			TransformationData trans, gmtl::Vec3f linearVel, gmtl::Vec3f angularVel,
			float dt);

//**********************************************************//
// PROTECTED METHODS INHERITED FROM: SynchronisationModel: *//
//**********************************************************//
	
	virtual void handleSyncMessage(NetMessage* msg);
	virtual void handleClientInputMessage(NetMessage* msg);
	virtual bool needPhysicsCalculation();

//********************//
// PROTECTED MEMBERS: //
//********************//

	/// Counter for logging the amount of bytes transmitted for synchronisation
	MessageSizeCounter* counter;
	
	/// Counter until next synchronisation messages has to be sent
	unsigned stepsUntilUpdate;
	
	/// Number of simulation steps between two updates 
	unsigned updateInterval;
	
	float simulationStepSize;

	int convergenceAlgorithm;

	float convergenceTime;
	
	std::map<uint64_t, RigidBodyState*> convergenceState;
}; // PhysicsFullSynchronisationModel

#endif /*_PHYSICSFULLSYNCHRONISATIONMODEL_H_*/
