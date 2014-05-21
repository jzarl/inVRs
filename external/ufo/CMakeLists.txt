# The name of our project is "UFO". CMakeLists files in this project can
# refer to the root source directory of the project as ${UFO_SOURCE_DIR} and
# to the root binary directory of the project as ${UFO_BINARY_DIR}.
###############################################################################
# Set Policies:
###############################################################################
cmake_minimum_required (VERSION 2.8.7)
# add_target_library(<target> /path/to/libfoo.so bar)
# creates linker-flags: /path/to/libfoo.so -lbar
# NOT: -L/path/to -lfoo -lbar
cmake_policy(SET CMP0003 NEW)
# Preprocessor definition values are now escaped automatically:
cmake_policy(SET CMP0005 NEW)

###############################################################################
if ( INVRS_SOURCE_DIR )
	#integrate with inVRs:

	set ( UFO_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
	# As long as these paths are not absolute, they are relative to 
	# CMAKE_INSTALL_PREFIX:
	set (UFO_TARGET_LIB_DIR  ${INVRS_TARGET_LIB_DIR})
	set (UFO_TARGET_INCLUDE_DIR  ${INVRS_TARGET_INCLUDE_DIR})
	set (UFO_TARGET_BIN_DIR  ${INVRS_TARGET_BIN_DIR})
	set (UFO_TARGET_DOC_DIR ${INVRS_TARGET_DOC_DIR}/libufo )

	set (UFO_ENABLE_TESTING ${INVRS_ENABLE_TESTING})

	# build inside of inVRs
	# i.e. we don't have a fully installed inVRs, but we can use inVRs build internals
	set ( UFO_HAVE_inVRs ON )
	# tell build system to directly use inVRs targets:
	set ( UFO_inVRs_internal ON )
	
	# this is needed for Ufo.h.in:
	if ( INVRS_ENABLE_MODULE_3DPHYSICS )
		set ( UFO_HAVE_inVRs_3DPhysics 1 )
	endif()
else ( INVRS_SOURCE_DIR )
	# Project Information:
	project (UFO)

	option(UFO_ENABLE_TESTING "Enable unit tests for ufo" OFF)
	if (UFO_ENABLE_TESTING)
		# enable rules to make ctest-targets:
		enable_testing()
	endif (UFO_ENABLE_TESTING)

	include (${UFO_SOURCE_DIR}/user.cmake OPTIONAL)

	# As long as these paths are not absolute, they are relative to 
	# CMAKE_INSTALL_PREFIX:
	set (UFO_TARGET_LIB_DIR lib)
	set (UFO_TARGET_INCLUDE_DIR include)
	set (UFO_TARGET_BIN_DIR bin)
	set (UFO_TARGET_DOC_DIR share/doc/libufo)

	# only add this when we do a standalone build:
	configure_file(
		"${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
		"${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake"
		IMMEDIATE @ONLY)
	add_custom_target(uninstall
		"${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake")

	# setup required packages (only when we don't do an inVRs-internal build,
	# so we don't search everything twice):
	include (${UFO_SOURCE_DIR}/cmake/config.cmake)
endif ( INVRS_SOURCE_DIR )
###############################################################################

###############################################################################
# Set Compiler flags
###############################################################################
# UFO_EXPORT_DEFINITIONS: flags, which should be used by programs linking against ufo
# UFO_EXPORT_INCLUDE_DIRS: include-directories needed when including ufo header-files
set (UFO_EXPORT_INCLUDE_DIRS ${GMTL_INCLUDE_DIRS} )
set (UFO_EXPORT_DEFINITIONS -rdynamic )

# this allows not to include ufo/Ufo.h during build:
#add_definitions ( -DUFO_BUILD_INTERNAL )

# don't overwrite inVRs CMAKE settings:
if ( NOT INVRS_SOURCE_DIR )
	if (NOT WIN32)
		set (CMAKE_CXX_FLAGS "-Wall -pedantic -Wno-variadic-macros ")
		set (CMAKE_CXX_FLAGS_RELEASE "-O3")
		set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g")
		set (CMAKE_CXX_FLAGS_DEBUG "-O0 -g -DDEBUG")
	else (NOT WIN32) # the following platforms are not tested yet:
		message (FATAL_ERROR "Found unsupported operating system: Only Linux systems are supported!")  
	endif (NOT WIN32)
	set (CMAKE_CXX_FLAGS_RELEASE "${UFO_CXX_FLAGS} ${UFO_EXPORT_DEFINITIONS} ${CMAKE_CXX_FLAGS_RELEASE} ${USERDEFINED_CXX_FLAGS}")
	set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${UFO_CXX_FLAGS} ${UFO_EXPORT_DEFINITIONS} ${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${USERDEFINED_CXX_FLAGS}")
	set (CMAKE_CXX_FLAGS_DEBUG "${UFO_CXX_FLAGS} ${UFO_EXPORT_DEFINITIONS} ${CMAKE_CXX_FLAGS_DEBUG} ${USERDEFINED_CXX_FLAGS}")
endif ( NOT INVRS_SOURCE_DIR )

include_directories (${UFO_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${GMTL_INCLUDE_DIRS})


###############################################################################
# UFO components/sources are in subdirectories:
###############################################################################
add_subdirectory (ufo)
add_subdirectory (ufoplugins)
if ( UFO_ENABLE_TESTING )
	add_subdirectory (test)
endif ( UFO_ENABLE_TESTING )

if ( UFO_HAVE_inVRs )
	add_subdirectory ( inVRs )
endif ( UFO_HAVE_inVRs )


###############################################################################
# General install-targets:
###############################################################################
install( FILES README README.gmtl README.inVRs sampleconfigplain.txt HOWTO.inVRs
	DESTINATION ${UFO_TARGET_DOC_DIR} )
