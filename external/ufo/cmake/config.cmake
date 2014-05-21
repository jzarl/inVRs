# Only execute commands if file was not included already
if (NOT UFO_CONFIG_CMAKE_LOADED)

	# Set a default build type if no build type is set
	if (NOT CMAKE_BUILD_TYPE)
		set (CMAKE_BUILD_TYPE Release)
	endif (NOT CMAKE_BUILD_TYPE)

	# Set the default install prefix to the UFO directory if not already set in
	# user.cmake
	if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
		set (CMAKE_INSTALL_PREFIX ${UFO_SOURCE_DIR}/install
			CACHE PATH "UFO install prefix" FORCE)
	endif (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

	# Allow own module-definitions to be found:
	list ( APPEND CMAKE_MODULE_PATH  "${UFO_SOURCE_DIR}/cmake_common/" )
	message ( STATUS "CMAKE_MODULE_PATH is ${CMAKE_MODULE_PATH}" )

	###############################################################################
	# Find inVRs include path
	###############################################################################
	#include (FindinVRs)
	find_package( inVRs )
	message ( STATUS "inVRs_LIBRARY: ${inVRs_LIBRARY}" )
	if ( INVRS_FOUND )
		option ( UFO_HAVE_inVRs "Enable support for inVRs framework?" ON )
		if ( UFO_HAVE_inVRs )
			# defines for inVRs:
			set ( UFO_inVRs_DEFINITIONS "-DUFO_HAVE_inVRs" )
			foreach (mod ${inVRs_FOUND_MODULES} )
				set ( UFO_HAVE_inVRs_${mod} true )
				list ( APPEND UFO_inVRs_DEFINITIONS "-DUFO_HAVE_inVRs_${mod}" )
			endforeach()
		endif ( UFO_HAVE_inVRs )
		#message( STATUS "Compiling with ${UFO_inVRs_DEFINITIONS}" )
		add_definitions ( ${UFO_inVRs_DEFINITIONS} )
	endif ( INVRS_FOUND )
	
	###############################################################################
	# Find GMTL include path
	###############################################################################
	# use default directory, if not set in user.cmake:
	if ( NOT GMTL_ROOT_DIR )
		# don't override environment variable:
		if ( "$ENV{GMTL_ROOT_DIR}" STREQUAL "" )
			if ( INVRS_FOUND )
				# if we have found inVRs, try its GMTL directory:
				# TODO: once inVRs installs gmtl, use the inVRs_INCLUDE_DIR
				set ( GMTL_ROOT_DIR ${inVRs_ROOT_DIR}/external )
			else ( INVRS_FOUND )
				# by default, search in our "external" directory:
				set ( GMTL_ROOT_DIR ${UFO_SOURCE_DIR}/external )
			endif ( INVRS_FOUND )
			message ( STATUS "GMTL_ROOT_DIR set to ${GMTL_ROOT_DIR}" )
		endif ( "$ENV{GMTL_ROOT_DIR}" STREQUAL "" )
	endif ( NOT GMTL_ROOT_DIR )
	include (FindGMTL)
	if ( NOT GMTL_FOUND )
		message( FATAL_ERROR "GMTL is needed, but could not be found! See README.gmtl for instructions how to get GMTL.")
	endif ( NOT GMTL_FOUND )


	set (UFO_CONFIG_CMAKE_LOADED ON)

endif (NOT UFO_CONFIG_CMAKE_LOADED)

