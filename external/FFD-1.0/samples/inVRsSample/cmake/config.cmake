###############################################################################
# Only execute commands if file was not included already
###############################################################################
if (NOT FFD_INVRSSAMPLE_CONFIG_CMAKE_LOADED)

###############################################################################
# Add local module path to CMAKE_MODULE_PATH
###############################################################################
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/" ${CMAKE_MODULE_PATH})


###############################################################################
# find inVRs itself:
###############################################################################
find_package ( inVRs REQUIRED ${inVRs_REQUIRED_MODULES} ${inVRs_REQUIRED_TOOLS})

###############################################################################
# add FRAMEWORK linker flags for MAC OSX systems
###############################################################################
foreach (framework ${inVRs_FRAMEWORKS} )
	set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -framework ${framework}")
	set (CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} -framework ${framework}")
	set (CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -framework ${framework}")
	set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework ${framework}")
	set (CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -framework ${framework}")
	set (CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -framework ${framework}")
endforeach (framework ${inVRs_FRAMEWORKS} )

set (FFD_INVRSSAMPLE_CONFIG_CMAKE_LOADED ON)
endif (NOT FFD_INVRSSAMPLE_CONFIG_CMAKE_LOADED)
