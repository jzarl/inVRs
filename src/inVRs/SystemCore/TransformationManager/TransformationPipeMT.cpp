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

#include "TransformationPipeMT.h"

#include <assert.h>

#include "../Timer.h"
#include "../DebugOutput.h"

TransformationPipeMT::TransformationPipeMT(uint64_t pipeId, User* owner) :
	TransformationPipe(pipeId, owner) {
	std::string lockName = "transformationPipe" + getUInt64AsString(pipeId);
#if OSG_MAJOR_VERSION >= 2
	pipeLock = OSG::dynamic_pointer_cast<OSG::Lock>(OSG::ThreadManager::the()->getLock(lockName.c_str(),false));	//pipeLock is not a global variable, just class member
#else //OpenSG1:
	pipeLock = dynamic_cast<OSG::Lock*> (OSG::ThreadManager::the()->getLock(lockName.c_str()));
#endif
} // TransformationPipeMT

TransformationPipeMT::~TransformationPipeMT() {

} // ~TransformationPipeMT

TransformationData TransformationPipeMT::getTransformation(unsigned index) {
	TransformationData result;
#if OSG_MAJOR_VERSION >= 2
	pipeLock->acquire();
#else //OpenSG1:
	pipeLock->aquire();
#endif
	assert(index < transformationData.size());
	result = transformationData[index].transf;
	pipeLock->release();
	return result;
} // getTransformation

void TransformationPipeMT::push_back(TransformationData& data) {
	TransformationPipeData pipeData;
	pipeData.transf = data;
	pipeData.timestamp = (unsigned)timer.getTimeInMilliseconds();
#if OSG_MAJOR_VERSION >= 2
	pipeLock->acquire();
#else //OpenSG1:
	pipeLock->aquire();
#endif
	transformationData.push_back(pipeData);
	pipeLock->release();
}

int TransformationPipeMT::size() {
	int result;
#if OSG_MAJOR_VERSION >= 2
	pipeLock->acquire();
#else //OpenSG1:
	pipeLock->aquire();
#endif
	result = transformationData.size();
	pipeLock->release();
	return result;
} // size

void TransformationPipeMT::flush() {
	TransformationPipeData dummy;

#if OSG_MAJOR_VERSION >= 2
	pipeLock->acquire();
#else //OpenSG1:
	pipeLock->aquire();
#endif
	if (flushStrategy == FLUSHSTRATEGY_QUORUM) {
		while (transformationData.size() > flushParam)
			transformationData.pop_front();
	} else {
		printd(WARNING, "TransformationPipe::flush(): strategy %u not implemented yet\n",
				(unsigned)flushStrategy);
	}
	pipeLock->release();
}
