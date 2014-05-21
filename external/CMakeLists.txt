# set TEST_PREFIX for subdirectories (they in turn can also add a sub-prefix):
set (TEST_PREFIX "${TEST_PREFIX}EXTERNAL_")

add_subdirectory(irrxml-1.2)

add_subdirectory(gmtl-0.6.1)

add_subdirectory(baselib)

autofeature(Avatara INVRS_ENABLE_EXTERNAL_AVATARA
	"Build the avatara library with inVRs"
	REQUIRED_PACKAGES 
	  OpenGL OpenSG:COMPONENTS:OSGBase:OSGSystem:OSGDrawable)
if(INVRS_ENABLE_EXTERNAL_AVATARA)
	add_subdirectory(avatara-1.0)
endif(INVRS_ENABLE_EXTERNAL_AVATARA)

autofeature(CAVESceneManager INVRS_ENABLE_EXTERNAL_CAVESCENEMANAGER
	"Build the CAVESceneManager library with inVRs"
	REQUIRED_PACKAGES OpenSG:COMPONENTS:OSGDrawable:OSGCluster:OSGUtil)
if(INVRS_ENABLE_EXTERNAL_CAVESCENEMANAGER)
	add_subdirectory(CAVESceneManager-1.0)
endif(INVRS_ENABLE_EXTERNAL_CAVESCENEMANAGER)

autofeature(FreeFormDeformation INVRS_ENABLE_EXTERNAL_FFD "Build inVRs support for free form deformation?" EXPERIMENTAL)
if(INVRS_ENABLE_EXTERNAL_FFD )
	add_subdirectory(FFD-1.0)
endif(INVRS_ENABLE_EXTERNAL_FFD )

autofeature("Steering(UFO)" INVRS_ENABLE_EXTERNAL_UFO "Build inVRs support for ufo?"
	REQUIRED_CONDITIONS "NOT:${CMAKE_VERSION}:VERSION_LESS:2.8.7" EXPERIMENTAL)
if(INVRS_ENABLE_EXTERNAL_UFO )
	add_subdirectory(ufo)
endif(INVRS_ENABLE_EXTERNAL_UFO )
