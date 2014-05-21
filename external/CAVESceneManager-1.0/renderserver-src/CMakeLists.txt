##############################################################################
# Define the sources of the render-server
##############################################################################
set ( RENDERSERVER_SRC csmrenderserver.cpp)
set ( RENDERSERVER_APPNAME csmrenderserver)

##############################################################################
# Make compilation optional
##############################################################################
if(COMMAND autofeature)
	autofeature(CAVESceneManager/GLUTServer CAVESCENEMANAGER_ENABLE_GLUTSERVER
		"Build the GLUT based CAVESceneManager renderserver (traditional renderserver)."
		REQUIRED_PACKAGES
		  OpenSG:COMPONENTS:OSGDrawable:OSGCluster:OSGUtil:OSGWindowGLUT:OSGEffectGroups
		  OpenGL GLUT)
else()
	option(CAVESCENEMANAGER_ENABLE_GLUTSERVER "Build the GLUT based CAVESceneManager renderserver (traditional renderserver)." ON)
endif()
if ( NOT CAVESCENEMANAGER_ENABLE_GLUTSERVER )
	return()
endif ( NOT CAVESCENEMANAGER_ENABLE_GLUTSERVER )

##############################################################################
# Find dependencies
##############################################################################

# renderserver depends on:
find_package ( OpenSG REQUIRED COMPONENTS OSGDrawable OSGCluster OSGUtil OSGWindowGLUT OSGEffectGroups)
include_directories ( ${OpenSG_INCLUDE_DIRS} )
add_definitions ( ${OpenSG_DEFINITIONS})
list ( APPEND DEP_LIBRARIES ${OpenSG_LIBRARIES})

find_package ( GLUT REQUIRED)
include_directories ( ${GLUT_INCLUDE_DIR})
add_definitions ( ${GLUT_DEFINITIONS})
list ( APPEND DEP_LIBRARIES ${GLUT_LIBRARIES})

find_package ( OpenGL REQUIRED)
include_directories ( DEP_INCLUDE_DIRS ${OPENGL_INCLUDE_DIR})
add_definitions ( ${OPENGL_DEFINITIONS})
list ( APPEND DEP_LIBRARIES ${OPENGL_LIBRARIES})

##############################################################################
# Build executables for renderserver
##############################################################################
add_executable ( ${RENDERSERVER_APPNAME} ${RENDERSERVER_SRC})
add_dependencies ( ${RENDERSERVER_APPNAME} ${LIBRARY_NAME})
if ( WIN32)
	list ( APPEND DEP_LIBRARIES ws2_32)
endif ( WIN32)
target_link_libraries ( ${RENDERSERVER_APPNAME} ${LIBRARY_NAME} ${DEP_LIBRARIES})

if(WIN32)
	foreach(CONFIGURATION_TYPE ${CMAKE_CONFIGURATION_TYPES})
		string(TOUPPER ${CONFIGURATION_TYPE} CONFIGURATION_TYPE_UPPERCASE)
		set(CSM_RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBRARY_DIRS_${CONFIGURATION_TYPE_UPPERCASE}})
		configure_file("start_csm.bat.in" "${CONFIGURATION_TYPE}/start_csm.bat" @ONLY)
		install(
			PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/${CONFIGURATION_TYPE}/start_csm.bat"
			CONFIGURATIONS ${CONFIGURATION_TYPE}
			DESTINATION ${TARGET_BIN_DIR}
		)
	endforeach()
endif()

##############################################################################
# Install headers, libraries, and binaries in the target directories
##############################################################################
install(
	TARGETS ${RENDERSERVER_APPNAME}
	DESTINATION ${TARGET_BIN_DIR}
)
