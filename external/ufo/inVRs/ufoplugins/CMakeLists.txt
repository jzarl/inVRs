# define inVRs-plugin-files
set ( UFOPLUGINS_inVRs
	InVRsSteerable
	FollowinVRsEntityBehaviour
	FollowinVRsUserBehaviour
	)

# additional libraries etc. for certain plugins:
# TODO: when we decide to require a newer cmake version (>=2.8.11), we could use generator expressions here:
if ( UFO_inVRs_internal AND INVRS_ENABLE_MODULE_3DPHYSICS )
	list(APPEND UFOPLUGINS_inVRs "InVRsPhysicsSteerable")
	set ( InVRsPhysicsSteerable_LIBRARIES inVRs3DPhysics )
	set ( InVRsPhysicsSteerable_INCLUDE_DIRS ${INVRS_SOURCE_DIR}/src/inVRs/Modules/3DPhysics/oops/include ${ODE_INCLUDE_DIR})
else()
	set ( InVRsPhysicsSteerable_INCLUDE_DIRS ${inVRs_3DPhysics_INCLUDE_DIRS} )
	set ( InVRsPhysicsSteerable_LIBRARY_DIRS ${inVRs_3DPhysics_LIBRARY_DIRS} )
	set ( InVRsPhysicsSteerable_DEFINITIONS ${inVRs_3DPhysics_DEFINITIONS} )
	set ( InVRsPhysicsSteerable_LIBRARIES ${inVRs_3DPhysics_LIBRARIES} )
endif()

################################################################################
# Create plugin rules:
################################################################################
if ( NOT UFO_inVRs_internal )
	# add dependencies:
	include_directories ( ${inVRs_INCLUDE_DIRS} )
	link_directories ( ${inVRs_LIBRARY_DIRS} )
	add_definitions ( ${inVRs_DEFINITIONS} )
endif ( NOT UFO_inVRs_internal )

foreach (plugin ${UFOPLUGINS_inVRs} )
	include_directories ( ${${plugin}_INCLUDE_DIRS} )
	link_directories ( ${${plugin}_LIBRARY_DIRS} )
	add_definitions ( ${${plugin}_DEFINITIONS} )
	# add plugins as MODULE (shared library meant to be included using dlopen or similiar)
	add_library ( ${plugin} MODULE ${plugin}.cpp )
	# make plugin dependent on libufo target
	target_link_libraries ( ${plugin} ufo ${${plugin}_LIBRARIES} )
	if ( UFO_inVRs_internal )
		target_link_libraries ( ${plugin} inVRsSystemCore )
	else ( UFO_inVRs_internal )
		target_link_libraries ( ${plugin} ${inVRs_LIBRARIES} )
	endif ( UFO_inVRs_internal )
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
	# additional linker flags for plugin:
	if ( NOT ${${plugin}_LINK_FLAGS} STREQUAL "" )
		set_target_properties(${plugin} PROPERTIES LINK_FLAGS ${${plugin}_LINK_FLAGS})
	endif ( NOT ${${plugin}_LINK_FLAGS} STREQUAL "" )
	# set install directive:
	install (TARGETS ${plugin} DESTINATION ${UFO_TARGET_LIB_DIR})
endforeach()
