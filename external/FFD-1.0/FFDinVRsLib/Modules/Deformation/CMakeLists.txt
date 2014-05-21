file(GLOB FFDINVRS_HEADERSMAIN *.h)
file(GLOB FFDINVRS_HEADERSEVENTS Events/*.h)
set(FFDINVRS_HEADERS ${FFDINVRS_HEADERSMAIN} ${FFDINVRS_HEADERSEVENTS})
file(GLOB_RECURSE FFDINVRS_SRCS *.cpp)

set(FFDINVRS_FILES ${FFDINVRS_SRCS} ${FFDINVRS_HEADERS})
if ( WIN32 )
	list ( APPEND FFDINVRS_FILES Deformation.def )
endif ( WIN32 )
add_library ( inVRsDeformation SHARED ${FFDINVRS_FILES} )

#OpenSG
find_package(OpenSG REQUIRED )
include_directories(${OpenSG_INCLUDE_DIRS})
add_definitions(${OpenSG_DEFINITIONS})
target_link_libraries(inVRsDeformation ${OpenSG_LIBRARIES})

#FFD
include_directories(${FFD_INCLUDE_DIRS})
target_link_libraries ( inVRsDeformation ${FFD_LIBRARIES} )

#inVRs
include_directories ( ${inVRs_INCLUDE_DIRS} )
link_directories ( ${inVRs_LIBRARY_DIRS} )
add_definitions ( ${inVRs_DEFINITIONS} )
target_link_libraries ( inVRsDeformation inVRsSystemCore inVRsOpenSGSceneGraphInterface)

# additional exports (for inVRS):
set ( INVRS_EXPORT_Deformation_INCLUDE_DIRS )
set ( INVRS_EXPORT_Deformation_LIBRARY_DIRS )
set ( INVRS_EXPORT_Deformation_LIBRARIES inVRsDeformation ${FFD_LIBRARIES})
set ( INVRS_EXPORT_Deformation_DEFINITIONS )
INVRS_ADD_EXPORTS(Deformation)

install ( FILES
	InVRsDeformActionManager.h
	Deformation.h
	DESTINATION ${FFD_TARGET_INCLUDE_DIR}/inVRs/Modules/Deformation/ )
install ( FILES
	Events/SyncDActionsEvent.h
	Events/ActionEvent.h
	DESTINATION ${FFD_TARGET_INCLUDE_DIR}/inVRs/Modules/Deformation/Events )

install ( TARGETS inVRsDeformation DESTINATION ${FFD_TARGET_LIB_DIR})

