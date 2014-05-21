/*
 * XmlElement.h
 *
 *  Created on: Jun 24, 2009
 *      Author: rlander
 */

#ifndef XMLELEMENT_H_
#define XMLELEMENT_H_

#include "XmlAttribute.h"
#include "Platform.h"

#include <vector>
#include <string>
#include <map>

/**
 *
 */
class INVRS_SYSTEMCORE_API XmlElement {
public:

	/**
	 *
	 */
	XmlElement(std::string name);

	/**
	 *
	 */
	virtual ~XmlElement();

	/**
	 *
	 */
	std::string getName() const;

	/**
	 *
	 */
	bool hasContent() const;

	/**
	 *
	 */
	bool hasAttribute(std::string attributeName) const;

	/**
	 *
	 */
	bool hasParentElement() const;

	/**
	 *
	 */
	bool hasSubElements() const;

	/**
	 *
	 */
	bool hasSubElement(std::string subElementName) const;

	/**
	 *
	 */
	std::string getContent() const;

	/**
	 *
	 */
	std::string getAttributeValue(std::string attributeName) const;

	/**
	 *
	 */
	float getAttributeValueAsFloat(std::string attributeName) const;

	/**
	 *
	 */
	int getAttributeValueAsInt(std::string attributeName) const;

	/**
	 *
	 */
	bool getAttributeValueAsBool(std::string attributeName) const;

	/**
	 *
	 */
	const XmlAttribute* getAttribute(std::string attributeName) const;

	/**
	 *
	 */
	XmlAttribute* getAttribute(std::string attributeName);

	/**
	 *
	 */
	std::vector<const XmlAttribute*> getAttributes(std::string attributeName) const;

	/**
	 *
	 */
	std::vector<XmlAttribute*> getAttributes(std::string attributeName);

	/**
	 *
	 */
	void renameAttributes(std::string originalAttributeName, std::string newAttributeName);

	/**
	 *
	 */
	void replaceAttributeValues(std::string attributeName, std::string oldValue,
			std::string newValue);

	/**
	 *
	 */
	const XmlElement* getFirstSubElement(std::string subElementName) const;

	/**
	 *
	 */
	XmlElement* getFirstSubElement(std::string subElementName);

	/**
	 *
	 */
	const XmlElement* getSubElement(std::string subElementName) const;

	/**
	 *
	 */
	XmlElement* getSubElement(std::string subElementName);

	/**
	 *
	 */
	std::vector<const XmlElement*> getSubElements(std::string subElementName) const;

	/**
	 *
	 */
	std::vector<XmlElement*> getSubElements(std::string subElementName);

	/**
	 *
	 */
	std::vector<const XmlElement*> getAllSubElements() const;

	/**
	 *
	 */
	std::vector<XmlElement*> getAllSubElements();

	/**
	 *
	 */
	void renameSubElements(std::string originalElementName, std::string newElementName);

	/**
	 *
	 */
	const XmlElement* getParentElement() const;

	/**
	 *
	 */
	XmlElement* getParentElement();

	/**
	 *
	 */
	void setName(std::string name);

	/**
	 *
	 */
	void setContent(const std::string& content);

	/**
	 *
	 */
	void addAttribute(XmlAttribute* attribute);

	/**
	 *
	 */
	XmlAttribute* removeAttribute(std::string attributeName);

	/**
	 *
	 */
	void deleteAllAttributes();

	/**
	 *
	 */
	void addSubElement(XmlElement* element);

	/**
	 *
	 */
	XmlElement* removeSubElement(std::string elementName);

	/**
	 *
	 */
	bool removeSubElement(const XmlElement* element);

	/**
	 *
	 */
	void deleteAllSubElements();

	/**
	 *
	 */
	void fixSubElementOrder(const std::vector<std::string>& subElementNames);

	/**
	 *
	 */
	void fixAttributeOrder(const std::vector<std::string>& attributeNames);

	/**
	 * Just for testing
	 */
	void dump() const;

	/**
	 * Just for testing
	 */
	void dumpToFile(FILE* file, int depth=0) const;


private:
	/**
	 *
	 */
	XmlElement();

	/**
	 *
	 */
	const XmlElement* getSubElementsInternal(std::string subElementName, bool useFirst,
			std::vector<const XmlElement*>* elementList = NULL) const;

	/**
	 *
	 */
	const XmlAttribute* getAttributesInternal(std::string attributeName,
			std::vector<const XmlAttribute*>* attributeList = NULL) const;

	std::string elementName;
	std::string content;
	std::vector<XmlAttribute*> attributes;
	std::vector<XmlElement*> subElements;
	XmlElement* parentElement;
};

#endif /* XMLELEMENT_H_ */
