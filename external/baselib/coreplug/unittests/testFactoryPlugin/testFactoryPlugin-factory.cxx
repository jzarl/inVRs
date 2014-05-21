#include "testFactoryPlugin-factory.h"


// interface definitions:

const std::string MyType::basetypename = "MyType";
const std::string MyType::interfaceversion = testFactoryPluginVERSION;

// MyType base type definitions

MyType::MyType()
{
}
void MyType::destroy()
{
	delete this;
}
MyType::~MyType()
{
}

std::string MyType::message( const std::string &s)
{
	msg = s;
	return msg;
}
