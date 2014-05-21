cmake_minimum_required( VERSION 2.8.9 )
project( baselib )


### Install paths:

if( NOT BASELIB_CMAKE_INSTALL_PREFIX_VALUE STREQUAL CMAKE_INSTALL_PREFIX)
	unset( BASELIB_CMAKE_INSTALL_PREFIX_VALUE CACHE )
	unset( INSTALL_LIB_DIR CACHE )
	unset( INSTALL_INCLUDE_DIR CACHE )
	unset( INSTALL_BIN_DIR CACHE )
	unset( INSTALL_DOC_DIR CACHE )
endif()
set( BASELIB_CMAKE_INSTALL_PREFIX_VALUE "${CMAKE_INSTALL_PREFIX}" CACHE INTERNAL "for detecting install prefix changes" )
set( INSTALL_LIB_DIR ${CMAKE_INSTALL_PREFIX}/lib CACHE PATH "Library installation path" )
set( INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include CACHE PATH "Header file installation path" )
set( INSTALL_BIN_DIR ${CMAKE_INSTALL_PREFIX}/bin CACHE PATH "Program  installation path" )
set( INSTALL_DOC_DIR ${CMAKE_INSTALL_PREFIX}/share/doc/baselib CACHE PATH "Documentation installation path" )
mark_as_advanced( INSTALL_LIB_DIR INSTALL_INCLUDE_DIR INSTALL_BIN_DIR INSTALL_DOC_DIR )

# to make install statements shorter:
set( DEFAULT_INSTALL_STANZA
	RUNTIME
		DESTINATION "${INSTALL_BIN_DIR}"
		COMPONENT runtime
	LIBRARY
		DESTINATION "${INSTALL_LIB_DIR}"
		COMPONENT runtime
	ARCHIVE
		DESTINATION "${INSTALL_LIB_DIR}"
		COMPONENT dev
	PUBLIC_HEADER
		DESTINATION "${INSTALL_INCLUDE_DIR}"
		COMPONENT dev
)


### Testing:

if( DEFINED INVRS_ENABLE_TESTING )
	set( BASELIB_ENABLE_TESTING ${INVRS_ENABLE_TESTING} )
else()
	option( BASELIB_ENABLE_TESTING "Enable unittests for baselib." ON)
endif()
# only set enable_testing if build independently of inVRs:
if( BASELIB_ENABLE_TESTING AND NOT INVRS_ENABLE_TESTING)
	enable_testing()
endif()


### Compatibility stuff:

list( APPEND CMAKE_MODULE_PATH "${baselib_SOURCE_DIR}/cmake/Modules")
# for target_compile_definitions etc.
include( cmake_compatibility )


### sub-libraries:

add_subdirectory( coreplug )
