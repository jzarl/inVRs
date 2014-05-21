# ADD SEPARATE LINE FOR EACH APPLICATION WHICH SHOULD BE BUILT

# Build JoystickServer
autofeature(JoystickServer INVRS_ENABLE_JOYSTICKSERVER
	"Enable the SDL-based Joystick input server."
	REQUIRED_PACKAGES SDL OpenSG:COMPONENTS:OSGBase)
if(INVRS_ENABLE_JOYSTICKSERVER)
	add_subdirectory (JoystickServer)
endif(INVRS_ENABLE_JOYSTICKSERVER)


# Build inVRsEditor
autofeature(inVRsEditor INVRS_ENABLE_EDITOR
	"Build and install the inVRs world editor"
	REQUIRED_PACKAGES GLUT OpenSG:COMPONENTS:OSGDrawable:OSGUtil Qt4:COMPONENTS:QtCore:QtGui:QtOpenGL)
if(INVRS_ENABLE_EDITOR)
	add_subdirectory (inVRsEditor)
endif(INVRS_ENABLE_EDITOR)

