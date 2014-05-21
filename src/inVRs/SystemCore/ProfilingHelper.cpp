/*
 * ProfilingHelper.cpp
 *
 *  Created on: May 19, 2009
 *      Author: rlander
 */

#include "ProfilingHelper.h"

#include "Timer.h"
#include "DebugOutput.h"

using namespace std;

namespace inVRsUtilities {

ProfilingHelper::ProfilingHelper() :
	logFile(NULL),
	firstRun(true),
	invalidStep(true),
	lastTime(0),
	index(0),
	numValues(0) {

} // ProfilingHelper

ProfilingHelper::~ProfilingHelper() {
	if (logFile)
		fclose(logFile);
} // ~ProfilingHelper

void ProfilingHelper::setLogFile(std::string file) {
	if (logFile) {
		printd(ERROR, "ProfilingHelper::setLogFile(): another logfile was already set!\n");
		return;
	} // if
	logFile = fopen(file.c_str(), "w");
} // setLogFile

void ProfilingHelper::start() {
	if (!logFile)
		return;

	lastTime = timer.getTime();
	index = 0;
	invalidStep = false;
} // start

void ProfilingHelper::step(std::string name) {
	if (!logFile || invalidStep)
		return;

	double currentTime = timer.getTime();
	double dt = currentTime - lastTime;

	if (firstRun) {
		names.push_back(name);
	} // if
	else {
		if (index >= (int)names.size()) {
			printd(ERROR, "ProfilingHelper::step(): index out of range! Please ensure that all steps are called every frame!\n");
			invalidStep = true;
			return;
		} // if
		else if (names.at(index) != name) {
			printd(ERROR, "ProfilingHelper::step(): name of step does not match! Please ensure that all steps are called every frame!\n");
			invalidStep = true;
			return;
		} // else if
	} // else
	values.push_back(dt);
	lastTime = currentTime;
	index++;
} // ProfilingHelper

void ProfilingHelper::interrupt() {
	invalidStep = true;
	values.clear();
	if (firstRun) {
		names.clear();
	} // if
} // interrupt

void ProfilingHelper::end() {
	if (!logFile)
		return;

	if (!invalidStep) {
		if (firstRun) {
			std::vector<string>::iterator it;
			for (it = names.begin(); it != names.end(); ++it) {
				fprintf(logFile, "%s\t", (*it).c_str());
			} // for
			fprintf(logFile, "\n");

			numValues = index;
			firstRun = false;
		} // if

		if (index != numValues) {
			printd(ERROR, "ProfilingHelper::stop(): one ore more steps are missing! Please ensure that all steps are called every frame!\n");
		} // if
		else {
			std::vector<double>::iterator it;
			for (it = values.begin(); it != values.end(); ++it) {
				fprintf(logFile, "%f\t", *it);
			} // for
			fprintf(logFile, "\n");
		} // else
	} // else
	values.clear();
} // ProfilingHelper

} // inVRsUtilities
