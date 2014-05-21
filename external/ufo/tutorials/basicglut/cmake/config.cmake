# Only execute commands if file was not included already
if (NOT UFO_tutorial_basicglut_CONFIG_CMAKE_LOADED)

	# Set a default build type if no build type is set
	if (NOT CMAKE_BUILD_TYPE)
		set (CMAKE_BUILD_TYPE Debug)
	endif (NOT CMAKE_BUILD_TYPE)

	# Set the default install prefix to the UFO directory if not already set in
	# user.cmake
	if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
		set (CMAKE_INSTALL_PREFIX ${UFO_tutorial_basicglut_SOURCE_DIR}/install
			CACHE PATH "UFO basicglut tutorial install prefix" FORCE)
	endif (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

	# Allow own module-definitions to be found:
	list ( APPEND CMAKE_MODULE_PATH "${UFO_tutorial_basicglut_SOURCE_DIR}/../../cmake_common" )
	message ( STATUS "CMAKE_MODULE_PATH is ${CMAKE_MODULE_PATH}" )


	###############################################################################
	# Find GLUT include path
	###############################################################################
	include (FindGLUT)
	if ( NOT GLUT_FOUND )
		message ( FATAL_ERROR "UFO tutorial 'basicglut' need GLUT, but libGLUT couldn't be found!")
	endif ( NOT GLUT_FOUND )
	add_library ( glut UNKNOWN IMPORTED )
	set_target_properties ( glut PROPERTIES IMPORTED_LOCATION ${GLUT_LIBRARIES} )
	include_directories ( ${GLUT_INCLUDE_DIR})

	###############################################################################
	# Find UFO include path
	###############################################################################
	if ( NOT UFO_ROOT_DIR )
		set ( UFO_ROOT_DIR ${UFO_tutorial_basicglut_SOURCE_DIR}/../../install )
	endif ( NOT UFO_ROOT_DIR )
	include (FindUFO)
	if ( NOT UFO_FOUND )
		message ( FATAL_ERROR "UFO tutorials need libufo. Did you forget to make the install target for ufo?")
	endif ( NOT UFO_FOUND )
	# import libufo:
	add_library( ufo SHARED IMPORTED )
	set_target_properties ( ufo PROPERTIES IMPORTED_LOCATION ${UFO_LIBRARIES} )
	include_directories ( ${UFO_INCLUDE_DIRS})

	###############################################################################
	# Set Compiler flags
	###############################################################################
	if (NOT WIN32)
		set (CMAKE_CXX_FLAGS "-Wall -pedantic -Wno-variadic-macros")
		set (CMAKE_CXX_FLAGS_RELEASE "-O3")
		set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g")
		set (CMAKE_CXX_FLAGS_DEBUG "-O0 -g -DDEBUG")
	else (NOT WIN32) # the following platforms are not tested yet:
		message (FATAL_ERROR "Found unsupported operating system: Only Linux systems are supported!")  
	endif (NOT WIN32)

	set (CMAKE_CXX_FLAGS_RELEASE "${UFO_tutorial_basicglut_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} ${USERDEFINED_CXX_FLAGS}")
	set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${UFO_tutorial_basicglut_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${USERDEFINED_CXX_FLAGS}")
	set (CMAKE_CXX_FLAGS_DEBUG "${UFO_tutorial_basicglut_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} ${USERDEFINED_CXX_FLAGS}")

	# set formatting for eclipse output
	if (CMAKE_COMPILER_IS_GNUCC)
		set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0")
	endif (CMAKE_COMPILER_IS_GNUCC)
	if (CMAKE_COMPILER_IS_GNUCXX)
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
	endif (CMAKE_COMPILER_IS_GNUCXX)


	set (UFO_tutorial_basicglut_CONFIG_CMAKE_LOADED ON)

endif (NOT UFO_tutorial_basicglut_CONFIG_CMAKE_LOADED)

