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

#include "ApplyForceModifier.h"

#include "Simulation2D.h"
#include <inVRs/SystemCore/DebugOutput.h>
#include <gmtl/AxisAngle.h>
#include <gmtl/MatrixOps.h>
#include <gmtl/Generate.h>
#include <gmtl/QuatOps.h>
#include <assert.h>
#include "Physics2DEntity.h"
#include <inVRs/SystemCore/SystemCore.h>


ApplyForceModifier::ApplyForceModifier(float forceMultiplier)
{
	simulation = (Simulation2D*)SystemCore::getModuleByName("Simulation2D");
	this->forceMultiplier = forceMultiplier;
} // ApplyForceModifier

TransformationData ApplyForceModifier::execute(TransformationData* resultLastStage, TransformationPipe* currentPipe)
{
	TransformationData lastPipeContent;
	TransformationData ret = *resultLastStage;
	int pipeSize = currentPipe->size();
	gmtl::Vec3f force;
	gmtl::Quatf torque;
	gmtl::AxisAnglef torqueSplit;
	gmtl::Vec3f torqueVec;
	Physics2DEntity* entity;
	RigidBody* object;
	User* owner = currentPipe->getOwner();

	entity = (Physics2DEntity*)owner->getAssociatedEntityByIndex(0);

	if (!entity)
		return ret;

	object = entity->getRigidBody();

	if (!object)
		return ret;

	ret = object->getTransformation();

	if (pipeSize > 0)
		lastPipeContent = currentPipe->getTransformation(pipeSize-1);
	else	
		return ret;

	force = ret.orientation * lastPipeContent.position;
	force *= forceMultiplier;
	object->setConstantForce(force);

	return ret;
} // execute

ApplyForceModifierFactory::ApplyForceModifierFactory()
{
	className = "ApplyForceModifier";
}

TransformationModifier* ApplyForceModifierFactory::createInternal(ArgumentVector* args)
{
	float forceMultiplier = 1;

	if (!args)
		return new ApplyForceModifier();

	args->get("forceMultiplier", forceMultiplier);
	return new ApplyForceModifier(forceMultiplier);
} // create

bool ApplyForceModifierFactory::needSingleton()
{
	return true;
}

