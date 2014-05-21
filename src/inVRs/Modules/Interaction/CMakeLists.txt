##############################################################################
# Define sourcefiles
##############################################################################
aux_source_directory(. INTERACTION_SRCS)
#if (WIN32)
#	set (ALL_SRCS ${INTERACTION_SRCS} Interaction.def)
#else (WIN32)
	set(ALL_SRCS ${INTERACTION_SRCS})
#endif (WIN32)

##############################################################################
# Define target-directories for header and library files
##############################################################################
set (TARGET_INCLUDE_DIR ${INVRS_TARGET_INCLUDE_DIR}/inVRs/Modules/Interaction)
set (TARGET_LIB_DIR ${INVRS_TARGET_LIB_DIR})
set (TARGET_BIN_DIR ${INVRS_TARGET_BIN_DIR})

##############################################################################
# Set definition for exporting DLL information
##############################################################################
add_definitions (-DINVRSINTERACTION_EXPORTS)

find_package(OpenSG REQUIRED OSGBase)
include_directories(${OpenSG_INCLUDE_DIRS})
add_definitions(${OpenSG_DEFINITIONS})

##############################################################################
# Build library
##############################################################################
add_library(inVRsInteraction SHARED ${ALL_SRCS})

##############################################################################
# Define dependencies when built within inVRs
##############################################################################
if (INVRS_SOURCE_DIR)
	add_dependencies (inVRsInteraction
		inVRsSystemCore
		irrXML)
endif (INVRS_SOURCE_DIR)

##############################################################################
# Define libraries which must be linked against
##############################################################################
target_link_libraries (inVRsInteraction
	inVRsSystemCore
	irrXML)



##############################################################################
# Add library to required libraries (when using this package)
##############################################################################
list (APPEND Interaction_LIBRARIES inVRsInteraction)

##############################################################################
# Install library and header files
##############################################################################
install (FILES ButtonPressManipulationChangeModel.h
		HighlightSelectionActionModel.h
		HomerManipulationActionModel.h
		HomerCursorModel.h
		Interaction.h
		InteractionEvents.h
		InteractionSharedLibraryExports.h
		LimitRayCastSelectionChangeModel.h
		ManipulationChangeModel.h
		ManipulationOffsetModifier.h
		RayCastSelectionChangeModel.h
		SelectionChangeModel.h
		SharedManipulationMerger.h
		StateActionModel.h
		StateTransitionModel.h
		VirtualHandManipulationActionModel.h
		VirtualHandSelectionChangeModel.h
	DESTINATION ${TARGET_INCLUDE_DIR})

install (TARGETS inVRsInteraction
	ARCHIVE DESTINATION ${TARGET_LIB_DIR}
	LIBRARY DESTINATION ${TARGET_LIB_DIR}
	RUNTIME DESTINATION ${TARGET_BIN_DIR}
)

##############################################################################
# Export all variables needed later for building
##############################################################################
set ( INVRS_EXPORT_Interaction_INCLUDE_DIRS ${Interaction_INCLUDE_DIRS})
set ( INVRS_EXPORT_Interaction_LIBRARY_DIRS ${Interaction_LIBRARY_DIRS})
set ( INVRS_EXPORT_Interaction_LIBRARIES ${Interaction_LIBRARIES})
set ( INVRS_EXPORT_Interaction_DEFINITIONS ${Interaction_DEFINITIONS})
INVRS_ADD_EXPORTS( Interaction )

