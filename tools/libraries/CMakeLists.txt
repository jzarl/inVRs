# ADD SEPARATE LINE FOR EACH LIBRARY WHICH SHOULD BE BUILT

# Build 3DMenu library
autofeature(3DMenu INVRS_ENABLE_3DMENU "Enable/disable 3D menu library"
	REQUIRED_PACKAGES OpenSG:COMPONENTS:OSGBase:OSGSystem)
if(INVRS_ENABLE_3DMENU)
	add_subdirectory (3DMenu)
endif(INVRS_ENABLE_3DMENU)

# Build AvataraWrapper
# INVRS_ENABLE_EXTERNAL_AVATARA is set in external/CMakeLists
if (INVRS_ENABLE_EXTERNAL_AVATARA)
	add_subdirectory (AvataraWrapper)
endif(INVRS_ENABLE_EXTERNAL_AVATARA)

# Build VrpnDevice (if enabled via cmake)
autofeature(Tracking/VRPN INVRS_ENABLE_VRPN_SUPPORT "Enable/disable support for VRPN"
	REQUIRED_PACKAGES VRPN OpenSG:COMPONENTS:OSGBase)
if (INVRS_ENABLE_VRPN_SUPPORT)
	add_subdirectory (VrpnDevice)
endif (INVRS_ENABLE_VRPN_SUPPORT)

# Build TrackdDevice (if enabled via cmake)
autofeature(Tracking/TrackD INVRS_ENABLE_TRACKD_SUPPORT "Enable/disable support for TrackD"
	REQUIRED_PACKAGES TRACKD OpenSG:COMPONENTS:OSGBase)
if (INVRS_ENABLE_TRACKD_SUPPORT)
	add_subdirectory (TrackdDevice)
endif (INVRS_ENABLE_TRACKD_SUPPORT)

# Build OpenSGApplicationBase
autofeature(OpenSGApplicationBase INVRS_ENABLE_OPENSGAPPLICATIONBASE "Enable/disable OpenSGApplicationBase"
	REQUIRED_PACKAGES GLUT OpenSG:COMPONENTS:OSGWindowGLUT:OSGUtil:OSGEffectGroups)
if(INVRS_ENABLE_OPENSGAPPLICATIONBASE)
	add_subdirectory(OpenSGApplicationBase)
endif(INVRS_ENABLE_OPENSGAPPLICATIONBASE)

# Build Sound
autofeature(Sound INVRS_ENABLE_SOUND "Enable/disable sound library"
	REQUIRED_PACKAGES OpenAL)
if(INVRS_ENABLE_SOUND)
	add_subdirectory (Sound)
endif(INVRS_ENABLE_SOUND)

autofeature(Particles INVRS_ENABLE_PARTICLES "Enable/disable particle effects library"
	REQUIRED_PACKAGES OpenSG:COMPONENTS:OSGBase PNG
	REQUIRED_CONDITIONS "${OpenSG_MAJOR_VERSION}:EQUAL:1")
if(INVRS_ENABLE_PARTICLES)
	add_subdirectory (Particles)
endif(INVRS_ENABLE_PARTICLES)

# Build PhysicsSoundManager
#add_subdirectory (PhysicsSoundManager)

# Build collisionmap library
add_subdirectory (CollisionMap)

# Build Heightmap library
add_subdirectory (HeightMap)

# Build Skybox library
add_subdirectory (Skybox)

# Build TextureManager library
add_subdirectory (TextureManager)
