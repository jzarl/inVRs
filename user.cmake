# CMAKE config file for inVRs

# DON'T COMMENT THIS OUT, IT IS NEEDED TO AVOID MULTIPLE INCLUDING OF THIS FILE
if (NOT INVRS_CONFIG_USER_LOADED)

# DEFINES TARGET BUILD TYPE
# The default target is Release. Uncomment the following line in order to
# change the target build type.
#set (CMAKE_BUILD_TYPE Debug)

# ALLOWS TO SET USER-DEFINED COMPILE FLAGS
#set (USERDEFINED_CXX_FLAGS "-Wall")

# DEFINES WHETHER VRPN SHOULD BE SUPPORTED OR NOT
# By uncommenting the following line you can specify if the VrpnDevice should
# be included in the build process or not. Building this device allows for
# using the VRPN tracking system as input device.
#set (INVRS_ENABLE_VRPN_SUPPORT ON)
#set (vrpn_ROOT_DIR $env(VRPN_HOME) )
#set (vrpn_INCLUDE_DIR $(vrpn_ROOT_DIR)/include )

# DEFINES WHETHER TRACKD SHOULD BE SUPPORTED OR NOT
# By uncommenting the following line you can specify if the TrackdDevice should
# be included in the build process or not. Building this device allows for
# using the Trackd tracking system as input device.
#set (INVRS_ENABLE_TRACKD_SUPPORT ON)

# DEFINES ODE INCLUDE DIRECTORY
# By uncommenting the following line you can specify the path where your ODE
# source distribution is located.
# If this entry is not set cmake tries to find the path by itself. By default
# cmake searches in the folder "external/ode-0.8"
#set (ODE_SOURCE_DIR /usr/local/src/ode-0.8)
#set (ODE_INCLUDE_DIR /usr/local/include)
#set (ODE_LIBRARY_DIR /usr/local/lib)

# DEFINES OPENAL INCLUDE/LIB DIRECTORIES
# By uncommenting the following line you can specify the path where your
# OpenAL headers and libraries can be found.
# If this entry is not set cmake tries to find the path by itself.
#set (OPENAL_INCLUDE_DIR /usr/local/share/OpenAL/include)
#set (OPENAL_LIBRARY_DIR /usr/local/share/OpenAL/lib)

# FOR LINUX SYSTEMS:
# DEFINES OPENSG BINARY DIRECTORY
# By uncommenting the following line you can specify the path where the binary
# "osg-config" from OpenSG is located.
# If this entry is not set cmake tries to find the path by itself.
#set (OPENSG_BIN_DIR /usr/local/bin)
#set (OPENSG_BIN_DIR /bin)

# FOR WINDOWS SYSTEMS:
# DEFINES OPENSG DIRECTORY
# By uncommenting the following line you can specify the path where OpenSG
# is installed.
# If this entry is not set cmake tries to find the path by itself.
#set (OPENSG_DIR C:/Program\ Files/OpenSG)
#set (OPENSG_DIR C:/OpenSGInstall)

# DEFINES THE GLUT DIRECTORIES
# By uncommenting the following lines you can specify the path where the glut
# include and library files can be found. If not specified these are searched
# in the system include and library directories on Linux installations or in
# the OpenSG directory on Windows systems.
#set (GLUT_INCLUDE_DIR C:/Dokumente\ und\ Einstellungen/Administrator/Desktop/inVRs_release/glut-3.7.6-bin)
#set (GLUT_LIBRARY_DIR C:/Dokumente\ und\ Einstellungen/Administrator/Desktop/inVRs_release/glut-3.7.6-bin)
#set (GLUT_INCLUDE_DIR C:/GLUT)
#set (GLUT_LIBRARY_DIR C:/GLUT)

#settings for T038 based setup
#set(BASEDIR C:/Libraries)
#set(BOOST_ROOT ${BASEDIR}/boost_1_44)
#set(OPENSG_ROOT ${BASEDIR}/OpenSG)
#set(OPENSG_DIR ${BASEDIR}/OpenSG)
#set(GLUT_ROOT ${BASEDIR}/OpenSGSupport)
#set(ENV{QTDIR} C:/Qt/4.7.2)
#set(VRPN_ROOT_DIR ${BASEDIR}/VRPN)

set (INVRS_CONFIG_USER_LOADED ON)
endif (NOT INVRS_CONFIG_USER_LOADED)
