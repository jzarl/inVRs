/*
 * XmlConfigurationLoader.h
 *
 *  Created on: Jun 25, 2009
 *      Author: rlander
 */

#ifndef XMLCONFIGURATIONLOADER_H_
#define XMLCONFIGURATIONLOADER_H_

#include <vector>

#include "Platform.h"
#include "DataTypes.h"
#include "XmlDocument.h"
#include "XmlConfigurationConverter.h"

/**
 *
 */
class INVRS_SYSTEMCORE_API XmlConfigurationLoader {
public:

	/**
	 *
	 */
	XmlConfigurationLoader();

	/**
	 *
	 */
	~XmlConfigurationLoader();

	/**
	 *
	 */
	bool hasConverters();

	/**
	 *
	 */
	void registerConverter(XmlConfigurationConverter* converter);

	/**
	 *
	 */
	const XmlDocument* loadConfiguration(std::string file, bool automaticUpdateIfConfigured = true);

	/**
	 *
	 */
	bool updateXmlElement(XmlElement* &element, const Version& srcVersion, const Version& dstVersion,
			std::string configFile);

	/**
	 *
	 */
	Version getLatestVersion() const;

	static const std::string XmlDtdUrl;

private:
	/**
	 *
	 */
	Version getConfigurationVersion(XmlDocument* document);

	/**
	 *
	 */
	bool convertDocumentToVersion(XmlDocument* document, Version& version,
			const Version& dstVersion, std::string configFile);

	/// List of all configuration converters in order
	std::vector<XmlConfigurationConverter*> converterList;
	/// Latest version
	Version latestVersion;
	/// Oldest supported version
	Version oldestVersion;
}; // XmlConfigurationLoader

#endif /* XMLCONFIGURATIONLOADER_H_ */
