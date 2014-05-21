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

#ifndef MULTIPIPEINTERRUPTER_H_
#define MULTIPIPEINTERRUPTER_H_

#include "TransformationModifierFactory.h"

class INVRS_SYSTEMCORE_API MultiPipeInterrupter : public TransformationModifier
{
public:
	MultiPipeInterrupter();
	virtual ~MultiPipeInterrupter();

	virtual TransformationData execute(TransformationData* resultLastStage, TransformationPipe* currentPipe);

	static void setActive();
	static void setInactive();

protected:
	static bool pipesActiveState;
	bool currentPipesActiveState;

	virtual bool interrupt(TransformationData* resultLastStage, TransformationPipe* currentPipe);

};

class INVRS_SYSTEMCORE_API MultiPipeInterrupterFactory : public TransformationModifierFactory
{
public:
	MultiPipeInterrupterFactory();

protected:

	virtual TransformationModifier* createInternal(ArgumentVector* args);
	virtual bool needInstanceForEachPipe();

}; // MultiPipeInterrupterFactory


#endif /* MULTIPIPEINTERRUPTER_H_ */
