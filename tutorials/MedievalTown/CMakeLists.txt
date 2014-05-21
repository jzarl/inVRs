cmake_minimum_required(VERSION 2.8)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../cmake/Modules)

include(AddProject)

project(MedievalTown)

set(INVRS_MODULES OpenSGApplicationBase Skybox HeightMap AvataraWrapper CollisionMap Navigation Interaction Network ControllerManager)

add_project(MedievalTown
	SOURCES "src/*.*"
	CONFIGS "config/*.*"
	MODELS "models/*.*"
	INVRS_MODULES ${INVRS_MODULES}
)

add_project(MedievalTownFinal
	SOURCES "final/src/*.*"
	CONFIGS "final/config/*.*"
	MODELS "models/*.*"
	DOCS "snippets/*.*"
	INVRS_MODULES ${INVRS_MODULES}
)
