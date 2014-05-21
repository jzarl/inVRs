# The name of our project is "FFD". CMakeLists files in this project can
# refer to the root source directory of the project as ${FFD_SOURCE_DIR} and
# to the root binary directory of the project as ${FFD_BINARY_DIR}.
###############################################################################
# Set Policies:
###############################################################################
cmake_minimum_required (VERSION 2.6)
# add_target_library(<target> /path/to/libfoo.so bar)
# creates linker-flags: /path/to/libfoo.so -lbar
# NOT: -L/path/to -lfoo -lbar
cmake_policy(SET CMP0003 NEW)
# Preprocessor definition values are now escaped automatically:
cmake_policy(SET CMP0005 NEW)

###############################################################################
if ( INVRS_SOURCE_DIR )
	#integrate with inVRs:

	set ( FFD_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
	# As long as these paths are not absolute, they are relative to 
	# CMAKE_INSTALL_PREFIX:
	set (FFD_TARGET_LIB_DIR  ${INVRS_TARGET_LIB_DIR})
	set (FFD_TARGET_INCLUDE_DIR  ${INVRS_TARGET_INCLUDE_DIR})
	set (FFD_TARGET_BIN_DIR  ${INVRS_TARGET_BIN_DIR})
	set (FFD_TARGET_DOC_DIR ${INVRS_TARGET_DOC_DIR}/libFFD )
	
	# build inside of inVRs
	# i.e. we don't have a fully installed inVRs, but we can use inVRs build internals
	set ( FFD_HAVE_inVRs ON )
	# tell build system to directly use inVRs targets:
	set ( FFD_inVRs_internal ON )
else ( INVRS_SOURCE_DIR )
	# Project Information:
	project (FFD)

	include (${FFD_SOURCE_DIR}/user.cmake OPTIONAL)

	# As long as these paths are not absolute, they are relative to 
	# CMAKE_INSTALL_PREFIX:
	set (FFD_TARGET_LIB_DIR lib)
	set (FFD_TARGET_INCLUDE_DIR include)
	set (FFD_TARGET_BIN_DIR bin)
	set (FFD_TARGET_DOC_DIR share/doc/libFFD)

	# only add this when we do a standalone build:
	configure_file(
		"${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
		"${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake"
		IMMEDIATE @ONLY)
	add_custom_target(uninstall
		"${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake")

	# setup required packages (only when we don't do an inVRs-internal build,
	# so we don't search everything twice):
	include (${FFD_SOURCE_DIR}/cmake/config.cmake)
endif ( INVRS_SOURCE_DIR )
###############################################################################


add_subdirectory (FFDlib)

option (FFD_ENABLE_SAMPLE "Build FFD sample?" FALSE)
mark_as_advanced(FFD_ENABLE_SAMPLE)
if ( FFD_ENABLE_SAMPLE )
	message(STATUS "building FFD sample")
	
	# Find GLUT headers and libraries used by the sample
	# do this here instead of config.cmake, so it works with inVRs, too:
	find_package(GLUT REQUIRED)
	
	add_subdirectory (FFDsample)
else(FFD_ENABLE_SAMPLE)
	message(STATUS "not building FFD sample")
endif ( FFD_ENABLE_SAMPLE )

if ( FFD_HAVE_inVRs )
	message(STATUS "building FFDinVRsLib")
	add_subdirectory(FFDinVRsLib)
endif ( FFD_HAVE_inVRs )

###############################################################################
# General install-targets:
###############################################################################
install( FILES README README.gmtl README.inVRs
	DESTINATION ${FFD_TARGET_DOC_DIR} )
