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

#include "MessageSizeCounter.h"

#include <assert.h>

#include "Physics.h"
#include <inVRs/SystemCore/DebugOutput.h>
#include <inVRs/SystemCore/Configuration.h>
#include <inVRs/SystemCore/UtilityFunctions.h>

//*****************//
// PUBLIC METHODS: //
//*****************//

MessageSizeCounter::MessageSizeCounter(Physics* physics) {
	assert(physics);
	this->physics = physics;
	stepSize = 0;
	stepCounter = 0;
	logFileName = "MessageSizeCounter.log";
} // MessageSizeCounter;

MessageSizeCounter::~MessageSizeCounter() {

} // ~MessageSizeCounter

void MessageSizeCounter::setLogFileName(std::string fileName) {
	logFileName = fileName;
} // setLogFileName

void MessageSizeCounter::countBytes(NetMessage* msg) {
	unsigned messageSize;

	messageSize = msg->getBufferSize();
	bytesPerStep.push_back(messageSize);
} // countBytes

void MessageSizeCounter::stepFinished() {
	int i;
	unsigned byteSum;

	if (stepSize == 0)
		stepSize = physics->getStepSize();

	stepCounter += stepSize;

	if (stepCounter >= 1) {
		byteSum = 0;
		for (i = 0; i < (int)bytesPerStep.size(); i++)
			byteSum += bytesPerStep[i];
		// normalize byteSum to one second
		byteSum = (unsigned)((float)byteSum/(float)stepCounter);
		bytesPerSecond.push_back(byteSum);

		stepCounter = 0;

		bytesPerStep.clear();
	} // if
} // stepFinished

void MessageSizeCounter::dumpResults() {
	int i;
	unsigned size;
	unsigned maxBytes, minBytes;
	double meanBytes;
	unsigned value;
	std::string logFileUrl;
	FILE* logFile;

	logFileUrl = getConcatenatedPath("logFileName", "PhysicsLogs");
//	if (Configuration::containsPath("PhysicsLogs"))
//		logFileUrl = Configuration::getPath("PhysicsLogs") + logFileName;
//	else
//		logFileUrl = logFileName;

	logFile = fopen(logFileUrl.c_str(), "w");

	if (!logFile) {
		printd(ERROR, "MessageSizeCounter::dumpResults(): Could not write to logFile!\n");
		return;
	} // if

	maxBytes = 0;
	minBytes = (unsigned)(-1);
	meanBytes = 0;
	size = bytesPerSecond.size();

	for (i=0; i < (int)size; i++)
	{
		value = bytesPerSecond[i];
		fprintf(logFile, "%i\t%u\n", i, value);
		meanBytes += ((double)value/(double)size);
		if (value > maxBytes)
			maxBytes = value;
		if (value < minBytes)
			minBytes = value;
	} // for

	fclose(logFile);

	printd(INFO, "MessageSizeCounter::dumpResults(): Sync Bytes per Second: MEAN: %f\tMIN: %u\tMAX: %u\n", meanBytes, minBytes, maxBytes);
} // dumpResults

