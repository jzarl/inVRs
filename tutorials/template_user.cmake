# Rename this file to, e.g. linux_64.cmake or vs11_32.cmake, and configure your default settings below.
# In your build-tree-directory call 'cmake -C this-file-with-path path-to-source-code'.

set(USER_CMAKE_TEXT "initialized by '${CMAKE_CURRENT_LIST_FILE}'")

set(CMAKE_CONFIGURATION_TYPES Debug CACHE STRING ${USER_CMAKE_TEXT})
set(CMAKE_GENERATOR "Visual Studio 11" CACHE INTERNAL ${USER_CMAKE_TEXT})
set(CMAKE_INSTALL_PREFIX "your-choice" CACHE PATH ${USER_CMAKE_TEXT})

# path to runtime library (dll-files)
set(RUNTIME_LIBRARY_DIRS_DEBUG "boost; glut; ..." CACHE PATH ${USER_CMAKE_TEXT})
set(RUNTIME_LIBRARY_DIRS_RELEASE "your-choice" CACHE PATH ${USER_CMAKE_TEXT})

set(Boost_USE_MULTITHREADED ON CACHE BOOL ${USER_CMAKE_TEXT})
set(Boost_USE_STATIC_LIBS ON CACHE BOOL ${USER_CMAKE_TEXT})
set(Boost_USE_STATIC_RUNTIME OFF CACHE BOOL ${USER_CMAKE_TEXT})
set(BOOST_ROOT "your-choice" CACHE PATH ${USER_CMAKE_TEXT})
set(GLUT_ROOT_DIR "your-choice" CACHE PATH ${USER_CMAKE_TEXT})
set(OPENSG_ROOT "your-choice" CACHE PATH ${USER_CMAKE_TEXT})
set(inVRs_ROOT_DIR "your-choice" CACHE PATH ${USER_CMAKE_TEXT})
set(CAVESceneManager_DIR "${inVRs_ROOT_DIR}/cmake" CACHE PATH ${USER_CMAKE_TEXT})
set(TRACKD_ROOT_DIR "your-choice" CACHE PATH ${USER_CMAKE_TEXT})

# set optionals on (default off)
set(ENABLE_VRPN_SUPPORT ON CACHE BOOL "Support for VRPN Tracking")
#set(ENABLE_TRACKD_SUPPORT ON CACHE BOOL "Support for TrackD Tracking")
set(ENABLE_INVRSVRPNDEVICE_SUPPORT ON CACHE BOOL "Support for inVRsVrpnDevice Tracking")
#set(ENABLE_INVRSTRACKDDEVICE_SUPPORT ON CACHE BOOL "Support for inVRsTrackdDevice Tracking")

# set single tutorials off (default on)
set(BUILD_MedievalTownPhysics OFF CACHE BOOL ${USER_CMAKE_TEXT})
set(BUILD_MedievalTownPhysicsFinal OFF CACHE BOOL ${USER_CMAKE_TEXT})
