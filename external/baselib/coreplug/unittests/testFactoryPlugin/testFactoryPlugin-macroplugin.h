#ifndef COREPLUG_UNITTESTS_MYOTHERSUBTYPE
#define COREPLUG_UNITTESTS_MYOTHERSUBTYPE

#include "testFactoryPlugin-factory.h"

class MyOtherSubType: public MyType {
	public:
		// factory function:
		static MyType* create();
		virtual std::string message( const std::string &s);
	protected:
		MyOtherSubType();
		virtual ~MyOtherSubType();
};

#endif
