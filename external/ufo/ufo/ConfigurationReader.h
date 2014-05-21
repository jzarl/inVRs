#ifndef UFO_CONFIGURATIONREADER_H
#define UFO_CONFIGURATIONREADER_H

#include <string>

namespace ufo
{
	class Configurator;
	class ConfigurationElement;

	/**
	 * A ConfigurationReader creates a Configurator.
	 * If the ConfigurationReader opens any files or streams, it must close them
	 * in its destructor (or earlier).
	 *
	 * A class implementing a ConfigurationReader can use the addPluginDir(),
	 * addElement() and addTemplate() methods to interact with the Configurator.
	 */
	class ConfigurationReader
	{
		public:
			/**
			 * The destructor.
			 */
			virtual ~ConfigurationReader();
			/**
			 * A ConfigurationReader must supply a readConfig method.
			 * This method must:
			 * @return a Configurator, or 0, if the config file could not be read.
			 */
			virtual Configurator *readConfig() = 0;
		protected:
			/**
			 * Adds a directory to the given Configurator's plugindirs.
			 *
			 * @param cfg a Configurator.
			 * @param dir a pathname.
			 */
			static void addPluginDir( Configurator *cfg, std::string &path ) ;
			/**
			 * Adds a ConfigurationElement to the given Configurator's elements.
			 *
			 * @param cfg a Configurator.
			 * @param elem a ConfigurationElement.
			 */
			static void addElement( Configurator *cfg, ConfigurationElement *elem ) ;
			/**
			 * Set the template element for the given name in the given Configurator.
			 *
			 * @param cfg a Configurator.
			 * @param name the template name.
			 * @param elem the corresponding ConfigurationElement
			 */
			static void setTemplate( Configurator *cfg, std::string &name, ConfigurationElement *elem ) ;
		private:
	};
}

#endif /* UFO_CONFIGURATIONREADER_H */
