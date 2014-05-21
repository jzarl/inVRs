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
#include <assert.h>
#include <memory>

#include "Configuration.h"
#include "DebugOutput.h"
#include "XmlDocument.h"
#include "XMLTools.h"
#include "UtilityFunctions.h"

// disable deprecation warning for std::auto_ptr when std::unique_ptr is not available.
#ifndef HAS_CXX11_UNIQUE_PTR
// Only do this on GCC versions that support it:
# if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 402
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
#endif

std::map<std::string, std::string> Configuration::pathMap;
std::string Configuration::rootPath = "";
Configuration::ConfigurationEntry Configuration::rootEntry;
XmlConfigurationLoader Configuration::xmlConfigLoader;

bool Configuration::loadConfig(std::string fileName) {

	// initialize xmlConfigLoader if not done already
	if (!xmlConfigLoader.hasConverters()) {
		xmlConfigLoader.registerConverter(new Configuration::ConverterToV1_0a4);
	} // if

	// load xml document via xmlConfigLoader
	std::auto_ptr<const XmlDocument> document(xmlConfigLoader.loadConfiguration(fileName, false));
	if (!document.get()) {
		printd(ERROR,
				"Configuration::loadConfig(): error at loading of configuration file %s!\n",
				fileName.c_str());
		return false;
	} // if

	bool success = parseGeneralConfig(document.get());
	success = parsePathConfig(document.get()) && success;

	if (success && Configuration::contains("XmlConfigLoader.updateFiles") &&
			Configuration::getBool("XmlConfigLoader.updateFiles") == true) {
		Version latestVersion = xmlConfigLoader.getLatestVersion();
		std::auto_ptr<XmlDocument> originalDocument(XmlDocument::loadXmlDocument(fileName));
		Version currentVersion;
		if (originalDocument->getRootElement()->hasAttribute("version")) {
			currentVersion = originalDocument->getRootElement()->getAttributeValue("version");
		} // if
		if (currentVersion < latestVersion) {
			std::auto_ptr<const XmlDocument> temp(xmlConfigLoader.loadConfiguration(fileName));
		} // if
	} // if

	return success;
} // loadConfig

void Configuration::printConfig() {
	std::map<std::string, std::string>::iterator it;

	printd(INFO, "--[Configuration::printConfig]-------------------------------------\n");
	printGeneralConfig(&rootEntry);
	printd(INFO, "Root-Path: %s\n", rootPath.c_str());
	for (it = pathMap.begin(); it != pathMap.end(); ++it) {
		printd(INFO, "%s-Path: %s\n", (*it).first.c_str(), (*it).second.c_str());
	} // for
	printd(INFO, "--[Configuration::printConfig end]---------------------------------\n");
} // printConfig

bool Configuration::containsPath(std::string name)
{
	return pathMap.find(name) != pathMap.end();
}

std::string Configuration::getPath(const std::string& name)
{
	std::string path = rootPath;
	const std::map<std::string, std::string>::iterator entry = pathMap.find(name);
	if (entry == pathMap.end() || entry->second.empty())
		printd(WARNING, "Configuration::getPath(): WARNING: Path with name %s has empty value!!!\n", name.c_str());
	else
		path.append(entry->second);

	return path;
}


bool Configuration::contains(std::string key) {
	std::string attributeName;
	ConfigurationEntry* entry = getConfigEntry(key, attributeName, false);
	if (!entry)
		return false;
	if (entry->attributes.find(attributeName) == entry->attributes.end())
		return false;
	return true;
} // contains

bool Configuration::getBool(std::string key) {
	std::string value;
	std::string attributeName;
	ConfigurationEntry* entry = getConfigEntry(key, attributeName);

	if (!entry || entry->attributes.find(attributeName) == entry->attributes.end()) {
		printd(ERROR, "Configuration::getBool(): could not find the attribute %s in %s!\n",
				attributeName.c_str(), key.c_str());
		return false;
	}
	value = entry->attributes[attributeName];
	if (value == "TRUE" || value == "true" || value == "True")
		return true;
	else if (value == "FALSE" || value == "false" || value == "False")
		return false;
	else
		printd(
				ERROR,
				"Configuration::getBool(): ERROR: could not find boolean member %s of element %s or the value %s is not a valid bool-value!\n",
				attributeName.c_str(), entry->nodeName.c_str(), value.c_str());

	return false;
} // getBool

int Configuration::getInt(std::string key) {
	std::string value;
	std::string attributeName;
	ConfigurationEntry* entry = getConfigEntry(key, attributeName);

	if (!entry || entry->attributes.find(attributeName) == entry->attributes.end()) {
		printd(ERROR, "Configuration::getInt(): could not find the attribute %s in %s!\n",
				attributeName.c_str(), key.c_str());
		return 0;
	}
	value = entry->attributes[attributeName];
	return atoi(value.c_str());
} // getInt

float Configuration::getFloat(std::string key) {
	std::string value;
	std::string attributeName;
	ConfigurationEntry* entry = getConfigEntry(key, attributeName);

	if (!entry || entry->attributes.find(attributeName) == entry->attributes.end()) {
		printd(ERROR, "Configuration::getFloat(): could not find the attribute %s in %s!\n",
				attributeName.c_str(), key.c_str());
		return 0.0f;
	}
	value = entry->attributes[attributeName];
	return atof(value.c_str());
} // getFloat

std::string Configuration::getString(std::string key) {
	std::string value;
	std::string attributeName;
	ConfigurationEntry* entry = getConfigEntry(key, attributeName);

	if (!entry || entry->attributes.find(attributeName) == entry->attributes.end()) {
		printd(ERROR, "Configuration::getString(): could not find the attribute %s in %s!\n",
				attributeName.c_str(), key.c_str());
		return "";
	}
	return entry->attributes[attributeName];
} // getString

void Configuration::printGeneralConfig(ConfigurationEntry* currentEntry) {
	char output[1024];
	output[0] = '\0';
	std::map<std::string, ConfigurationEntry>::iterator itElements;
	std::map<std::string, std::string>::iterator itAttributes;

	printd(INFO, "Element: %s\n", currentEntry->nodeName.c_str());
	for (itAttributes = currentEntry->attributes.begin(); itAttributes
			!= currentEntry->attributes.end(); ++itAttributes) {
		printd(INFO, "  - [%s] = %s\n", (*itAttributes).first.c_str(),
				(*itAttributes).second.c_str());
	} // for

	if (!currentEntry->subNodes.empty()) {
		strcat(output, "  - subElements: ");
		for (itElements = currentEntry->subNodes.begin(); itElements
				!= currentEntry->subNodes.end(); ++itElements) {
			sprintf(output, "%s%s ", output, (*itElements).first.c_str());
		} // for
		strcat(output, "\n");
	} // if
	strcat(output, "\n");
	printd(INFO, output);

	for (itElements = currentEntry->subNodes.begin(); itElements != currentEntry->subNodes.end(); ++itElements)
		printGeneralConfig(&((*itElements).second));

} // printGeneralConfig

Configuration::ConfigurationEntry* Configuration::getConfigEntry(std::string key,
		std::string &attributeName, bool dumpError) {
	int startPos, newPos;
	std::string nodeName;

	ConfigurationEntry* currentEntry = &rootEntry;
	ConfigurationEntry* newEntry;

	startPos = 0;

	do {
		newPos = key.find('.', startPos);
		if (newPos != (int)std::string::npos) {
			nodeName = key.substr(startPos, (newPos - startPos));
			if (currentEntry->subNodes.find(nodeName) != currentEntry->subNodes.end()) {
				newEntry = &(currentEntry->subNodes[nodeName]);
			} // if
			else {
				if (dumpError) {
					printd(ERROR,
						"Configuration::getConfigEntry(): ERROR: sub-element %s not found in element %s!\n",
						nodeName.c_str(), currentEntry->nodeName.c_str());
				} // if
				return NULL;
			} // else
			currentEntry = newEntry;
			startPos = newPos + 1;
		} // if
		else {
			attributeName = key.substr(startPos);
		} // else
	} while (newPos != (int)std::string::npos);
	return currentEntry;
} // getConfigEntry

//*****************************************************************************
// Configuration loading
//*****************************************************************************

bool Configuration::parseGeneralConfig(const XmlDocument *document) {
	const XmlElement* generalElement = document->getElement("generalConfig.general");
	if (!generalElement) {
		printd(ERROR,
				"Configuration::loadConfig(): unable to find node <general> in configuration file!\n");
		return false;
	} // if

	rootEntry.nodeName = generalElement->getName();
	return parseGeneralConfigRecursive(generalElement, rootEntry);
} // parseGeneralConfig

bool Configuration::parseGeneralConfigRecursive(const XmlElement* element,
		ConfigurationEntry& currentEntry) {
	const XmlElement* subElement = NULL;
	std::vector<const XmlElement*> subElements;
	std::vector<const XmlElement*>::iterator seIt;
	bool success = true;
	std::string key;
	std::string value;
	std::string name;

	if (element->hasSubElement("option")) {
		subElements = element->getSubElements("option");
		for (seIt = subElements.begin(); seIt != subElements.end(); ++seIt) {
			subElement = *seIt;
			if (!subElement->hasAttribute("key") || !subElement->hasAttribute("value")) {
				printd(ERROR,
						"Configuration::loadGeneralConfig(): <option> element without key and/or value attribute in node %s found! Please check your configuration!\n",
						currentEntry.nodeName.c_str());
				success = false;
				break;
			} // if
			key = subElement->getAttributeValue("key");
			value = subElement->getAttributeValue("value");
			if (currentEntry.attributes.find(key) !=
				currentEntry.attributes.end()) {
				printd(WARNING,
						"Configuration::loadConfig(): Option %s.%s already registered! Previous value (%s) will be replaced with new value (%s)!\n",
						currentEntry.nodeName.c_str(), key.c_str(),
						currentEntry.attributes[key].c_str(), value.c_str());
			} // if
			currentEntry.attributes[key] = value;
		} // for
	} // if
	else if (element->hasSubElements()) {
		subElements = element->getAllSubElements();
		for (seIt = subElements.begin(); seIt != subElements.end(); ++seIt) {
			subElement = *seIt;
			name = subElement->getName();
			ConfigurationEntry newEntry;
			if (currentEntry.subNodes.find(name) != currentEntry.subNodes.end()) {
				newEntry = currentEntry.subNodes[name];
			} // if
			else {
				newEntry.nodeName = name;
			} // else
			success = parseGeneralConfigRecursive(subElement, newEntry) && success;
			currentEntry.subNodes[name] = newEntry;
		} // for
	} // else if
	return success;
} // parseGeneralConfigRecursive

bool Configuration::parsePathConfig(const XmlDocument *document) {
	char pathEnding;
	std::string path, pathName;

	// check for root path
	if (!document->hasElement("generalConfig.paths.root")) {
		printd(ERROR,
				"Configuration::loadConfig(): Missing node <root> in paths section found! Please check your configuration!\n");
		return false;
	} // if
	if (!document->hasAttribute("generalConfig.paths.root.directory")) {
		printd(ERROR,
				"Configuration::loadConfig(): Missing directory attribute in node <root> found! Please check your configuration!\n");
		return false;
	} // if
	rootPath = document->getAttributeValue("generalConfig.paths.root.directory");
	if (!rootPath.empty())
	{
		pathEnding = rootPath.at(rootPath.size() - 1);
		if (pathEnding != '/' && pathEnding != '\\')
			rootPath.push_back('/');
	}
	// parse all paths
	bool success = true;
	std::vector<const XmlElement*> paths = document->getElements("generalConfig.paths.path");
	std::vector<const XmlElement*>::iterator pathIt;
	const XmlElement* pathElement = NULL;
	for (pathIt = paths.begin(); pathIt != paths.end(); ++pathIt) {
		pathElement = *pathIt;
		if (!pathElement->hasAttribute("name")) {
			printd(ERROR,
					"Configuration::loadConfig(): Found path-entry without 'name' attribute!\n");
			success = false;
			break;
		} // if
		pathName = pathElement->getAttributeValue("name");
		if (!pathElement->hasAttribute("directory")) {
			printd(ERROR,
					"Configuration::loadConfig(): Found path-entry for name %s without 'directory' attribute!\n",
					pathName.c_str());
			success = false;
			break;
		} // if
		path = pathElement->getAttributeValue("directory");
		if (!path.empty())
		{
			pathEnding = path.at(path.size() - 1);
			if (pathEnding != '/' && pathEnding != '\\')
				path.push_back('/');
		}
		pathMap[pathName] = path;
	} // for
	return success;
} // parsePathConfig

Configuration::ConverterToV1_0a4::ConverterToV1_0a4() :
	XmlConfigurationConverter("0.0", "1.0a4") {
} // ConverterToV1_0a4

bool Configuration::ConverterToV1_0a4::convert(XmlDocument* document, const Version& version,
		std::string configFile) {
	document->getRootElement()->setName("generalConfig");
	if (document->hasAttribute("generalConfig.version")) {
		document->getAttribute("generalConfig.version")->setValue("1.0a4");
	} // if
	else {
		XmlAttribute* versionAttribute = new XmlAttribute("version", "1.0a4");
		document->getRootElement()->addAttribute(versionAttribute);
	} // else
	document->renameAttributes("generalConfig.paths.root.path", "directory");
	document->renameAttributes("generalConfig.paths.path.path", "directory");

	XmlDtdReference* dtdRef = document->getDtd();
	if (!dtdRef) {
		dtdRef = new XmlDtdReference("generalConfig",
				XmlConfigurationLoader::XmlDtdUrl + "generalConfig_v1.0a4.dtd");
		document->setDtd(dtdRef);
	} else {
		dtdRef->setName("generalConfig");
		dtdRef->setDtd(XmlConfigurationLoader::XmlDtdUrl + "generalConfig_v1.0a4.dtd");
	} // else

	// rename EntityType configuration entry
	document->replaceAttributeValues("generalConfig.paths.path.name", "EntityConfiguration",
			"EntityTypeConfiguration");

	// rename ControllerManager configuration entry
	document->replaceAttributeValues("generalConfig.paths.path.name", "ControllerConfiguration",
			"ControllerManagerConfiguration");

	// rename CursorTransformationModel configuration entry
	document->replaceAttributeValues("generalConfig.paths.path.name", "CursorModelConfiguration",
			"CursorTransformationModelConfiguration");

	// rename NetworkModule configuration entry
	document->replaceAttributeValues("generalConfig.paths.path.name", "NetworkConfiguration",
			"NetworkModuleConfiguration");

	// rename NetworkModule configuration entry
	document->replaceAttributeValues("generalConfig.paths.path.name", "NavigationConfiguration",
			"NavigationModuleConfiguration");

	return true;
} // convert
