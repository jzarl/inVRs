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

#include "TransformationLoggerModifier.h"
#include "../UserDatabase/UserDatabase.h"
#include "../Timer.h"
#include "../DebugOutput.h"
#include "TransformationManager.h"

TransformationLoggerModifier::TransformationLoggerModifier(LOGPOLICY logPolicy,
		unsigned logPolicyParam) {
	this->logPolicy = logPolicy;
	this->logPolicyParam = logPolicyParam;
	iterationIdx = 0;
	lastTime = (unsigned)timer.getTimeInMilliseconds();
	remainingTime = 0;
}

TransformationLoggerModifier::~TransformationLoggerModifier() {
	int i;
	// 	myFactory->dump(this);
	for (i = 0; i < (int)loggedData.size(); i++)
		((TransformationLoggerModifierFactory*)factory)->additionalData.push_back(loggedData[i]);

	((TransformationLoggerModifierFactory*)factory)->removeModifier(this);
	// 	printd(INFO, "TransformationLoggerModifier destructor: im called\n");
}

TransformationData TransformationLoggerModifier::execute(TransformationData* resultLastStage,
		TransformationPipe* currentPipe) {
	TRANSFORMATIONDATALOGSTRUCT transfLogData;
	unsigned currentTime = (unsigned)timer.getTimeInMilliseconds();
	unsigned dt;

	transfLogData.iterationIdx = iterationIdx;
	transfLogData.timeStamp = currentTime;
	transfLogData.data = *resultLastStage;
	transfLogData.userId = currentPipe->getOwner()->getId();
	transfLogData.pipeId = currentPipe->getPipeId();

	switch (logPolicy) {
	case LOGPOLICY_ITERATIONBASED:
		if (iterationIdx % logPolicyParam == 0) {
			loggedData.push_back(transfLogData);
		}
		break;

	case LOGPOLICY_TIMEBASED:
		dt = currentTime - lastTime + remainingTime;
		if (dt >= logPolicyParam) {
			loggedData.push_back(transfLogData);
			remainingTime = 0;
		} else {
			remainingTime = dt;
		}
		break;

	default:
		assert(false);
	}

	iterationIdx++;
	lastTime = currentTime;
	return *resultLastStage;
}

void TransformationLoggerModifier::dump(FILE* writeTo) {
	char output[2048];
	TRANSFORMATIONDATALOGSTRUCT transfLogData;
	int i;
	// 	unsigned srcId, dstId, pipeType, objectClass, objectType, objectId;
	// 	bool fromNetwork;

	for (i = 0; i < (int)loggedData.size(); i++) {
		// 		transfLogData = loggedData[i];
		// 		user = UserDatabase::getUserById(transfLogData.userId);
		// 		if(user)
		// 		{
		// 			TransformationManager::unpackPipeId(transfLogData.pipeId, &srcId, &dstId, &pipeType, &objectClass, &objectType, &objectId, &fromNetwork);
		// 			sprintf(output, "username %s, user id %u\n", user->getName().c_str(), transfLogData.userId);
		// 			sprintf(output, "%stime stamp %u, iteration %u\n", output, transfLogData.timeStamp, transfLogData.iterationIdx);
		// 			sprintf(output, "%ssource: %u, dest: %u, type: %u, objectclass %u, objecttype %u, objectid %u, %s\n",
		// 				output, srcId, dstId, pipeType, objectClass, objectType, objectId, fromNetwork ? "external" : "local");
		// 			sprintf(output, "%s\tposition:         (%0.6f, %0.6f, %0.6f)\n", output,
		// 				transfLogData.data.position[0], transfLogData.data.position[1], transfLogData.data.position[2]);
		// 			sprintf(output, "%s\torientation:      (%0.6f, %0.6f, %0.6f, %0.6f)\n", output,
		// 				transfLogData.data.orientation[0], transfLogData.data.orientation[1], transfLogData.data.orientation[2], transfLogData.data.orientation[3]);
		// 			sprintf(output, "%s\tscale:            (%0.6f, %0.6f, %0.6f)\n", output,
		// 				transfLogData.data.scale[0], transfLogData.data.scale[1], transfLogData.data.scale[2]);
		// 			sprintf(output, "%s\tscaleOrientation: (%0.6f, %0.6f, %0.6f, %0.6f)\n", output,
		// 				transfLogData.data.scaleOrientation[0], transfLogData.data.scaleOrientation[1], transfLogData.data.scaleOrientation[2], transfLogData.data.scaleOrientation[3]);
		// 			fprintf(writeTo, output);
		// 		} else
		// 		{
		// 			printd(WARNING, "TransformationLoggerModifier::dump(): encountered transformation with invalid user id\n");
		// 		}
		if (!dataToString(output, loggedData[i])) {
			printd(WARNING,
					"TransformationLoggerModifier::dump(): encountered invalid transformation\n");
		} else {
			fprintf(writeTo, output);
		}
	}

	loggedData.clear();
}

unsigned TransformationLoggerModifier::getNumberOfLoggedData() {
	return loggedData.size();
}

bool TransformationLoggerModifier::dataToString(char* output,
		TRANSFORMATIONDATALOGSTRUCT& transfLogData) {
	unsigned srcId, dstId, pipeType, objectClass, objectType, objectId;
	User* user;
	bool fromNetwork;

	user = UserDatabase::getUserById(transfLogData.userId);
	if (!user) {
		sprintf(output, "TransformationLoggerModifier::dataToString(): invalid user id %d\n",
				transfLogData.userId);
		return false;
	}

	TransformationManager::unpackPipeId(transfLogData.pipeId, &srcId, &dstId, &pipeType,
			&objectClass, &objectType, &objectId, &fromNetwork);
	sprintf(output, "username %s, user id %u\n", user->getName().c_str(), transfLogData.userId);
	sprintf(output, "%stime stamp %u, iteration %u\n", output, transfLogData.timeStamp,
			transfLogData.iterationIdx);
	sprintf(output,
			"%ssource: %u, dest: %u, type: %u, objectclass %u, objecttype %u, objectid %u, %s\n",
			output, srcId, dstId, pipeType, objectClass, objectType, objectId,
			fromNetwork ? "external" : "local");
	sprintf(output, "%s\tposition:         (%0.6f, %0.6f, %0.6f)\n", output,
			transfLogData.data.position[0], transfLogData.data.position[1],
			transfLogData.data.position[2]);
	sprintf(output, "%s\torientation:      (%0.6f, %0.6f, %0.6f, %0.6f)\n", output,
			transfLogData.data.orientation[0], transfLogData.data.orientation[1],
			transfLogData.data.orientation[2], transfLogData.data.orientation[3]);
	sprintf(output, "%s\tscale:            (%0.6f, %0.6f, %0.6f)\n", output,
			transfLogData.data.scale[0], transfLogData.data.scale[1], transfLogData.data.scale[2]);
	sprintf(output, "%s\tscaleOrientation: (%0.6f, %0.6f, %0.6f, %0.6f)\n", output,
			transfLogData.data.scaleOrientation[0], transfLogData.data.scaleOrientation[1],
			transfLogData.data.scaleOrientation[2], transfLogData.data.scaleOrientation[3]);

	return true;
}

TransformationLoggerModifierFactory::TransformationLoggerModifierFactory() {
	className = "TransformationLoggerModifier";
	totalSizeOfLoggedData = 0;
	logFile = NULL;
}

TransformationLoggerModifierFactory::~TransformationLoggerModifierFactory() {
	dump();
	printd(INFO, "TransformationLoggerModifierFactory() destructor: logged %u MB of data\n",
			(unsigned)(totalSizeOfLoggedData / (uint64_t)(1024 * 1024)));
	if (logFile)
		fclose(logFile);
}

void TransformationLoggerModifierFactory::removeModifier(TransformationLoggerModifier* instance) {
	int i;
	for (i = 0; i < (int)loggerList.size(); i++) {
		if (loggerList[i] == instance) {
			loggerList.erase(loggerList.begin() + i);
			return;
		}
	}
	printd(WARNING,
			"TransformationLoggerModifierFactory::removeModifier(): requested modifier didn't appear in list\n");
}

void TransformationLoggerModifierFactory::dump(TransformationLoggerModifier* instance) {
	int i;
	char output[2048];

	if (!logFile)
		return;

	for (i = 0; i < (int)loggerList.size(); i++) {
		if (!instance || (loggerList[i] == instance)) {
			totalSizeOfLoggedData += (uint64_t)loggerList[i]->getNumberOfLoggedData()
					* sizeof(TransformationLoggerModifier::TRANSFORMATIONDATALOGSTRUCT);
			loggerList[i]->dump(logFile);
		}
	}

	if (instance == NULL) {
		totalSizeOfLoggedData += (uint64_t)additionalData.size()
				* (uint64_t)sizeof(TransformationLoggerModifier::TRANSFORMATIONDATALOGSTRUCT);
		for (i = 0; i < (int)additionalData.size(); i++) {
			if (!TransformationLoggerModifier::dataToString(output, additionalData[i])) {
				printd(WARNING,
						"TransformationLoggerModifierFactory::dump(): encountered invalid transformation\n");
			} else {
				fprintf(logFile, output);
			}
		}
		additionalData.clear();
	}
}

void TransformationLoggerModifierFactory::setLogFile(std::string path) {
	FILE* newLogFile;
	newLogFile = fopen(path.c_str(), "w");
	if (!newLogFile) {
		printd(ERROR, "TransformationLoggerModifierFactory(): failed to create logfile %s\n",
				path.c_str());
	} else {
		if (logFile)
			fclose(logFile);
		logFile = newLogFile;
	}
}

TransformationModifier* TransformationLoggerModifierFactory::createInternal(ArgumentVector* args) {
	TransformationLoggerModifier* ret;
	std::string policyArg = "iterationbased";
	int policyParamArg = 1;

	if (args) {
		args->get("policy", policyArg);
		args->get("policyParam", policyParamArg);
	} // if

	if (policyParamArg < 1)
		policyParamArg = 1;

	ret = new TransformationLoggerModifier(
					policyArg == "iterationbased" ? TransformationLoggerModifier::LOGPOLICY_ITERATIONBASED : TransformationLoggerModifier::LOGPOLICY_TIMEBASED,
					(unsigned)policyParamArg);

	loggerList.push_back(ret);
	return ret;
}

bool TransformationLoggerModifierFactory::needInstanceForEachPipe() {
	return true;
}
