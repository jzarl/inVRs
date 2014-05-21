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

#ifndef HEADPOSITIONUSERTRANSFORMATIONMODEL_H_
#define HEADPOSITIONUSERTRANSFORMATIONMODEL_H_

#include "UserTransformationModel.h"

/******************************************************************************
 * This class calculates the user's transformation based on the head sensor.
 * It therefore takes the position of the head-sensor as position information
 * for the user. The y-axis value is set to zero to remove the height of the
 * head sensor. For the orientation the model returns zero rotation assuming
 * that the user is oriented in direction of the screen.
 * This model is useful in environments with tracking systems where no sensor
 * is available for the user's transformation and the user's avatar is able to
 * turn its head. Thus the avatar is always oriented as if the user was looking
 * forwards and the avatar's head can be updated according to the head-sensor
 * value.
 */
class INVRS_SYSTEMCORE_API HeadPositionUserTransformationModel : public UserTransformationModel {
public:

	/**
	 * Empty constructor.
	 */
	HeadPositionUserTransformationModel() {};

	/**
	 * Empty destructor.
	 */
	virtual ~HeadPositionUserTransformationModel() {};

	/**
	 * Generates the user's transformation:
	 * position = position of user's head sensor - sensor's height value
	 * orientation = identity (no rotation)
	 * @param dst target variable
	 * @param user a pointer to the User
	 * @return the transformation of the user relative to the tracking system
	 *         center
	 */
	virtual TransformationData& generateUserTransformation(TransformationData& dst, User* user);

	/**
	 * This method provides a human readable name for the
	 * UserTransformationModel
	 * @return the class name (HeadPositionUserTransformationModel)
	 */
	virtual std::string getName();
};

/******************************************************************************
 * This class is used to create the HeadPositionUserTransformationModel.
 */
class INVRS_SYSTEMCORE_API HeadPositionUserTransformationModelFactory : public UserTransformationModelFactory {
public:
	/**
	 * The method creates a HeadPositionUserTransformationModel if the passed
	 * className is equal to "HeadPositionUserTransformationModel".
	 * @param args arguments for the model (none available in this model)
	 * @return a new HeadPositionUserTransformationModel if the passed name
	 *         matches, NULL otherwise
	 */
	virtual UserTransformationModel* create(std::string className, ArgumentVector* args = NULL);
}; // HeadPositionUserTransformationModelFactory

#endif /* HEADPOSITIONUSERTRANSFORMATIONMODEL_H_ */
