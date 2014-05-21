include(GenerateExportHeader)

add_compiler_export_flags()
include_directories(${baselib_BINARY_DIR}/coreplug)

set( COREPLUG_SOURCES
	Dso.cxx
	FactoryPlugin.cxx
)
# public headers:
set( COREPLUG_HEADERS
	Dso.h
	FactoryPlugin.h
)

add_library( coreplug SHARED ${COREPLUG_SOURCES})
target_compile_definitions( coreplug
	PRIVATE SHARED_MODULE_PREFIX="${CMAKE_SHARED_MODULE_PREFIX}" SHARED_MODULE_SUFFIX="${CMAKE_SHARED_MODULE_SUFFIX}"
)
generate_export_header( coreplug
	PREFIX_NAME BASELIB_
	EXPORT_MACRO_NAME COREPLUG_API
)
set_target_properties( coreplug PROPERTIES PUBLIC_HEADER "${COREPLUG_HEADERS};${baselib_BINARY_DIR}/coreplug/coreplug_export.h")

install( TARGETS coreplug
	EXPORT BaseLibTargets
	${DEFAULT_INSTALL_STANZA}
)

if( WIN32 )
	# Windows 32/64, including cygwin
	# if( CYGWIN )
elseif( UNIX )
	# if( APPLE )
	target_link_libraries( coreplug dl)
endif()


if( BASELIB_ENABLE_TESTING )
	add_subdirectory( unittests )
endif()
