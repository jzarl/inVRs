# - Find avatara
# Find the avatara library.
#
# This module defines:
#  avatara_FOUND - if false, you cannot use avatara.
#  avatara_DEFINITIONS - compile- and link-definitions for avatara
#  avatara_INCLUDE_DIR - the directory in which you can find avatara/SystemCore/SystemCore.h
#  avatara_INCLUDE_DIRS - the same as avatara_INCLUDE_DIR
#  avatara_LIBRARY - the full path of libavatara
#  avatara_LIBRARIES - the full path of libavatara + names of libraries linked against
#  avatara_LIBRARY_DIR - the directory containing libavatara
#  avatara_LIBRARY_DIRS - the library directories needed for avatara_LIBRARIES
#  avatara_FRAMEWORKS - the frameworks needed for avatara (needed on MAC systems)
#
# To aid in the detection_process, you can set the following variables:
#  avatara_ROOT_DIR - give a hint, which directory to use (=install prefix of avatara)
#		 if this is not set, the environment variable avatara_ROOT_DIR is used, if available.
#
# Examples:
# set ( avatara_ROOT_DIR /usr/local )
#
###############################################################################
# The following variables are used internally and unset after use:
###############################################################################
# CHANGES:
# - 13.10.2009 (rlander): created
# - 26.03.2010 (rlander): updated for new cmake structure
###############################################################################


###############################################################################
# set default paths for avatara_ROOT_DIR
###############################################################################
set (avatara_DEFAULT_PATHS ${inVRs_ROOT_DIR} $ENV{avatara_ROOT_DIR} CACHE 
	INTERNAL "Default paths for avatara_ROOT_DIR")


###############################################################################
# Place avatara_ROOT_DIR in CACHE (in case it is defined in user.cmake)
###############################################################################
if ( DEFINED avatara_ROOT_DIR )
	mark_as_advanced (CLEAR avatara_ROOT_DIR)
	set ( avatara_ROOT_DIR ${avatara_ROOT_DIR} CACHE PATH 
		"Install prefix of the avatara library" FORCE )
endif ( DEFINED avatara_ROOT_DIR )


###############################################################################
# try to find the root directory (if not set):
###############################################################################
if ( NOT avatara_ROOT_DIR )
	find_path( avatara_ROOT_DIR NAMES include/avatara/Avatar.h
		HINTS ${avatara_DEFAULT_PATHS} )
endif ( NOT avatara_ROOT_DIR )

###############################################################################
# try to find the include directory:
###############################################################################
find_path( avatara_INCLUDE_DIR NAMES avatara/Avatar.h
	HINTS ${avatara_ROOT_DIR}/include 
	DOC "The directory in which you can find avatara/Avatar.h" )

###############################################################################
# try to find the library:
###############################################################################
find_library( avatara_LIBRARY NAMES avatara
	HINTS ${avatara_ROOT_DIR}/lib 
	DOC "The directory in which you can find libavatara.so")

###############################################################################
# obtain library directory from library
###############################################################################
if ( avatara_LIBRARY )
	get_filename_component(avatara_LIBRARY_DIR ${avatara_LIBRARY} PATH )
endif ( avatara_LIBRARY )


###############################################################################
# advance variables which should not be visible to user
###############################################################################
MARK_AS_ADVANCED(
	avatara_INCLUDE_DIR
	avatara_LIBRARY
	avatara_LIBRARY_DIR
	)

###############################################################################
# set default stuff:
###############################################################################
include( FindPackageHandleStandardArgs )

FIND_PACKAGE_HANDLE_STANDARD_ARGS(avatara 
	"Unable to find avatara_LIBRARY or avatara_INCLUDE_DIR! Please set the correct avatara_ROOT_DIR to the prefix of your avatara installation!"
	avatara_LIBRARY avatara_INCLUDE_DIR)
set (avatara_FOUND ${AVATARA_FOUND})

if ( avatara_FOUND )
	set ( avatara_INCLUDE_DIRS ${avatara_INCLUDE_DIR} )
	set ( avatara_LIBRARIES ${avatara_LIBRARY} )
	set ( avatara_LIBRARY_DIRS ${avatara_LIBRARY_DIR} )
	MARK_AS_ADVANCED ( FORCE avatara_ROOT_DIR )
endif ( avatara_FOUND )

###############################################################################
# Add compile-flags and definitions:
###############################################################################

include( ${avatara_INCLUDE_DIR}/avatara/config_avatara.cmake OPTIONAL 
	RESULT_VARIABLE avatara_EXPORT_INCLUDED )

if ( avatara_EXPORT_INCLUDED )
	if ( AVATARA_EXPORT_INCLUDE_DIRS )
		list ( APPEND avatara_INCLUDE_DIRS ${AVATARA_EXPORT_INCLUDE_DIRS} )
	endif ( AVATARA_EXPORT_INCLUDE_DIRS )
	if ( AVATARA_EXPORT_LIBRARY_DIRS )
		list ( APPEND avatara_LIBRARY_DIRS ${AVATARA_EXPORT_LIBRARY_DIRS} )
	endif ( AVATARA_EXPORT_LIBRARY_DIRS )
	if ( AVATARA_EXPORT_LIBRARIES )
		list ( APPEND avatara_LIBRARIES ${AVATARA_EXPORT_LIBRARIES} )
	endif ( AVATARA_EXPORT_LIBRARIES )
	set ( avatara_DEFINITIONS ${AVATARA_EXPORT_DEFINITIONS} )
	set ( avatara_FRAMEWORKS ${AVATARA_EXPORT_FRAMEWORKS} )
	# remove internal variables:
endif ( avatara_EXPORT_INCLUDED )

MARK_AS_ADVANCED(
	avatara_FOUND
	avatara_DEFINITIONS
	avatara_INCLUDE_DIR
	avatara_INCLUDE_DIRS
	avatara_LIBRARY
	avatara_LIBRARIES
	avatara_LIBRARY_DIR
	avatara_LIBRARY_DIRS 
	avatara_FRAMEWORKS
	avatara_EXPORT_INCLUDED
	)

