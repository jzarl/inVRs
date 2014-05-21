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

/*---------------------------------------------------------------------------*\
 *                                Changes                                    *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
\*---------------------------------------------------------------------------*/

/*
 * TargetPipeTransformationWriter.h
 *
 *  Created on: Sep 19, 2008
 *      Author: rlander
 */

#ifndef _TARGETPIPETRANSFORMATIONWRITER_H_
#define _TARGETPIPETRANSFORMATIONWRITER_H_

#include "TransformationModifier.h"
#include "TransformationModifierFactory.h"

class INVRS_SYSTEMCORE_API TargetPipeTransformationWriter : public TransformationModifier {
public:
	TargetPipeTransformationWriter(unsigned srcId, unsigned dstId, unsigned pipeType,
			unsigned objectClass, unsigned objectType, unsigned objectId, bool fromNetwork);
	virtual ~TargetPipeTransformationWriter();

	virtual TransformationData execute(TransformationData* resultLastStage,
			TransformationPipe* currentPipe);

protected:
	void initialize(unsigned currentPipePriority, unsigned currentUserId);

	bool initialized;
	unsigned srcId, dstId, pipeType, objectClass, objectType, objectId;
	bool fromNetwork;
	bool openedTargetPipe;
	TransformationPipe* targetPipe;
};

class INVRS_SYSTEMCORE_API TargetPipeTransformationWriterFactory :
		public TransformationModifierFactory {
public:
	TargetPipeTransformationWriterFactory();

protected:
	virtual TransformationModifier* createInternal(ArgumentVector *args = NULL);
	virtual bool needInstanceForEachPipe();
};

#endif // _TARGETPIPETRANSFORMATIONWRITER_H_
