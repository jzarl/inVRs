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
			set ( GMTL_ROOT_DIR ${FFD_SOURCE_DIR}/external )
		endif ( INVRS_FOUND )
		message ( STATUS "GMTL_ROOT_DIR set to ${GMTL_ROOT_DIR}" )
	endif ( "$ENV{GMTL_ROOT_DIR}" STREQUAL "" )
endif ( NOT GMTL_ROOT_DIR )

find_package(GMTL REQUIRED)
if ( NOT GMTL_FOUND )
	message( FATAL_ERROR "GMTL is needed, but could not be found! See README.gmtl for instructions how to get GMTL.")
endif ( NOT GMTL_FOUND )