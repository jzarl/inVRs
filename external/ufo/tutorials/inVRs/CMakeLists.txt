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
cmake_policy(SET CMP0005 NEW)

###############################################################################
project (UFO_tutorial_inVRs)

enable_testing()

include (${UFO_tutorial_inVRs_SOURCE_DIR}/user.cmake OPTIONAL)
include (${UFO_tutorial_inVRs_SOURCE_DIR}/cmake/config.cmake)

# As long as these paths are not absolute, they are relative to 
# CMAKE_INSTALL_PREFIX:
set (UFO_tutorial_inVRs_TARGET_LIB_DIR lib)
set (UFO_tutorial_inVRs_TARGET_INCLUDE_DIR include)
set (UFO_tutorial_inVRs_TARGET_BIN_DIR bin)
set (UFO_tutorial_inVRs_TARGET_DATA_DIR share/ufo )

include_directories ( ${inVRs_INCLUDE_DIRS} )
link_directories ( ${inVRs_LIBRARY_DIRS} )
add_executable (ufo_inVRs main.cpp )
target_link_libraries (ufo_inVRs ${inVRs_LIBRARIES} 
	inVRsOpenSGSceneGraphInterface
	inVRsControllerManager
	inVRsHeightMap
	inVRsCollisionMap
	inVRsSkybox
	inVRsOpenSGApplicationBase
	inVRsAvataraWrapper
	ufo
	)
add_definitions ( ${inVRs_DEFINITIONS} )

install ( TARGETS ufo_inVRs 
	DESTINATION ${UFO_tutorial_inVRs_TARGET_BIN_DIR} )

###
# Config and Models:
###
configure_file (
	config/general_desktop_client.xml.in
	${CMAKE_CURRENT_BINARY_DIR}/config/general_desktop_client.xml
	@only )

configure_file (
	config/general_desktop_server.xml.in
	${CMAKE_CURRENT_BINARY_DIR}/config/general_desktop_server.xml
	@only )

# exclude .svn stuff and general_desktop*.xml symlinks:
install ( DIRECTORY config DESTINATION ${UFO_tutorial_inVRs_TARGET_DATA_DIR}/config
	FILES_MATCHING PATTERN '*.xml' 
	PATTERN "*/.svn*" EXCLUDE 
	PATTERN "config/general_desktop_server.xml" EXCLUDE 
	PATTERN "config/general_desktop_client.xml" EXCLUDE )
install ( FILES ${CMAKE_CURRENT_BINARY_DIR}/config/general_desktop_server.xml
	${CMAKE_CURRENT_BINARY_DIR}/config/general_desktop_client.xml
	DESTINATION ${UFO_tutorial_inVRs_TARGET_DATA_DIR}/config )
install ( DIRECTORY models DESTINATION ${UFO_tutorial_inVRs_TARGET_DATA_DIR}/models
	PATTERN "*/.svn*" EXCLUDE )


if ( inVRs_3DPhysics_FOUND )
	include_directories ( ${inVRs_INCLUDE_DIRS} ${inVRs_Interaction_INCLUDE_DIRS} ${inVRs_3DPhysics_INCLUDE_DIRS} )
	link_directories ( ${inVRs_LIBRARY_DIRS} ${inVRs_Interaction_LIBRARY_DIRS} ${inVRs_3DPhysics_LIBRARY_DIRS} )
	add_executable (ufo_inVRs_3DPhysics main-3DPhysics.cpp )
	target_link_libraries (ufo_inVRs_3DPhysics ${inVRs_LIBRARIES} 
		inVRsOpenSGSceneGraphInterface
		inVRsControllerManager
		inVRsNavigation
		inVRsHeightMap
		inVRsCollisionMap
		inVRsSkybox
		inVRsOpenSGApplicationBase
		inVRsAvataraWrapper
		${inVRs_Interaction_LIBRARIES}
		${inVRs_3DPhysics_LIBRARIES}
		avatara
		ufo
		)
	add_definitions ( ${inVRs_DEFINITIONS} ${inVRs_Interaction_DEFINITIONS} ${inVRs_3DPhysics_DEFINITIONS} )

	install ( TARGETS ufo_inVRs_3DPhysics 
		DESTINATION ${UFO_tutorial_inVRs_TARGET_BIN_DIR} )
	###
	# Config:
	###
	configure_file (
		config-3DPhysics/general_desktop_client.xml.in
		${CMAKE_CURRENT_BINARY_DIR}/config-3DPhysics/general_desktop_client.xml
		@only )

	configure_file (
		config-3DPhysics/general_desktop_server.xml.in
		${CMAKE_CURRENT_BINARY_DIR}/config-3DPhysics/general_desktop_server.xml
		@only )
	# exclude .svn stuff and general_desktop*.xml symlinks:
	install ( DIRECTORY config-3DPhysics DESTINATION ${UFO_tutorial_inVRs_TARGET_DATA_DIR}/config-3DPhysics
		FILES_MATCHING PATTERN '*.xml' 
		PATTERN "*/.svn*" EXCLUDE 
		PATTERN "config-3DPhysics/general_desktop_server.xml" EXCLUDE 
		PATTERN "config-3DPhysics/general_desktop_client.xml" EXCLUDE )
	install ( FILES ${CMAKE_CURRENT_BINARY_DIR}/config-3DPhysics/general_desktop_server.xml
		${CMAKE_CURRENT_BINARY_DIR}/config-3DPhysics/general_desktop_client.xml
		DESTINATION ${UFO_tutorial_inVRs_TARGET_DATA_DIR}/config-3DPhysics )

endif ( inVRs_3DPhysics_FOUND )

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake"
	IMMEDIATE @ONLY)

add_custom_target ( uninstall
	"${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake")

