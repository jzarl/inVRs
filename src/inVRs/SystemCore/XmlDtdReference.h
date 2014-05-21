/*
 * XmlDtdReference.h
 *
 *  Created on: Jun 30, 2009
 *      Author: rlander
 */

#ifndef XMLDTDREFERENCE_H_
#define XMLDTDREFERENCE_H_

#include <string>

#include "Platform.h"

/**
 *
 */
class INVRS_SYSTEMCORE_API XmlDtdReference {
public:
	/**
	 *
	 */
	XmlDtdReference(std::string name, std::string dtd);

	/**
	 *
	 */
	virtual ~XmlDtdReference();

	/**
	 *
	 */
	const std::string& getName() const;

	/**
	 *
	 */
	const std::string& getDtd() const;

	/**
	 *
	 */
	void setName(std::string name);

	/**
	 *
	 */
	void setDtd(std::string dtd);

private:
	std::string name;
	std::string dtd;
}; // XmlDtdReference

#endif /* XMLDTDREFERENCE_H_ */
