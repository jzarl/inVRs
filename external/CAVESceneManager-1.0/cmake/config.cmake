###############################################################################
# Only execute commands if file was not included already
###############################################################################
if (NOT CAVESCENEMANAGER_CONFIG_CMAKE_LOADED)


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


###############################################################################
# Add local module path to CMAKE_MODULE_PATH
###############################################################################
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

###############################################################################
# Set RPATH settings
###############################################################################
# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

# the RPATH to be used when installing
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


###############################################################################
# set formatting for eclipse output
###############################################################################
if (CMAKE_COMPILER_IS_GNUCC)
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0")
endif (CMAKE_COMPILER_IS_GNUCC)
if (CMAKE_COMPILER_IS_GNUCXX)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
endif (CMAKE_COMPILER_IS_GNUCXX)


set (CAVESCENEMANAGER_CONFIG_CMAKE_LOADED ON)
endif (NOT CAVESCENEMANAGER_CONFIG_CMAKE_LOADED)

