/*
 * XmlConfigurationConverter.cpp
 *
 *  Created on: Jun 25, 2009
 *      Author: rlander
 */

#include "XmlConfigurationConverter.h"

XmlConfigurationConverter::XmlConfigurationConverter(Version minVersion, Version destinationVersion) :
	minVersion(minVersion),
	destinationVersion(destinationVersion) {
} // ~XmlConfigurationConverter

XmlConfigurationConverter::XmlConfigurationConverter(std::string minVersion, std::string destinationVersion) :
	minVersion(Version(minVersion)),
	destinationVersion(Version(destinationVersion)) {
} // XmlConfigurationConverter


XmlConfigurationConverter::~XmlConfigurationConverter() {
}

Version XmlConfigurationConverter::getMinimumVersion() const {
	return minVersion;
} // getMinimumVersion

Version XmlConfigurationConverter::getDestinationVersion() const {
	return destinationVersion;
} // getDestinationVersion

bool XmlConfigurationConverter::updateDocument(XmlDocument* document, std::string dtd,
		const Version& version, std::string rootElementName) {

	// update root element name
	XmlElement* root = document->getRootElement();
	if (!root) {
		return false;
	} // if
	root->setName(rootElementName);

	// update version entry in root element
	if (root->hasAttribute("version")) {
		root->getAttribute("version")->setValue(version.toString());
	} // if
	else {
		XmlAttribute* versionAttribute = new XmlAttribute("version", version.toString());
		root->addAttribute(versionAttribute);
	} // else

	// update dtd reference
	XmlDtdReference* dtdRef = document->getDtd();
	if (!dtdRef) {
		dtdRef = new XmlDtdReference(rootElementName, dtd);
		document->setDtd(dtdRef);
	} else {
		dtdRef->setName(rootElementName);
		dtdRef->setDtd(dtd);
	} // else

	return true;
} // updateRootElement

