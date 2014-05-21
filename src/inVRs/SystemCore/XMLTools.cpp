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

#include <stdarg.h>
#include <assert.h>
#include <string>
#ifdef INVRS_HAVE_STAT
	#include <sys/stat.h>
#endif

#include <gmtl/AxisAngle.h>
#include <gmtl/MatrixOps.h>
#include <gmtl/Generate.h>

#include "Platform.h"
#include "XMLTools.h"
#include "DebugOutput.h"
#include "../OutputInterface/SceneGraphInterface.h"
#include "../OutputInterface/OutputInterface.h"
#include "UtilityFunctions.h"
#include "Configuration.h"
#include "WorldDatabase/WorldDatabase.h"


XmlConfigurationLoader XmlArgumentVectorLoader::xmlConfigLoader;
XmlConfigurationLoader XmlRepresentationLoader::xmlConfigLoader;
XmlConfigurationLoader XmlTransformationLoader::xmlConfigLoader;

XmlConfigurationLoader& XmlArgumentVectorLoader::get() {
	if (!xmlConfigLoader.hasConverters()) {
		xmlConfigLoader.registerConverter(new ConverterToV1_0a4);
	} // if
	return xmlConfigLoader;
} // get

XmlArgumentVectorLoader::ConverterToV1_0a4::ConverterToV1_0a4() :
	XmlConfigurationConverter("0.0", "1.0a4") {
} // ConverterToV1_0a4

bool XmlArgumentVectorLoader::ConverterToV1_0a4::convert(XmlDocument* document,
		const Version& version, std::string configFile) {

	std::vector<XmlAttribute*> attributes = document->getAttributes("arguments.arg.type");
	std::vector<XmlAttribute*>::iterator argIt;
	XmlAttribute* argAttribute;
	for (argIt = attributes.begin(); argIt != attributes.end(); ++argIt) {
		argAttribute = *argIt;
		if (argAttribute->getValue() == "unsigned") {
			argAttribute->setValue("uint");
		} // if
	} // for

	return true;
} // convert

XmlConfigurationLoader& XmlRepresentationLoader::get() {
	if (!xmlConfigLoader.hasConverters()) {
		xmlConfigLoader.registerConverter(new ConverterToV1_0a4);
	} // if
	return xmlConfigLoader;
} // get

XmlRepresentationLoader::ConverterToV1_0a4::ConverterToV1_0a4() :
	XmlConfigurationConverter("0.0", "1.0a4") {
} // ConverterToV1_0a4

bool XmlRepresentationLoader::ConverterToV1_0a4::convert(XmlDocument* document,
		const Version& version, std::string configFile) {
	bool success = true;

	XmlAttribute* attribute = NULL;
	XmlElement* rootElement = document->getRootElement();
	XmlElement* modelElement = rootElement->getSubElement("model");
	XmlElement* modelTransformationElement = rootElement->getSubElement("modelTransformation");
	if (!modelElement) {
		printd(ERROR,
				"XmlRepresentationLoader::ConverterToV1_0a4::convert(): could not convert file because node <model> is missing! Please upgrade to a current xml file version\n");
		return false;
	} // if

	// create new representation node
	XmlElement* representationElement = new XmlElement("representation");
	// remove model node from root
	success = rootElement->removeSubElement(modelElement);
	assert(success);
	// add representation node to root
	rootElement->addSubElement(representationElement);
	// add model element below representation node
	representationElement->addSubElement(modelElement);
	// update <model file=...> to <file name=...>
	modelElement->setName("file");
	if (modelElement->hasAttribute("path"))
		attribute = modelElement->getAttribute("path");
	else
		attribute = modelElement->getAttribute("file");

	if (!attribute) {
		printd(ERROR,
				"XmlRepresentationLoader::ConverterToV1_0a4::convert(): could not convert file because attribute path/file in node <model> is missing! Please upgrade to a current xml file version\n");
		return false;
	} // if
	attribute->setKey("name");

	// move modelTransformationElement from root node into representation and rename it to
	// transformation
	if (modelTransformationElement) {
		success = rootElement->removeSubElement(modelTransformationElement);
		assert(success);
		representationElement->addSubElement(modelTransformationElement);
		modelTransformationElement->setName("transformation");
		success = XmlTransformationLoader::get().updateXmlElement(modelTransformationElement,
				version, destinationVersion, configFile);
	} // if

	return success;
} // XmlRepresentationLoader

XmlConfigurationLoader& XmlTransformationLoader::get() {
	if (!xmlConfigLoader.hasConverters()) {
		xmlConfigLoader.registerConverter(new ConverterToV1_0a4);
	} // if
	return xmlConfigLoader;
} // get

XmlTransformationLoader::ConverterToV1_0a4::ConverterToV1_0a4() :
	XmlConfigurationConverter("0.0", "1.0a4") {
} // ConverterToV1_0a4

bool XmlTransformationLoader::ConverterToV1_0a4::convert(XmlDocument* document,
		const Version& version, std::string configFile) {
	XmlElement* rootElement = document->getRootElement();
	if (rootElement->getName() != "transformation" &&
			rootElement->getName() != "modelTransformation" &&
			rootElement->getName() != "adjustment") {
		printd(WARNING,
				"XmlTransformationLoader::ConverterToV1_0a4::convert(): Found unexpected node <%s>! Trying to convert it into a transformation node!\n",
				rootElement->getName().c_str());
	} // if

	// rename rotation angle to angleDeg
	XmlAttribute* angleAttribute = rootElement->getAttribute("rotation.angle");
	if (angleAttribute) {
		angleAttribute->setKey("angleDeg");
	} // if

	// fix order of elements
	std::vector<std::string> elementOrder;
	elementOrder.push_back("translation");
	elementOrder.push_back("rotation");
	elementOrder.push_back("scale");
	elementOrder.push_back("scaleOrientation");
	rootElement->fixSubElementOrder(elementOrder);

	return true;
} // XmlRepresentationLoader

INVRS_SYSTEMCORE_API bool readTransformationDataFromXmlElement(TransformationData& dst,
		const XmlElement* element) {

	gmtl::AxisAnglef axAng;
	gmtl::Vec3f axis;
	float angle;
	const XmlElement* translation = element->getSubElement("translation");
	const XmlElement* rotation = element->getSubElement("rotation");
	const XmlElement* scale = element->getSubElement("scale");
	const XmlElement* scaleOrientation = element->getSubElement("scaleOrientation");

	if (translation) {
		dst.position = gmtl::Vec3f(translation->getAttributeValueAsFloat("x"),
				translation->getAttributeValueAsFloat("y"),
				translation->getAttributeValueAsFloat("z"));
	} // if
	if (scale) {
		dst.scale = gmtl::Vec3f(scale->getAttributeValueAsFloat("x"),
				scale->getAttributeValueAsFloat("y"),
				scale->getAttributeValueAsFloat("z"));
	}
	if (rotation) {
		if (rotation->hasAttribute("angleDeg")) {
			angle = rotation->getAttributeValueAsFloat("angleDeg") * M_PI / 180.f;
		} else {
			angle = rotation->getAttributeValueAsFloat("angleRad");
		} // else
		// load and normalize axis separately because of gmtl problems on windows systems
		axis = gmtl::Vec3f(rotation->getAttributeValueAsFloat("x"),
				rotation->getAttributeValueAsFloat("y"),
				rotation->getAttributeValueAsFloat("z"));
		gmtl::normalize(axis);
		axAng = gmtl::AxisAnglef(angle, axis);
		gmtl::set(dst.orientation, axAng);
	} // if
	if (scaleOrientation) {
		if (scaleOrientation->hasAttribute("angleDeg")) {
			angle = scaleOrientation->getAttributeValueAsFloat("angleDeg") * M_PI / 180.f;
		} else {
			angle = scaleOrientation->getAttributeValueAsFloat("angleRad");
		} // else
		// load and normalize axis separately because of gmtl problems on windows systems
		axis = gmtl::Vec3f(rotation->getAttributeValueAsFloat("x"),
				rotation->getAttributeValueAsFloat("y"),
				rotation->getAttributeValueAsFloat("z"));
		gmtl::normalize(axis);
		axAng = gmtl::AxisAnglef(angle, axis);
		gmtl::set(dst.scaleOrientation, axAng);
	} // if
	return true;
} // readTransformationDataFromXmlElement

INVRS_SYSTEMCORE_API XmlElement* createXmlElementFromTransformationData(const TransformationData& src) {
	XmlElement *xmlElemTransformation = new XmlElement("transformation");

	XmlElement *xmlElemTranslation = new XmlElement("translation");
	xmlElemTranslation->addAttribute(new XmlAttribute("x", toString(src.position[0])));
	xmlElemTranslation->addAttribute(new XmlAttribute("y", toString(src.position[1])));
	xmlElemTranslation->addAttribute(new XmlAttribute("z", toString(src.position[2])));
	xmlElemTransformation->addSubElement(xmlElemTranslation);

	gmtl::AxisAnglef rotation = gmtl::make<gmtl::AxisAnglef>(src.orientation);
	XmlElement *xmlElemRotation = new XmlElement("rotation");
	xmlElemRotation->addAttribute(new XmlAttribute("x", toString(rotation.getAxis()[0])));
	xmlElemRotation->addAttribute(new XmlAttribute("y", toString(rotation.getAxis()[1])));
	xmlElemRotation->addAttribute(new XmlAttribute("z", toString(rotation.getAxis()[2])));
	xmlElemRotation->addAttribute(new XmlAttribute("angleDeg", toString(
					gmtl::Math::rad2Deg(rotation.getAngle()))));
	xmlElemTransformation->addSubElement(xmlElemRotation);

	XmlElement *xmlElemScale = new XmlElement("scale");
	xmlElemScale->addAttribute(new XmlAttribute("x", toString(src.scale[0])));
	xmlElemScale->addAttribute(new XmlAttribute("y", toString(src.scale[1])));
	xmlElemScale->addAttribute(new XmlAttribute("z", toString(src.scale[2])));
	xmlElemTransformation->addSubElement(xmlElemScale);

	return xmlElemTransformation;
} //createXmlElementFromTransformationData

INVRS_SYSTEMCORE_API XmlElement* createXmlElementFromModelInterface(ModelInterface* src) {
	XmlElement *xmlElemRepresentation = new XmlElement("representation");

	if (src->getDeepCloneMode())
		xmlElemRepresentation->addAttribute(new XmlAttribute("copy", "true"));
	else
		xmlElemRepresentation->addAttribute(new XmlAttribute("copy", "false"));

	std::string tmpString;
	XmlElement *xmlElemFile = new XmlElement("file");
	tmpString = src->getFilePath();
	tmpString = tmpString.substr(tmpString.find_last_of(".")+1);
	if (tmpString == "wrl") tmpString = "VRML";
	xmlElemFile->addAttribute(new XmlAttribute("type", tmpString));
	tmpString = src->getFilePath();
	tmpString = tmpString.substr(tmpString.find_last_of("/\\")+1);
	xmlElemFile->addAttribute(new XmlAttribute("name", tmpString));
	xmlElemRepresentation->addSubElement(xmlElemFile);

	XmlElement *xmlElemTransformation = createXmlElementFromTransformationData(src->getModelTransformation());

	xmlElemRepresentation->addSubElement(xmlElemTransformation);

	return xmlElemRepresentation;
} // createXmlElementFromModelInterface

INVRS_SYSTEMCORE_API bool readTransformationDataFromXML(TransformationData& dst, IrrXMLReader* xml, const char* endTag) {
	gmtl::Quatf rotationQuat;
	gmtl::AxisAnglef rotation;
	TransformationData result = identityTransformation();

	assert(xml);

	bool finished = false;
	bool success = true;

	while (!finished && xml && xml->read()) {
		switch (xml -> getNodeType()) {
		case EXN_ELEMENT:
			if (!strcmp(endTag, xml->getNodeName())) {
			} // if
			else if (!strcmp("translation", xml->getNodeName())) {
				result.position = gmtl::Vec3f(xml->getAttributeValueAsFloat("x"), xml->getAttributeValueAsFloat("y"), xml->getAttributeValueAsFloat("z"));
			} // if
			else if (!strcmp("rotation", xml->getNodeName())) {
				rotation = gmtl::AxisAnglef(xml->getAttributeValueAsFloat("angle")*M_PI/180.f, xml->getAttributeValueAsFloat("x"), xml->getAttributeValueAsFloat("y"), xml->getAttributeValueAsFloat("z"));
				gmtl::set(rotationQuat, rotation);
				result.orientation = rotationQuat;
			} // else if
			else if (!strcmp("scale", xml->getNodeName())) {
				result.scale = gmtl::Vec3f(xml->getAttributeValueAsFloat("x"), xml->getAttributeValueAsFloat("y"), xml->getAttributeValueAsFloat("z"));
			} // else if
			else if (!strcmp("scaleOrientation", xml->getNodeName())) {
				rotation = gmtl::AxisAnglef(xml->getAttributeValueAsFloat("angle")*M_PI/180.f, xml->getAttributeValueAsFloat("x"), xml->getAttributeValueAsFloat("y"), xml->getAttributeValueAsFloat("z"));
				gmtl::set(rotationQuat, rotation);
				result.scaleOrientation = rotationQuat;
			} // else if
			else {
				success = false;
				finished = true;
			} // else
			break;

		case EXN_ELEMENT_END:
			if (!strcmp(endTag, xml->getNodeName())) {
				finished = true;
			} // if
			break;

		default:
			break;
		} // switch
	} // while

	if (!finished) {
		printd(ERROR, "readTransformationDataFromXML(): Could not find EntTag </%s>!\n", endTag);
		success = false;
	} // if

	if (success) {
		dst = result;
	} // if

	return success;
} // readTransformationDataFromXML

INVRS_SYSTEMCORE_API ArgumentVector* readArgumentsFromXmlElement(const XmlElement* element) {
	std::string key;
	std::string value;
	std::string type;
	ArgumentVector* result = NULL;
	std::vector<const XmlElement*> arguments;
	std::vector<const XmlElement*>::iterator argIt;
	const XmlElement* argElement;

	const XmlElement* argumentsElement;
	if (element->getName() == "arguments") {
		argumentsElement = element;
	} // if
	else {
		argumentsElement = element->getSubElement("arguments");
	} // else
	if (!argumentsElement) {
		printd(WARNING, "readArgumentsFromXmlElement(): Unable to find arguments in passed XML node!\n");
		return result;
	} // if

	result = new ArgumentVector();
	arguments = argumentsElement->getSubElements("arg");

	for (argIt = arguments.begin(); argIt != arguments.end(); ++argIt) {
		argElement = *argIt;
		if (!argElement->hasAttribute("key") || !argElement->hasAttribute("value")) {
			printd(ERROR,
					"readAttributesFromXML(): incomplete argument specification: missing key or value attribute in <arg> element!\n");
			break;
		} // if
		key = argElement->getAttributeValue("key");
		value = argElement->getAttributeValue("value");
		type = argElement->getAttributeValue("type");
		if (type.size() == 0)
			type = "string";
		if (type == "string") {
			result->push_back(key, value);
		} else if (type == "int") {
			result->push_back(key, atoi(value.c_str()));
		} else if (type == "float") {
			result->push_back(key, (float)atof(value.c_str()));
		} else if (type == "bool") {
			if (!strcasecmp(value.c_str(), "true") || (value == "1")) {
				result->push_back(key, true);
			} else {
				result->push_back(key, false);
			} // else
		} else if (type == "uint") {
			result->push_back(key, (unsigned)atoi(value.c_str()));
		} else {
			printd(ERROR,
					"readAttributesFromXML(): encountered unsupported datatype %s! IGNORING VALUE!\n",
					type.c_str());
		} // else
	} // for
	return result;
} // readArgumentsFromXmlElement


INVRS_SYSTEMCORE_API ArgumentVector* readArgumentsFromXML(IrrXMLReader* xml,
		std::string endTag) {
	ArgumentVector* ret;
	const char* nodeName;
	const char *key, *value, *type;
	bool finished = false;
	bool err = false;
	bool argumentNodeFound = false;
	ret = new ArgumentVector();
	while (!finished && xml && xml->read() && !err) {
		nodeName = xml->getNodeName();
		switch (xml -> getNodeType()) {
		case EXN_ELEMENT:
			if (!strcmp("arguments", nodeName)) {
				argumentNodeFound = true;
			} else if (!strcmp("arg", nodeName)) {
				key = xml->getAttributeValue("key");
				value = xml->getAttributeValue("value");
				type = xml->getAttributeValue("type");
				if (!key || !value) {
					printd(ERROR, "readAttributesFromXML(): incomplete argument specification!\n");
					err = true;
					break;
				}
				if (!type)
					type = "string";
				if (!strcmp(type, "string")) {
					ret->push_back(key, std::string(value));
				} else if (!strcmp(type, "int")) {
					ret->push_back(key, atoi(value));
				} else if (!strcmp(type, "float")) {
					ret->push_back(key, (float)atof(value));
				} else if (!strcmp(type, "bool")) {
					if (!strcasecmp(value, "true") || (*value == '1')) {
						ret->push_back(key, true);
					} else {
						ret->push_back(key, false);
					}
				} else if (!strcmp(type, "unsigned")) {
					ret->push_back(key, (unsigned)atoi(value));
				} else
					printd(
							ERROR,
							"readAttributesFromXML(): encountered unsupported datatype %s! IGNORING VALUE!\n",
							type);
			} else {
				printd(ERROR, "readAttributesFromXML(): encountered unexpected tag %s\n", nodeName);
				err = true;
			}
			break;

		case EXN_ELEMENT_END:
			if (!strcmp("arguments", nodeName)) {
				finished = true;
			} else if (!argumentNodeFound && !strcmp(nodeName, endTag.c_str())) {
				err = true;
				finished = true;
			}
			else {
				printd(ERROR, "readAttributesFromXML(): encountered unexpected end-tag %s\n",
						nodeName);
				err = true;
			}
			break;

		default:
			break;
		} // switch
	} // while

	if (err) {
		delete ret;
		return NULL;
	} // if
	return ret;
}

INVRS_SYSTEMCORE_API ModelInterface* readRepresentationFromXmlElement(const XmlElement* element,
		OBJECTTYPE type) {

	ModelInterface* model = NULL;
	bool copy = true;
	std::string copyStr;
	std::string modelPath;
	int referenceId = -1;
	Tile* referenceTile = NULL;
	EntityType* referenceEntityType = NULL;
	ModelInterface* referenceModel = NULL;
	TransformationData modelTransformation;
	const XmlElement* transformationElement = NULL;
	SceneGraphInterface* sgIF = OutputInterface::getSceneGraphInterface();

	if (!sgIF) {
		printd(ERROR, "readRepresentationFromXML(): no scenegraph interface found! Model can't be loaded!\n");
		return NULL;
	} // if

	copyStr = element->getAttributeValue("copy");
	if (copyStr.size() > 0)
		convert(copyStr, copy);

	if (type == OBJECTTYPE_ENTITY)
		modelPath = Configuration::getPath("Entities");
	else if (type == OBJECTTYPE_TILE)
		modelPath = Configuration::getPath("Tiles");
	else if (type == OBJECTTYPE_AVATAR)
		modelPath = Configuration::getPath("Avatars");
	else if (type == OBJECTTYPE_CURSOR)
		modelPath = Configuration::getPath("Cursors");

	if (element->hasSubElement("file")) {
		std::string fileType = element->getAttributeValue("file.type");
		std::string fileName = element->getAttributeValue("file.name");
		model = sgIF->loadModel(fileType, modelPath + fileName);
		if (!model) {
			printd(ERROR,
					"readRepresentationFromXmlElement(): failed to load model from file %s\n",
					fileName.c_str());
			return NULL;
		} // else
	} else if (element->hasSubElement("reference")) {
		referenceId = element->getAttributeValueAsInt("reference.sourceId");
		if (type == OBJECTTYPE_ENTITY) {
			referenceEntityType = WorldDatabase::getEntityTypeWithId(referenceId);
			if (!referenceEntityType) {
				printd(ERROR,
						"readRepresentationFromXmlElement(): Could not find EntityType with ID %i for model reference!\n",
						referenceId);
				return NULL;
			} // if
			referenceModel = referenceEntityType->getModel();
			if (!referenceModel) {
				printd(ERROR,
						"readRepresentationFromXmlElement(): Referenced EntityType with ID %i does not contain a Model!\n",
						referenceId);
				return NULL;
			} // if
			model = referenceModel->clone();
		} // if
		else if (type == OBJECTTYPE_TILE) {
			referenceTile = WorldDatabase::getTileWithId(referenceId);
			if (!referenceTile) {
				printd(ERROR,
						"readRepresentationFromXmlElement(): Could not find Tile with ID %i for model reference!\n",
						referenceId);
				return NULL;
			} // if
			referenceModel = referenceTile->getModel();
			if (!referenceModel) {
				printd(ERROR,
						"readRepresentationFromXmlElement(): Referenced Tile with ID %i does not contain a Model!\n",
						referenceId);
				return NULL;
			} // if
			model = referenceModel->clone();
		} // else if
		else if (type == OBJECTTYPE_AVATAR) {
			printd(ERROR, "readRepresentationFromXmlElement(): Invalid use of a model reference in the avatar configuration file! Please specify the model file!\n");
			return NULL;
		} // else if
		else if (type == OBJECTTYPE_CURSOR) {
			printd(ERROR, "readRepresentationFromXmlElement(): Invalid use of a model reference in the cursor configuration file! Please specify the model file!\n");
			return NULL;
		} // else if
		else  {
			printd(ERROR, "readRepresentationFromXmlElement(): Invalid use of a model reference in a configuration file! Please specify the model file!\n");
			return NULL;
		} // else
	} else {
		printd(ERROR,
				"readRepresentationFromXmlElement(): unable to load representation: missing node <file> or <reference>!\n");
		return NULL;
	} // else
	if (!model) {
		printd(ERROR,
				"readRepresentationFromXmlElement(): internal error: model could not be loaded!\n");
		return NULL;
	} // if

	if (copyStr.size() > 0) {
		model->setDeepCloneMode(copy);
	} // if

	transformationElement = element->getSubElement("transformation");
	if (transformationElement) {
		modelTransformation = IdentityTransformation;
		readTransformationDataFromXmlElement(modelTransformation, transformationElement);
		model->setModelTransformation(modelTransformation);
	} // if

	return model;
} // readRepresentationFromXmlElement


INVRS_SYSTEMCORE_API ModelInterface* readRepresentationFromXML(IrrXMLReader* xml, OBJECTTYPE type) {
	bool finished = false;

	std::string modelPath;
	ModelInterface* model = NULL;
	char* copyStr = NULL;
	char* fileType = NULL;
	char* fileName = NULL;
	bool copy = true;
	int referenceId = -1;
	Tile* referenceTile = NULL;
	EntityType* referenceEntityType = NULL;
	ModelInterface* referenceModel = NULL;
	SceneGraphInterface* sgIF = OutputInterface::getSceneGraphInterface();
	TransformationData modelTransformation;

	if (!sgIF)
		printd(WARNING, "readRepresentationFromXML(): no scenegraph interface found!\n");

	copyStr = (char*)xml->getAttributeValue("copy");
	if (copyStr)
		convert(copyStr, copy);

	if (type == OBJECTTYPE_ENTITY)
		modelPath = Configuration::getPath("Entities");
	else if (type == OBJECTTYPE_TILE)
		modelPath = Configuration::getPath("Tiles");
	else if (type == OBJECTTYPE_AVATAR)
		modelPath = Configuration::getPath("Avatars");

	while (!finished && xml && xml->read()) {
		switch (xml->getNodeType()) {
		case EXN_ELEMENT:
			if (!strcmp("file", xml->getNodeName())) {
				if (model != NULL) {
					printd(ERROR,
							"readRepresentationFromXML(): Found <file> entry but model already loaded!\n");
					break;
				} // if
				fileType = (char*)xml->getAttributeValue("type");
				fileName = (char*)xml->getAttributeValue("name");
				model = sgIF->loadModel(fileType, modelPath + fileName);
				if (model)
					model->setDeepCloneMode(copy);
				else
					printd(ERROR,
							"readRepresentationFromXML(): failed to load model from file %s\n",
							fileName);
			} // if
			else if (!strcmp("reference", xml->getNodeName())) {
				if (model != NULL) {
					printd(ERROR,
							"readRepresentationFromXML(): Found <reference> entry but model already loaded!\n");
					break;
				} // if
				referenceId = xml->getAttributeValueAsInt("sourceId");
				if (type == OBJECTTYPE_ENTITY) {
					referenceEntityType = WorldDatabase::getEntityTypeWithId(referenceId);
					if (!referenceEntityType) {
						printd(
								ERROR,
								"readRepresentationFromXML(): Could not find EntityType with ID %i for model reference!\n",
								referenceId);
						break;
					} // if
					referenceModel = referenceEntityType->getModel();
					if (!referenceModel) {
						printd(
								ERROR,
								"readRepresentationFromXML(): Referenced EntityType with ID %i does not contain a Model!\n",
								referenceId);
						break;
					} // if
					model = referenceModel->clone();
				} // if
				else if (type == OBJECTTYPE_TILE) {
					referenceTile = WorldDatabase::getTileWithId(referenceId);
					if (!referenceTile) {
						printd(
								ERROR,
								"readRepresentationFromXML(): Could not find Tile with ID %i for model reference!\n",
								referenceId);
						break;
					} // if
					referenceModel = referenceTile->getModel();
					if (!referenceModel) {
						printd(
								ERROR,
								"readRepresentationFromXML(): Referenced Tile with ID %i does not contain a Model!\n",
								referenceId);
						break;
					} // if
					model = referenceModel->clone();
				} // else if
				else if (type == OBJECTTYPE_AVATAR) {
					printd(ERROR, "readRepresentationFromXML(): Invalid use of a model reference in the avatar configuration file! Please specify the model file!\n");
					break;
				}
				else  {
					printd(ERROR, "readRepresentationFromXML(): Invalid use of a model reference in a configuration file! Please specify the model file!\n");
					break;
				}
			} // else if
			else if (!strcmp("transformation", xml->getNodeName())) {
				if (!model) {
					printd(ERROR,
							"readRepresentationFromXML(): can not set model-transformation, no Model loaded!\n");
					break;
				} // if
				modelTransformation = identityTransformation();
				readTransformationDataFromXML(modelTransformation, xml, "transformation");
				model->setModelTransformation(modelTransformation);
			} // else if
			break;
		case EXN_ELEMENT_END:
			if (!strcmp("representation", xml->getNodeName()))
				finished = true;
			break;
		default:
			break;
		} // switch
	} // while

	if (!finished)
		printd(ERROR, "readRepresentationFromXML(): Could not parse <representation> element!\n");

	return model;
} // readRepresentationFromXML

INVRS_SYSTEMCORE_API bool stringEqualsAny(std::string& src) {
	if (src == "any" || src == "ANY" || src == "Any")
		return true;
	return false;
} // stringEqualsAny

INVRS_SYSTEMCORE_API bool fileExists(const std::string& file) {
#ifndef INVRS_HAVE_STAT
	FILE* f = fopen(file.c_str(), "r");
	if (!f)
		return false;
	fclose(f);
	return true;
#else
	// using stat does not open the file, and thus can avoid some side-effects
	// (and is of course faster)
	struct stat sb;
	if ( stat(file.c_str(),&sb) == 0)
	{
		// file exists, but is it really a file?
#if defined S_ISREG
		return S_ISREG(sb.st_mode);
#else
		// win32 has flaky posix compliance/omits S_ISREG:
		return (S_IFREG & sb.st_mode);
#endif
	}
	return false;
#endif
} // fileExists
