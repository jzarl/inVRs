/*
 * XmlConfigurationConverter.h
 *
 *  Created on: Jun 25, 2009
 *      Author: rlander
 */

#ifndef XMLCONFIGURATIONCONVERTER_H_
#define XMLCONFIGURATIONCONVERTER_H_

#include <string>

#include "Platform.h"
#include "DataTypes.h"
#include "XmlDocument.h"

/**
 *
 */
class INVRS_SYSTEMCORE_API XmlConfigurationConverter {
public:

	/**
	 *
	 */
	XmlConfigurationConverter(Version minVersion, Version destinationVersion);

	/**
	 *
	 */
	XmlConfigurationConverter(std::string minVersion, std::string destinationVersion);

	/**
	 *
	 */
	virtual ~XmlConfigurationConverter();

	/**
	 *
	 */
	Version getMinimumVersion() const;

	/**
	 *
	 */
	Version getDestinationVersion() const;

	/**
	 *
	 */
	virtual bool convert(XmlDocument* document, const Version& version, std::string configFile) = 0;

protected:

	/**
	 *
	 */
	bool updateDocument(XmlDocument* document, std::string dtd, const Version& version,
			std::string rootElementName);

	Version minVersion;
	Version destinationVersion;
};

#endif /* XMLCONFIGURATIONCONVERTER_H_ */
