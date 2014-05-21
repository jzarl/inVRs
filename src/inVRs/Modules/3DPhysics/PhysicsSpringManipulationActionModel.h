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

#ifndef _PHYSICSSPRINGMANIPULATIONACTIONMODEL_H
#define _PHYSICSSPRINGMANIPULATIONACTIONMODEL_H

#include "PhysicsEntity.h"

#include "../Interaction/StateActionModel.h"

#include <inVRs/SystemCore/UserDatabase/User.h>
#include <inVRs/SystemCore/TransformationManager/TransformationManager.h>

/** ManipulationActionModel for physics based interaction.
 * This ManipulationActionModel can be used for interaction with
 * PhysicsEntities. It also implements natural interaction with
 * non-PhysicsEntities.
 */
class PhysicsSpringManipulationActionModel : public StateActionModel
{
public:

//*****************//
// PUBLIC METHODS: //
//*****************//

	/** Empty constructor.
	 */
	PhysicsSpringManipulationActionModel();

	/** Empty destructor.
	 */
	virtual ~PhysicsSpringManipulationActionModel();

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
	virtual void action(std::vector<PICKEDENTITY>* addedEntities, std::vector<PICKEDENTITY>* removedEntities, float dt);

protected:

	friend class PhysicsBeginSpringManipulationActionEvent;
	friend class PhysicsEndSpringManipulationActionEvent;

//********************//
// PROTECTED METHODS: //
//********************//

	/** DEPRECATED: PIPE FOR PHYSICSENTITY IS MANAGED BY PHYSICS MODULE
	 * Sets the Manipulation-pipe for the PhysicsEntity with the passed Id.
	 */
//	void setPhysicsEntityPipe(unsigned entityId, TransformationPipe* pipe);

	/** DEPRECATED: PIPE FOR PHYSICSENTITY IS MANAGED BY PHYSICS MODULE
	 * Clears the Manipulation-pipe for the PhysicsEntity with the passed Id.
	 */
//	void clearPhysicsEntityPipe(unsigned entityId);

	/** Sets the Manipulation-pipe for the non-physics Entity with the passed Id.
	 */
	void setNonPhysicsEntityPipe(unsigned entityId, TransformationPipe* pipe);

	/** Clears the Manipulation-pipe for the non-physics Entity with the passed Id.
	 */
	void clearNonPhysicsEntityPipe(unsigned entityId);

//********************//
// PROTECTED MEMBERS: //
//********************//

	/// Pointer to the local User
	User* localUser;

	/// List of all Entities which are manipulated
	std::vector<PhysicsEntity*> physicsEntities;
	/// List of all Entities which are not PhysicsEntities
	std::vector<Entity*> nonPhysicsEntities;

	/// DEPRECATED: PIPE FOR PHYSICSENTITY IS MANAGED BY PHYSICS MODULE
	/// Map for TransformationPipes of Physics Entities
//	std::map<unsigned, TransformationPipe*> physicsEntityPipes;
	/// Map for TransformationPipes of non-physics Entities
	std::map<unsigned, TransformationPipe*> nonPhysicsEntityPipes;

}; // PhysicsSpringManipulationActionModel

class PhysicsSpringManipulationActionModelFactory : public StateActionModelFactory
{
public:

//*****************//
// PUBLIC METHODS: //
//*****************//

	StateActionModel* create(std::string className, void* args = NULL);
}; // PhysicsSpringManipulationModelFactory

#endif // _PHYSICSSPRINGMANIPULATIONACTIONMODEL_H
