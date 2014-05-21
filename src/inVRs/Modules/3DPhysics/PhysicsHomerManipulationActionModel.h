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

#ifndef _PHYSICSHOMERMANIPULATIONACTIONMODEL_H
#define _PHYSICSHOMERMANIPULATIONACTIONMODEL_H

#include <inVRs/Modules/Interaction/StateActionModel.h>
#include <inVRs/SystemCore/UserDatabase/CursorTransformationModel.h>
#include <inVRs/SystemCore/WorldDatabase/Entity.h>
#include "PhysicsEntity.h"
#include "3DPhysicsSharedLibraryExports.h"

class User;
class TransformationPipe;

/******************************************************************************
 * @class PhysicsHomerManipulationActionModel
 * @brief This class implements the HOMER-technique.
 *
 * Use together with RayCastSelectionChangeModel and HomerCursorModel
 */
class PhysicsHomerManipulationActionModel : public StateActionModel {
public:
	/**
	 * Empty constructor.
	 */
	PhysicsHomerManipulationActionModel();

	/**
	 * Empty destructor.
	 */
	virtual ~PhysicsHomerManipulationActionModel();

	/**
	 * Called when the state is entered.
	 */
	virtual void enter(std::vector<PICKEDENTITY>* entities);

	/**
	 * Called when the state is left.
	 */
	virtual void exit();

	/**
	 * Called when the DFA is in the state where the Model is registered for.
	 */
	virtual void action(std::vector<PICKEDENTITY>* addedEntities,
			std::vector<PICKEDENTITY>* removedEntities, float dt);

protected:

	/// Pointer to the local User
	User* localUser;

private:
	CursorTransformationModel* cursorModel;
	bool forwardAnimationRunning;

	/// List of all Entities which are manipulated
	std::vector<PhysicsEntity*> physicsEntities;

}; // PhysicsHomerManipulationActionModel


class INVRS_3DPHYSICS_API PhysicsHomerManipulationActionModelFactory : public StateActionModelFactory {
public:
	StateActionModel* create(std::string className, void* args = NULL);
}; // PhysicsHomerManipulationActionModelFactory

#endif /*_PHYSICSHOMERMANIPULATIONACTIONMODEL_H*/
