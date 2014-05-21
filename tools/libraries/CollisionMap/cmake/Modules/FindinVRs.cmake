# - Find inVRs
# Find the inVRs library.
#
# This module defines:
#  inVRs_FOUND - if false, you cannot use inVRs.
#  inVRs_DEFINITIONS - compile- and link-definitions for inVRs
#  inVRs_INCLUDE_DIR - the directory in which you can find inVRs/SystemCore/SystemCore.h
#  inVRs_INCLUDE_DIRS - the same as inVRs_INCLUDE_DIR
#  inVRs_LIBRARY - the full path of libinVRsSystemCore
#  inVRs_LIBRARIES - the full path of libinVRsSystemCore + names of libraries linked against
#  inVRs_LIBRARY_DIR - the directory containing libinVRsSystemCore
#  inVRs_LIBRARY_DIRS - the library directories needed for inVRs_LIBRARIES
#  inVRs_FRAMEWORKS - the frameworks needed for inVRs (needed on MAC systems)
#  inVRs_FOUND_MODULES - the set of modules actually found.
#  inVRs_EXPORT_INCLUDED - set to true, if config_inVRs.cmake could be found.
#
#  inVRs_<MODULE>_FOUND - set, if <MODULE> is found
#  inVRs_<MODULE>_INCLUDE_DIRS - include directories needed for <MODULE>
#  inVRs_<MODULE>_LIBRARY - the full path of libinVRs<MODULE>
#  inVRs_<MODULE>_LIBRARIES - the full path of libinVRs<MODULE> + libraries needed to link agains <MODULE>
#  inVRs_<MODULE>_LIBRARY_DIR - the directory containing libinVRsSystemCore
#  inVRs_<MODULE>_LIBRARY_DIRS - the library directories needed for inVRs_LIBRARIES
#
# To aid in the detection_process, you can set the following variables:
#  inVRs_ROOT_DIR - give a hint, which directory to use (=install prefix of inVRs)
#                 if this is not set, the environment variable inVRs_ROOT_DIR is used, if available.
#  inVRs_MODULES - override default inVRs modules
#  inVRs_ADDITIONAL_MODULES - specify additional modules
#
# Examples:
# set ( inVRs_ROOT_DIR /usr/local )
#
# To check for required inVRs modules you can pass COMPONENTS to the Module. Each component will be
# interpreted as a required inVRs-Module. If a passed inVRs-Module is not found an error is thrown.
#  Example: 
#   find_package ( inVRs REQUIRED Navigation Interaction Network )
#
###############################################################################
# The following variables are used internally and unset after use:
#  inVRs_CHECK_MODULES
#  inVRs_EXPORT_DEFINITIONS
#  inVRs_EXPORT_INCLUDED
#  inVRs_EXPORT_INCLUDE_DIRS
#  inVRs_EXPORT_LIBRARIES
#  inVRs_EXPORT_LIBRARY_DIRS
# 
###############################################################################
# CHANGES:
# - 14.10.2009 (rlander): added flags of modules and tools passed as COMPONENTS
#                         to inVRs_XXX variables (e.g. inVRs_LIBRARIES)
# - 13.10.2009 (rlander): fixed order of required Module check
# - 12.10.2009 (rlander): added check for inVRs Modules via COMPONENTS
# - 09.10.2009 (rlander): updated file for CMake 2.4 compatibility
###############################################################################

# check for CMAKE 24 version
if (CMAKE_MAJOR_VERSION EQUAL 2 AND CMAKE_MINOR_VERSION EQUAL 4)
  set (CMAKE_VERSION_24 TRUE)
endif (CMAKE_MAJOR_VERSION EQUAL 2 AND CMAKE_MINOR_VERSION EQUAL 4)

# default modules:
set ( inVRs_CHECK_MODULES 2DPhysics 3DPhysics Interaction JointInteraction Navigation Network )

#make cache-entry:
set ( inVRs_ROOT_DIR ${inVRs_ROOT_DIR} CACHE PATH "Install prefix of the inVRs library")
set ( inVRs_MODULES ${inVRs_MODULES} CACHE STRING "Override default inVRs modules (Default: ${inVRs_CHECK_MODULES})")
set ( inVRs_ADDITIONAL_MODULES ${inVRs_ADDITIONAL_MODULES} CACHE STRING "Additional inVRs modules to search for.")

# honor environment variables:
if ( NOT inVRs_ROOT_DIR )
	set ( inVRs_ROOT_DIR $ENV{inVRs_ROOT_DIR} )
endif ( NOT inVRs_ROOT_DIR )

# inVRs_CHECK_MODULES holds the module-names which are actually searched for
# -> := inVRs_MODULES + inVRs_ADDITIONAL_MODULES
if ( inVRs_MODULES ) #override default ?
	set ( inVRs_CHECK_MODULES ${inVRs_MODULES} )
endif ( inVRs_MODULES )
if ( inVRs_ADDITIONAL_MODULES )
	list ( APPEND inVRs_CHECK_MODULES ${inVRs_ADDITIONAL_MODULES} )
endif ( inVRs_ADDITIONAL_MODULES )
if ( inVRs_FIND_COMPONENTS )
	list ( APPEND inVRs_CHECK_MODULES ${inVRs_FIND_COMPONENTS} )
	if ( NOT CMAKE_VERSION_24 )
		list ( REMOVE_DUPLICATES inVRs_CHECK_MODULES )
	endif ( NOT CMAKE_VERSION_24 )
endif ( inVRs_FIND_COMPONENTS )

###############################################################################
# find inVRs itself:
###############################################################################
# try to find the include directory:
# CMAKE 2.4.x compatibility
if ( CMAKE_VERSION_24 )
	find_path( inVRs_INCLUDE_DIR NAMES inVRs/SystemCore/SystemCore.h
		PATHS ${inVRs_ROOT_DIR}/include 
		DOC "The directory in which you can find inVRs/SystemCore/SystemCore.h" )
else ( CMAKE_VERSION_24 )
	find_path( inVRs_INCLUDE_DIR NAMES inVRs/SystemCore/SystemCore.h
		HINTS ${inVRs_ROOT_DIR}/include 
		DOC "The directory in which you can find inVRs/SystemCore/SystemCore.h" )
endif ( CMAKE_VERSION_24 )

# try to find the library:
# CMAKE 2.4.x compatibility
if ( CMAKE_VERSION_24 )
	find_library( inVRs_LIBRARY NAMES inVRsSystemCore 
		PATHS ${inVRs_ROOT_DIR}/lib 
		DOC "The directory in which you can find libinVRsSystemCore.so")
else ( CMAKE_VERSION_24 )
	find_library( inVRs_LIBRARY NAMES inVRsSystemCore 
		HINTS ${inVRs_ROOT_DIR}/lib 
		DOC "The directory in which you can find libinVRsSystemCore.so")
endif ( CMAKE_VERSION_24 )

# set the library directory:
get_filename_component(inVRs_LIBRARY_DIR ${inVRs_LIBRARY} PATH )

MARK_AS_ADVANCED(
	inVRs_ADDITIONAL_MODULES
	inVRs_MODULES 
	inVRs_INCLUDE_DIR
	inVRs_LIBRARY
	)

###############################################################################
# find Modules:
###############################################################################
set ( inVRs_FOUND_MODULES "" CACHE INTERNAL "The modules, which have been actually found by the FindinVRs script.")
foreach (module ${inVRs_CHECK_MODULES} )
	find_library( inVRs_${module}_LIBRARY NAMES inVRs${module}
		PATHS ${inVRs_LIBRARY_DIR} 
		NO_DEFAULT_PATH NO_CMAKE_ENVIRONMENT_PATH NO_CMAKE_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH
		DOC "Full name of the library inVRs_${module}"
		)
	MARK_AS_ADVANCED (inVRs_${module}_LIBRARY ) 
	if ( inVRs_${module}_LIBRARY )
		# set the library directory:
		get_filename_component ( inVRs_${module}_LIBRARY_DIR ${inVRs_${module}_LIBRARY} PATH )
		# add module to list:
		list ( APPEND inVRs_FOUND_MODULES ${module} )
		set ( inVRs_${module}_FOUND true )
		MARK_AS_ADVANCED (
			inVRs_${module}_FOUND
			inVRs_${module}_LIBRARY_DIR
			inVRs_${module}_LIBRARY_DIRS
			inVRs_${module}_INCLUDE_DIR
			inVRs_${module}_INCLUDE_DIRS
			)
	endif ( inVRs_${module}_LIBRARY )
endforeach(module ${inVRs_CHECK_MODULES} )

###############################################################################
# set default stuff:
###############################################################################
# handle the QUIETLY and REQUIRED arguments and set inVRs_FOUND to TRUE if
# all listed variables are TRUE
# CMAKE 2.4.x support
if ( NOT CMAKE_VERSION_24 )
	include( FindPackageHandleStandardArgs OPTIONAL RESULT_VARIABLE FindPackageHandleStandardsArgs_INCLUDED )
endif ( NOT CMAKE_VERSION_24 )

if ( FindPackageHandleStandardsArgs_INCLUDED )
	FIND_PACKAGE_HANDLE_STANDARD_ARGS(inVRs DEFAULT_MSG inVRs_LIBRARY inVRs_INCLUDE_DIR)
else ( FindPackageHandleStandardsArgs_INCLUDED )
	set ( INVRS_FOUND TRUE )
	if ( NOT inVRs_LIBRARY )
		message( FATAL_ERROR "Unable to find inVRs_LIBRARY! Please set the correct inVRs_ROOT_DIR to the place of your inVRs installation!")
		set ( INVRS_FOUND FALSE )
	endif ( NOT inVRs_LIBRARY )
	if ( NOT inVRs_INCLUDE_DIR )
		message( FATAL_ERROR "Unable to find inVRs_INCLUDE_DIR! Please set the correct inVRs_ROOT_DIR to the place of your inVRs installation!")
		set ( INVRS_FOUND FALSE )
	endif ( NOT inVRs_INCLUDE_DIR )
endif ( FindPackageHandleStandardsArgs_INCLUDED )

###############################################################################
# check for required modules:
###############################################################################
foreach (module ${inVRs_FIND_COMPONENTS} )
	if (NOT inVRs_${module}_FOUND)
		message (FATAL_ERROR "ERROR: Required inVRs Module or Tool \"${module}\" not found! Please check your inVRs installation!")
	endif (NOT inVRs_${module}_FOUND)
endforeach (module ${inVRs_FIND_COMPONENTS} )

###############################################################################
# set remaining variables:
###############################################################################
if ( INVRS_FOUND )
	set ( inVRs_FOUND true ) # FIND_PACKAGE_HANDLE_STANDARD_ARGS only sets INVRS_FOUND
	set ( inVRs_INCLUDE_DIRS ${inVRs_INCLUDE_DIR} )
	set ( inVRs_LIBRARIES ${inVRs_LIBRARY} )
endif (INVRS_FOUND )

###############################################################################
# Add compile-flags and definitions:
###############################################################################

# CMAKE 2.4.x support
if ( CMAKE_VERSION_24 )
	include( ${inVRs_INCLUDE_DIR}/inVRs/config_inVRs.cmake OPTIONAL )
	if ( INVRS_EXPORT_INCLUDE_DIRS OR INVRS_EXPORT_LIBRARY_DIRS )
		set ( inVRs_EXPORT_INCLUDED TRUE )
	endif ( INVRS_EXPORT_INCLUDE_DIRS OR INVRS_EXPORT_LIBRARY_DIRS )
else ( CMAKE_VERSION_24 )
	include( ${inVRs_INCLUDE_DIR}/inVRs/config_inVRs.cmake OPTIONAL RESULT_VARIABLE inVRs_EXPORT_INCLUDED )
endif ( CMAKE_VERSION_24 )

if ( inVRs_EXPORT_INCLUDED )
	if ( INVRS_EXPORT_INCLUDE_DIRS )
		list ( APPEND inVRs_INCLUDE_DIRS ${INVRS_EXPORT_INCLUDE_DIRS} )
	endif ( INVRS_EXPORT_INCLUDE_DIRS )
	if ( INVRS_EXPORT_LIBRARY_DIRS )
		list ( APPEND inVRs_LIBRARY_DIRS ${INVRS_EXPORT_LIBRARY_DIRS} )
	endif ( INVRS_EXPORT_LIBRARY_DIRS )
	if ( INVRS_EXPORT_LIBRARIES )
		list ( APPEND inVRs_LIBRARIES ${INVRS_EXPORT_LIBRARIES} )
	endif ( INVRS_EXPORT_LIBRARIES )
	set ( inVRs_DEFINITIONS ${INVRS_EXPORT_DEFINITIONS} )
	set ( inVRs_FRAMEWORKS ${INVRS_EXPORT_FRAMEWORKS} )
	# remove internal variables:
#RLANDER: removed unsets for compatibility with different cmake versions
#	unset ( inVRs_EXPORT_INCLUDE_DIRS )
#	unset ( inVRs_EXPORT_LIBRARY_DIRS )
#	unset ( inVRs_EXPORT_LIBRARIES )
#	unset ( inVRs_EXPORT_DEFINITIONS )
	foreach ( mod ${inVRs_FOUND_MODULES} )
		if ( INVRS_EXPORT_${mod}_INCLUDE_DIRS )
			list ( APPEND inVRs_${mod}_INCLUDE_DIRS ${INVRS_EXPORT_${mod}_INCLUDE_DIRS} )
		endif ( INVRS_EXPORT_${mod}_INCLUDE_DIRS )
		if ( INVRS_EXPORT_${mod}_LIBRARY_DIRS )
			list ( APPEND inVRs_${mod}_LIBRARY_DIRS ${INVRS_EXPORT_${mod}_LIBRARY_DIRS} )
		endif ( INVRS_EXPORT_${mod}_LIBRARY_DIRS )
		if ( INVRS_EXPORT_${mod}_LIBRARIES )
			list ( APPEND inVRs_${mod}_LIBRARIES ${INVRS_EXPORT_${mod}_LIBRARIES} )
		endif ( INVRS_EXPORT_${mod}_LIBRARIES )
		set ( inVRs_${mod}_DEFINITIONS ${INVRS_EXPORT_${mod}_DEFINITIONS} )
		# remove internal variables:
#		unset ( inVRs_${mod}_EXPORT_INCLUDE_DIRS )
#		unset ( inVRs_${mod}_EXPORT_LIBRARY_DIRS )
#		unset ( inVRs_${mod}_EXPORT_LIBRARIES )
#		unset ( inVRs_${mod}_EXPORT_DEFINITIONS )
	endforeach ( mod ${inVRs_FOUND_MODULES} )

	# add flags from required modules (and tools) to inVRs_... variables
	foreach ( comp ${inVRs_FIND_COMPONENTS} )
		if ( inVRs_${comp}_INCLUDE_DIRS )
			list (APPEND inVRs_INCLUDE_DIRS ${inVRs_${comp}_INCLUDE_DIRS})
		endif ( inVRs_${comp}_INCLUDE_DIRS )
		if ( inVRs_${comp}_LIBRARY_DIRS )
			list (APPEND inVRs_LIBRARY_DIRS ${inVRs_${comp}_LIBRARY_DIRS})
		endif ( inVRs_${comp}_LIBRARY_DIRS )
		if ( inVRs_${comp}_LIBRARIES )
			list (APPEND inVRs_LIBRARIES ${inVRs_${comp}_LIBRARIES})
		endif ( inVRs_${comp}_LIBRARIES )
		if ( inVRs_${comp}_DEFINITIONS )
			list (APPEND inVRs_DEFINITIONS ${inVRs_${comp}_DEFINITIONS})
		endif ( inVRs_${comp}_DEFINITIONS)
		if ( inVRs_${comp}_FRAMEWORKS )
			list (APPEND inVRs_FRAMEWORKS ${inVRs_${comp}_FRAMEWORKS})
		endif ( inVRs_${comp}_FRAMEWORKS )
	endforeach ( comp ${inVRs_FIND_COMPONENTS} )

	# remove double entries
	if ( inVRs_INCLUDE_DIRS AND NOT CMAKE_VERSION_24 )
		list ( REMOVE_DUPLICATES inVRs_INCLUDE_DIRS )
	endif ( inVRs_INCLUDE_DIRS AND NOT CMAKE_VERSION_24 )
	if ( inVRs_LIBRARY_DIRS AND NOT CMAKE_VERSION_24 )
		list ( REMOVE_DUPLICATES inVRs_LIBRARY_DIRS )
	endif ( inVRs_LIBRARY_DIRS AND NOT CMAKE_VERSION_24 )
	if ( inVRs_LIBRARIES AND NOT CMAKE_VERSION_24 )
		list ( REMOVE_DUPLICATES inVRs_LIBRARIES )
	endif ( inVRs_LIBRARIES AND NOT CMAKE_VERSION_24 )
	if ( inVRs_DEFINITIONS AND NOT CMAKE_VERSION_24 )
		list ( REMOVE_DUPLICATES inVRs_DEFINITIONS )
	endif ( inVRs_DEFINITIONS AND NOT CMAKE_VERSION_24 )
	if ( inVRs_FRAMEWORKS AND NOT CMAKE_VERSION_24 )
		list ( REMOVE_DUPLICATES inVRs_FRAMEWORKS )
	endif ( inVRs_FRAMEWORKS AND NOT CMAKE_VERSION_24 )
endif ( inVRs_EXPORT_INCLUDED )

MARK_AS_ADVANCED(
	inVRs_FOUND
	inVRs_FOUND_MODULES
	inVRs_DEFINITIONS
	inVRs_INCLUDE_DIRS
	inVRs_LIBRARIES
	inVRs_LIBRARY_DIR
	inVRs_LIBRARY_DIRS 
	inVRs_EXPORT_INCLUDED
	)

# unset internal variables:
#unset(inVRs_CHECK_MODULES)

