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

#ifndef _SPEEDMODELS_H
#define _SPEEDMODELS_H

#include <irrXML.h>

#include "NavigationSharedLibraryExports.h"
#include <inVRs/InputInterface/ControllerInterface.h>
#include <inVRs/SystemCore/ClassFactory.h>
#include <inVRs/SystemCore/XMLTools.h>

using namespace irr;
using namespace io;

class INVRS_NAVIGATION_API SpeedModel {
public:
	SpeedModel();
	virtual ~SpeedModel() {};

	void setSpeed(float Speed);
	virtual float getSpeed(ControllerInterface* controller, const gmtl::Quatf& rotationChange,
			const gmtl::Vec3f& translationChange, float dt)=0;

protected:
	float speed;
	bool useTimestep;
}; // SpeedModel

// Declare Factory-type for Speed-models
typedef ClassFactory<SpeedModel, ArgumentVector*> SpeedModelFactory;

/******************************************************************************
 * @class SpeedFixedSpeedModel
 *
 * a speed model which always returns the configured speed value
 */
class INVRS_NAVIGATION_API SpeedFixedSpeedModel : public SpeedModel {
public:
	SpeedFixedSpeedModel(bool useTimestep = true);
	virtual float getSpeed(ControllerInterface* controller, const gmtl::Quatf& rotationChange,
			const gmtl::Vec3f& translationChange, float dt);
};

/******************************************************************************
 * @class SpeedAxisModel
 */
class INVRS_NAVIGATION_API SpeedAxisModel : public SpeedModel {
public:
	SpeedAxisModel(unsigned axisIdx, float minThreshold);
	virtual float getSpeed(ControllerInterface* controller, const gmtl::Quatf& rotationChange,
			const gmtl::Vec3f& translationChange, float dt);

protected:
	unsigned axisIdx;
	float minThreshold;
};

/******************************************************************************
 * @class SpeedButtonModel
 */
class INVRS_NAVIGATION_API SpeedButtonModel : public SpeedModel {
public:
	SpeedButtonModel(int accelButtonIdx, int decelButtonIdx);
	virtual float getSpeed(ControllerInterface* controller, const gmtl::Quatf& rotationChange,
			const gmtl::Vec3f& translationChange, float dt);

protected:
	int accelButtonIdx, decelButtonIdx;
};

/******************************************************************************
 * @class SpeedMultiButtonModel
 */
class INVRS_NAVIGATION_API SpeedMultiButtonModel : public SpeedModel {
public:
	SpeedMultiButtonModel(std::string accelButtonString, std::string decelButtonString);
	virtual float getSpeed(ControllerInterface* controller, const gmtl::Quatf& rotationChange,
			const gmtl::Vec3f& translationChange, float dt);

protected:
	std::vector<unsigned> accelButtonIndices;
	std::vector<unsigned> decelButtonIndices;
};

/******************************************************************************
 * @class SpeedDualAxisModel
 */
class INVRS_NAVIGATION_API SpeedDualAxisModel : public SpeedModel {
public:
	SpeedDualAxisModel(unsigned axis1Idx, unsigned axis2Idx);
	virtual float getSpeed(ControllerInterface* controller, const gmtl::Quatf& rotationChange,
			const gmtl::Vec3f& translationChange, float dt);

protected:
	unsigned axis1Idx, axis2Idx;
};

/******************************************************************************
 * @class DefaultSpeedModelFactory
 */
class DefaultSpeedModelFactory : public SpeedModelFactory {
public:
	virtual ~DefaultSpeedModelFactory();
	virtual SpeedModel* create(std::string className, ArgumentVector* args = NULL);

protected:
	SpeedModel* createSpeedFixedSpeedModel(ArgumentVector* args);
	SpeedModel* createSpeedAxisModel(ArgumentVector* args);
	SpeedModel* createSpeedButtonModel(ArgumentVector* args);
	SpeedModel* createSpeedMultiButtonModel(ArgumentVector* args);
	SpeedModel* createSpeedDualAxisModel(ArgumentVector* args);
}; // DefaultSpeedModelFactory

#endif //_SPEEDMODELS_H
