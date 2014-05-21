###############################################################################
# Only execute commands if file was not included already
###############################################################################
if (NOT AVATARAWRAPPER_CONFIG_CMAKE_LOADED)

###############################################################################
# Check if Avatara will be built or if it must be found externally
###############################################################################
if (NOT INVRS_ENABLE_EXTERNAL_AVATARA)

	#######################################################################
	# Add local module path to CMAKE_MODULE_PATH
	#######################################################################
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

	#######################################################################
	# find avatara itself:
	#######################################################################
	find_package ( avatara REQUIRED )
	include_directories ( ${avatara_INCLUDE_DIRS} )
	link_directories ( ${avatara_LIBRARY_DIRS} )
	add_definitions ( ${avatara_DEFINITIONS} )

	###############################################################
	# Add avatara variables to the AvataraWrapper export values
	###############################################################
	list (APPEND AvataraWrapper_INCLUDE_DIRS ${avatara_INCLUDE_DIRS})
	list (APPEND AvataraWrapper_LIBRARY_DIRS ${avatara_LIBRARY_DIRS})

else (NOT INVRS_ENABLE_EXTERNAL_AVATARA)

	#######################################################################
	# Ensure that avatara sources are available
	#######################################################################
	find_file(AVATARA_CHECK_HEADER avatara/OSGAvatar.h
		${INVRS_SOURCE_DIR}/external/avatara-1.0/include)

	mark_as_advanced(AVATARA_CHECK_HEADER)
	mark_as_advanced (FORCE avatara_ROOT_DIR)

	if (NOT AVATARA_CHECK_HEADER)
		message(FATAL_ERROR "ERROR: AvataraWrapper can not be built
			without avatara library. Please download avatara from
			http://www.inVRs.org and unpack it in the
			<inVRs>/external folder!")
	endif (NOT AVATARA_CHECK_HEADER)

	#######################################################################
	# Add avatara include directory and set the required avatara libraries
	#######################################################################
	include_directories(${INVRS_SOURCE_DIR}/external/avatara-1.0/include
		${INVRS_SOURCE_DIR}/external/avatara-1.0/include/avatara)

	set (avatara_LIBRARIES avatara)

	set ( avatara_FOUND TRUE )

endif (NOT INVRS_ENABLE_EXTERNAL_AVATARA)

###############################################################################
# Finish
###############################################################################
set (AVATARAWRAPPER_CONFIG_CMAKE_LOADED ON)
endif (NOT AVATARAWRAPPER_CONFIG_CMAKE_LOADED)

