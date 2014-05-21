###############################################################################
# Only execute commands if file was not included already
###############################################################################
if (NOT CMAKE_MODULE_CONFIG_CMAKE_LOADED)


###############################################################################
# Add local module path to CMAKE_MODULE_PATH
###############################################################################
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")


###############################################################################
# include user-defined configuration
###############################################################################
include (${CMAKE_CURRENT_SOURCE_DIR}/user.cmake OPTIONAL)


###############################################################################
# find inVRs itself:
###############################################################################
find_package ( inVRs REQUIRED )
include_directories ( ${inVRs_INCLUDE_DIRS} )
link_directories ( ${inVRs_LIBRARY_DIRS} )
add_definitions ( ${inVRs_DEFINITIONS} )


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


###############################################################################
# include platform-dependent compiler flags
###############################################################################
include (${CMAKE_CURRENT_SOURCE_DIR}/cmake/config_buildFlags.cmake)


###############################################################################
# add uninstall target
###############################################################################
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

add_custom_target(uninstall
        "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake")


###############################################################################
# macro for adding module flags in config_inVRs.cmake file
###############################################################################
macro(INVRS_ADD_EXPORTS _module )
	# define inVRs config file
	set (INVRS_EXPORTS_FILE "${inVRs_INCLUDE_DIR}/inVRs/config_inVRs.cmake")

	# check for inVRs config file
	if (NOT EXISTS ${INVRS_EXPORTS_FILE})
		message (FATAL_ERROR "Cannot find inVRs configuration file: \"${INVRS_EXPORTS_FILE}\"")
	endif (NOT EXISTS ${INVRS_EXPORTS_FILE})

	# read inVRs config file
	file (READ "${INVRS_EXPORTS_FILE}" originalFile)
	# create a backup of the original file
	file (WRITE "${INVRS_EXPORTS_FILE}.backup" "${originalFile}")

	# remove comment for current module
	string (REGEX REPLACE "[\n][#] definitions from ${CMAKE_CURRENT_SOURCE_DIR}:[\n]" "" updatedFile "${originalFile}")
	# remove all exported variables for current module
	string (REGEX REPLACE "[^\n]*INVRS_EXPORT_${_module}[^\n]*[\n]" "" updatedFile "${updatedFile}")

	# replace original file with updated one (without the exports of the current module)
	file (WRITE "${INVRS_EXPORTS_FILE}" "${updatedFile}")

	# append exports from module (similar to macro in inVRs CMakeLists.txt)
	file ( APPEND ${INVRS_EXPORTS_FILE} "\n# definitions from ${CMAKE_CURRENT_SOURCE_DIR}:\n" )
	foreach ( var INCLUDE_DIRS LIBRARY_DIRS DEFINITIONS LIBRARIES )
		file ( APPEND ${INVRS_EXPORTS_FILE} "set (INVRS_EXPORT_${_module}_${var} \"${INVRS_EXPORT_${_module}_${var}}\" )\n" )
	endforeach( var INCLUDE_DIRS LIBRARY_DIRS DEFINITIONS LIBRARIES )
endmacro(INVRS_ADD_EXPORTS _module )


###############################################################################
# Finish
###############################################################################
set (CMAKE_MODULE_CONFIG_CMAKE_LOADED ON)
endif (NOT CMAKE_MODULE_CONFIG_CMAKE_LOADED)
