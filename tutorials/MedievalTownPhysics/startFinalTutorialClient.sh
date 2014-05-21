#!/bin/sh

# set opensg library path
export OPENSG_LIB_PATH=/usr/local/lib/opt
# set inVRs library path
export INVRS_LIB_PATH=../../lib

# set library path for Linux systems
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$OPENSG_LIB_PATH:$INVRS_LIB_PATH
# set library path for MAC systems
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$OPENSG_LIB_PATH:$INVRS_LIB_PATH

./build/MedievalTownPhysicsFinal -config=final/config/general_desktop_client.xml $*
