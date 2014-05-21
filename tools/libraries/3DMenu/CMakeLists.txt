aux_source_directory(core 3DMENU_SRCS)
aux_source_directory(libAnimations ANIMATIONS_SRCS)
aux_source_directory(libWidgets WIDGETS_SRCS)

set (TARGET_INCLUDE_DIR ${INVRS_TARGET_INCLUDE_DIR}/inVRs/tools/libraries/3DMenu)
set (TARGET_LIB_DIR ${INVRS_TARGET_LIB_DIR})
set (TARGET_BIN_DIR ${INVRS_TARGET_BIN_DIR})

add_definitions (-D_3DMENU_EXPORTS)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/core )

find_package(OpenSG REQUIRED COMPONENTS OSGBase OSGSystem)
include_directories(${OpenSG_INCLUDE_DIRS})
add_definitions(${OpenSG_DEFINITIONS})

add_library(3DMenu SHARED ${3DMENU_SRCS})
add_dependencies (3DMenu irrXML)
target_link_libraries (3DMenu irrXML)

add_library(3DMenu_Widgets SHARED ${WIDGETS_SRCS})
add_dependencies (3DMenu_Widgets 3DMenu irrXML)
target_link_libraries (3DMenu_Widgets 3DMenu irrXML)

add_library(3DMenu_Animations SHARED ${ANIMATIONS_SRCS})
add_dependencies (3DMenu_Animations 3DMenu irrXML)
target_link_libraries (3DMenu_Animations 3DMenu irrXML)

target_link_libraries(3DMenu ${OpenSG_LIBRARIES})


install (FILES core/Config.h
		core/Loader.h
		core/MenuControl.h
		core/Menu.h
		core/MenuObject.h
		core/NamedObject.h
		core/ObjectStore.h
		core/SceneGraph.h
		core/Widget.h
		core/XMLNode.h
	DESTINATION ${TARGET_INCLUDE_DIR})

install (TARGETS 3DMenu
		3DMenu_Widgets
		3DMenu_Animations
	ARCHIVE DESTINATION ${TARGET_LIB_DIR}
	LIBRARY DESTINATION ${TARGET_LIB_DIR}
	RUNTIME DESTINATION ${TARGET_BIN_DIR}
)
