# - Find oops
# Find the oops library.
#
# This module defines:
#  oops_FOUND - if false, you cannot use oops.
#  oops_DEFINITIONS - compile- and link-definitions for oops
#  oops_INCLUDE_DIR - the directory in which you can find oops/Simulation.h
#  oops_INCLUDE_DIRS - the same as oops_INCLUDE_DIR
#  oops_LIBRARY - the full path of the oops library
#  oops_LIBRARIES - the full path of fhe oops library + names of libraries linked against
#  oops_LIBRARY_DIR - the directory containing the oops library
#  oops_LIBRARY_DIRS - the library directories needed for oops_LIBRARIES
#  oops_EXPORT_INCLUDED - set to true, if config_oops.cmake could be found.
#
# To aid in the detection_process, you can set the following variables:
#  oops_ROOT_DIR - give a hint, which directory to use (=install prefix of oops)
#                 if this is not set, the environment variable oops_ROOT_DIR is used, if available.
#
# Examples:
# set ( oops_ROOT_DIR /usr/local )
###############################################################################
# The following variables are used internally and unset after use:
#  oops_EXPORT_DEFINITIONS
#  oops_EXPORT_INCLUDED
#  oops_EXPORT_INCLUDE_DIRS
#  oops_EXPORT_LIBRARIES
#  oops_EXPORT_LIBRARY_DIRS
# 
###############################################################################

#make cache-entry:
set ( oops_ROOT_DIR ${oops_ROOT_DIR} CACHE PATH "Install prefix of the oops library")

# honor environment variables:
if ( NOT oops_ROOT_DIR )
		set ( oops_ROOT_DIR $ENV{oops_ROOT_DIR} )
endif ( NOT oops_ROOT_DIR )


###############################################################################
# find oops itself:
###############################################################################
# try to find the include directory:
find_path( oops_INCLUDE_DIR NAMES oops/Simulation.h
	HINTS ${oops_ROOT_DIR}/include ${inVRs_INCLUDE_DIR}/inVRs/tools/libraries/
	DOC "The directory in which you can find oops/Simulation.h" )

# try to find the library:
find_library( oops_LIBRARY NAMES oops 
	HINTS ${oops_ROOT_DIR}/lib ${inVRs_LIBRARY_DIR}
	DOC "The directory in which you can find oops.dll/liboops.so")

# set the library directory:
get_filename_component(oops_LIBRARY_DIR ${oops_LIBRARY} PATH )

MARK_AS_ADVANCED(
	oops_INCLUDE_DIR
	oops_LIBRARY
	)

###############################################################################
# set default stuff:
###############################################################################
# handle the QUIETLY and REQUIRED arguments and set oops_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(oops DEFAULT_MSG oops_LIBRARY oops_INCLUDE_DIR)

if ( OOPS_FOUND )
	set ( oops_FOUND true ) # FIND_PACKAGE_HANDLE_STANDARD_ARGS only sets OOPS_FOUND
	set ( oops_INCLUDE_DIRS ${oops_INCLUDE_DIR} )
	set ( oops_LIBRARIES ${oops_LIBRARY} )
endif (OOPS_FOUND )

###############################################################################
# Add compile-flags and definitions:
###############################################################################

include( ${oops_INCLUDE_DIR}/oops/config_oops.cmake OPTIONAL RESULT_VARIABLE oops_EXPORT_INCLUDED )
if ( oops_EXPORT_INCLUDED )
	list ( APPEND oops_INCLUDE_DIRS ${OOPS_EXPORT_INCLUDE_DIRS} )
	list ( APPEND oops_LIBRARY_DIRS ${OOPS_EXPORT_LIBRARY_DIRS} )
	list ( APPEND oops_LIBRARIES ${OOPS_EXPORT_LIBRARIES} )
	set ( oops_DEFINITIONS ${OOPS_EXPORT_DEFINITIONS} )
	# remove internal variables:
	unset ( OOPS_EXPORT_INCLUDE_DIRS )
	unset ( OOPS_EXPORT_LIBRARY_DIRS )
	unset ( OOPS_EXPORT_LIBRARIES )
	unset ( OOPS_EXPORT_DEFINITIONS )
endif ( oops_EXPORT_INCLUDED )

MARK_AS_ADVANCED(
	oops_ROOT_DIR
	oops_FOUND
	oops_DEFINITIONS
	oops_INCLUDE_DIRS
	oops_LIBRARIES
	oops_LIBRARY_DIR
	oops_LIBRARY_DIRS 
	oops_EXPORT_INCLUDED
	)
