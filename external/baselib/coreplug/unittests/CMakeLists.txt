################################################################################
# general prefix for test-names:
################################################################################
set (TEST_PREFIX "${TEST_PREFIX}COREPLUG_" )
set (TEST_LINK_LIBRARIES coreplug)
include_directories(${baselib_SOURCE_DIR})
# for export files:
include_directories(${CMAKE_CURRENT_BINARY_DIR})

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

macro(add_my_test testname testsources)
	# add target for test
	add_executable ( ${testname} ${testsources} )
	# make test dependant on libSystemCore
	target_link_libraries ( ${testname} ${TEST_LINK_LIBRARIES} )
	# add test:
	add_test ( ${TEST_PREFIX}${testname} ${testname} ${ARGN} )
	# for win32:
	add_runtime(${testname})
endmacro(add_my_test)



# 1. testDso
add_library(testDso_dso SHARED "testDso_dso.cxx")
get_target_property(testDso_dso_LOCATION testDso_dso LOCATION)
add_my_test(testDso "testDso.cxx" "${testDso_dso_LOCATION}")
generate_export_header(testDso_dso)

# 2. testFactoryPlugin
add_definitions(-DtestFactoryPluginVERSION="1")

add_library(testFactoryPluginLib SHARED "${CMAKE_CURRENT_SOURCE_DIR}/testFactoryPlugin/testFactoryPlugin-factory.cxx")
generate_export_header(testFactoryPluginLib)

add_library(testFactoryPlugin-plugin MODULE "${CMAKE_CURRENT_SOURCE_DIR}/testFactoryPlugin/testFactoryPlugin-plugin.cxx")
target_link_libraries(testFactoryPlugin-plugin testFactoryPluginLib)
get_target_property(testFactoryPlugin-plugin_LOCATION testFactoryPlugin-plugin LOCATION)

add_library(testFactoryPlugin-macroplugin MODULE "${CMAKE_CURRENT_SOURCE_DIR}/testFactoryPlugin/testFactoryPlugin-macroplugin.cxx")
target_link_libraries(testFactoryPlugin-macroplugin testFactoryPluginLib)
get_target_property(testFactoryPlugin-macroplugin_LOCATION testFactoryPlugin-macroplugin LOCATION)

list(APPEND TEST_LINK_LIBRARIES testFactoryPluginLib)
add_my_test(testFactoryPlugin "${CMAKE_CURRENT_SOURCE_DIR}/testFactoryPlugin/testFactoryPlugin.cxx" "${testFactoryPlugin-plugin_LOCATION}" "${testFactoryPlugin-macroplugin_LOCATION}")
list(REMOVE_ITEM TEST_LINK_LIBRARIES testFactoryPluginLib)
