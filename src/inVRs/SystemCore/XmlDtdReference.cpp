/*
 * XmlDtdReference.cpp
 *
 *  Created on: Jun 30, 2009
 *      Author: rlander
 */

#include "XmlDtdReference.h"

XmlDtdReference::XmlDtdReference(std::string name, std::string dtd) :
	name(name),
	dtd(dtd) {
} // XmlDtdReference

XmlDtdReference::~XmlDtdReference() {
} // ~XmlDtdReference

const std::string& XmlDtdReference::getName() const {
	return name;
} // getName

const std::string& XmlDtdReference::getDtd() const {
	return dtd;
} // getDtd

void XmlDtdReference::setName(std::string name) {
	this->name = name;
} // setName

void XmlDtdReference::setDtd(std::string dtd) {
	this->dtd = dtd;
} // setDtd
