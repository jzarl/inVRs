#!/bin/sh

# set opensg library path
export OPENSG_LIB_PATH=/usr/local/lib/opt
# set inVRs library path
export INVRS_LIB_PATH=../../lib

# set library path for Linux systems
export LD_LIBRARY_PATH=$INVRS_LIB_PATH:$OPENSG_LIB_PATH:.:$LD_LIBRARY_PATH
# set library path for MAC systems
export DYLD_LIBRARY_PATH=$INVRS_LIB_PATH:$OPENSG_LIB_PATH:.:$DYLD_LIBRARY_PATH

./build/GoingImmersive $*
