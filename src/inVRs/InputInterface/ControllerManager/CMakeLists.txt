##############################################################################
# Define sourcefiles
##############################################################################
aux_source_directory(. CONTROLLERMANAGER_SRCS)
#if (WIN32)
#	set (ALL_SRCS ${CONTROLLERMANAGER_SRCS} ControllerManager.def)
#else (WIN32)
	set(ALL_SRCS ${CONTROLLERMANAGER_SRCS})
#endif (WIN32)

##############################################################################
# Define target-directories for header and library files
##############################################################################
set (TARGET_INCLUDE_DIR ${INVRS_TARGET_INCLUDE_DIR}/inVRs/InputInterface/ControllerManager)
set (TARGET_LIB_DIR ${INVRS_TARGET_LIB_DIR})
set (TARGET_BIN_DIR ${INVRS_TARGET_BIN_DIR})

##############################################################################
# Set definition for exporting DLL information
##############################################################################
add_definitions (-DCONTROLLERMANAGER_EXPORTS)

find_package(OpenSG REQUIRED COMPONENTS OSGBase)
include_directories(${OpenSG_INCLUDE_DIRS})
add_definitions(${OpenSG_DEFINITIONS})

find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})
add_definitions(${GLUT_DEFINITIONS})

##############################################################################
# Build library
##############################################################################
add_library(inVRsControllerManager SHARED ${ALL_SRCS})

##############################################################################
# Define dependencies when built within inVRs
##############################################################################
if (INVRS_SOURCE_DIR)
	add_dependencies (inVRsControllerManager
		inVRsSystemCore
		irrXML)
endif (INVRS_SOURCE_DIR)

##############################################################################
# Define libraries which must be linked against
##############################################################################
target_link_libraries (inVRsControllerManager
	inVRsSystemCore
	irrXML)

target_link_libraries(inVRsControllerManager ${OpenSG_LIBRARIES})
target_link_libraries(inVRsControllerManager ${GLUT_LIBRARIES})

##############################################################################
# Add library to required libraries (when using this package)
##############################################################################
list (APPEND ControllerManager_LIBRARIES inVRsControllerManager)

##############################################################################
# Install library and header files
##############################################################################
install (FILES AxisCorrection.h
		ButtonCorrection.h
		Controller.h
		ControllerManager.h
		ControllerManagerSharedLibraryExports.h
		DeviceIndexMapping.h
		GlutCharKeyboardDevice.h
		GlutMouseDevice.h
		GlutSensorEmulatorDevice.h
		InputDevice.h
		InputDeviceBase.h
		SensorCorrection.h
		StrangeDevice.h
		UdpDevice.h
	DESTINATION ${TARGET_INCLUDE_DIR})

install(
	TARGETS inVRsControllerManager
	ARCHIVE DESTINATION ${TARGET_LIB_DIR}
	LIBRARY DESTINATION ${TARGET_LIB_DIR}
	RUNTIME DESTINATION ${TARGET_BIN_DIR}
)

##############################################################################
# Export all variables needed later for building
##############################################################################
set ( INVRS_EXPORT_ControllerManager_INCLUDE_DIRS ${ControllerManager_INCLUDE_DIRS} ${OpenSG_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
set ( INVRS_EXPORT_ControllerManager_LIBRARY_DIRS ${ControllerManager_LIBRARY_DIRS})
set ( INVRS_EXPORT_ControllerManager_LIBRARIES ${ControllerManager_LIBRARIES} ${OpenSG_LIBRARIES} ${GLUT_LIBRARIES})
set ( INVRS_EXPORT_ControllerManager_DEFINITIONS ${GLUT_DEFINITIONS})
INVRS_ADD_EXPORTS( ControllerManager )


##############################################################################
# Duplicate Library but this time without GLUT
##############################################################################

set(GLUTLESS_SRCS
	AxisCorrection.cpp
	ButtonCorrection.cpp
	Controller.cpp
	ControllerManager.cpp
	DeviceIndexMapping.cpp
	InputDevice.cpp
	InputDeviceBase.cpp
	SensorCorrection.cpp
	StrangeDevice.cpp
	UdpDevice.cpp
)
#if (WIN32)
#	set (GLUTLESS_SRCS ${GLUTLESS_SRCS} ControllerManager.def)
#endif (WIN32)

add_library(inVRsControllerManagerGlutless SHARED ${GLUTLESS_SRCS})
set_target_properties(inVRsControllerManagerGlutless PROPERTIES COMPILE_DEFINITIONS "DISABLE_GLUT_DEVICES")

if (INVRS_SOURCE_DIR)
	add_dependencies (inVRsControllerManagerGlutless inVRsSystemCore irrXML)
endif (INVRS_SOURCE_DIR)

target_link_libraries (inVRsControllerManagerGlutless inVRsSystemCore irrXML)

find_package(OpenSG REQUIRED COMPONENTS OSGBase)
include_directories(${OpenSG_INCLUDE_DIRS})
target_link_libraries(inVRsControllerManagerGlutless ${OpenSG_LIBRARIES})

install (TARGETS inVRsControllerManagerGlutless
	ARCHIVE DESTINATION ${TARGET_LIB_DIR}
	LIBRARY DESTINATION ${TARGET_LIB_DIR}
	RUNTIME DESTINATION ${TARGET_BIN_DIR}
)

##############################################################################
# Export all variables needed later for building
##############################################################################
set ( INVRS_EXPORT_ControllerManagerGlutless_INCLUDE_DIRS ${ControllerManager_INCLUDE_DIRS} ${OpenSG_INCLUDE_DIRS})
set ( INVRS_EXPORT_ControllerManagerGlutless_LIBRARY_DIRS ${ControllerManager_LIBRARY_DIRS})
set ( INVRS_EXPORT_ControllerManagerGlutless_LIBRARIES inVRsControllerManagerGlutless ${OpenSG_LIBRARIES})
set ( INVRS_EXPORT_ControllerManagerGlutless_DEFINITIONS)
INVRS_ADD_EXPORTS( ControllerManagerGlutless )

