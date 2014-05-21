cmake_minimum_required(VERSION 2.8)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../cmake/Modules)

include(AddProject)

project(MedievalTownCSM)

set(INVRS_MODULES OpenSGApplicationBase Skybox HeightMap AvataraWrapper CollisionMap Navigation Interaction Network ControllerManager TextureManager)

add_project(MedievalTownCSM
	SOURCES "src/MedievalTownCSM.cpp"
	CONFIGS "config/*.*"
	MODELS "models/*.*"
	IMAGES "images/*.*"
	DOCS "docs/*.*"
	INVRS_MODULES ${INVRS_MODULES}
)
