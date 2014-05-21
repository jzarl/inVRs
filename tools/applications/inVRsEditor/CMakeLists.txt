set (TARGET_BIN_DIR ${INVRS_TARGET_BIN_DIR})

find_package(Qt4 REQUIRED COMPONENTS QtCore QtGui QtOpenGL)
include(${QT_USE_FILE})

include_directories ( ${inVRs_INCLUDE_DIRS} )
link_directories ( ${inVRs_LIBRARY_DIRS} )
add_definitions ( ${inVRs_DEFINITIONS} )

find_package(OpenSG COMPONENTS OSGDrawable OSGUtil)
include_directories(${OpenSG_INCLUDE_DIRS})
add_definitions(${OpenSG_DEFINITIONS})

find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})
add_definitions(${GLUT_DEFINITIONS})

set(Editor_SRCS
    Main.cpp
    MainWindow.cpp

    EditorBase.h
    EditorBase.cpp
    EditorLayer.cpp
    EditorDataTypes.h
    EditorDataTypes.cpp
    ConfigCreator.h
    ConfigCreator.cpp

    Widgets/OpenSGWidget.cpp
    Widgets/EntityTypeWidget.cpp
    Widgets/EntityWidget.cpp
    Widgets/EnvironmentWidget.cpp
    Widgets/PropertiesWidget.cpp
    Widgets/TransformationWidget.cpp

    Dialogs/NewVirtualEnvironmentDialog.cpp
    Dialogs/NewEnvironmentDialog.cpp
    Dialogs/EntryPointDialog.cpp
)

set(Editor_MOC_SRCS
    MainWindow.h
    EditorLayer.h

    Widgets/EntityTypeWidget.h
    Widgets/EntityWidget.h
    Widgets/EnvironmentWidget.h
    Widgets/PropertiesWidget.h
    Widgets/TransformationWidget.h

    Dialogs/NewVirtualEnvironmentDialog.h
    Dialogs/NewEnvironmentDialog.h
    Dialogs/EntryPointDialog.h
)

set(Editor_QRC_RESOURCES
    ImageResources.qrc
)

qt4_wrap_cpp(Editor_MOCS ${Editor_MOC_SRCS} OPTIONS -DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED)

qt4_add_resources(Editor_QRCS ${Editor_QRC_RESOURCES} OPTIONS -no-compress)

add_executable(inVRsEditor
    ${Editor_SRCS}
    ${Editor_MOCS}
    ${Editor_QRCS}
)

add_dependencies(inVRsEditor
    inVRsOpenSGSceneGraphInterface
)

target_link_libraries(inVRsEditor
    inVRsOpenSGSceneGraphInterface
    irrXML
    ${QT_QTOPENGL_LIBRARY}
    ${inVRs_LIBRARIES}
    ${QT_LIBRARIES}
)
target_link_libraries(inVRsEditor ${OpenSG_LIBRARIES})

install (TARGETS inVRsEditor
    DESTINATION ${TARGET_BIN_DIR}
)


