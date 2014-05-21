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

#include <gmtl/MatrixOps.h>

#include "TransformationMerger.h"
#include "TransformationManager.h"
#include "../DataTypes.h"
#include "../DebugOutput.h"

TransformationMerger::TransformationMerger() {
	pipeRefCounter = 0;
	// 	lastPipe = NULL;
	id = -1;
}

TransformationMerger::~TransformationMerger() {
	TransformationManager::removeMerger(this);
}

void TransformationMerger::setId(int id) {
	this->id = id;
}

int TransformationMerger::getId() {
	return id;
}

int TransformationMerger::getMaximumNumberOfInputPipes() {
	return 0x7FFFFFFF;
}

int TransformationMerger::getInputMaskListSize() {
	return inputMaskList.size();
}

int TransformationMerger::getOutputMaskListSize() {
	return outputMaskList.size();
}

TransformationPipe::MASK TransformationMerger::getInputMask(int idx) {
	TransformationPipe::MASK dummy;
	if (idx < (int)inputMaskList.size()) {
		return inputMaskList[idx].unpackedMask;
	}
	printd("TransformationMerger::getInputMask(): idx %d out of range\n", idx);
	return dummy;
}

TransformationPipe::MASK TransformationMerger::getOutputMask(int idx) {
	TransformationPipe::MASK dummy;
	if (idx < (int)outputMaskList.size()) {
		return outputMaskList[idx].unpackedMask;
	}
	printd("TransformationMerger::getOutputMask(): idx %d out of range\n", idx);
	return dummy;
}

void TransformationMerger::execute(TransformationPipe* pipe, TransformationData transf) {
	int i;
	TransformationData res;
	TransformationPipe* outputPipe;

	// 	assert(lastPipe);
	for (i = 0; i < (int)inputPipeList.size(); i++)
		if (inputPipeList[i].pipe == pipe)
			break;
	if (i == (int)inputPipeList.size()) {
		assert(i!=0);
		printd(ERROR, "TransformationMerger::execute(): input pipe not registered\n");
		return;
	}

	// 	printd("got transf for pipe %d\n", i);

	inputPipeList[i].transf = transf;
	inputPipeList[i].dirty = true;

	for (i = 0; i < (int)inputPipeList.size(); i++)
		if (!inputPipeList[i].dirty)
			break;

	if (i != (int)inputPipeList.size()) {
		// not all pipes have been executed
		return;
	}

	res = calculateResult();
	outputPipe = chooseOutputPipe();
	assert(outputPipe);
	outputPipe->executeAfterMerger(res);

	for (i = 0; i < (int)inputPipeList.size(); i++)
		inputPipeList[i].dirty = false;

	// 	if(inputPipeList[i].pipe == lastPipe)
	// 	{
	// 		res = calculateResult();
	// 		outputPipe = chooseOutputPipe();
	// 		assert(outputPipe);
	// 		outputPipe->executeAfterMerger(res);
	// 	}
}

bool TransformationMerger::addInputPipe(TransformationPipe* pipe) {
	INPUTDESC inpDesc;
	std::vector<INTERNALMASK> oldInputMaskList;
	INTERNALMASK intMask;
	unsigned unpackedPipeId[7];
	bool fromNetwork;
	int i, j;

	if ((int)inputPipeList.size() >= getMaximumNumberOfInputPipes()) {
		printd("TransformationMerger::addInputPipe(): maximum pipe number exceeded\n");
		return false;
	}

	for (i = 0; i < (int)inputMaskList.size(); i++) {
		if ((inputMaskList[i].anyPipeMask & pipe->getPipeId()) == inputMaskList[i].testPipeId)
			break;
	}

	if (i == (int)inputMaskList.size()) {
		assert(i!=0);
		printd("TransformationMerger::addInputPipe(): pipe does not match\n");
		return false;
	}

	if (inputPipeList.size() == 0) {
		// make a copy
		for (i = 0; i < (int)inputMaskList.size(); i++)
			oldInputMaskList.push_back(inputMaskList[i]);
		inputMaskList.clear();

		// replace EQUALPIPE fields with value from pipe:
		TransformationManager::unpackPipeId(pipe->getPipeId(), unpackedPipeId, unpackedPipeId + 1,
				unpackedPipeId + 2, unpackedPipeId + 3, unpackedPipeId + 4, unpackedPipeId + 5,
				&fromNetwork);
		if (fromNetwork)
			unpackedPipeId[6] = 1;
		else
			unpackedPipeId[6] = 0;
		for (i = 0; i < (int)oldInputMaskList.size(); i++) {
			intMask = oldInputMaskList[i];
			for (j = 0; j < 7; j++)
				if (intMask.unpackedMask.asArray[j] == TransformationManager::EQUALPIPE) {
					intMask.unpackedMask.asArray[j] = unpackedPipeId[j];
				}

			buildIds(&intMask.unpackedMask, &intMask);
			inputMaskList.push_back(intMask);
		}
		printd("TransformationMerger::addInputPipe(): first pipe added\n");
	}

	printd("TransformationMerger::addInputPipe(): adding a pipe, i have now %d pipes attached!\n",
			inputPipeList.size() + 1);

	// 	if(lastPipe == NULL)
	// 	{
	// 		lastPipe = pipe;
	// 	} else
	// 	{
	// 		if(pipe->getPriority() < lastPipe->getPriority())
	// 		{
	// 			// if they are equal the newly added pipe will be executed first
	// 			// therefore we stick to lastPipe
	// 			lastPipe = pipe;
	// 		}
	// 	}

	inpDesc.pipe = pipe;
	inpDesc.transf = identityTransformation();
	inpDesc.dirty = false;
	inputPipeList.push_back(inpDesc);
	pipeRefCounter++;
	return true;
}

void TransformationMerger::removeInputPipe(TransformationPipe* pipe) {
	int i;

	for (i = 0; i < (int)inputPipeList.size(); i++)
		if (pipe == inputPipeList[i].pipe) {
			inputPipeList.erase(inputPipeList.begin() + i);
			pipeRefCounter--;

			if (pipeRefCounter == 0) {
				printd(INFO,
						"TransformationMerger::removeInputPipe(): removed last pipe, deleting merger instance\n");
				delete this;
			}
			return;
		}

	if (i == (int)inputPipeList.size()) {
		printd(WARNING, "TransformationMerger::removeInputPipe(): couldn't find pipe!\n");
	}
}

TransformationData TransformationMerger::calculateResult() {
	int i;
	gmtl::Matrix44f final[2], temp;
	TransformationData ret;

	zero(final[0]);
	for (i = 0; i < (int)inputPipeList.size(); i++) {
		transformationDataToMatrix(inputPipeList[i].transf, temp);
		add(final[(i + 1) & 1], final[(i & 1)], temp);
	}
	mult(final[i & 1], 1.0f / (float)i);

	matrixToTransformationData(final[i & 1], ret);
	return ret;
}

TransformationPipe* TransformationMerger::chooseOutputPipe() {
	int i, j;
	for (i = 0; i < (int)outputMaskList.size(); i++) {
		for (j = 0; j < (int)inputPipeList.size(); i++)
			if ((outputMaskList[i].anyPipeMask & inputPipeList[j].pipe->getPipeId())
					== outputMaskList[i].testPipeId)
				return inputPipeList[j].pipe;
	}
	assert(i!=0);
	printd(ERROR, "TransformationMerger::chooseOutputPipe(): no matching outputPipe found\n");
	return NULL;
}

void TransformationMerger::addInputMask(TransformationPipe::MASK* mask) {
	INTERNALMASK intMask;
	if (inputPipeList.size() > 0) {
		printd(ERROR,
				"TransformationMerger::addInputMask(): have already at least one pipe attached!\n");
		return;
	}
	buildIds(mask, &intMask); // treats EQUALPIPE and ANYPIPE equaly
	inputMaskList.push_back(intMask);
}

void TransformationMerger::addOutputMask(TransformationPipe::MASK* mask) {
	INTERNALMASK intMask;

	buildIds(mask, &intMask);
	outputMaskList.push_back(intMask);
}

void TransformationMerger::buildIds(TransformationPipe::MASK* mask, INTERNALMASK* intMask) {
	unsigned srcId, dstId, pipeType, objectClass, objectType, objectId, bFromNetwork;

	intMask->unpackedMask = *mask;
	intMask->anyPipeMask = (uint64_t)(-1);

	srcId = mask->individual.srcId;
	dstId = mask->individual.dstId;
	pipeType = mask->individual.pipeType;
	objectClass = mask->individual.objectClass;
	objectType = mask->individual.objectType;
	objectId = mask->individual.objectId;
	bFromNetwork = mask->individual.bFromNetwork;

	if (bFromNetwork & (TransformationManager::ANYPIPE | TransformationManager::EQUALPIPE)) {
		intMask->anyPipeMask &= ~(uint64_t)1;
		bFromNetwork = 0;
	}
	if (srcId & (TransformationManager::ANYPIPE | TransformationManager::EQUALPIPE)) {
		intMask->anyPipeMask &= ~(uint64_t)0xFE;
		srcId = 0;
	}
	if (dstId & (TransformationManager::ANYPIPE | TransformationManager::EQUALPIPE)) {
		intMask->anyPipeMask &= ~(uint64_t)0xFE00;
		dstId = 0;
	}
	if (pipeType & (TransformationManager::ANYPIPE | TransformationManager::EQUALPIPE)) {
		intMask->anyPipeMask &= ~(uint64_t)0xFF0000;
		pipeType = 0;
	}
	if (objectClass & (TransformationManager::ANYPIPE | TransformationManager::EQUALPIPE)) {
		intMask->anyPipeMask &= ~(uint64_t)0xFF000000;
		objectClass = 0;
	}
	if (objectType & (TransformationManager::ANYPIPE | TransformationManager::EQUALPIPE)) {
		intMask->anyPipeMask &= ~((uint64_t)0xFFFF << 32);
		objectType = 0;
	}
	if (objectId & (TransformationManager::ANYPIPE | TransformationManager::EQUALPIPE)) {
		intMask->anyPipeMask &= ~((uint64_t)0xFFFF << 48);
		objectId = 0;
	}

	intMask->testPipeId = TransformationManager::packPipeId(srcId, dstId, pipeType, objectClass,
			objectType, objectId, bFromNetwork);
}
