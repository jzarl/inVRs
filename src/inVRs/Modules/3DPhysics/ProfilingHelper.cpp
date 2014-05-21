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

/*
 * ProfilingHelper.cpp
 *
 *  Created on: Aug 13, 2008
 *      Author: landi
 */

#include "ProfilingHelper.h"
#include <inVRs/SystemCore/DebugOutput.h>
#include <inVRs/SystemCore/Timer.h>
#include <assert.h>
#include <string.h>

ProfilingHelper::ProfilingHelper(std::string logFile, int numValues,
		char** dataNames) {
	this->initialize(logFile, numValues, dataNames);
} // ProfilingHelper

ProfilingHelper::~ProfilingHelper() {
	fclose(logFile);
	delete[] data;
} // ~ProfilingHelper

void ProfilingHelper::start(Timer& timer) {
	lastTime = timer.getTime();
} // start

void ProfilingHelper::dt(unsigned index, Timer& timer) {
	assert(lastTime != 0);
	assert((int)index < numValues);

	double currentTime = timer.getTime();

	data[index] += (currentTime-lastTime);
	sumValues += (currentTime-lastTime);

	lastTime = currentTime;
} // ProfilingHelper

void ProfilingHelper::stop(Timer& timer) {
	for (int i=0; i < numValues; i++) {
//		fprintf(logFile, "%f\t", data[i]/sumValues);
		fprintf(logFile, "%f\t", data[i]);
	} // for
	fprintf(logFile, "\n");

	memset(this->data, 0, sizeof(double)*numValues);
	sumValues = 0;
} // ProfilingHelper

void ProfilingHelper::initialize(std::string logFile, int numValues,
		char** dataNames)
{
	this->lastTime = 0;
	this->numValues = numValues;
	this->data = new double[numValues];
	memset(this->data, 0, sizeof(double)*numValues);
	this->sumValues = 0;

	this->logFile = fopen(logFile.c_str(), "w");

	for (int i=0; i < numValues; i++) {
		fprintf(this->logFile, "%s\t", dataNames[i]);
	} // for

	fprintf(this->logFile, "\n");
} // initialize
