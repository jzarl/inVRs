set (FFDSAMPLE sample)

add_executable (${FFDSAMPLE} src/main.cpp)

#FFD
add_dependencies(${FFDSAMPLE} FFD)
include_directories (${FFD_INCLUDE_DIRS})
target_link_libraries(${FFDSAMPLE} ${FFD_LIBRARIES})
include_directories (${FFD_DEP_INCLUDE_DIRS})
target_link_libraries(${FFDSAMPLE} ${FFD_DEP_LIBRARIES})

#GLUT
include_directories (${GLUT_INCLUDE_DIR})
target_link_libraries(${FFDSAMPLE} ${GLUT_LIBRARIES})
if ( ${OPENSG_VERSION_MAJOR} EQUAL 2)
       target_link_libraries (${FFDSAMPLE} OSGDrawable OSGUtil )
endif ( ${OPENSG_VERSION_MAJOR} EQUAL 2)

install (TARGETS ${FFDSAMPLE} RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin)


