##############################################################################
# Define the sources of the render-server
##############################################################################
set ( RENDERSERVER_SRC csmrenderserver_qt.cpp)
set ( RENDERSERVER_APPNAME csmrenderserver-qt)

##############################################################################
# Make compilation optional
##############################################################################
if(COMMAND autofeature)
	autofeature(CAVESceneManager/QtServer CAVESCENEMANAGER_ENABLE_QTSERVER
		"Build the Qt4 based CAVESceneManager renderserver-qt."
		REQUIRED_PACKAGES
		  OpenGL
		  OpenSG:COMPONENTS:OSGDrawable:OSGCluster:OSGUtil:OSGWindowQT4:OSGEffectGroups
		  Qt4:COMPONENTS:QtCore:QtGui:QtOpenGL:QtNetwork)
else()
	option(CAVESCENEMANAGER_ENABLE_QTSERVER "Build the Qt4 based CAVESceneManager renderserver-qt." OFF)
endif()
if ( NOT CAVESCENEMANAGER_ENABLE_QTSERVER )
	return()
endif ( NOT CAVESCENEMANAGER_ENABLE_QTSERVER )

##############################################################################
# Find dependencies
##############################################################################

# renderserver-qt depends on:
find_package ( OpenSG REQUIRED COMPONENTS OSGDrawable OSGCluster OSGUtil OSGWindowQT4 OSGEffectGroups)
include_directories ( ${OpenSG_INCLUDE_DIRS} )
list ( APPEND DEP_LIBRARIES ${OpenSG_LIBRARIES} )
add_definitions ( ${OpenSG_DEFINITIONS} )

find_package ( OpenGL REQUIRED)
include_directories ( ${OPENGL_INCLUDE_DIR} )
list ( APPEND DEP_LIBRARIES ${OPENGL_LIBRARIES} )
add_definitions ( ${OPENGL_DEFINITIONS} )

find_package ( Qt4 REQUIRED COMPONENTS QtCore QtGui QtOpenGL QtNetwork)
include ( ${QT_USE_FILE})
list ( APPEND DEP_LIBRARIES ${QT_LIBRARIES} )

##############################################################################
# Build executables for renderserver
##############################################################################
add_executable ( ${RENDERSERVER_APPNAME} ${RENDERSERVER_SRC})
add_dependencies ( ${RENDERSERVER_APPNAME} ${LIBRARY_NAME})
target_link_libraries ( ${RENDERSERVER_APPNAME} ${LIBRARY_NAME} ${DEP_LIBRARIES})

##############################################################################
# Install headers, libraries, and binaries in the target directories
##############################################################################
install ( TARGETS ${RENDERSERVER_APPNAME}
		DESTINATION ${TARGET_BIN_DIR})
