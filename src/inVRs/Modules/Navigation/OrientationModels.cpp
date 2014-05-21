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

#include "OrientationModels.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#include <gmtl/AxisAngle.h>
#include <gmtl/Generate.h>

#include <irrXML.h>

#include <inVRs/SystemCore/UserDatabase/UserDatabase.h>
#include <inVRs/SystemCore/UserDatabase/User.h>
#include <inVRs/SystemCore/DataTypes.h>
#include <inVRs/SystemCore/DebugOutput.h>

#ifndef M_PI
#define M_PI 3.1459f
#endif

using namespace irr;
using namespace io;

OrientationModel::OrientationModel()
{
	angle = M_PI*0.1f;
} // OrientationModel

void OrientationModel::setAngle(float ang){
	angle=ang;
}

OrientationButtonModel::OrientationButtonModel(int leftIdx, int rightIdx, int upIdx, int downIdx, int cwIdx, int ccwIdx)
: firstRun(true)
{
	buttonIndices[0] = upIdx;
	buttonIndices[1] = downIdx;
	buttonIndices[2] = leftIdx;
	buttonIndices[3] = rightIdx;
	buttonIndices[4] = ccwIdx;
	buttonIndices[5] = cwIdx;
} // OrientationButtonModel

void OrientationButtonModel::getOrientation(ControllerInterface* controller,
		gmtl::Quatf& result, float dt)
{
	int i;
	int num_buttons = controller->getNumberOfButtons();
	gmtl::Quatf xRot;
	gmtl::Quatf yRot;
	gmtl::Quatf zRot;
	float ret_vec[3];

	// reset the resulting vector
	memset(&ret_vec, 0, 3*sizeof(float));

	// check for invalid button configurations
	if (firstRun) {
		for (int i=0; i < 6; i++) {
			if (buttonIndices[i] >= num_buttons) {
				printd(WARNING, "OrientationButtonModel::getOrientation(): Button for rotation with index %i not provided by controller! Rotation may not work properly!\n",
					buttonIndices[i]);
			} // if
		} // for
		firstRun = false;
	} // if

	// since the rotation is applied locally to the user (multiplied to the
	// current user orientation) we have to multiply the y-axis with the
	// inverted user orientation to get the rotation around the global Y axis
	gmtl::Vec3f yDirection = gmtl::Vec3f(0, 1, 0);
	User* localUser = UserDatabase::getLocalUser();
	if (localUser) {
		gmtl::Quatf userOrienation = localUser->getNavigatedTransformation().orientation;
		gmtl::invert(userOrienation);
		yDirection = userOrienation * yDirection;
	} // if

	for (i=0; i < 6; i++) {
		// if button pressed then add the angle times the timestep to the
		// corresponding axis
		if (buttonIndices[i] >= 0 && buttonIndices[i] < num_buttons && controller->getButtonValue(buttonIndices[i]))
			ret_vec[i/2] += pow(-(float)1, (float)(i%2)) * angle * dt;
	} // for

	// create rotation datatypes (rotation around y axis follows global y axis):
	gmtl::AxisAnglef xAng(ret_vec[0], 1, 0, 0);
	gmtl::AxisAnglef yAng(ret_vec[1], yDirection);
	gmtl::AxisAnglef zAng(ret_vec[2], 0, 0, 1);

	gmtl::set(xRot, xAng);
	gmtl::set(yRot, yAng);
	gmtl::set(zRot, zAng);

	// first rotate around the local X-axis, then around the global Y-axis and
	// afterwards around the local Z-Axis
	result = xRot;
	result *= yRot;
	result *= zRot;
} // getOrientation


OrientationSingleAxisModel::OrientationSingleAxisModel(unsigned axisIdx, float minThreshold)
{
	this->axisIdx = axisIdx;
	this->minThreshold = minThreshold;
} // OrientationSingleAxisModel

void OrientationSingleAxisModel::getOrientation(ControllerInterface* controller,
		gmtl::Quatf& result, float dt)
{
	float new_axis;
	float ret_vec[3];
	gmtl::Quatf xRot;
	gmtl::Quatf yRot;
	gmtl::Quatf zRot;

	// initialize the axis value and the resulting vector
	new_axis = 0;
	memset(ret_vec, 0, 3*sizeof(float));

	// read axis value (if axis index is valid)
	if ((int)axisIdx < controller->getNumberOfAxes())
		new_axis = controller->getAxisValue(axisIdx);

	// reset controller axis value if it is below minimum threshold,
	// otherwise recalculate value so that range goes from
	// minThresold (which gets to 0) to 1
	if (fabs(new_axis) < minThreshold)
		new_axis = 0;
	else if (new_axis < 0)
		new_axis = (new_axis + minThreshold) / (1.f - minThreshold);
	else
		new_axis = (new_axis - minThreshold) / (1.f - minThreshold);

	// add the resulting axis value to the rotation around the local Y-axis
	ret_vec[1] = angle*new_axis * dt;

	// convert the calculated angles into rotations:
	// - positive value of controller axis 0 means rotation to right (which
	//   corresponds to clockwise around Y which is negative by default). Thus
	//   we have to invert the rotation angle around the Y-Axis
	gmtl::AxisAnglef xAng(ret_vec[0], 1, 0, 0);
	gmtl::AxisAnglef yAng(-ret_vec[1], 0, 1, 0);
	gmtl::AxisAnglef zAng(ret_vec[2], 0, 0, 1);

	gmtl::set(xRot, xAng);
	gmtl::set(yRot, yAng);
	gmtl::set(zRot, zAng);

	// first rotate around the local X-axis, then around the local Y-axis and
	// afterwards around the local Z-Axis
	result = xRot;
	result *= yRot;
	result *= zRot;
} // getOrientation

OrientationDualAxisModel::OrientationDualAxisModel(int xAxisIdx, int yAxisIdx, int buttonIdx,float minThresholdX, float minThresholdY):
		xAxisIdx(xAxisIdx), yAxisIdx(yAxisIdx), buttonIdx(buttonIdx)
{
	minThreshold[0] = minThresholdX;
	minThreshold[1] = minThresholdY;
}

void OrientationDualAxisModel::getOrientation(ControllerInterface* controller,
		gmtl::Quatf& result, float dt)
{
	int num_buttons = controller->getNumberOfButtons();
	int num_axis = controller->getNumberOfAxes();
	int button = 0;
	float new_axis[2];
	float ret_vec[3];
	gmtl::Quatf xRot;
	gmtl::Quatf yRot;
	gmtl::Quatf zRot;
	TransformationData navigatedPosition;
	gmtl::Vec3f yDirection;

	// initialize vector for axis values and for resulting vector
	memset(new_axis, 0, 2*sizeof(float));
	memset(ret_vec, 0, 3*sizeof(float));

	// read the button and axis values from the controller
	if (buttonIdx >= 0 && buttonIdx < num_buttons)
		button = controller->getButtonValue(buttonIdx);
	if (xAxisIdx >= 0 && xAxisIdx < num_axis)
		new_axis[0] = controller->getAxisValue(xAxisIdx);
	if (yAxisIdx >= 0 && yAxisIdx < num_axis)
		new_axis[1] = controller->getAxisValue(yAxisIdx);

	// since the rotation is applied locally to the user (multiplied to the
	// current user orientation) we have to multiply the y-axis with the
	// inverted user orientation to get the rotation around the global Y axis
	yDirection = gmtl::Vec3f(0, 1, 0);
	User* localUser = UserDatabase::getLocalUser();
	if (localUser) {
		gmtl::Quatf userOrienation = localUser->getNavigatedTransformation().orientation;
		gmtl::invert(userOrienation);
		yDirection = userOrienation * yDirection;
	} // if

	// reset controller axis value if it is below minimum threshold,
	// otherwise recalculate value so that range goes from
	// minThresold (which gets to 0) to 1
	for(int i = 0; i < 2; ++i) {
		if (fabs(new_axis[i]) < minThreshold[i])
			new_axis[i] = 0;
		else if (new_axis[i] < 0)
			new_axis[i] = (new_axis[i] + minThreshold[i]) / (1.f - minThreshold[i]);
		else
			new_axis[i] = (new_axis[i] - minThreshold[i]) / (1.f - minThreshold[i]);
	}

	// calculate the rotational change around the X-Axis (up/down)
	ret_vec[0] = angle * new_axis[1] * dt;

	// calculate the rotational change around the Y-Axis (left/right)
	// or the Z-Axis (clockwise/counterclockwise) depending on the button value
	if (button)
		ret_vec[2] = angle * new_axis[0] * dt;
	else
		ret_vec[1] = angle * new_axis[0] * dt;

	// convert the calculated angles into rotations (rotation around y axis
	// follows global y axis):
	// - positive value of controller axis 0 means rotation to right (which
	//   corresponds to clockwise which is negative by default). Thus we have
	//   to invert the rotation angle around the Y-Axis
	gmtl::AxisAnglef xAng(ret_vec[0], 1, 0, 0);
	gmtl::AxisAnglef yAng(-ret_vec[1], yDirection);
	gmtl::AxisAnglef zAng(ret_vec[2], 0, 0, -1);

	gmtl::set(xRot, xAng);
	gmtl::set(yRot, yAng);
	gmtl::set(zRot, zAng);

	// first rotate around the local X-axis, then around the global Y-axis and
	// afterwards around the local Z-Axis
	result = xRot;
	result *= yRot;
	result *= zRot;
} // getOrientation

OrientationDualAxisSensorViewDirectionModel::OrientationDualAxisSensorViewDirectionModel(
	int xAxisIdx, int yAxisIdx, int buttonIdx, int sensorIdx)
{
	this->xAxisIdx = xAxisIdx;
	this->yAxisIdx = yAxisIdx;
	this->buttonIdx = buttonIdx;
	this->sensorIdx = sensorIdx;
} // OrientationDualAxisSensorViewDirectionModel

void OrientationDualAxisSensorViewDirectionModel::getOrientation(
		ControllerInterface* controller, gmtl::Quatf& result, float dt)
{
		int num_buttons = controller->getNumberOfButtons();
		int num_axis = controller->getNumberOfAxes();
		int button = 0;
		float new_axis[2];
		float ret_vec[3];
		gmtl::Quatf xRot;
		gmtl::Quatf yRot;
		gmtl::Quatf zRot;
		TransformationData navigatedPosition;
		gmtl::Vec3f yDirection;

		// initialize vector for axis values and for resulting vector
		memset(new_axis, 0, 2*sizeof(float));
		memset(ret_vec, 0, 3*sizeof(float));

		// read the button and axis values from the controller
		if (buttonIdx >= 0 && buttonIdx < num_buttons)
				button = controller->getButtonValue(buttonIdx);
		if (xAxisIdx >= 0 && xAxisIdx < num_axis)
				new_axis[0] = controller->getAxisValue(xAxisIdx);
		if (yAxisIdx >= 0 && yAxisIdx < num_axis)
				new_axis[1] = controller->getAxisValue(yAxisIdx);

		// since the rotation is applied locally to the user (multiplied to the
		// current user orientation) we have to multiply the y-axis with the
		// inverted user orientation to get the rotation around the global Y axis
		yDirection = gmtl::Vec3f(0, 1, 0);
		User* localUser = UserDatabase::getLocalUser();
		if (localUser) {
				gmtl::Quatf userOrienation = localUser->getNavigatedTransformation().orientation;
				gmtl::invert(userOrienation);
				yDirection = userOrienation * yDirection;
		} // if

		// calculate the rotational change around the X-Axis (up/down)
		ret_vec[0] = angle * new_axis[1] * dt;

		// calculate the rotational change around the Y-Axis (left/right)
		// or the Z-Axis (clockwise/counterclockwise) depending on the button value
		if (button)
				ret_vec[2] = angle * new_axis[0] * dt;
		else
				ret_vec[1] = angle * new_axis[0] * dt;

	// rotate the axes by the orientation of the sensor
	gmtl::Vec3f xAxis(1, 0, 0);
	gmtl::Vec3f zAxis(0, 0, -1);
		SensorData sensor = controller->getSensorValue(sensorIdx);
		// rotate the axes by the orientation of the sensor
		xAxis = (sensor.orientation) * xAxis;
	zAxis = (sensor.orientation) * zAxis;

		// convert the calculated angles into rotations (rotation around y axis
		// follows global y axis):
		// - positive value of controller axis 0 means rotation to right (which
		//   corresponds to clockwise which is negative by default). Thus we have
		//   to invert the rotation angle around the Y-Axis
		gmtl::AxisAnglef xAng(ret_vec[0], xAxis);
		gmtl::AxisAnglef yAng(-ret_vec[1], yDirection);
		gmtl::AxisAnglef zAng(ret_vec[2], zAxis);

		gmtl::set(xRot, xAng);
		gmtl::set(yRot, yAng);
		gmtl::set(zRot, zAng);

		// first rotate around the local X-axis, then around the global Y-axis and
		// afterwards around the local Z-Axis
		result = xRot;
		result *= yRot;
		result *= zRot;
} // getOrientation


OrientationSensorModel::OrientationSensorModel(unsigned sensorIdx,
		float minThreshold, gmtl::Quatf adjust, gmtl::Vec3f mirrorAdjust)
{
	this->sensorIdx = sensorIdx;
	this->minThreshold = minThreshold;
	this->adjust = adjust;
	this->mirrorAdj = mirrorAdjust;
}

void OrientationSensorModel::getOrientation(ControllerInterface* controller,
		gmtl::Quatf& result, float dt)
{
	gmtl::AxisAnglef sensorOri;
	gmtl::Quatf sensorQuat;

	// obtain sensor value from controller
	SensorData sensor = controller->getSensorValue(sensorIdx);

	// Trying to correct sensor Orientation
	gmtl::set(sensorOri, sensor.orientation);
	sensorOri[1] *= mirrorAdj[0];
	sensorOri[2] *= mirrorAdj[1];
	sensorOri[3] *= mirrorAdj[2];
	gmtl::set(sensorQuat, sensorOri);

	// correct sensor rotation
	sensorQuat = adjust*sensorQuat;
	gmtl::set(sensorOri, sensorQuat);

	// check if sensor orientation angle is in predefined threshold for each
	// axis and remove axis part if below threshold
	if (fabs(sensorOri[0] * sensorOri[1]) < minThreshold) {
		sensorOri[1] = 0;
	} // if
	if (fabs(sensorOri[0] * sensorOri[2]) < minThreshold) {
		sensorOri[2] = 0;
	} // if
	if (fabs(sensorOri[0] * sensorOri[3]) < minThreshold) {
		sensorOri[3] = 0;
	} // if

	// check if a rotation around at least one axis is still there, otherwise
	// return no rotation change
	if (sensorOri[1] == sensorOri[2] == sensorOri[3] == 0) {
		result = gmtl::QUAT_MULT_IDENTITYF;
		return;
	} // if

	// reduce the rotation angle by the length of the current rotation vector
	// since some axis parts may have been removed
	float axisLength = gmtl::length(sensorOri.getAxis());
	sensorOri[0] *= axisLength;
	gmtl::Vec3f axis = sensorOri.getAxis();
	gmtl::normalize(axis);
	sensorOri.setAxis(axis);

	if (sensorOri[0] < 0)
		// assuming that sensor range is from -PI to +PI
		sensorOri[0] = (sensorOri[0] + minThreshold) / (M_PI - minThreshold);
	else
		sensorOri[0] = (sensorOri[0] - minThreshold) / (M_PI - minThreshold);

	// multiply rotation angle and dt for rotational change
	sensorOri[0] *= angle * dt;
	// set result
	gmtl::set(result, sensorOri);
} // getOrientation

/************************************
 * OrientationSensorFlyModel
 ***********************************/

OrientationSensorFlyModel::OrientationSensorFlyModel(unsigned sensorIdxOrientation, unsigned buttonIdx, float minThreshold)
{
	this->sensorIdxOrientation = sensorIdxOrientation;
	this->minThreshold = minThreshold;
	this->buttonIdx = buttonIdx;
	localUser = UserDatabase::getLocalUser();
}

void OrientationSensorFlyModel::getOrientation(ControllerInterface* controller,
		gmtl::Quatf& result, float dt)
{
	gmtl::AxisAnglef sensorOri;
	gmtl::Quatf sensorQuat;
	gmtl::Vec3f zRotated, yRotated, xRotated;

	SensorData sensor = controller->getSensorValue(sensorIdxOrientation);
	gmtl::set(sensorOri, sensor.orientation);

	if(controller->getButtonValue(buttonIdx) == 0)
	{
		result = gmtl::QUAT_IDENTITYF;
		return;
	}

	if(fabs(sensorOri[0]) < minThreshold)
	{
		result = gmtl::QUAT_IDENTITYF;
		return;
	}

// 	gmtl::invert(navigatedPosition.orientation);

	zRotated = sensor.orientation * gmtl::Vec3f(0, 0, 1);
	yRotated = sensor.orientation * gmtl::Vec3f(0, -1, 0);
	xRotated = sensor.orientation * gmtl::Vec3f(-1, 0, 0);

	float angleY = acosf(yRotated[1]); // between 0 und PI
	float angleX = acosf(xRotated[0]);

	result = gmtl::QUAT_IDENTITYF;

	if(angleX > minThreshold)
	{
		TransformationData navigatedPosition;
		gmtl::Vec3f yDirection;

// 		printf("angleX: %0.03f\n", angleX);

		navigatedPosition = localUser->getNavigatedTransformation();
		gmtl::invert(navigatedPosition.orientation);
		yDirection = gmtl::Vec3f(0, 1, 0);
		yDirection = navigatedPosition.orientation * yDirection;

		angleX *= zRotated[0] > 0.0f ? 1 : -1;
		sensorOri = gmtl::AxisAnglef(angleX, yDirection);
// 		sensorOri = gmtl::AxisAnglef(angleX, gmtl::Vec3f(0, 1, 0));

		sensorOri[0] *= 0.5f*dt;
		gmtl::set(sensorQuat, sensorOri);
		result *= sensorQuat;
	}
	if(angleY > minThreshold)
	{
		angleY *= zRotated[1] > 0.0f ? -1 : 1;
// 		printf("angleY: %0.03f\n", angleY);
// 		dumpVec(zRotated, "zRotated");
// 		dumpVec(yRotated, "yRotated");
		sensorOri = gmtl::AxisAnglef(angleY, gmtl::Vec3f(1, 0, 0));

		sensorOri[0] *= 0.5f*dt; // this here is a little bit framerate dependent :-(
		gmtl::set(sensorQuat, sensorOri);
		result *= sensorQuat;
	}

}

OrientationModel* DefaultOrientationModelFactory::create(std::string className,
		ArgumentVector* args) {
	if(className == "OrientationSensorFlyModel")
		return createOrientationSensorFlyModel(args);
	if(className == "OrientationButtonModel")
		return createOrientationButtonModel(args);
	if(className == "OrientationSingleAxisModel")
		return createOrientationSingleAxisModel(args);
	if(className == "OrientationDualAxisModel")
		return createOrientationDualAxisModel(args);
	if(className == "OrientationSensorModel")
		return createOrientationSensorModel(args);
	if(className == "OrientationDualAxisSensorViewDirectionModel")
		return createOrientationDualAxisSensorViewDirectionModel(args);

	return NULL;
} // create

OrientationModel* DefaultOrientationModelFactory::createOrientationSensorFlyModel(
		ArgumentVector* args) {
	OrientationSensorFlyModel* model;
	unsigned sensorIdx = 0;
	unsigned buttonIdx = 0;
	float minThreshold = 0;
//	bool finished = false;

	if (args && args->keyExists("sensorIndex"))
		args->get("sensorIndex", sensorIdx);
	else
		printd(WARNING,
				"DefaultOrientationModelFactory::createOrientationSensorFlyModel(): WARNING: argument sensorIndex missing, assuming default sensor 0!!!\n");

	if (args && args->keyExists("minThreshold"))
		args->get("minThreshold", minThreshold);

	if(args && args->keyExists("buttonIndex"))
		args->get("buttonIndex", buttonIdx);
	else
		printd(WARNING,
				"DefaultOrientationModelFactory::createOrientationSensorFlyModel(): WARNING: argument buttonIndex missing, assuming default button 0!!!\n");

	if (minThreshold < 0 || minThreshold >= M_PI) {
		printd(WARNING, "OrientationSensorModelFactory::create(): WARNING: minThreshold must be between 0 and +PI!!!\n");
		minThreshold = 0;
	} // if

	model = new OrientationSensorFlyModel(sensorIdx, buttonIdx, minThreshold);

	return model;
}

OrientationModel* DefaultOrientationModelFactory::createOrientationButtonModel(
		ArgumentVector* args) {
	int leftIdx = -1;
	int rightIdx = -1;
	int upIdx = -1;
	int downIdx = -1;
	int cwIdx = -1;
	int ccwIdx = -1;
	unsigned int temp;

	if (args && args->keyExists("leftIndex")) {
		args->get("leftIndex", temp);
		leftIdx = temp;
	} // if
	if (args && args->keyExists("rightIndex")) {
		args->get("rightIndex", temp);
		rightIdx = temp;
	} // if
	if (args && args->keyExists("upIndex")) {
		args->get("upIndex", temp);
		upIdx = temp;
	} //  if
	if (args && args->keyExists("downIndex")) {
		args->get("downIndex", temp);
		downIdx = temp;
	} // if
	if (args && args->keyExists("clockwiseIndex")) {
		args->get("clockwiseIndex", temp);
		cwIdx = temp;
	} // if
	if (args && args->keyExists("counterclockwiseIndex")) {
		args->get("counterclockwiseIndex", temp);
		ccwIdx = temp;
	} // if

	return new OrientationButtonModel(leftIdx, rightIdx, upIdx, downIdx, cwIdx, ccwIdx);
} // createOrientationButtonModel

OrientationModel* DefaultOrientationModelFactory::createOrientationSingleAxisModel(
		ArgumentVector* args) {
	float minThreshold = 0;
	unsigned axisIdx = 0;

	if (!args || !args->keyExists("axisIndex"))
		printd(WARNING, "OrientationSingleAxisModelFactory::create: missing attribute axisIndex! Using default axis 0!\n");
	else if (args)
		args->get("axisIndex", axisIdx);

	if (args && args->keyExists("minThreshold"))
		args->get("minThreshold", minThreshold);

	return new OrientationSingleAxisModel(axisIdx, minThreshold);
}

OrientationModel* DefaultOrientationModelFactory::createOrientationDualAxisModel(
		ArgumentVector* args) {
	int xAxisIdx = -1;
	int yAxisIdx = -1;
	int buttonIdx = -1;
	float minThresholdX = 0;
	float minThresholdY = 0;

	if (args && args->keyExists("xAxisIndex"))
		args->get("xAxisIndex", xAxisIdx);
	if (args && args->keyExists("yAxisIndex"))
		args->get("yAxisIndex", yAxisIdx);
	if (args && args->keyExists("buttonIndex"))
		args->get("buttonIndex", buttonIdx);
	if (args && args->keyExists("minThreshold")) {
		args->get("minThreshold", minThresholdX);
		minThresholdY = minThresholdX;
	}
	if (args && args->keyExists("minThresholdX"))
		args->get("minThresholdX", minThresholdX);
	if (args && args->keyExists("minThresholdY"))
		args->get("minThresholdY", minThresholdY);

	if (xAxisIdx == -1 && yAxisIdx == -1 && buttonIdx == -1) {
		printd(ERROR,
				"OrientationAxisModelFactory::create(): ERROR: attributes xAxisIndex, yAxisIndex and/or buttonIndex missing in OrientationModel config!!!\n");
		return NULL;
	} // if

	return new OrientationDualAxisModel(xAxisIdx, yAxisIdx, buttonIdx, minThresholdX, minThresholdY);
}

OrientationModel* DefaultOrientationModelFactory::createOrientationDualAxisSensorViewDirectionModel(
		ArgumentVector* args) {
		int xAxisIdx = -1;
		int yAxisIdx = -1;
		int buttonIdx = -1;
	int sensorIdx = -1;

		if (args && args->keyExists("xAxisIndex"))
				args->get("xAxisIndex", xAxisIdx);
		if (args && args->keyExists("yAxisIndex"))
				args->get("yAxisIndex", yAxisIdx);
		if (args && args->keyExists("buttonIndex"))
				args->get("buttonIndex", buttonIdx);
	if (args && args->keyExists("sensorIndex"))
		args->get("sensorIndex", sensorIdx);

		if ((xAxisIdx == -1 && yAxisIdx == -1 && buttonIdx == -1) || sensorIdx == -1) {
				printd(ERROR,
								"DefaultOrientationModelFactory::createOrientationDualAxisSensorViewDirectionModel(): ERROR: attributes xAxisIndex, yAxisIndex, buttonIndex and/or sensorIndex missing in OrientationModel config!!!\n");
				return NULL;
		} // if

		return new OrientationDualAxisSensorViewDirectionModel(xAxisIdx, yAxisIdx, buttonIdx, sensorIdx);

} // createOrientationDualAxisSensorViewDirectionModel

OrientationModel* DefaultOrientationModelFactory::createOrientationSensorModel(
		ArgumentVector* args) {
	OrientationSensorModel* model;
	unsigned sensorIdx = 0;
	float minThreshold = 0;
	gmtl::Quatf rotationAdjust = gmtl::QUAT_IDENTITYF;
	gmtl::Vec3f mirrorAdjust = gmtl::Vec3f(1, 1, 1);

	if (args && args->keyExists("sensorIndex"))
		args->get("sensorIndex", sensorIdx);
	else
		printd(WARNING, "OrientationSensorModelFactory::create(): WARNING: argument sensorIndex missing, assuming default sensor 0!!!\n");

	if (args && args->keyExists("minThreshold"))
		args->get("minThreshold", minThreshold);

	if (minThreshold < 0 || minThreshold >= M_PI)
	{
		printd(WARNING, "OrientationSensorModelFactory::create(): WARNING: minThreshold must be between 0 and +PI!!!\n");
		minThreshold = 0;
	} // if

	float adj[4];
	memset(&adj, 0, 4*sizeof(float));

	if(args && args->keyExists("rotationAdjAxisX"))
		args->get("rotationAdjAxisX", adj[0]);
	if(args && args->keyExists("rotationAdjAxisY"))
		args->get("rotationAdjAxisY", adj[1]);
	if(args && args->keyExists("rotationAdjAxisZ"))
		args->get("rotationAdjAxisZ", adj[2]);
	if(args && args->keyExists("rotationAdjAngleDeg")) {
		args->get("rotationAdjAngleDeg", adj[3]);
		adj[3] = adj[3] * gmtl::Math::PI/180.0f;
	} // if

	if (adj[3] != 0) {
		gmtl::AxisAnglef adjAng(adj[3], adj[0], adj[1], adj[2]);
		gmtl::set(rotationAdjust, adjAng);
	} // if

	if(args && args->keyExists("mirrorAdjXFactor"))
		args->get("mirrorAdjXFactor", mirrorAdjust[0]);
	if(args && args->keyExists("mirrorAdjYFactor"))
		args->get("mirrorAdjYFactor", mirrorAdjust[1]);
	if(args && args->keyExists("mirrorAdjZFactor"))
		args->get("mirrorAdjZFactor", mirrorAdjust[2]);

	model = new OrientationSensorModel(sensorIdx, minThreshold, rotationAdjust,
			mirrorAdjust);

	return model;
} // createOrientationSensorModel
