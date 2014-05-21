##############################################################################
# Define sourcefiles
##############################################################################
aux_source_directory(. NAVIGATION_SRCS)
#if (WIN32)
#	set (ALL_SRCS ${NAVIGATION_SRCS} Navigation.def)
#else (WIN32)
	set(ALL_SRCS ${NAVIGATION_SRCS})
#endif (WIN32)

##############################################################################
# Define target-directories for header and library files
##############################################################################
set (TARGET_INCLUDE_DIR ${INVRS_TARGET_INCLUDE_DIR}/inVRs/Modules/Navigation)
set (TARGET_LIB_DIR ${INVRS_TARGET_LIB_DIR})
set (TARGET_BIN_DIR ${INVRS_TARGET_BIN_DIR})

##############################################################################
# Set definition for exporting DLL information
##############################################################################
add_definitions (-DINVRSNAVIGATION_EXPORTS)

find_package(OpenSG REQUIRED OSGBase)
include_directories(${OpenSG_INCLUDE_DIRS})
add_definitions(${OpenSG_DEFINITIONS})

##############################################################################
# Build library
##############################################################################
add_library(inVRsNavigation SHARED ${ALL_SRCS})

##############################################################################
# Define dependencies when built within inVRs
##############################################################################
if (INVRS_SOURCE_DIR)
	add_dependencies (inVRsNavigation
		inVRsSystemCore
		irrXML)
endif (INVRS_SOURCE_DIR)

##############################################################################
# Define libraries which must be linked against
##############################################################################
target_link_libraries(inVRsNavigation
	inVRsSystemCore
	irrXML)


##############################################################################
# Add library to required libraries (when using this package)
##############################################################################
list (APPEND Navigation_LIBRARIES inVRsNavigation)

##############################################################################
# Install library and header files
##############################################################################
install (FILES AllInOneTranslationButtonModel.h
		AllInOneTranslationModel.h
		Navigation.h
		NavigationSharedLibraryExports.h
		NavigationEvents.h
		OrientationModels.h
		SpeedModels.h
		TranslationModels.h
	DESTINATION ${TARGET_INCLUDE_DIR})

install (TARGETS inVRsNavigation
	ARCHIVE DESTINATION ${TARGET_LIB_DIR}
	LIBRARY DESTINATION ${TARGET_LIB_DIR}
	RUNTIME DESTINATION ${TARGET_BIN_DIR}
)

##############################################################################
# Export all variables needed later for building
##############################################################################
set ( INVRS_EXPORT_Navigation_INCLUDE_DIRS ${Navigation_INCLUDE_DIRS})
set ( INVRS_EXPORT_Navigation_LIBRARY_DIRS ${Navigation_LIBRARY_DIRS})
set ( INVRS_EXPORT_Navigation_LIBRARIES ${Navigation_LIBRARIES})
set ( INVRS_EXPORT_Navigation_DEFINITIONS ${Navigation_DEFINITIONS})
INVRS_ADD_EXPORTS( Navigation )

