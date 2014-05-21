###############################################################################
# Check if built within inVRs folder structure (tools-folder) or separately
###############################################################################
if ( NOT INVRS_SOURCE_DIR )

	#######################################################################
	# Set minimum cmake version
	#######################################################################
	cmake_minimum_required (VERSION 2.4)

	#######################################################################
	# Set project name
	#######################################################################
	project (INVRS_TOOL_OPENSGAPPLICATIONBASE)

	#######################################################################
	# Set required inVRs Modules and Tools)
	#######################################################################
	set (inVRs_REQUIRED_MODULES
		ControllerManager
		OpenSGSceneGraphInterface)

	#######################################################################
	# Include config.cmake
	#######################################################################
	include (${CMAKE_CURRENT_SOURCE_DIR}/cmake/config_buildExternal.cmake)

	#######################################################################
	# Set target include and library directories
	#######################################################################
	set (INVRS_TARGET_INCLUDE_DIR ${inVRs_INCLUDE_DIR})
	set (INVRS_TARGET_LIB_DIR ${inVRs_LIBRARY_DIR})

endif ( NOT INVRS_SOURCE_DIR )

###############################################################################
# Include config file which loads dependencies (like CAVESceneManager)
###############################################################################
include (${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.cmake)

##############################################################################
# Define sourcefiles
##############################################################################
set (OPENSGAPPLICATIONBASE_SRCS
	OpenSGApplicationBase.cpp
	shadowviewportmgr.cpp
)
set (ALL_SRCS ${OPENSGAPPLICATIONBASE_SRCS})

##############################################################################
# Define target-directories for header and library files
##############################################################################
set (TARGET_INCLUDE_DIR	${INVRS_TARGET_INCLUDE_DIR}/inVRs/tools/libraries/OpenSGApplicationBase)
set (TARGET_LIB_DIR ${INVRS_TARGET_LIB_DIR})
set (TARGET_BIN_DIR ${INVRS_TARGET_BIN_DIR})

##############################################################################
# Set definition for exporting DLL information
##############################################################################
add_definitions (-DINVRSOPENSGAPPLICATIONBASE_EXPORTS)

find_package(OpenSG REQUIRED OSGWindowGLUT OSGUtil OSGEffectGroups)
include_directories(${OpenSG_INCLUDE_DIRS})
add_definitions(${OpenSG_DEFINITIONS})

find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})
add_definitions(${GLUT_DEFINITIONS})

##############################################################################
# Build library
##############################################################################
add_library(inVRsOpenSGApplicationBase SHARED ${ALL_SRCS})

##############################################################################
# Define dependencies when built within inVRs
##############################################################################
if (INVRS_SOURCE_DIR)
	add_dependencies (inVRsOpenSGApplicationBase
		inVRsSystemCore
		inVRsControllerManager
		inVRsOpenSGSceneGraphInterface)
	if (INVRS_ENABLE_EXTERNAL_CAVESCENEMANAGER)
		add_dependencies (inVRsOpenSGApplicationBase CAVESceneManager)
	endif (INVRS_ENABLE_EXTERNAL_CAVESCENEMANAGER)
endif (INVRS_SOURCE_DIR)

##############################################################################
# Define libraries which must be linked against
##############################################################################
target_link_libraries (inVRsOpenSGApplicationBase
	inVRsSystemCore
	inVRsControllerManager
	inVRsOpenSGSceneGraphInterface)

if (INVRS_ENABLE_EXTERNAL_CAVESCENEMANAGER)
	target_link_libraries (inVRsOpenSGApplicationBase
		CAVESceneManager)
endif (INVRS_ENABLE_EXTERNAL_CAVESCENEMANAGER)

target_link_libraries(inVRsOpenSGApplicationBase ${OpenSG_LIBRARIES})

	
target_link_libraries(inVRsOpenSGApplicationBase ${GLUT_LIBRARIES})

##############################################################################
# Add library to required libraries (when using this package)
##############################################################################
list (APPEND OpenSGApplicationBase_LIBRARIES inVRsOpenSGApplicationBase)

##############################################################################
# Install library and header files
##############################################################################
install (FILES OpenSGApplicationBase.h
	DESTINATION ${TARGET_INCLUDE_DIR})

install (TARGETS inVRsOpenSGApplicationBase
	ARCHIVE DESTINATION ${TARGET_LIB_DIR}
	LIBRARY DESTINATION ${TARGET_LIB_DIR}
	RUNTIME DESTINATION ${TARGET_BIN_DIR}
)

##############################################################################
# Export all variables needed later for building
##############################################################################
set ( INVRS_EXPORT_OpenSGApplicationBase_INCLUDE_DIRS ${OpenSGApplicationBase_INCLUDE_DIRS} ${OpenSG_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
set ( INVRS_EXPORT_OpenSGApplicationBase_LIBRARY_DIRS ${OpenSGApplicationBase_LIBRARY_DIRS})
set ( INVRS_EXPORT_OpenSGApplicationBase_LIBRARIES ${OpenSGApplicationBase_LIBRARIES} ${OpenSG_LIBRARIES} ${GLUT_LIBRARIES} inVRsOpenSGSceneGraphInterface)
set ( INVRS_EXPORT_OpenSGApplicationBase_DEFINITIONS ${OpenSGApplicationBase_DEFINITIONS} ${OpenSG_DEFINITIONS})
INVRS_ADD_EXPORTS( OpenSGApplicationBase )

