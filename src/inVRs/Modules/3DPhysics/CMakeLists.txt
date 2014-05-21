#######################################################################
# Set policies
#######################################################################
# included files don't affect outside policies:
cmake_policy(SET CMP0011 NEW)

find_package(OpenSG REQUIRED OSGBase)
include_directories(${OpenSG_INCLUDE_DIRS})
add_definitions(${OpenSG_DEFINITIONS})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/oops)

aux_source_directory (. 3DPHYSICS_SRCS)

# ODE is used by oops header files.
# TODO: this should be done automatically when linking against oops targets
find_package(ODE)
include_directories (${ODE_INCLUDE_DIR})
include_directories ( "${CMAKE_CURRENT_BINARY_DIR}/include" )

# generated header files for oops:
include_directories (${CMAKE_CURRENT_BINARY_DIR}/oops/include)
# static header files for oops:
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/oops/include)

set (TARGET_INCLUDE_DIR ${INVRS_TARGET_INCLUDE_DIR}/inVRs/Modules/3DPhysics)
set (TARGET_LIB_DIR ${INVRS_TARGET_LIB_DIR})
set (TARGET_BIN_DIR ${INVRS_TARGET_BIN_DIR})

set(ALL_SRCS ${3DPHYSICS_SRCS})

add_definitions (-DINVRS3DPHYSICS_EXPORTS)

add_library(inVRs3DPhysics SHARED ${ALL_SRCS})
target_link_libraries (inVRs3DPhysics ${OpenSG_LIBRARIES} inVRsSystemCore inVRsInteraction oops irrXML)

install(
	FILES
		3DPhysicsSharedLibraryExports.h
		AccelerationSynchronisationModel.h
		AccelerationSynchronisationModelFactory.h
		ArticulatedPhysicsEntity.h
		ArticulatedPhysicsEntityController.h
		ArticulatedPhysicsEntityType.h
		ArticulatedPhysicsEntityTypeFactory.h
		ChangeSynchronisationModelEvent.h
		ChangedSynchronisationModel.h
		ChangedSynchronisationModelFactory.h
		ChangedUserDataEvent.h
		CreateCursorSpringConnectorEvent.h
		CreateLinkedPhysicsEntityEvent.h
		CursorSpringConnector.h
		DeleteCursorSpringConnectorEvent.h
		FullSynchronisationModel.h
		FullSynchronisationModelFactory.h
		LinkedPhysicsEntity.h
		LinkedPhysicsEntityType.h
		MessageSizeCounter.h
		OopsObjectID.h
		OopsPhysicsEntityTransformationWriter.h
		Physics.h
		PhysicsEntity.h
		PhysicsEntityTransformationWriterModifier.h
		PhysicsFullSynchronisationModel.h
		PhysicsFullSynchronisationModelFactory.h
		PhysicsHomerManipulationActionModel.h
		PhysicsInputSynchronisationModel.h
		PhysicsInputSynchronisationModelFactory.h
		PhysicsMessageFunctions.h
		PhysicsObjectID.h
		PhysicsObjectInterface.h
		PhysicsObjectManager.h
		PhysicsObjectManagerFactory.h
		PhysicsSpringManipulationActionEvents.h
		PhysicsSpringManipulationActionModel.h
		ProfilingHelper.h
		SimplePhysicsEntity.h
		SimplePhysicsEntityController.h
		SimplePhysicsEntityType.h
		SimplePhysicsEntityTypeFactory.h
		SimulationStepListenerInterface.h
		SingleServerPhysicsObjectManager.h
		SingleServerPhysicsObjectManagerFactory.h
		StartSimulationEvent.h
		SynchronisationMacros.h
		SynchronisationModel.h
		SynchronisationModelFactory.h
		SynchronisePhysicsEvent.h
		SystemThreadListenerInterface.h
		UserData.h
#		VehicleEntity.h
#		VehicleEntityType.h
#		VehicleEntityTypeFactory.h
		VelocitySynchronisationModel.h
		VelocitySynchronisationModelFactory.h
	DESTINATION
		${TARGET_INCLUDE_DIR}
)

install (TARGETS inVRs3DPhysics
	ARCHIVE DESTINATION ${TARGET_LIB_DIR}
	LIBRARY DESTINATION ${TARGET_LIB_DIR}
	RUNTIME DESTINATION ${TARGET_BIN_DIR}
)


# additional exports (compared to SystemCore):
set ( INVRS_EXPORT_3DPhysics_INCLUDE_DIRS ${ODE_INCLUDE_DIR} ${OpenSG_INCLUDE_DIRS})
set ( INVRS_EXPORT_3DPhysics_LIBRARY_DIRS )
set ( INVRS_EXPORT_3DPhysics_LIBRARIES inVRs3DPhysics oops ${ODE_LIBRARIES} )
set ( INVRS_EXPORT_3DPhysics_DEFINITIONS ${OOPS_DEFINITIONS} )
INVRS_ADD_EXPORTS( 3DPhysics)
