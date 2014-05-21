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

#include "SensorCorrection.h"

const SensorCorrection DefaultSensorCorrection = SensorCorrection();

SensorCorrection::SensorCorrection(const TransformationData& coordinateCorrection,
		const TransformationData& positionCorrection,
		const TransformationData& orientationCorrection) :
	coordinateCorrection(coordinateCorrection),
	positionCorrection(positionCorrection),
	orientationCorrection(orientationCorrection) {

} // SensorCorrection

SensorData SensorCorrection::correctSensor(const SensorData& src) const {

	//TODO: convert corrections into single Transformation-Matrix to fix transformations via multiply
	SensorData result = src;
	// move coordinate system relative to sensor
	// inverted because if coordinate system moves 1pt then sensor result changes by -1pt
	result.position -= coordinateCorrection.position;
	// rotate coordinate system relative to sensors
	gmtl::Quatf quat = coordinateCorrection.orientation;
	gmtl::invert(quat);
	result.position = quat * result.position;
	// scale sensor value (this one is scaling the sensor, not the coordinate system)
	result.position[0] *= coordinateCorrection.scale[0];
	result.position[1] *= coordinateCorrection.scale[1];
	result.position[2] *= coordinateCorrection.scale[2];
	result.position += positionCorrection.position;
	result.position[0] *= positionCorrection.scale[0];
	result.position[1] *= positionCorrection.scale[1];
	result.position[2] *= positionCorrection.scale[2];

	// calculate rotation axis
	gmtl::AxisAnglef axAng;
	gmtl::set(axAng, result.orientation);
	gmtl::Vec3f axis = axAng.getAxis();
	// multiply rotation axis with inverted coordinate correction rotation
	axis = quat * axis;
	// scale rotation axis (in order to fix inversions)
	axis[0] *= coordinateCorrection.scale[0];
	axis[1] *= coordinateCorrection.scale[1];
	axis[2] *= coordinateCorrection.scale[2];
	gmtl::normalize(axis);
	axAng.setAxis(axis);
	gmtl::set(result.orientation, axAng);
	// rotate rotation axis by orientation correction
	result.orientation *= orientationCorrection.orientation;

	return result;
} // correctSensor

void SensorCorrection::setCoordinateSystemCorrection(const TransformationData& coordinateCorrection) {
	this->coordinateCorrection = coordinateCorrection;
} // setCoordinateSystemCorrection

void SensorCorrection::setPositionCorrection(const TransformationData& positionCorrection) {
	this->positionCorrection = positionCorrection;
} // setPositionCorrection

void SensorCorrection::setOrientationCorrection(const TransformationData& orientationCorrection) {
	this->orientationCorrection = orientationCorrection;
} // setOrientationCorrection

TransformationData SensorCorrection::getCoordinateSystemCorrection() const {
	return coordinateCorrection;
} // getCoordinateSystemCorrection

TransformationData SensorCorrection::getPositionCorrection() const {
	return positionCorrection;
} // getPositionCorrection

TransformationData SensorCorrection::getOrientationCorrection() const {
	return orientationCorrection;
} // getOrientationCorrection
