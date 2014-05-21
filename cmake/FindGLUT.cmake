# - Try to find the OpenGL Utility Toolkit (GLUT)
# Once done this module will define
#  GLUT_FOUND
#  GLUT_INCLUDE_DIRS
#  GLUT_LIBRARIES
#  GLUT_DEFINITIONS


# set where to find the library
if(GLUT_ROOT_DIR)
	set(_GLUT_PATHS ${GLUT_ROOT_DIR})
elseif(ENV{GLUT_ROOT_DIR})
	set(_GLUT_PATHS $ENV{GLUT_ROOT_DIR})
	set(GLUT_ROOT_DIR $ENV{GLUT_ROOT_DIR} CACHE PATH "where to find GLUT")
else()
	set(GLUT_ROOT_DIR "GLUT_ROOT_DIR-NOTFOUND" CACHE PATH "where to find GLUT")
endif()

# reset all cached variables if the root directory changed
if(NOT "${_GLUT_PATHS}" STREQUAL "${_GLUT_PATHS_PREV}")
	set(_GLUT_PATHS_PREV ${_GLUT_PATHS} CACHE INTERNAL "previous path for detecting changes")
	unset(GLUT_INCLUDE_DIR CACHE)
	unset(GLUT_LIBRARY CACHE)
	unset(GLUT_LIBRARY_DEBUG CACHE)
	unset(GLUT_LIBRARY_RELEASE CACHE)
endif()

# definitions
set(GLUT_DEFINITION "")

# headers
find_path(GLUT_INCLUDE_DIR
	NAMES GL/glut.h
	PATHS ${_GLUT_PATHS}
	PATH_SUFFIXES include
	DOC "GLUT include path"
)

# x_LIBRARY_(RELEASE|DEBUG)
find_library(
	GLUT_LIBRARY_RELEASE
	NAMES glut glut32 freeglut freeglut_static
	PATHS ${_GLUT_PATHS}
	PATH_SUFFIXES lib lib/x86 lib/glut/Release
	DOC "GLUT library, release configuration"
)
find_library(
	GLUT_LIBRARY_DEBUG
	NAMES glut glut32 freeglutd freeglut freeglut_staticd freeglut_static
	PATHS ${_GLUT_PATHS}
	PATH_SUFFIXES lib lib/x86/Debug lib/glut/Debug
	DOC "GLUT library, debug configuration"
)

# assemble GLUT_LIBRARY with configurations (debug, release)
include(SelectLibraryConfigurations)
select_library_configurations(GLUT)
# The set(... CACHE ...) in select_library_configurations is a bug. Reported.
# Workaround:
set(GLUT_LIBRARY_TMP ${GLUT_LIBRARY})
unset(GLUT_LIBRARY CACHE)
set(GLUT_LIBRARY ${GLUT_LIBRARY_TMP})
unset(GLUT_LIBRARY_TMP)
# workaround for bug in cmake <= 2.8.9:
if(NOT GLUT_LIBRARY AND "${GLUT_LIBRARY_RELEASE}" STREQUAL "${GLUT_LIBRARY_DEBUG}")
	set(GLUT_LIBRARY "${GLUT_LIBRARY_RELEASE}")
endif()

# mark all cached variables as advanced except GLUT_ROOT_DIR
mark_as_advanced(
	GLUT_INCLUDE_DIR
	GLUT_LIBRARY_DEBUG
	GLUT_LIBRARY_RELEASE
)

# dependencies
set(FIND_ARGS)
if(GLUT_FIND_QUIETLY)
	list(APPEND FIND_ARGS QUIET)
endif()
if(GLUT_FIND_REQUIRED)
	list(APPEND FIND_ARGS REQUIRED)
endif()
find_package(OpenGL ${FIND_ARGS})

if(WIN32)
	# glut and freeglut depend on winmm (function gettimeofday).
	list(APPEND GLUT_LIBRARIES winmm.lib)
endif()

# set results
set(GLUT_DEFINITIONS ${GLUT_DEFINITION} ${OPENGL_DEFINITIONS})
set(GLUT_INCLUDE_DIRS ${GLUT_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})
set(GLUT_LIBRARIES ${GLUT_LIBRARY} ${OPENGL_LIBRARIES})

# set x_FOUND
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
	GLUT
	REQUIRED_VARS GLUT_LIBRARY GLUT_INCLUDE_DIR OPENGL_FOUND
	FAIL_MESSAGE "Tip: set GLUT_ROOT_DIR or set the advanced variables directly"
)

if(GLUT_FOUND)
	mark_as_advanced(GLUT_ROOT_DIR)
endif()
