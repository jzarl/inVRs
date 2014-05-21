###############################################################################
# Only execute commands if file was not included already
###############################################################################
if (NOT OPENSGAPPLICATIONBASE_CONFIG_CMAKE_LOADED)

###############################################################################
# Check if CAVESceneManager-support is desired
###############################################################################
if (INVRS_ENABLE_EXTERNAL_CAVESCENEMANAGER)

	#######################################################################
	# Ensure that CAVESceneManager sources are available
	#######################################################################
	find_file(CAVESCENEMANAGER_CHECK_HEADER OSGCSM/OSGCAVESceneManager.h
		${INVRS_SOURCE_DIR}/external/CAVESceneManager-1.0/include)
	# if not found, search again with old namespace:
	find_file(CAVESCENEMANAGER_CHECK_HEADER OpenSG/OSGCAVESceneManager.h
		${INVRS_SOURCE_DIR}/external/CAVESceneManager-1.0/include)

	mark_as_advanced(CAVESCENEMANAGER_CHECK_HEADER)
	mark_as_advanced (FORCE CAVESceneManager_ROOT_DIR)

	# TODO: make error message more clear, this should never happen!!!
	if (NOT CAVESCENEMANAGER_CHECK_HEADER)
		message(FATAL_ERROR 
				"ERROR: OpenSGApplicationBase can not be built without CAVESceneManager library. "
				"Please download CAVESceneManager from http://www.inVRs.org "
				"and unpack it in the <inVRs>/external folder!")
	endif (NOT CAVESCENEMANAGER_CHECK_HEADER)

	#######################################################################
	# Add CAVESceneManager include directory and set the required libraries
	#######################################################################
	include_directories(${INVRS_SOURCE_DIR}/external/CAVESceneManager-1.0/include
		${INVRS_SOURCE_DIR}/external/CAVESceneManager-1.0/include/OpenSG)

	set (CAVESceneManager_LIBRARIES CAVESceneManager)

else (INVRS_ENABLE_EXTERNAL_CAVESCENEMANAGER)

	#######################################################################
	# Disable the CAVESceneManager support during building
	#######################################################################
	add_definitions (-DDISABLE_CAVESCENEMANAGER_SUPPORT)

	mark_as_advanced (FORCE CAVESceneManager_ROOT_DIR)

endif (INVRS_ENABLE_EXTERNAL_CAVESCENEMANAGER)

###############################################################################
# Finish
###############################################################################
set (OPENSGAPPLICATIONBASE_CONFIG_CMAKE_LOADED ON)
endif (NOT OPENSGAPPLICATIONBASE_CONFIG_CMAKE_LOADED)

