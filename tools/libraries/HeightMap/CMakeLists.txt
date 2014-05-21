###############################################################################
# Check if built within inVRs folder structure (tools-folder) or separately
###############################################################################
if ( NOT INVRS_SOURCE_DIR )

	#######################################################################
	# Set minimum cmake version
	#######################################################################
	cmake_minimum_required (VERSION 2.6)

	#######################################################################
	# Set project name
	#######################################################################
	project (INVRS_TOOL_HEIGHTMAP)

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
	AbstractHeightMapManager.cpp
	HeightMapModifierBase.cpp
)
set(ALL_SRCS
	HeightMap.cpp
	HeightMapManager.cpp
	OpenSGHeightMapManager.cpp
	)

# disable multi-character character constant warning on file HeightMap.cpp:
if( CMAKE_COMPILER_IS_GNUCXX )
	set_property( SOURCE HeightMap.cpp APPEND PROPERTY COMPILE_FLAGS "-Wno-multichar")
endif()

##############################################################################
# Define target-directories for header and library files
##############################################################################
set (TARGET_INCLUDE_DIR ${INVRS_TARGET_INCLUDE_DIR}/inVRs/tools/libraries/HeightMap)
set (TARGET_LIB_DIR ${INVRS_TARGET_LIB_DIR})
set (TARGET_BIN_DIR ${INVRS_TARGET_BIN_DIR})

##############################################################################
# Set definition for exporting DLL information
##############################################################################
add_definitions (-DINVRSHEIGHTMAP_EXPORTS)

##############################################################################
# Build library
##############################################################################
add_library(inVRsHeightMapBase STATIC ${BASE_SRCS})

##############################################################################
# Define dependencies when built within inVRs
##############################################################################
if (INVRS_SOURCE_DIR)
	add_dependencies (inVRsHeightMapBase inVRsSystemCore)
endif (INVRS_SOURCE_DIR)

target_link_libraries (inVRsHeightMapBase inVRsSystemCore)

##############################################################################
# Install library and header files
##############################################################################
install (FILES
		HeightMapInterface.h
		AbstractHeightMapManager.h
		HeightMapModifierBase.h
	DESTINATION ${TARGET_INCLUDE_DIR})

install (TARGETS inVRsHeightMapBase
	ARCHIVE DESTINATION ${TARGET_LIB_DIR}
	LIBRARY DESTINATION ${TARGET_LIB_DIR}
	RUNTIME DESTINATION ${TARGET_BIN_DIR}
)

set ( INVRS_EXPORT_HeightMapBase_INCLUDE_DIRS ${HeightMap_INCLUDE_DIRS})
set ( INVRS_EXPORT_HeightMapBase_LIBRARIES inVRsHeightMapBase)
INVRS_ADD_EXPORTS( HeightMapBase )

find_package(OpenSG REQUIRED OSGBase)
include_directories(${OpenSG_INCLUDE_DIRS})
add_definitions(${OpenSG_DEFINITIONS})

##############################################################################
# Build library
##############################################################################
add_library(inVRsHeightMap SHARED ${ALL_SRCS})

##############################################################################
# Define dependencies when built within inVRs
##############################################################################
add_dependencies(inVRsHeightMap inVRsHeightMapBase)
target_link_libraries(inVRsHeightMap inVRsHeightMapBase)
if (INVRS_SOURCE_DIR)
	add_dependencies (inVRsHeightMap inVRsOpenSGSceneGraphInterface)
endif (INVRS_SOURCE_DIR)

##############################################################################
# Define libraries which must be linked against
##############################################################################
target_link_libraries (inVRsHeightMap
	inVRsSystemCore
	inVRsOpenSGSceneGraphInterface)

target_link_libraries(inVRsHeightMap ${OpenSG_LIBRARIES})

##############################################################################
# Add library to required libraries (when using this package)
##############################################################################
list (APPEND HeightMap_LIBRARIES inVRsHeightMap inVRsHeightMapBase)

##############################################################################
# Install library and header files
##############################################################################
install (FILES HeightMap.h
		HeightMapManager.h
		HeightMapModifier.h
		OpenSGHeightMapManager.h
	DESTINATION ${TARGET_INCLUDE_DIR})

install (TARGETS inVRsHeightMap
	ARCHIVE DESTINATION ${TARGET_LIB_DIR}
	LIBRARY DESTINATION ${TARGET_LIB_DIR}
	RUNTIME DESTINATION ${TARGET_BIN_DIR}
)

##############################################################################
# Export all variables needed later for building
##############################################################################
set ( INVRS_EXPORT_HeightMap_INCLUDE_DIRS ${HeightMap_INCLUDE_DIRS} ${OpenSG_INCLUDE_DIRS})
set ( INVRS_EXPORT_HeightMap_LIBRARY_DIRS ${HeightMap_LIBRARY_DIRS})
set ( INVRS_EXPORT_HeightMap_LIBRARIES ${HeightMap_LIBRARIES} ${OpenSG_LIBRARIES})
set ( INVRS_EXPORT_HeightMap_DEFINITIONS ${HeightMap_DEFINITIONS})
INVRS_ADD_EXPORTS( HeightMap )

