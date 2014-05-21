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

#ifndef GOGOCURSORMODEL_H_
#define GOGOCURSORMODEL_H_

#include "CursorTransformationModel.h"
#include "User.h"

/******************************************************************************
 * This class implements an non-linear mapping between the user's hand and the 
 * virtual hand (described in "The Go-Go Interaction Technique: Non-linear 
 * Mapping for Direct Manipulation in VR" by Poupyrev, Billinghurst, Weghorst, 
 * Ichikawa).
 */
class INVRS_SYSTEMCORE_API GoGoCursorModel : public CursorTransformationModel {
public:
	/**
	 * The method calculates the transformation of the cursor in world 
	 * coordinates. If the distance between the user and the user's hand exceeds 
	 * the distance threshold, the non-linear mapping is performed.
	 * resultVec = resultVec + distVec/dist * (dist-distanceThreshold)^2 * k
	 * resultVec...result vector
	 * distVec...distance vector betwenn user and user's hand
	 * dist...length of the distance vector
	 * @param userTransformation transformation of the User
	 * @param user the user to calculate the cursor-transformation for
	 * @return transformation of the user's cursor
	 */
	virtual TransformationData generateCursorTransformation(TransformationData userTransformation,
					User* user);

	/**
	 * The method returns the name of the CursorTransformationModel, which is also 
	 * the name of the class: "GoGoCursorModel". This name has to be equal with 
	 * the name which is checked in the CursorTransformationModelFactory.
	 * @return name of the model
	 */
	virtual std::string getName();

private:
	friend class GoGoCursorModelFactory;

	/**
	 * Default Constructor. Default values for the distance threshold are 20, 
	 * headHeigth = 10 and k = 0.5.
	 */
	GoGoCursorModel();

	/**
	 * Constructor
	 * @param an argument vector containing either the distance
	 * Threshold, k, the distance between head and shoulders or all of them.
	 */
	GoGoCursorModel(ArgumentVector* args);

	ArgumentVector* args;
	int distanceThreshold; //threshold for GoGo
	float k; //coefficient for GoGo
	int headHeight;

}; // GoGoCursorModel

/******************************************************************************
 * This class is used to create the GoGoCursorModel.
 */
class INVRS_SYSTEMCORE_API GoGoCursorModelFactory : public CursorTransformationModelFactory {
public:
	/**
	 * The method creates a GoGoCursorModel if the passed className is equal to 
	 * "GoGoCursorModel". If specified in userConfigFile, args contains the 
	 * Distance-Threshold and/or k.
	 * @param args arguments form the userConfigFile containing distanceThreshold 
	 * (default 20), k (default 0,5) and/or the distance between the user's head 
	 * and his shoulders (default 10)
	 * @return a new VirtualHandCursorModel if the passed name matches, NULL 
	 * otherwise
	 */
	virtual CursorTransformationModel* create(std::string className, void* args = NULL);
}; // GoGoCursorModelFactory

#endif /*GOGOCURSORMODEL_H_*/
