###
# Check for deprecated build-options and emit warnings.
###
if(DEFINED BUILD_EXTERNAL_AVATARA OR DEFINED INVRS_ENABLE_AVATAR_WRAPPER)
	message(WARNING "The options BUILD_EXTERNAL_AVATARA and INVRS_ENABLE_AVATAR_WRAPPER have been deprecated!")
	message(STATUS "It is now sufficient to enable INVRS_ENABLE_EXTERNAL_AVATARA for avatara support.")
	message(STATUS "Unsetting (and ignoring) the deprecated options...")
	unset(BUILD_EXTERNAL_AVATARA CACHE)
	unset(INVRS_ENABLE_AVATAR_WRAPPER CACHE)
endif()

if(DEFINED INVRS_ENABLE_CAVESCENEMANAGER_SUPPORT)
	message(WARNING "The option INVRS_ENABLE_CAVESCENEMANAGER_SUPPORT has been deprecated!")
	message(STATUS "It is now sufficient to enable INVRS_ENABLE_EXTERNAL_CAVESCENEMANAGER to use CAVESceneManager.")
	message(STATUS "Unsetting (and ignoring) the deprecated option...")
	unset(INVRS_ENABLE_CAVESCENEMANAGER_SUPPORT CACHE)
endif()
