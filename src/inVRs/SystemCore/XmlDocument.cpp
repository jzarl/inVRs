/*
 * XmlDocument.cpp
 *
 *  Created on: Jun 24, 2009
 *      Author: rlander
 */

#include "XmlDocument.h"

#include <memory>
#include <sstream>

#include "XMLTools.h"
#include "DebugOutput.h"

// disable deprecation warning for std::auto_ptr when std::unique_ptr is not available.
#ifndef HAS_CXX11_UNIQUE_PTR
// Only do this on GCC versions that support it:
# if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 402
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
#endif

XmlDocument* XmlDocument::loadXmlDocument(std::string file) {
	bool success = true;
	bool finished = false;
	XmlDocument* document = NULL;
	XmlElement* currentElement = NULL;
	XmlElement* element = NULL;
	XmlAttribute* attribute = NULL;
	std::string attributeName;
	std::string attributeValue;
	std::string documentDtd = "";

	if (!fileExists(file)) {
		printd(ERROR, "XmlDocument::loadXmlDocument(): ERROR: could not find config-file %s\n",
				file.c_str());
		return NULL;
	} // if

	std::auto_ptr<IrrXMLReader> xml(createIrrXMLReader(file.c_str()));
	if (!xml.get()) {
		printd(ERROR, "XmlDocument::loadXmlDocument(): ERROR: Config file %s not found!\n",
				file.c_str());
		return NULL;
	} // if

	document = new XmlDocument();

	while (!finished && xml->read()) {
//		printf("(%i): %s\n", xml->getNodeType(), xml->getNodeData());
		switch (xml->getNodeType()) {

		case EXN_ELEMENT:
			// check if node has a name
			if (!xml->getNodeName()) {
				printd(ERROR, "XmlDocument::loadXmlDocument(): empty node <> found in file %s!\n",
						file.c_str());
				success = false;
				finished = true;
				break;
			} // if
			element = new XmlElement(xml->getNodeName());

			// parse all attributes
			for (int i=0; i < xml->getAttributeCount(); i++) {
				if (element->hasAttribute(xml->getAttributeName(i))) {
					printd(ERROR, "XmlDocument::loadXmlDocument(): duplicate attribute %s in element <%s> found! Please fix your config file %s!\n",
							xml->getAttributeName(i), xml->getNodeName(), file.c_str());
					delete element;
					success = false;
					finished = true;
					break;
				} // if
				attribute = new XmlAttribute(xml->getAttributeName(i), xml->getAttributeValue(i));
				element->addAttribute(attribute);
			} // for

			// add element to tree
			if (currentElement) {
				currentElement->addSubElement(element);
			} // if
			else {
				// check for sub elements of root because parser has the problem that the last
				// element is paresed two times, so an empty root node would cause the following
				// error
				if (document->rootElement != NULL && document->rootElement->hasSubElements()) {
					printd(ERROR, "XmlDocument::loadXmlDocument(): more than one node at root level found (<%s> and <%s>). Please fix your config file %s!\n",
							document->rootElement->getName().c_str(), element->getName().c_str(), file.c_str());
					delete element;
					success = false;
					finished = true;
					break;
				} // if
				document->rootElement = element;
			} // if

			if (!xml->isEmptyElement())
				currentElement = element;
			break;

		case EXN_CDATA:
			// parse content of element which is CDDATA thing
			if (currentElement) {
				std::string content = xml->getNodeData();
				if (content.size() > 0 && currentElement->hasContent()) {
					printd(WARNING,
							"XmlDocument::loadXmlDocument(): duplicate content in element <%s> found! Second content %s will be ignored!\n",
							currentElement->getName().c_str(), content.c_str());
				} // if
				else if (content.size() > 0) {
					currentElement->setContent(content);
				} // else if
			} // if
			break;

		case EXN_TEXT:
			// parse content of element
			if (currentElement) {
				std::string content = xml->getNodeData();
				// remove spaces/tabs/newlines at beginning of each line
				bool startOfLine = true;
				std::string::iterator it = content.begin();
				while  (it != content.end()) {
					if (startOfLine && (*it == ' ' || *it == '\t' || *it == '\n' || *it == '\r')) {
						content.erase(it);
					} // if
					else {
						if (startOfLine) {
							startOfLine = false;
						} // else if
						else if (*it == '\n') {
							startOfLine = true;
						} // else if
						++it;
					} // else
				} // while
				// remove newline at end of content string
				if (content.size() > 0 && content[content.size()-1] == '\n') {
					content = content.substr(0, content.size() - 1);
				} // if

				if (content.size() > 0 && currentElement->hasContent()) {
					printd(WARNING,
							"XmlDocument::loadXmlDocument(): duplicate content in element <%s> found! Second content %s will be ignored!\n",
							currentElement->getName().c_str(), content.c_str());
				} // if
				else if (content.size() > 0) {
					currentElement->setContent(content);
				} // else if
			} // if
			break;

		case EXN_ELEMENT_END:
			if (currentElement) {
				std::string nodeName = xml->getNodeName();
				if (currentElement->getName() != nodeName) {
					printd(ERROR,
							"XmlDocument::loadXmlDocument(): invalid end tag </%s> found - expected end tag </%s>! Please fix config file %s!\n",
							nodeName.c_str(), currentElement->getName().c_str(), file.c_str());
					success = false;
					finished = true;
					break;
				} // if
				currentElement = currentElement->getParentElement();
			} // if
			else if (!document->rootElement || document->rootElement->getName() != xml->getNodeName()) {
				printd(ERROR, "XmlDocument::loadXmlDocument(): Unexpected end element %s found! Please fix config file %s!\n",
						xml->getNodeName(), file.c_str());
				success = false;
				finished = true;
				break;
			} // else
			break;

		case EXN_COMMENT:
			// first two characters of <!DOCTYPE are interpreted as comment
			if (documentDtd.size() == 0 && !strncmp("CTYPE", xml->getNodeData(), 5)) {
				documentDtd = xml->getNodeData();
			} // if
			break;

		default:
			break;
		} // switch
	} // while

	if (!document->rootElement) {
		printd(ERROR, "XmlDocument::loadXmlDocument(): Cannot read empty config file %s!\n",
				file.c_str());
		success = false;
	} // if

	if (documentDtd.size() > 0) {
		parseDocumentDtdAndEntities(documentDtd, document);
	} // if

	if (!success) {
		printd(ERROR, "XmlDocument::loadXmlDocument(): Error loading XML file %s!\n", file.c_str());
		delete document;
		return NULL;
	} // if

	return document;
} // loadXmlDocument

XmlDocument* XmlDocument::createXmlDocument(XmlElement* element) {
	XmlDocument* document = new XmlDocument;
	document->rootElement = element;
	return document;
} // createXmlDocument

XmlDocument::~XmlDocument() {
	if (rootElement)
		delete rootElement;
	if (documentDtd)
		delete documentDtd;

	std::vector<XmlDtdReference*>::iterator dtdIt;
	for (dtdIt = entityDtds.begin(); dtdIt != entityDtds.end(); ++dtdIt) {
		delete *dtdIt;
	}
	entityDtds.clear();
} // ~XmlDocument

const XmlElement* XmlDocument::getRootElement() const {
	return rootElement;
} // getRootNode

XmlElement* XmlDocument::getRootElement() {
	return rootElement;
} // getRootElement

bool XmlDocument::hasAttribute(std::string attributeName) const {
	return (getAttribute(attributeName) != NULL);
} // hasAttribute

bool XmlDocument::hasElement(std::string elementName) const {
	return (getElement(elementName) != NULL);
} // hasElement

std::string XmlDocument::getAttributeValue(std::string attributeName) const {
	std::string result = "";
	const XmlAttribute* attribute = ((const XmlDocument*)this)->getAttribute(attributeName);
	if (attribute)
		result = attribute->getValue();
	return result;
} // getAttributeValue

float XmlDocument::getAttributeValueAsFloat(std::string attributeName) const {
	std::string value = getAttributeValue(attributeName);
	return (float)atof(value.c_str());
} // getAttributeValueAsFloat

int XmlDocument::getAttributeValueAsInt(std::string attributeName) const {
	std::string value = getAttributeValue(attributeName);
	return (int)atoi(value.c_str());
} // getAttributeValueAsInt

const XmlAttribute* XmlDocument::getAttribute(std::string attributeName) const {
	if (!rootElement)
		return NULL;

	size_t firstDotIndex = attributeName.find_first_of('.');

	// if no element is passed then no attribute can be found
	if (firstDotIndex == std::string::npos)
		return NULL;

	std::string elementName = attributeName.substr(0, firstDotIndex);
	std::string newAttributeName = attributeName.substr(firstDotIndex+1);

	if (elementName != rootElement->getName())
		return NULL;

	return rootElement->getAttribute(newAttributeName);
} // getAttribute

XmlAttribute* XmlDocument::getAttribute(std::string attributeName) {
	// call const method and cast to non const result
	return const_cast<XmlAttribute*>(
			((const XmlDocument*)(this))->getAttribute(attributeName));
} // getAttribute

std::vector<const XmlAttribute*> XmlDocument::getAttributes(std::string attributeName) const {
	std::vector<const XmlAttribute*> result;

	if (!rootElement)
		return result;

	size_t firstDotIndex = attributeName.find_first_of('.');

	// if no element is passed then no attribute can be found
	if (firstDotIndex == std::string::npos)
		return result;

	std::string elementName = attributeName.substr(0, firstDotIndex);
	std::string newAttributeName = attributeName.substr(firstDotIndex+1);

	if (elementName != rootElement->getName())
		return result;

	return ((const XmlElement*)rootElement)->getAttributes(newAttributeName);
} // getAttributes

std::vector<XmlAttribute*> XmlDocument::getAttributes(std::string attributeName) {
	std::vector<XmlAttribute*> result;
	std::vector<const XmlAttribute*> foundAttributes;
	std::vector<const XmlAttribute*>::iterator attIt;

	// call const method and cast to non const result
	foundAttributes = ((const XmlDocument*)(this))->getAttributes(attributeName);
	for (attIt = foundAttributes.begin(); attIt != foundAttributes.end(); ++attIt) {
		result.push_back(const_cast<XmlAttribute*>(*attIt));
	} // for
	return result;
} // getAttributes

void XmlDocument::renameAttributes(std::string originalAttributeName, std::string newAttributeName) {
	std::vector<XmlAttribute*> attributes = getAttributes(originalAttributeName);
	std::vector<XmlAttribute*>::iterator attIt;
	for (attIt = attributes.begin(); attIt != attributes.end(); ++attIt) {
		(*attIt)->setKey(newAttributeName);
	} // for
} // renameAttributes

void XmlDocument::replaceAttributeValues(std::string attributeName, std::string oldValue,
		std::string newValue) {
	std::vector<XmlAttribute*> attributes = getAttributes(attributeName);
	std::vector<XmlAttribute*>::iterator it;
	for (it = attributes.begin(); it != attributes.end(); ++it) {
		if ((*it)->getValue() == oldValue) {
			(*it)->setValue(newValue);
		} // if
	} // for
} // replaceAttributeValues

const XmlElement* XmlDocument::getElement(std::string elementName) const {
	if (!rootElement)
		return NULL;

	if (elementName == rootElement->getName())
		return rootElement;

	size_t firstDotIndex = elementName.find_first_of('.');
	std::string firstElementName = elementName.substr(0, firstDotIndex);

	if (firstElementName != rootElement->getName())
		return NULL;

	std::string remainingElementName = elementName.substr(firstDotIndex+1);

	return rootElement->getSubElement(remainingElementName);
} // getElement

XmlElement* XmlDocument::getElement(std::string elementName) {
	// call const method and cast to non const result
	return const_cast<XmlElement*>(
			((const XmlDocument*)(this))->getElement(elementName));
} // getElement

std::vector<const XmlElement*> XmlDocument::getElements(std::string elementName) const {
	std::vector<const XmlElement*> result;

	if (!rootElement)
		return result;

	if (elementName == rootElement->getName()) {
		result.push_back(rootElement);
		return result;
	} // if

	size_t firstDotIndex = elementName.find_first_of('.');
	std::string firstElementName = elementName.substr(0, firstDotIndex);

	if (firstElementName != rootElement->getName())
		return result;

	std::string remainingElementName = elementName.substr(firstDotIndex+1);

	//TODO: check if the const method of rootElement is called!!!
	return ((const XmlElement*)rootElement)->getSubElements(remainingElementName);
} // getElements

std::vector<XmlElement*> XmlDocument::getElements(std::string elementName) {
	std::vector<XmlElement*> result;
	std::vector<const XmlElement*> foundElements;
	std::vector<const XmlElement*>::iterator it;

	// call const method and cast to non const result
	foundElements = ((const XmlDocument*)(this))->getElements(elementName);
	for (it = foundElements.begin(); it != foundElements.end(); ++it) {
		result.push_back(const_cast<XmlElement*>(*it));
	} // for
	return result;
} // getElements

void XmlDocument::renameElements(std::string originalElementName, std::string newElementName) {
	std::vector<XmlElement*> elements = getElements(originalElementName);
	std::vector<XmlElement*>::iterator it;
	for (it = elements.begin(); it != elements.end(); ++it) {
		(*it)->setName(newElementName);
	} // for
} // renameElements

std::string XmlDocument::getElementContent(std::string elementName) const {
	std::string result = "";

	const XmlElement* element = getElement(elementName);
	if (element)
		result = element->getContent();

	return result;
} // getElementContent

XmlElement* XmlDocument::replaceRootElement(XmlElement* element) {
	XmlElement* oldRootElement = rootElement;
	rootElement = element;
	return oldRootElement;
} // setRootElement

const XmlDtdReference* XmlDocument::getDtd() const {
	return documentDtd;
} // getDtd

XmlDtdReference* XmlDocument::getDtd() {
	return documentDtd;
} // getDtd

void XmlDocument::setDtd(XmlDtdReference* dtd) {
	if (this->documentDtd) {
		delete this->documentDtd;
	} // if
	this->documentDtd = dtd;
} // setDtd

void XmlDocument::addEntityDtd(XmlDtdReference* entityDtd) {
	XmlDtdReference* oldDtd = getEntityDtd(entityDtd->getName());
	if (oldDtd) {
		removeEntityDtd(oldDtd);
		delete oldDtd;
	} // if
	entityDtds.push_back(entityDtd);
} // addEntityDtd

void XmlDocument::removeEntityDtd(XmlDtdReference* entityDtd) {
	std::vector<XmlDtdReference*>::iterator it;
	for (it = entityDtds.begin(); it != entityDtds.end(); ++it) {
		if (*it == entityDtd) {
			entityDtds.erase(it);
			break;
		} // if
	} // for
} // addEntityDtd

std::vector<XmlDtdReference*> XmlDocument::getEntityDtds() {
	return entityDtds;
} // getEntityDtds

XmlDtdReference* XmlDocument::getEntityDtd(std::string name) {
	XmlDtdReference* result = NULL;

	std::vector<XmlDtdReference*>::iterator it;
	for (it = entityDtds.begin(); it != entityDtds.end(); ++it) {
		if ((*it)->getName() == name) {
			result = *it;
			break;
		} // if
	} // for
	return result;
} // getEntityDtd

void XmlDocument::fixEntityDtdOrder(const std::vector<std::string>& entityNames) {
	std::vector<XmlDtdReference*>::iterator dtdIt;
	std::vector<std::string>::const_iterator dtdNameIt;
	std::vector<XmlDtdReference*> newEntityDtdList;

	// iterate over all passed entity names
	for (dtdNameIt = entityNames.begin(); dtdNameIt != entityNames.end(); ++dtdNameIt) {
		// find all entity dtds with the current name and add them in the
		// new entitydtd list
		dtdIt = entityDtds.begin();
		while (dtdIt != entityDtds.end()) {
			if ((*dtdIt)->getName() == *dtdNameIt) {
				newEntityDtdList.push_back(*dtdIt);
				dtdIt = entityDtds.erase(dtdIt);
			} // if
			else {
				++dtdIt;
			} // else
		} // while
	} // for

	// finally move all subelements which were not moved over to the new vector
	for (dtdIt = entityDtds.begin(); dtdIt != entityDtds.end(); ++dtdIt) {
		newEntityDtdList.push_back(*dtdIt);
	} // for

	// swap the new list with the stored one to get the new sorted list
	entityDtds.swap(newEntityDtdList);
} // fixEntityDtdOrder

void XmlDocument::dump() const {
	if (rootElement) {
		printDocumentHeader(stdout);
		rootElement->dumpToFile(stdout);
	} // if
} // dump

bool XmlDocument::dumpToFile(std::string fileName) const {
	bool success = true;
	if (rootElement) {
		FILE* file = fopen(fileName.c_str(), "w");
		if (!file) {
			printd(ERROR, "XmlDocument::dumpToFile(): unable to open file %s!\n", fileName.c_str());
			success = false;
		} else {
			printDocumentHeader(file);
			rootElement->dumpToFile(file);
			fclose(file);
		} // else
	} // if
	return success;
} // dumpToFile

XmlDocument::XmlDocument() :
	rootElement(NULL),
	documentDtd(NULL) {
} // XmlDocument

void XmlDocument::printDocumentHeader(FILE* file) const {
	std::vector<XmlDtdReference*>::const_iterator it;
	fprintf(file, "<?xml version=\"1.0\"?>\n");
	if (documentDtd != NULL) {
		fprintf(file, "<!DOCTYPE %s SYSTEM \"%s\"%s\n", documentDtd->getName().c_str(),
				documentDtd->getDtd().c_str(), (entityDtds.size() == 0) ? ">" : "\n[");
		for (it = entityDtds.begin(); it != entityDtds.end(); ++it) {
			fprintf(file, "<!ENTITY %% %s SYSTEM \"%s\">\n", (*it)->getName().c_str(), (*it)->getDtd().c_str());
			fprintf(file, "%%%s;\n", (*it)->getName().c_str());
		} // for
		if (entityDtds.size() > 0) {
			fprintf(file, "]>\n");
		} // if
	} // if
} // printDocumentHeader

void XmlDocument::parseDocumentDtdAndEntities(std::string definition, XmlDocument* document) {
	std::stringstream stream(definition);
	std::string temp;
	std::string entityName;
	std::string entityDtd;

	// (DO)CTYPE, root node, SYSTEM
	stream >> temp;
	if (temp != "CTYPE")
		return;
	stream >> temp;
	stream >> temp;
	if (temp != "SYSTEM")
		return;

	// dtd
	stream >> temp;
	// remove starting "
	if (temp[0] == '\"')
		temp = temp.substr(1);
	// remove ending "
	if (temp[temp.size()-1] == '\"')
		temp = temp.substr(0, temp.size()-1);
	// add d for dtd if missing (can happen because parse expects -->)
	if (temp[temp.size()-1] == 't' && temp[temp.size()-2] == 'd' && temp[temp.size()-3] == '.')
		temp += "d";

//	printf("Setting DTD: %s\n", temp.c_str());
	XmlDtdReference* dtdRef =
		new XmlDtdReference(document->getRootElement()->getName(), temp);
	document->setDtd(dtdRef);

	while (!stream.eof()) {
		stream >> temp;
		if (temp == "<!ENTITY") {
			// %
			stream >> temp;
			// entity name
			stream >> entityName;
			// SYSTEM
			stream >> temp;
			if (temp != "SYSTEM")
				continue;

			// entity dtd
			stream >> entityDtd;

			// remove starting "
			if (entityDtd[0] == '\"')
				entityDtd = entityDtd.substr(1);
			// remove ending >
			if (entityDtd[entityDtd.size()-1] == '>')
				entityDtd = entityDtd.substr(0, entityDtd.size()-1);
			// remove ending "
			if (entityDtd[entityDtd.size()-1] == '\"')
				entityDtd = entityDtd.substr(0, entityDtd.size()-1);

//			printf("Setting Entity: %s %s\n", entityName.c_str(), entityDtd.c_str());

			dtdRef = new XmlDtdReference(entityName, entityDtd);
			document->addEntityDtd(dtdRef);
		} // if
	} // while

} // parseDocumentDtdAndEntities

