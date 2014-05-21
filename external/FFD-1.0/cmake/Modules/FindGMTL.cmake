# - Find GMTL
# Find the GMTL include directory.
#
# This module defines:
#  GMTL_FOUND - if false, you cannot use GMTL.
#  GMTL_INCLUDE_DIR - the directory in which you can find gmtl/gmtl.h
#  GMTL_INCLUDE_DIRS - the same as GMTL_INCLUDE_DIR
#
# To aid in the detection_process, you can set the following variables:
#  GMTL_ROOT_DIR - give a hint, which directory to use (=install prefix of GMTL)
#  GMTL_PROBE_VERSIONS - set versions, which to probe for 
#                        (this is for include/gmtl-VERSION type directories)
# Examples:
# set ( GMTL_ROOT_DIR /usr/local )
# set ( GMTL_PROBE_VERSIONS 0.5.4 0.5.3 0.5.2 )
###############################################################################

#make a cache-entry:
set ( GMTL_ROOT_DIR ${GMTL_ROOT_DIR} CACHE PATH "install prefix of gmtl")

# specifically probe for the following versions:
set ( GMTL_PROBE_VERSIONS 0.6.0 0.5.4 0.4.11 
	CACHE STRING "List of GMTL versions to search for")

set ( GMTL_PROBE_VERSION_DIRS CACHE INTERNAL "" )
foreach (version ${GMTL_PROBE_VERSIONS} )
	list ( APPEND GMTL_PROBE_VERSION_DIRS gmtl-${version} )
endforeach()
#message(STATUS "FindGMTL probes for these version-specific subdirectories: ${GMTL_PROBE_VERSION_DIRS}")

# try to find the include directory:
find_path( GMTL_INCLUDE_DIR NAMES gmtl/gmtl.h
	HINTS ${GMTL_ROOT_DIR} ${GMTL_ROOT_DIR}/include
	PATH_SUFFIXES  ${GMTL_PROBE_VERSION_DIRS} 
	DOC "the directory in which you can find gmtl/gmtl.h")
#message(STATUS "Candidate for GMTL_INCLUDE_DIR: ${GMTL_INCLUDE_DIR}")

# handle the QUIETLY and REQUIRED arguments and set GMTL_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GMTL DEFAULT_MSG GMTL_INCLUDE_DIR)

if ( GMTL_FOUND )
	set ( GMTL_INCLUDE_DIRS ${GMTL_INCLUDE_DIR} CACHE PATH "do not set this, but use GMTL_ROOT_DIR instead" FORCE)
endif (GMTL_FOUND )

MARK_AS_ADVANCED(
	GMTL_PROBE_VERSIONS
	GMTL_INCLUDE_DIR
	GMTL_INCLUDE_DIRS
	)

