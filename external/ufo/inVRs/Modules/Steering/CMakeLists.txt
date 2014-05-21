include_directories ( ${inVRs_INCLUDE_DIRS} )
link_directories ( ${inVRs_LIBRARY_DIRS} )
add_definitions ( ${inVRs_DEFINITIONS} )

add_library ( inVRsSteering SHARED Steering.cpp UfoXmlConfigurationReader.cpp)
target_link_libraries ( inVRsSteering ufo )

if ( INVRS_SOURCE_DIR )
	target_link_libraries ( inVRsSteering ${inVRs_LIBRARIES} )
	install ( TARGETS inVRsSteering DESTINATION ${INVRS_TARGET_LIB_DIR} )

	# additional exports (for inVRS):
	set ( INVRS_EXPORT_Steering_INCLUDE_DIRS )
	set ( INVRS_EXPORT_Steering_LIBRARY_DIRS )
	set ( INVRS_EXPORT_Steering_LIBRARIES inVRsSteering ufo)
	set ( INVRS_EXPORT_Steering_DEFINITIONS )
	INVRS_ADD_EXPORTS(Steering)
else ( INVRS_SOURCE_DIR )
	target_link_libraries ( inVRsSteering ${inVRs_LIBRARIES} )
	install ( TARGETS inVRsSteering DESTINATION ${inVRs_LIBRARY_DIR} )
endif ( INVRS_SOURCE_DIR )

install ( FILES Steering.h DESTINATION ${UFO_TARGET_INCLUDE_DIR}/inVRs/Modules/Steering/ )

