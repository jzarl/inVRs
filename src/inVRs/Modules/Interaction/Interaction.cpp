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

#include "Interaction.h"

#include <assert.h>

#include "HomerCursorModel.h"
#include "ManipulationOffsetModifier.h"
#include <inVRs/SystemCore/UserDatabase/User.h>
#include <inVRs/SystemCore/SystemCore.h>
#include <inVRs/SystemCore/DebugOutput.h>
#include <inVRs/SystemCore/XMLTools.h>
#include <inVRs/SystemCore/UtilityFunctions.h>
#include <inVRs/SystemCore/TransformationManager/TransformationManager.h>
#include <inVRs/SystemCore/ComponentInterfaces/module_core_api.h>

// disable deprecation warning for std::auto_ptr when std::unique_ptr is not available.
#ifndef HAS_CXX11_UNIQUE_PTR
// Only do this on GCC versions that support it:
# if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 402
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
#endif

std::vector<SelectionChangeModelFactory*>		Interaction::selectionChangeModelFactoryList;
std::vector<ManipulationChangeModelFactory*>	Interaction::manipulationChangeModelFactoryList;
std::vector<StateActionModelFactory*>			Interaction::actionModelFactoryList;
XmlConfigurationLoader							Interaction::xmlConfigLoader;

Interaction::Interaction() {
	//	SystemCore::registerModuleInterface(this);

	registerSelectionChangeModelFactory(new DefaultSelectionChangeModelsFactory);
	registerManipulationChangeModelFactory(new DefaultManipulationChangeModelsFactory);
	User::registerCursorTransformationModelFactory(new HomerCursorModelFactory);

	manipulationConfirmationModel = NULL;
	manipulationTerminationModel = NULL;
	selectionChangeModel = NULL;
	unselectionChangeModel = NULL;
	incomingEvents = NULL;
	idleActionModel = NULL;
	selectionActionModel = NULL;
	manipulationActionModel = NULL;
	state = STATE_IDLE;

	EventManager::registerEventFactory("InteractionHighlightSelectionActionEvent",
			new InteractionHighlightSelectionActionEvent::Factory());
	EventManager::registerEventFactory("InteractionBeginVirtualHandManipulationActionEvent",
			new InteractionBeginVirtualHandManipulationActionEvent::Factory());
	EventManager::registerEventFactory("InteractionEndVirtualHandManipulationActionEvent",
			new InteractionEndVirtualHandManipulationActionEvent::Factory());
	EventManager::registerEventFactory("InteractionChangeStateEvent",
			new InteractionChangeStateEvent::Factory());
	EventManager::registerEventFactory("InteractionChangeStateToFromManipulationEvent",
			new InteractionChangeStateToFromManipulationEvent::Factory());
	EventManager::registerEventFactory("InteractionBeginHomerManipulationActionEvent",
			new InteractionBeginHomerManipulationActionEvent::Factory());
	EventManager::registerEventFactory("InteractionEndHomerManipulationActionEvent",
			new InteractionEndHomerManipulationActionEvent::Factory());

	actionModelFactoryList.push_back(new HighlightSelectionActionModelFactory);
	actionModelFactoryList.push_back(new HomerManipulationActionModelFactory);
	actionModelFactoryList.push_back(new VirtualHandManipulationActionModelFactory);

	sharedManipulationMergerFactory = new SharedManipulationMergerFactory();
	TransformationManager::registerMergerFactory(sharedManipulationMergerFactory);
	TransformationManager::registerModifierFactory(new ManipulationOffsetModifierFactory());

	xmlConfigLoader.registerConverter(new ConverterToV1_0a4);
}

Interaction::~Interaction() {
	int i;

	printd(INFO, "Interaction destructor: entering ...\n");

	if (selectionChangeModel != NULL)
		delete selectionChangeModel;
	if (unselectionChangeModel != NULL)
		delete unselectionChangeModel;
	if (manipulationConfirmationModel != NULL)
		delete manipulationConfirmationModel;
	if (manipulationTerminationModel != NULL)
		delete manipulationTerminationModel;
	if (idleActionModel != NULL)
		delete idleActionModel;
	if (selectionActionModel != NULL)
		delete selectionActionModel;
	if (manipulationActionModel != NULL)
		delete manipulationActionModel;

	for (i = 0; i < (int)selectionChangeModelFactoryList.size(); i++)
		delete selectionChangeModelFactoryList[i];

	for (i = 0; i < (int)manipulationChangeModelFactoryList.size(); i++)
		delete manipulationChangeModelFactoryList[i];

	for (i = 0; i < (int)actionModelFactoryList.size(); i++)
		delete actionModelFactoryList[i];

	printd(INFO, "Interaction destructor: finished!\n");

	//	SystemCore::unregisterModuleInterface(this);
}

bool Interaction::loadConfig(std::string configFile) {

	std::string configFileConcatenatedPath = getConcatenatedPath(configFile, "InteractionModuleConfiguration");

	// load xml document via xmlConfigLoader
	std::auto_ptr<const XmlDocument> document(
			xmlConfigLoader.loadConfiguration(configFileConcatenatedPath));
	if (!document.get()) {
		printd(ERROR,
				"Interaction::loadConfig(): error at loading of configuration file %s!\n",
				configFile.c_str());
		return false;
	} // if

	bool success = true;

	const XmlElement* element;
	std::string type;

	if (document->hasElement("interaction.stateActionModels.idleActionModel")) {
		std::auto_ptr<ArgumentVector> arguments;
		element = document->getElement("interaction.stateActionModels.idleActionModel");
		type = element->getAttributeValue("type");
		if (element->hasSubElement("arguments")) {
			arguments.reset(readArgumentsFromXmlElement(element));
		} // if
		success = setIdleActionModel(type, arguments.get()) && success;
	} // if
	if (document->hasElement("interaction.stateActionModels.selectionActionModel")) {
		std::auto_ptr<ArgumentVector> arguments;
		element = document->getElement("interaction.stateActionModels.selectionActionModel");
		type = element->getAttributeValue("type");
		if (element->hasSubElement("arguments")) {
			arguments.reset(readArgumentsFromXmlElement(element));
		} // if
		success = setSelectionActionModel(type, arguments.get()) && success;
	} // if
	if (document->hasElement("interaction.stateActionModels.manipulationActionModel")) {
		std::auto_ptr<ArgumentVector> arguments;
		element = document->getElement("interaction.stateActionModels.manipulationActionModel");
		type = element->getAttributeValue("type");
		if (element->hasSubElement("arguments")) {
			arguments.reset(readArgumentsFromXmlElement(element));
		} // if
		success = setManipulationActionModel(type, arguments.get()) && success;
	} // if
	if (document->hasElement("interaction.stateTransitionModels.selectionChangeModel")) {
		std::auto_ptr<ArgumentVector> arguments;
		element = document->getElement("interaction.stateTransitionModels.selectionChangeModel");
		type = element->getAttributeValue("type");
		if (element->hasSubElement("arguments")) {
			arguments.reset(readArgumentsFromXmlElement(element));
		} // if
		success = setSelectionChangeModel(type, arguments.get()) && success;
	} // if
	if (document->hasElement("interaction.stateTransitionModels.unselectionChangeModel")) {
		std::auto_ptr<ArgumentVector> arguments;
		element = document->getElement("interaction.stateTransitionModels.unselectionChangeModel");
		type = element->getAttributeValue("type");
		if (element->hasSubElement("arguments")) {
			arguments.reset(readArgumentsFromXmlElement(element));
		} // if
		success = setUnselectionChangeModel(type, arguments.get()) && success;
	} // if
	if (document->hasElement("interaction.stateTransitionModels.manipulationConfirmationModel")) {
		std::auto_ptr<ArgumentVector> arguments;
		element = document->getElement("interaction.stateTransitionModels.manipulationConfirmationModel");
		type = element->getAttributeValue("type");
		if (element->hasSubElement("arguments")) {
			arguments.reset(readArgumentsFromXmlElement(element));
		} // if
		success = setManipulationConfirmationModel(type, arguments.get()) && success;
	} // if
	if (document->hasElement("interaction.stateTransitionModels.manipulationTerminationModel")) {
		std::auto_ptr<ArgumentVector> arguments;
		element = document->getElement("interaction.stateTransitionModels.manipulationTerminationModel");
		type = element->getAttributeValue("type");
		if (element->hasSubElement("arguments")) {
			arguments.reset(readArgumentsFromXmlElement(element));
		} // if
		success = setManipulationTerminationModel(type, arguments.get()) && success;
	} // if

	return success;
} // loadConfig

void Interaction::cleanup() {

} // cleanup

std::string Interaction::getName() {
	return "Interaction";
}

void Interaction::destroyEntityNotification(Entity* entity) {
	int i;
	std::vector<PICKEDENTITY> entitiesToUnselect;
	PICKEDENTITY pickedEntity;
	float dt = 0;

	switch (state) {
	case STATE_SELECTION: {
		for (i = 0; i < (int)selectedEntitiesList.size(); i++) {
			if (selectedEntitiesList[i].entity == entity)
				break;
		} // for
		if (i == (int)selectedEntitiesList.size())
			return;

		pickedEntity.entity = entity;
		pickedEntity.pickingOffset = identityTransformation();
		entitiesToUnselect.push_back(pickedEntity);
		if (selectedEntitiesList.size() == 1) {
			// okay go back to idle state
			entitiesToSelect.clear();
			selectedEntitiesList.clear();

			// trigger state change
			InteractionChangeStateEvent* evt = new InteractionChangeStateEvent(
					&selectedEntitiesList, STATE_SELECTION, STATE_IDLE, selectionActionModel,
					idleActionModel);
			EventManager::sendEvent(evt, EventManager::EXECUTE_LOCAL);
		} // if

		// perform selection action here
		if (selectionActionModel)
			selectionActionModel->action(&entitiesToSelect, &entitiesToUnselect, dt);

		removeFromList(&selectedEntitiesList, &entitiesToUnselect);
		addToList(&selectedEntitiesList, &entitiesToSelect);

		entitiesToSelect.clear();
	} break;

	case STATE_MANIPULATION: {
		for (i = 0; i < (int)selectedEntitiesList.size(); i++) {
			if (selectedEntitiesList[i].entity == entity)
				break;
		} // for
		if (i == (int)selectedEntitiesList.size())
			return;

		Event* evt = new InteractionChangeStateToFromManipulationEvent(&selectedEntitiesList,
				false, manipulationActionModel, idleActionModel);
		EventManager::sendEvent(evt, EventManager::EXECUTE_GLOBAL);

		selectedEntitiesList.clear();
	} break;

	default:
		break;

	} // switch
} // destroyEntityNotification

void Interaction::update(float dt) {
	Event* evt;
	std::vector<PICKEDENTITY> entitiesToUnselect;

	if (!incomingEvents) {
		incomingEvents = EventManager::getPipe(INTERACTION_MODULE_ID);
		if (!incomingEvents) {
			printd(ERROR, "Interaction::step(): failed to fetch my event queue!\n");
			return;
		}
	}

	if (incomingEvents) {
		while (incomingEvents->size() > 0) {
			evt = incomingEvents->pop_front();
			evt->execute();
			delete evt;
		} // while
	} // if

	if (!selectionChangeModel)
		printd(ERROR, "Interaction::step(): Selection Change Model not set!\n");
	if (!unselectionChangeModel)
		printd(ERROR, "Interaction::step(): Unselection Change Model not set!\n");
	if (!manipulationConfirmationModel)
		printd(ERROR, "Interaction::step(): Selection Comfirmation Transition Model not set!\n");
	if (!manipulationTerminationModel)
		printd(ERROR, "Interaction::step(): Manipulation Termination Transition Model not set!\n");
	if (!selectionChangeModel || !unselectionChangeModel || !manipulationConfirmationModel
			|| !manipulationTerminationModel) {
		return;
	}

	//TODO: destroying of entities which are stored in one of the entity lists is still unhandled
	// possible solution: some kind of a finalize at the ModuleInterface level, which is invoked whenever a entity is destroyed ...

	switch (state) {
	case STATE_IDLE: // S0

		// perform idle action here!
		if (idleActionModel)
			idleActionModel->action(NULL, NULL, dt);

		assert(selectedEntitiesList.size() == 0);
		assert(entitiesToSelect.size() == 0);

		selectionChangeModel->select(&selectedEntitiesList, &entitiesToSelect, &entitiesToUnselect);

		if (selectionChangeModel->changeState()) {
			// go into selection state
			assert(entitiesToSelect.size()>=1);
			addToList(&selectedEntitiesList, &entitiesToSelect);
			entitiesToSelect.clear();

			// trigger state change event:
			evt = new InteractionChangeStateEvent(&selectedEntitiesList, STATE_IDLE,
					STATE_SELECTION, idleActionModel, selectionActionModel);
			EventManager::sendEvent(evt, EventManager::EXECUTE_LOCAL);

			// prepare selectionConfirmationModel for waiting for confirmation
			manipulationConfirmationModel->enterTransitionSource();

			// 				if(idleActionModel) idleActionModel->exit();
			// 				if(selectionActionModel) selectionActionModel->enter(&selectedEntitiesList);
		} else {
			// stay in idle state
			assert(entitiesToSelect.size() == 0);
		}
		break;

	case STATE_SELECTION: // S1

		if (manipulationConfirmationModel->changeState()) {
			assert(selectedEntitiesList.size()> 0);
			assert(selectedEntitiesList.size() <= 1);
			//if(selectionActionModel) selectionActionModel->exit();

			manipulationTerminationModel->enterTransitionSource();
			//if(manipulationActionModel) manipulationActionModel->enter(&selectedEntitiesList);

			entitiesToSelect.clear();
			// trigger state change

			evt = new InteractionChangeStateToFromManipulationEvent(&selectedEntitiesList, true,
					selectionActionModel, manipulationActionModel);
			EventManager::sendEvent(evt, EventManager::EXECUTE_GLOBAL);

			break;
		}

		// do unselect / selection here if no confirmation occured
		assert(selectedEntitiesList.size()> 0);
		unselectionChangeModel->unselect(&selectedEntitiesList, &entitiesToUnselect);
		selectionChangeModel->select(&selectedEntitiesList, &entitiesToSelect, &entitiesToUnselect);

		if (unselectionChangeModel->changeState()) {

			// all selected entities have been unselected
			// it could be that other entities have been selected afterwards
			if ((entitiesToSelect.size() == 0) && (entitiesToUnselect.size()
					== selectedEntitiesList.size())) {
				// okay go back to idle state

				// 					if(selectionActionModel) selectionActionModel->exit();
				entitiesToSelect.clear();
				selectedEntitiesList.clear();
				// 					if(idleActionModel) idleActionModel->enter(NULL);

				// trigger state change
				evt = new InteractionChangeStateEvent(&selectedEntitiesList, STATE_SELECTION,
						STATE_IDLE, selectionActionModel, idleActionModel);
				EventManager::sendEvent(evt, EventManager::EXECUTE_LOCAL);
				break;
			}
		}

		// perform selection action here
		if (selectionActionModel)
			selectionActionModel->action(&entitiesToSelect, &entitiesToUnselect, dt);

		removeFromList(&selectedEntitiesList, &entitiesToUnselect);
		addToList(&selectedEntitiesList, &entitiesToSelect);
		entitiesToSelect.clear();

		break;

	case STATE_MANIPULATION: // S2

		assert(entitiesToSelect.size() == 0);
		assert(selectedEntitiesList.size()> 0);

		if (manipulationTerminationModel->changeState()) {
			// go back into idle state

			evt = new InteractionChangeStateToFromManipulationEvent(&selectedEntitiesList, false,
					manipulationActionModel, idleActionModel);
			EventManager::sendEvent(evt, EventManager::EXECUTE_GLOBAL);

			selectedEntitiesList.clear();
			break;
		}

		if (manipulationActionModel)
			manipulationActionModel->action(NULL, NULL, dt);

		break;
	}
}

void Interaction::registerSelectionChangeModelFactory(
		SelectionChangeModelFactory* newSelectionChangeModelFactory) {
	selectionChangeModelFactoryList.push_back(newSelectionChangeModelFactory);
}

void Interaction::registerManipulationChangeModelFactory(
		ManipulationChangeModelFactory* newManipulationChangeModelFactory) {
	manipulationChangeModelFactoryList.push_back(newManipulationChangeModelFactory);
}

void Interaction::registerStateActionModelFactory(
		StateActionModelFactory* newStateActionModelFactory) {
	actionModelFactoryList.push_back(newStateActionModelFactory);
} // registerStateActionModelFactory

StateActionModel* Interaction::getSelectionActionModel() {
	return selectionActionModel;
}

StateActionModel* Interaction::getManipulationActionModel() {
	return manipulationActionModel;
}

SelectionChangeModel* Interaction::getSelectionChangeModel() {
	return selectionChangeModel;
}

ManipulationChangeModel* Interaction::getManipulationTerminationModel()
{
	return manipulationTerminationModel;
}

Interaction::INTERACTIONSTATE Interaction::getState() {
	return state;
}

bool Interaction::setSelectionChangeModel(std::string newSelectionChangeModel,
		ArgumentVector* argVec) {
	int i;
	SelectionChangeModel* found;
	for (i = 0; i < (int)selectionChangeModelFactoryList.size(); i++) {
		found = selectionChangeModelFactoryList[i]->create(newSelectionChangeModel, argVec);
		if (found) {
			selectionChangeModel = found;
			return true;
		}
	}

	printd(ERROR, "Interaction::setSelectionChangeModel(): could not produce instance of %s\n",
			newSelectionChangeModel.c_str());
	return false;
}

bool Interaction::setUnselectionChangeModel(std::string newUnselectionChangeModel,
		ArgumentVector* argVec) {
	int i;
	SelectionChangeModel* found;
	for (i = 0; i < (int)selectionChangeModelFactoryList.size(); i++) {
		found = selectionChangeModelFactoryList[i]->create(newUnselectionChangeModel, argVec);
		if (found) {
			unselectionChangeModel = found;
			return true;
		}
	}

	printd(ERROR, "Interaction::setUnselectionChangeModel(): could not produce instance of %s\n",
			newUnselectionChangeModel.c_str());
	return false;
}

bool Interaction::setManipulationConfirmationModel(std::string newManipulationConfirmationModel,
		ArgumentVector* argVec) {
	int i;
	ManipulationChangeModel* found;
	for (i = 0; i < (int)manipulationChangeModelFactoryList.size(); i++) {
		found = manipulationChangeModelFactoryList[i]->create(newManipulationConfirmationModel, argVec);
		if (found) {
			manipulationConfirmationModel = found;
			return true;
		}
	}

	printd(ERROR,
			"Interaction::setSelectionConfirmationModel(): could not produce instance of %s\n",
			newManipulationConfirmationModel.c_str());
	return false;
}

bool Interaction::setManipulationTerminationModel(std::string newManipulationTerminationModel,
		ArgumentVector* argVec) {
	int i;
	ManipulationChangeModel* found;
	for (i = 0; i < (int)manipulationChangeModelFactoryList.size(); i++) {
		found = manipulationChangeModelFactoryList[i]->create(newManipulationTerminationModel, argVec);
		if (found) {
			manipulationTerminationModel = found;
			return true;
		}
	}

	printd(ERROR,
			"Interaction::setManipulationTerminationModel(): could not produce instance of %s\n",
			newManipulationTerminationModel.c_str());
	return false;
}

bool Interaction::setIdleActionModel(std::string newIdleActionModel, ArgumentVector* argVec) {
	int i;
	StateActionModel* found;
	for (i = 0; i < (int)actionModelFactoryList.size(); i++) {
		found = actionModelFactoryList[i]->create(newIdleActionModel, argVec);
		if (found) {
			idleActionModel = found;
			return true;
		} // if
	} // for

	printd(ERROR, "Interaction::setIdleActionModel(): could not produce instance of %s\n",
			newIdleActionModel.c_str());
	return false;
} // setIdleActionModel

bool Interaction::setSelectionActionModel(std::string newSelectionActionModel,
		ArgumentVector* argVec) {
	int i;
	StateActionModel* found;
	for (i = 0; i < (int)actionModelFactoryList.size(); i++) {
		found = actionModelFactoryList[i]->create(newSelectionActionModel, argVec);
		if (found) {
			selectionActionModel = found;
			return true;
		} // if
	} // for

	printd(ERROR, "Interaction::setSelectionActionModel(): could not produce instance of %s\n",
			newSelectionActionModel.c_str());
	return false;
}

bool Interaction::setManipulationActionModel(std::string newManipulationActionModel,
		ArgumentVector* argVec) {
	int i;
	StateActionModel* found;
	for (i = 0; i < (int)actionModelFactoryList.size(); i++) {
		found = actionModelFactoryList[i]->create(newManipulationActionModel, argVec);
		if (found) {
			manipulationActionModel = found;
			return true;
		}
	}

	printd(ERROR, "Interaction::setManipulationActionModel(): could not produce instance of %s\n",
			newManipulationActionModel.c_str());
	return false;
}

void Interaction::addToList(std::vector<PICKEDENTITY>* dst, std::vector<PICKEDENTITY>* src) {
	int i;
	for (i = 0; i < (int)src->size(); i++)
		dst->push_back((*src)[i]);
}

void Interaction::removeFromList(std::vector<PICKEDENTITY>* dst,
		std::vector<PICKEDENTITY>* removeMe) {
	int i, j;
	Entity* ent;
	for (i = 0; i < (int)removeMe->size(); i++) {
		ent = (*removeMe)[i].entity;
		for (j = 0; j < (int)dst->size(); j++) {
			if ((*dst)[j].entity == ent) {
				dst->erase(dst->begin() + j);
				break; // there should be only one pointer of a single entity in the whole list
			}
		}
	}
}

//*****************************************************************************
// Configuration loading
//*****************************************************************************
Interaction::ConverterToV1_0a4::ConverterToV1_0a4() :
	XmlConfigurationConverter("0.0", "1.0a4") {

} // ConverterToV1_0a4

bool Interaction::ConverterToV1_0a4::convert(XmlDocument* document, const Version& version,
		std::string configFile) {
	bool success = true;

	updateDocument(document, XmlConfigurationLoader::XmlDtdUrl + "interaction_v1.0a4.dtd",
			destinationVersion, "interaction");

	// get root element and list of all subelements
	XmlElement* rootElement = document->getRootElement();
	std::vector<XmlElement*> childElements = rootElement->getAllSubElements();

	// create new elements and add them to root element
	XmlElement* stateActionModelsElement = new XmlElement("stateActionModels");
	XmlElement* stateTransitionModelsElement = new XmlElement("stateTransitionModels");
	rootElement->addSubElement(stateActionModelsElement);
	rootElement->addSubElement(stateTransitionModelsElement);

	// iterate over all previous sub elements, detach them from root and attach them to the correct
	// new subnode of root
	std::vector<XmlElement*>::iterator elementIt;
	XmlElement* element;
	std::string elementName;
	for (elementIt = childElements.begin(); elementIt != childElements.end(); ++elementIt) {
		element = *elementIt;
		// update argumentvector if available
		if (element->hasSubElement("arguments")) {
			XmlElement* argumentElement = element->getSubElement("arguments");
			XmlArgumentVectorLoader::get().updateXmlElement(argumentElement, version,
					destinationVersion, configFile);
		} // if
		// remove element from root
		rootElement->removeSubElement(element);
		// add element to corresponding subelement of root
		elementName = element->getName();
		if (elementName == "idleActionModel" || elementName == "selectionActionModel"
				|| elementName == "manipulationActionModel") {
			stateActionModelsElement->addSubElement(element);
		} // if
		else if (elementName == "selectionChangeModel" || elementName == "unselectionChangeModel"
				|| elementName == "selectionConfirmationModel"
				|| elementName == "manipulationTerminationModel") {
			stateTransitionModelsElement->addSubElement(element);
		} // else if
		else {
			printd(ERROR,
					"Interaction::ConverterToV1_0a4::convert(): found unexpected node <%s>! Please fix your Interaction configuration file!\n",
					elementName.c_str());
			success = false;
			delete element;
		} // else
	} // for

	// rename element
	document->renameElements("interaction.stateTransitionModels.selectionConfirmationModel",
			"manipulationConfirmationModel");

	// replace all name attributes with type attributes
	document->renameAttributes("interaction.stateActionModels.idleActionModel.name", "type");
	document->renameAttributes("interaction.stateActionModels.selectionActionModel.name", "type");
	document->renameAttributes("interaction.stateActionModels.manipulationActionModel.name",
			"type");
	document->renameAttributes("interaction.stateTransitionModels.selectionChangeModel.name",
			"type");
	document->renameAttributes("interaction.stateTransitionModels.unselectionChangeModel.name",
			"type");
	document->renameAttributes(
			"interaction.stateTransitionModels.manipulationConfirmationModel.name",	"type");
	document->renameAttributes(
			"interaction.stateTransitionModels.manipulationTerminationModel.name", "type");

	// rename ButtonPressStateTransitionModel to ButtonPressManipulationChangeModel
	document->replaceAttributeValues(
			"interaction.stateTransitionModels.manipulationConfirmationModel.type",
			"ButtonPressStateTransitionModel", "ButtonPressManipulationChangeModel");
	document->replaceAttributeValues(
			"interaction.stateTransitionModels.manipulationTerminationModel.type",
			"ButtonPressStateTransitionModel", "ButtonPressManipulationChangeModel");

	return success;
} // convert


MAKEMODULEPLUGIN(Interaction, SystemCore)

