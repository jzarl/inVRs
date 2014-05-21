# CMAKE config file for AVATARA

if (NOT AVATARA_CONFIG_USER_LOADED)

# DEFINES IF THE WHOLE AVATARA PACKAGE INCLUDING EXAMPLE BINARIES SHOULD
# BE BUILT OR IF ONLY THE AVATARA LIBRARY IS CREATED.
# By setting this option you can remove the glut dependency by deactivating the
# build of the example applications
#set (AVATARA_BUILD_EXAMPLE_APPLICATIONS OFF)

# DEFINES IF THE OPENSG SUPPORT SHOULD BE INCLUDED IN THE AVATARA LIBRARY
# OR NOT.
# By setting this flag to OFF you can remove the OpenSG dependency 
#set (AVATARA_ENABLE_OPENSG_SUPPORT OFF)

# DEFINES TARGET BUILD TYPE
# The default target is Release. Uncomment the following line in order to
# change the target build type.
#set (CMAKE_BUILD_TYPE Debug)

# DEFINES OPENSG ROOT DIRECTORY
# By uncommenting the following line you can specify the path where the OpenSG
# installation can be found. On linux systems this is the path which contains
# bin/osg-config, on Windows systems this is the path containing include/lib
# folders
# If this entry is not set cmake tries to find the path by itself.
#set (OPENSG_ROOT_DIR /usr/local)

set (AVATARA_CONFIG_USER_LOADED ON)
endif (NOT AVATARA_CONFIG_USER_LOADED)
