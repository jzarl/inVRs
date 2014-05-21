# module CMakePackageConfigHelpers requires cmake 2.8.8:
cmake_minimum_required ( VERSION 2.8.8 )

######################################################################
# The name of our project is "CAVESCENEMANAGER". CMakeLists files in
# this project refer to the root source directory of the project as
# ${CAVESCENEMANAGER_SOURCE_DIR} and to the root binary directory of
# the project as ${CAVESCENEMANAGER_BINARY_DIR}.
######################################################################
project ( CAVESCENEMANAGER)

##############################################################################
# Check if CAVESceneManager is built within inVRs or by itself
# If it is built within inVRs the libraries and headers are installed in
# different locations than when built independently.
##############################################################################
if ( NOT INVRS_SOURCE_DIR)

	#######################################################################
	# Define target paths for installation
	#######################################################################
	set ( TARGET_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)
	set ( TARGET_LIB_DIR ${CMAKE_INSTALL_PREFIX}/lib)
	set ( TARGET_BIN_DIR ${CMAKE_INSTALL_PREFIX}/bin)

	#######################################################################
	# Include user-defined configuration
	#######################################################################
	include ( ${CMAKE_CURRENT_SOURCE_DIR}/user.cmake OPTIONAL)

else ( NOT INVRS_SOURCE_DIR)

	#######################################################################
	# If CAVESceneManager is built within inVRs then install the libraries
	# and header files in the inVRs target directories
	#######################################################################
	set ( TARGET_INCLUDE_DIR ${INVRS_TARGET_INCLUDE_DIR})
	set ( TARGET_LIB_DIR ${INVRS_TARGET_LIB_DIR})
	set ( TARGET_BIN_DIR ${INVRS_TARGET_BIN_DIR})

endif ( NOT INVRS_SOURCE_DIR)

# At least increase the patch version when doing some non-fully-compatible stuff
# That also includes adding new targets or public header files!
# (Think if someone might want to say "I want at least _this_ commit")
set(CAVESCENEMANAGER_VERSION_MAJOR 1)
set(CAVESCENEMANAGER_VERSION_MINOR 0)
set(CAVESCENEMANAGER_VERSION_PATCH 0)
set(CAVESCENEMANAGER_VERSION ${CAVESCENEMANAGER_VERSION_MAJOR}.${CAVESCENEMANAGER_VERSION_MINOR}.${CAVESCENEMANAGER_VERSION_PATCH})

set ( TARGET_DEVELOPMENT_COMPONENT "CAVESceneManager-dev" )
set ( TARGET_RUNTIME_COMPONENT "CAVESceneManager-runtime" )

# FIXME: this is not really ideal, I guess:
if(WIN32 AND NOT CYGWIN)
	set(CMAKECONFIG_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/cmake)
else()
	set(CMAKECONFIG_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib/cmake/CAVESceneManager)
endif()

###############################################################################
# Include config.cmake
###############################################################################
include ( ${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.cmake)

##############################################################################
# Build-Instructions for the CAVESceneManager library
##############################################################################

# Set definition for exporting DLL information
add_definitions ( -DCAVESCENEMANAGER_COMPILELIB)

# Store all library source-files in the CAVESCENEMANAGER_SRCS variable
aux_source_directory ( src CAVESCENEMANAGER_SRCS)

# dependencies for libCAVESceneManager:
find_package ( OpenSG REQUIRED COMPONENTS OSGDrawable OSGCluster OSGUtil )
include_directories ( ${OpenSG_INCLUDE_DIRS} )
list ( APPEND CSMLIB_DEP_LIBRARIES ${OpenSG_LIBRARIES})
add_definitions ( ${OpenSG_DEFINITIONS})

# Build library for CAVESceneManager
set ( LIBRARY_NAME CAVESceneManager)
add_library ( ${LIBRARY_NAME} SHARED ${CAVESCENEMANAGER_SRCS})
target_link_libraries ( ${LIBRARY_NAME} ${CSMLIB_DEP_LIBRARIES})
# set version of the shared library
set_target_properties( ${LIBRARY_NAME} PROPERTIES
	VERSION ${CAVESCENEMANAGER_VERSION}
	SOVERSION ${CAVESCENEMANAGER_VERSION_MAJOR} )

# Append 'D' to debug lib name in Windows
set ( _OSG_DEBUG_POSTFIX "")
if ( WIN32)
    set ( _OSG_DEBUG_POSTFIX DEBUG_POSTFIX "D")
endif ( WIN32)

set_target_properties ( ${LIBRARY_NAME} PROPERTIES
        ${_OSG_DEBUG_POSTFIX}
        DEBUGOPT_POSTFIX "DO"
        RELEASENOOPT_POSTFIX "RN"
)

# Install headers, libraries, and binaries in the target directories
install ( DIRECTORY include/OSGCSM
	DESTINATION ${TARGET_INCLUDE_DIR}
	COMPONENT ${TARGET_DEVELOPMENT_COMPONENT}
	PATTERN ".svn" EXCLUDE)
install ( TARGETS ${LIBRARY_NAME}
	EXPORT CAVESceneManagerExport
	ARCHIVE DESTINATION ${TARGET_LIB_DIR}
	LIBRARY DESTINATION ${TARGET_LIB_DIR}
	RUNTIME DESTINATION ${TARGET_BIN_DIR}
	COMPONENT ${TARGET_RUNTIME_COMPONENT})
install ( TARGETS ${RENDERSERVER_APPNAME}
	DESTINATION ${TARGET_BIN_DIR}
	COMPONENT ${TARGET_RUNTIME_COMPONENT})
install ( EXPORT CAVESceneManagerExport
	DESTINATION ${CMAKECONFIG_INSTALL_DIR}
	FILE CAVESceneManagerTargets.cmake)

include(CMakePackageConfigHelpers)
configure_package_config_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/CAVESceneManagerConfig.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/CAVESceneManagerConfig.cmake"
	INSTALL_DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
	PATH_VARS TARGET_INCLUDE_DIR TARGET_LIB_DIR TARGET_BIN_DIR CMAKECONFIG_INSTALL_DIR)
write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/CAVESceneManagerConfigVersion.cmake"
	VERSION ${CAVESCENEMANAGER_VERSION}
	COMPATIBILITY SameMajorVersion )
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/CAVESceneManagerConfig.cmake" "${CMAKE_CURRENT_BINARY_DIR}/CAVESceneManagerConfigVersion.cmake"
	DESTINATION "${CMAKECONFIG_INSTALL_DIR}")

###############################################################################
# Build renderservers
###############################################################################
add_subdirectory ( renderserver-src )
add_subdirectory ( renderserver-qt-src )


if ( NOT INVRS_SOURCE_DIR)
	configure_file ( 
		"${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
		"${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake"
		IMMEDIATE @ONLY)
	add_custom_target ( uninstall
		"${CMAKE_COMMAND}" -P
		"${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake")
endif ( NOT INVRS_SOURCE_DIR)

