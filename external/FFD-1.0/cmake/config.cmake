# Only execute commands if file was not included already
if (NOT FFD_CONFIG_CMAKE_LOADED)

###############################################################################
# Set a default build type for single-configuration CMake generators if no
# build type is set.
###############################################################################
if (NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
	set (CMAKE_BUILD_TYPE Release CACHE STRING "Build type (Release/Debug)" 
		FORCE)
elseif (NOT CMAKE_CONFIGURATION_TYPES)
	set (CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING 
		"Build type (Release/Debug)" FORCE)
endif (NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)

# Set the default install prefix to the FFD directory if not already set in
# user.cmake
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set (CMAKE_INSTALL_PREFIX ${FFD_SOURCE_DIR}/install
		CACHE PATH "FFD install prefix" FORCE)
endif (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

###############################################################################
# Add local module path to CMAKE_MODULE_PATH
###############################################################################
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

###############################################################################
# Find inVRs include path
###############################################################################
#include (FindinVRs)
find_package( inVRs )
message ( STATUS "inVRs_LIBRARY: ${inVRs_LIBRARY}" )
if ( INVRS_FOUND )
	option ( FFD_HAVE_inVRs "Enable support for inVRs framework?" ON )
	if ( FFD_HAVE_inVRs )
		# defines for inVRs:
		set ( FFD_inVRs_DEFINITIONS "-DFFD_HAVE_inVRs" )
		foreach (mod ${inVRs_FOUND_MODULES} )
			set ( FFD_HAVE_inVRs_${mod} true )
			list ( APPEND FFD_inVRs_DEFINITIONS "-DFFD_HAVE_inVRs_${mod}" )
		endforeach()
	endif ( FFD_HAVE_inVRs )
	#message( STATUS "Compiling with ${FFD_inVRs_DEFINITIONS}" )
	add_definitions ( ${FFD_inVRs_DEFINITIONS} )
endif ( INVRS_FOUND )

###############################################################################
# GMTL
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/useGMTL.cmake)

###############################################################################
# OpenSG
find_package ( OpenSG REQUIRED COMPONENTS OSGBase OSGSystem OSGWindowGLUT OSGDrawable OSGUtil OSGGroup)
include_directories ( ${OpenSG_INCLUDE_DIRS} )
list ( APPEND CSMLIB_DEP_LIBRARIES ${OpenSG_LIBRARIES})
add_definitions ( ${OpenSG_DEFINITIONS})


###############################################################################
###############################################################################
###############################################################################

###############################################################################
# set formatting for eclipse output
###############################################################################
if (CMAKE_COMPILER_IS_GNUCC)
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0")
endif (CMAKE_COMPILER_IS_GNUCC)
if (CMAKE_COMPILER_IS_GNUCXX)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
endif (CMAKE_COMPILER_IS_GNUCXX)


set (FFD_CONFIG_CMAKE_LOADED ON)
endif (NOT FFD_CONFIG_CMAKE_LOADED)
