/*
 * XmlDocument.h
 *
 *  Created on: Jun 24, 2009
 *      Author: rlander
 */

#ifndef XMLDOCUMENT_H_
#define XMLDOCUMENT_H_

#include "XmlElement.h"
#include "Platform.h"
#include "XmlDtdReference.h"

/**
 *
 */
class INVRS_SYSTEMCORE_API XmlDocument {
public:
	/**
	 *
	 */
	static XmlDocument* loadXmlDocument(std::string file);

	/**
	 *
	 */
	static XmlDocument* createXmlDocument(XmlElement* element);

	/**
	 *
	 */
	virtual ~XmlDocument();

	/**
	 *
	 */
	const XmlElement* getRootElement() const;

	/**
	 *
	 */
	XmlElement* getRootElement();

	/**
	 *
	 */
	bool hasAttribute(std::string attributeName) const;

	/**
	 *
	 */
	bool hasElement(std::string elementName) const;

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
	const XmlElement* getElement(std::string elementName) const;

	/**
	 *
	 */
	XmlElement* getElement(std::string elementName);

	/**
	 *
	 */
	std::vector<const XmlElement*> getElements(std::string elementName) const;

	/**
	 *
	 */
	std::vector<XmlElement*> getElements(std::string elementName);

	/**
	 *
	 */
	void renameElements(std::string originalElementName, std::string newElementName);

	/**
	 *
	 */
	std::string getElementContent(std::string elementName) const;

	/**
	 *
	 */
	XmlElement* replaceRootElement(XmlElement* element);

	/**
	 *
	 */
	const XmlDtdReference* getDtd() const;

	/**
	 *
	 */
	XmlDtdReference* getDtd();

	/**
	 *
	 */
	void setDtd(XmlDtdReference* dtd);

	/**
	 *
	 */
	void addEntityDtd(XmlDtdReference* entityDtd);

	/**
	 *
	 */
	void removeEntityDtd(XmlDtdReference* entityDtd);

	/**
	 *
	 */
	std::vector<XmlDtdReference*> getEntityDtds();

	/**
	 *
	 */
	XmlDtdReference* getEntityDtd(std::string name);

	/**
	 *
	 */
	void fixEntityDtdOrder(const std::vector<std::string>& entityNames);

	/**
	 * Just for testing
	 */
	void dump() const;

	/**
	 *
	 */
	bool dumpToFile(std::string fileName) const;

private:
	/**
	 * Avoid construction of XmlDocument
	 */
	XmlDocument();

	/**
	 *
	 */
	void printDocumentHeader(FILE* file) const;

	/**
	 *
	 */
	static void parseDocumentDtdAndEntities(std::string definition, XmlDocument* document);

	XmlElement* rootElement;
	XmlDtdReference* documentDtd;
	std::vector<XmlDtdReference*> entityDtds;
}; // XmlDocument

#endif /* XMLDOCUMENT_H_ */
