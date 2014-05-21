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

#ifndef _JOINTINTERACTIONMANIPULATIONMODEL_H
#define _JOINTINTERACTIONMANIPULATIONMODEL_H

#include "../Interaction/StateActionModel.h"
#include <inVRs/SystemCore/WorldDatabase/Entity.h>

#include "JointInteractionSharedLibraryExports.h"

class JointInteraction;
class User;

/** StateActionModel for Manipulation State of Interaction Module.
 * This class is the implementation of the StateActionModel for the
 * Manipulation-state of the Interaction DFA.
 */
class INVRS_JOINTINTERACTION_API JointInteractionManipulationModel : public StateActionModel
{
public:

//*****************//
// PUBLIC METHODS: //
//*****************//

	/** Constructor initializes pointer-members.
	 */
	JointInteractionManipulationModel();

//*************************************************//
// PUBLIC METHODS INHERITED FROM: StateActionModel //
//*************************************************//

	/** Called when the state is entered.
	 */
	virtual void enter(std::vector<PICKEDENTITY>* entities);

	/** Called when the state is left.
	 */
	virtual void exit();

	/** Called when the DFA is in the state where the Model is registered for.
	 */
	virtual void action(std::vector<PICKEDENTITY>* addedEntities, std::vector<PICKEDENTITY>* removedEntities, float dt);

// 	virtual TransformationData manipulate(Entity* entity, std::vector<BUTTONPRESSED*>* buttonPressed, gmtl::Vec3f position, gmtl::Quatf orientation);
// 	virtual void beginManipulation(Entity* entity = NULL, TransformationData offset = identityTransformation());
// 	virtual void endManipulation(Entity* entity = NULL);

protected:

// 	friend class JointInteractionBeginManipulationEvent;
//
// //********************//
// // PROTECTED METHODS: //
// //********************//
//
// 	/** Called by the JointInteractionBeginManipulationEvent to register the
// 	 * Manipulation-Pipe where to write the Manipulations to.
// 	 */
// 	void registerManipulationPipe(TransformationPipe* pipe);

//********************//
// PROTECTED MEMBERS: //
//********************//

	/// Pipe for Entity Manipulation
//	TransformationPipe* manipulationPipe;

	/// Pointer to the JointInteraction class
	JointInteraction* jointInteraction;
	/// Pointer to the local User
	User* localUser;
// 	bool firstTime;

};

#endif // _JOINTINTERACTIONMANIPULATIONMODEL_H
