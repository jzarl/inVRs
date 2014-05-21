/*
 * XmlAttribute.cpp
 *
 *  Created on: Jun 24, 2009
 *      Author: rlander
 */

#include "XmlAttribute.h"

XmlAttribute::XmlAttribute(std::string key, std::string value) :
	key(key),
	value(value) {
} // XmlAttribute

XmlAttribute::~XmlAttribute() {
} // ~XmlAttribute

const std::string& XmlAttribute::getKey() const {
	return key;
} // getKey

const std::string& XmlAttribute::getValue() const {
	return value;
} // getValue

void XmlAttribute::setKey(std::string key) {
	this->key = key;
} // setKey

void XmlAttribute::setValue(std::string value) {
	this->value = value;
} // setValue
