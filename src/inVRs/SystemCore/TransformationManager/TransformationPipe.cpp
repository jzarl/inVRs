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

#include "TransformationPipe.h"

#include <iostream>
#include <sstream>
#include <assert.h>

#include "TransformationModifier.h"
#include "../Timer.h"
#include "../DebugOutput.h"
#include "TransformationMerger.h"
#include "TransformationManager.h"

TransformationPipe::TransformationPipe(uint64_t pipeId, User* owner) {
	this->pipeId = pipeId;
	priority = 0;
	flushStrategy = FLUSHSTRATEGY_QUORUM;
	flushParam = 2;
	this->owner = owner;
	this->executionInterval = 0;
	this->timeToNextExecution = 0;
	this->merger = NULL;
	this->mergerIndex = -1;
}

TransformationPipe::~TransformationPipe() {
	int i;
	TransformationModifierFactory* factory;

	// check modifier list for indivdual modifiers and delete them
	for (i = 0; i < (int)stages.size(); i++) {
		// if(!stages[i]) continue; what is that for ???
		assert(stages[i]);
		factory = stages[i]->getFactory();
		assert(factory);
		factory->releaseModifier(stages[i]);
		stages[i] = NULL;
	}

	if (merger)
		merger->removeInputPipe(this);
}

void TransformationPipe::push_back(TransformationData& data) {
	TransformationPipeData pipeData;
	pipeData.transf = data;
	pipeData.timestamp = (unsigned)timer.getTimeInMilliseconds();
	transformationData.push_back(pipeData);
}

int TransformationPipe::size() {
	return transformationData.size();
}

TransformationData TransformationPipe::getTransformation(unsigned index) {
	assert(index < transformationData.size());
	return transformationData[index].transf;
} // getTransformation

void TransformationPipe::flush() {
	TransformationPipeData dummy;

	if (flushStrategy == FLUSHSTRATEGY_QUORUM) {
		while (transformationData.size() > flushParam)
			transformationData.pop_front();
	} else {
		printd(WARNING, "TransformationPipe::flush(): strategy %u not implemented yet\n",
				(unsigned)flushStrategy);
	}
}

void TransformationPipe::addStage(TransformationModifier* stage) {
	assert(stage != NULL);
	stages.push_back(stage);
}

bool TransformationPipe::setMerger(TransformationMerger* merger) {
	if (this->merger) {
		printd(
				ERROR,
				"TransformationPipe::setMerger(): Pipe already has an TransformationMerger (at index %i)!\n",
				mergerIndex);
		return false;
	} // if

	this->merger = merger;
	this->mergerIndex = stages.size();
	assert(mergerIndex >= 0);
	return true;
} // addMerger

TransformationData TransformationPipe::execute() {
	int i;
	TransformationData lastResult, temp;

	if (size() > 0)
		lastResult = getTransformation(size() - 1);
	else
		lastResult = identityTransformation();

	for (i = 0; i < (int)stages.size(); i++) {
		if (i == mergerIndex)
			break;

		temp = lastResult;

		if (stages[i]->interrupt(&temp, this))
			break;
		lastResult = stages[i]->executeInternal(&temp, this);
	} // for

	if (i == mergerIndex)
		merger->execute(this, lastResult);

	return lastResult;
} // execute

TransformationData TransformationPipe::executeAfterMerger(TransformationData lastResult) {
	int i;
	TransformationData temp;

	assert(merger);

	for (i = mergerIndex; i < (int)stages.size(); i++) {
		temp = lastResult;
		lastResult = stages[i]->executeInternal(&temp, this);
	} // for
	return lastResult;
} // executeAfterMerger

User* TransformationPipe::getOwner() {
	return owner;
}

uint64_t TransformationPipe::getPipeId() {
	return pipeId;
} // getPipeId

unsigned TransformationPipe::getPriority() {
	return priority;
} // getPriority

void TransformationPipe::setExecutionInterval(float sec, float offset) {
	float time = timer.getTime() + offset;
	this->executionInterval = sec;
	this->timeToNextExecution = fmodf(time, sec);
} // setExecutionInterval

float TransformationPipe::getExecutionInterval() {
	return executionInterval;
} // getExecutionInterval

void TransformationPipe::setFlushStrategy(FLUSHSTRATEGY stratetgy, unsigned param) {
	flushStrategy = stratetgy;
	flushParam = param;
}

void PipeSelector::setup(TransformationPipe::MASK* mask) {
	unsigned srcId, dstId, pipeType, objectClass, objectType, objectId, bFromNetwork;

	anyPipeMask = (uint64_t)(-1); //0xFFFFFFFFFFFFFFFFF

	srcId = mask->individual.srcId;
	dstId = mask->individual.dstId;
	pipeType = mask->individual.pipeType;
	objectClass = mask->individual.objectClass;
	objectType = mask->individual.objectType;
	objectId = mask->individual.objectId;
	bFromNetwork = mask->individual.bFromNetwork;

	// check, if at least one of ANYPIPE or EQUALPIPE flags is set
	if (bFromNetwork & (TransformationManager::ANYPIPE | TransformationManager::EQUALPIPE)) {
		anyPipeMask &= ~(uint64_t)1;
		bFromNetwork = 0;
	}
	if (srcId & (TransformationManager::ANYPIPE | TransformationManager::EQUALPIPE)) {
		anyPipeMask &= ~(uint64_t)0xFE;
		srcId = 0;
	}
	if (dstId & (TransformationManager::ANYPIPE | TransformationManager::EQUALPIPE)) {
		anyPipeMask &= ~(uint64_t)0xFE00;
		dstId = 0;
	}
	if (pipeType & (TransformationManager::ANYPIPE | TransformationManager::EQUALPIPE)) {
		anyPipeMask &= ~(uint64_t)0xFF0000;
		pipeType = 0;
	}
	if (objectClass & (TransformationManager::ANYPIPE | TransformationManager::EQUALPIPE)) {
		anyPipeMask &= ~(uint64_t)0xFF000000;
		objectClass = 0;
	}
	if (objectType & (TransformationManager::ANYPIPE | TransformationManager::EQUALPIPE)) {
		anyPipeMask &= ~((uint64_t)0xFFFF << 32);
		objectType = 0;
	}
	if (objectId & (TransformationManager::ANYPIPE | TransformationManager::EQUALPIPE)) {
		anyPipeMask &= ~((uint64_t)0xFFFF << 48);
		objectId = 0;
	}

	testPipeId = TransformationManager::packPipeId(srcId, dstId, pipeType, objectClass, objectType,
			objectId, bFromNetwork);
}

INVRS_SYSTEMCORE_API std::string getUInt64AsString(uint64_t src) {
	std::string result;
	std::stringstream ss(std::stringstream::in | std::stringstream::out);
	ss << src;
	ss >> result;
	return result;
}
