set (OUTPUTINTERFACE_SOURCE_DIR ${INVRS_SOURCE_DIR}/src/inVRs/OutputInterface)

set (OUTPUTINTERFACE_SRCS
	${INVRS_SOURCE_DIR}/src/inVRs/OutputInterface/OutputInterface.cpp
	${INVRS_SOURCE_DIR}/src/inVRs/OutputInterface/SceneGraphInterface.cpp)

set (OUTPUTINTERFACE_TARGET_INCLUDE_DIR ${INVRS_TARGET_INCLUDE_DIR}/inVRs/OutputInterface)

add_definitions (-DINVRSOUTPUTINTERFACE_EXPORTS)

install (FILES 
		${OUTPUTINTERFACE_SOURCE_DIR}/GeometrySceneGraphNodeInterface.h
        ${OUTPUTINTERFACE_SOURCE_DIR}/GroupSceneGraphNodeInterface.h 
        ${OUTPUTINTERFACE_SOURCE_DIR}/ModelInterface.h
		${OUTPUTINTERFACE_SOURCE_DIR}/OutputInterface.h
		${OUTPUTINTERFACE_SOURCE_DIR}/OutputInterfaceSharedLibraryExports.h
		${OUTPUTINTERFACE_SOURCE_DIR}/SceneGraphAttachment.h
		${OUTPUTINTERFACE_SOURCE_DIR}/SceneGraphInterface.h
		${OUTPUTINTERFACE_SOURCE_DIR}/SceneGraphNodeInterface.h
		${OUTPUTINTERFACE_SOURCE_DIR}/TransformationSceneGraphNodeInterface.h
	DESTINATION ${OUTPUTINTERFACE_TARGET_INCLUDE_DIR})
