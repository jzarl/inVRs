#ifndef COREPLUG_UNITTESTS_MYSUBTYPE
#define COREPLUG_UNITTESTS_MYSUBTYPE

#include "testFactoryPlugin-factory.h"

class MySubType: public MyType {
	public:
		// factory function:
		static MyType* create();
		virtual std::string message( const std::string &s);
	protected:
		MySubType();
		virtual ~MySubType();
};

#endif
