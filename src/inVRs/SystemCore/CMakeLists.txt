include (../InputInterface/build.cmake)
include (../OutputInterface/build.cmake)
aux_source_directory(. SYSTEMCORE_SRCS)
aux_source_directory(ComponentInterfaces COMPONENTINTERFACES_SRCS)
aux_source_directory(EventManager EVENTMANAGER_SRCS)
aux_source_directory(TransformationManager TRANSFORMATIONMANAGER_SRCS)
aux_source_directory(UserDatabase USERDATABASE_SRCS)
aux_source_directory(WorldDatabase WORLDDATABASE_SRCS)

set (TARGET_INCLUDE_DIR ${INVRS_TARGET_INCLUDE_DIR}/inVRs)
set (TARGET_LIB_DIR ${INVRS_TARGET_LIB_DIR})
set (TARGET_BIN_DIR ${INVRS_TARGET_BIN_DIR})

set(ALL_SRCS
${SYSTEMCORE_SRCS} ${INPUTINTERFACE_SRCS} ${OUTPUTINTERFACE_SRCS}
${COMPONENTINTERFACES_SRCS} ${EVENTMANAGER_SRCS}
${TRANSFORMATIONMANAGER_SRCS} ${USERDATABASE_SRCS}
${WORLDDATABASE_SRCS})

add_definitions (-DINVRSSYSTEMCORE_EXPORTS)

find_package(OpenSG REQUIRED COMPONENTS OSGBase OSGSystem)
include_directories(${OpenSG_INCLUDE_DIRS})
add_definitions(${OpenSG_DEFINITIONS})

add_library(inVRsSystemCore SHARED ${ALL_SRCS})
add_dependencies (inVRsSystemCore irrXML)
target_link_libraries(inVRsSystemCore irrXML)

target_link_libraries(inVRsSystemCore ${OpenSG_LIBRARIES})

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})
target_link_libraries(inVRsSystemCore ${OPENGL_LIBRARIES})

if (WIN32)
	target_link_libraries(inVRsSystemCore Ws2_32.lib)
endif (WIN32)
install (FILES ApplicationBase.h
		ArgumentVector.h
		BSpline.h
		ClassFactory.h
		CommandLineArguments.h
		Configuration.h
#		CursorRepresentation.h
		DataTypes.h
		DebugOutput.h
		IdPool.h
		IdPoolListener.h
		IdPoolManager.h
		MessageFunctions.h
		ModuleIds.h
		NetMessage.h
		Platform.h
		ProfilingHelper.h
		RequestListener.h
		SyncPipe.h
		SystemCore.h
		SystemCoreEvents.h
		Timer.h
		UtilityFunctions.h
		XmlAttribute.h
		XmlConfigurationConverter.h
		XmlConfigurationLoader.h
		XmlDocument.h
		XmlDtdReference.h
		XmlElement.h
		XMLTools.h
	DESTINATION ${TARGET_INCLUDE_DIR}/SystemCore)

install (FILES ComponentInterfaces/AvatarInterface.h
		ComponentInterfaces/CursorRepresentationInterface.h
		ComponentInterfaces/InteractionInterface.h
		ComponentInterfaces/ModuleInterface.h
		ComponentInterfaces/NavigationInterface.h
		ComponentInterfaces/NetworkInterface.h
	DESTINATION ${TARGET_INCLUDE_DIR}/SystemCore/ComponentInterfaces)

install (FILES EventManager/AbstractEventFactory.h
		EventManager/Event.h
		EventManager/EventFactory.h
		EventManager/EventManager.h
	DESTINATION ${TARGET_INCLUDE_DIR}/SystemCore/EventManager)

install (FILES TransformationManager/ApplyCursorTransformationModifier.h
		TransformationManager/ApplyNavigationModifier.h
		TransformationManager/ApplyUserTransformationModifier.h
		TransformationManager/AssociatedEntityInterrupter.h
		TransformationManager/AvatarTransformationWriter.h
		TransformationManager/CameraTransformationWriter.h
		TransformationManager/CursorTransformationWriter.h
		TransformationManager/EntityTransformationWriter.h
		TransformationManager/MultiPipeInterrupter.h
		TransformationManager/TargetPipeTransformationWriter.h
		TransformationManager/TrackingDataWriter.h
		TransformationManager/TrackingOffsetModifier.h
		TransformationManager/TransformationDistributionModifier.h
		TransformationManager/TransformationLoggerModifier.h
		TransformationManager/TransformationManager.h
		TransformationManager/TransformationManagerEvents.h
		TransformationManager/TransformationMerger.h
		TransformationManager/TransformationModifier.h
		TransformationManager/TransformationModifierFactory.h
		TransformationManager/TransformationPipe.h
		TransformationManager/TransformationPipeMT.h
		TransformationManager/UserTransformationWriter.h
	DESTINATION ${TARGET_INCLUDE_DIR}/SystemCore/TransformationManager)

install (FILES UserDatabase/CursorTransformationModel.h
		UserDatabase/GoGoCursorModel.h
		UserDatabase/HeadPositionUserTransformationModel.h
		UserDatabase/InteractionCursorRepresentation.h
		UserDatabase/SimpleCursorRepresentation.h
		UserDatabase/User.h
		UserDatabase/UserDatabase.h
		UserDatabase/UserDatabaseEvents.h
		UserDatabase/UserTransformationModel.h
		UserDatabase/VirtualHandCursorModel.h
	DESTINATION ${TARGET_INCLUDE_DIR}/SystemCore/UserDatabase)

install (FILES WorldDatabase/AttachmentKey.h
		WorldDatabase/CameraTransformation.h
		WorldDatabase/Entity.h
		WorldDatabase/EntityType.h
		WorldDatabase/EntityTypeFactory.h
		WorldDatabase/Environment.h
		WorldDatabase/SimpleAvatar.h
		WorldDatabase/Tile.h
		WorldDatabase/WorldDatabase.h
		WorldDatabase/WorldDatabaseEvents.h
	DESTINATION ${TARGET_INCLUDE_DIR}/SystemCore/WorldDatabase)

install (TARGETS inVRsSystemCore
	ARCHIVE DESTINATION ${TARGET_LIB_DIR}
	LIBRARY DESTINATION ${TARGET_LIB_DIR}
	RUNTIME DESTINATION ${TARGET_BIN_DIR}
)

if (INVRS_ENABLE_TESTING)
	add_subdirectory(unittests)
endif (INVRS_ENABLE_TESTING)
