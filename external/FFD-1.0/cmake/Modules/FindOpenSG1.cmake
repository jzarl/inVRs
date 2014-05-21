# Find OpenSG Module
#
# This module defines:
#  OpenSG_FOUND - if false OpenSG could not be found
#  OpenSG_DEFINITIONS - Compiler flags for OpenSG (release version)
#  OpenSG_DEFINITIONS_RELEASE - same as above
#  OpenSG_DEFINITIONS_DEBUG - Compiler flags for OpenSG (debug version)
#  OpenSG_INCLUDE_DIR - defines the folder to the OpenSG Header files
#  OpenSG_INCLUDE_DIRS - same as above
#  OpenSG_LIBRARIES - all OpenSG libraries (release version)
#  OpenSG_LIBRARIES_RELEASE - same as OpenSG_LIBRARIES
#  OpenSG_LIBRARIES_DEBUG - all OpenSG libraries (debug version)
#  OpenSG_LIBRARY_DIRS - Library directory for release libraries
#  OpenSG_LIBRARY_DIRS_RELEASE - same as above
#  OpenSG_LIBRARY_DIRS_DEBUG - Library directory for debug libraries
#
# To aid the detection process you can set the following variables:
#  OpenSG_ROOT_DIR - main directory to the OpenSG installation
#
###############################################################################
# CHANGES:
# - 25.03.2010 (rlander): created
###############################################################################


###############################################################################
# Include required methods
###############################################################################
include (FindPackageHandleStandardArgs)


###############################################################################
# Set default paths for OpenSG_ROOT_DIR
###############################################################################
if (WIN32)
	set (OpenSG_DEFAULT_PATHS C:/Programme/OpenSG C:/Program\ Files/OpenSG)
elseif (UNIX)
	set (OpenSG_DEFAULT_PATHS /usr/local)
elseif (APPLE AND ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set (OpenSG_DEFAULT_PATHS /usr/local)
else (WIN32)
	message (FATAL_ERROR "Unable to identify operating system")
endif (WIN32)


###############################################################################
# Place OpenSG_ROOT_DIR into cache (in case it is defined in user.cmake)
###############################################################################
if (DEFINED OpenSG_ROOT_DIR)
	mark_as_advanced (CLEAR OpenSG_ROOT_DIR)
	set (OpenSG_ROOT_DIR ${OpenSG_ROOT_DIR} CACHE PATH
		"Install prefix of OpenSG 1.X" FORCE)
endif (DEFINED OpenSG_ROOT_DIR)


###############################################################################
# Try to find the root directory (if not set)
###############################################################################
if (NOT OpenSG_ROOT_DIR)
	if (WIN32)
		find_path (OpenSG_ROOT_DIR NAMES include/OpenSG/OSGBase.h
			HINTS ${OpenSG_DEFAULT_PATHS})	
	elseif (UNIX OR APPLE)
		find_path (OpenSG_ROOT_DIR NAMES bin/osg-config
			HINTS ${OpenSG_DEFAULT_PATHS})
	endif (WIN32)	
endif (NOT OpenSG_ROOT_DIR)


###############################################################################
# Different search strategies for WIN32 and other systems
###############################################################################
if (WIN32)

	#######################################################################
	# Try to find the include directory
	#######################################################################
	find_path (OpenSG_INCLUDE_DIR NAMES OpenSG/OSGBase.h
		HINTS ${OpenSG_ROOT_DIR}/include)

	#######################################################################
	# Try to find the OpenSG release libraries
	#######################################################################
	find_path (OpenSG_LIBRARY NAMES OSGBase
		HINTS ${OpenSG_ROOT_DIR}/lib)
	find_path (OpenSG_LIBRARY_SYSTEM NAMES OSGSystem
		HINTS ${OpenSG_ROOT_DIR}/lib)
	find_path (OpenSG_LIBRARY_WINDOWGLUT NAMES OSGWindowGLUT
		HINTS ${OpenSG_ROOT_DIR}/lib)
	find_path (OpenSG_LIBRARY_WINDOWQT NAMES OSGWindowQT
		HINTS ${OpenSG_ROOT_DIR}/lib)
	find_path (OpenSG_LIBRARY_WINDOWWIN32 NAMES OSGWindowWIN32
		HINTS ${OpenSG_ROOT_DIR}/lib)
	
	# TODO: check for required libraries (pass them in to module)
	set (OpenSG_LIBRARIES ${OpenSG_LIBRARY}
		${OpenSG_LIBRARY_SYSTEM}
		${OpenSG_LIBRARY_WINDOWGLUT})

	mark_as_advanced (OpenSG_LIBRARY_DEBUG
		OpenSG_LIBRARY_SYSTEM_DEBUG
		OpenSG_LIBRARY_WINDOWGLUT_DEBUG
		OpenSG_LIBRARY_WINDOWQT_DEBUG
		OpenSG_LIBRARY_WINDOWWIN32_DEBUG)

	#######################################################################
	# Obtain library directory from library
	#######################################################################
	if (OpenSG_LIBRARY)
		get_filename_component (OpenSG_LIBRARY_DIRS_RELEASE ${OpenSG_LIBRARY} PATH)
		set (OpenSG_LIBRARY_DIRS ${OpenSG_LIBRARY_DIRS_RELEASE})
	endif (OpenSG_LIBRARY)
	if (OpenSG_LIBRARY_DEBUG)
		get_filename_component (OpenSG_LIBRARY_DIRS_DEBUG ${OpenSG_LIBRARY_DEBUG} PATH)
	endif (OpenSG_LIBRARY_DEBUG)

	#######################################################################
	# Set definitions
	#######################################################################
	set (OpenSG_DEFINITIONS_DEBUG "/D WIN32 /D _LIB /D _DEBUG /D _WINDOWS /D _OSG_HAVE_CONFIGURED_H_ /D OSG_BUILD_DLL /D OSG_DEBUG /D OSG_WITH_GLUT /D OSG_WITH_GIF /D OSG_WITH_TIF /D OSG_WITH_JPG")
	set (OpenSG_DEFINITIONS_RELEASE "/D WIN32 /D _LIB /D _WINDOWS /D _OSG_HAVE_CONFIGURED_H_ /D OSG_BUILD_DLL /D OSG_WITH_GLUT /D OSG_WITH_GIF /D OSG_WITH_TIF /D OSG_WITH_JPG")
		
	#######################################################################
	# Check if libraries and headers were found
	#######################################################################
	FIND_PACKAGE_HANDLE_STANDARD_ARGS (OpenSG
		"Unable to find OpenSG_LIBRARY or OpenSG_INCLUDE_DIR! Please set the correct OpenSG_ROOT_DIR to the prefix of your OpenSG installation!"
		OpenSG_LIBRARY OpenSG_INCLUDE_DIR)
	set (OpenSG_FOUND ${OPENSG_FOUND})

else (WIN32)
	
	#######################################################################
	# Try to find the directory to osg-config
	#######################################################################
	find_path (OpenSG_BIN_DIR NAMES osg-config
		HINTS ${OpenSG_ROOT_DIR}/bin)

	mark_as_advanced (OpenSG_BIN_DIR)

	#######################################################################
	# Check if path to osg-config was found
	#######################################################################
	FIND_PACKAGE_HANDLE_STANDARD_ARGS (OpenSG
		"Unable to find OpenSG_BIN_DIR (where osg-config is installed)! Please set the correct OpenSG_ROOT_DIR to the prefix of your OpenSG installation!"
		OpenSG_BIN_DIR)
	set (OpenSG_FOUND ${OPENSG_FOUND})
	
	if (OpenSG_FOUND)
		
		# TODO: check for required libraries (pass them in to module)
		set (OPENSG_LIBS Base System GLUT)

		###############################################################
		# Determine DEFINITIONS and LIBRARIES
		###############################################################
		exec_program (${OpenSG_BIN_DIR}/osg-config
			ARGS --dbg --cflags ${OPENSG_LIBS}
			OUTPUT_VARIABLE OPENSG_CXX_FLAGS_DEBUG)
		exec_program (${OpenSG_BIN_DIR}/osg-config
			ARGS --opt --cflags ${OPENSG_LIBS}
			OUTPUT_VARIABLE OPENSG_CXX_FLAGS_RELEASE)
		exec_program (${OpenSG_BIN_DIR}/osg-config
			ARGS --dbg --libs ${OPENSG_LIBS}
			OUTPUT_VARIABLE OPENSG_LINKER_FLAGS_DEBUG)
		exec_program (${OpenSG_BIN_DIR}/osg-config
			ARGS --opt --libs ${OPENSG_LIBS}
			OUTPUT_VARIABLE OPENSG_LINKER_FLAGS_RELEASE)

		###############################################################
		# process CXX_FLAGS:
		###############################################################
		foreach (_BUILD_TYPE RELEASE DEBUG)
			# Set variable for required cxx flags
			set (tmp ${OPENSG_CXX_FLAGS_${_BUILD_TYPE}})
			# remove -I entries
			string( REGEX REPLACE "\ -[I][^ ]*" "" tmp " ${tmp}")
			# set definitions
			string (STRIP ${tmp} OpenSG_DEFINITIONS_${_BUILD_TYPE})
			#set (OpenSG_DEFINITIONS_${_BUILD_TYPE} ${tmp})

			# Set variable for required cxx flags
			set (tmp ${OPENSG_CXX_FLAGS_${_BUILD_TYPE}})
			# Get -I entries
			string( REGEX REPLACE "\ -[^I][^ ]*" "" tmp " ${tmp}")
			# Remove -I from result
			string( REPLACE "-I" "" tmp "${tmp}")
			# Set include dirs
			string (REGEX MATCHALL "[^ ]+" OpenSG_INCLUDE_DIRS "${tmp}")
			#string (STRIP ${tmp} OpenSG_INCLUDE_DIRS)
			#set (OpenSG_INCLUDE_DIRS ${tmp})

			# Process linker flags
			set (tmp ${OPENSG_LINKER_FLAGS_${_BUILD_TYPE}})
			# Remove everything but -l flags
			string( REGEX REPLACE "\ -[^l][^ ]*" "" tmp " ${tmp}")
			# Remove -l from result
			string( REPLACE "-l" "" tmp "${tmp}")
			# Set libraries
			string (REGEX MATCHALL "[^ ]+" OpenSG_LIBRARIES_${_BUILD_TYPE} "${tmp}")
			#string (STRIP ${tmp} OpenSG_LIBRARIES_${_BUILD_TYPE})
			#set (OpenSG_LIBRARIES_${_BUILD_TYPE} ${tmp})

			# Process linker flags
			set (tmp ${OPENSG_LINKER_FLAGS_${_BUILD_TYPE}})
			# Remove everything but -L flags
			string( REGEX REPLACE "\ -[^L][^ ]*" "" tmp " ${tmp}")
			# Remove -L from result
			string( REPLACE "-L" "" tmp "${tmp}")
			# Set library dirs
			string (REGEX MATCHALL "[^ ]+" OpenSG_LIBRARY_DIRS_${_BUILD_TYPE} "${tmp}")
			#string (STRIP ${tmp} OpenSG_LIBRARY_DIRS_${_BUILD_TYPE})
			#set (OpenSG_LIBRARY_DIRS ${tmp})
		endforeach (_BUILD_TYPE)
		
		###############################################################
		# Get missing variables from obtained values
		###############################################################
		set (OpenSG_LIBRARY_DIRS ${OpenSG_LIBRARY_DIRS_RELEASE})
		list (GET OpenSG_INCLUDE_DIRS 0 OpenSG_INCLUDE_DIR)
		set (OpenSG_DEFINITIONS ${OpenSG_DEFINITIONS_RELEASE})
		set (OpenSG_LIBRARIES ${OpenSG_LIBRARIES_RELEASE})

	endif (OpenSG_FOUND)

	mark_as_advanced (FORCE OpenSG_ROOT_DIR)
	
endif (WIN32)

