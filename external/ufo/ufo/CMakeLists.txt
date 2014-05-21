# define header-files
set ( UFO_HEADERS
	Behaviour.h
	ConfigurationElement.h
	ConfigurationReader.h
	Configurator.h
	Debug.h
	Flock.h
	Pilot.h
	PlainConfigurationReader.h
	Plugin_def.h
	Plugin.h
	Steerable.h
	SteeringDecision.h
	UfoDB.h
	)

#add source-files for all headers:
foreach (tmp ${UFO_HEADERS})
	# change .h -> .cpp
	string ( REPLACE .h .cpp tmp1 ${tmp} )
	# not every .h file has a corresponding .cpp file:
	if ( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${tmp1}" )
		# add .cpp file to UFO_SOURCES
		list ( APPEND UFO_SOURCES ${tmp1} )
	endif ( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${tmp1}" )
endforeach()

# create Ufo.h
add_custom_target( UpdateUFOVersion ALL
	COMMAND ${CMAKE_COMMAND}
	-DUFO_SOURCE_DIR="${UFO_SOURCE_DIR}" -DINCLUDE_DIR="${CMAKE_CURRENT_BINARY_DIR}"
	-DUFO_HAVE_inVRs="${UFO_HAVE_inVRs}" -DUFO_HAVE_inVRs_3DPhysics="${UFO_HAVE_inVRs_3DPhysics}"
	-P "${UFO_SOURCE_DIR}/cmake/preconfigure_ufo_h.cmake"
	COMMENT "Updating UFO version header."
	)

add_library(ufo SHARED ${UFO_SOURCES})
target_link_libraries(ufo dl)

install (FILES ${UFO_HEADERS} "${CMAKE_CURRENT_BINARY_DIR}/Ufo.h"
	DESTINATION ${UFO_TARGET_INCLUDE_DIR}/ufo)

install (TARGETS ufo DESTINATION ${UFO_TARGET_LIB_DIR})

# save definitions, so another project can use them:
configure_file (
	"config_ufo.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/config_ufo.cmake"
	@ONLY )
install ( FILES "${CMAKE_CURRENT_BINARY_DIR}/config_ufo.cmake"
	DESTINATION ${UFO_TARGET_INCLUDE_DIR}/ufo)

# add ufo core exports at the beginning of the exports-lists:
list ( INSERT UFO_EXPORT_INCLUDE_DIRS 0 ${UFO_TARGET_INCLUDE_DIR} )
list ( INSERT UFO_EXPORT_LIBRARIES 0 ufo dl )
list ( INSERT UFO_EXPORT_LIBRARY_DIRS 0 ${UFO_TARGET_LIB_DIR} )
