/*
 * SimpleCursorRepresentation.cpp
 *
 *  Created on: Jul 27, 2009
 *      Author: rlander
 */

#include "SimpleCursorRepresentation.h"
#include "../UtilityFunctions.h"
#include "../DebugOutput.h"
#include "../../OutputInterface/OutputInterface.h"

// disable deprecation warning for std::auto_ptr when std::unique_ptr is not available.
#ifndef HAS_CXX11_UNIQUE_PTR
// Only do this on GCC versions that support it:
# if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 402
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
#endif

XmlConfigurationLoader	SimpleCursorRepresentation::xmlConfigLoader;

SimpleCursorRepresentation::SimpleCursorRepresentation() :
		model(NULL) {
	if (!xmlConfigLoader.hasConverters()) {
		xmlConfigLoader.registerConverter(new ConverterToV1_0a4);
	} // if
} // SimpleCursorRepresentation

SimpleCursorRepresentation::~SimpleCursorRepresentation() {
	if (model)
		delete model;
} // ~SimpleCursorRepresentation

bool SimpleCursorRepresentation::loadConfig(std::string configFile) {
	bool success = true;

	std::string configFileConcatenatedPath = getConcatenatedPath(configFile, "CursorRepresentationConfiguration");
	printd(INFO, "SimpleCursorRepresentation::loadConfig(): loading configuration %s\n",
			configFile.c_str());

	// load xml document via xmlConfigLoader
	std::auto_ptr<const XmlDocument> document(
			xmlConfigLoader.loadConfiguration(configFileConcatenatedPath));
	if (!document.get()) {
		printd(ERROR,
				"SimpleCursorRepresentation::loadConfig(): error at loading of configuration file %s!\n",
				configFile.c_str());
		return false;
	} // if

	// obtain model from representation element
	const XmlElement* representationElement =
		document->getElement("simpleCursorRepresentation.representation");
	if (!representationElement) {
		printd(ERROR,
				"SimpleCursorRepresentation::loadConfig(): Missing node <representation>!\n");
		return false;
	} // if
	model = readRepresentationFromXmlElement(representationElement, OBJECTTYPE_CURSOR);
	if (!model) {
		printd(WARNING,
				"SimpleCursorRepresentation::loadConfig(): Model for cursor could not be loaded! Cursor will not be visible!\n");
	} // if

	return success;
} // loadConfig

void SimpleCursorRepresentation::setTransformation(const TransformationData& trans) {
	SceneGraphInterface* sgIF = OutputInterface::getSceneGraphInterface();
	if (sgIF && model) {
		sgIF->updateModel(model, trans);
	} // if
} // setTransformation

void SimpleCursorRepresentation::update(float dt) {
	// nothing to do here
} // update

ModelInterface* SimpleCursorRepresentation::getModel() {
	return model;
} // getModel

//*****************************************************************************
// Configuration loading
//*****************************************************************************
SimpleCursorRepresentation::ConverterToV1_0a4::ConverterToV1_0a4() :
	XmlConfigurationConverter("0.0", "1.0a4") {
} // ConverterToV1_0a4

bool SimpleCursorRepresentation::ConverterToV1_0a4::convert(XmlDocument* document, const Version& version,
		std::string configFile) {
	bool success = true;

	updateDocument(document, XmlConfigurationLoader::XmlDtdUrl + "simpleCursorRepresentation_v1.0a4.dtd",
			destinationVersion, "simpleCursorRepresentation");

	return success;
} // convert

CursorRepresentationInterface* SimpleCursorRepresentationFactory::create(std::string className,
		std::string configFile) {
	if (className != "SimpleCursorRepresentation") {
		return NULL;
	} // if

	SimpleCursorRepresentation* result = new SimpleCursorRepresentation;

	if (!result->loadConfig(configFile)) {
		printd(ERROR, "SimpleCursorRepresentationFactory::create(): Error at configuration of SimpleCursorRepresentation!\n");
		delete result;
		result = NULL;
	} // if

	return result;
} // create
