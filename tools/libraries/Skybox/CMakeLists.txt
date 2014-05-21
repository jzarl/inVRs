##############################################################################
# Define sourcefiles
##############################################################################
aux_source_directory(. SKYBOX_SRCS)
set(ALL_SRCS ${SKYBOX_SRCS})

##############################################################################
# Define target-directories for header and library files
##############################################################################
set (TARGET_INCLUDE_DIR ${INVRS_TARGET_INCLUDE_DIR}/inVRs/tools/libraries/Skybox)
set (TARGET_LIB_DIR ${INVRS_TARGET_LIB_DIR})
set (TARGET_BIN_DIR ${INVRS_TARGET_BIN_DIR})

##############################################################################
# Set definition for exporting DLL information
##############################################################################
add_definitions (-DINVRSSKYBOX_EXPORTS)

find_package(OpenSG REQUIRED COMPONENTS OSGBase OSGGroup OSGDrawable)
include_directories(${OpenSG_INCLUDE_DIRS})
add_definitions(${OpenSG_DEFINITIONS})

##############################################################################
# Build library
##############################################################################
add_library(inVRsSkybox SHARED ${ALL_SRCS})

##############################################################################
# Define dependencies when built within inVRs
##############################################################################
if (INVRS_SOURCE_DIR)
	add_dependencies (inVRsSkybox
		inVRsSystemCore
		inVRsTextureManager)
endif (INVRS_SOURCE_DIR)

##############################################################################
# Define libraries which must be linked against
##############################################################################
target_link_libraries (inVRsSkybox
	inVRsSystemCore
	inVRsTextureManager)

target_link_libraries(inVRsSkybox ${OpenSG_LIBRARIES})

##############################################################################
# Add library to required libraries (when using this package)
##############################################################################
list (APPEND Skybox_LIBRARIES inVRsSkybox)

##############################################################################
# Install library and header files
##############################################################################
install (FILES Skybox.h
	DESTINATION ${TARGET_INCLUDE_DIR})

install (TARGETS inVRsSkybox
	ARCHIVE DESTINATION ${TARGET_LIB_DIR}
	LIBRARY DESTINATION ${TARGET_LIB_DIR}
	RUNTIME DESTINATION ${TARGET_BIN_DIR}
)

##############################################################################
# Export all variables needed later for building
##############################################################################
set ( INVRS_EXPORT_Skybox_INCLUDE_DIRS ${Skybox_INCLUDE_DIRS} ${OpenSG_INCLUDE_DIRS})
set ( INVRS_EXPORT_Skybox_LIBRARY_DIRS ${Skybox_LIBRARY_DIRS})
set ( INVRS_EXPORT_Skybox_LIBRARIES ${Skybox_LIBRARIES} ${OpenSG_LIBRARIES})
set ( INVRS_EXPORT_Skybox_DEFINITIONS ${Skybox_DEFINITIONS})
INVRS_ADD_EXPORTS( Skybox )

