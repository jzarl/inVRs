# - Find OpenSG
# Find the OpenSG library.
#
# This module defines:
# The following variables are set according to the required modules:
#  OpenSG_FOUND - if false, you cannot use OpenSG.
#  OpenSG_INCLUDE_DIRS - the same as OpenSG_INCLUDE_DIR
#  OpenSG_LIBRARIES - the full path of libOpenSGSystemCore
#  OpenSG_LIBRARY_DIRS - the directory containing libOpenSGSystemCore
#  OpenSG_DEFINITIONS - compile-time definitions 
#  OpenSG_LINK_DEFINITIONS - link-time definitions
#
# for each additional module, the following variables are set:
#  OpenSG_<MODULE>_FOUND - set, if <MODULE> is found
#  OpenSG_<MODULE>_INCLUDE_DIRS - include directories needed for <MODULE>
#  OpenSG_<MODULE>_LIBRARIES - libraries needed to link agains <MODULE>
#  OpenSG_<MODULE>_DEFINITIONS - compile-time definitions 
#  OpenSG_<MODULE>_LINK_DEFINITIONS - link-time definitions
#
# To aid in the detection_process, you can set the following variables:
#  OpenSG_REQUIRED_MODULES - required OpenSG modules (default: OSGBase OSGSystem )
#  OpenSG_ADDITIONAL_MODULES - specify additional modules (e.g. OSGGLUT OSGQT4 OSGQT OSGWindowX )
#
###############################################################################

#make cache-entry:
set ( OpenSG_REQUIRED_MODULES ${OpenSG_MODULES} CACHE STRING "Override default required OpenSG modules")
set ( OpenSG_ADDITIONAL_MODULES ${OpenSG_ADDITIONAL_MODULES} CACHE STRING "Additional OpenSG modules to use.")

if ( NOT OpenSG_REQUIRED_MODULES ) #override default ?
	#default value:
	set ( OpenSG_REQUIRED_MODULES OSGBase OSGSystem )
endif ( NOT OpenSG_REQUIRED_MODULES )

###############################################################################
# Invoke pkg-config:
###############################################################################
# pkg-config variables with OpenSG
# OpenSG_LIBRARIES          : libraries (as for -l)
# OpenSG_LIBRARY_DIRS       : library dirs (as for -L)
# OpenSG_LDFLAGS            : linker-flags with -l and -L statements
# OpenSG_LDFLAGS_OTHER      : linker-flags without -l and -L stuff
# OpenSG_INCLUDE_DIRS       : list of include dirs
# OpenSG_CFLAGS             : CFLAGS with -I stuff
# OpenSG_CFLAGS_OTHER       : CFLAGS without -I's
include(FindPkgConfig)

pkg_check_modules( OpenSG ${OpenSG_REQUIRED_MODULES} )

if ( PKG_CONFIG_FOUND )
	# debug-output for pkg-config:
	#message( STATUS "PKG_CONFIG_FOUND is ${PKG_CONFIG_FOUND}")
	#message( STATUS "PKG_CONFIG_EXECUTABLE is ${PKG_CONFIG_EXECUTABLE}")
	#message( STATUS "OpenSG_FOUND is ${OpenSG_FOUND}")
	#foreach (var LIBRARIES LIBRARY_DIRS LDFLAGS LDFLAGS_OTHER INCLUDE_DIRS CFLAGS CFLAGS_OTHER)
	#	message (STATUS "OpenSG_${var} is ${OpenSG_${var}}")
	#endforeach()
	#foreach (mod ${OpenSG_REQUIRED_MODULES})
	#	foreach (var VERSION PREFIX INCLUDEDIR LIBDIR)
	#		message (STATUS "OpenSG_${mod}_${var} is ${OpenSG_${mod}_${var}}")
	#	endforeach()
	#endforeach()

	# set "our" variables:
	#keep OpenSG_FOUND
	#keep OpenSG_LIBRARIES
	set ( OpenSG_DEFINITIONS ${OpenSG_CFLAGS_OTHER} )
	set ( OpenSG_LINK_DEFINITIONS ${OpenSG_LDFLAGS_OTHER} )
	#keep OpenSG_INCLUDE_DIRS
	#keep  OpenSG_LIBRARY_DIRS
	# unset pkg-config variables: 
	foreach (var OpenSG_LDFLAGS OpenSG_LDFLAGS_OTHER OpenSG_CFLAGS OpenSG_CFLAGS_OTHER )
		unset (${var})
	endforeach()
	foreach (mod ${OpenSG_REQUIRED_MODULES} )
		# (over)write OpenSG_VERSION:
		set ( OpenSG_VERSION ${OpenSG_${mod}_VERSION} )
		list (APPEND OpenSG_LIBRARY_DIR ${OpenSG_${mod}_LIBDIR} )
		list (APPEND OpenSG_INCLUDE_DIR ${OpenSG_${mod}_INCLUDEDIR} )
		foreach (var OpenSG_${mod}_VERSION OpenSG_${mod}_PREFIX OpenSG_${mod}_INCLUDEDIR OpenSG_${mod}_LIBDIR )
			unset (${var})
		endforeach()
	endforeach()
	list (REMOVE_DUPLICATES OpenSG_LIBRARY_DIR)
	list (REMOVE_DUPLICATES OpenSG_INCLUDE_DIR)
	list (APPEND OpenSG_INCLUDE_DIRS ${OpenSG_INCLUDE_DIR}) # for some reason, this is not correctly set
	list (REMOVE_DUPLICATES OpenSG_INCLUDE_DIRS)

	foreach( mod ${OpenSG_ADDITIONAL_MODULES} )
		pkg_check_modules( OpenSG_${mod} ${mod} )
		if ( OpenSG_${mod}_FOUND )
			# set "our" variables:
			#keep OpenSG_${mod}_FOUND
			#keep OpenSG_${mod}_LIBRARIES
			set ( OpenSG_${mod}_DEFINITIONS ${OpenSG_${mod}_CFLAGS_OTHER} )
			set ( OpenSG_${mod}_LINK_DEFINITIONS ${OpenSG_${mod}_LDFLAGS_OTHER} )
			#keep OpenSG_${mod}_INCLUDE_DIRS
			set ( OpenSG_${mod}_INCLUDE_DIR ${OpenSG${mod}__INCLUDEDIR} )
			#keep  OpenSG_${mod}_LIBRARY_DIRS
			set ( OpenSG_${mod}_LIBRARY_DIR ${OpenSG${mod}__LIBDIR} )
			# unset pkg-config variables: 
			foreach (var OpenSG_${mod}_LDFLAGS OpenSG_${mod}_LDFLAGS_OTHER OpenSG_${mod}_CFLAGS OpenSG_${mod}_CFLAGS_OTHER 
				OpenSG_${mod}_VERSION OpenSG_${mod}_PREFIX OpenSG_${mod}_INCLUDEDIR OpenSG_${mod}_LIBDIR )
				unset (${var})
			endforeach()
		endif ( OpenSG_${mod}_FOUND )
	endforeach()
else ( PKG_CONFIG_FOUND )
	if ( NOT OpenSG_FIND_QUIETLY )
		message ( STATUS "pkg-config not found. Falling back to osg-config...")
	endif ( NOT OpenSG_FIND_QUIETLY )
endif ( PKG_CONFIG_FOUND )

###############################################################################
# Invoke osg-config:
###############################################################################
if ( NOT OpenSG_FOUND )
		message ( STATUS "osg-config not yet implemented..." )
endif ( NOT OpenSG_FOUND )

###############################################################################
# set default stuff:
###############################################################################
# handle the QUIETLY and REQUIRED arguments and set OpenSG_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OpenSG DEFAULT_MSG OpenSG_VERSION OpenSG_LIBRARIES OpenSG_LIBRARY_DIRS OpenSG_INCLUDE_DIRS )

MARK_AS_ADVANCED(
	OpenSG_INCLUDE_DIRS 
	OpenSG_LIBRARY_DIRS
	OpenSG_LIBRARIES 
	OpenSG_LIBRARY_DIRS 
	OpenSG_REQUIRED_MODULES 
	OpenSG_ADDITIONAL_MODULES 
	)

