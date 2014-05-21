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

#include "MultiPipeInterrupter.h"
#include "TransformationManager.h"
#include "../UserDatabase/UserDatabase.h"

bool MultiPipeInterrupter::pipesActiveState = true;


MultiPipeInterrupter::MultiPipeInterrupter()
{
	currentPipesActiveState = pipesActiveState;
} // MultiPipeInterrupter

MultiPipeInterrupter::~MultiPipeInterrupter()
{
} // ~MultiPipeInterrupter

TransformationData MultiPipeInterrupter::execute(TransformationData* resultLastStage, TransformationPipe* currentPipe)
{
	TransformationData result;
	if (currentPipesActiveState != pipesActiveState)
	{
		result = identityTransformation();
		currentPipesActiveState = pipesActiveState;
	} // if
	else
		result = *resultLastStage;

	return result;
} // execute


void MultiPipeInterrupter::setActive()
{
	pipesActiveState = true;
} // setActivePipe

void MultiPipeInterrupter::setInactive()
{
	pipesActiveState = false;
} // setInactivePipe

bool MultiPipeInterrupter::interrupt(TransformationData* resultLastStage, TransformationPipe* currentPipe)
{
	if (currentPipesActiveState)
		return false;
	else
	{
		if (currentPipesActiveState != pipesActiveState)
			currentPipesActiveState = pipesActiveState;
		return true;
	} // else
} // interrupt

MultiPipeInterrupterFactory::MultiPipeInterrupterFactory()
{
	className = "MultiPipeInterrupter";
}

TransformationModifier* MultiPipeInterrupterFactory::createInternal(ArgumentVector* args)
{
	return new MultiPipeInterrupter;
} // create

bool MultiPipeInterrupterFactory::needInstanceForEachPipe()
{
	return true;
} // needInstanceForEachPipe


