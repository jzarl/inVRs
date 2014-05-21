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

#include "SimpleAvatar.h"

#include <assert.h>
#include <memory>

//#include "../SystemCore.h"
#include "../../OutputInterface/OutputInterface.h"
#include "../Configuration.h"
#include "../XMLTools.h"
#include "../DebugOutput.h"
#include "../UtilityFunctions.h"

// disable deprecation warning for std::auto_ptr when std::unique_ptr is not available.
#ifndef HAS_CXX11_UNIQUE_PTR
// Only do this on GCC versions that support it:
# if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 402
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
#endif

using namespace io;
using namespace irr;

XmlConfigurationLoader SimpleAvatar::xmlConfigLoader;

SimpleAvatar::SimpleAvatar() {
	modelTransform = identityTransformation();
	avatarTransformation = identityTransformation();
	avatarModel = NULL;
	sgIF = OutputInterface::getSceneGraphInterface();
	if (!xmlConfigLoader.hasConverters()) {
		xmlConfigLoader.registerConverter(new ConverterToV1_0a4);
	} // if
} // SimpleAvatar

SimpleAvatar::~SimpleAvatar() {
	// Nothing to do here
} // ~SimpleAvatar

bool SimpleAvatar::loadConfig(std::string configFile) {
	std::string configFileConcatenatedPath = getConcatenatedPath(configFile, "AvatarConfiguration");
	printd(INFO, "SimpleAvatar::loadConfig(): loading configuration %s\n", configFile.c_str());

	// load xml document via xmlConfigLoader
	std::auto_ptr<const XmlDocument> document(
			xmlConfigLoader.loadConfiguration(configFileConcatenatedPath));
	if (!document.get()) {
		printd(ERROR,
				"SimpleAvatar::loadConfig(): error at loading of configuration file %s!\n",
				configFile.c_str());
		return false;
	} // if

	// load avatar name
	if (document->hasAttribute("simpleAvatar.name.value")) {
		this->avatarName = document->getAttributeValue("simpleAvatar.name.value");
	} // if

	const XmlElement* representationElement =
		document->getElement("simpleAvatar.representation");
	if (!representationElement) {
		printd(ERROR,
				"SimpleAvatar::loadConfig(): Missing node <representation>!\n");
		return false;
	} // if
	avatarModel = readRepresentationFromXmlElement(representationElement, OBJECTTYPE_AVATAR);

	return (avatarModel != NULL);
} // loadConfig

void SimpleAvatar::showAvatar(bool active) {
	if (avatarModel != NULL && avatarModel->isVisible() != active) {
		avatarModel->setVisible(active);
	} // if
} // showAvatar

void SimpleAvatar::setTransformation(TransformationData trans) {
	avatarTransformation = trans;

	if (avatarModel != NULL && sgIF) {
		sgIF->updateModel(avatarModel, avatarTransformation);
	}
} // setTransformation

ModelInterface* SimpleAvatar::getModel() {
	return avatarModel;
} // getModel

bool SimpleAvatar::isVisible() {
	if (avatarModel != NULL)
		return avatarModel->isVisible();
	return false;
} // isVisible

//*****************************************************************************
// Configuration loading
//*****************************************************************************
SimpleAvatar::ConverterToV1_0a4::ConverterToV1_0a4() :
	XmlConfigurationConverter("0.0", "1.0a4") {
} // ConverterToV1_0a4

bool SimpleAvatar::ConverterToV1_0a4::convert(XmlDocument* document, const Version& version,
		std::string configFile) {
	if (document->getRootElement()->getName() != "avatar" &&
			document->getRootElement()->getName() != "config") {
		printd(ERROR,
				"SimpleAvatar::ConverterToV1_0a4::convert(): could not convert file because of invalid root node <%s> in Avatar configuration file! Please upgrade to a current xml file version\n",
				document->getRootElement()->getName().c_str());
		return false;
	} // if

	XmlElement* avatarElement = NULL;

	// replace root node (if existing) and make <avatar> node as root
	if (document->getRootElement()->getName() != "avatar") {
		avatarElement = document->getRootElement()->getSubElement("avatar");
		if (!avatarElement) {
			printd(ERROR,
					"SimpleAvatar::ConverterToV1_0a4::convert(): could not convert file because of misssing node <avatar>! Please upgrade to a current xml file version!\n");
			return false;
		} // if
		document->getRootElement()->removeSubElement(avatarElement);
		XmlElement* oldRootElement = document->replaceRootElement(avatarElement);
		delete oldRootElement;
	} // if
	else {
		avatarElement = document->getRootElement();
	} // else

	// replace avatar node with type based node
	// get type of avatar and use it as new node name, afterwards delete all attributes
	std::string avatarType = avatarElement->getAttributeValue("type");
	if (avatarType != "SimpleAvatar") {
		printd(ERROR,
				"SimpleAvatar::ConverterToV1_0a4::convert(): expected type SimpleAvatar, found type %s!\n",
				avatarType.c_str());
		return false;
	} // if
	avatarElement->deleteAllAttributes();

	updateDocument(document, XmlConfigurationLoader::XmlDtdUrl + "simpleAvatar_v1.0a4.dtd",
			destinationVersion, "simpleAvatar");

	bool success = XmlRepresentationLoader::get().updateXmlElement(avatarElement,
			version, destinationVersion, configFile);

	return success;
} // convert


AvatarInterface* SimpleAvatarFactory::create(std::string className, std::string configFile) {
	if (className != "SimpleAvatar")
		return NULL;

	AvatarInterface *avatar;

	avatar = new SimpleAvatar();
	if (!avatar->loadConfig(configFile)) {
		printd(ERROR, "SimpleAvatarFactory::create(): Error at configuration of SimpleAvatar!\n");
		delete avatar;
		return NULL;
	} // if

	return avatar;
} // create
