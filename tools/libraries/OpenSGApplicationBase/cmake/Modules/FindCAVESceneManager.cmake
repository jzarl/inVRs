# - Find CAVESceneManager
# Find the CAVESceneManager library.
#
# This module defines:
#  CAVESceneManager_FOUND - if false, you cannot use CAVESceneManager.
#  CAVESceneManager_DEFINITIONS - compile- and link-definitions for CAVESceneManager
#  CAVESceneManager_INCLUDE_DIR - the directory in which you can find OpenSG/OSGCAVESceneManager.h
#  CAVESceneManager_INCLUDE_DIRS - the same as CAVESceneManager_INCLUDE_DIR
#  CAVESceneManager_LIBRARY - the full path of libCAVESceneManager
#  CAVESceneManager_LIBRARIES - the full path of libCAVESceneManager + names of libraries linked against
#  CAVESceneManager_LIBRARY_DIR - the directory containing libCAVESceneManager
#  CAVESceneManager_LIBRARY_DIRS - the library directories needed for CAVESceneManager_LIBRARIES
#  CAVESceneManager_FRAMEWORKS - the frameworks needed for CAVESceneManager (needed on MAC systems)
#
# To aid in the detection_process, you can set the following variables:
#  CAVESceneManager_ROOT_DIR - give a hint, which directory to use (=install prefix of CAVESceneManager)
#                 if this is not set, the environment variable CAVESceneManager_ROOT_DIR is used, if available.
#
# Examples:
# set ( CAVESceneManager_ROOT_DIR /usr/local )
#
###############################################################################
# The following variables are used internally and unset after use:
###############################################################################
# CHANGES:
# - 13.10.2009 (rlander): created
###############################################################################

# check for CMAKE 24 version
if (CMAKE_MAJOR_VERSION EQUAL 2 AND CMAKE_MINOR_VERSION EQUAL 4)
  set (CMAKE_VERSION_24 TRUE)
endif (CMAKE_MAJOR_VERSION EQUAL 2 AND CMAKE_MINOR_VERSION EQUAL 4)

#make cache-entry:
set ( CAVESceneManager_ROOT_DIR ${CAVESceneManager_ROOT_DIR} CACHE PATH "Install prefix of the CAVESceneManager library")

# honor environment variables:
if ( NOT CAVESceneManager_ROOT_DIR )
	set ( CAVESceneManager_ROOT_DIR $ENV{CAVESceneManager_ROOT_DIR} )
endif ( NOT CAVESceneManager_ROOT_DIR )


###############################################################################
# find CAVESceneManager itself:
###############################################################################
# try to find the include directory:
# CMAKE 2.4.x compatibility
if ( CMAKE_VERSION_24 )
	find_path( CAVESceneManager_INCLUDE_DIR NAMES OpenSG/OSGCAVESceneManager.h
		PATHS ${CAVESceneManager_ROOT_DIR}/include 
		DOC "The directory in which you can find OpenSG/OSGCAVESceneManager.h" )
else ( CMAKE_VERSION_24 )
	find_path( CAVESceneManager_INCLUDE_DIR NAMES OpenSG/OSGCAVESceneManager.h
		HINTS ${CAVESceneManager_ROOT_DIR}/include 
		DOC "The directory in which you can find OpenSG/OSGCAVESceneManager.h" )
endif ( CMAKE_VERSION_24 )

# try to find the library:
# CMAKE 2.4.x compatibility
if ( CMAKE_VERSION_24 )
	find_library( CAVESceneManager_LIBRARY NAMES CAVESceneManager
		PATHS ${CAVESceneManager_ROOT_DIR}/lib 
		DOC "The directory in which you can find libCAVESceneManager.so")
else ( CMAKE_VERSION_24 )
	find_library( CAVESceneManager_LIBRARY NAMES CAVESceneManager
		HINTS ${CAVESceneManager_ROOT_DIR}/lib 
		DOC "The directory in which you can find libCAVESceneManager.so")
endif ( CMAKE_VERSION_24 )

# set the library directory:
get_filename_component(CAVESceneManager_LIBRARY_DIR ${CAVESceneManager_LIBRARY} PATH )

MARK_AS_ADVANCED(
	CAVESceneManager_INCLUDE_DIR
	CAVESceneManager_LIBRARY
	CAVESceneManager_LIBRARY_DIR
	)

###############################################################################
# set default stuff:
###############################################################################
# handle the QUIETLY and REQUIRED arguments and set CAVESceneManager_FOUND to TRUE if
# all listed variables are TRUE
# CMAKE 2.4.x support
if ( NOT CMAKE_VERSION_24 )
	include( FindPackageHandleStandardArgs OPTIONAL RESULT_VARIABLE FindPackageHandleStandardsArgs_INCLUDED )
endif ( NOT CMAKE_VERSION_24 )

if ( FindPackageHandleStandardsArgs_INCLUDED )
	FIND_PACKAGE_HANDLE_STANDARD_ARGS(CAVESceneManager DEFAULT_MSG CAVESceneManager_LIBRARY CAVESceneManager_INCLUDE_DIR)
else ( FindPackageHandleStandardsArgs_INCLUDED )
	set ( CAVESceneManager_FOUND TRUE )
	if ( NOT CAVESceneManager_LIBRARY )
		message( FATAL_ERROR "Unable to find CAVESceneManager_LIBRARY! Please set the correct CAVESceneManager_ROOT_DIR to the prefix of your CAVESceneManager installation!")
		set ( CAVESCENEMANAGER_FOUND FALSE )
	endif ( NOT CAVESceneManager_LIBRARY )
	if ( NOT CAVESceneManager_INCLUDE_DIR )
		message( FATAL_ERROR "Unable to find CAVESceneManager_INCLUDE_DIR! Please set the correct CAVESceneManager_ROOT_DIR to the place of your CAVESceneManager installation!")
		set ( CAVESCENEMANAGER_FOUND FALSE )
	endif ( NOT CAVESceneManager_INCLUDE_DIR )
endif ( FindPackageHandleStandardsArgs_INCLUDED )

if ( CAVESCENEMANAGER_FOUND )
	set ( CAVESceneManager_FOUND true ) # FIND_PACKAGE_HANDLE_STANDARD_ARGS only sets CAVESCENEMANAGER_FOUND
	set ( CAVESceneManager_INCLUDE_DIRS ${CAVESceneManager_INCLUDE_DIR} )
	set ( CAVESceneManager_LIBRARIES ${CAVESceneManager_LIBRARY} )
	set ( CAVESceneManager_LIBRARY_DIRS ${CAVESceneManager_LIBRARY_DIR} )
endif (CAVESCENEMANAGER_FOUND )

###############################################################################
# Add compile-flags and definitions:
###############################################################################

# CMAKE 2.4.x support
if ( CMAKE_VERSION_24 )
	include( ${CAVESceneManager_INCLUDE_DIR}/CAVESceneManager/config_CAVESceneManager.cmake OPTIONAL )
	if ( CAVESCENEMANAGER_EXPORT_INCLUDE_DIRS OR CAVESCENEMANAGER_EXPORT_LIBRARY_DIRS )
		set ( CAVESceneManager_EXPORT_INCLUDED TRUE )
	endif ( CAVESCENEMANAGER_EXPORT_INCLUDE_DIRS OR CAVESCENEMANAGER_EXPORT_LIBRARY_DIRS )
else ( CMAKE_VERSION_24 )
	include( ${CAVESceneManager_INCLUDE_DIR}/CAVESceneManager/config_CAVESceneManager.cmake OPTIONAL RESULT_VARIABLE CAVESceneManager_EXPORT_INCLUDED )
endif ( CMAKE_VERSION_24 )

if ( CAVESceneManager_EXPORT_INCLUDED )
	if ( CAVESCENEMANAGER_EXPORT_INCLUDE_DIRS )
		list ( APPEND CAVESceneManager_INCLUDE_DIRS ${CAVESCENEMANAGER_EXPORT_INCLUDE_DIRS} )
	endif ( CAVESCENEMANAGER_EXPORT_INCLUDE_DIRS )
	if ( CAVESCENEMANAGER_EXPORT_LIBRARY_DIRS )
		list ( APPEND CAVESceneManager_LIBRARY_DIRS ${CAVESCENEMANAGER_EXPORT_LIBRARY_DIRS} )
	endif ( CAVESCENEMANAGER_EXPORT_LIBRARY_DIRS )
	if ( CAVESCENEMANAGER_EXPORT_LIBRARIES )
		list ( APPEND CAVESceneManager_LIBRARIES ${CAVESCENEMANAGER_EXPORT_LIBRARIES} )
	endif ( CAVESCENEMANAGER_EXPORT_LIBRARIES )
	set ( CAVESceneManager_DEFINITIONS ${CAVESCENEMANAGER_EXPORT_DEFINITIONS} )
	set ( CAVESceneManager_FRAMEWORKS ${CAVESCENEMANAGER_EXPORT_FRAMEWORKS} )
	# remove internal variables:
#RLANDER: removed unsets for compatibility with different cmake versions
#	unset ( CAVESceneManager_EXPORT_INCLUDE_DIRS )
#	unset ( CAVESceneManager_EXPORT_LIBRARY_DIRS )
#	unset ( CAVESceneManager_EXPORT_LIBRARIES )
#	unset ( CAVESceneManager_EXPORT_DEFINITIONS )
endif ( CAVESceneManager_EXPORT_INCLUDED )

MARK_AS_ADVANCED(
	CAVESceneManager_FOUND
	CAVESceneManager_DEFINITIONS
	CAVESceneManager_INCLUDE_DIR
	CAVESceneManager_INCLUDE_DIRS
	CAVESceneManager_LIBRARY
	CAVESceneManager_LIBRARIES
	CAVESceneManager_LIBRARY_DIR
	CAVESceneManager_LIBRARY_DIRS 
	CAVESceneManager_FRAMEWORKS
	CAVESceneManager_EXPORT_INCLUDED
	)

