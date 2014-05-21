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

#include "InteractionEvents.h"

#include <assert.h>

#include "Interaction.h"
#include "HighlightSelectionActionModel.h"
#include "VirtualHandManipulationActionModel.h"
#include "HomerManipulationActionModel.h"
#include <inVRs/SystemCore/WorldDatabase/WorldDatabase.h>
#include <inVRs/SystemCore/SystemCore.h>
#include <inVRs/SystemCore/MessageFunctions.h>
#include <inVRs/SystemCore/UtilityFunctions.h>
#include <inVRs/SystemCore/UserDatabase/UserDatabase.h>
#include <inVRs/OutputInterface/OutputInterface.h>


//Event* InteractionEventsFactory::create(unsigned eventId)
//{
//	switch(eventId)
//	{
//		case InteractionChangeStateType:
//			return new InteractionChangeStateEvent();
//
//		case InteractionChangeStateToFromManipulationType:
//			return new InteractionChangeStateToFromManipulationEvent();
//
//		case InteractionHighlightSelectionActionType:
//			return new InteractionHighlightSelectionActionEvent();
//
//		case InteractionBeginVirtualHandManipulationActionType:
//			return new InteractionBeginVirtualHandManipulationActionEvent();
//
//		case InteractionEndVirtualHandManipulationActionType:
//			return new InteractionEndVirtualHandManipulationActionEvent();
//
//// 		case InteractionVirtualHandManipulationActionType:
//// 			return new InteractionVirtualHandManipulationActionEvent;
//	}
//
//	return NULL;
//}

InteractionChangeStateEvent::InteractionChangeStateEvent(
		std::vector<PICKEDENTITY>* pickedEntitiesList, unsigned previousState, unsigned nextState,
		StateActionModel* previousStateActionModel, StateActionModel* nextStateActionModel) :
	Event(INTERACTION_MODULE_ID, INTERACTION_MODULE_ID, "InteractionChangeStateEvent") {
	int i;

	this->previousState = previousState;
	this->nextState = nextState;
	this->previousStateActionModel = previousStateActionModel;
	this->nextStateActionModel = nextStateActionModel;

	for (i = 0; i < (int)pickedEntitiesList->size(); i++) {
		pickedEntitiesCopy.push_back((*pickedEntitiesList)[i]);
	}
}

InteractionChangeStateEvent::InteractionChangeStateEvent() {
	previousStateActionModel = NULL;
	nextStateActionModel = NULL;
}

InteractionChangeStateEvent::~InteractionChangeStateEvent() {

}

void InteractionChangeStateEvent::encode(NetMessage* message) {
	message->putUInt32(previousState);
	message->putUInt32(nextState);
}

void InteractionChangeStateEvent::decode(NetMessage* message) {
	message->getUInt32(previousState);
	message->getUInt32(nextState);
}

void InteractionChangeStateEvent::execute() {
	Interaction* intIntf;
	User* localUser = UserDatabase::getLocalUser();

	if (localUser->getId() == getUserId()) {
		intIntf = (Interaction*)SystemCore::getModuleByName("Interaction");
		assert(intIntf);

		if (previousStateActionModel)
			previousStateActionModel->exit();
		intIntf->state = (Interaction::INTERACTIONSTATE)nextState;
		if (nextStateActionModel)
			nextStateActionModel->enter(&pickedEntitiesCopy);
	}

}

InteractionChangeStateToFromManipulationEvent::InteractionChangeStateToFromManipulationEvent(
		std::vector<PICKEDENTITY>* pickedEntitiesList, bool toManipulation,
		StateActionModel* previousStateActionModel, StateActionModel* nextStateActionModel) :
	InteractionChangeStateEvent(pickedEntitiesList,
			toManipulation ? Interaction::STATE_SELECTION : Interaction::STATE_MANIPULATION,
			toManipulation ? Interaction::STATE_MANIPULATION : Interaction::STATE_IDLE,
			previousStateActionModel, nextStateActionModel) {
	setEventName("InteractionChangeStateToFromManipulationEvent");

	this->toManipulation = toManipulation ? 1 : 0;
}

InteractionChangeStateToFromManipulationEvent::InteractionChangeStateToFromManipulationEvent() :
	InteractionChangeStateEvent() {

}

InteractionChangeStateToFromManipulationEvent::~InteractionChangeStateToFromManipulationEvent() {

}

void InteractionChangeStateToFromManipulationEvent::encode(NetMessage* message) {
	int i;

	InteractionChangeStateEvent::encode(message);

	message->putUInt32(toManipulation);
	message->putUInt32(pickedEntitiesCopy.size());

	for (i = 0; i < (int)pickedEntitiesCopy.size(); i++) {
		message->putUInt32(pickedEntitiesCopy[i].entity->getTypeBasedId());
		addTransformationToBinaryMsg(&pickedEntitiesCopy[i].pickingOffset, message);
	}
}

void InteractionChangeStateToFromManipulationEvent::decode(NetMessage* message) {
	int i;
	unsigned maxi;
	PICKEDENTITY pickedEntity;
	unsigned typeInstId;

	InteractionChangeStateEvent::decode(message);

	message->getUInt32(toManipulation);
	message->getUInt32(maxi);

	for (i = 0; i < (int)maxi; i++) {
		message->getUInt32(typeInstId);
		pickedEntity.entity = WorldDatabase::getEntityWithTypeInstanceId(typeInstId);
		if (pickedEntity.entity == NULL) {
			printd(
					ERROR,
					"InteractionChangeStateToManipulationEvent::decode(): couldn't find entity with type instance id (%0.8X)\n",
					typeInstId);
			continue;
		}
		pickedEntity.pickingOffset = readTransformationFrom(message);
		pickedEntitiesCopy.push_back(pickedEntity);
	}
}

void InteractionChangeStateToFromManipulationEvent::execute() {
	int i;
	User* user;
	PICKEDENTITY pickedEntity;

	user = UserDatabase::getUserById(getUserId());

	if (!user) {
		printd(
				ERROR,
				"InteractionChangeStateToManipulationEvent::execute(): user %u doesn't exist here!\n",
				getUserId());
		return;
	}

	for (i = 0; i < (int)pickedEntitiesCopy.size(); i++) {
		pickedEntity = pickedEntitiesCopy[i];
		if (toManipulation)
			user->pickUpEntity(pickedEntity.entity, pickedEntity.pickingOffset);
		else
			user->dropEntity(pickedEntity.entity->getTypeBasedId());
	}

	InteractionChangeStateEvent::execute();
}

InteractionHighlightSelectionActionEvent::InteractionHighlightSelectionActionEvent() :
	Event() {

} // InteractionHighlightSelectionActionEvent

InteractionHighlightSelectionActionEvent::InteractionHighlightSelectionActionEvent(std::vector<
		PICKEDENTITY>* highlightEntities, std::vector<PICKEDENTITY>* unhighlightEntities,
		std::string modelType, std::string modelUrl) :
	Event(INTERACTION_MODULE_ID, INTERACTION_MODULE_ID, "InteractionHighlightSelectionActionEvent") {
	int i;

	if (highlightEntities) {
		for (i = 0; i < (int)highlightEntities->size(); i++)
			highlightEntityIds.push_back(((highlightEntities->at(i)).entity)->getTypeBasedId());
	} // if

	if (unhighlightEntities) {
		for (i = 0; i < (int)unhighlightEntities->size(); i++)
			unhighlightEntityIds.push_back(((unhighlightEntities->at(i)).entity)->getTypeBasedId());
	} // if

	this->modelType = modelType;
	this->modelUrl = modelUrl;
	this->userId = UserDatabase::getLocalUserId();
} // InteractionHighlightSelectionActionEvent

InteractionHighlightSelectionActionEvent::~InteractionHighlightSelectionActionEvent() {

} // ~InteractionHighlightSelectionActionEvent

//**************************************//
// PUBLIC METHODS INHERITED FROM: Event //
//**************************************//

void InteractionHighlightSelectionActionEvent::encode(NetMessage* message) {
	int i;

	msgFunctions::encode(userId, message);
	msgFunctions::encode(modelType, message);
	msgFunctions::encode(modelUrl, message);

	msgFunctions::encode((unsigned)highlightEntityIds.size(), message);
	for (i = 0; i < (int)highlightEntityIds.size(); i++)
		msgFunctions::encode(highlightEntityIds[i], message);

	msgFunctions::encode((unsigned)unhighlightEntityIds.size(), message);
	for (i = 0; i < (int)unhighlightEntityIds.size(); i++)
		msgFunctions::encode(unhighlightEntityIds[i], message);
} // encode

void InteractionHighlightSelectionActionEvent::decode(NetMessage* message) {
	int i;
	unsigned size;
	unsigned entityId;

	msgFunctions::decode(userId, message);
	msgFunctions::decode(modelType, message);
	msgFunctions::decode(modelUrl, message);

	msgFunctions::decode(size, message);
	for (i = 0; i < (int)size; i++) {
		msgFunctions::decode(entityId, message);
		highlightEntityIds.push_back(entityId);
	} // for

	msgFunctions::decode(size, message);
	for (i = 0; i < (int)size; i++) {
		msgFunctions::decode(entityId, message);
		unhighlightEntityIds.push_back(entityId);
	} // for
} // decode

void InteractionHighlightSelectionActionEvent::execute() {
	int i;
	Entity* entity;
	ModelInterface* highlightModel;
	HighlightSelectionActionModel* selectionModel = NULL;
//	std::string modelPath = Configuration::getPath("Highlighters");
	SceneGraphInterface* sgIF = OutputInterface::getSceneGraphInterface();

	if (!sgIF) {
		printd(ERROR,
				"InteractionHighlightSelectionActionEvent::execute(): no SceneGraphInterface found!\n");
		return;
	} // if

//	modelPath = modelPath + modelUrl;
	std::string modelPath = getConcatenatedPath(modelUrl, "Highlighters");
	highlightModel = sgIF->loadModel(modelType, modelPath);
	if (!highlightModel) {
		printd(
				ERROR,
				"InteractionHighlightSelectionActionEvent::execute(): failed to load highlightmodel %s\n",
				modelPath.c_str());
		return;
	} // if

	if (userId == UserDatabase::getLocalUserId()) {
		Interaction* interactionModule = (Interaction*)SystemCore::getModuleByName("Interaction");
		assert(interactionModule);
		selectionModel
				= dynamic_cast<HighlightSelectionActionModel*> (interactionModule->getSelectionActionModel());
	} // if

	for (i = 0; i < (int)highlightEntityIds.size(); i++) {
		entity = WorldDatabase::getEntityWithTypeInstanceId(highlightEntityIds[i]);
		if (entity)
			highlightEntity(entity, sgIF, highlightModel, selectionModel);
		else
			printd(WARNING,
					"InteractionHighlightSelectionActionEvent::execute(): Could not find Entity for highlighting!\n");
		//		highlightEntity(highlightEntityIds[i], sgIF, highlightModel, selectionModel)
	} // for

	for (i = 0; i < (int)unhighlightEntityIds.size(); i++) {
		entity = WorldDatabase::getEntityWithTypeInstanceId(unhighlightEntityIds[i]);
		if (entity)
			unhighlightEntity(entity, sgIF, selectionModel);
		else
			printd(WARNING,
					"InteractionHighlightSelectionActionEvent::execute(): Could not find Entity for unhighlighting!\n");
		//		unhighlightEntity(unhighlightEntityIds[i], sgIF, highlightModel, selectionModel)
	} // for
} // execute

void InteractionHighlightSelectionActionEvent::highlightEntity(Entity* entity,
		SceneGraphInterface* sgIF, ModelInterface* model,
		HighlightSelectionActionModel* selectionModel) {
	AttachmentKey key;
	TransformationData boxTransform, invEntity, boxFinal;
	ModelInterface* entityModel = entity->getVisualRepresentation();
	ModelInterface* entityHighlightModel = model->clone();
	AABB bbox = entityModel->getAABB();
	bbox.p1 += gmtl::Vec3f(0.01, 0.01, 0.01);
	bbox.p0 -= gmtl::Vec3f(0.01, 0.01, 0.01);

	boxTransform = identityTransformation();
	boxTransform.scale = bbox.p1 - bbox.p0;
	boxTransform.position = (bbox.p1 + bbox.p0) * 0.5f;

	invEntity = entity->getWorldTransformation();
	invert(invEntity);
	multiply(boxFinal, invEntity, boxTransform);

	sgIF->attachModelToEntity(entity, entityHighlightModel, boxFinal);
	key.userId = userId;
	key.moduleId = INTERACTION_MODULE_ID;
	entity->setAttachment(key, entityHighlightModel);

	if (selectionModel)
		selectionModel->highlightEntity(entity, entityHighlightModel);
} // highlightEntity

void InteractionHighlightSelectionActionEvent::highlightEntity(unsigned typeBasedId,
		SceneGraphInterface* sgIF, ModelInterface* model,
		HighlightSelectionActionModel* selectionModel) {
	TransformationData boxTransform, invEntity, boxFinal;
	ModelInterface* entityModel;
	ModelInterface* entityHighlightModel;
	unsigned short typeId, instanceId;
	AABB bbox;

	Entity* entity = WorldDatabase::getEntityWithTypeInstanceId(typeBasedId);
	if (!entity) {
		split(typeBasedId, typeId, instanceId);
		printd(
				WARNING,
				"InteractionHighlightSelectionActionEvent::highlightEntity(): Could not find Entity to highlight with IDS: type: %u \tinstance: %u!\n",
				typeId, instanceId);
		return;
	} // if

	entityModel = entity->getVisualRepresentation();
	entityHighlightModel = model->clone();
	bbox = entityModel->getAABB();
	bbox.p1 += gmtl::Vec3f(0.01, 0.01, 0.01);
	bbox.p0 -= gmtl::Vec3f(0.01, 0.01, 0.01);

	boxTransform = identityTransformation();
	boxTransform.scale = bbox.p1 - bbox.p0;
	boxTransform.position = (bbox.p1 + bbox.p0) * 0.5f;

	invEntity = entity->getWorldTransformation();
	invert(invEntity);
	multiply(boxFinal, invEntity, boxTransform);

	sgIF->attachModelToEntity(entity, entityHighlightModel, boxFinal);

	//TODO: register model in Interaction-class!!!

	if (selectionModel)
		selectionModel->highlightEntity(entity, entityHighlightModel);
} // highlightEntity


void InteractionHighlightSelectionActionEvent::unhighlightEntity(Entity* entity,
		SceneGraphInterface* sgIF, HighlightSelectionActionModel* selectionModel) {
	AttachmentKey key;
	ModelInterface* entityHighlightModel;

	key.userId = userId;
	key.moduleId = INTERACTION_MODULE_ID;

	entityHighlightModel = (ModelInterface*)entity->getAttachment(key);
	if (!entityHighlightModel) {
		printd(
				WARNING,
				"InteractionHighlightSelectionActionEvent::unhighlightEntity(): Could not find model to unhighlight Entity  from user %u\n",
				userId);
		return;
	} // if

	if (selectionModel)
		selectionModel->unhighlightEntity(entity, entityHighlightModel);

	sgIF->detachModel(entityHighlightModel);
	entity->setAttachment(key, NULL);
	delete entityHighlightModel;

} // unhighlightEntity


//*****************//
// PUBLIC METHODS: //
//*****************//

InteractionVirtualHandManipulationActionEvent::InteractionVirtualHandManipulationActionEvent() :
	Event() {

} // InteractionVirtualHandManipulationActionEvent

InteractionVirtualHandManipulationActionEvent::InteractionVirtualHandManipulationActionEvent(
		std::vector<PICKEDENTITY>* manipulatingEntities, bool beginManipulation) :
			Event(
					INTERACTION_MODULE_ID, INTERACTION_MODULE_ID,
					beginManipulation ? "InteractionBeginVirtualHandManipulationActionEvent" : "InteractionEndVirtualHandManipulationActionEvent") {
	int i;

	for (i = 0; i < (int)manipulatingEntities->size(); i++)
		manipulatingEntityIds.push_back(((manipulatingEntities->at(i)).entity)->getTypeBasedId());

	this->beginManipulation = beginManipulation;
	this->userId = UserDatabase::getLocalUserId();
} // InteractionVirtualHandManipulationActionEvent

InteractionVirtualHandManipulationActionEvent::~InteractionVirtualHandManipulationActionEvent() {

} // ~InteractionVirtualHandManipulationActionEvent

//**************************************//
// PUBLIC METHODS INHERITED FROM: Event //
//**************************************//

void InteractionVirtualHandManipulationActionEvent::encode(NetMessage* message) {
	int i;

	msgFunctions::encode(beginManipulation, message);
	msgFunctions::encode(userId, message);

	msgFunctions::encode((unsigned)manipulatingEntityIds.size(), message);
	for (i = 0; i < (int)manipulatingEntityIds.size(); i++)
		msgFunctions::encode(manipulatingEntityIds[i], message);

} // encode

void InteractionVirtualHandManipulationActionEvent::decode(NetMessage* message) {
	int i;
	unsigned size;
	unsigned entityId;

	msgFunctions::decode(beginManipulation, message);
	msgFunctions::decode(userId, message);

	msgFunctions::decode(size, message);
	for (i = 0; i < (int)size; i++) {
		msgFunctions::decode(entityId, message);
		manipulatingEntityIds.push_back(entityId);
	} // for
} // decode

//*****************//
// PUBLIC METHODS: //
//*****************//
InteractionBeginVirtualHandManipulationActionEvent::InteractionBeginVirtualHandManipulationActionEvent() :
	InteractionVirtualHandManipulationActionEvent() {

}

InteractionBeginVirtualHandManipulationActionEvent::InteractionBeginVirtualHandManipulationActionEvent(
		std::vector<PICKEDENTITY>* manipulatingEntities) :
	InteractionVirtualHandManipulationActionEvent(manipulatingEntities, true) {

}


InteractionBeginVirtualHandManipulationActionEvent::~InteractionBeginVirtualHandManipulationActionEvent() {

} // ~InteractionBeginVirtualHandManipulationActionEvent

//******************************************************************************//
// PUBLIC METHODS INHERITED FROM: InteractionVirtualHandManipulationActionEvent //
//******************************************************************************//

void InteractionBeginVirtualHandManipulationActionEvent::execute() {
	int i;
	unsigned short type, id;
	User* user;
	bool localPipe = false;
	VirtualHandManipulationActionModel* manipulationModel = NULL;

	user = UserDatabase::getUserById(userId);
	assert(user);
	if (userId == UserDatabase::getLocalUserId())
		localPipe = true;

	if (localPipe) {
		Interaction* interactionModule = (Interaction*)SystemCore::getModuleByName("Interaction");
		assert(interactionModule);
		manipulationModel
				= dynamic_cast<VirtualHandManipulationActionModel*> (interactionModule->getManipulationActionModel());
	} // if

	for (i = 0; i < (int)manipulatingEntityIds.size(); i++) {
		split(manipulatingEntityIds[i], type, id);

		// 	printd("Opening Pipe with type %i and id %i\n", type, id);
		TransformationPipe* manipulationPipe = TransformationManager::openPipe(
				INTERACTION_MODULE_ID, WORLD_DATABASE_ID, 1, 0, type, id, 0, !localPipe, user);

		if (manipulationModel)
			manipulationModel->setManipulationPipe(manipulatingEntityIds[i], manipulationPipe);
	} // for
} // execute

//*****************//
// PUBLIC METHODS: //
//*****************//

InteractionEndVirtualHandManipulationActionEvent::InteractionEndVirtualHandManipulationActionEvent() :
	InteractionVirtualHandManipulationActionEvent() {

}

InteractionEndVirtualHandManipulationActionEvent::InteractionEndVirtualHandManipulationActionEvent(
		std::vector<PICKEDENTITY>* manipulatingEntities) :
	InteractionVirtualHandManipulationActionEvent(manipulatingEntities, false) {

}

InteractionEndVirtualHandManipulationActionEvent::~InteractionEndVirtualHandManipulationActionEvent() {

} // ~InteractionEndVirtualHandManipulationActionEvent

//******************************************************************************//
// PUBLIC METHODS INHERITED FROM: InteractionVirtualHandManipulationActionEvent //
//******************************************************************************//

void InteractionEndVirtualHandManipulationActionEvent::execute() {
	int i;
	unsigned short type, id;
	User* user;
	bool localPipe = false;
	VirtualHandManipulationActionModel* manipulationModel = NULL;

	user = UserDatabase::getUserById(userId);
	assert(user);
	if (userId == UserDatabase::getLocalUserId())
		localPipe = true;

	if (localPipe) {
		Interaction* interactionModule = (Interaction*)SystemCore::getModuleByName("Interaction");
		assert(interactionModule);
		manipulationModel
				= dynamic_cast<VirtualHandManipulationActionModel*> (interactionModule->getManipulationActionModel());
	} // if

	for (i = 0; i < (int)manipulatingEntityIds.size(); i++) {
		split(manipulatingEntityIds[i], type, id);

		// 	printd("Opening Pipe with type %i and id %i\n", type, id);
		TransformationPipe* manipulationPipe = TransformationManager::getPipe(
				INTERACTION_MODULE_ID, WORLD_DATABASE_ID, 1, 0, type, id, 0, !localPipe, user);

		if (!manipulationPipe)
			printd(
					WARNING,
					"InteractionEndVirtualHandManipulationActionType::execute(): could not find manipulation Pipe for user with ID %u\n",
					userId);
		else
			TransformationManager::closePipe(manipulationPipe);

		if (manipulationModel)
			manipulationModel->clearManipulationPipe(manipulatingEntityIds[i]);
	} // for
}

//-------------------InteractionEvents for HomerManipulationAction------------------

InteractionHomerManipulationActionEvent::InteractionHomerManipulationActionEvent() :
	Event() {

}

InteractionHomerManipulationActionEvent::~InteractionHomerManipulationActionEvent() {

}

InteractionHomerManipulationActionEvent::InteractionHomerManipulationActionEvent(std::vector<
		PICKEDENTITY>* manipulatingEntities, bool beginManipulation) :
			Event(
					INTERACTION_MODULE_ID, INTERACTION_MODULE_ID,
					beginManipulation ? "InteractionBeginHomerManipulationActionEvent" : "InteractionEndHomerManipulationActionEvent") {
	int i;

	for (i = 0; i < (int)manipulatingEntities->size(); i++)
		manipulatingEntityIds.push_back(((manipulatingEntities->at(i)).entity)->getTypeBasedId());

	this->beginManipulation = beginManipulation;
	this->userId = UserDatabase::getLocalUserId();
} // InteractionHomerManipulationActionEvent


void InteractionHomerManipulationActionEvent::encode(NetMessage* message) {
	int i;

	msgFunctions::encode(beginManipulation, message);
	msgFunctions::encode(userId, message);

	msgFunctions::encode((unsigned)manipulatingEntityIds.size(), message);
	for (i = 0; i < (int)manipulatingEntityIds.size(); i++)
		msgFunctions::encode(manipulatingEntityIds[i], message);

}
void InteractionHomerManipulationActionEvent::decode(NetMessage* message) {
	int i;
	unsigned size;
	unsigned entityId;

	msgFunctions::decode(beginManipulation, message);
	msgFunctions::decode(userId, message);

	msgFunctions::decode(size, message);
	for (i = 0; i < (int)size; i++) {
		msgFunctions::decode(entityId, message);
		manipulatingEntityIds.push_back(entityId);
	} // for
}

//--------class InteractionBeginManipulationActionEvent------------

InteractionBeginHomerManipulationActionEvent::InteractionBeginHomerManipulationActionEvent() :
	InteractionHomerManipulationActionEvent() {

}

InteractionBeginHomerManipulationActionEvent::InteractionBeginHomerManipulationActionEvent(
		std::vector<PICKEDENTITY>* manipulatingEntities) :
	InteractionHomerManipulationActionEvent(manipulatingEntities, true) {
}

InteractionBeginHomerManipulationActionEvent::~InteractionBeginHomerManipulationActionEvent() {
}

void InteractionBeginHomerManipulationActionEvent::execute() {
	int i;
	unsigned short type, id;
	User* user;
	bool localPipe = false;
	HomerManipulationActionModel* manipulationModel = NULL;

	user = UserDatabase::getUserById(userId);
	assert(user);
	if (userId == UserDatabase::getLocalUserId())
		localPipe = true;

	if (localPipe) {
		Interaction* interactionModule = (Interaction*)SystemCore::getModuleByName("Interaction");
		assert(interactionModule);
		manipulationModel
				= dynamic_cast<HomerManipulationActionModel*> (interactionModule->getManipulationActionModel());
	} // if

	for (i = 0; i < (int)manipulatingEntityIds.size(); i++) {
		split(manipulatingEntityIds[i], type, id);

		// 	printd("Opening Pipe with type %i and id %i\n", type, id);
		TransformationPipe* manipulationPipe = TransformationManager::openPipe(
				INTERACTION_MODULE_ID, WORLD_DATABASE_ID, 1, 0, type, id, 0, !localPipe, user);

		if (manipulationModel)
			manipulationModel->setManipulationPipe(manipulatingEntityIds[i], manipulationPipe);
	} // for
}

//class InteractionEndHomerManipulationActionEvent


InteractionEndHomerManipulationActionEvent::InteractionEndHomerManipulationActionEvent() :
	InteractionHomerManipulationActionEvent() {
}

InteractionEndHomerManipulationActionEvent::InteractionEndHomerManipulationActionEvent(std::vector<
		PICKEDENTITY>* manipulatingEntities) :
	InteractionHomerManipulationActionEvent(manipulatingEntities, false) {

}

InteractionEndHomerManipulationActionEvent::~InteractionEndHomerManipulationActionEvent() {
}

void InteractionEndHomerManipulationActionEvent::execute() {
	int i;
	unsigned short type, id;
	User* user;
	bool localPipe = false;
	HomerManipulationActionModel* manipulationModel = NULL;

	user = UserDatabase::getUserById(userId);
	assert(user);
	if (userId == UserDatabase::getLocalUserId())
		localPipe = true;

	if (localPipe) {
		Interaction* interactionModule = (Interaction*)SystemCore::getModuleByName("Interaction");
		assert(interactionModule);
		manipulationModel
				= dynamic_cast<HomerManipulationActionModel*> (interactionModule->getManipulationActionModel());
	} // if

	for (i = 0; i < (int)manipulatingEntityIds.size(); i++) {
		split(manipulatingEntityIds[i], type, id);

		// 	printd("Opening Pipe with type %i and id %i\n", type, id);
		TransformationPipe* manipulationPipe = TransformationManager::getPipe(
				INTERACTION_MODULE_ID, WORLD_DATABASE_ID, 1, 0, type, id, 0, !localPipe, user);

		if (!manipulationPipe)
			printd(
					WARNING,
					"InteractionEndHomerManipulationActionType::execute(): could not find manipulation Pipe for user with ID %u\n",
					userId);
		else
			TransformationManager::closePipe(manipulationPipe);

		if (manipulationModel)
			manipulationModel->clearManipulationPipe(manipulatingEntityIds[i]);
	} // for
}

