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

#include "TranslationModels.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gmtl/Xforms.h>

#include "Navigation.h"

/*********************************
 * TranslationViewDirectionModel
 ********************************/

void TranslationViewDirectionModel::getTranslation(ControllerInterface* controller,
		const gmtl::Quatf& rotationChange, gmtl::Vec3f& result, float dt) {

	// return the view direction in local coordinates (relative to user) which
	// corresponds to the negative y axis!
	gmtl::Vec3f axis(0, 0, -1);
	result = axis;
} // getTranslation


/***************************************
 * TranslationSensorViewDirectionModel
 **************************************/

TranslationSensorViewDirectionModel::TranslationSensorViewDirectionModel(unsigned sensorIdx,
		bool ignoreYAxis) {
	this->sensorIdx = sensorIdx;
	this->ignoreYAxis = ignoreYAxis;
} // TranslationSensorViewDirectionModel

void TranslationSensorViewDirectionModel::getTranslation(ControllerInterface* controller,
		const gmtl::Quatf& rotationChange, gmtl::Vec3f& result, float dt) {

	// default view vector (forward)
	gmtl::Vec3f axis(0, 0, -1);
	// get sensor value
	SensorData sensor = controller->getSensorValue(sensorIdx);
	// rotate the forward vector by the orientation of the sensor
	result = (sensor.orientation) * axis;

	if (ignoreYAxis) {
		// remove the Y-axis part
		result[1] = 0;
	} // if
	// normalize result
	gmtl::normalize(result);
} // getTranslation


/***************************************
 * TranslationViewDirectionButtonStrafeModel
 **************************************/

TranslationViewDirectionButtonStrafeModel::TranslationViewDirectionButtonStrafeModel(
		unsigned frontIdx, unsigned backIdx, unsigned leftIdx, unsigned rightIdx) {
	bttnIdx[0] = frontIdx;
	bttnIdx[1] = backIdx;
	bttnIdx[2] = rightIdx;
	bttnIdx[3] = leftIdx;
} // TranslationViewDirectionButtonStrafeModel

void TranslationViewDirectionButtonStrafeModel::getTranslation(ControllerInterface* controller,
		const gmtl::Quatf& rotationChange, gmtl::Vec3f& result, float dt) {
	int i;
	gmtl::Vec3f dirs[4];
	gmtl::Vec3f temp;

	// forward vector
	dirs[0] = gmtl::Vec3f(0, 0, -1);
	// backward vector
	dirs[1] = gmtl::Vec3f(0, 0, 1);
	// right vector
	dirs[2] = gmtl::Vec3f(1, 0, 0);
	// left vector
	dirs[3] = gmtl::Vec3f(-1, 0, 0);

	// initialize result
	result = gmtl::Vec3f(0, 0, 0);
	for (i = 0; i < 4; i++) {
		// check for valid button index
		if ((unsigned)controller->getNumberOfButtons() <= bttnIdx[i])
			break;
		// add axis to result if corresponding button is pressed
		if (controller->getButtonValue(bttnIdx[i]) != 0) {
			result += dirs[i];
		} // if
	} // for
	// normalize result
	gmtl::normalize(result);
} // getTranslation


/*******************************************
 * TranslationViewDirectionAxisStrafeModel
 ******************************************/

TranslationViewDirectionAxisStrafeModel::TranslationViewDirectionAxisStrafeModel(
		unsigned frontBackIdx, unsigned leftRightIdx) {
	axisIdx[0] = leftRightIdx;
	axisIdx[1] = frontBackIdx;
} // TranslationViewDirectionAxisStrafeModel

void TranslationViewDirectionAxisStrafeModel::getTranslation(ControllerInterface* controller,
		const gmtl::Quatf& rotationChange, gmtl::Vec3f& result, float dt) {
	int i;
	gmtl::Vec3f dirs[2];
	gmtl::Vec3f temp;

	// Right direction
	dirs[0] = gmtl::Vec3f(1, 0, 0);
	// View direction (front)
	dirs[1] = gmtl::Vec3f(0, 0, -1);

	// initialize result
	result = gmtl::Vec3f(0, 0, 0);

	for (i = 0; i < 2; i++) {
		// check if axisindex is inside the number of available axes
		if (controller->getNumberOfAxes() <= (int)axisIdx[i])
			break;
		// multiply the vector with the axis value (the more the axis value
		// the more the vector influence) and add the calculated vector to the
		// result
		result += dirs[i] * controller->getAxisValue(axisIdx[i]);
	} // for
	// normalize the vector (so that the speed is solely defined by the speed
	// model and not influenced by the vector length)
	gmtl::normalize(result);
} // getTranslation

/*******************************************
 * DefaultTranslationModelFactory
 ******************************************/

DefaultTranslationModelFactory::~DefaultTranslationModelFactory() {

} // ~DefaultTranslationModelFactory

TranslationModel* DefaultTranslationModelFactory::create(std::string className,
		ArgumentVector* args) {

	if (className == "TranslationViewDirectionModel")
		return createTranslationViewDirectionModel(args);
	if (className == "TranslationSensorViewDirectionModel")
		return createTranslationSensorViewDirectionModel(args);
	if (className == "TranslationViewDirectionButtonStrafeModel")
		return createTranslationViewDirectionButtonStrafeModel(args);
	if (className == "TranslationViewDirectionAxisStrafeModel")
		return createTranslationViewDirectionAxisStrafeModel(args);

	return NULL;
} // create

TranslationModel* DefaultTranslationModelFactory::createTranslationViewDirectionModel(
		ArgumentVector* args) {
	return new TranslationViewDirectionModel();
} // createTranslationViewDirectionModel

TranslationModel* DefaultTranslationModelFactory::createTranslationSensorViewDirectionModel(
		ArgumentVector* args) {
	std::string inputString;
	unsigned sensorIdx = 0;
	bool ignoreYAxis = false;

	if (args && args->keyExists("sensorIndex"))
		args->get("sensorIndex", sensorIdx);
	else
		printd(WARNING,
				"TranslationSensorViewDirectionModelFactory::create(): WARNING: missing attribute sensorIdx, assuming sensor 0!\n");

	if (args && args->keyExists("ignoreYAxis")) {
		args->get("ignoreYAxis", ignoreYAxis);
	} // if

	return new TranslationSensorViewDirectionModel(sensorIdx, ignoreYAxis);
} // createTranslationSensorViewDirectionModel

TranslationModel* DefaultTranslationModelFactory::createTranslationViewDirectionButtonStrafeModel(
		ArgumentVector* args) {
	bool error = false;
	unsigned frontIdx, backIdx, leftIdx, rightIdx;

	if (!args || !args->keyExists("frontIndex")) {
		printd(ERROR,
				"TranslationViewDirectionButtonStrafeModelFactory::create(): ERROR: attribute frontIndex missing in config File!\n");
		error = true;
	} // if
	if (!args || !args->keyExists("backIndex")) {
		printd(ERROR,
				"TranslationViewDirectionButtonStrafeModelFactory::create(): ERROR: attribute backIndex missing in config File!\n");
		error = true;
	} // if
	if (!args || !args->keyExists("leftIndex")) {
		printd(ERROR,
				"TranslationViewDirectionButtonStrafeModelFactory::create(): ERROR: attribute leftIndex missing in config File!\n");
		error = true;
	} // if
	if (!args || !args->keyExists("rightIndex")) {
		printd(ERROR,
				"TranslationViewDirectionButtonStrafeModelFactory::create(): ERROR: attribute rightIndex missing in config File!\n");
		error = true;
	} // if

	if (error)
		return NULL;

	args->get("frontIndex", frontIdx);
	args->get("backIndex", backIdx);
	args->get("leftIndex", leftIdx);
	args->get("rightIndex", rightIdx);

	return new TranslationViewDirectionButtonStrafeModel(frontIdx, backIdx, leftIdx, rightIdx);
} // createTranslationViewDirectionButtonStrafeModel

TranslationModel* DefaultTranslationModelFactory::createTranslationViewDirectionAxisStrafeModel(
		ArgumentVector* args) {
	unsigned leftRightIdx = 0;
	unsigned frontBackIdx = 1;

	if (!args->keyExists("leftRightIndex"))
		printd(
				WARNING,
				"TranslationViewDirectionAxisStrafeModelFactory::create(): WARNING: missing attribute leftRightIndex for TranslationModel, using default value %u!!!\n",
				leftRightIdx);
	else
		args->get("leftRightIndex", leftRightIdx);

	if (!args->keyExists("frontBackIndex"))
		printd(
				WARNING,
				"TranslationViewDirectionAxisStrafeModelFactory::create(): WARNING: missing attribute frontBackIndex for TranslationModel, using default value %u!!!\n",
				frontBackIdx);
	else
		args->get("frontBackIndex", frontBackIdx);

	return new TranslationViewDirectionAxisStrafeModel(frontBackIdx, leftRightIdx);
} // createTranslationViewDirectionAxisStrafeModel
