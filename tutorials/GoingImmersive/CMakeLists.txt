cmake_minimum_required(VERSION 2.8)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../cmake/Modules)

include(AddProject)

project(GoingImmersive)

set(INVRS_MODULES OpenSGApplicationBase AvataraWrapper)

add_project(GoingImmersive
	SOURCES "src/*.*"
	CONFIGS "config/*.*"
	MODELS "models/*.*"
	IMAGES "images/*.*"
	DOCS "snippets/*.*"
	INVRS_MODULES ${INVRS_MODULES}
)

add_project(GoingImmersiveFinal
	SOURCES "final/src/*.*"
	CONFIGS "final/config/*.*"
	MODELS "models/*.*"
	IMAGES "images/*.*"
	DOCS "snippets/*.*"
	INVRS_MODULES ${INVRS_MODULES}
)
