# CMAKE config file for CAVESCENEMANAGER

# DON'T COMMENT THIS OUT, IT IS NEEDED TO AVOID MULTIPLE INCLUDING OF THIS FILE
if (NOT CAVESCENEMANAGER_CONFIG_USER_LOADED)

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

set (CAVESCENEMANAGER_CONFIG_USER_LOADED ON)
endif (NOT CAVESCENEMANAGER_CONFIG_USER_LOADED)
