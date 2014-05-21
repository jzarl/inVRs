#include <iostream>
#include <stdexcept>
#include <typeinfo>

#undef INVRSSYSTEMCORE_EXPORTS
#include "inVRs/SystemCore/UtilityFunctions.h"

#undef NDEBUG
#include <cassert>

#define test_bool_true(x) try { \
	if ( !(x) ) \
	{ \
		std::cout << "Test condition ``" # x "'' failed!" <<std::endl; \
		failed=true; \
	} \
} catch (std::exception &e ) \
{ \
	std::cout << "Exception " << typeid(e).name() << " during test ``" # x "'': " << e.what() << std::endl; \
	failed=true;\
}



int main()
{
	bool failed=false;

	test_bool_true ( tokenizeString("part1",":").size() == 1 );
	test_bool_true ( tokenizeString("::",":").size() == 0 );
	test_bool_true ( tokenizeString("::part1::part2","::").size() == 2 );
	test_bool_true ( tokenizeString("::part1::part2",":").size() == 2 );
	test_bool_true ( tokenizeString(";:part1:part2;part3",":;").size() == 3 );
	test_bool_true ( tokenizeString("",":").size() == 0 );
	test_bool_true ( tokenizeString(std::string(),":").size() == 0 );
	
//	assert(isRelativePath("../asset/toolbox") == true);
//	assert(isRelativePath("unQualified") == true);
//	assert(isRelativePath(".unQualified") == true);
//	assert(isRelativePath("..unQualified") == true);
//	assert(isRelativePath("u") == true);
//	assert(isRelativePath("") == true);
//	assert(isRelativePath(std::string()) == true);
//	assert(isRelativePath(".") == true);
//	assert(isRelativePath("..") == true);
//	assert(isRelativePath("./inthisdir") == true);
//	assert(isRelativePath("../inthisdir") == true);
//	assert(isRelativePath("../../greatgrandparent") == true);
//	assert(isRelativePath("unQualified/subDir") == true);
//	assert(isRelativePath("\\unQualified") == true);
//	assert(isRelativePath("C:foo") == true);
//#ifdef WIN32
//	assert(isRelativePath("c:\\absolute") == false);
//	assert(isRelativePath("/home/user/docs") == true);
//	assert(isRelativePath("/foo") == true);
//	assert(isRelativePath("~/.rcinfo") == true);
//	assert(isRelativePath("~absolute") == true);
//	assert(isRelativePath("~root/absolute") == true);
//	assert(isRelativePath("/") == true);
//	assert(isRelativePath("\\") == true);
//	assert(isRelativePath(".\\relative") == true);
//	assert(isRelativePath("..\\relative") == true);
//	assert(isRelativePath("foo\\subDir") == true);
//#else
//	assert(isRelativePath("/home/user/docs") == false);
//	assert(isRelativePath("/foo") == false);
//	assert(isRelativePath("~/.rcinfo") == false);
//	assert(isRelativePath("~absolute") == false);
//	assert(isRelativePath("~root/absolute") == false);
//	assert(isRelativePath("/") == false);
//#endif

	return (failed) ? 1 : 0;
}
