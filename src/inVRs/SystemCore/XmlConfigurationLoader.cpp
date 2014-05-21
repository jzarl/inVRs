/*
 * XmlConfigurationLoader.cpp
 *
 *  Created on: Jun 25, 2009
 *      Author: rlander
 */

#include "XmlConfigurationLoader.h"

#include <sstream>
#include <memory>

#include "DebugOutput.h"
#include "XMLTools.h"
#include "Configuration.h"
#include "UtilityFunctions.h"

// disable deprecation warning for std::auto_ptr when std::unique_ptr is not available.
#ifndef HAS_CXX11_UNIQUE_PTR
// Only do this on GCC versions that support it:
# if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 402
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
#endif

namespace
{
	bool copyFile(const std::string& srcFile, const std::string dstFile) {

		size_t size;
		char *buffer;
		size_t result;
		bool success = true;
		FILE* in = fopen(srcFile.c_str(), "r");
		FILE* out = fopen(dstFile.c_str(), "w");

		// check if files handles could be opened
		if (!in || !out) {
			if (in)
				fclose(in);
			if (out)
				fclose(out);
			return false;
		} // if

		// obtain file size:
		fseek(in, 0, SEEK_END);
		size = ftell(in);
		rewind(in);

		// allocate memory to contain the whole file:
		buffer = (char*)malloc(sizeof(char) * size);
		if (buffer == NULL) {
			fclose(in);
			fclose(out);
			return false;
		} // if

		// copy the file into the buffer:
		result = fread(buffer, 1, size, in);
		if (result != size) {
			fclose(in);
			fclose(out);
			free(buffer);
			return false;
		} // if

		// write file into file
		result = fwrite(buffer, 1, size, out);
		if (result != size) {
			success = false;
		} // if

		// terminate
		fclose(in);
		fclose(out);
		free(buffer);

		return success;
	}
}

const std::string XmlConfigurationLoader::XmlDtdUrl = "http://dtd.inVRs.org/";

XmlConfigurationLoader::XmlConfigurationLoader() :
	oldestVersion(Version(1000, 0)) {
} // XmlConfigurationLoader

XmlConfigurationLoader::~XmlConfigurationLoader() {
	std::vector<XmlConfigurationConverter*>::iterator it;
	for (it = converterList.begin(); it != converterList.end(); ++it) {
		delete *it;
	} // for
	converterList.clear();
} // ~XmlConfigurationLoader

bool XmlConfigurationLoader::hasConverters() {
	return (converterList.size() > 0);
} // hasConverters

void XmlConfigurationLoader::registerConverter(XmlConfigurationConverter* converter) {
	std::vector<XmlConfigurationConverter*>::iterator it;
	bool inserted = false;

	for (it = converterList.begin(); it != converterList.end(); ++it) {
		if ((*it)->getDestinationVersion() > converter->getDestinationVersion()) {
			converterList.insert(it, converter);
			inserted = true;
			break;
		} // if
	} // for

	// when not inserted then it must be attached at the end
	if (!inserted) {
		converterList.push_back(converter);
		latestVersion = converter->getDestinationVersion();
	} // if

	if (converter->getMinimumVersion() < oldestVersion){
		oldestVersion = converter->getMinimumVersion();
	} // if
} // registerConverter

const XmlDocument* XmlConfigurationLoader::loadConfiguration(std::string file,
		bool automaticUpdateIfConfigured) {
	bool updateFile = false;
	bool successfullBackup = false;

	if (automaticUpdateIfConfigured && Configuration::contains("XmlConfigLoader.updateFiles")) {
		updateFile = Configuration::getBool("XmlConfigLoader.updateFiles");
	} // if

	XmlDocument* document = XmlDocument::loadXmlDocument(file);
	if (!document || !document->getRootElement()) {
		printd(ERROR,
				"XmlConfigurationLoader::loadConfig(): error at loading of configuration file %s!\n",
				file.c_str());
		if (document)
			delete document;
		return NULL;
	} // if
	if (converterList.size() > 0) {
		Version version = getConfigurationVersion(document);
		if (version == UndefinedVersion) {
			printd(WARNING, "XmlConfigurationLoader::loadConfiguration(): Missing version entry in configuration file %s! It is recommended to update the config file to a current version!\n",
					file.c_str());
		} // if
		if (version < oldestVersion) {
			printd(ERROR,
					"XmlConfigurationLoader::loadConfiguration(): Version of config file %s is too old (v%s required, v%s found)! Please update the configuration file!\n",
					file.c_str(), version.toString().c_str(), oldestVersion.toString().c_str());
			delete document;
			return NULL;
		} // if
		if (version < latestVersion) {
			if (updateFile) {
				std::string backupFile;
				int backupIndex = 0;
				do {
					std::stringstream backupFileHelper;
					backupFileHelper << file << ".backup_" << backupIndex++;
					backupFile = backupFileHelper.str();
				} while (fileExists(backupFile));

				printd(WARNING,
						"XmlConfigurationLoader::loadConfiguration(): Version v%s of config file %s is deprecated! Trying to automatically update file to current version v%s! Backup of current version will be stored in %s!\n",
						version.toString().c_str(), file.c_str(), latestVersion.toString().c_str(),
						backupFile.c_str());

//				successfullBackup = document->dumpToFile(backupFile);
				successfullBackup = copyFile(file, backupFile);
			} else {
				printd(WARNING,
						"XmlConfigurationLoader::loadConfiguration(): Version of config file %s is deprecated! It is recommended to update to a current version!\n",
						file.c_str());
			} // else
			if (!convertDocumentToVersion(document, version, latestVersion, file)) {
				printd(ERROR,
						"XmlConfigurationLoader::loadConfiguration(): Conversion to latest version failed!\n");
				delete document;
				return NULL;
			} // if
			if (updateFile && successfullBackup) {
				printd(INFO,
						"XmlConfigurationLoader::loadConfiguration(): Writing updated file to %s!\n",
						file.c_str());
				document->dumpToFile(file);
			} // if
		} // if
		else if (version > latestVersion) {
			printd(ERROR,
					"XmlConfigurationLoader::loadConfiguration(): Version of config file %s (v%s) is newer than the latest known version (v%s)! Loading of configuration file will most likely fail! Please downgrade configuration file or upgrade inVRs!\n",
					file.c_str(), version.toString().c_str(), latestVersion.toString().c_str());
		} // else if
	} // if
	return document;
} // loadConfiguration

bool XmlConfigurationLoader::updateXmlElement(XmlElement* &element, const Version& srcVersion,
		const Version& dstVersion, std::string configFile) {

	bool success;
	Version version = srcVersion;

	// create temporary document for conversion
	std::auto_ptr<XmlDocument> document(XmlDocument::createXmlDocument(element));
	// convert to passed destination version
	success = convertDocumentToVersion(document.get(), version, dstVersion, configFile);
	// remove element from document to avoid automatic deletion
	element = document->replaceRootElement(NULL);

	return success;
} // updateXmlElement

Version XmlConfigurationLoader::getLatestVersion() const {
	return latestVersion;
} // getLatestVersion


Version XmlConfigurationLoader::getConfigurationVersion(XmlDocument* document) {
	Version version = UndefinedVersion;
	if (document->getRootElement()->hasAttribute("version")) {
		version = document->getRootElement()->getAttributeValue("version");
	} // if
	return version;
} // getConfigurationVersion

bool XmlConfigurationLoader::convertDocumentToVersion(XmlDocument* document, Version& version,
		const Version& dstVersion, std::string configFile) {
	if (version >= dstVersion)
		return true;

	bool success = true;
	std::vector<XmlConfigurationConverter*>::iterator it;
	for (it = converterList.begin(); it != converterList.end(); ++it) {
		Version converterVersion = (*it)->getDestinationVersion();
		if (converterVersion > version && converterVersion <= dstVersion) {
			success = (*it)->convert(document, version, configFile);
			if (!success)
				break;
			// update version information
			version = converterVersion;
		} // if
	} // for
	if (success)
		version = dstVersion;
	return success;
} // convertDocumentToLatest
