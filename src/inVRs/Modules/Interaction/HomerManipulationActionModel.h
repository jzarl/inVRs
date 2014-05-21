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

#ifndef _HOMERMANIPULATIONACTIONMODEL_H
#define _HOMERMANIPULATIONACTIONMODEL_H

#include "StateActionModel.h"
#include <inVRs/SystemCore/UserDatabase/CursorTransformationModel.h>
#include <inVRs/SystemCore/WorldDatabase/Entity.h>

class User;
class TransformationPipe;

/******************************************************************************
 * @class HomerManipulationActionModel
 * @brief This class implements the HOMER-technique.
 *  
 * Use together with RayCastSelectionChangeModel and HomerCursorModel
 */
class INVRS_INTERACTION_API HomerManipulationActionModel : public StateActionModel {
public:
	/** 
	 * Empty constructor.
	 */
	HomerManipulationActionModel();

	/** 
	 * Empty destructor.
	 */
	virtual ~HomerManipulationActionModel();

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

	friend class InteractionBeginHomerManipulationActionEvent;
	friend class InteractionEndHomerManipulationActionEvent;

	/** 
	 * Sets the Manipulation-pipe for the Entity with the passed Id.
	 */
	void setManipulationPipe(unsigned entityId, TransformationPipe* pipe);

	/** 
	 * Clears the Manipulation-pipe for the Entity with the passed Id.
	 */
	void clearManipulationPipe(unsigned entityId);

	/// Pointer to the local User
	User* localUser;
	/// List of all Entities which are manipulated
	std::vector<PICKEDENTITY> manipulatingEntities;
	/// Map for TransformationPipes of Entities
	std::map<unsigned, TransformationPipe*> manipulationPipes;

private:
	CursorTransformationModel* cursorModel;

}; // HomerManipulationActionModel


class INVRS_INTERACTION_API HomerManipulationActionModelFactory : public StateActionModelFactory {
public:
	StateActionModel* create(std::string className, void* args = NULL);
}; // HomerManipulationActionModelFactory

#endif /*HOMERMANIPULATIONACTIONMODEL_H_*/
