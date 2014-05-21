# The name of our project is "UFO". CMakeLists files in this project can
# refer to the root source directory of the project as ${UFO_SOURCE_DIR} and
# to the root binary directory of the project as ${UFO_BINARY_DIR}.
###############################################################################
# Set Policies:
###############################################################################
cmake_minimum_required (VERSION 2.4)
# add_target_library(<target> /path/to/libfoo.so bar)
# creates linker-flags: /path/to/libfoo.so -lbar
# NOT: -L/path/to -lfoo -lbar
cmake_policy(SET CMP0003 NEW)

###############################################################################
project (UFO_tutorial_basicglut)

enable_testing()

include (${UFO_tutorial_basicglut_SOURCE_DIR}/user.cmake OPTIONAL)
include (${UFO_tutorial_basicglut_SOURCE_DIR}/cmake/config.cmake)

# As long as these paths are not absolute, they are relative to 
# CMAKE_INSTALL_PREFIX:
set (UFO_tutorial_basicglut_TARGET_LIB_DIR lib)
set (UFO_tutorial_basicglut_TARGET_INCLUDE_DIR include)
set (UFO_tutorial_basicglut_TARGET_BIN_DIR bin)

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake"
	IMMEDIATE @ONLY)

add_custom_target ( uninstall
	"${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake" )

###############################################################################
# Target definitions:
###############################################################################
add_executable ( basicglut main.cpp SimpleObject.cpp )
target_link_libraries ( basicglut ufo glut GLU)

add_custom_target ( run 
	COMMAND basicglut 
	WORKING_DIRECTORY ${UFO_tutorial_basicglut_SOURCE_DIR}
	COMMENT "Running basicglut tutorial in directory ${UFO_tutorial_basicglut_SOURCE_DIR}"
	DEPENDS basicglut VERBATIM )
