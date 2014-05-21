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

#include "HighlightSelectionActionModel.h"

#include <assert.h>

#include "InteractionEvents.h"
#include <inVRs/SystemCore/Timer.h>
#include <inVRs/SystemCore/DebugOutput.h>
#include <inVRs/SystemCore/Configuration.h>
#include <inVRs/SystemCore/ArgumentVector.h>
#include <inVRs/SystemCore/WorldDatabase/WorldDatabase.h>
#include <inVRs/SystemCore/WorldDatabase/Entity.h>
#include <inVRs/SystemCore/SystemCore.h>
#include <inVRs/SystemCore/EventManager/EventManager.h>
#include <inVRs/OutputInterface/SceneGraphInterface.h>
#include <inVRs/OutputInterface/OutputInterface.h>

HighlightSelectionActionModel::HighlightSelectionActionModel(std::string modelType,
		std::string modelPath) {
	// 	std::string modelDir = Configuration::getPath("Highlighters");
	// 	modelPath = modelDir + modelPath;

	// 	printd("HighlightSelectionActionModel::HighlightSelectionActionModel(): i would take model %s\n", modelPath.c_str());

	sgIF = OutputInterface::getSceneGraphInterface();
	assert(sgIF);
	this->modelType = modelType;
	this->modelUrl = modelPath;
	time = 0;
} // HighlightSelectionActionModel

HighlightSelectionActionModel::~HighlightSelectionActionModel() {

} // ~HighlightSelectionActionModel

void HighlightSelectionActionModel::enter(std::vector<PICKEDENTITY>* entities) {
	//	int i;

	InteractionHighlightSelectionActionEvent* event = new InteractionHighlightSelectionActionEvent(
			entities, NULL, modelType, modelUrl);
	EventManager::sendEvent(event, EventManager::EXECUTE_GLOBAL);

	time = 0;
} // enter

void HighlightSelectionActionModel::exit() {
	int i;
	PICKEDENTITY pickedEntity;
	std::vector<PICKEDENTITY> remainingEntities;

	for (i = 0; i < (int)highlightedEntities.size(); i++) {
		//		pickedEntity.entity = highlightedEntities[i]->entity;
		pickedEntity.entity = WorldDatabase::getEntityWithTypeInstanceId(
				highlightedEntities[i]->entityId);
		pickedEntity.pickingOffset = identityTransformation();
		if (pickedEntity.entity)
			remainingEntities.push_back(pickedEntity);
	}

	InteractionHighlightSelectionActionEvent* event = new InteractionHighlightSelectionActionEvent(
			NULL, &remainingEntities, modelType, modelUrl);
	EventManager::sendEvent(event, EventManager::EXECUTE_GLOBAL);
} // exit

void HighlightSelectionActionModel::action(std::vector<PICKEDENTITY>* addedEntities, std::vector<
		PICKEDENTITY>* removedEntities, float dt) {
	if ((addedEntities && addedEntities->size() > 0) || (removedEntities && removedEntities->size()
			> 0)) {
		InteractionHighlightSelectionActionEvent* event =
				new InteractionHighlightSelectionActionEvent(addedEntities, removedEntities,
						modelType, modelUrl);
		EventManager::sendEvent(event, EventManager::EXECUTE_GLOBAL);
	} // if

	update(dt);
} // action

void HighlightSelectionActionModel::highlightEntity(Entity* entity, ModelInterface* model) {
	HighlightData* data = new HighlightData;
	//	data->entity = entity;
	data->entityId = entity->getTypeBasedId();
	data->model = model;
	highlightedEntities.push_back(data);
} // highlightEntity

void HighlightSelectionActionModel::unhighlightEntity(Entity* entity, ModelInterface* model) {
	unsigned i;

	for (i = 0; i < highlightedEntities.size(); i++) {
		//		if (entity == highlightedEntities[i]->entity)
		if (entity->getTypeBasedId() == highlightedEntities[i]->entityId)
			break;
	} // for

	if (i == highlightedEntities.size()) {
		printd(WARNING,
				"HighlightSelectionActionModel::unhighlightEntity(): Entity to unhighlight not found in List!\n");
		return;
	} // if

	delete highlightedEntities[i];
	highlightedEntities.erase(highlightedEntities.begin() + i);
} // unHighlightEntity

void HighlightSelectionActionModel::update(float dt) {
	int i;
	ModelInterface* entityModel;
	ModelInterface* entityHighlightModel;
	TransformationData invEntity, boxFinal;
	TransformationData pulsationTransform;
	Entity* entity;

	time += dt;
	float scale = 1 + (sin(5 * time) + 1) / 20.f;

	assert(sgIF);

	for (i = 0; i < (int)highlightedEntities.size(); i++) {
		entityHighlightModel = highlightedEntities[i]->model;
		entity = WorldDatabase::getEntityWithTypeInstanceId(highlightedEntities[i]->entityId);
		if (!entity) {
			printd(
					ERROR,
					"HighlightSelectionActionModel::update(): Could not find Entity for highlighting! removing Entity!\n");
			highlightedEntities.erase(highlightedEntities.begin() + i);
			i--;
			continue;
		} // if

		//		entityModel = highlightedEntities[i]->entity->getVisualRepresentation();
		entityModel = entity->getVisualRepresentation();
		AABB bbox = entityModel->getAABB();
		bbox.p1 += gmtl::Vec3f(0.01, 0.01, 0.01);
		bbox.p0 -= gmtl::Vec3f(0.01, 0.01, 0.01);

		pulsationTransform = identityTransformation();
		pulsationTransform.scale = bbox.p1 - bbox.p0;
		pulsationTransform.position = (bbox.p1 + bbox.p0) * 0.5f;
		pulsationTransform.scale *= scale;

		//		invEntity = highlightedEntities[i]->entity->getWorldTransformation();
		invEntity = entity->getWorldTransformation();
		invert(invEntity);
		multiply(boxFinal, invEntity, pulsationTransform);

		sgIF->updateModel(entityHighlightModel, boxFinal);
	} // for
} // update

StateActionModel* HighlightSelectionActionModelFactory::create(std::string className, void* args) {
	if (className != "HighlightSelectionActionModel")
		return NULL;

	std::string modelType;
	std::string modelPath;

	if (!args || !((ArgumentVector*)args)->get("modelType", modelType)
			|| !((ArgumentVector*)args)->get("modelPath", modelPath)) {
		printd(
				ERROR,
				"HighlightSelectionActionModelFactory::create(): Missing entry in config file: Expected argument \"modelType\" or \"modelPath\"!\n");
		return NULL;
	} // if

	return new HighlightSelectionActionModel(modelType, modelPath);
} // create
