###############################################################################
# Check if avatara is built within inVRs or by itself
# If it is built within inVRs the libraries and headers are installed in
# different locations than when built independently.
###############################################################################
if (NOT INVRS_SOURCE_DIR)
	cmake_minimum_required (VERSION 2.6)
	project (AVATARA)

	#######################################################################
	# Define target paths for installation
	#######################################################################
	set (TARGET_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)
	set (TARGET_LIB_DIR ${CMAKE_INSTALL_PREFIX}/lib)
	set (TARGET_BIN_DIR ${CMAKE_INSTALL_PREFIX}/bin)

	#######################################################################
	# Include user-defined configuration
	#######################################################################
	include (${CMAKE_CURRENT_SOURCE_DIR}/user.cmake OPTIONAL)

else (NOT INVRS_SOURCE_DIR)

	#######################################################################
	# If avatara is built within inVRs then install the libraries and
	# header files in the inVRs target directories
	#######################################################################
	set (TARGET_INCLUDE_DIR ${INVRS_TARGET_INCLUDE_DIR})
	set (TARGET_LIB_DIR ${INVRS_TARGET_LIB_DIR})
	set (TARGET_BIN_DIR ${INVRS_TARGET_BIN_DIR})
endif (NOT INVRS_SOURCE_DIR)

###############################################################################
# Include config.cmake
###############################################################################
include (${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.cmake)

###############################################################################
# Add include directory for avatara
###############################################################################
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

###############################################################################
# Store all source-files in the AVATARA_SRCS variable
###############################################################################
set (LIBAVATARA_SRCS
	src/avatara/Animation.cpp
	src/avatara/Avatar.cpp
	src/avatara/AvataraTimer.cpp
	src/avatara/Bone.cpp
	src/avatara/Debug.cpp
	src/avatara/Matrix.cpp
	src/avatara/Model.cpp
	src/avatara/Pose.cpp
	src/avatara/Quaternion.cpp
	src/avatara/Skeleton.cpp
	src/avatara/Texture.cpp
	src/avatara/Utilities.cpp
	src/avatara/Vector.cpp
	src/avatara/Vertex.cpp)

###############################################################################
# Add OpenSG-specific files if required
###############################################################################
if (AVATARA_ENABLE_OPENSG_SUPPORT)
	set (LIBAVATARA_SRCS ${LIBAVATARA_SRCS}
		src/avatara/OSGAvatar.cpp)
	
	find_package(OpenSG REQUIRED COMPONENTS OSGBase OSGSystem OSGDrawable)
	add_definitions(${OpenSG_DEFINITIONS})
endif (AVATARA_ENABLE_OPENSG_SUPPORT)

###############################################################################
# Set definitions for dlls
###############################################################################
add_definitions (-DAVATARA_EXPORTS)
if(WIN32) #use the math defines e.g. PI when including cmath
	add_definitions(-D_USE_MATH_DEFINES)
endif(WIN32)
###############################################################################
# Build library for avatara
###############################################################################
add_library(avatara SHARED ${LIBAVATARA_SRCS})

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})
target_link_libraries (avatara ${OPENGL_LIBRARIES})

###############################################################################
# Link against OpenSG libraries if needed
###############################################################################
if (AVATARA_ENABLE_OPENSG_SUPPORT)
	
	include_directories(${OpenSG_INCLUDE_DIRS})
	target_link_libraries(avatara ${OpenSG_LIBRARIES})
endif (AVATARA_ENABLE_OPENSG_SUPPORT)

###############################################################################
# Define target folder for avatara library
###############################################################################
if (NOT INVRS_SOURCE_DIR)
	set_target_properties (avatara PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib
		LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib
		RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
endif (NOT INVRS_SOURCE_DIR)


###############################################################################
# Handle building of example applications
###############################################################################
if (AVATARA_BUILD_EXAMPLE_APPLICATIONS)

	###############################################################################
	# Define sources for AvataraViewer
	###############################################################################
	aux_source_directory (src/AvataraViewer AVATARAVIEWER_SRCS)

	find_package(GLUT REQUIRED)
	include_directories(${GLUT_INCLUDE_DIRS})
	add_definitions(${GLUT_DEFINITIONS})
	
	######################################################################
	# Build executables for AvataraViewer
	######################################################################
	add_executable (AvataraViewer ${AVATARAVIEWER_SRCS})

	add_dependencies (AvataraViewer avatara)
	target_link_libraries (AvataraViewer avatara)

	find_package(OpenGL REQUIRED)
	include_directories(${OPENGL_INCLUDE_DIRS})
	target_link_libraries (AvataraViewer ${OPENGL_LIBRARIES})

	target_link_libraries(AvataraViewer ${GLUT_LIBRARIES})

	######################################################################
	# Define target folder for AvataraViewer binary
	######################################################################
	if (NOT INVRS_SOURCE_DIR)
		set_target_properties (AvataraViewer PROPERTIES
			RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
	endif (NOT INVRS_SOURCE_DIR)

	######################################################################
	# Build OSGViewer
	######################################################################
	if (AVATARA_ENABLE_OPENSG_SUPPORT)
		find_package(OpenSG REQUIRED COMPONENTS OSGWindowGLUT OSGUtil)
		add_definitions(${OpenSG_DEFINITIONS})
		include_directories(${OpenSG_INCLUDE_DIRS})
		
		add_executable (OSGViewer example/OpenSGViewer/OSGViewer.cpp)

		add_dependencies (OSGViewer avatara)
		target_link_libraries (OSGViewer avatara)

		target_link_libraries (OSGViewer ${OPENGL_LIBRARIES})
		target_link_libraries(OSGViewer ${GLUT_LIBRARIES})
		target_link_libraries(OSGViewer ${OpenSG_LIBRARIES})

		##############################################################
		# Set output directory
		##############################################################
		if (NOT INVRS_SOURCE_DIR)
			set_target_properties (OSGViewer PROPERTIES
				RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
		endif (NOT INVRS_SOURCE_DIR)

		##############################################################
		# Define installation target
		##############################################################
		install (TARGETS OSGViewer
			DESTINATION ${TARGET_BIN_DIR})
	endif (AVATARA_ENABLE_OPENSG_SUPPORT)

	######################################################################
	# Define installation target
	######################################################################
	install (TARGETS AvataraViewer
		DESTINATION ${TARGET_BIN_DIR})

endif (AVATARA_BUILD_EXAMPLE_APPLICATIONS)

###############################################################################
# Define installation targets
###############################################################################
if (TARGET_INCLUDE_DIR)
	install (DIRECTORY include/avatara
		DESTINATION ${TARGET_INCLUDE_DIR}
		PATTERN ".svn" EXCLUDE)
endif (TARGET_INCLUDE_DIR)

install(
	TARGETS avatara
	ARCHIVE DESTINATION ${TARGET_LIB_DIR}
	LIBRARY DESTINATION ${TARGET_LIB_DIR}
	RUNTIME DESTINATION ${TARGET_BIN_DIR}
)

if (NOT INVRS_SOURCE_DIR)
	configure_file(
		"${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
		"${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake"
		IMMEDIATE @ONLY)
	add_custom_target(uninstall
		"${CMAKE_COMMAND}" -P
		"${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake")
endif (NOT INVRS_SOURCE_DIR)
