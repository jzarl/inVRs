##############################################################################
# Define sourcefiles
##############################################################################
aux_source_directory(. NETWORK_SRCS)
#if (WIN32)
#	set (ALL_SRCS ${NETWORK_SRCS} Network.def)
#else (WIN32)
	set (ALL_SRCS ${NETWORK_SRCS})
#endif (WIN32)

##############################################################################
# Define target-directories for header and library files
##############################################################################
set (TARGET_INCLUDE_DIR ${INVRS_TARGET_INCLUDE_DIR}/inVRs/Modules/Network)
set (TARGET_LIB_DIR ${INVRS_TARGET_LIB_DIR})
set (TARGET_BIN_DIR ${INVRS_TARGET_BIN_DIR})

##############################################################################
# Set definition for exporting DLL information
##############################################################################
add_definitions (-DINVRSNETWORK_EXPORTS)

find_package(OpenSG REQUIRED COMPONENTS OSGBase)
include_directories(${OpenSG_INCLUDE_DIRS})
add_definitions(${OpenSG_DEFINITIONS})

##############################################################################
# Build library
##############################################################################
add_library(inVRsNetwork SHARED ${ALL_SRCS})

##############################################################################
# Define dependencies when built within inVRs
##############################################################################
if (INVRS_SOURCE_DIR)
	add_dependencies (inVRsNetwork
		inVRsSystemCore
		irrXML)
endif (INVRS_SOURCE_DIR)

##############################################################################
# Define libraries which must be linked against
##############################################################################
target_link_libraries (inVRsNetwork
	inVRsSystemCore
	irrXML)


target_link_libraries(inVRsNetwork ${OpenSG_LIBRARIES})

if (WIN32)
	target_link_libraries(inVRsNetwork Ws2_32.lib)
endif (WIN32)

##############################################################################
# Add library to required libraries (when using this package)
##############################################################################
list (APPEND Network_LIBRARIES inVRsNetwork)

##############################################################################
# Install library and header files
##############################################################################
install (FILES Network.h
		SendReceiveThread.h
		ServerThread.h
		NetworkSharedLibraryExports.h
	DESTINATION ${TARGET_INCLUDE_DIR})

install (TARGETS inVRsNetwork
	ARCHIVE DESTINATION ${TARGET_LIB_DIR}
	LIBRARY DESTINATION ${TARGET_LIB_DIR}
	RUNTIME DESTINATION ${TARGET_BIN_DIR}
)

##############################################################################
# Export all variables needed later for building
##############################################################################
set ( INVRS_EXPORT_Network_INCLUDE_DIRS ${Network_INCLUDE_DIRS} ${OpenSG_INCLUDE_DIRS})
set ( INVRS_EXPORT_Network_LIBRARY_DIRS ${Network_LIBRARY_DIRS})
set ( INVRS_EXPORT_Network_LIBRARIES ${Network_LIBRARIES} ${OpenSG_LIBRARIES})
set ( INVRS_EXPORT_Network_DEFINITIONS ${Network_DEFINITIONS})
INVRS_ADD_EXPORTS( Network )

