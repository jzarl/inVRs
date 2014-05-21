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

#ifndef _INTERACTION_H
#define _INTERACTION_H

#include <vector>

#include "HighlightSelectionActionModel.h"
#include "VirtualHandManipulationActionModel.h"
#include "HomerManipulationActionModel.h"
#include "SelectionChangeModel.h"
#include "ManipulationChangeModel.h"
#include "StateActionModel.h"
#include "InteractionEvents.h"
#include "SharedManipulationMerger.h"
#include "InteractionSharedLibraryExports.h"

#include <inVRs/SystemCore/ComponentInterfaces/InteractionInterface.h>
#include <inVRs/SystemCore/SyncPipe.h>
#include <inVRs/SystemCore/TransformationManager/TransformationManager.h>
#include <inVRs/SystemCore/TransformationManager/TransformationPipe.h>
#include <inVRs/SystemCore/XmlConfigurationLoader.h>

/******************************************************************************
 * @class Interaction
 * @brief This is the main class for Interaction module
 */
class INVRS_INTERACTION_API Interaction : public InteractionInterface {
public:

	Interaction();
	~Interaction();

	virtual bool loadConfig(std::string configFile);
	virtual void cleanup();

	virtual std::string getName();
	virtual void destroyEntityNotification(Entity* entity);

	virtual void update(float dt);

	INTERACTIONSTATE getState();

	static void registerSelectionChangeModelFactory(
			SelectionChangeModelFactory* newSelectionChangeModelFactory);
	static void registerManipulationChangeModelFactory(
			ManipulationChangeModelFactory* newManipulationChangeModelFactory);
	static void
			registerStateActionModelFactory(StateActionModelFactory* newStateActionModelFactory);

	StateActionModel* getSelectionActionModel();
	StateActionModel* getManipulationActionModel();
	SelectionChangeModel* getSelectionChangeModel();
	ManipulationChangeModel* getManipulationTerminationModel();

protected:
	friend class InteractionChangeStateEvent;

	bool setSelectionChangeModel(std::string newSelectionChangeModel, ArgumentVector* argVec);
	bool setUnselectionChangeModel(std::string newUnselectionChangeModel, ArgumentVector* argVec);
	bool setManipulationConfirmationModel(std::string newManipulationConfirmationModel,
			ArgumentVector* argVec);
	bool setManipulationTerminationModel(std::string newManipulationTerminationModel,
			ArgumentVector* argVec);

	bool setIdleActionModel(std::string newIdleActionModel, ArgumentVector* argVec);
	bool setSelectionActionModel(std::string newSelectionActionModel, ArgumentVector* argVec);
	bool setManipulationActionModel(std::string newManipulationActionModel, ArgumentVector* argVec);

	// some helpers:
	void addToList(std::vector<PICKEDENTITY>* dst, std::vector<PICKEDENTITY>* src);
	void removeFromList(std::vector<PICKEDENTITY>* dst, std::vector<PICKEDENTITY>* removeMe);

	static std::vector<SelectionChangeModelFactory*> selectionChangeModelFactoryList;
	static std::vector<ManipulationChangeModelFactory*> manipulationChangeModelFactoryList;
	static std::vector<StateActionModelFactory*> actionModelFactoryList;

	EventPipe* incomingEvents;
	SelectionChangeModel* selectionChangeModel;
	SelectionChangeModel* unselectionChangeModel;
	ManipulationChangeModel* manipulationConfirmationModel;
	ManipulationChangeModel* manipulationTerminationModel;
	StateActionModel* idleActionModel;
	StateActionModel* selectionActionModel;
	StateActionModel* manipulationActionModel;
	SharedManipulationMergerFactory* sharedManipulationMergerFactory;
	INTERACTIONSTATE state;
	std::vector<PICKEDENTITY> selectedEntitiesList;
	std::vector<PICKEDENTITY> entitiesToSelect;

//*****************************************************************************
// Configuration loading
//*****************************************************************************
private:
	static XmlConfigurationLoader xmlConfigLoader;

	class ConverterToV1_0a4 : public XmlConfigurationConverter {
	public:
		ConverterToV1_0a4();
		virtual bool convert(XmlDocument* document, const Version& version, std::string configFile);
	}; // ConverterToV1_0a4

}; // Interaction

#endif
