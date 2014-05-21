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

#ifndef _TRANSFORMATIONMERGER_H
#define _TRANSFORMATIONMERGER_H

#include <vector>
#include <string>

#include "TransformationPipe.h"

class INVRS_SYSTEMCORE_API TransformationMerger
{
public:
	TransformationMerger();
	virtual ~TransformationMerger();

	void setId(int id);

	int getId();
	virtual int getMaximumNumberOfInputPipes();
	virtual std::string getName() = 0;
	int getInputMaskListSize();
	int getOutputMaskListSize();
	TransformationPipe::MASK getInputMask(int idx);
	TransformationPipe::MASK getOutputMask(int idx);

	void execute(TransformationPipe* pipe, TransformationData transf);
	bool addInputPipe(TransformationPipe* pipe);
	void removeInputPipe(TransformationPipe* pipe);

	virtual TransformationData calculateResult(); // iterate over inputPipeList
	virtual TransformationPipe* chooseOutputPipe();

	void addInputMask(TransformationPipe::MASK* mask);
	void addOutputMask(TransformationPipe::MASK* mask);

protected:
	struct INTERNALMASK
	{
		TransformationPipe::MASK unpackedMask;
		uint64_t testPipeId;
		uint64_t anyPipeMask;
	};

	struct INPUTDESC
	{
		TransformationPipe* pipe;
		TransformationData transf;
		bool dirty;
	};

	static void buildIds(TransformationPipe::MASK* mask, INTERNALMASK* intMask);

	std::vector<INTERNALMASK> inputMaskList;
	std::vector<INPUTDESC> inputPipeList;
	std::vector<INTERNALMASK> outputMaskList;
	int pipeRefCounter;
// 	TransformationPipe* lastPipe;
	int id;
};

class INVRS_SYSTEMCORE_API TransformationMergerFactory : public ClassFactory<TransformationMerger>
{
public:
	virtual TransformationMerger* create(std::string className, void* args = NULL) = 0;
};

#endif
