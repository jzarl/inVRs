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

#include "ApplyAngularVelocityModifier.h"

#include "Simulation2D.h"
#include <inVRs/SystemCore/DebugOutput.h>
#include <gmtl/AxisAngle.h>
#include <gmtl/MatrixOps.h>
#include <gmtl/Generate.h>
#include <gmtl/QuatOps.h>
#include <assert.h>
#include "Physics2DEntity.h"
#include <inVRs/SystemCore/SystemCore.h>

ApplyAngularVelocityModifier::ApplyAngularVelocityModifier()
{
	simulation = (Simulation2D*)SystemCore::getModuleByName("Simulation2D");
} // ApplyAngularVelocityModifier

TransformationData ApplyAngularVelocityModifier::execute(TransformationData* resultLastStage, TransformationPipe* currentPipe)
{
	TransformationData lastPipeContent;
	TransformationData ret;
	int pipeSize = currentPipe->size();
	gmtl::Quatf angVel;
	gmtl::AxisAnglef angVelSplit;
	gmtl::Vec3f angVelVec;
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

 	angVel = lastPipeContent.orientation;
 	gmtl::set(angVelSplit, angVel);
 	angVelVec[0] = angVelSplit[0] * angVelSplit[1];
 	angVelVec[1] = angVelSplit[0] * angVelSplit[2];
 	angVelVec[2] = angVelSplit[0] * angVelSplit[3];
 	angVelVec *= 5;
 	object->setAngularVel(angVelVec);

	return ret;
} // execute

ApplyAngularVelocityModifierFactory::ApplyAngularVelocityModifierFactory()
{
	className = "ApplyAngularVelocityModifier";
}

TransformationModifier* ApplyAngularVelocityModifierFactory::createInternal(ArgumentVector* args)
{
	return new ApplyAngularVelocityModifier();
} // create

bool ApplyAngularVelocityModifierFactory::needSingleton()
{
	return true;
}

