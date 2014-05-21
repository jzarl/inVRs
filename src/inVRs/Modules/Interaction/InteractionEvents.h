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

#ifndef _INTERACTIONEVENTS_H
#define _INTERACTIONEVENTS_H

#include <inVRs/SystemCore/EventManager/Event.h>
#include <inVRs/SystemCore/EventManager/EventFactory.h>
#include <inVRs/SystemCore/WorldDatabase/Environment.h>
#include "SelectionChangeModel.h"
#include "StateActionModel.h"

class Interaction;
class HighlightSelectionActionModel;

//class InteractionEventsFactory : public EventFactory
//{
//public:
//	virtual Event* create(unsigned eventId);
//
//};

class INVRS_INTERACTION_API InteractionChangeStateEvent : public Event {
public:
	typedef EventFactory<InteractionChangeStateEvent> Factory;

	InteractionChangeStateEvent(std::vector<PICKEDENTITY>* pickedEntitiesList,
			unsigned previousState, unsigned nextState, StateActionModel* previousStateActionModel,
			StateActionModel* nextStateActionModel);
	InteractionChangeStateEvent(); // constructor for Event::decode()
	virtual ~InteractionChangeStateEvent();

	virtual void encode(NetMessage* message);
	virtual void decode(NetMessage* message);
	virtual void execute();

protected:

	unsigned previousState;
	unsigned nextState;
	StateActionModel* previousStateActionModel; // not going to be encoded
	StateActionModel* nextStateActionModel;
	std::vector<PICKEDENTITY> pickedEntitiesCopy;

};

class INVRS_INTERACTION_API InteractionChangeStateToFromManipulationEvent : public InteractionChangeStateEvent {
public:
	typedef EventFactory<InteractionChangeStateToFromManipulationEvent> Factory;

	InteractionChangeStateToFromManipulationEvent(std::vector<PICKEDENTITY>* pickedEntitiesList,
			bool toManipulation, StateActionModel* previousStateActionModel,
			StateActionModel* nextStateActionModel);
	InteractionChangeStateToFromManipulationEvent();
	virtual ~InteractionChangeStateToFromManipulationEvent();

	virtual void encode(NetMessage* message);
	virtual void decode(NetMessage* message);
	virtual void execute();

protected:

	unsigned toManipulation;

};

/******************************************************************************
 * @class InteractionHighlightSelectionActionEvent
 *  
 * Event for displaying a model for highlighting of all Entities in the
 * highlightEntities-list and for revoving the models for Entities in the
 * unhighlightEntities-list.
 */
class INVRS_INTERACTION_API InteractionHighlightSelectionActionEvent : public Event {
public:
	typedef EventFactory<InteractionHighlightSelectionActionEvent> Factory;

	/** 
	 * Empty constructor.
	 */
	InteractionHighlightSelectionActionEvent();

	/** 
	 * Constructor
	 */
	InteractionHighlightSelectionActionEvent(std::vector<PICKEDENTITY>* highlightEntities,
			std::vector<PICKEDENTITY>* unhighlightEntities, std::string modelType,
			std::string modelUrl);
	/** 
	 * Empty destructor.
	 */
	virtual ~InteractionHighlightSelectionActionEvent();

	virtual void encode(NetMessage* message);
	virtual void decode(NetMessage* message);
	virtual void execute();

protected:
	/** 
	 * Displays a clone of the passed model for highlighting the passed Entity.
	 */
	void highlightEntity(Entity* entity, SceneGraphInterface* sgIF, ModelInterface* model,
			HighlightSelectionActionModel* selectionModel);

	void highlightEntity(unsigned typeBasedId, SceneGraphInterface* sgIF, ModelInterface* model,
			HighlightSelectionActionModel* selectionModel);

	/** 
	 * Removes the highlight-model for the passed Entity if existing.
	 */
	void unhighlightEntity(Entity* entity, SceneGraphInterface* sgIF,
			HighlightSelectionActionModel* selectionModel);

	/// ID of the user sending this event
	unsigned userId;
	/// File-type of the Model that should be displayed
	std::string modelType;
	/// URL to the Model that should be displayed
	std::string modelUrl;
	/// List of all Entities which should be highlighted
	std::vector<unsigned> highlightEntityIds;
	/// List of all Entities which should be unhighlighted
	std::vector<unsigned> unhighlightEntityIds;
}; // InteractionHighlightSelectionActionEvent

/******************************************************************************
 * @class InteractionVirtualHandManipulationActionEvent
 *  
 * Event for starting and stopping the VirtualHandManipulation for a number of
 * entities.
 */
class INVRS_INTERACTION_API InteractionVirtualHandManipulationActionEvent : public Event {
public:
	typedef EventFactory<InteractionVirtualHandManipulationActionEvent> Factory;
  /** 
   * Empty constructor.
	 */
	InteractionVirtualHandManipulationActionEvent();

	/** 
	 * Constructor ... .
	 */
	InteractionVirtualHandManipulationActionEvent(std::vector<PICKEDENTITY>* manipulatingEntities,
			bool beginManipulation);

	/** 
	 * Empty destructor.
	 */
	virtual ~InteractionVirtualHandManipulationActionEvent();

	virtual void encode(NetMessage* message);
	virtual void decode(NetMessage* message);
	virtual void execute() = 0;

protected:
	/// boolean value indicating if the manipulation should be started or ended
	bool beginManipulation;
	/// ID of the user sending this event
	unsigned userId;
	/// List of all Entities which should be highlighted
	std::vector<unsigned> manipulatingEntityIds;
}; // InteractionVirtualHandManipulationActionEvent

/******************************************************************************
 * @class InteractionBeginVirtualHandManipulationActionEvent
 * Event for starting VirtualHandManipulation.
 */
class INVRS_INTERACTION_API InteractionBeginVirtualHandManipulationActionEvent :
		public InteractionVirtualHandManipulationActionEvent {
public:
	typedef EventFactory<InteractionBeginVirtualHandManipulationActionEvent> Factory;

	InteractionBeginVirtualHandManipulationActionEvent();

	/** 
	 * Dummy constructor calling parent constructor.
	 */
	InteractionBeginVirtualHandManipulationActionEvent(
			std::vector<PICKEDENTITY>* manipulatingEntities);

	/** 
	 * Empty destructor.
	 */
	virtual ~InteractionBeginVirtualHandManipulationActionEvent();

	virtual void execute();

}; // InteractionBeginVirtualHandManipulationActionEvent

/******************************************************************************
 * @class InteractionEndVirtualHandManipulationActionEvent
 *  
 * Event for stopping VirtualHandManipulation.
 */
class INVRS_INTERACTION_API InteractionEndVirtualHandManipulationActionEvent :
		public InteractionVirtualHandManipulationActionEvent {
public:
	typedef EventFactory<InteractionEndVirtualHandManipulationActionEvent> Factory;

	InteractionEndVirtualHandManipulationActionEvent();

	/** 
	 * Dummy constructor calling parent constructor.
	 */
	InteractionEndVirtualHandManipulationActionEvent(
			std::vector<PICKEDENTITY>* manipulatingEntities);

	/** 
	 * Empty destructor.
	 */
	virtual ~InteractionEndVirtualHandManipulationActionEvent();

	virtual void execute();

}; // InteractionEndVirtualHandManipulationActionEvent


//-------------------InteractionEvents for HomerManipulationAction--------------

/******************************************************************************
 * @class InteractionHomerManipulationActionEvent
 *  
 * Event for starting and stopping the HomerManipulation
 */
class INVRS_INTERACTION_API InteractionHomerManipulationActionEvent : public Event {
public:
	typedef EventFactory<InteractionHomerManipulationActionEvent> Factory;

	/** 
	 * Empty constructor.
	 */
	InteractionHomerManipulationActionEvent();

	/** 
	 * Constructor ... .
	 */
	InteractionHomerManipulationActionEvent(std::vector<PICKEDENTITY>* manipulatingEntities,
			bool beginManipulation);

	/** 
	 * Empty destructor.
	 */
	virtual ~InteractionHomerManipulationActionEvent();

	virtual void encode(NetMessage* message);
	virtual void decode(NetMessage* message);
	virtual void execute() = 0;

protected:
	/// boolean value indicating if the manipulation should be started or ended
	bool beginManipulation;
	/// ID of the user sending this event
	unsigned userId;
	/// List of all Entities which should be highlighted
	std::vector<unsigned> manipulatingEntityIds;
}; //InteractionHomerManipulationActionEvent

/******************************************************************************
 * @class InteractionBeginHomerManipulationActionEvent
 *  
 * Event for starting HomerManipulation.
 */
class INVRS_INTERACTION_API InteractionBeginHomerManipulationActionEvent : public InteractionHomerManipulationActionEvent {
public:
	typedef EventFactory<InteractionBeginHomerManipulationActionEvent> Factory;

	InteractionBeginHomerManipulationActionEvent();

	/** 
	 * Dummy constructor calling parent constructor.
	 */
	InteractionBeginHomerManipulationActionEvent(std::vector<PICKEDENTITY>* manipulatingEntities);

	/** 
	 * Empty destructor.
	 */
	virtual ~InteractionBeginHomerManipulationActionEvent();

	virtual void execute();
}; //InteractionBeginHomerManipulationActionEvent

/******************************************************************************
 * @class InteractionEndHomerManipulationActionEvent
 *  
 * Event for stopping HomerManipulation.
 */
class INVRS_INTERACTION_API InteractionEndHomerManipulationActionEvent : public InteractionHomerManipulationActionEvent {
public:
	typedef EventFactory<InteractionEndHomerManipulationActionEvent> Factory;

	InteractionEndHomerManipulationActionEvent();

	/** 
	 * Dummy constructor calling parent constructor.
	 */
	InteractionEndHomerManipulationActionEvent(std::vector<PICKEDENTITY>* manipulatingEntities);

	/** 
	 * Empty destructor.
	 */
	virtual ~InteractionEndHomerManipulationActionEvent();

	virtual void execute();

}; //InteractionEndHomerManipulationActionEvent

#endif
