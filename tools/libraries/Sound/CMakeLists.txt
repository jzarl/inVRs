aux_source_directory(. SOUND_SRCS)

set(CMAKE_MODULE_PATH $CMAKE_MODULE_PATH ${INVRS_SOURCE_DIR}/cmake)
find_package(OpenAL)
include_directories (${OPENAL_INCLUDE_DIRS})

set (TARGET_INCLUDE_DIR ${INVRS_TARGET_INCLUDE_DIR}/inVRs/tools/libraries/Sound)
set (TARGET_LIB_DIR ${INVRS_TARGET_LIB_DIR})
set (TARGET_BIN_DIR ${INVRS_TARGET_BIN_DIR})

set(ALL_SRCS ${SOUND_SRCS})

add_definitions (-DINVRSSOUND_EXPORTS)
add_library(inVRsSound SHARED ${ALL_SRCS})
add_dependencies (inVRsSound inVRsSystemCore irrXML)
add_definitions (-DUSE_ALCCHAR)
target_link_libraries (inVRsSound inVRsSystemCore irrXML ${OPENAL_LIBRARIES})

install (FILES Sound.h
		SoundLoader.h
	DESTINATION ${TARGET_INCLUDE_DIR})

install (TARGETS inVRsSound
	ARCHIVE DESTINATION ${TARGET_LIB_DIR}
	LIBRARY DESTINATION ${TARGET_LIB_DIR}
	RUNTIME DESTINATION ${TARGET_BIN_DIR}
)

##############################################################################
# Export all variables needed later for building
##############################################################################
set ( INVRS_EXPORT_Sound_INCLUDE_DIRS ${Sound_INCLUDE_DIRS} ${OPENAL_INCLUDE_DIRS})
set ( INVRS_EXPORT_Sound_LIBRARY_DIRS ${Sound_LIBRARY_DIRS})
set ( INVRS_EXPORT_Sound_LIBRARIES ${Sound_LIBRARIES} ${OPENAL_LIBRARIES})
set ( INVRS_EXPORT_Sound_DEFINITIONS ${Sound_DEFINITIONS} -DUSE_ALCCHAR)
INVRS_ADD_EXPORTS( Sound )
