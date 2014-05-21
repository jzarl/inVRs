/*
 * XmlElement.cpp
 *
 *  Created on: Jun 24, 2009
 *      Author: rlander
 */

#include "XmlElement.h"

#include "DebugOutput.h"

#include <stdlib.h>
#include <algorithm>

XmlElement::XmlElement(std::string name) :
	elementName(name),
	content(""),
	parentElement(NULL) {

} // XmlElement


XmlElement::~XmlElement() {
	std::vector<XmlElement*>::iterator seIt;
	std::vector<XmlAttribute*>::iterator attIt;

	for (seIt = subElements.begin(); seIt != subElements.end(); ++seIt) {
		delete *seIt;
	} // for
	subElements.clear();

	for (attIt = attributes.begin(); attIt != attributes.end(); ++attIt) {
		delete *attIt;
	} // for
	attributes.clear();

	parentElement = NULL;
} // ~XmlElement

std::string XmlElement::getName() const {
	return elementName;
} // getName

bool XmlElement::hasContent() const {
	return (content.size() > 0);
} // hasContent

bool XmlElement::hasAttribute(std::string attributeName) const {
	return (getAttribute(attributeName) != NULL);
} // hasAttribute

bool XmlElement::hasParentElement() const {
	return (parentElement != NULL);
} // hasParentElement

bool XmlElement::hasSubElements() const {
	return (subElements.size() > 0);
} // hasSubElements

bool XmlElement::hasSubElement(std::string subElementName) const {
	return (getFirstSubElement(subElementName) != NULL);
} // hasSubElement

std::string XmlElement::getContent() const {
	return content;
} // getContent

std::string XmlElement::getAttributeValue(std::string attributeName) const {
	std::string result = "";

	const XmlAttribute* attribute = getAttribute(attributeName);
	if (attribute)
		result = attribute->getValue();

	return result;
} // getAttributeValue

float XmlElement::getAttributeValueAsFloat(std::string attributeName) const {
	std::string value = getAttributeValue(attributeName);
	return (float)(atof(value.c_str()));
} // getAttributeValueAsFloat

int XmlElement::getAttributeValueAsInt(std::string attributeName) const {
	std::string value = getAttributeValue(attributeName);
	return (int)(atoi(value.c_str()));
} // getAttributeValueAsInt

bool XmlElement::getAttributeValueAsBool(std::string attributeName) const {
	std::string value = getAttributeValue(attributeName);
	std::transform(value.begin(), value.end(),value.begin(), ::toupper);

	return value == "TRUE" || value == "1" || value == "T";
} // getAttributeValueAsInt

const XmlAttribute* XmlElement::getAttribute(std::string attributeName) const {
	return getAttributesInternal(attributeName);
} // getAttribute

XmlAttribute* XmlElement::getAttribute(std::string attributeName) {
	return const_cast<XmlAttribute*>(
			((const XmlElement*)(this))->getAttribute(attributeName));
} // getAttribute

std::vector<const XmlAttribute*> XmlElement::getAttributes(std::string attributeName) const {
	std::vector<const XmlAttribute*> result;
	getAttributesInternal(attributeName, &result);
	return result;
} // getAttributes

void XmlElement::renameAttributes(std::string originalAttributeName, std::string newAttributeName) {
	std::vector<XmlAttribute*> attributes = getAttributes(originalAttributeName);
	std::vector<XmlAttribute*>::iterator attIt;
	for (attIt = attributes.begin(); attIt != attributes.end(); ++attIt) {
		(*attIt)->setKey(newAttributeName);
	} // for
} // renameAttributes

void XmlElement::replaceAttributeValues(std::string attributeName, std::string oldValue,
		std::string newValue) {
	std::vector<XmlAttribute*> attributes = getAttributes(attributeName);
	std::vector<XmlAttribute*>::iterator it;
	for (it = attributes.begin(); it != attributes.end(); ++it) {
		if ((*it)->getValue() == oldValue) {
			(*it)->setValue(newValue);
		} // if
	} // for
} // replaceAttributeValues

std::vector<XmlAttribute*> XmlElement::getAttributes(std::string attributeName) {
	std::vector<XmlAttribute*> result;
	std::vector<const XmlAttribute*> foundAttributes;
	std::vector<const XmlAttribute*>::iterator attIt;

	// call const method and cast to non const result
	foundAttributes = ((const XmlElement*)(this))->getAttributes(attributeName);
	for (attIt = foundAttributes.begin(); attIt != foundAttributes.end(); ++attIt) {
		result.push_back(const_cast<XmlAttribute*>(*attIt));
	} // for
	return result;
} // getAttributes

const XmlElement* XmlElement::getFirstSubElement(std::string subElementName) const {
	return getSubElementsInternal(subElementName, true);
} // getFirstSubElement

XmlElement* XmlElement::getFirstSubElement(std::string subElementName) {
	// call const-version of method
	return const_cast<XmlElement*>(
			((const XmlElement*)this)->getFirstSubElement(subElementName));
} // getFirstSubElement

const XmlElement* XmlElement::getSubElement(std::string subElementName) const {
	return getSubElementsInternal(subElementName, false);
} // getSubElement

XmlElement* XmlElement::getSubElement(std::string subElementName) {
	// call const-version of method
	return const_cast<XmlElement*>(
			((const XmlElement*)this)->getSubElement(subElementName));
} // getSubElement

std::vector<const XmlElement*> XmlElement::getSubElements(std::string subElementName) const {
	std::vector<const XmlElement*> result;
	getSubElementsInternal(subElementName, false, &result);
	return result;
} // getSubElements

std::vector<XmlElement*> XmlElement::getSubElements(std::string subElementName) {
	std::vector<XmlElement*> result;
	std::vector<const XmlElement*> elements;
	std::vector<const XmlElement*>::iterator it;

	// call const method
	elements = ((const XmlElement*)this)->getSubElements(subElementName);
	for (it = elements.begin(); it != elements.end(); ++it) {
		result.push_back(const_cast<XmlElement*>(*it));
	} // for
	return result;
} // getSubElements

std::vector<const XmlElement*> XmlElement::getAllSubElements() const {
	std::vector<const XmlElement*> result;
	std::vector<XmlElement*>::const_iterator seIt;
	for (seIt = subElements.begin(); seIt != subElements.end(); ++seIt) {
		result.push_back(*seIt);
	} // for
	return result;
} // getSubElements

std::vector<XmlElement*> XmlElement::getAllSubElements() {
	return subElements;
} // getAllSubElements

void XmlElement::renameSubElements(std::string originalElementName, std::string newElementName) {
	std::vector<XmlElement*> elements = getSubElements(originalElementName);
	std::vector<XmlElement*>::iterator it;
	for (it = elements.begin(); it != elements.end(); ++it) {
		(*it)->setName(newElementName);
	} // for
} // renameElements

const XmlElement* XmlElement::getParentElement() const {
	return parentElement;
} // getParentElement

XmlElement* XmlElement::getParentElement() {
	return parentElement;
} // getParentElementPtr

void XmlElement::setName(std::string name) {
	elementName = name;
} // setName

void XmlElement::setContent(const std::string& content) {
	if (this->content.size() > 0) {
		printd(ERROR,
				"XmlElement::setContent(): Element %s has already some content stored! Previous content will be overwritten!\n",
				elementName.c_str());
	} // if
	this->content = content;
} // setContent

void XmlElement::addAttribute(XmlAttribute* attribute) {
	std::vector<XmlAttribute*>::iterator attIt;
	for (attIt = attributes.begin(); attIt != attributes.end(); ++attIt) {
		if ((*attIt)->getKey() == attribute->getKey()) {
			printd(ERROR,
					"XmlElement::setAttribute(): Attribute %s already existing in element %s! Previous value %s will be overwritten with value %s!\n",
					attribute->getKey().c_str(), elementName.c_str(),
					(*attIt)->getValue().c_str(), attribute->getValue().c_str());
			delete (*attIt);
			attributes.erase(attIt);
		} // if
	} // for
	attributes.push_back(attribute);
} // setAttribute

XmlAttribute* XmlElement::removeAttribute(std::string attributeName) {
	XmlAttribute* result = NULL;

	std::vector<XmlAttribute*>::iterator attIt;
	for (attIt = attributes.begin(); attIt != attributes.end(); ++attIt) {
		if ((*attIt)->getKey() == attributeName) {
			result = *attIt;
			attributes.erase(attIt);
			break;
		} // if
	} // for
	return result;
} // removeAttribute

void XmlElement::deleteAllAttributes() {
	std::vector<XmlAttribute*>::iterator attIt;
	for (attIt = attributes.begin(); attIt != attributes.end(); ++attIt) {
		delete *attIt;
	} // for
	attributes.clear();
} // deleteAllAttributes

void XmlElement::addSubElement(XmlElement* element) {
	subElements.push_back(element);
	element->parentElement = this;
} // addSubElement

XmlElement* XmlElement::removeSubElement(std::string elementName) {
	XmlElement* result = NULL;

	std::vector<XmlElement*>::iterator seIt;
	for (seIt = subElements.begin(); seIt != subElements.end(); ++seIt) {
		if ((*seIt)->getName() == elementName) {
			result = *seIt;
			subElements.erase(seIt);
			break;
		} // if
	} // for
	return result;
} // removeSubElement

bool XmlElement::removeSubElement(const XmlElement* element) {
	std::vector<XmlElement*>::iterator seIt;
	bool success = false;
	for (seIt = subElements.begin(); seIt != subElements.end(); ++seIt) {
		if ((*seIt) == element) {
			subElements.erase(seIt);
			success = true;
			break;
		} // if
	} // for
	return success;
} // removeSubElement;


void XmlElement::deleteAllSubElements() {
	std::vector<XmlElement*>::iterator seIt;
	for (seIt = subElements.begin(); seIt != subElements.end(); ++seIt) {
		delete *seIt;
	} // for
	subElements.clear();
} // deleteAllSubElements

void XmlElement::fixSubElementOrder(const std::vector<std::string>& subElementNames) {
	std::vector<XmlElement*>::iterator seIt;
	std::vector<std::string>::const_iterator seNameIt;
	std::vector<XmlElement*> newSubElementList;

	// iterate over all passed element names
	for (seNameIt = subElementNames.begin(); seNameIt != subElementNames.end(); ++seNameIt) {
		// find all elements with the current element name and add them in the
		// new subelement list
		seIt = subElements.begin();
		while (seIt != subElements.end()) {
			if ((*seIt)->getName() == *seNameIt) {
				newSubElementList.push_back(*seIt);
				seIt = subElements.erase(seIt);
			} // if
			else {
				++seIt;
			} // else
		} // while
	} // for

	// finally move all subelements which were not moved over to the new vector
	for (seIt = subElements.begin(); seIt != subElements.end(); ++seIt) {
		newSubElementList.push_back(*seIt);
	} // for

	// swap the new list with the stored one to get the new sorted list
	subElements.swap(newSubElementList);
} // fixSubElementOrder

void XmlElement::fixAttributeOrder(const std::vector<std::string>& attributeNames) {
	std::vector<XmlAttribute*>::iterator attIt;
	std::vector<std::string>::const_iterator attNameIt;
	std::vector<XmlAttribute*> newAttributeList;

	// iterate over all passed attribute names
	for (attNameIt = attributeNames.begin(); attNameIt != attributeNames.end(); ++attNameIt) {
		// find all attributes with the current attribute name and add them in the
		// new attribute list
		attIt = attributes.begin();
		while (attIt != attributes.end()) {
			if ((*attIt)->getKey() == *attNameIt) {
				newAttributeList.push_back(*attIt);
				attIt = attributes.erase(attIt);
			} // if
			else {
				++attIt;
			} // else
		} // while
	} // for

	// finally move all attributes which were not moved over to the new vector
	for (attIt = attributes.begin(); attIt != attributes.end(); ++attIt) {
		newAttributeList.push_back(*attIt);
	} // for

	// swap the new list with the stored one to get the new sorted list
	attributes.swap(newAttributeList);
} // fixAttributeOrder

void XmlElement::dump() const {
	dumpToFile(stdout);
} // dump

void XmlElement::dumpToFile(FILE* file, int depth) const {
	std::string text;
	std::vector<XmlElement*>::const_iterator seIt;
	std::vector<XmlAttribute*>::const_iterator attIt;

	std::string indention = "";
	for (int i=0; i < depth; i++) {
		indention += "\t";
	}

	text = indention + "<" + elementName;
	for (attIt = attributes.begin(); attIt != attributes.end(); ++attIt) {
		text += " " + (*attIt)->getKey() + "=\"" + (*attIt)->getValue() + "\"";
	} // for
	if (hasSubElements() || hasContent())
		text = text + ">";
	else
		text = text + "/>";
	fprintf(file, "%s\n", text.c_str());

	if (hasContent()) {
		fprintf(file, "%s\t%s\n", indention.c_str(), content.c_str());
	} // if

	for (seIt = subElements.begin(); seIt != subElements.end(); ++seIt) {
		(*seIt)->dumpToFile(file, depth+1);
	} // for
	if (hasSubElements() || hasContent())
		fprintf(file, "%s</%s>\n", indention.c_str(), elementName.c_str());
} // dumpToFile


XmlElement::XmlElement() :
	elementName(""),
	content(""),
	parentElement(NULL) {

} // XmlElement

const XmlElement* XmlElement::getSubElementsInternal(std::string subElementName, bool useFirst,
		std::vector<const XmlElement*>* elementList) const {

	// Search for sub element in local list first (could contain a dot in the element name)
	std::vector<XmlElement*>::const_iterator seIt;
	const XmlElement* result = NULL;
	for (seIt = subElements.begin(); seIt != subElements.end(); ++seIt) {
		if ((*seIt)->getName() == subElementName) {
			if (!result) {
				result = *seIt;
			} // if

			if (elementList) {
				elementList->push_back(*seIt);
			} // if
			else if (useFirst) {
				return result;
			} // if

			if (!elementList && !useFirst && result != *seIt) {
				printd(WARNING, "XmlElement::getSubElement(): Result for subnode %s is ambiguous - multiple matches in config file found! Only first one will be returned!\n",
						subElementName.c_str());
			} // if
		}// if
	} // for
	if (result)
		return result;

	size_t firstDotIndex = subElementName.find_first_of('.');
	std::string firstElementName = subElementName.substr(0, firstDotIndex);

	for (seIt = subElements.begin(); seIt != subElements.end(); ++seIt) {
		if ((*seIt)->getName() == firstElementName) {
			std::string remainingElementName = subElementName.substr(firstDotIndex+1);
			if (!result) {
				result = (*seIt)->getSubElementsInternal(remainingElementName, useFirst, elementList);
			} // if
			else if (elementList) {
				(*seIt)->getSubElementsInternal(remainingElementName, useFirst, elementList);
			} // else if
			else if (!useFirst) {
				printd(WARNING, "XmlElement::getSubElement(): Result for subnode %s is ambiguous - multiple matches in config file found! Only first one will be returned!\n",
						subElementName.c_str());
			} // else if

			if (!elementList && result && useFirst)
				return result;
		} // if
	} // for

	return result;
} // getSubElementInternal

const XmlAttribute* XmlElement::getAttributesInternal(std::string attributeName,
		std::vector<const XmlAttribute*>* attributeList) const {

	// Search for Attribute(s) in current element
	bool foundAttribute = false;
	std::vector<XmlAttribute*>::const_iterator attIt;
	for (attIt = attributes.begin(); attIt != attributes.end(); ++attIt) {
		if ((*attIt)->getKey() == attributeName) {
			foundAttribute = true;
			if (attributeList)
				attributeList->push_back(*attIt);
			else
				return (*attIt);
		} // if
	} // for
	if (foundAttribute) {
		return attributeList->at(0);
	} // if

	size_t firstDotIndex = attributeName.find_first_of('.');

	// if no element is passed then no attribute can be found
	if (firstDotIndex == std::string::npos)
		return NULL;

	std::string subElementName = attributeName.substr(0, firstDotIndex);
	std::string newAttributeName = attributeName.substr(firstDotIndex+1);

	const XmlAttribute* result = NULL;
	const XmlAttribute* attribute = NULL;

	std::vector<const XmlElement*> foundSubElements = getSubElements(subElementName);
	std::vector<const XmlElement*>::iterator seIt;
	for (seIt = foundSubElements.begin(); seIt != foundSubElements.end(); ++seIt) {
		attribute = (*seIt)->getAttributesInternal(newAttributeName, attributeList);
		if (!attributeList && attribute && result) {
			printd(WARNING, "XmlElement::getAttribute(): Result for attribute %s is ambiguous - multiple matches in config file found! Only first one will be returned!\n",
					attributeName.c_str());
		} // if
		else if (!attributeList && attribute) {
			result = attribute;
		} // else if
	} // for
	return result;
} // getAttributesInternal
