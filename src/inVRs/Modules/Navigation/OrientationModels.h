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

#ifndef _ORIENTATIONMODELS_H
#define _ORIENTATIONMODELS_H

#include <gmtl/Quat.h>
#include <gmtl/Matrix.h>

#include "NavigationSharedLibraryExports.h"
#include <inVRs/InputInterface/ControllerInterface.h>
#include <inVRs/SystemCore/ClassFactory.h>
#include <inVRs/SystemCore/XMLTools.h>

#ifdef WIN32
#pragma warning( disable : 4507 )
#endif

class User;

class INVRS_NAVIGATION_API OrientationModel {
public:
	OrientationModel();
	virtual ~OrientationModel(){};

	void setAngle(float ang);
	virtual void getOrientation(ControllerInterface* controller, gmtl::Quatf& result, float dt)=0;

protected:
	float angle;
};

typedef ClassFactory<OrientationModel, ArgumentVector*> OrientationModelFactory;

/******************************************************************************
 * @class OrientationButtonModel
 */
class INVRS_NAVIGATION_API OrientationButtonModel : public OrientationModel
{
public:
	OrientationButtonModel(int leftIdx, int rightIdx, int upIdx, int downIdx, int cwIdx, int ccwIdx);
	virtual void getOrientation(ControllerInterface* controller, gmtl::Quatf& result, float dt);

protected:
	bool firstRun;
	int buttonIndices[6];
}; // OrientationButtonModel


/******************************************************************************
 * @class OrientationSingleAxisModel
 */
class INVRS_NAVIGATION_API OrientationSingleAxisModel : public OrientationModel {
public:
	OrientationSingleAxisModel(unsigned axisIdx, float minThreshold);
	virtual void getOrientation(ControllerInterface* controller, gmtl::Quatf& result, float dt);

protected:
	unsigned axisIdx;
	float minThreshold;
};


/******************************************************************************
 * @class OrientationDualAxisModel
 */
class INVRS_NAVIGATION_API OrientationDualAxisModel : public OrientationModel {
public:
	OrientationDualAxisModel(int xAxisIdx, int yAxisIdx, int buttonIdx, float minThresholdX, float minThresholdY);
	virtual void getOrientation(ControllerInterface* controller, gmtl::Quatf& result, float dt);

protected:
	int xAxisIdx, yAxisIdx;
	int buttonIdx;
	float minThreshold[2];
};

/******************************************************************************
 * @class OrientationDualAxisSensorViewDirectionModel
 */
class INVRS_NAVIGATION_API OrientationDualAxisSensorViewDirectionModel : public OrientationModel {
public:
	OrientationDualAxisSensorViewDirectionModel(int xAxisIdx, int yAxisIdx, int buttonIdx, int sensorIdx);
	virtual void getOrientation(ControllerInterface* controller, gmtl::Quatf& result, float dt);

protected:
		int xAxisIdx, yAxisIdx;
		int buttonIdx;
	int sensorIdx;
};

/******************************************************************************
 * @class OrientationSensorModel
 */
class INVRS_NAVIGATION_API OrientationSensorModel : public OrientationModel
{

public:
	OrientationSensorModel(unsigned sensorIdx, float minThreshold, gmtl::Quatf adjust,
			gmtl::Vec3f mirrorAdjust);
	virtual ~OrientationSensorModel() {};
	virtual void getOrientation(ControllerInterface* controller, gmtl::Quatf& result, float dt);

protected:
	unsigned sensorIdx;
	float minThreshold;
	gmtl::Quatf adjust;
	gmtl::Vec3f mirrorAdj;

	friend class OrientationAbsoluteSensorModelFactory;
	friend class DefaultOrientationModelFactory;
};

/******************************************************************************
 * @class OrientationSensorFlyModel
 */
class INVRS_NAVIGATION_API OrientationSensorFlyModel : public OrientationModel
{
public:
	OrientationSensorFlyModel(unsigned sensorIdxOrientation, unsigned buttonIdx, float minThreshold);
	virtual void getOrientation(ControllerInterface* controller, gmtl::Quatf& result, float dt);

protected:
	User* localUser;
	float minThreshold;
	unsigned sensorIdxOrientation;
	unsigned buttonIdx;
};

/******************************************************************************
 * @class DefaultOrientationModelFactory
 */
class DefaultOrientationModelFactory : public OrientationModelFactory
{
public:
	OrientationModel* create(std::string className, ArgumentVector* args = NULL);

protected:
	OrientationModel* createOrientationSensorFlyModel(ArgumentVector* args);
	OrientationModel* createOrientationButtonModel(ArgumentVector* args);
	OrientationModel* createOrientationSingleAxisModel(ArgumentVector* args);
	OrientationModel* createOrientationDualAxisModel(ArgumentVector* args);
	OrientationModel* createOrientationDualAxisSensorViewDirectionModel(ArgumentVector* args);
	OrientationModel* createOrientationSensorModel(ArgumentVector* args);
};

#endif //_ORIENTATIONMODELS_H
