#undef INVRSSYSTEMCORE_EXPORTS
#include <iostream>
#include <stdexcept>
#include <typeinfo>
#include <inVRs/SystemCore/XMLTools.h>

// Set TESTS_PERMISSIVE to be more lenient when it comes to edge cases.
// I.e. don't throw an error when fileExists returns true for a directory.
#define TESTS_PERMISSIVE 1

//BEGIN test_bool_true
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
//END test_bool_true

#define test_bool_false(x) test_bool_true(!(x))

/** Test-Suite for functions provided by XMLTools.h.
 * Exit values: 0, if all tests are ok, 1 otherwise.
 * On error, a diagnostic message is displayed to stdout.
 *
 * <h3>Tested functions</h3>
 * bool fileExists(const std::string& file);
 */
int main(int argc, char **argv) {
	bool failed=false;
	///////////////////////////////////////////////////////////////////////////
	// bool fileExists(const std::string& file);
	test_bool_false( fileExists("aQuiteImprobableFilename@/*:") );
#ifndef TESTS_PERMISSIVE
	// "." exists, but is a directory:
	test_bool_false( fileExists(".") );
#endif
	// this program certainly exists:
	test_bool_true( argv[0] );
#ifndef TESTS_PERMISSIVE
#ifdef WIN32
	//Windows specific tests:
	// "\\" exists, but is a directory:
	test_bool_false( fileExists("\\") );
#else
	//Unix specific tests:
	// "/" exists, but is a directory:
	test_bool_false( fileExists("/") );
#endif
#endif

	return (failed) ? 1 : 0;
}
