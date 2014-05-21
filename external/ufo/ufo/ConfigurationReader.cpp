#include "Debug.h" //assert
#include "ConfigurationReader.h"
#include "Configurator.h"
#include "ConfigurationElement.h"

using namespace std;

ufo::ConfigurationReader::~ConfigurationReader()
{
}

void ufo::ConfigurationReader::addPluginDir( Configurator *cfg, std::string &path ) 
{
	assert (cfg != 0);

	cfg->plugindirs.push_back(path);
}

void ufo::ConfigurationReader::addElement( Configurator *cfg, ConfigurationElement *elem ) 
{
	assert (cfg != 0);
	cfg->elements.push_back(elem);
}

void ufo::ConfigurationReader::setTemplate( Configurator *cfg, std::string &name, ConfigurationElement *elem ) 
{
	assert (cfg != 0);
	cfg->templates[name] = elem;
}

