###############################################################################
# Only execute commands if file was not included already
###############################################################################
if (NOT AVATARA_CONFIG_CMAKE_LOADED)

###############################################################################
# Add local module path to CMAKE_MODULE_PATH
###############################################################################
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

###############################################################################
# Define variables for different build options
###############################################################################
find_package(OpenSG)
if (OpenSG_FOUND)
	option(AVATARA_ENABLE_OPENSG_SUPPORT "Enable/disable support for OpenSG" ON)
else (OpenSG_FOUND)
	option(AVATARA_ENABLE_OPENSG_SUPPORT "Enable/disable support for OpenSG" ON)
endif (OpenSG_FOUND)
mark_as_advanced(AVATARA_ENABLE_OPENSG_SUPPORT)

option(AVATARA_BUILD_EXAMPLE_APPLICATIONS "Enable/disable building of example applications" ON)
mark_as_advanced(AVATARA_BUILD_EXAMPLE_APPLICATIONS)

###############################################################################
# Find GLUT headers and libraries
###############################################################################
if (AVATARA_BUILD_EXAMPLE_APPLICATIONS)
	find_package(GLUT)
	include_directories (${GLUT_INCLUDE_DIR})
endif (AVATARA_BUILD_EXAMPLE_APPLICATIONS)

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


set (AVATARA_CONFIG_CMAKE_LOADED ON)
endif (NOT AVATARA_CONFIG_CMAKE_LOADED)

