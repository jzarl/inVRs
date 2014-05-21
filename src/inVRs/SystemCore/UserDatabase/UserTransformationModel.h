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

#ifndef USERTRANSFORMATIONMODEL_H_
#define USERTRANSFORMATIONMODEL_H_

#include "../Platform.h"
#include "../DataTypes.h"
#include "../ClassFactory.h"
#include "../ArgumentVector.h"

class User;

/******************************************************************************
 * @class UserTransformationModel
 * @brief The UserTransformationModel calculates the user's transformation
 * relative to the center point of the tracking system.
 *
 * This class is used to calculate the transformation of the user relative to
 * the tracking system's center. The class has two abstract methods, one to get
 * the name of the model and one to calculate the transformation of the user.
 * To implement a model for the user transformation it has to be derived from
 * the UserTransformationModel.
 */
class INVRS_SYSTEMCORE_API UserTransformationModel {
public:

	/**
	 * Empty destructor.
	 */
	virtual ~UserTransformationModel() {};

	/**
	 * A transformation matrix is generated based on the current selected
	 * UserTransformationModel for the certain User.
	 * @param dst target variable
	 * @param user a pointer to the User
	 * @return the transformation of the user relative to the tracking system
	 *         center
	 */
	virtual TransformationData& generateUserTransformation(TransformationData& dst, User* user) = 0;

	/**
	 * This method provides a human readable name for the
	 * UserTransformationModel
	 * @return the name of the model.
	 */
	virtual std::string getName() = 0;

}; // UserTransformationModel

/**
 * @class UserTransformationModelFactory
 * @brief The Factory for creating UserTransformationModels
 *
 * This class is used to create a UserTransformationModel. Every time you
 * implement a new UserTransformationModel you also have to implement a new
 * UserTransformationModelFactory for your model. This new factory can then be
 * registered in the User-class is then used to generate the
 * UserTransformationModel. The UserTransformationModelFactory has the
 * abstract create-method which is inherited from the ClassFactory-class. In
 * this method you have to check if the passed string is equal to the name of
 * your UserTransformationModel. If so you have to return a new instance
 * of your UserTransformationModel, if not you have to return NULL. This is
 * important since the User-class iterates over every registered
 * UserTransformationModelFactory and calls their create-method with the name
 * of the desired UserTransformationModel. If one factory returns a value
 * other than NULL it assumes that this is the new UserTransformationModel.
 */
typedef ClassFactory<UserTransformationModel, ArgumentVector*> UserTransformationModelFactory;
//typedef ClassFactory<UserTransformationModel> UserTransformationModelFactory;

#endif /* USERTRANSFORMATIONMODEL_H_ */
