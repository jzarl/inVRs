# define plugin-files
set ( UFOPLUGINS
	AlignmentBehaviour
	AverageBehaviour
	CachingBehaviour
	CohesionBehaviour
	DeltaMinBehaviour
	FixedVelocityBehaviour
	FollowPilotBehaviour
	FollowWaypointsBehaviour
	InverseBehaviour
	RandomBehaviour
	SampleBehaviour
	SampleSteerable
	ScaleBehaviour
	SeparationBehaviour
	StrongestBehaviour
	FaceForwardBehaviour
	)

# add summary-target for all plugins:
add_custom_target( ufoplugins DEPENDS ${UFOPLUGINS} )

################################################################################
# Create plugin rules:
################################################################################
foreach (plugin ${UFOPLUGINS})
	# add plugins as MODULE (shared library meant to be included using dlopen or similiar)
	add_library ( ${plugin} MODULE ${plugin}.cpp )
	# make plugin dependant on libufo target
	target_link_libraries ( ${plugin} ufo )
	# determine type from "MAKEPLUGIN" macro:
	# -> 1. extrace MAKEPLUGIN line from source file:
	file(STRINGS "${plugin}.cpp" type
		LIMIT_COUNT 1
		REGEX "MAKEPLUGIN\(.*\)")
	# -> 2. the second parameter is the plugin type in "normal" capitalization:
	string(REGEX REPLACE "MAKEPLUGIN\\([^,]+,([^,]+),.*" "\\1" type "${type}")
	# -> 3. space is allowed before and after the parameter:
	string(STRIP "${type}" type)
	# rename for ufo's plugin system:
	# (i.e. not libSampleBehaviour.so, but ufoplugin_Behaviour_SampleBehaviour.so)
	set_target_properties ( ${plugin} PROPERTIES OUTPUT_NAME "ufoplugin_${type}_${plugin}" )
	# set install directive:
	install (TARGETS ${plugin} DESTINATION ${UFO_TARGET_LIB_DIR})
endforeach()

# group sources for IDEs:
source_group(ufoplugins\\core FILES ${UFOPLUGINS})
