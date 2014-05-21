# The name of our project is "inVRs". CMakeLists files in this project can
# refer to the root source directory of the project as ${inVRs_SOURCE_DIR} and
# to the root binary directory of the project as ${inVRs_BINARY_DIR}.
cmake_minimum_required (VERSION 2.8.3)
project (INVRS)

if (CMAKE_MAJOR_VERSION EQUAL 2 AND CMAKE_MINOR_VERSION EQUAL 4)
	set (CMAKE_VERSION_24 TRUE)
endif (CMAKE_MAJOR_VERSION EQUAL 2 AND CMAKE_MINOR_VERSION EQUAL 4)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT OR NOT CMAKE_INSTALL_PREFIX)
	set (CMAKE_INSTALL_PREFIX ${INVRS_SOURCE_DIR}
		CACHE PATH "inVRs install prefix" FORCE)
endif (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT OR NOT CMAKE_INSTALL_PREFIX)

set (INVRS_TARGET_LIB_DIR ${CMAKE_INSTALL_PREFIX}/lib)
set (INVRS_TARGET_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)
set (INVRS_TARGET_BIN_DIR ${CMAKE_INSTALL_PREFIX}/bin)
set (INVRS_TARGET_DOC_DIR ${CMAKE_INSTALL_PREFIX}/share/doc/inVRs)

# use predefined configuration if found:
include (${INVRS_SOURCE_DIR}/user.cmake)
# use predefined per-host configuration if found:
if(WIN32)
	string(TOLOWER $ENV{COMPUTERNAME} COMPUTERNAME)
elseif(UNIX)
	string(TOLOWER "$ENV{HOSTNAME}" COMPUTERNAME)
endif(WIN32)
include (${INVRS_SOURCE_DIR}/user.${COMPUTERNAME}.cmake OPTIONAL)

# search for cmake modules in this directory:
set (CMAKE_MODULE_PATH ${INVRS_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# Set a default build type if no build type is set
# (CACHE variables are only set if they don't already have a value)
set (CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type (Release/Debug)")
# when first generating the cache, cache-variables are not instantly available (only at subsequent runs):
if (NOT CMAKE_BUILD_TYPE)
	set (CMAKE_BUILD_TYPE Release)
endif (NOT CMAKE_BUILD_TYPE)
if ( NOT ${CMAKE_VERSION} VERSION_LESS "2.8" )
	# cmake >= 2.8 provides combo-boxes:
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Release Debug)
endif ( NOT ${CMAKE_VERSION} VERSION_LESS "2.8" )


# Set the default install prefix to the inVRs directory if not already set in
# user.cmake
if (NOT INVRS_INSTALL_PREFIX)
	set (INVRS_INSTALL_PREFIX ${INVRS_SOURCE_DIR})
endif (NOT INVRS_INSTALL_PREFIX)

# Automatic enabling/disabling of build-features:
include(AutoFeature)
###############################################################################
# Set up CTest
###############################################################################
# (this must be done AFTER setting CMAKE_MODULE_PATH)
option (INVRS_ENABLE_TESTING "Enable CTest test-drivers functionality checks in inVRs." ON)
if ( INVRS_ENABLE_TESTING )
	# make sure that OpenSG version can be determined:
	find_package(OpenSG QUIET REQUIRED)
	# set useful build name for CDash:
	set(BUILDNAME "${CMAKE_SYSTEM_NAME}-OpenSG${OpenSG_MAJOR_VERSION}-${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}")
	message(STATUS "Build name: ${BUILDNAME}")
	include(CTest)
	mark_as_advanced(BUILD_TESTING)
else ( INVRS_ENABLE_TESTING )
	# force BUILD_TESTING to off, if INVRS_ENABLE_TESTING is false:
	set(BUILD_TESTING CACHE INTERNAL FORCE)
endif ( INVRS_ENABLE_TESTING )

###############################################################################
# Find IrrXML include path
###############################################################################
set (IRRXML_INCLUDE_DIR ${INVRS_SOURCE_DIR}/external/irrxml-1.2/src)
#will be installed into target dir which is already appended
#list (APPEND INVRS_EXPORT_INCLUDE_DIRS ${INVRS_TARGET_INCLUDE_DIR}/irrxml )
include_directories(${IRRXML_INCLUDE_DIR})

###############################################################################
# Find GMTL include path
###############################################################################
set (GMTL_INCLUDE_DIR ${INVRS_SOURCE_DIR}/external/gmtl-0.6.1)
#will be installed into target dir which is already appended
#can be skipped because the normal direcotry is already there
#list (APPEND INVRS_EXPORT_INCLUDE_DIRS ${INVRS_TARGET_INCLUDE_DIR}) 
include_directories(${GMTL_INCLUDE_DIR})


# Set Compiler flags
if (NOT WIN32)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC ${USERDEFINED_CXX_FLAGS}")
	set (CMAKE_CXX_FLAGS_PROFILE "${CMAKE_CXX_FLAGS_PROFILE} -O3 -g -DNDEBUG -fPIC ${USERDEFINED_CXX_FLAGS}")
	set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O3 -g -DNDEBUG -fPIC ${USERDEFINED_CXX_FLAGS}")
	list (APPEND INVRS_EXPORT_DEFINITIONS "-fPIC")
else (NOT WIN32)
	if (UNIX)
		#mingw, cygwin etc.
		message (FATAL_ERROR "Found unsupported operating system: Only Windows and Linux systems are supported!")
	else (UNIX)
		add_definitions(-D_WINSOCKAPI_)
	endif (UNIX)
endif (NOT WIN32)

# set formatting for eclipse output
if (CMAKE_COMPILER_IS_GNUCC)
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0")
endif (CMAKE_COMPILER_IS_GNUCC)
if (CMAKE_COMPILER_IS_GNUCXX)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
endif (CMAKE_COMPILER_IS_GNUCXX)

###############################################################################
# inVRs include path
###############################################################################
include_directories(${INVRS_SOURCE_DIR}/src)
include_directories(${INVRS_SOURCE_DIR}/tools/libraries)

###############################################################################
# Platform tests (query environment features, set appropriate definitions)
###############################################################################
include(${INVRS_SOURCE_DIR}/cmake/platformtests.cmake)


# allow header-files to differ between build-time and installed-time
# i.e. some header files use #ifdefs, which are then "baked" into a
# separate header-file, which is only included when inVRs is installed:
# #ifdef INVRS_BUILD_TIME
# # include "config.h"
# #endif
add_definitions( -DINVRS_BUILD_TIME )

# save INVRS_EXPORT_* definitions, so another project can use them:
list ( APPEND INVRS_EXPORT_LIBRARY_DIRS ${INVRS_TARGET_LIB_DIR} )
list ( APPEND INVRS_EXPORT_INCLUDE_DIRS ${INVRS_TARGET_INCLUDE_DIR} )
list ( APPEND INVRS_EXPORT_INCLUDE_DIRS ${INVRS_TARGET_INCLUDE_DIR}/irrXML )
list ( APPEND INVRS_EXPORT_INCLUDE_DIRS ${INVRS_TARGET_INCLUDE_DIR}/inVRs/tools/libraries )
if (INVRS_EXPORT_INCLUDE_DIRS AND NOT CMAKE_VERSION_24)
	list ( REMOVE_DUPLICATES INVRS_EXPORT_INCLUDE_DIRS )
endif (INVRS_EXPORT_INCLUDE_DIRS AND NOT CMAKE_VERSION_24)
if (INVRS_EXPORT_LIBRARY_DIRS AND NOT CMAKE_VERSION_24)
	list ( REMOVE_DUPLICATES INVRS_EXPORT_LIBRARY_DIRS )
endif (INVRS_EXPORT_LIBRARY_DIRS AND NOT CMAKE_VERSION_24)
if (INVRS_EXPORT_DEFINITIONS AND NOT CMAKE_VERSION_24)
	list ( REMOVE_DUPLICATES INVRS_EXPORT_DEFINITIONS )
endif (INVRS_EXPORT_DEFINITIONS AND NOT CMAKE_VERSION_24)
if (INVRS_EXPORT_LIBRARIES AND NOT CMAKE_VERSION_24)
	list ( REMOVE_DUPLICATES INVRS_EXPORT_LIBRARIES )
endif (INVRS_EXPORT_LIBRARIES AND NOT CMAKE_VERSION_24)

set ( INVRS_EXPORTS_FILE "${CMAKE_CURRENT_BINARY_DIR}/config_inVRs.cmake" )
macro(INVRS_ADD_EXPORTS _module )
	file ( APPEND ${INVRS_EXPORTS_FILE} "\n# definitions from ${CMAKE_CURRENT_SOURCE_DIR}:\n" )
	foreach ( var INCLUDE_DIRS LIBRARY_DIRS DEFINITIONS LIBRARIES )
		file ( APPEND ${INVRS_EXPORTS_FILE} "set (INVRS_EXPORT_${_module}_${var} \"${INVRS_EXPORT_${_module}_${var}}\" )\n" )
	endforeach( var INCLUDE_DIRS LIBRARY_DIRS DEFINITIONS LIBRARIES )
endmacro(INVRS_ADD_EXPORTS _module )
configure_file (
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/config_inVRs.cmake.in"
	${INVRS_EXPORTS_FILE}
	@ONLY )
install ( FILES ${INVRS_EXPORTS_FILE}
	DESTINATION ${INVRS_TARGET_INCLUDE_DIR}/inVRs)


# Recurse into the defined subdirectories. This does not actually
# cause another cmake executable to run. The same process will walk through
# the project's entire directory structure.

add_subdirectory (external)

add_subdirectory (src/inVRs/SystemCore)
add_subdirectory (src/inVRs/InputInterface/ControllerManager)
add_subdirectory (src/inVRs/OutputInterface/OpenSGSceneGraphInterface)

add_subdirectory (tools/libraries)
add_subdirectory (tools/applications)

add_subdirectory (src/inVRs/Modules)

set(BUILD_tutorials OFF CACHE BOOL "example applications")
if(BUILD_tutorials)
	add_subdirectory(tutorials)
endif()

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake"
	IMMEDIATE @ONLY)

add_custom_target(uninstall
	"${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake")

# add documentation files to install:
install( FILES Changelog.txt COPYING COPYING.LESSER LICENSE README
	DESTINATION ${INVRS_TARGET_DOC_DIR} )

include(compatibility)
include(configSummary)
