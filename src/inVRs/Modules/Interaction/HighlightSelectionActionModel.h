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

#ifndef _HIGHLIGHTSELECTIONACTIONMODEL_H
#define _HIGHLIGHTSELECTIONACTIONMODEL_H

#include <vector>
#include <gmtl/VecOps.h>

#include "StateActionModel.h"

#include <inVRs/OutputInterface/SceneGraphInterface.h>

/******************************************************************************
 * @class HighlightSelectionActionModel
 * @brief StateActionModel for the Selection-state of the Interaction DFA
 */
class INVRS_INTERACTION_API HighlightSelectionActionModel : public StateActionModel
{
public:
	/**
	 * Constructor initializing models for Highlighting.
	 * The constructor loads the model used for highlighting of the entities
	 * from the passed url.
	 * @param modelType type of the model file
	 * @param modelPath url of the model used for highlighting
	 */
	HighlightSelectionActionModel(std::string modelType, std::string modelPath);

	/**
	 * Empty destructor.
	 */
	virtual ~HighlightSelectionActionModel();

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
	virtual void action(std::vector<PICKEDENTITY>* addedEntities, std::vector<PICKEDENTITY>* removedEntities, float dt);

protected:

	friend class InteractionHighlightSelectionActionEvent;

	struct HighlightData
	{
		unsigned entityId;
//		Entity* entity;
		ModelInterface* model;
	}; // HighlightData

	/**
	 * Displays the loaded Model for Entity highlighting.
	 * @param entity Entity which should be highlighted
	 */
	virtual void highlightEntity(Entity* entity, ModelInterface* model);

	/**
	 * Removes the Model used for highlighting the passed Entity.
	 * @param entity Entity from which the highlighted model should be removed
	 */
	virtual void unhighlightEntity(Entity* entity, ModelInterface* model);

	/**
	 * Updates the animation of the highlighted Models.
	 * @param dt timestep since the last iteration
	 */
	virtual void update(float dt);

	/// Pointer to the SceneGraphInterface
	SceneGraphInterface* sgIF;
	/// FileType of the Model used for highlighting
	std::string modelType;
	/// URL of the Model which is used for highlighting (exclusive path information)
	std::string modelUrl;
	/// List of all Entities which are currently highlighted
	std::vector<HighlightData*> highlightedEntities;
	/// List of all highlight-Models for local selection attached to the scenegraph
	std::vector<ModelInterface*> attachedModels;
	/// Timer variable used for animation
	float time;

}; // HighlightSelectionActionModel

class INVRS_INTERACTION_API HighlightSelectionActionModelFactory : public StateActionModelFactory{
public:
	virtual StateActionModel* create(std::string className, void* args = NULL);
}; // HighlightSelectionActionModelFactory

#endif // _HIGHLIGHTSELECTIONACTIONMODEL_H
