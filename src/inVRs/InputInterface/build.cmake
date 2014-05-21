set (INPUTINTERFACE_SOURCE_DIR ${INVRS_SOURCE_DIR}/src/inVRs/InputInterface)

set (INPUTINTERFACE_SRCS
	${INVRS_SOURCE_DIR}/src/inVRs/InputInterface/ControllerInterface.cpp
	${INVRS_SOURCE_DIR}/src/inVRs/InputInterface/InputInterface.cpp)

set (INPUTINTERFACE_TARGET_INCLUDE_DIR ${INVRS_TARGET_INCLUDE_DIR}/inVRs/InputInterface)

add_definitions (-DINVRSINPUTINTERFACE_EXPORTS)

install (FILES
		${INPUTINTERFACE_SOURCE_DIR}/ControllerButtonChangeCB.h
		${INPUTINTERFACE_SOURCE_DIR}/ControllerButtonChangeCB.inl
		${INPUTINTERFACE_SOURCE_DIR}/ControllerInterface.h
		${INPUTINTERFACE_SOURCE_DIR}/ControllerManagerInterface.h
		${INPUTINTERFACE_SOURCE_DIR}/InputInterface.h
		${INPUTINTERFACE_SOURCE_DIR}/InputInterfaceSharedLibraryExports.h
	DESTINATION ${INPUTINTERFACE_TARGET_INCLUDE_DIR})
