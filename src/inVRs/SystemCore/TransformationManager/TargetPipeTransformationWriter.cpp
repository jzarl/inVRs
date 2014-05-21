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

#include "TargetPipeTransformationWriter.h"
#include "TransformationManager.h"
#include "TransformationManagerEvents.h"
#include "../DataTypes.h"

TargetPipeTransformationWriter::TargetPipeTransformationWriter(unsigned srcId, unsigned dstId,
		unsigned pipeType, unsigned objectClass, unsigned objectType, unsigned objectId,
		bool fromNetwork) {
	initialized = false;
	openedTargetPipe = false;
	targetPipe = NULL;
	this->srcId = srcId;
	this->dstId = dstId;
	this->pipeType = pipeType;
	this->objectClass = objectClass;
	this->objectType = objectType;
	this->objectId = objectId;
	this->fromNetwork = fromNetwork;
}

TargetPipeTransformationWriter::~TargetPipeTransformationWriter() {
	// TODO Auto-generated destructor stub
}

TransformationData TargetPipeTransformationWriter::execute(TransformationData* resultLastStage,
		TransformationPipe* currentPipe) {
	if (!initialized)
		initialize(currentPipe->getPriority(), currentPipe->getOwner()->getId());
	else if (!targetPipe)
		targetPipe = TransformationManager::getPipe(srcId, dstId, pipeType, objectClass,
				objectType, objectId, 0, false);
	else
		targetPipe->push_back(*resultLastStage);

	return *resultLastStage;
} // execute

void TargetPipeTransformationWriter::initialize(unsigned currentPipePriority,
		unsigned currentUserId) {
	targetPipe = TransformationManager::getPipe(srcId, dstId, pipeType, objectClass, objectType,
			objectId, 0, false);

	if (targetPipe) {
		printd(WARNING,
				"TargetPipeTransformationWriter::initialize(): Target Pipe already existing: USING existing one!\n");
	} // if
	else {
		printd(INFO,
				"TargetPipeTransformationWriter::initialize(): Target Pipe not found: Opening new Pipe!\n");
		TransformationManagerOpenPipeEvent* event = new TransformationManagerOpenPipeEvent(srcId,
				dstId, pipeType, objectClass, objectType, objectId, currentPipePriority - 1,
				currentUserId);
		EventManager::sendEvent(event, EventManager::EXECUTE_GLOBAL);
	} // else
	initialized = true;
}

TargetPipeTransformationWriterFactory::TargetPipeTransformationWriterFactory() {
	className = "TargetPipeTransformationWriter";
}

TransformationModifier* TargetPipeTransformationWriterFactory::createInternal(ArgumentVector *args) {
	unsigned srcId, dstId, pipeType, objectClass, objectType, objectId;
	bool fromNetwork;

	if (!args) {
		printd(ERROR,
				"TargetPipeTransformationWriterFactory::createInternal(): Missing arguments in XML-file!\n");
		assert(false);
	} // if
	args->get("srcId", srcId);
	args->get("dstId", dstId);
	args->get("pipeType", pipeType);
	args->get("objectClass", objectClass);
	args->get("objectType", objectType);
	args->get("objectId", objectId);
	args->get("fromNetwork", fromNetwork);

	return new TargetPipeTransformationWriter(srcId, dstId, pipeType, objectClass, objectType,
			objectId, fromNetwork);
} // createInternal

bool TargetPipeTransformationWriterFactory::needInstanceForEachPipe() {
	return true;
} // needInstanceForEachPipe
