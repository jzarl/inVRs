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

#ifndef _VIRTUALHANDMANIPULATIONMODEL_H
#define _VIRTUALHANDMANIPULATIONMODEL_H

#include "StateActionModel.h"
#include <inVRs/SystemCore/WorldDatabase/Entity.h>

class User;
class TransformationPipe;

/**
 * @class VirtualHandManipulationActionModel
 * @brief StateActionModel for the Manipulation-state of the Interaction DFA
 */
class INVRS_INTERACTION_API VirtualHandManipulationActionModel : public StateActionModel {
public:

	//*****************//
	// PUBLIC METHODS: //
	//*****************//

	/** Empty constructor.
	 */
	VirtualHandManipulationActionModel();

	/** Empty destructor.
	 */
	virtual ~VirtualHandManipulationActionModel();

	//***********************************************//
	// PUBLIC METHODS DERIVED FROM: StateActionModel //
	//***********************************************//

	/** Called when the state is entered.
	 */
	virtual void enter(std::vector<PICKEDENTITY>* entities);

	/** Called when the state is left.
	 */
	virtual void exit();

	/** Called when the DFA is in the state where the Model is registered for.
	 */
	virtual void action(std::vector<PICKEDENTITY>* addedEntities,
			std::vector<PICKEDENTITY>* removedEntities, float dt);

protected:

	friend class InteractionBeginVirtualHandManipulationActionEvent;
	friend class InteractionEndVirtualHandManipulationActionEvent;

	//********************//
	// PROTECTED METHODS: //
	//********************//

	/** Sets the Manipulation-pipe for the Entity with the passed Id.
	 */
	void setManipulationPipe(unsigned entityId, TransformationPipe* pipe);

	/** Clears the Manipulation-pipe for the Entity with the passed Id.
	 */
	void clearManipulationPipe(unsigned entityId);

	//********************//
	// PROTECTED MEMBERS: //
	//********************//

	/// Pointer to the local User
	User* localUser;

	/// List of all Entities which are manipulated
	std::vector<PICKEDENTITY> manipulatingEntities;

	/// Map for TransformationPipes of Entities
	std::map<unsigned, TransformationPipe*> manipulationPipes;

}; // VirtualHandManipulationActionModel

// class VirtualHandManipulationModel : public ManipulationModel {
// protected:
// 	User* localUser;
// public:
// 	VirtualHandManipulationModel();
// 	virtual TransformationData manipulate(Entity* entity, std::vector<BUTTONPRESSED*>* buttonPressed, gmtl::Vec3f position, gmtl::Quatf orientation);
// };

class INVRS_INTERACTION_API VirtualHandManipulationActionModelFactory : public StateActionModelFactory {
public:
	StateActionModel* create(std::string className, void* args = NULL);
}; // VirtualHandManipulationModelFactory

#endif // _VIRTUALHANDMANIPULATIONMODEL_H
