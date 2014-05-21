/*
 * XmlAttribute.h
 *
 *  Created on: Jun 24, 2009
 *      Author: rlander
 */

#ifndef XMLATTRIBUTE_H_
#define XMLATTRIBUTE_H_

#include <string>

#include "Platform.h"

/**
 *
 */
class INVRS_SYSTEMCORE_API XmlAttribute {
public:
	/**
	 *
	 */
	XmlAttribute(std::string key, std::string value);

	/**
	 *
	 */
	virtual ~XmlAttribute();

	/**
	 *
	 */
	const std::string& getKey() const;

	/**
	 *
	 */
	const std::string& getValue() const;

	/**
	 *
	 */
	void setKey(std::string key);

	/**
	 *
	 */
	void setValue(std::string value);

private:
	std::string key;
	std::string value;
}; // XmlAttribute

#endif /* XMLATTRIBUTE_H_ */
