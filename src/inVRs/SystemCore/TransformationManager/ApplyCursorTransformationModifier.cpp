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

#include "ApplyCursorTransformationModifier.h"

#include <assert.h>

#include "TransformationManager.h"
#include "../UserDatabase/User.h"
#include "../UserDatabase/CursorTransformationModel.h"

ApplyCursorTransformationModifier::ApplyCursorTransformationModifier(
		bool useLocalUser) {
	this->useLocalUser = useLocalUser;
} // ApplyCursorTransformationModifier


TransformationData ApplyCursorTransformationModifier::execute(
		TransformationData* resultLastStage, TransformationPipe* currentPipe) {
	User* user;
	TransformationData worldUserTrans;

	if (useLocalUser)
		user = UserDatabase::getLocalUser();
	else
		user = currentPipe->getOwner();

	CursorTransformationModel* cursorModel = user->getCursorTransformationModel();
	assert(cursorModel != NULL);

	worldUserTrans = *resultLastStage;

	TransformationData cursorTrans =
			cursorModel->generateCursorTransformation(worldUserTrans, user);

	return cursorTrans;
} // execute


ApplyCursorTransformationModifierFactory::ApplyCursorTransformationModifierFactory() {
	className = "ApplyCursorTransformationModifier";
} // ApplyCursorTransformationModifierFactory


TransformationModifier* ApplyCursorTransformationModifierFactory::createInternal(
		ArgumentVector* args) {
	bool useLocalUser = false;

	if (!args)
		return new ApplyCursorTransformationModifier(useLocalUser);

	args->get("useLocalUser", useLocalUser);

	return new ApplyCursorTransformationModifier(useLocalUser);
} // createInternal


bool ApplyCursorTransformationModifierFactory::needInstanceForEachPipe() {
	return true;
} // needInstanceForEachPipe

