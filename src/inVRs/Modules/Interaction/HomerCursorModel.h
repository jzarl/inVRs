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

#ifndef _HOMERCURSORMODEL_H
#define _HOMERCURSORMODEL_H

#include <inVRs/SystemCore/UserDatabase/CursorTransformationModel.h>
#include "Interaction.h"
#include <inVRs/SystemCore/UserDatabase/User.h>
#include <inVRs/SystemCore/UserDatabase/UserDatabase.h>
#include <inVRs/SystemCore/SystemCore.h>
#include "StateActionModel.h"
#include "RayCastSelectionChangeModel.h"

/**
 * This class implements the Homer-Technique. Use togehter with HomerManipulationActionModel
 * and RayCastSelectionChangeModel
 */
class INVRS_INTERACTION_API HomerCursorModel : public CursorTransformationModel {
public:
	/** The method returns the name of the CursorTransformationModel,
	 * which is also the name of the class: "HomerCursorModel".
	 * This name has to be equal with the name which is checked in
	 * the CursorTransformationModelFactory.
	 * @return name of the model
	 */
	virtual std::string getName();

	/** The method calculates the transformation of the cursor in
	 * world coordinates. The transformation maps the cursor to the
	 * user's hand if nothing is picked. If an entity is picked, it returns
	 * the user's hand transformation + the offset to the entity and
	 * calculates the transformation while animating as well.
	 * If you want it to look nice, you should enable the usePickingOffset - option
	 * in RayCastSeletionChangeModel
	 * @param userTransformation transformation of the User
	 * @param user the user to calculate the cursor-transformation for
	 * @return transformation of the user's cursor
	 */
	virtual TransformationData generateCursorTransformation(TransformationData userTransformation, User* user);

	void setCursorRange(float range);

	/**
	 * starts the animation of moving the cursor to the entity
	 */
	virtual void startForwardAnimation(std::vector<PICKEDENTITY>* entities);

	/**
	 * starts the animation of moving the cursor from the entity
	 * back to the position of the user's hand
	 */
	virtual void startBackwardAnimation();

	/**
	 * @return true if the model is animating the cursor at the moment, false otherwise
	 */
	virtual bool isAnimating();

	virtual bool isForwardAnimating();

	virtual bool isBackwardAnimating();

private:
	friend class HomerCursorModelFactory;

	/**
	 * Constructor
	 */
//	HomerCursorModel();

	/**
	 * TODO not very pretty (use timestamps!)
	 * Constructor
	 * @param an argument vector containg the animationSpeed
	 */
	HomerCursorModel(ArgumentVector* args = NULL);

	/**
	 * moves the cursor from the handTransformation to the positon where
	 * the entity should be picked
	 */
	TransformationData forwardAnimation(TransformationData userTransformation, User* user);

	/**
	 * moves the cursor from the entity back to the handTransformation
	 */
	TransformationData backwardAnimation(TransformationData userTransformation, User* user);

	TransformationData getNotAnimatedCursorTrans(TransformationData userTransformation, User* user);

	Entity* entity;
//	bool reached;

//	User* localUser;
	ArgumentVector* args;

	bool picked;
	bool forwardAnimating;
	bool backwardAnimating;
	double lastTime;

	float dist;
	gmtl::Vec3f direction;
	TransformationData cursorTrans;
//	TransformationData initialCursorOffset;
	TransformationData rayDestTrans;
//	TransformationData userHandTrans;
	RayCastSelectionChangeModel* rayCastSelectionChangeModel;
	float animationSpeed;
	float range;
	gmtl::Vec3f cursorOffset;

	float forwardDistanceThreshold, backwardDistanceThreshold;

}; // HomerCursorModel

/**
 * This class is used to create the HomerCursorModel. The argument vector may
 * contain the animationSpeed (if sepcifed in userConfig)
 */
class INVRS_INTERACTION_API HomerCursorModelFactory : public CursorTransformationModelFactory {
public:
	virtual CursorTransformationModel* create(std::string className, void* args = NULL);
}; // HomerCursorModelFactory

#endif /*_HOMERCURSORMODEL_H*/

