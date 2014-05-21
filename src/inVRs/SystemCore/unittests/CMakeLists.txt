################################################################################
# general prefix for test-names:
################################################################################

set (TEST_PREFIX "INVRS_SYSTEMCORE_" )
set (TEST_LINK_LIBRARIES inVRsSystemCore)
	

################################################################################
# define tests
################################################################################

function(add_runtime TESTNAME)
	if (MSVC10 OR MSVC11)
		set(RUNTIME_LIBRARY_DIRS_DEBUG "${CMAKE_BINARY_DIR}/src/inVRs/SystemCore/Debug;${OPENSG_ROOT}/bin/debug" CACHE PATH "path to debug dll")
		set(RUNTIME_LIBRARY_DIRS_RELEASE "${CMAKE_BINARY_DIR}/src/inVRs/SystemCore/Release;${OPENSG_ROOT}/bin/rel" CACHE PATH "path to release dll")
		configure_file("cmake/Modules/template_vs10-11.vcxproj.user.in" "${CMAKE_CURRENT_BINARY_DIR}/${TESTNAME}.vcxproj.user" @ONLY)
	endif()
endfunction()

macro(add_my_test testname testsources parameters)
	# add target for test
	add_executable ( ${testname} ${testsources} )
	# make test dependant on libSystemCore
	target_link_libraries ( ${testname} ${TEST_LINK_LIBRARIES} )
	# add test:
	add_test ( ${TEST_PREFIX}${testname} ${testname} ${parameters} )
	add_runtime(${testname})
endmacro(add_my_test)

#set_tests_properties ( ${TEST_PREFIX}sometest PROPERTIES WILL_FAIL true )
add_runtime("../RUN_TESTS")
add_runtime("RUN_TESTS")

add_my_test(testUtilityFunctions testUtilityFunctions.cpp "")
add_my_test(testXMLTools testXMLTools.cpp "")

# more complex stuff:
add_library(testPlugins_lib SHARED testPlugins_lib.cpp)
target_link_libraries(testPlugins_lib inVRsSystemCore )
get_target_property(testPlugins_lib_DSOFILE testPlugins_lib LOCATION)
#message("before: ${testPlugins_lib_DSOFILE}")
#file(TO_NATIVE_PATH "${testPlugins_lib_DSOFILE}" testPlugins_lib_DSOFILE)
#message("after: ${testPlugins_lib_DSOFILE}")
add_my_test(testPlugins testPlugins.cpp ${testPlugins_lib_DSOFILE})

