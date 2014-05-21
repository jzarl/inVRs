find_package(OpenSG REQUIRED OSGBase)
include_directories(${OpenSG_INCLUDE_DIRS})
add_definitions(${OpenSG_DEFINITIONS})

find_package(ODE)
include_directories (${ODE_INCLUDE_DIR})

aux_source_directory(. JOINTINTERACTION_SRCS)
aux_source_directory(Events EVENTS_SRCS)

set (TARGET_INCLUDE_DIR ${INVRS_TARGET_INCLUDE_DIR}/inVRs/Modules/JointInteraction)
set (TARGET_LIB_DIR ${INVRS_TARGET_LIB_DIR})
set (TARGET_BIN_DIR ${INVRS_TARGET_BIN_DIR})

#if (WIN32)
#set (ALL_SRCS ${JOINTINTERACTION_SRCS} ${EVENTS_SRCS} JointInteraction.def)
#else (WIN32)
set(ALL_SRCS ${JOINTINTERACTION_SRCS} ${EVENTS_SRCS})
#endif (WIN32)

add_definitions (-DINVRSJOINTINTERACTION_EXPORTS)

add_library(inVRsJointInteraction SHARED ${ALL_SRCS})
add_dependencies (inVRsJointInteraction inVRsSystemCore inVRsInteraction irrXML)

target_link_libraries (inVRsJointInteraction ${OpenSG_LIBRARIES} inVRsSystemCore inVRsInteraction irrXML ${ODE_LIBRARIES})

install(
	FILES
		Constraint.h
		JointInteraction.h
		JointInteractionManipulationModel.h
		JointInteractionManipulationModelFactory.h
		JointInteractionMath.h
		JointInteractionSharedLibraryExports.h
		Joints.h
		Events/JointInteractionEvent.h
	DESTINATION
		${TARGET_INCLUDE_DIR}
)

install (TARGETS inVRsJointInteraction
	ARCHIVE DESTINATION ${TARGET_LIB_DIR}
	LIBRARY DESTINATION ${TARGET_LIB_DIR}
	RUNTIME DESTINATION ${TARGET_BIN_DIR}
)

# additional exports (compared to SystemCore):
set ( INVRS_EXPORT_JointInteraction_INCLUDE_DIRS ${ODE_INCLUDE_DIR} ${OpenSG_INCLUDE_DIRS})
set ( INVRS_EXPORT_JointInteraction_LIBRARY_DIRS )
set ( INVRS_EXPORT_JointInteraction_LIBRARIES inVRsJointInteraction )
set ( INVRS_EXPORT_JointInteraction_DEFINITIONS )
INVRS_ADD_EXPORTS( JointInteraction )

