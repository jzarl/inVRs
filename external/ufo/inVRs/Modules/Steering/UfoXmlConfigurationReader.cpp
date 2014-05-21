#include <string>
#include <stdexcept> // std::runtime_error

// for printd:
#include <inVRs/SystemCore/DebugOutput.h>
// for getFullPath():
#include <inVRs/SystemCore/UtilityFunctions.h>
// XML stuff
#include <inVRs/SystemCore/XMLTools.h>

#include <ufo/Configurator.h>
#include <ufo/ConfigurationElement.h>
#include <ufo/ConfigurationReader.h>
#include <ufo/PlainConfigurationReader.h>

#include "UfoXmlConfigurationReader.h"

using namespace std;
using namespace ufo;

	UfoXmlConfigurationReader::UfoXmlConfigurationReader( const std::string filename )
: filename(filename), document(0)
{
	//printd(INFO,"UfoXmlConfigurationReader: Loading XML configuration from file \"%s\".\n", filename.c_str());

	XmlConfigurationLoader cfgLoader;
	// when changing the dtd, register converter here:
	// cfgLoader.registerConverter(...);
	document = cfgLoader.loadConfiguration(filename);
	if ( document == 0 )
	{
		throw runtime_error("UfoXmlConfigurationReader: Couldn't open file \"" + filename + "\"");
	}
	//printd(INFO,"UfoXmlConfigurationReader: XML configuration file loaded.\n");
}

UfoXmlConfigurationReader::~UfoXmlConfigurationReader()
{
	delete document;
}

ufo::Configurator * UfoXmlConfigurationReader::readConfig()
{
	printd(INFO,"UfoXmlConfigurationReader: Parsing XML configuration\n");
	Configurator *cfg = 0;

	if ( ! document->hasElement("ufoConfiguration") )
	{
		printd(ERROR, "UfoXmlConfigurationReader(): configuration element ``ufoConfiguration'' not found!\n");
		return 0;
	}

	//<!ELEMENT ufoConfiguration (plainConfiguration | ((flock,pilot,template)*, arguments?))>
	//<!ATTLIST ufoConfiguration version (1.0a4) #REQUIRED>
	// <plainConfiguration> -> use PlainConfigurationReader
	if ( document->hasElement("ufoConfiguration.plainConfiguration") )
	{
		const XmlElement *plainCfg = document->getElement("ufoConfiguration.plainConfiguration");
		if ( ! plainCfg->hasAttribute("configFile") )
		{
			printd(WARNING,"UfoXmlConfigurationReader.readConfig(): XML element ``plainConfiguration'' found without ``configFile'' attribute!\n");
			printd(WARNING,"UfoXmlConfigurationReader.readConfig(): Ignoring element ``plainConfiguration''...\n");
		} else {
			try {
				printd(INFO, "UfoXmlConfigurationReader.readConfig(): using (legacy) plain configuration file...\n");
				// constructor may throw an exception:
				PlainConfigurationReader *plainCfgReader = 
					new PlainConfigurationReader( plainCfg->getAttributeValue("configFile"));
				// let the PlainConfigurationReader handle the configFile:
				cfg = plainCfgReader->readConfig();
				delete plainCfgReader;

				return cfg;
			} catch ( runtime_error e ) 
			{
				printd(WARNING, "UfoXmlConfigurationReader.readConfig(): PlainConfigurationReader couldn't be created!\n");
				printd(WARNING, "UfoXmlConfigurationReader.readConfig(): Error was: %s\n", e.what());
				printd(WARNING, "UfoXmlConfigurationReader.readConfig(): Ignoring element ``plainConfiguration''...\n");
			}
		}
	}

	cfg = new Configurator();

	// ((flock,pilot)*, arguments?)
	if ( document->hasElement("ufoConfiguration.arguments") )
	{
		ArgumentVector *arguments = readArgumentsFromXmlElement(
				document->getElement("ufoConfiguration.arguments"));
		if ( arguments->keyExists("pluginDirectories") )
		{
			std::string path;
			// pluginDirectories is a colon-delimited list of paths
			// (like unix-paths, e.g. "/usr/lib:/usr:local/lib")
			arguments->get("pluginDirectories",path);
			// separate the path entries:
			std::vector<std::string> pluginDirectories;
			pluginDirectories = tokenizeString(path,getPathSeparator());
			// and add them to the configurationReader:
			for(std::vector<std::string>::iterator it = pluginDirectories.begin()
				; it != pluginDirectories.end()
				; ++it )
			{
				ConfigurationReader::addPluginDir(cfg,*it);
			}
		}
		delete arguments;
	}

	//if ( document->hasElement("ufoConfiguration.template") )
	{
		std::vector< const XmlElement*> templates = 
			document->getElements("ufoConfiguration.template");
		for ( std::vector<const XmlElement*>::const_iterator it = templates.begin()
				; it != templates.end()
				; ++it )
		{
			std::string templateName = (*it)->getAttributeValue("name");
			if ( templateName.empty() )
			{
				printd(WARNING,"UfoXmlConfigurationReader.readConfig(): ignoring template without name!\n");
				continue;
			}
			if ( (*it)->getAllSubElements().size() != 1 )
			{
				printd(WARNING,"UfoXmlConfigurationReader.readConfig(): ignoring template with more/less than 1 child!\n");
				continue;
			}
			const XmlElement *subElement = (*it)->getAllSubElements()[0];
			ConfigurationElement *templateElement = 0;
			if ( subElement->getName() == "flock" )
				templateElement = readFlockFromXmlElement(subElement);
			else if ( subElement->getName() == "pilot" )
				templateElement = readPilotFromXmlElement(subElement);
			else if ( subElement->getName() == "behaviour" )
				templateElement = readBehaviourFromXmlElement(subElement);
			else if ( subElement->getName() == "steerable" )
				templateElement = readSteerableFromXmlElement(subElement);
			else
				printd(WARNING,"UfoXmlConfigurationReader.readConfig(): ignoring template with unknown type %s!\n",subElement->getName().c_str());

			if ( templateElement )
			{
				//printd(INFO,"UfoXmlConfigurationReader.readConfig(): Adding template %s\n",templateName.c_str());
				ConfigurationReader::setTemplate(cfg,templateName,templateElement);
			}
		}
	}
	//if ( document->hasElement("ufoConfiguration.flock") )
	{
		std::vector< const XmlElement*> flocks = 
			document->getElements("ufoConfiguration.flock");
		for ( std::vector<const XmlElement*>::const_iterator it = flocks.begin()
				; it != flocks.end()
				; ++it )
		{
			ConfigurationElement *elem = readFlockFromXmlElement(*it);
			if (elem)
				ConfigurationReader::addElement(cfg,elem);
		}
	}
	
	//if ( document->hasElement("ufoConfiguration.pilot") )
	{
		std::vector< const XmlElement*> pilots = 
			document->getElements("ufoConfiguration.pilot");
		for ( std::vector<const XmlElement*>::const_iterator it = pilots.begin()
				; it != pilots.end()
				; ++it )
		{
			ConfigurationElement *elem = readPilotFromXmlElement(*it);
			if (elem)
				ConfigurationReader::addElement(cfg,elem);
		}
	}
	return cfg;
}

ufo::ConfigurationElement *UfoXmlConfigurationReader::readFlockFromXmlElement( const XmlElement *element )
{
	bool fromTemplate = false;
	if ( element->getAttributeValue("fromTemplate") == "true" )
	{
		fromTemplate = true;
	}
	ConfigurationElement *flock = new ConfigurationElement(ConfigurationElement::FLOCK,fromTemplate);
	if ( element->hasAttribute("typename") )
	{
		flock->name = element->getAttributeValue("typename");
	} else {
		printd(WARNING,"UfoXmlConfigurationReader::readFlockFromXmlElement(): ignoring flock without typename!\n");
		delete flock;
		return 0;
	}
	// parse argumentVector:
	readAttributeVectorForConfigurationElement(element,flock);
	// add subElements:
	if ( element->hasSubElement("pilot"))
	{
		std::vector< const XmlElement*> pilots = 
			element->getSubElements("pilot");
		for ( std::vector<const XmlElement*>::const_iterator it = pilots.begin()
				; it != pilots.end()
				; ++it )
		{
			ConfigurationElement *pilot = readPilotFromXmlElement(*it);
			if (pilot)
				flock->children.push_back(pilot);
		}
	}
	return flock;
}

ufo::ConfigurationElement *UfoXmlConfigurationReader::readPilotFromXmlElement( const XmlElement *element )
{
	bool fromTemplate = false;
	if ( element->getAttributeValue("fromTemplate") == "true" )
	{
		fromTemplate = true;
	}
	ConfigurationElement *pilot = new ConfigurationElement(ConfigurationElement::PILOT,fromTemplate);
	if ( element->hasAttribute("typename") )
	{
		pilot->name = element->getAttributeValue("typename");
	} else {
		printd(WARNING,"UfoXmlConfigurationReader::readPilotFromXmlElement(): ignoring pilot without typename!\n");
		delete pilot;
		return 0;
	}
	// parse argumentVector:
	readAttributeVectorForConfigurationElement(element,pilot);
	// add subElements:
	if ( element->hasSubElements())
	{
		std::vector< const XmlElement*> subElements = 
			element->getAllSubElements();
		for ( std::vector<const XmlElement*>::const_iterator it = subElements.begin()
				; it != subElements.end()
				; ++it )
		{
			// a Pilot can have behaviour and steerable subelements:
			if ( (*it)->getName() == "behaviour" )
			{
				ConfigurationElement *behaviour = readBehaviourFromXmlElement(*it);
				if (pilot)
					pilot->children.push_back(behaviour);
			} else if ( (*it)->getName() == "steerable" )
			{
				ConfigurationElement *steerable = readSteerableFromXmlElement(*it);
				if (pilot)
					pilot->children.push_back(steerable);
			} 
			// other subelements are silently ignored...
		}
	}
	return pilot;
}

ufo::ConfigurationElement *UfoXmlConfigurationReader::readBehaviourFromXmlElement( const XmlElement *element )
{
	bool fromTemplate = false;
	if ( element->getAttributeValue("fromTemplate") == "true" )
	{
		fromTemplate = true;
	}
	ConfigurationElement *behaviour = new ConfigurationElement(ConfigurationElement::BEHAVIOUR,fromTemplate);
	if ( element->hasAttribute("typename") )
	{
		behaviour->name = element->getAttributeValue("typename");
	} else {
		printd(WARNING,"UfoXmlConfigurationReader::readBehaviourFromXmlElement(): ignoring behaviour without typename!\n");
		delete behaviour;
		return 0;
	}
	// parse argumentVector:
	readAttributeVectorForConfigurationElement(element,behaviour);
	// add subElements:
	if ( element->hasSubElement("behaviour"))
	{
		std::vector< const XmlElement*> behaviours = 
			element->getSubElements("behaviour");
		for ( std::vector<const XmlElement*>::const_iterator it = behaviours.begin()
				; it != behaviours.end()
				; ++it )
		{
			ConfigurationElement *subBehaviour = readBehaviourFromXmlElement(*it);
			if (subBehaviour)
				behaviour->children.push_back(subBehaviour);
		}
	}
	return behaviour;
}

ufo::ConfigurationElement *UfoXmlConfigurationReader::readSteerableFromXmlElement( const XmlElement *element )
{
	bool fromTemplate = false;
	if ( element->getAttributeValue("fromTemplate") == "true" )
	{
		fromTemplate = true;
	}
	ConfigurationElement *steerable = new ConfigurationElement(ConfigurationElement::STEERABLE,fromTemplate);
	if ( element->hasAttribute("typename") )
	{
		steerable->name = element->getAttributeValue("typename");
	} else {
		printd(WARNING,"UfoXmlConfigurationReader::readSteerableFromXmlElement(): ignoring steerable without typename!\n");
		delete steerable;
		return 0;
	}
	// parse argumentVector:
	readAttributeVectorForConfigurationElement(element,steerable);
	return steerable;
}

void UfoXmlConfigurationReader::readAttributeVectorForConfigurationElement( const XmlElement *element, ufo::ConfigurationElement *ufoElement )
{
	if ( element->hasSubElement("arguments") )
	{
		// we don't create an argumentVector, because (contrary to what its name might suggest)
		// an argumentVector doesn't preserve the order of its arguments
		std::vector<const XmlElement*> arguments;
		arguments = element->getSubElements("arguments.arg");
		for (std::vector<const XmlElement*>::const_iterator elemIt = arguments.begin()
				; elemIt != arguments.end()
				;++elemIt)
		{
			if ( (*elemIt)->hasAttribute("key") &&  (*elemIt)->hasAttribute("value"))
			{
				//printd(INFO,"UfoXmlConfigurationReader: element %s has parameter %s := %s\n",
				//		ufoElement->name.c_str(), (*elemIt)->getAttributeValue("key").c_str(), (*elemIt)->getAttributeValue("value").c_str());
				ufoElement->parameters.push_back(std::pair<std::string,std::string > (
							(*elemIt)->getAttributeValue("key"), (*elemIt)->getAttributeValue("value") ));
			} else {
				printd(WARNING,"UfoXmlConfigurationReader::readAttributeVectorForConfigurationElement(): ignoring arg with missing key and/or value!\n");
				printd(WARNING,"UfoXmlConfigurationReader::readAttributeVectorForConfigurationElement(): in element %s.\n",ufoElement->name.c_str());
				//(*elemIt)->dump();
			}
		}
	} 
	//else printd(INFO,"UfoXmlConfigurationReader: element %s has no parameters.\n", ufoElement->name.c_str());
}
