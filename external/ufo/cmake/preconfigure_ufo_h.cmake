# Generate Version information for Plugin-Interfaces:
file (STRINGS "${UFO_SOURCE_DIR}/BRANCHNAME" UFO_BRANCHNAME LIMIT_COUNT 1 LIMIT_INPUT 20 LIMIT_OUTPUT 20 )

foreach ( tmp_type Flock Pilot Behaviour Steerable)
	string ( TOUPPER ${tmp_type} tmp_TYPE )

	# get "canonical" interface description of header file:
	file(STRINGS  "${UFO_SOURCE_DIR}/ufo/${tmp_type}.h" tmp_interface )
	# remove C-style comments ("/* */")
	string(REGEX REPLACE "/[*]([^*]+|[*][^/])*[*]/" "" tmp_interface "${tmp_interface}")
	# remove "//" comments
	string(REGEX REPLACE "//[^;]*" "" tmp_interface "${tmp_interface}")
	# remove whitespace
	string(REGEX REPLACE "[ 	]+" "" tmp_interface "${tmp_interface}")
	# remove line-breaks
	string(REGEX REPLACE "([^\\]);+" "\\1" tmp_interface "${tmp_interface}")
	#message("interface for ${tmp_type}: ${tmp_interface}")

	string(SHA1 UFO_${tmp_TYPE}_ID "${tmp_interface}")
	message(STATUS "Hash for ${tmp_type}: ${UFO_${tmp_TYPE}_ID}")

	if ( NOT UFO_${tmp_TYPE}_ID )
		message ( SEND_ERROR ": Couldn't determine hash for UFO_${tmp_TYPE}_ID!" )
	endif()
endforeach()

configure_file("${UFO_SOURCE_DIR}/ufo/Ufo.h.in" "${INCLUDE_DIR}/Ufo.h~")
execute_process ( COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${INCLUDE_DIR}/Ufo.h~" "${INCLUDE_DIR}/Ufo.h")
