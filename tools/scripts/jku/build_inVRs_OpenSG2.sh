#!/bin/sh -e
PARALLEL_JOBS=4
DASHBOARD="$1"
#DASHBOARD=Experimental
SOURCE_DIR=/home/edvz/zing/scratch/inVRs_OSG
BUILD_DIR=/home/edvz/zing/scratch/inVRs_OSG/build

rm -rf "$BUILD_DIR"/*
cd "$BUILD_DIR"

CC="$BUILD_DIR/cc"
CXX="$BUILD_DIR/g++"
ln -s "`which ccache`" "$CC"
ln -s "`which ccache`" "$CXX"

. /usr/share/modules/init/sh
module purge

module load cmake

CMAKE_OPTIONS="-DINVRS_ENABLE_TESTING=ON"

module load OpenSG2
# quick&dirty hack around my being to smart for my own good:
HOST=virtu module load trackdAPI

module load vrpn

module load OpenAL

module load ODE
CMAKE_OPTIONS="$CMAKE_OPTIONS -DODE_SOURCE_DIR=/jkuvrc/packages/ODE/DIST/${ODE_VERSION}"

#module load qt
#CMAKE_OPTIONS="$CMAKE_OPTIONS -DINVRS_ENABLE_EDITOR=ON"

if [ -n "$DASHBOARD" ]
then
	DASHBOARD="-D $DASHBOARD"
fi
time ctest --build-and-test "$SOURCE_DIR" "$BUILD_DIR" --build-generator "Unix Makefiles" --build-makeprogram make -j "$PARALLEL_JOBS" --build-options -DCMAKE_CXX_COMPILER="$CXX" -DCMAKE_C_COMPILER="$CC" $CMAKE_OPTIONS --test-command ctest $DASHBOARD

