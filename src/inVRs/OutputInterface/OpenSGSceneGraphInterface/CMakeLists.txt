##############################################################################
# Define sourcefiles
##############################################################################
aux_source_directory(. OPENSGSCENEGRAPHINTERFACE_SRCS)
#if (WIN32)
#	set (ALL_SRCS ${OPENSGSCENEGRAPHINTERFACE_SRCS} OpenSGSceneGraphInterface.def)
#else (WIN32)
	set(ALL_SRCS ${OPENSGSCENEGRAPHINTERFACE_SRCS})
#endif (WIN32)

##############################################################################
# Define target-directories for header and library files
##############################################################################
set (TARGET_INCLUDE_DIR ${INVRS_TARGET_INCLUDE_DIR}/inVRs/OutputInterface/OpenSGSceneGraphInterface)
set (TARGET_LIB_DIR ${INVRS_TARGET_LIB_DIR})
set (TARGET_BIN_DIR ${INVRS_TARGET_BIN_DIR})

##############################################################################
# Set definition for exporting DLL information
##############################################################################
add_definitions (-DINVRSOPENSGSCENEGRAPHINTERFACE_EXPORTS)

find_package(OpenSG REQUIRED COMPONENTS OSGDrawable OSGGroup OSGUtil OSGFileIO OSGImageFileIO)
include_directories(${OpenSG_INCLUDE_DIRS})
add_definitions(${OpenSG_DEFINITIONS})

##############################################################################
# Build library
##############################################################################
add_library(inVRsOpenSGSceneGraphInterface SHARED ${ALL_SRCS})

##############################################################################
# Define dependencies when built within inVRs
##############################################################################
if (INVRS_SOURCE_DIR)
	add_dependencies (inVRsOpenSGSceneGraphInterface
		inVRsSystemCore
		irrXML)
endif (INVRS_SOURCE_DIR)

##############################################################################
# Define libraries which must be linked against
##############################################################################
target_link_libraries (inVRsOpenSGSceneGraphInterface
	inVRsSystemCore
	irrXML)


target_link_libraries(inVRsOpenSGSceneGraphInterface ${OpenSG_LIBRARIES})


##############################################################################
# Add library to required libraries (when using this package)
##############################################################################
list (APPEND OpenSGSceneGraphInterface_LIBRARIES inVRsOpenSGSceneGraphInterface)

##############################################################################
# Install library and header files
##############################################################################
install (FILES OpenSGEntitySceneGraphAttachment.h
		OpenSGEnvironmentSceneGraphAttachment.h
		OpenSGGeometrySceneGraphNode.h
		OpenSGGroupSceneGraphNode.h
		OpenSGModel.h
		OpenSGSceneGraphInterface.h
		OpenSGSceneGraphNode.h
		OpenSGTileSceneGraphAttachment.h
		OpenSGTransformationSceneGraphNode.h
	DESTINATION ${TARGET_INCLUDE_DIR})

install (TARGETS inVRsOpenSGSceneGraphInterface
	ARCHIVE DESTINATION ${TARGET_LIB_DIR}
	LIBRARY DESTINATION ${TARGET_LIB_DIR}
	RUNTIME DESTINATION ${TARGET_BIN_DIR}
)

##############################################################################
# Export all variables needed later for building
##############################################################################
set ( INVRS_EXPORT_OpenSGSceneGraphInterface_INCLUDE_DIRS ${OpenSGSceneGraphInterface_INCLUDE_DIRS} ${OpenSG_INCLUDE_DIRS})
set ( INVRS_EXPORT_OpenSGSceneGraphInterface_LIBRARY_DIRS ${OpenSGSceneGraphInterface_LIBRARY_DIRS} ${OpenSG_LIBRARY_DIRS})
set ( INVRS_EXPORT_OpenSGSceneGraphInterface_LIBRARIES ${OpenSGSceneGraphInterface_LIBRARIES} ${OpenSG_LIBRARIES})
set ( INVRS_EXPORT_OpenSGSceneGraphInterface_DEFINITIONS ${OpenSGSceneGraphInterface_DEFINITIONS})
INVRS_ADD_EXPORTS( OpenSGSceneGraphInterface )

