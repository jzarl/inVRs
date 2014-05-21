/**
 * Implementation of a simple dummy-plugin to test plugin-loading.
 */
#undef INVRSSYSTEMCORE_EXPORTS
#include <string>
#include <inVRs/SystemCore/ComponentInterfaces/module_core_api.h>
#include <inVRs/SystemCore/DebugOutput.h>
#include <inVRs/SystemCore/SystemCore.h>
class testPlugins_lib : public ModuleInterface {
public:
	virtual bool loadConfig(std::string configFile);

	virtual void update(float dt);

	virtual std::string getName();
};

bool testPlugins_lib::loadConfig(std::string configFile)
{
	printd(DEBUG,"testPlugins_lib::loadConfig() called.\n");
	return true;
}

void testPlugins_lib::update(float dt)
{
	printd(DEBUG,"testPlugins_lib::update() called.\n");
}

std::string testPlugins_lib::getName()
{
	return std::string( "testPlugins_lib" );
}

MAKEMODULEPLUGIN(testPlugins_lib, SystemCore)
