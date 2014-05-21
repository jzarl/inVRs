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

#ifndef _VIRTUALHANDCURSORMODEL_H
#define _VIRTUALHANDCURSORMODEL_H

#include "CursorTransformationModel.h"

/******************************************************************************
 * @class VirtualHandCursorModel
 * @brief The cursor transformation for virtual hand interaction is calculated
 *  
 * This class calculates the transformation of the User's cursor by taking the 
 * value of the hand-transformation stored in the User-class.
 */
class INVRS_SYSTEMCORE_API VirtualHandCursorModel : public CursorTransformationModel {
public:
	/**
	 * The method returns the name of the CursorTransformationModel, which 
	 * identical to the name of the class: "VirtualHandCursorModel". This name has 
	 * to be equal with the name which is checked in the 
	 * CursorTransformationModelFactory.
	 * @return name of the model
	 */
	virtual std::string getName();

	/**
	 * The method calculates the transformation of the cursor in world 
	 * coordinates. It therefore gets the hand-transformation from the User 
	 * (which is equal to the hand sensor if available) and adds it to the passed 
	 * user-transformation, which is the transformation of the User's feet in 
	 * world coordinates.
	 * @param userTransformation the transformation of the User
	 * @param user the user to calculate the cursor-transformation for
	 * @return the transformation of the user's cursor
	 */
	virtual TransformationData generateCursorTransformation(TransformationData userTransformation,
			User* user);

private:
	friend class VirtualHandCursorModelFactory;

	VirtualHandCursorModel();

}; // VirtualHandCursorModel

/******************************************************************************
 * @class CursorTransformationModelFactory
 * @brief A standard factory for generating the appropriate model
 *  
 * This class is used to create the VirtualHandCursorModel.
 */
class INVRS_SYSTEMCORE_API VirtualHandCursorModelFactory : public CursorTransformationModelFactory {
public:
	/**
	 * The method creates a VirtualHandCursorModel if the passed className is 
	 * equal to "VirtualHandCursorModel".
	 * @param className the attribute should be "VirtualHandCursorModel"	 
	 * @return a new VirtualHandCursorModel if the passed name matches, NULL otherwise
	 */
	virtual CursorTransformationModel* create(std::string className, void* args = NULL);
}; // VirtualHandCursorModelFactory

#endif // _VIRTUALHANDCURSORMODEL_H
