#include <inVRs/SystemCore/SystemCore.h>
// for printd:
#include <inVRs/SystemCore/DebugOutput.h>
// for getFullPath():
#include <inVRs/SystemCore/UtilityFunctions.h>
// MAKEMODULEPLUGIN macro:
#include <inVRs/SystemCore/ComponentInterfaces/module_core_api.h>

#include <ufo/UfoDB.h>
#include <ufo/Configurator.h>
#include <ufo/ConfigurationReader.h>

#include "UfoXmlConfigurationReader.h"
#include "Steering.h"

using namespace std;
using namespace gmtl;
using namespace ufo;

Steering::Steering()
	: ModuleInterface()
{
}

Steering::~Steering()
{
}

bool Steering::loadConfig(std::string configFile)
{
	std::string configFileFullPath = getConcatenatedPath(configFile, "SteeringModuleConfiguration");
	printd(INFO, "Steering::loadConfig(): Loading configuration file %s\n",
			configFileFullPath.c_str());

	ConfigurationReader *cfgReader = new UfoXmlConfigurationReader( configFileFullPath );

	Configurator *cfg = cfgReader->readConfig();

	// we don't need the cfgReader any more:
	delete cfgReader;

	if ( cfg == 0 )
	{
		printd(ERROR, "Steering::loadConfig(): readConfig() failed!\n");
		return false;
	}

	if ( ! cfg->bind() )
	{
		printd(ERROR, "Steering::loadConfig(): bind() failed!\n");
		cfg->print();
		return false;
	}
	// Configurator is no longer needed:
	delete cfg;
	printd(INFO,"Steering::loadConfig(): configuration has %d Flocks and %d independent Pilots.\n"
			, UfoDB::the()->getFlocks().size(), UfoDB::the()->getPilots().size());
	return true;
}

void Steering::cleanup()
{
	UfoDB::reset();
}

Steering::CONFIGURATION_ORDER Steering::getConfigurationOrder()
{
	return CONFIGURE_AFTER_CORECOMPONENTS;
}

std::string Steering::getName()
{
	return "Steering";
}

void Steering::update(const float dt)
{
	// tell UfoDB to do a simulation step
	UfoDB::the()->update(dt);
}

MAKEMODULEPLUGIN(Steering, SystemCore)
