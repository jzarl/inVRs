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
        project (INVRS_TOOL_TEXTUREMANAGER)

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
aux_source_directory(. TEXTUREMANAGER_SRCS)
set(ALL_SRCS ${TEXTUREMANAGER_SRCS})

##############################################################################
# Define target-directories for header and library files
##############################################################################
set (TARGET_INCLUDE_DIR ${INVRS_TARGET_INCLUDE_DIR}/inVRs/tools/libraries/TextureManager)
set (TARGET_LIB_DIR ${INVRS_TARGET_LIB_DIR})
set (TARGET_BIN_DIR ${INVRS_TARGET_BIN_DIR})

##############################################################################
# Set definition for exporting DLL information
##############################################################################
add_definitions (-DINVRSTEXTUREMANAGER_EXPORTS)

find_package(OpenSG REQUIRED OSGBase OSGDrawable)
include_directories(${OpenSG_INCLUDE_DIRS})
add_definitions(${OpenSG_DEFINITIONS})
##############################################################################
# Build library
##############################################################################
add_library(inVRsTextureManager SHARED ${ALL_SRCS})

##############################################################################
# Define dependencies when built within inVRs
##############################################################################
if (INVRS_SOURCE_DIR)
	add_dependencies (inVRsTextureManager
		inVRsSystemCore
		irrXML)
endif (INVRS_SOURCE_DIR)

##############################################################################
# Define libraries which must be linked against
##############################################################################
target_link_libraries (inVRsTextureManager
	inVRsSystemCore
	irrXML)


target_link_libraries(inVRsTextureManager ${OpenSG_LIBRARIES})


##############################################################################
# Add library to required libraries (when using this package)
##############################################################################
list (APPEND TextureManager_LIBRARIES inVRsTextureManager)

##############################################################################
# Install library and header files
##############################################################################
install (FILES EffectTextureManager.h
	DESTINATION ${TARGET_INCLUDE_DIR})

install (TARGETS inVRsTextureManager
	ARCHIVE DESTINATION ${TARGET_LIB_DIR}
	LIBRARY DESTINATION ${TARGET_LIB_DIR}
	RUNTIME DESTINATION ${TARGET_BIN_DIR}
)

##############################################################################
# Export all variables needed later for building
##############################################################################
set ( INVRS_EXPORT_TextureManager_INCLUDE_DIRS ${TextureManager_INCLUDE_DIRS} ${OpenSG_INCLUDE_DIRS})
set ( INVRS_EXPORT_TextureManager_LIBRARY_DIRS ${TextureManager_LIBRARY_DIRS})
set ( INVRS_EXPORT_TextureManager_LIBRARIES ${TextureManager_LIBRARIES} ${OpenSG_LIBRARIES})
set ( INVRS_EXPORT_TextureManager_DEFINITIONS ${TextureManager_DEFINITIONS})
INVRS_ADD_EXPORTS( TextureManager )

