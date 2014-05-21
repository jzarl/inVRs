cmake_minimum_required (VERSION 2.6)

###############################################################################
# Set policy
###############################################################################
if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

###############################################################################
# Set project name
###############################################################################
project (FFD_INVRSSAMPLE)

include (user.cmake OPTIONAL)

set (inVRs_REQUIRED_MODULES Navigation Deformation Network Interaction OpenSGSceneGraphInterface)
set (inVRs_REQUIRED_TOOLS )
include (${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.cmake)

#collect data
set(FFD_INVRSSAMPLE_NAME inVRsSample)
aux_source_directory(src FFD_INVRSSAMPLE_SRCS)

include_directories(${inVRs_INCLUDE_DIRS})
link_directories(${inVRs_LIBRARY_DIRS})
add_definitions(${inVRs_DEFINITIONS})

#define executable
add_executable (${FFD_INVRSSAMPLE_NAME} ${FFD_INVRSSAMPLE_SRCS})

target_link_libraries (${FFD_INVRSSAMPLE_NAME} ${inVRs_LIBRARIES})


#configure config files
configure_file(config/general.in.xml ${CMAKE_SOURCE_DIR}/config/general.xml @ONLY)
configure_file(config/generalClient.in.xml ${CMAKE_SOURCE_DIR}/config/generalClient.xml @ONLY)

