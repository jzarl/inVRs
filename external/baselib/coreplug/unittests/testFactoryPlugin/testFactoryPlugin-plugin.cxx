#include "testFactoryPlugin-plugin.h"

MyType* MySubType::create()
{
	return new MySubType();
}

MySubType::MySubType()
{
}
MySubType::~MySubType()
{
}

std::string MySubType::message( const std::string &s)
{
	return MyType::message("MySubType: " + s);
}

extern "C" {
	const char TESTFACTORYPLUGINLIB_EXPORT *CP_NAME_MyType = "MySubType";

	bool TESTFACTORYPLUGINLIB_EXPORT CP_IS_COMPATIBLE( const std::string &version) {
		return ( version == MyType::interfaceversion );
	}

	MyTypeFactory TESTFACTORYPLUGINLIB_EXPORT CP_FACTORY() {
		return MySubType::create;
	}
}

