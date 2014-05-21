# included files don't affect outside policies:
cmake_policy(SET CMP0011 NEW)


find_package(OpenSG REQUIRED COMPONENTS OSGBase OSGSystem)
include_directories( ${OpenSG_INCLUDE_DIRS} )

include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/include" )
include (${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.cmake)

#aux_source_directory(src/oops OOPS_SRCS)
set ( OOPS_SRCS
	src/oops/ArticulatedBody.cpp
	src/oops/DefaultArticulatedBodyFactory.cpp
	src/oops/DefaultRigidBodyFactory.cpp
	src/oops/FrictionValues.cpp
	src/oops/Geometries.cpp
	src/oops/GeometryFactories.cpp
	src/oops/HtmpHeightFieldLoader.cpp
	src/oops/JointFactories.cpp
	src/oops/Joints.cpp
	src/oops/RigidBody.cpp
	src/oops/Simulation.cpp
	src/oops/SimulationObjectInputListener.cpp
	src/oops/XMLLoader.cpp
	src/oops/OpenSGRenderer.cpp
	src/oops/OpenSGRendererFactory.cpp
	src/oops/OpenSGTransformationWriter.cpp
	src/oops/OpenSGTransformationWriterFactory.cpp
	src/oops/OpenSGTriangleMeshLoader.cpp
	)
aux_source_directory(src/oops/odeJoints ODEJOINTS_SRCS)

set (TARGET_INCLUDE_DIR ${INVRS_TARGET_INCLUDE_DIR}/inVRs/Modules/3DPhysics)
set (TARGET_LIB_DIR ${INVRS_TARGET_LIB_DIR})
set (TARGET_BIN_DIR ${INVRS_TARGET_BIN_DIR})

set(ALL_SRCS ${OOPS_SRCS} ${ODEJOINTS_SRCS})

set(BUILD_TOOLS_OOPS_SHARED OFF CACHE BOOL "build oops as shared library or not")
mark_as_advanced(BUILD_TOOLS_OOPS_SHARED)
if(BUILD_TOOLS_OOPS_SHARED)
	add_library(oops SHARED ${ALL_SRCS})
else()
	add_library(oops STATIC ${ALL_SRCS})
endif()
add_dependencies (oops inVRsSystemCore)

target_link_libraries(oops
    inVRsSystemCore
    ${ODE_LIBRARIES}
	${OpenSG_LIBRARIES}
	)

install (TARGETS oops
	ARCHIVE DESTINATION ${TARGET_LIB_DIR}
	LIBRARY DESTINATION ${TARGET_LIB_DIR}
	RUNTIME DESTINATION ${TARGET_BIN_DIR}
)

configure_file (
	${CMAKE_CURRENT_SOURCE_DIR}/include/oops/configOops.h.in 
	${CMAKE_CURRENT_BINARY_DIR}/include/oops/configOops.h
	)

install (
	FILES 
#	include/oops/TireGeometry.h
        include/oops/Geometries.h
#        include/oops/VehicleFactory.h
#        include/oops/TireGeometryFactory.h
        include/oops/HtmpHeightFieldLoader.h
        include/oops/ArticulatedBody.h
        include/oops/JointFactories.h
        include/oops/OopsMath.h
        include/oops/GeometryFactories.h
        include/oops/Simulation.h
#        include/oops/Vehicle.h
        include/oops/DefaultArticulatedBodyFactory.h
        include/oops/RigidBodyMethodData.h
        include/oops/FrictionValues.h
        include/oops/RigidBody.h
        include/oops/SimulationObjectInputListener.h
        include/oops/Joints.h
        include/oops/XMLLoader.h
        include/oops/DefaultRigidBodyFactory.h
        include/oops/OpenSGRendererFactory.h
        include/oops/OpenSGTransformationWriterFactory.h
        include/oops/OpenSGTransformationWriter.h
        include/oops/OpenSGTriangleMeshLoader.h
        include/oops/OpenSGRenderer.h
		${CMAKE_CURRENT_BINARY_DIR}/include/oops/configOops.h
	DESTINATION
		${TARGET_INCLUDE_DIR}
)

install (FILES include/oops/odeJoints/ODEJoints.h
	DESTINATION ${TARGET_INCLUDE_DIR}/odeJoints)

install (FILES include/oops/Interfaces/SimulationObjectInterface.h
        include/oops/Interfaces/RigidBodyFactory.h
        include/oops/Interfaces/CollisionListenerInterface.h
        include/oops/Interfaces/RendererInterface.h
        include/oops/Interfaces/TriangleMeshLoader.h
        include/oops/Interfaces/HeightFieldLoader.h
        include/oops/Interfaces/TransformationWriterInterface.h
        include/oops/Interfaces/ArticulatedBodyFactory.h
        include/oops/Interfaces/TransformationWriterFactory.h
        include/oops/Interfaces/RendererFactory.h
	DESTINATION ${TARGET_INCLUDE_DIR}/Interfaces)
	
