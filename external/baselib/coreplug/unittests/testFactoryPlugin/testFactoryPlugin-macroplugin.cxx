#include "testFactoryPlugin-macroplugin.h"
#include <coreplug/FactoryPluginCoreAPI.h>

MyType* MyOtherSubType::create()
{
	return new MyOtherSubType();
}

MyOtherSubType::MyOtherSubType()
{
}
MyOtherSubType::~MyOtherSubType()
{
}

std::string MyOtherSubType::message( const std::string &s)
{
	return MyType::message("MyOtherSubType: " + s);
}

bool isCompatible( const std::string &version)
{
	return ( version == MyType::interfaceversion );
}

// create "entry points" for FactoryPlugin:
MAKEFACTORYPLUGIN(MyType, MyOtherSubType, isCompatible, MyTypeFactory)

