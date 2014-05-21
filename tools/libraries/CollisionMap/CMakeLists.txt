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
	project (INVRS_TOOL_COLLISIONMAP)

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

##############################################################################
# Define sourcefiles
##############################################################################
set(BASE_SRCS
	CheckCollisionModifierBase.cpp
	CollisionCircle.cpp
	CollisionLineSet.cpp
	CollisionMap.cpp
	CollisionObject.cpp
)
set(ALL_SRCS
	OpenSGCollisionLineSetFactory.cpp)

##############################################################################
# Define target-directories for header and library files
##############################################################################
set (TARGET_INCLUDE_DIR ${INVRS_TARGET_INCLUDE_DIR}/inVRs/tools/libraries/CollisionMap)
set (TARGET_LIB_DIR ${INVRS_TARGET_LIB_DIR})
set (TARGET_BIN_DIR ${INVRS_TARGET_BIN_DIR})

##############################################################################
# Set definition for exporting DLL information
##############################################################################
add_definitions (-DINVRSCOLLISIONMAP_EXPORTS)

##############################################################################
# Build library
##############################################################################
add_library(inVRsCollisionMapBase STATIC ${BASE_SRCS})

if (INVRS_SOURCE_DIR)
	add_dependencies (inVRsCollisionMapBase inVRsSystemCore irrXML)
endif (INVRS_SOURCE_DIR)

target_link_libraries (inVRsCollisionMapBase inVRsSystemCore irrXML)

install (FILES
		CheckCollisionModifierBase.h
		CollisionCircle.h
		CollisionLineSet.h
		CollisionMapExports.h
		CollisionMap.h
		CollisionObject.h
	DESTINATION ${TARGET_INCLUDE_DIR})

install (TARGETS inVRsCollisionMapBase
	ARCHIVE DESTINATION ${TARGET_LIB_DIR}
	LIBRARY DESTINATION ${TARGET_LIB_DIR}
	RUNTIME DESTINATION ${TARGET_BIN_DIR}
)

set ( INVRS_EXPORT_CollisionMapBase_INCLUDE_DIRS ${CollisionMap_INCLUDE_DIRS})
set ( INVRS_EXPORT_CollisionMapBase_LIBRARIES inVRsCollisionMapBase irrXML)
INVRS_ADD_EXPORTS( CollisionMapBase )

find_package(OpenSG REQUIRED OSGDrawable)
include_directories(${OpenSG_INCLUDE_DIRS})
add_definitions(${OpenSG_DEFINITIONS})

##############################################################################
# Build library
##############################################################################
add_library(inVRsCollisionMap SHARED ${ALL_SRCS})

add_dependencies(inVRsCollisionMap inVRsCollisionMapBase)
target_link_libraries(inVRsCollisionMap inVRsCollisionMapBase)

##############################################################################
# Define libraries which must be linked against
##############################################################################
target_link_libraries (inVRsCollisionMap inVRsSystemCore irrXML)

target_link_libraries(inVRsCollisionMap ${OpenSG_LIBRARIES})

##############################################################################
# Add library to required libraries (when using this package)
##############################################################################
list (APPEND CollisionMap_LIBRARIES inVRsCollisionMap inVRsCollisionMapBase)

##############################################################################
# Install library and header files
##############################################################################
install (FILES
		CheckCollisionModifier.h
		OpenSGCollisionLineSetFactory.h
	DESTINATION ${TARGET_INCLUDE_DIR})

install (TARGETS inVRsCollisionMap
	ARCHIVE DESTINATION ${TARGET_LIB_DIR}
	LIBRARY DESTINATION ${TARGET_LIB_DIR}
	RUNTIME DESTINATION ${TARGET_BIN_DIR}
)

##############################################################################
# Export all variables needed later for building
##############################################################################
set ( INVRS_EXPORT_CollisionMap_INCLUDE_DIRS ${CollisionMap_INCLUDE_DIRS} ${INVRS_EXPORT_CollisionMapBase_INCLUDE_DIRS} ${OpenSG_INCLUDE_DIRS})
set ( INVRS_EXPORT_CollisionMap_LIBRARY_DIRS ${CollisionMap_LIBRARY_DIRS})
set ( INVRS_EXPORT_CollisionMap_LIBRARIES ${CollisionMap_LIBRARIES} ${INVRS_EXPORT_CollisionMapBase_LIBRARIES} ${OpenSG_LIBRARIES})
set ( INVRS_EXPORT_CollisionMap_DEFINITIONS ${CollisionMap_DEFINITIONS})
INVRS_ADD_EXPORTS( CollisionMap )

