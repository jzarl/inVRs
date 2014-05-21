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

#ifndef _CURSORTRANSFORMATIONMODEL_H
#define _CURSORTRANSFORMATIONMODEL_H

#include <string>

#include "../ClassFactory.h"
#include "../DataTypes.h"

class User;

/******************************************************************************
 * @class CursorTransformationModel
 * @brief The CursorTransformationModel describes the behaviour of the User's
 * Cursor.
 *
 * This class is used to calculate the transformation of the User's
 * cursor. The class has two abstract methods, one to get the name
 * of the model and one to calculate the transformation of the cursor. To
 * implement a cursor transformation it has to be derived from the
 * CursorTransformationModel.
 */
class INVRS_SYSTEMCORE_API CursorTransformationModel {
public:

	/**
	 * Empty destructor.
	 */
	virtual ~CursorTransformationModel() {};

	/**
	 * A transformation matrix is generated based on the current selected
	 * CursorTransformationModel for the certain User.
	 * @param userTransformation the Transformation of the User
	 * @param	user a pointer to the User
	 * @return the transformation matrix of the Cursor in world coordinates
	 */
	virtual TransformationData generateCursorTransformation(TransformationData userTransformation,
			User* user) = 0;

  /**
	 * This method provides a human readable name for the
	 * CursorTransformationModel
	 * @return the name of the model.
	 */
	virtual std::string getName() = 0;
}; // CursorTransformationModel

/**
 * @class CursorTransformationModelFactory
 * @brief The Factory for creating CursorTransformationModels
 *
 * This class is used to create a CursorTransformationModel. Every time you
 * implement a new CursorTransformationModel you also have to implement a new
 * CursorTransformationModelFactory for your model. This new factory can then be
 * registered in the User-class is then used to generate the
 * CursorTransformationModel. The CursorTransformationModelFactory has the
 * abstract create-method which is inherited from the ClassFactory-class. In
 * this method you have to check if the passed string is equal to the name of
 * your CursorTransformationModel. If so you have to return a new instance
 * of your CursorTransformationModel, if not you have to return NULL. This is
 * important since the User-class iterates over every registered
 * CursorTransformationModelFactory and calls their create-method with the name
 * of the desired CursorTransformationModel. If one factory returns a value
 * other than NULL it assumes that this is the new CursorTransformationModel.
 */
typedef ClassFactory<CursorTransformationModel> CursorTransformationModelFactory;

#endif // _CURSORTRANSFORMATIONMODEL_H
