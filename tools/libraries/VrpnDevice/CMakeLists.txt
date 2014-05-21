##############################################################################
# Define sourcefiles
##############################################################################
set(ALL_SRCS VrpnDevice.cpp VrpnDevice.h)

##############################################################################
# Define target-directories for header and library files
##############################################################################
set (TARGET_INCLUDE_DIR ${INVRS_TARGET_INCLUDE_DIR}/inVRs/tools/libraries/VrpnDevice)
set (TARGET_LIB_DIR ${INVRS_TARGET_LIB_DIR})
set (TARGET_BIN_DIR ${INVRS_TARGET_BIN_DIR})

##############################################################################
# Set definition for exporting DLL information
##############################################################################
add_definitions (-DINVRSVRPNDEVICE_EXPORTS)

find_package(VRPN REQUIRED)
include_directories(${VRPN_INCLUDE_DIRS})
include_directories(${VRPN_INCLUDE_DIR}/vrpn)
add_definitions(${VRPN_DEFINITIONS})

find_package(OpenSG REQUIRED COMPONENTS OSGBase)
include_directories(${OpenSG_INCLUDE_DIRS})
add_definitions(${OpenSG_DEFINITIONS})

##############################################################################
# Build library
##############################################################################
add_library(inVRsVrpnDevice SHARED ${ALL_SRCS})

##############################################################################
# Define dependencies when built within inVRs
##############################################################################
if (INVRS_SOURCE_DIR)
	add_dependencies (inVRsVrpnDevice 
		inVRsSystemCore 
		inVRsControllerManager
		irrXML)
endif (INVRS_SOURCE_DIR)

##############################################################################
# Define libraries which must be linked against
##############################################################################
target_link_libraries (inVRsVrpnDevice 
	inVRsSystemCore inVRsControllerManager
	irrXML)
target_link_libraries(inVRsVrpnDevice ${VRPN_LIBRARIES})
target_link_libraries(inVRsVrpnDevice ${OpenSG_LIBRARIES})

##############################################################################
# Add library to required libraries (when using this package)
##############################################################################
list (APPEND VrpnDevice_INCLUDE_DIRS ${VRPN_INCLUDE_DIRS} ${VRPN_INCLUDE_DIR}/vrpn)
list (APPEND VrpnDevice_LIBRARIES ${VRPN_LIBRARIES})
list (APPEND VrpnDevice_LIBRARIES inVRsVrpnDevice)

##############################################################################
# Install library and header files
##############################################################################
install (FILES VrpnDevice.h
	DESTINATION ${TARGET_INCLUDE_DIR})

install (TARGETS inVRsVrpnDevice
	ARCHIVE DESTINATION ${TARGET_LIB_DIR}
	LIBRARY DESTINATION ${TARGET_LIB_DIR}
	RUNTIME DESTINATION ${TARGET_BIN_DIR}
)

##############################################################################
# Export all variables needed later for building
##############################################################################
set ( INVRS_EXPORT_VrpnDevice_INCLUDE_DIRS ${VrpnDevice_INCLUDE_DIRS})
set ( INVRS_EXPORT_VrpnDevice_LIBRARY_DIRS ${VrpnDevice_LIBRARY_DIRS})
set ( INVRS_EXPORT_VrpnDevice_LIBRARIES ${VrpnDevice_LIBRARIES} inVRsSystemCore inVRsControllerManager irrXML)
set ( INVRS_EXPORT_VrpnDevice_DEFINITIONS ${VrpnDevice_DEFINITIONS})
INVRS_ADD_EXPORTS( VrpnDevice )

