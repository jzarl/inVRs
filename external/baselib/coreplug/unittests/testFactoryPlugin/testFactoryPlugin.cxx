#include "testFactoryPlugin-factory.h"
#include <iostream>
#include <stdexcept>

/**
 * testFactoryPlugin
 * Loads a DSO file that implements the interface for FactoryPlugin.
 * If this works, the plugin factory method is used to create an object,
 * and a method of that object is called.
 * This is done twice: once with MySubType, once with MyOtherSubType.
 * MySubType directly defines the symbols needed by FactoryPlugin, whereas
 * MyOtherSubType uses the macro declared in FactoryPluginCoreAPI.h
 */
int main(int argc, const char** argv)
{
	if ( argc != 3 )
	{
		std::cerr << "Test case called with invalid number of arguments!" << std::endl;
		std::cerr << "Expected a DSO file (lib*.so, *.dll) as argument." << std::endl;
		return 255;
	}
	MyType *myObject=0;
	MyType *myOtherObject=0;
	MyTypePlugin *myPlugin=0;
	MyTypePlugin *myOtherPlugin=0;

	// 1. MyPlugin:
	// load the plugin from the dso file
	try {
		myPlugin = new MyTypePlugin( argv[1], MyType::basetypename );
	} catch ( std::runtime_error e )
	{
		std::cerr << "Could not load plugin: " << e.what() << std::endl;
		return 1;
	}
	// check plugin compatibility
	if ( myPlugin != 0 && myPlugin->isCompatible(MyType::interfaceversion) )
	{
		try {
			std::cerr << "Plugin has type " << myPlugin->name() << std::endl;
			MyTypeFactory factory= create_factory<MyTypeFactory>(myPlugin->get_dso());
			// create object:
			myObject = factory();
		} catch ( std::runtime_error e ) {
			std::cerr << "Could not get factory for MyType: " << e.what() << std::endl;
			return 1;
		}
	}
	if ( !myObject )
	{
		std::cerr << "Factory returned NULL oject." << std::endl;
		return 1;
	}
	// call method on created object
	if ( myObject->message("foo") != "MySubType: foo" )
	{
		std::cerr << "Expected message 'MySubType: foo', but got '" << myObject->message("foo") << "'." << std::endl;
		return 1;
	}
	
	// 2. MyOtherPlugin
	// load the plugin from the dso file
	try {
		myOtherPlugin = new MyTypePlugin( argv[2], MyType::basetypename );
	} catch ( std::runtime_error e )
	{
		std::cerr << "Could not load plugin: " << e.what() << std::endl;
		return 1;
	}
	// check plugin compatibility
	if ( myOtherPlugin != 0 && myOtherPlugin->isCompatible(MyType::interfaceversion) )
	{
		try {
			std::cerr << "Plugin has type " << myOtherPlugin->name() << std::endl;
			MyTypeFactory factory= create_factory<MyTypeFactory>(myOtherPlugin->get_dso());
			// create object:
			myOtherObject = factory();
		} catch ( std::runtime_error e ) {
			std::cerr << "Could not get factory for MyType: " << e.what() << std::endl;
			return 1;
		}
	}
	if ( !myOtherObject )
	{
		std::cerr << "Factory returned NULL oject." << std::endl;
		return 1;
	}
	// call method on created object
	if ( myOtherObject->message("foo") != "MyOtherSubType: foo" )
	{
		std::cerr << "Expected message 'MyOtherSubType: foo', but got '" << myOtherObject->message("foo") << "'." << std::endl;
		return 1;
	}
	// test deallocation:
	myObject->destroy();
	myOtherObject->destroy();
	// close DSO:
	delete myPlugin;
	delete myOtherPlugin;
	std::cerr << "All ok." << std::endl;
	return 0;
}
