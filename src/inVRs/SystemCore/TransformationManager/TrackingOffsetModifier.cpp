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

#include "TrackingOffsetModifier.h"

#include <gmtl/Generate.h>
#include <gmtl/QuatOps.h>
#include <gmtl/Xforms.h>

#include "../XMLTools.h"
#include "../UserDatabase/UserDatabase.h"

TrackingOffsetModifier::TrackingOffsetModifier(bool useHeadSensor, bool removeYAxis,
		bool removeOrientation, bool useLocalUser) {
	this->useLocalUser = useLocalUser;
	this->useHead = useHeadSensor;
	this->removeYAxis = removeYAxis;
	this->removeOrientation = removeOrientation;
} // TrackingOffsetModifier

TransformationData TrackingOffsetModifier::execute(TransformationData* resultLastStage,
		TransformationPipe* currentPipe) {
	TransformationData result, result1;
	TransformationData sensorData;

	User* user = NULL;
	User* owner = currentPipe->getOwner();
	User* localUser = UserDatabase::getLocalUser();
	owner = currentPipe->getOwner();

	/*
	 if (!useLocalUser && owner == localUser && UserDatabase::getNumberOfRemoteUsers() > 0)
	 user = UserDatabase::getRemoteUserByIndex(0);
	 */
	if (!useLocalUser)
		user = owner;
	else
		//if (useLocalUser)
		user = localUser;

	if (!user)
		return *resultLastStage;

	if (useHead)
		sensorData = user->getTrackedHeadTransformation();
	else
		sensorData = user->getTrackedHandTransformation();

	if (removeYAxis)
		sensorData.position[1] = 0;

	if (removeOrientation)
		sensorData.orientation = gmtl::QUAT_IDENTITYF;

	// TODO: @LANDI: SHOULDN'T THIS MULTIPLY BE IN THE OPPOSITE ORDER???
	return multiply(result, *resultLastStage, sensorData);
} // execute

TrackingOffsetModifierFactory::TrackingOffsetModifierFactory() {
	className = "TrackingOffsetModifier";
}

TransformationModifier* TrackingOffsetModifierFactory::createInternal(ArgumentVector *args) {
	bool useHeadSensor = true;
	bool removeYAxis = false;
	bool removeOrientation = false;
	bool useLocalUser = false;

	if (!args)
		return new TrackingOffsetModifier();

	args->get("useHeadSensor", useHeadSensor);
	args->get("removeYAxis", removeYAxis);
	args->get("removeOrientation", removeOrientation);
	args->get("useLocalUser", useLocalUser);

	return new TrackingOffsetModifier(useHeadSensor, removeYAxis, removeOrientation, useLocalUser);
} // create

bool TrackingOffsetModifierFactory::needInstanceForEachPipe() {
	return true;
}

