# - Find UFO
# Find the UFO library.
# Note: UFO depends on GMTL, so GMTL is normally included here automatically.
#
# This module defines:
#  UFO_FOUND - if false, you cannot use UFO.
#  UFO_INCLUDE_DIR - the directory in which you can find ufo/UfoDB.h
#  UFO_INCLUDE_DIRS - UFO_INCLUDE_DIR + GMTL_INCLUDE_DIRS
#  UFO_LIBRARY - the full path of libufo.so
#  UFO_LIBRARIES - UFO_LIBRARY
#  UFO_DEFINITIONS - definitions needed for linking against libufo
#
# To aid in the detection_process, you can set the following variables:
#  UFO_ROOT_DIR - give a hint, which directory to use (=install prefix of UFO)
#                 if this is not set, the environment variable UFO_ROOT_DIR is used, if available.
#
# Examples:
# set ( UFO_ROOT_DIR /usr/local )
###############################################################################

#create cache-entry:
set (UFO_ROOT_DIR ${UFO_ROOT_DIR} CACHE PATH "Install prefix of UFO library")

# honor environment variables:
if ( NOT UFO_ROOT_DIR )
	if ( $ENV{UFO_ROOT_DIR} )
		set ( UFO_ROOT_DIR $ENV{UFO_ROOT_DIR} )
	endif ( $ENV{UFO_ROOT_DIR} )
endif ( NOT UFO_ROOT_DIR )
#message(STATUS "UFO root directory is: ${UFO_ROOT_DIR}" )

# try to find the include directory:
find_path( UFO_INCLUDE_DIR NAMES ufo/UfoDB.h
	HINTS ${UFO_ROOT_DIR}/include 
	DOC "The directory containing ufo/UfoDB.h")
#message(STATUS "Candidate for UFO_INCLUDE_DIR: ${UFO_INCLUDE_DIR}")

# try to find the library:
find_library( UFO_LIBRARY NAMES ufo
	HINTS ${UFO_ROOT_DIR}/lib 
	DOC "The full path of libufo.so" )
#message(STATUS "Candidate for UFO_LIBRARY: ${UFO_LIBRARY}")

# handle the QUIETLY and REQUIRED arguments and set UFO_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(UFO DEFAULT_MSG UFO_LIBRARY UFO_INCLUDE_DIR)

if ( UFO_FOUND )
	set ( UFO_INCLUDE_DIRS ${UFO_INCLUDE_DIR} CACHE PATH "The include directories needed for using libufo" FORCE)
	set ( UFO_LIBRARIES ${UFO_LIBRARY} CACHE FILEPATH "The libraries needed to link against libufo" FORCE)
	include( ${UFO_INCLUDE_DIR}/ufo/config_ufo.cmake OPTIONAL RESULT_VARIABLE UFO_EXPORT_INCLUDED )
	if ( UFO_EXPORT_INCLUDED )
		list ( APPEND UFO_INCLUDE_DIRS ${UFO_EXPORT_INCLUDE_DIRS} )
		list ( APPEND UFO_LIBRARIES ${UFO_EXPORT_LIBRARIES} )
		set ( UFO_DEFINITIONS ${UFO_EXPORT_DEFINITIONS} CACHE STRING "definitions needed for linking against libufo" FORCE)
	endif ( UFO_EXPORT_INCLUDED )
endif (UFO_FOUND )

MARK_AS_ADVANCED(
	UFO_INCLUDE_DIR 
	UFO_INCLUDE_DIRS 
	UFO_LIBRARY 
	UFO_LIBRARIES 
	UFO_DEFINITIONS
	)

