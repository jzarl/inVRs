set (TARGET_BIN_DIR ${INVRS_TARGET_BIN_DIR})
set (TARGET_DOC_DIR ${INVRS_TARGET_DOC_DIR})

# Store all source-files in the JOYSTICKSERVER_SRCS variable
set(JOYSTICKSERVER_SRCS JoystickServer.cpp JoystickStateBlockDistributer.cpp)

FIND_PACKAGE(SDL)

if (NOT SDL_FOUND)
   message(FATAL, "Unable to find SDL!")
endif (NOT SDL_FOUND)

include_directories(${SDL_INCLUDE_DIR})

find_package(OpenSG REQUIRED COMPONENTS OSGBase)
include_directories(${OpenSG_INCLUDE_DIRS})
add_definitions(${OpenSG_DEFINITIONS})

# build executables for JoystickServer
add_executable (JoystickServer ${JOYSTICKSERVER_SRCS})

add_dependencies (JoystickServer
	inVRsSystemCore
	irrXML)

target_link_libraries (JoystickServer
	inVRsSystemCore
	irrXML
	${SDL_LIBRARY})

target_link_libraries(JoystickServer ${OpenSG_LIBRARIES})

install (TARGETS JoystickServer
	DESTINATION ${TARGET_BIN_DIR})

install (DIRECTORY config
	DESTINATION ${TARGET_DOC_DIR}/JoystickServer
	PATTERN ".svn" EXCLUDE )

