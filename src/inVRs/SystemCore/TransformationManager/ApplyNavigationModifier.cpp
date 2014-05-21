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

#include "ApplyNavigationModifier.h"

#include <assert.h>

#include <gmtl/AxisAngle.h>
#include <gmtl/MatrixOps.h>
#include <gmtl/Generate.h>
#include <gmtl/QuatOps.h>

ApplyNavigationModifier::ApplyNavigationModifier(bool centerAtWorldUser) {
	this->centerAtWorldUser = centerAtWorldUser;
} // ApplyNavigationModifier

TransformationData ApplyNavigationModifier::execute(TransformationData* resultLastStage,
		TransformationPipe* currentPipe) {
	// 	printd("ApplyNavigationModifier::execute(): BEGIN\n");
	User* user;
	TransformationData navigatedTransformation;
	TransformationData ret, lastPipeContent;
	int pipeSize = currentPipe->size();

	user = currentPipe->getOwner();
	assert(user);

	navigatedTransformation = user->getNavigatedTransformation();

	if (pipeSize > 0)
		lastPipeContent = currentPipe->getTransformation(pipeSize - 1);
	else
		return navigatedTransformation;

	if (!centerAtWorldUser) {
		// Apply transformation to Navigated-Transformation
		multiply(ret, navigatedTransformation, lastPipeContent);
	} // if
	else {
		// calculate offset from worldUserTrans to navigated, apply navigation
		// changes on worldUserTrans, and finally multiply result with offset
		// to get new navigated transformation as result:
		// W ... WorldUserTrans
		// O ... Offset
		// N ... NavigatedTrans
		// P ... transformation change (lastPipeContent)
		// W * O = N
		// O = W⁻1 * N
		// Wnew = W * P
		// Nnew = Wnew * O
		TransformationData temp;
		TransformationData offset;
		TransformationData invWorldUserTrans;
		TransformationData worldUserTrans = user->getWorldUserTransformation();
		invert(invWorldUserTrans, worldUserTrans);
		multiply(offset, invWorldUserTrans, navigatedTransformation);

		// The following two commands are equal
		// 	multiply(ret, navigatedPosition, offset);
		multiply(temp, worldUserTrans, lastPipeContent);
		multiply(ret, temp, offset);
	} // else

	// 	printd("ApplyNavigationModifier::execute(): END\n");

	return ret;
} // execute

ApplyNavigationModifierFactory::ApplyNavigationModifierFactory() {
	className = "ApplyNavigationModifier";
}

TransformationModifier* ApplyNavigationModifierFactory::createInternal(ArgumentVector* args) {
	bool centerAtWorldUser;

	if (args && args->get("centerAtWorldUser", centerAtWorldUser))
		return new ApplyNavigationModifier(centerAtWorldUser);

	return new ApplyNavigationModifier;
} // create

bool ApplyNavigationModifierFactory::needSingleton() {
	return true;
}

