##############################################################################
# Define sourcefiles
##############################################################################
aux_source_directory(. TRACKDDEVICE_SRCS)
set(ALL_SRCS ${TRACKDDEVICE_SRCS} TrackdDevice.h)

##############################################################################
# Define target-directories for header and library files
##############################################################################
set (TARGET_INCLUDE_DIR ${INVRS_TARGET_INCLUDE_DIR}/inVRs/tools/libraries/TrackdDevice)
set (TARGET_LIB_DIR ${INVRS_TARGET_LIB_DIR})
set (TARGET_BIN_DIR ${INVRS_TARGET_BIN_DIR})

##############################################################################
# Set definition for exporting DLL information
##############################################################################
add_definitions (-DINVRSTRACKDDEVICE_EXPORTS)

find_package(TRACKD REQUIRED)
include_directories(${TRACKD_INCLUDE_DIRS})
add_definitions(${TRACKD_DEFINITIONS})

find_package(OpenSG REQUIRED COMPONENTS OSGBase)
include_directories(${OpenSG_INCLUDE_DIRS})
add_definitions(${OpenSG_DEFINITIONS})

##############################################################################
# Build library
##############################################################################
add_library(inVRsTrackdDevice SHARED ${ALL_SRCS})

##############################################################################
# Define dependencies when built within inVRs
##############################################################################
if (INVRS_SOURCE_DIR)
	add_dependencies (inVRsTrackdDevice
		inVRsSystemCore
		inVRsControllerManager
		irrXML)
endif (INVRS_SOURCE_DIR)

##############################################################################
# Define libraries which must be linked against
##############################################################################
target_link_libraries (inVRsTrackdDevice 
	inVRsSystemCore inVRsControllerManager
	irrXML)

target_link_libraries(inVRsTrackdDevice ${TRACKD_LIBRARIES})
target_link_libraries(inVRsTrackdDevice ${OpenSG_LIBRARIES})

##############################################################################
# Add library to required libraries (when using this package)
##############################################################################
list (APPEND TrackdDevice_INCLUDE_DIRS ${TRACKD_INCLUDE_DIRS})
list (APPEND TrackdDevice_LIBRARIES ${TRACKD_LIBRARIES})
list (APPEND TrackdDevice_LIBRARIES inVRsTrackdDevice)

##############################################################################
# Install library and header files
##############################################################################
install (FILES TrackdDevice.h
	DESTINATION ${TARGET_INCLUDE_DIR})

install (TARGETS inVRsTrackdDevice
	ARCHIVE DESTINATION ${TARGET_LIB_DIR}
	LIBRARY DESTINATION ${TARGET_LIB_DIR}
	RUNTIME DESTINATION ${TARGET_BIN_DIR}
)

##############################################################################
# Export all variables needed later for building
##############################################################################
set ( INVRS_EXPORT_TrackdDevice_INCLUDE_DIRS ${TrackdDevice_INCLUDE_DIRS})
set ( INVRS_EXPORT_TrackdDevice_LIBRARY_DIRS ${TrackdDevice_LIBRARY_DIRS})
set ( INVRS_EXPORT_TrackdDevice_LIBRARIES ${TrackdDevice_LIBRARIES})
set ( INVRS_EXPORT_TrackdDevice_DEFINITIONS ${TrackdDevice_DEFINITIONS})
INVRS_ADD_EXPORTS( TrackdDevice )

