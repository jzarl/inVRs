cmake_minimum_required(VERSION 2.8)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../cmake/Modules)

include(AddProject)

project(MedievalTownPhysics)

set(INVRS_MODULES OpenSGApplicationBase 3DPhysics Skybox HeightMap AvataraWrapper CollisionMap Interaction)

add_project(MedievalTownPhysics
	SOURCES "src/*.*"
	CONFIGS "config/*.*"
	MODELS "models/*.*"
	DOCS "docs/*.*"
	INVRS_MODULES ${INVRS_MODULES}
)

add_project(MedievalTownPhysicsFinal
	SOURCES "final/src/*.*"
	CONFIGS "final/config/*.*"
	MODELS "models/*.*"
	DOCS "docs/*.*"
	INVRS_MODULES ${INVRS_MODULES}
)
