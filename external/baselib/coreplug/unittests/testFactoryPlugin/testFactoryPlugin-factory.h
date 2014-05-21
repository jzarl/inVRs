#ifndef COREPLUG_UNITTESTS_MYTYPEFACTORY
#define COREPLUG_UNITTESTS_MYTYPEFACTORY

#include <testfactorypluginlib_export.h>
#include <coreplug/FactoryPlugin.h>
#include <string>

class TESTFACTORYPLUGINLIB_EXPORT MyType
{
	public:
		MyType();
		// all "DSO-boundary crossing" objects should provide ::destroy(), and have a private dtor:
		virtual void destroy();
		// for testing method calls:
		virtual std::string message( const std::string &s);
		// for FactoryPlugin:
		static const std::string basetypename;
		static const std::string interfaceversion;
	protected:
		virtual ~MyType();
	private:
		std::string msg;
};

typedef MyType* (*MyTypeFactory)();
typedef FactoryPlugin MyTypePlugin;

#endif
