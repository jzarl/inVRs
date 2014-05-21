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

#include "SpeedModels.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <sstream>
#include <assert.h>
#include "Navigation.h"
#include <inVRs/SystemCore/DebugOutput.h>

SpeedModel::SpeedModel() {
	speed = 1;
} // SpeedModel

void SpeedModel::setSpeed(float spd) {
	speed = spd;
} // setSpeed

SpeedFixedSpeedModel::SpeedFixedSpeedModel(bool useTimestep) {
	this->useTimestep = useTimestep;
} // SpeedFixedSpeedModel

float SpeedFixedSpeedModel::getSpeed(ControllerInterface* controller,
		const gmtl::Quatf& rotationChange, const gmtl::Vec3f& translationChange, float dt) {

	// return fix speed (either multiplied with timestep or not)
	if (useTimestep)
		return speed * dt;
	else
		return speed;
} // getSpeed


SpeedAxisModel::SpeedAxisModel(unsigned axisIdx, float minThreshold) {
	this->axisIdx = axisIdx;
	this->minThreshold = minThreshold;
} // SpeedAxisModel

float SpeedAxisModel::getSpeed(ControllerInterface* controller,
		const gmtl::Quatf& rotationChange, const gmtl::Vec3f& translationChange, float dt) {

	// initialize result and axis value
	float ret = 0;
	float axisValue = controller->getAxisValue(axisIdx);

	// normalize axis value in range between minThreshold an 1
	if (fabs(axisValue) < minThreshold)
		axisValue = 0;
	else if (axisValue > 0)
		axisValue = (axisValue - minThreshold) / (1.f - minThreshold);
	else
		axisValue = (axisValue + minThreshold) / (1.f - minThreshold);

	// calculate speed result according to axis value
	ret = axisValue * speed * dt;
	return ret;
} // getSpeed

SpeedButtonModel::SpeedButtonModel(int accelButtonIdx, int decelButtonIdx) {
	this->accelButtonIdx = accelButtonIdx;
	this->decelButtonIdx = decelButtonIdx;
} // SpeedButtonModel

float SpeedButtonModel::getSpeed(ControllerInterface* controller,
		const gmtl::Quatf& rotationChange, const gmtl::Vec3f& translationChange, float dt) {
	float result = 0;

	// get button value for acceleration and add speed if pressed
	if (accelButtonIdx >= 0 && controller->getButtonValue(accelButtonIdx))
		result += speed * dt;
	// get button value for deceleration and add inverse speed if pressed
	if (decelButtonIdx >= 0 && controller->getButtonValue(decelButtonIdx))
		result -= speed * dt;

	return result;
} // getSpeed


SpeedMultiButtonModel::SpeedMultiButtonModel(std::string accelButtonString,
		std::string decelButtonString) {
	std::stringstream ssAccel(std::stringstream::in | std::stringstream::out);
	std::stringstream ssDecel(std::stringstream::in | std::stringstream::out);

	// reset button Index to -1 (because eof() function is false at the first
	// run)
	int buttonIdx = -1;
	// parse all acceleration button indices
	ssAccel << accelButtonString;
	while (!ssAccel.eof()) {
		ssAccel >> buttonIdx;
		if (buttonIdx >= 0) {
			printd(INFO,
					"SpeedAbsoluteMultiButtonModel::SpeedAbsoluteMultiButtonModel(): adding accel button %u!\n",
					buttonIdx);
			accelButtonIndices.push_back(buttonIdx);
		} // if
	} // while

	// reset button Index (because eof() function is false at the first run)
	buttonIdx = -1;
	// parse all deceleration button indices
	ssDecel << decelButtonString;
	while (!ssDecel.eof()) {
		ssDecel >> buttonIdx;
		if (buttonIdx >= 0) {
			printd(INFO,
					"SpeedAbsoluteMultiButtonModel::SpeedAbsoluteMultiButtonModel(): adding decel button %u!\n",
					buttonIdx);
			decelButtonIndices.push_back(buttonIdx);
		} // if
	} // while
} // SpeedMultiButtonModel

float SpeedMultiButtonModel::getSpeed(ControllerInterface* controller,
		const gmtl::Quatf& rotationChange, const gmtl::Vec3f& translationChange, float dt) {

	int i;
	// initialize the number of buttons and the result variable
	unsigned numButtons = controller->getNumberOfButtons();
	float result = 0;

	// if any of the acceleration buttons is pressed then set the resulting
	// acceleration speed and exit the loop
	for (i = 0; i < (int)accelButtonIndices.size(); i++) {
		if (accelButtonIndices[i] < numButtons && controller->getButtonValue(accelButtonIndices[i])) {
			result += speed * dt;
			break;
		} // if
	} // for
	// if any of the deceleration buttons is pressed then subtract the speed
	// value from the result and exit the loop
	for (i = 0; i < (int)decelButtonIndices.size(); i++) {
		if (decelButtonIndices[i] < numButtons && controller->getButtonValue(decelButtonIndices[i])) {
			result -= speed * dt;
			break;
		} // if
	} // for

	// return the result
	return result;
} // getSpeed

SpeedDualAxisModel::SpeedDualAxisModel(unsigned axis1Idx, unsigned axis2Idx) {
	this->axis1Idx = axis1Idx;
	this->axis2Idx = axis2Idx;
} // SpeedDualAxisModel

float SpeedDualAxisModel::getSpeed(ControllerInterface* controller,
		const gmtl::Quatf& rotationChange, const gmtl::Vec3f& translationChange, float dt) {
	float a0, a1;
	float ret;
	// get the two axis values
	a0 = controller->getAxisValue(axis1Idx);
	a1 = controller->getAxisValue(axis2Idx);

	// set the speed according to the two axis values
	ret = sqrtf(a0 * a0 + a1 * a1) * speed * dt;

	// return result
	return ret;
} // getSpeed

DefaultSpeedModelFactory::~DefaultSpeedModelFactory() {

} // DefaultSpeedModelFactory

SpeedModel* DefaultSpeedModelFactory::create(std::string className, ArgumentVector* args) {
	if (className == "SpeedFixedSpeedModel")
		return createSpeedFixedSpeedModel(args);
	if (className == "SpeedAxisModel")
		return createSpeedAxisModel(args);
	if (className == "SpeedButtonModel")
		return createSpeedButtonModel(args);
	if (className == "SpeedMultiButtonModel")
		return createSpeedMultiButtonModel(args);
	if (className == "SpeedDualAxisModel")
		return createSpeedDualAxisModel(args);

	return NULL;
} // create

SpeedModel* DefaultSpeedModelFactory::createSpeedFixedSpeedModel(ArgumentVector* args) {
	if (args && args->keyExists("useTimestep")) {
		bool useTimestep;
		args->get("useTimestep", useTimestep);
		return new SpeedFixedSpeedModel(useTimestep);
	} // if
	else {
		return new SpeedFixedSpeedModel();
	} // else
} // createSpeedFixedSpeedModel

SpeedModel* DefaultSpeedModelFactory::createSpeedAxisModel(ArgumentVector* args) {
	unsigned axisIdx = 0;
	float minThreshold = 0;

	if (args && args->keyExists("axisIndex"))
		args->get("axisIndex", axisIdx);
	if (args && args->keyExists("minThreshold"))
		args->get("minThreshold", minThreshold);
	if (minThreshold < 0 || minThreshold >= 1) {
		printd(WARNING,
				"DefaultSpeedModelFactory::createSpeedAxisModel(): invalid minimum Threshold %f - assumed a value beween 0 and 1!!!\n",
				minThreshold);
		minThreshold = 0;
	} // if

	return new SpeedAxisModel(axisIdx, minThreshold);
} // createSpeedAxisModel

SpeedModel* DefaultSpeedModelFactory::createSpeedButtonModel(ArgumentVector* args) {
	int accelButtonIdx = -1;
	int decelButtonIdx = -1;

	if (!args || (!args->keyExists("accelButtonIndex") && !args->keyExists("decelButtonIndex"))) {
		printd(ERROR,
				"DefaultSpeedModelFactory::createSpeedButtonModel(): Missing button index attributes accelButtonIdx and/or decelButtonIdx!\n");
		return NULL;
	} // if

	if (args->keyExists("accelButtonIndex"))
		args->get("accelButtonIndex", accelButtonIdx);
	if (args->keyExists("decelButtonIndex"))
		args->get("decelButtonIndex", decelButtonIdx);

	return new SpeedButtonModel(accelButtonIdx, decelButtonIdx);
} // createSpeedButtonModel

SpeedModel* DefaultSpeedModelFactory::createSpeedMultiButtonModel(ArgumentVector* args) {
	std::string accelButtonString, decelButtonString;

	if (!args ||
			(!args->keyExists("accelButtonIndices") && !args->keyExists("decelButtonIndices"))) {
		printd(ERROR,
				"DefaultSpeedModelFactory::createSpeedMultiButtonModel(): Missing attributes accelButtonIndices and/or decelButtonIndices!\n");
		return NULL;
	} // if

	if (args->keyExists("accelButtonIndices"))
		args->get("accelButtonIndices", accelButtonString);

	if (args->keyExists("decelButtonIndices"))
		args->get("decelButtonIndices", decelButtonString);

	return new SpeedMultiButtonModel(accelButtonString, decelButtonString);
} // createSpeedMultiButtonModel

SpeedModel* DefaultSpeedModelFactory::createSpeedDualAxisModel(ArgumentVector* args) {
	unsigned axis1Idx, axis2Idx;

	if (!args || !args->keyExists("axis1Index") || !args->keyExists("axis2Index")) {
		printd(ERROR,
				"DefaultSpeedModelFactory::createSpeedDualAxisModel(): Missing argument axis1Idx or axis2Idx for SpeedAbsoluteDualAxisModel!!!\n");
		return NULL;
	} // if

	args->get("axis1Index", axis1Idx);
	args->get("axis2Index", axis2Idx);

	return new SpeedDualAxisModel(axis1Idx, axis2Idx);
} // createSpeedDualAxisModel
