#ifndef UFO_PLAINCONFIGURATIONREADER_H
#define UFO_PLAINCONFIGURATIONREADER_H

#include <string>
#include <fstream>
#include <iostream>

#include "ConfigurationReader.h"

namespace ufo
{
	class ConfigurationElement;
	class PlainConfigurationReader : public ConfigurationReader
	{
		public:
			/**
			 * Constructor using the path of a configuration file.
			 * If the configuration file couldn't be opened, a
			 * runtime_error is thrown.
			 */
			PlainConfigurationReader( const std::string filename );
			/**
			 * The destructor.
			 */
			virtual ~PlainConfigurationReader();
			/**
			 * A ConfigurationReader must supply a readConfig method.
			 * This method must:
			 * @return a Configurator, or 0, if the config file could not be read.
			 */
			virtual Configurator *readConfig();

		protected:
		private:
			/**
			 * Read a ConfigurationElement from the cfgfile stream.
			 * @param fromTemplate (immediate|fromTemplate) status of the Element (this has already been read).
			 * If neccessary, recurse.
			 */
			ConfigurationElement *readElement(bool fromTemplate);
			/**
			 * Read a string constant.
			 * A string constant may be either one word, or one or more words enclosed by double quotes ('"').
			 */
			std::string readString();
			bool skip(std::string token);
			bool skipComment();
			std::string getConfigDirectory();

			std::string filename;
			std::ifstream &cfgfile;
	};
}
#endif /* UFO_PLAINCONFIGURATIONREADER_H */
