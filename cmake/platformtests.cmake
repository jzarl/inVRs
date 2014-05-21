# unset a cache variable if the trigger text has changed
function(unset_if_changed variable trigger)
	if ( NOT "${trigger}" STREQUAL "${CHANGE_TRIGGER_${variable}}" )
		unset( ${variable} CACHE )
	endif()
	set(CHANGE_TRIGGER_${variable} ${trigger} CACHE INTERNAL "Change trigger for unset_if_changed(${variable})" FORCE )
endfunction(unset_if_changed)

include(CheckCXXSourceCompiles)

# use a function so that no variables are polluted:
function(OPENSG_FEATURE_TEST program_text feature_name)
	# unset existing OpenSG stuff:
	set(OpenSG_INCLUDE_DIRS)
	set(OpenSG_LIBRARIES)
	set(OpenSG_DEFINITIONS)
	find_package(OpenSG REQUIRED COMPONENTS OSGBase QUIET)
	set(CMAKE_REQUIRED_INCLUDES ${OpenSG_INCLUDE_DIRS})
	set(CMAKE_REQUIRED_LIBRARIES ${OpenSG_LIBRARIES} )
	set(CMAKE_REQUIRED_DEFINITIONS ${OpenSG_DEFINITIONS} )
	# make sure to reevaluate if OpenSG has changed:
	unset_if_changed(${feature_name} "${OpenSG_INCLUDE_DIRS}${OpenSG_LIBRARIES}${OpenSG_DEFINITIONS}")
	CHECK_CXX_SOURCE_COMPILES("${program_text}" "${feature_name}")
	# no need to export ${feature_name} to parent scope: this is set as internal cache variable
endfunction(OPENSG_FEATURE_TEST)

##############################################################################
# Platform tests:
##############################################################################

# C++11 status:
include(CheckCXX11Features)
if(CXX11_COMPILER_FLAGS)
	# some compilers need to enable C++11 support explicitly:
	message(STATUS "Enabling compiler support for C++11")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX11_COMPILER_FLAGS}")
endif()
foreach(feature IN ITEMS
	HAS_CXX11_AUTO HAS_CXX11_AUTO_RET_TYPE HAS_CXX11_CLASS_OVERRIDE
	HAS_CXX11_CONSTEXPR HAS_CXX11_CSTDINT_H HAS_CXX11_DECLTYPE
	HAS_CXX11_FUNC HAS_CXX11_INITIALIZER_LIST HAS_CXX11_LAMBDA
	HAS_CXX11_LIB_REGEX HAS_CXX11_LONG_LONG HAS_CXX11_NULLPTR
	HAS_CXX11_RVALUE_REFERENCES HAS_CXX11_SIZEOF_MEMBER
	HAS_CXX11_STATIC_ASSERT HAS_CXX11_VARIADIC_TEMPLATES
	HAS_CXX11_UNIQUE_PTR
	)
	if(${feature})
		add_definitions(-D${feature})
	endif()
endforeach()
# allow general usage of nullptr:
if(NOT HAS_CXX11_NULLPTR)
	message(STATUS "Enabling nullptr=0 workaround for older compilers...")
	add_definitions(-Dnullptr=0)
endif()

### stat()
# generally, using stat(2) is safer than trying to fopen a file.
# Thus, stat should be used by default:
CHECK_CXX_SOURCE_COMPILES("#include <sys/stat.h>\nint main(){ struct stat sb; stat(\"filename\",&sb); }\n" INVRS_HAVE_STAT)
if(INVRS_HAVE_STAT)
	add_definitions(-DINVRS_HAVE_STAT)
endif()

### OSG::GeoProperty::size():
# size() was introduced in OpenSG2; older versions always use getSize()
OPENSG_FEATURE_TEST("#include <OpenSG/OSGGeoProperty.h>\nint main() { OSG::GeoProperty *prop=0; return prop->size(); }\n" OPENSG_GEOPROPERTY_HAVE_SIZE_FUNCTION)
# test for old implementation, too:
OPENSG_FEATURE_TEST("#include <OpenSG/OSGGeoProperty.h>\nint main() { OSG::GeoProperty *prop=0; return prop->getSize(); }\n" OPENSG_GEOPROPERTY_HAVE_GETSIZE_FUNCTION)
if (OPENSG_GEOPROPERTY_HAVE_SIZE_FUNCTION)
	# needed at invrs creation time only
	add_definitions(-DOPENSG_GEOPROPERTY_HAVE_SIZE_FUNCTION)
endif ()
# For OpenSG1, both checks are expected to fail
if(OpenSG_MAJOR_VERSION EQUAL 2)
	if(NOT (OPENSG_GEOPROPERTY_HAVE_SIZE_FUNCTION OR OPENSG_GEOPROPERTY_HAVE_GETSIZE_FUNCTION))
		message(SEND_ERROR
			"Could not determine whether OpenSG uses GeoProperty::getSize() or GeoProperty::size()!\n"
			"This error occurs if OpenSG is not functioning properly. Try running cmake with '--debug-trycompile' and check the compiler output."
		)
		unset(OPENSG_GEOPROPERTY_HAVE_SIZE_FUNCTION CACHE)
		unset(OPENSG_GEOPROPERTY_HAVE_GETSIZE_FUNCTION CACHE)
	endif()
endif()

### OSG::ThreadManager::getLock
# The getLock API was changed in git commit 517b200 (Oct 2009):
OPENSG_FEATURE_TEST("#include <OpenSG/OSGThreadManager.h>\n#include <OpenSG/OSGLock.h>\nint main() { OSG::ThreadManager::the()->getLock( \"testLock\", \"OSGLock\" );\n return 0; }\n" OPENSG_THREADMANAGER_GETLOCK_HAVE_OLDAPI)
# the new interface has an additional parameter "UInt32 bGlobal", which is used as boolean parameter (if in doubt, use bGlobal=false):
OPENSG_FEATURE_TEST("#include <OpenSG/OSGThreadManager.h>\n#include <OpenSG/OSGLock.h>\nint main() { bool f=false; OSG::ThreadManager::the()->getLock( \"testLock\",f );\n return 0; }\n" OPENSG_THREADMANAGER_GETLOCK_HAVE_BGLOBAL)
if ( OPENSG_THREADMANAGER_GETLOCK_HAVE_BGLOBAL )
	# needed at invrs creation time only
	add_definitions(-DOPENSG_THREADMANAGER_GETLOCK_HAVE_BGLOBAL)
endif()
if ( NOT (OPENSG_THREADMANAGER_GETLOCK_HAVE_BGLOBAL OR OPENSG_THREADMANAGER_GETLOCK_HAVE_OLDAPI))
	message(SEND_ERROR
		"Could not determine whether OpenSG has old or new API for OSG::ThreadManager::getLock!\n"
		"This error occurs if OpenSG is not functioning properly. Try running cmake with '--debug-trycompile' and check the compiler output."
	)
	unset(OPENSG_THREADMANAGER_GETLOCK_HAVE_OLDAPI CACHE)
	unset(OPENSG_THREADMANAGER_GETLOCK_HAVE_BGLOBAL CACHE)
endif()

### OSG::DgramSocket::recvFromRaw()
# old: socket->recvFrom(buffer, msgSize + 4, *src);
OPENSG_FEATURE_TEST("#include <OpenSG/OSGDgramSocket.h>\nint main() { OSG::DgramSocket *socket=0; OSG::SocketAddress* src=0; uint8_t* buffer=0; socket->recvFrom(buffer, 0, *src); return 0; }\n" OPENSG_DGRAMSOCKET_HAVE_RECVFROM_FUNCTION)
# new: socket->recvFromRaw(buffer, msgSize + 4, *src);
OPENSG_FEATURE_TEST("#include <OpenSG/OSGDgramSocket.h>\nint main() { OSG::DgramSocket *socket=0; OSG::SocketAddress* src=0; uint8_t* buffer=0; socket->recvFromRaw(buffer, 0, *src); return 0; }\n" OPENSG_DGRAMSOCKET_HAVE_RECVFROMRAW_FUNCTION)
if ( OPENSG_DGRAMSOCKET_HAVE_RECVFROMRAW_FUNCTION )
	# needed at invrs creation time only
	add_definitions(-DOPENSG_DGRAMSOCKET_HAVE_RECVFROMRAW_FUNCTION)
endif()
if ( NOT (OPENSG_DGRAMSOCKET_HAVE_RECVFROMRAW_FUNCTION OR OPENSG_DGRAMSOCKET_HAVE_RECVFROM_FUNCTION))
	message(SEND_ERROR
		"Could not determine whether OpenSG uses OSG::DgramSocket::recvFrom() or OSG::DgramSocket::recvFromRaw()!\n"
		"This error occurs if OpenSG is not functioning properly. Try running cmake with '--debug-trycompile' and check the compiler output."
	)
	unset(OPENSG_DGRAMSOCKET_HAVE_RECVFROMRAW_FUNCTION CACHE)
	unset(OPENSG_DGRAMSOCKET_HAVE_RECVFROM_FUNCTION CACHE)
endif()
