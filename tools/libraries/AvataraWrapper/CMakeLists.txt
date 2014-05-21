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
    project (INVRS_TOOL_AVATARAAVATAR)

    #######################################################################
    # Set required inVRs Modules and Tools
    #######################################################################
    set (inVRs_REQUIRED_MODULES
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
# Include config file which loads dependencies (like avatara)
###############################################################################
include (${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.cmake)

##############################################################################
# Define sourcefiles
##############################################################################
set(BASE_SRCS
	AvataraAvatarBase.cpp
	AvatarAnimationWriter.cpp
)
set(ALL_SRCS
	OpenSGAvataraAvatar.cpp
)

##############################################################################
# Define target-directories for header and library files
##############################################################################
set (TARGET_INCLUDE_DIR ${INVRS_TARGET_INCLUDE_DIR}/inVRs/tools/libraries/AvataraWrapper)
set (TARGET_LIB_DIR ${INVRS_TARGET_LIB_DIR})
set (TARGET_BIN_DIR ${INVRS_TARGET_BIN_DIR})

add_library(inVRsAvataraWrapperBase STATIC ${BASE_SRCS})
if (INVRS_SOURCE_DIR)
    add_dependencies (inVRsAvataraWrapperBase inVRsSystemCore)
    if (INVRS_ENABLE_EXTERNAL_AVATARA)
        add_dependencies (inVRsAvataraWrapperBase avatara)
    endif (INVRS_ENABLE_EXTERNAL_AVATARA)
endif (INVRS_SOURCE_DIR)

target_link_libraries (inVRsAvataraWrapperBase
    inVRsSystemCore irrXML ${avatara_LIBRARIES})

install(FILES
        AvatarAnimationWriter.h
        AvataraAvatarBase.h
    DESTINATION ${TARGET_INCLUDE_DIR})
install (TARGETS inVRsAvataraWrapperBase
	ARCHIVE DESTINATION ${TARGET_LIB_DIR}
	LIBRARY DESTINATION ${TARGET_LIB_DIR}
	RUNTIME DESTINATION ${TARGET_BIN_DIR}
)

set ( INVRS_EXPORT_AvataraWrapperBase_INCLUDE_DIRS ${AvataraWrapper_INCLUDE_DIRS})
set ( INVRS_EXPORT_AvataraWrapperBase_LIBRARIES inVRsAvataraWrapperBase ${avatara_LIBRARIES})
INVRS_ADD_EXPORTS( AvataraWrapperBase )

##############################################################################
# Set definition for exporting DLL information
##############################################################################
add_definitions (-DINVRSAVATARAWRAPPER_EXPORTS)


find_package(OpenSG REQUIRED OSGBase)
include_directories(${OpenSG_INCLUDE_DIRS})
add_definitions(${OpenSG_DEFINITIONS})

##############################################################################
# Build library
##############################################################################
add_library(inVRsAvataraWrapper SHARED ${ALL_SRCS})

##############################################################################
# Define dependencies when built within inVRs
##############################################################################
if (INVRS_SOURCE_DIR)
    add_dependencies (inVRsAvataraWrapper
        inVRsOpenSGSceneGraphInterface
        inVRsAvataraWrapperBase)
endif (INVRS_SOURCE_DIR)

target_link_libraries (inVRsAvataraWrapper
    inVRsSystemCore
    inVRsOpenSGSceneGraphInterface
    irrXML
    inVRsAvataraWrapperBase
    ${avatara_LIBRARIES})


target_link_libraries(inVRsAvataraWrapper ${OpenSG_LIBRARIES})

##############################################################################
# Add library to required libraries (when using this package)
##############################################################################
list (APPEND AvataraWrapper_LIBRARIES inVRsAvataraWrapper inVRsAvataraWrapperBase)

##############################################################################
# Install library and header files
##############################################################################
install (FILES
        AvataraAvatar.h
        AvataraWrapperExports.h
        OpenSGAvataraAvatar.h
    DESTINATION ${TARGET_INCLUDE_DIR})

install (TARGETS inVRsAvataraWrapper
	ARCHIVE DESTINATION ${TARGET_LIB_DIR}
	LIBRARY DESTINATION ${TARGET_LIB_DIR}
	RUNTIME DESTINATION ${TARGET_BIN_DIR}
)

##############################################################################
# Export all variables needed later for building
##############################################################################
set ( INVRS_EXPORT_AvataraWrapper_INCLUDE_DIRS ${AvataraWrapper_INCLUDE_DIRS} ${INVRS_EXPORT_AvataraWrapperBase_INCLUDE_DIRS})
set ( INVRS_EXPORT_AvataraWrapper_LIBRARY_DIRS ${AvataraWrapper_LIBRARY_DIRS})
set ( INVRS_EXPORT_AvataraWrapper_LIBRARIES ${AvataraWrapper_LIBRARIES} ${INVRS_EXPORT_AvataraWrapperBase_LIBRARIES})
set ( INVRS_EXPORT_AvataraWrapper_DEFINITIONS ${AvataraWrapper_DEFINITIONS})
INVRS_ADD_EXPORTS( AvataraWrapper )

