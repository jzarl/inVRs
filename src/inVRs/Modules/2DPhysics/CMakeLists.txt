aux_source_directory(. 2DPHYSICS_SRCS)

set (TARGET_INCLUDE_DIR ${INVRS_TARGET_INCLUDE_DIR}/inVRs/Modules/2DPhysics)
set (TARGET_LIB_DIR ${INVRS_TARGET_LIB_DIR})
set (TARGET_BIN_DIR ${INVRS_TARGET_BIN_DIR})

set(ALL_SRCS ${2DPHYSICS_SRCS})

find_package(OpenSG REQUIRED OSGBase)
include_directories(${OpenSG_INCLUDE_DIRS})
add_definitions(${OpenSG_DEFINITIONS})

add_definitions (-DINVRS2DPHYSICS_EXPORTS)

add_library(inVRs2DPhysics SHARED ${ALL_SRCS})
add_dependencies (inVRs2DPhysics inVRsSystemCore inVRsCollisionMapBase inVRsHeightMap irrXML)
target_link_libraries (inVRs2DPhysics inVRsSystemCore inVRsCollisionMapBase inVRsHeightMap inVRsHeightMapBase irrXML)



install(
	FILES
		2DPhysicsEvents.h
		2DPhysicsSharedLibraryExports.h
		ApplyAngularVelocityModifier.h
		ApplyForceModifier.h
		Physics2DEntity.h
		Physics2DEntityType.h
		RigidBody.h
		RunAtPhysicsServerModifier.h
		Simulation2D.h
	DESTINATION
		${TARGET_INCLUDE_DIR}
)

install(
	TARGETS inVRs2DPhysics
	ARCHIVE DESTINATION ${TARGET_LIB_DIR}
	LIBRARY DESTINATION ${TARGET_LIB_DIR}
	RUNTIME DESTINATION ${TARGET_BIN_DIR}
)

# additional exports (compared to SystemCore):
set(INVRS_EXPORT_2DPhysics_INCLUDE_DIRS ${OpenSG_INCLUDE_DIRS})
set(INVRS_EXPORT_2DPhysics_LIBRARY_DIRS )
set(INVRS_EXPORT_2DPhysics_LIBRARIES inVRs2DPhysics inVRsCollisionMapBase inVRsHeightMap)
set(INVRS_EXPORT_2DPhysics_DEFINITIONS )
INVRS_ADD_EXPORTS( 2DPhysics)

