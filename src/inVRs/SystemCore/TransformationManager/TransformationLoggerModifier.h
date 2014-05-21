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

#ifndef _TRANSFORMATIONLOGGERMODIFIER_H
#define _TRANSFORMATIONLOGGERMODIFIER_H

#include <stdio.h>
#include <string>

#include "TransformationModifier.h"
#include "TransformationModifierFactory.h"

class TransformationLoggerModifierFactory;

class INVRS_SYSTEMCORE_API TransformationLoggerModifier : public TransformationModifier {
public:
	enum LOGPOLICY {
		LOGPOLICY_ITERATIONBASED, LOGPOLICY_TIMEBASED
	};

	TransformationLoggerModifier(LOGPOLICY logPolicy, unsigned logPolicyParam);
	virtual ~TransformationLoggerModifier();

	virtual TransformationData execute(TransformationData* resultLastStage,
			TransformationPipe* currentPipe);

	void dump(FILE* writeTo);
	unsigned getNumberOfLoggedData();

protected:
	struct TRANSFORMATIONDATALOGSTRUCT {
		unsigned iterationIdx;
		unsigned timeStamp;
		TransformationData data;
		unsigned userId;
		uint64_t pipeId;
	};

	unsigned lastTime;
	unsigned iterationIdx;
	LOGPOLICY logPolicy;
	unsigned logPolicyParam;
	unsigned remainingTime;
	std::vector<TRANSFORMATIONDATALOGSTRUCT> loggedData;
	TransformationLoggerModifierFactory* myFactory;

	static bool dataToString(char* output, TRANSFORMATIONDATALOGSTRUCT& transfLogData);

	friend class TransformationLoggerModifierFactory;
};

class INVRS_SYSTEMCORE_API TransformationLoggerModifierFactory :
		public TransformationModifierFactory {
public:
	TransformationLoggerModifierFactory();
	virtual ~TransformationLoggerModifierFactory();

	void removeModifier(TransformationLoggerModifier* instance);
	void dump(TransformationLoggerModifier* instance = NULL); // pass NULL to invoke dump() for all instances
	void setLogFile(std::string path);

protected:
	virtual TransformationModifier* createInternal(ArgumentVector* args);
	virtual bool needInstanceForEachPipe();

	std::vector<TransformationLoggerModifier*> loggerList;
	FILE* logFile;
	uint64_t totalSizeOfLoggedData;
	std::vector<TransformationLoggerModifier::TRANSFORMATIONDATALOGSTRUCT> additionalData; // log data from terminated modifiers

	friend class TransformationLoggerModifier;
};

#endif
