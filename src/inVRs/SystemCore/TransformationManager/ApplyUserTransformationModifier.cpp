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

#include "ApplyUserTransformationModifier.h"

#include <assert.h>

#include "TransformationManager.h"
#include "../UserDatabase/User.h"

ApplyUserTransformationModifier::ApplyUserTransformationModifier(
		bool useLocalUser) {
	this->useLocalUser = useLocalUser;
} // ApplyUserTransformationModifier


TransformationData ApplyUserTransformationModifier::execute(
		TransformationData* resultLastStage, TransformationPipe* currentPipe) {
	User* user;
	TransformationData worldUserTrans;

	if (useLocalUser)
		user = UserDatabase::getLocalUser();
	else
		user = currentPipe->getOwner();

	TransformationData userOffset = user->getTrackedUserTransformation();

	return multiply(worldUserTrans, *resultLastStage, userOffset);
} // execute

ApplyUserTransformationModifierFactory::ApplyUserTransformationModifierFactory() {
	className = "ApplyUserTransformationModifier";
} // ApplyUserTransformationModifierFactory


TransformationModifier* ApplyUserTransformationModifierFactory::createInternal(
		ArgumentVector* args) {
	bool useLocalUser = false;

	if (!args)
		return new ApplyUserTransformationModifier(useLocalUser);

	args->get("useLocalUser", useLocalUser);

	return new ApplyUserTransformationModifier(useLocalUser);
} // createInternal


bool ApplyUserTransformationModifierFactory::needInstanceForEachPipe() {
	return true;
} // needInstanceForEachPipe


