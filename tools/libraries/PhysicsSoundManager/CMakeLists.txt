include (${INVRS_SOURCE_DIR}/tools/libraries/oops/cmake/config.cmake)
include (${INVRS_SOURCE_DIR}/cmake/findOpenAL.cmake)
include_directories (${OPENAL_INCLUDE_DIRS})

aux_source_directory(. PHYSICSSOUNDMANAGER_SRCS)

set (TARGET_INCLUDE_DIR ${INVRS_TARGET_INCLUDE_DIR}/inVRs/tools/libraries/PhysicsSoundManager)
set (TARGET_LIB_DIR ${INVRS_TARGET_LIB_DIR})
set (TARGET_BIN_DIR ${INVRS_TARGET_BIN_DIR})

set(ALL_SRCS ${PHYSICSSOUNDMANAGER_SRCS})

include_directories (${INVRS_SOURCE_DIR}/tools/libraries/oops/include)

add_definitions (-DINVRSPHYSICSSOUNDMANAGER_EXPORTS)
add_library(inVRsPhysicsSoundManager SHARED ${ALL_SRCS})
add_dependencies (inVRsPhysicsSoundManager inVRs3DPhysics inVRsSound irrXML)
if (WIN32)
target_link_libraries(inVRsPhysicsSoundManager 
	debug OSGBaseD.lib 
	debug OSGSystemD.lib 
	debug OSGWindowGLUTD.lib 
	optimized OSGBase.lib 
	optimized OSGSystem.lib 
	optimized OSGWindowGLUT.lib)
endif (WIN32)
target_link_libraries(inVRsPhysicsSoundManager inVRs3DPhysics inVRsSound
	irrXML)

install (FILES PhysicsSoundManager.h
	DESTINATION ${TARGET_INCLUDE_DIR})

install (TARGETS inVRsPhysicsSoundManager DESTINATION
	ARCHIVE DESTINATION ${TARGET_LIB_DIR}
	LIBRARY DESTINATION ${TARGET_LIB_DIR}
	RUNTIME DESTINATION ${TARGET_BIN_DIR}
)
