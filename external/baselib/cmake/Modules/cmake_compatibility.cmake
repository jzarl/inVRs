if( "${CMAKE_VERSION}" VERSION_LESS "2.8.9" )
	# for CMakePackageConfigHelpers
	list( APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake-pre-2.8.9")
endif()

if( NOT COMMAND target_compile_definitions)
	message(STATUS "Using compatibility implementation for target_compile_definitions.")
	### target_compile_definitions
	# ignore the PRIVATE/PUBLIC/INTERFACE keywords and just blindly add the definitions to the target's COMPILE_DEFINITIONS
	function( target_compile_definitions target)
		get_target_property(defs "${target}" COMPILE_DEFINITIONS)
		if(NOT defs)
			unset( defs )
		endif()
		# filter out keywords:
		foreach(arg IN LISTS ARGN)
			if( NOT (arg STREQUAL "INTERFACE" OR arg STREQUAL "PUBLIC" OR arg STREQUAL "PRIVATE") )
				LIST( APPEND defs "${arg}")
			endif()
		endforeach()
	set_target_properties("${target}" PROPERTIES COMPILE_DEFINITIONS "${defs}")
	endfunction()
endif()

# TODO: target_compile_options
#       target_include_directories
