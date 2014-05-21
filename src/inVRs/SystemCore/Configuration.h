/*---------------------------------------------------------------------------*\
 *           interactive networked Virtual Reality system (inVRs)            *
 *                                                                           *
 *    Copyright (C) 2005-2009 by the Johannes Kepler University, Linz        *
 *                                                                           *
 *                            www.inVRs.org                                  *
 *                                                                           *
 *              contact: canthes@inVRs.org, rlander@inVRs.org                *
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *                                License                                    *
 *                                                                           *
 * This library is free software; you can redistribute it and/or modify it   *
 * under the terms of the GNU Library General Public License as published    *
 * by the Free Software Foundation, version 2.                               *
 *                                                                           *
 * This library is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Library General Public License for more details.                          *
 *                                                                           *
 * You should have received a copy of the GNU Library General Public         *
 * License along with this library; if not, write to the Free Software       *
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 *
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *                                Changes                                    *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
\*---------------------------------------------------------------------------*/

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>
#include <map>
#include <iostream>
#include <irrXML.h>

#include "Platform.h"
#include "DataTypes.h"
#include "XmlConfigurationLoader.h"
#include "XmlDocument.h"


using namespace irr;
using namespace io;

/******************************************************************************
 * This class provides an interface to access the configuration in the xml-files.
 */
class INVRS_SYSTEMCORE_API Configuration {
public:
	/**
	 * Load a configuration file.
	 * @param cfg the path of the configuration file.
	 * @return true on success, false otherwise.
	 */
	static bool loadConfig(std::string cfg);
	/**
	 * Dump the configuration to stdout.
	 */
	static void printConfig(); // debug method for printing out the configuration

	/**
	 * Returns whether the passed path is set.
	 * @param path-name to search for
	 */
	static bool containsPath(std::string name);

	/**
	 * Get the directory, where <code>name</code> is stored.
	 * @param The named label of the desired path (e.g. "Models", "Tiles", "WorldConfiguration").
	 * @return the base-directory for the given name.
	 */
	static std::string getPath(const std::string& name);

	/** Returns if the passed key occurs in the config-file.
	 * Keys have the format "Element.Subelement".
	 * @param key searched configuration file entry
	 * @return true if the configuration file entry exists, false otherwise
	 */
	static bool contains(std::string key);

	/**
	 * Query a boolean value from the configuration.
	 * @param key a configuration file entry.
	 * @return the value, or false, if it doesn't exist.
	 */
	static bool getBool(std::string key);

	/**
	 * Query an integer value from the configuration.
	 * @param key a configuration file entry.
	 * @return the value, or 0, if it doesn't exist.
	 */
	static int getInt(std::string key);

	/**
	 * Query a float value from the configuration.
	 * @param key a configuration file entry.
	 * @return the value, or 0.0f, if it doesn't exist.
	 */
	static float getFloat(std::string key);

	/**
	 * Query a string value from the configuration.
	 * @param key a configuration file entry.
	 * @return the value, or an empty string, if it doesn't exist.
	 */
	static std::string getString(std::string key);

private:
	struct ConfigurationEntry {
		std::string nodeName;
		std::map<std::string, ConfigurationEntry> subNodes;
		std::map<std::string, std::string> attributes;
	};

	static std::string rootPath;
	static std::map<std::string, std::string> pathMap;
	static ConfigurationEntry rootEntry;

	static void printGeneralConfig(ConfigurationEntry* currentEntry);
	static ConfigurationEntry* getConfigEntry(std::string key, std::string &attributeName,
			bool dumpError = true);

//*****************************************************************************
// Configuration loading
//*****************************************************************************
private:
	static bool parseGeneralConfig(const XmlDocument *document);
	static bool parseGeneralConfigRecursive(const XmlElement* element,
			ConfigurationEntry& currentEntry);
	static bool parsePathConfig(const XmlDocument *document);


	static XmlConfigurationLoader xmlConfigLoader;

	class ConverterToV1_0a4 : public XmlConfigurationConverter {
	public:
		ConverterToV1_0a4();
		virtual bool convert(XmlDocument* document, const Version& version, std::string configFile);
	}; // ConverterToV1_04a
};
#endif //CONFIGURATION_H
