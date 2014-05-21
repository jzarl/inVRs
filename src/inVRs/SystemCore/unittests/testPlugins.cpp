#undef INVRSSYSTEMCORE_EXPORTS
#include <OpenSG/OSGConfig.h>
#if OSG_MAJOR_VERSION >= 2
#include <OpenSG/OSGBaseInitFunctions.h>
#else
#include <OpenSG/OSGBaseFunctions.h>
#endif
#include <inVRs/SystemCore/DebugOutput.h>
#include <inVRs/SystemCore/SystemCore.h>

OSG_USING_NAMESPACE

/** Basic test for plugin loading.
 * Call this with the file name of the test-plugin as the sole parameter.
 * Exit values: 0, if all tests are ok, 1 otherwise.
 */
int main(int argc, char **argv) {
	// needed for printd locks:
	osgInit(argc, argv);

	if ( argc != 2 )
	{
		printd(ERROR, "Exactly one parameter expected!\n");
		return 1;
	}

	ModuleInterface *myplugin = 0;
	myplugin = SystemCore::getModuleByName("testPlugins_lib", argv[1]);
	if ( ! myplugin )
	{
		printd(ERROR, "SystemCore::getModuleByName(testPlugins_lib,%s) returned NULL!\n", argv[1]);
		return 1;
	}
	if ( ! SystemCore::isModuleLoaded("testPlugins_lib") )
	{
		printd(ERROR, "SystemCore::isModuleLoaded(testPlugins_lib) returned false!\n");
		return 1;
	}
	if ( myplugin->getName() != "testPlugins_lib" )
	{
		printd(ERROR, "Test plugin name doesn't match expected value!\n");
		return 1;
	}
	printd_severity(DEBUG);
	myplugin->loadConfig("");
	myplugin->update(0.0);

	SystemCore::cleanup();
	osgExit();
	return 0;
}
