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

#include <assert.h>

#include "TransformationModifier.h"
#include "TransformationModifierFactory.h"
#include "../DebugOutput.h"

TransformationModifier::~TransformationModifier() {

}

TransformationModifier::TransformationModifier() {
	outputMode = OUTPUT_RESULT;
	managerPrivateData.anymaskPipeId = (uint64_t)(-1);
	managerPrivateData.testPipeId = 0;
	factory = NULL;
}

void TransformationModifier::setFactory(TransformationModifierFactory* factory) {
	this->factory = factory;
}

void TransformationModifier::setOutputMode(OUTPUT mode) {
	outputMode = mode;
} // setOutputMode

TransformationModifierFactory* TransformationModifier::getFactory() {
	assert(factory);
	return factory;
}

std::string TransformationModifier::getClassName() {
	assert(factory);
	return factory->getClassName();
}

TransformationData TransformationModifier::execute(TransformationData* resultLastStage,
		TransformationPipe* currentPipe) {
	return *resultLastStage;
} // execute

TransformationData TransformationModifier::executeInternal(TransformationData* resultLastStage,
		TransformationPipe* currentPipe) {
	if (outputMode == OUTPUT_RESULT)
		return execute(resultLastStage, currentPipe);

	// otherwise (outputMode == OUTPUT_PASSTHROUGH)
	execute(resultLastStage, currentPipe);
	return *resultLastStage;
} // executeInternal

bool TransformationModifier::interrupt(TransformationData* resultLastStage,
		TransformationPipe* currentPipe) {
	return false;
} // interrupt
