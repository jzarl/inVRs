#ifndef INVRS_UFOXMLCONFIGURATIONREADER_H
#define INVRS_UFOXMLCONFIGURATIONREADER_H

#include <string>
#include <fstream>
#include <iostream>

#include <ufo/ConfigurationReader.h>

// defined in inVRs/SystemCore/XMLTools.h
class XmlDocument;

namespace ufo
{
	class ConfigurationElement;
	class ConfigurationReader;
	class PlainConfigurationReader;
}

class UfoXmlConfigurationReader : public ufo::ConfigurationReader
{
	public:
		/**
		 * Constructor using the path of a configuration file.
		 * If the configuration file couldn't be opened, a
		 * runtime_error is thrown.
		 */
		UfoXmlConfigurationReader( const std::string filename );
		/**
		 * The destructor.
		 */
		virtual ~UfoXmlConfigurationReader();
		/**
		 * A ConfigurationReader must supply a readConfig method.
		 * This method must:
		 * @return a Configurator, or 0, if the config file could not be read.
		 */
		virtual ufo::Configurator *readConfig();

	protected:
	private:
		std::string filename;
		const XmlDocument *document;

		/**
		 * Read a Flock from an XMLElement.
		 * @return A ufo::ConfigurationElement with the flock and all child elements.
		 * @return <code>Null</code> if the Element could not be constructed.
		 */
		ufo::ConfigurationElement *readFlockFromXmlElement( const XmlElement *element );
		/**
		 * Read a Pilot from an XMLElement.
		 * @return A ufo::ConfigurationElement with the Pilot and all child elements.
		 * @return <code>Null</code> if the Element could not be constructed.
		 */
		ufo::ConfigurationElement *readPilotFromXmlElement( const XmlElement *element );
		/**
		 * Read a Behaviour from an XMLElement.
		 * @return A ufo::ConfigurationElement with the Behaviour and all child elements.
		 * @return <code>Null</code> if the Element could not be constructed.
		 */
		ufo::ConfigurationElement *readBehaviourFromXmlElement( const XmlElement *element );
		/**
		 * Read a Steerable from an XMLElement.
		 * @return A ufo::ConfigurationElement with the Steerable and all child elements.
		 * @return <code>Null</code> if the Element could not be constructed.
		 */
		ufo::ConfigurationElement *readSteerableFromXmlElement( const XmlElement *element );

		void readAttributeVectorForConfigurationElement(const XmlElement *element, ufo::ConfigurationElement *ufoElement);
};
#endif /* INVRS_UFOXMLCONFIGURATIONREADER_H */
