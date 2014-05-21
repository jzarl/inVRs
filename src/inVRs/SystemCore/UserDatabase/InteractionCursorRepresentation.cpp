/*
 * InteractionCursorRepresentation.cpp
 *
 *  Created on: Jul 27, 2009
 *      Author: rlander
 */

#include "InteractionCursorRepresentation.h"
#include "../SystemCore.h"
#include "../UtilityFunctions.h"
#include "../ComponentInterfaces/InteractionInterface.h"
#include "../../OutputInterface/OutputInterface.h"
#include "inVRs/SystemCore/DebugOutput.h"

// disable deprecation warning for std::auto_ptr when std::unique_ptr is not available.
#ifndef HAS_CXX11_UNIQUE_PTR
// Only do this on GCC versions that support it:
# if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 402
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
#endif

XmlConfigurationLoader InteractionCursorRepresentation::xmlConfigLoader;

InteractionCursorRepresentation::InteractionCursorRepresentation() :
		idleModel(NULL),
		selectionModel(NULL),
		manipulationModel(NULL),
		currentModel(NULL),
		cursorTrans(IdentityTransformation) {
	if (!xmlConfigLoader.hasConverters()) {
		xmlConfigLoader.registerConverter(new ConverterToV1_0a4);
	} // if
} // InteractionCursorRepresentation

InteractionCursorRepresentation::~InteractionCursorRepresentation() {
	if (idleModel)
		delete idleModel;
	if (selectionModel)
		delete selectionModel;
	if (manipulationModel)
		delete manipulationModel;
} // ~InteractionCursorRepresentation

bool InteractionCursorRepresentation::loadConfig(std::string configFile) {
	bool success = true;

	std::string configFileConcatenatedPath = getConcatenatedPath(configFile, "CursorRepresentationConfiguration");
	printd(INFO, "InteractionCursorRepresentation::loadConfig(): loading configuration %s\n",
			configFile.c_str());

	// load xml document via xmlConfigLoader
	std::auto_ptr<const XmlDocument> document(
			xmlConfigLoader.loadConfiguration(configFileConcatenatedPath));
	if (!document.get()) {
		printd(ERROR,
				"InteractionCursorRepresentation::loadConfig(): error at loading of configuration file %s!\n",
				configFile.c_str());
		return false;
	} // if

	// obtain model from <idleModel> element
	const XmlElement* idleModelElement =
		document->getElement("interactionCursorRepresentation.idleModel.representation");
	if (!idleModelElement) {
		printd(ERROR,
				"InteractionCursorRepresentation::loadConfig(): Missing node <idleModel> or subnode <representation>! Please fix your cursor configuration file!\n");
		return false;
	} // if
	idleModel = readRepresentationFromXmlElement(idleModelElement, OBJECTTYPE_CURSOR);
	if (!idleModel) {
		printd(WARNING,
				"InteractionCursorRepresentation::loadConfig(): Model for cursor could not be loaded! Cursor will not be visible!\n");
	} // if

	// obtain model from <selectionModel> element
	const XmlElement* selectionModelElement =
		document->getElement("interactionCursorRepresentation.selectionModel.representation");
	if (selectionModelElement) {
		selectionModel = readRepresentationFromXmlElement(selectionModelElement, OBJECTTYPE_CURSOR);
		if (!selectionModel) {
			printd(WARNING,
					"InteractionCursorRepresentation::loadConfig(): Model for cursor for selection state could not be loaded!\n");
		} // if
	} // if

	// obtain model from <manipulation> element
	const XmlElement* manipulationModelElement =
		document->getElement("interactionCursorRepresentation.manipulationModel.representation");
	if (manipulationModelElement) {
		manipulationModel = readRepresentationFromXmlElement(manipulationModelElement,
				OBJECTTYPE_CURSOR);
		if (!manipulationModel) {
			printd(WARNING,
					"InteractionCursorRepresentation::loadConfig(): Model for cursor for manipulation state could not be loaded!\n");
		} // if
	} // if

	currentModel = idleModel;

	return success;
} // loadConfig

void InteractionCursorRepresentation::setTransformation(const TransformationData& trans) {
	cursorTrans = trans;
	SceneGraphInterface* sgIF = OutputInterface::getSceneGraphInterface();
	if (sgIF && currentModel) {
		sgIF->updateModel(currentModel, trans);
	} // if
} // setTransformation

void InteractionCursorRepresentation::update(float dt) {
	InteractionInterface* interaction =
		(InteractionInterface*)SystemCore::getModuleByName("Interaction");
	SceneGraphInterface* sgIF = OutputInterface::getSceneGraphInterface();
	if (interaction && sgIF) {
		InteractionInterface::INTERACTIONSTATE state = interaction->getState();
		if (state == InteractionInterface::STATE_IDLE && currentModel != idleModel) {
			sgIF->detachModel(currentModel);
			currentModel = idleModel;
			sgIF->attachModel(currentModel, cursorTrans);
		} // if
		else if (state == InteractionInterface::STATE_SELECTION && selectionModel != NULL &&
				currentModel != selectionModel) {
			sgIF->detachModel(currentModel);
			currentModel = selectionModel;
			sgIF->attachModel(currentModel, cursorTrans);
		} // if
		else if (state == InteractionInterface::STATE_MANIPULATION && manipulationModel != NULL &&
				currentModel != manipulationModel) {
			sgIF->detachModel(currentModel);
			currentModel = manipulationModel;
			sgIF->attachModel(currentModel, cursorTrans);
		} // if
	} // if
} // update

ModelInterface* InteractionCursorRepresentation::getModel() {
	return currentModel;
} // getModel

//*****************************************************************************
// Configuration loading
//*****************************************************************************
InteractionCursorRepresentation::ConverterToV1_0a4::ConverterToV1_0a4() :
	XmlConfigurationConverter("0.0", "1.0a4") {
} // ConverterToV1_0a4

bool InteractionCursorRepresentation::ConverterToV1_0a4::convert(XmlDocument* document, const Version& version,
		std::string configFile) {
	bool success = true;

	updateDocument(document, XmlConfigurationLoader::XmlDtdUrl + "interactionCursorRepresentation_v1.0a4.dtd",
			destinationVersion, "interactionCursorRepresentation");

	return success;
} // convert


CursorRepresentationInterface* InteractionCursorRepresentationFactory::create(std::string className,
		std::string configFile) {
	if (className != "InteractionCursorRepresentation") {
		return NULL;
	} // if

	InteractionCursorRepresentation* result = new InteractionCursorRepresentation;

	if (!result->loadConfig(configFile)) {
		printd(ERROR, "InteractionCursorRepresentationFactory::create(): Error at configuration of InteractionCursorRepresentation!\n");
		delete result;
		result = NULL;
	} // if

	return result;
} // create
