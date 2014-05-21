cmake_minimum_required (VERSION 2.4)
project (MEDIEVAL_TOWN_PHYSICS)

if (NOT WITH_TRACKD_SUPPORT)
  set (WITH_TRACKD_SUPPORT OFF
  	CACHE BOOL "Enable/disable support for TrackD" FORCE)
endif (NOT WITH_TRACKD_SUPPORT)

include (${MEDIEVAL_TOWN_PHYSICS_SOURCE_DIR}/cmake/config.cmake)

set (MEDIEVAL_TOWN_PHYSICS_SRCS src/MedievalTownPhysics.cpp)

add_executable (MedievalTownPhysics ${MEDIEVAL_TOWN_PHYSICS_SRCS})

target_link_libraries (MedievalTownPhysics
	inVRsSystemCore 
	inVRsOpenSGSceneGraphInterface
	inVRsControllerManager
	inVRsInteraction
	inVRsNavigation
	inVRsHeightMap
	inVRsCollisionMap
	inVRsTextureManager
	inVRsSkybox
	inVRsHandRepresentation
	CAVESceneManager
	inVRsOpenSGApplicationBase
	irrXML
	oops
	inVRs3DPhysics)

if (WITH_TRACKD_SUPPORT)
add_definitions (-DWITH_TRACKD_SUPPORT)
target_link_libraries (MedievalTownPhysics
	inVRsTrackdDevice)
endif (WITH_TRACKD_SUPPORT)

if (WIN32)
target_link_libraries(MedievalTownPhysics
	debug OSGBaseD.lib 
	debug OSGSystemD.lib 
	debug OSGWindowGLUTD.lib 
	optimized OSGBase.lib 
	optimized OSGSystem.lib 
	optimized OSGWindowGLUT.lib)
endif (WIN32)

# Final Tutorial
set (MEDIEVAL_TOWN_PHYSICS_FINAL_SRCS final/src/MedievalTownPhysics.cpp)

add_executable (MedievalTownPhysicsFinal ${MEDIEVAL_TOWN_PHYSICS_FINAL_SRCS})

target_link_libraries (MedievalTownPhysicsFinal
	inVRsSystemCore 
	inVRsOpenSGSceneGraphInterface
	inVRsControllerManager
	inVRsInteraction
	inVRsNavigation
	inVRsHeightMap
	inVRsCollisionMap
	inVRsTextureManager
	inVRsSkybox
	inVRsHandRepresentation
	CAVESceneManager
	inVRsOpenSGApplicationBase
	inVRsAvataraWrapper
	inVRs3DPhysics
	avatara
	oops
	irrXML)

if (WITH_TRACKD_SUPPORT)
#already defined previously
#add_definitions (-DWITH_TRACKD_SUPPORT)
target_link_libraries (MedievalTownPhysicsFinal
	inVRsTrackdDevice)
endif (WITH_TRACKD_SUPPORT)

if (WIN32)
target_link_libraries(MedievalTownPhysicsFinal
	debug OSGBaseD.lib 
	debug OSGSystemD.lib 
	debug OSGWindowGLUTD.lib 
	optimized OSGBase.lib 
	optimized OSGSystem.lib 
	optimized OSGWindowGLUT.lib)
endif (WIN32)

