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
 * rlander:                                                                  *
 *   Created on: Jul 29, 2009                                                *
 *                                                                           *
\*---------------------------------------------------------------------------*/

#ifndef SENSORCORRECTION_H_
#define SENSORCORRECTION_H_

#include "ControllerManagerSharedLibraryExports.h"
#include "../../SystemCore/DataTypes.h"

/******************************************************************************
 * Stores correction values for controller sensor data
 */
class CONTROLLERMANAGER_API SensorCorrection {
public:

	/**
	 * Constructor
	 */
	explicit SensorCorrection(const TransformationData& coordinateCorrection=IdentityTransformation,
			const TransformationData& positionCorrection=IdentityTransformation,
			const TransformationData& orientationCorrection=IdentityTransformation);

	/**
	 * Corrects the passed sensor value and returns the result
	 */
	SensorData correctSensor(const SensorData& src) const;

	/**
	 *
	 */
	void setCoordinateSystemCorrection(const TransformationData& coordinateCorrection);

	/**
	 *
	 */
	void setPositionCorrection(const TransformationData& positionCorrection);

	/**
	 *
	 */
	void setOrientationCorrection(const TransformationData& orientationCorrection);

	/**
	 *
	 */
	TransformationData getCoordinateSystemCorrection() const;

	/**
	 *
	 */
	TransformationData getPositionCorrection() const;

	/**
	 *
	 */
	TransformationData getOrientationCorrection() const;


private:
	TransformationData coordinateCorrection;
	TransformationData positionCorrection;
	TransformationData orientationCorrection;
}; // SensorCorrection

extern const SensorCorrection DefaultSensorCorrection;

#endif /* SENSORCORRECTION_H_ */
