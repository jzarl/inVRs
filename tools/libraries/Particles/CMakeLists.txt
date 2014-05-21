set (TARGET_INCLUDE_DIR ${INVRS_TARGET_INCLUDE_DIR}/inVRs/tools/libraries/Particles)
set (TARGET_LIB_DIR ${INVRS_TARGET_LIB_DIR})
set (TARGET_BIN_DIR ${INVRS_TARGET_BIN_DIR})

set(ALL_SRCS ParticleEmitter.cpp  ParticleSystem.cpp  ParticleSystemConfig.cpp  ParticleType.cpp  Particles.cpp)

find_package(OpenSG REQUIRED COMPONENTS OSGBase )
include_directories(${OpenSG_INCLUDE_DIRS})
add_definitions(${OpenSG_DEFINITIONS})

find_package(PNG REQUIRED)
add_definitions(${PNG_DEFINITIONS})
include_directories(${PNG_INCLUDE_DIR})

add_definitions (-DINVRSPARTICLES_EXPORTS)
add_library(inVRsParticles SHARED ${ALL_SRCS})
target_link_libraries (inVRsParticles inVRsSystemCore irrXML ${PNG_LIBRARIES})

install (
	FILES 
		ParticleEmitter.h
		ParticleSystem.h
		ParticleSystemConfig.h
		ParticleSystemParticles.h
		ParticleType.h
		Particles.h
		TimeLine.h
	DESTINATION ${TARGET_INCLUDE_DIR})

install (TARGETS inVRsParticles DESTINATION
	ARCHIVE DESTINATION ${TARGET_LIB_DIR}
	LIBRARY DESTINATION ${TARGET_LIB_DIR}
	RUNTIME DESTINATION ${TARGET_BIN_DIR}
)

##############################################################################
# Export all variables needed later for building
##############################################################################
set ( INVRS_EXPORT_Particles_INCLUDE_DIRS ${Particles_INCLUDE_DIRS} ${OpenSG_INCLUDE_DIRS})
set ( INVRS_EXPORT_Particles_LIBRARY_DIRS ${Particles_LIBRARY_DIRS})
set ( INVRS_EXPORT_Particles_LIBRARIES ${Particles_LIBRARIES})
set ( INVRS_EXPORT_Particles_DEFINITIONS ${Particles_DEFINITIONS} ${OpenSG_DEFINITIONS})
INVRS_ADD_EXPORTS( Particles )
