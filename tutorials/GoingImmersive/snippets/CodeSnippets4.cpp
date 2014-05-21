//----------------------------------------------------------------------------//
// Snippet-4-1 - BEGIN                                                        //
//----------------------------------------------------------------------------//
	int numberOfSensors;
//----------------------------------------------------------------------------//
// Snippet-4-1 - END                                                          //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-4-2 - BEGIN                                                        //
//----------------------------------------------------------------------------//
		ControllerInterface* controller = controllerManager->getController();
		if (!controller) {
			printd(ERROR,
					"GoingImmersive::initialize(): unable to obtain controller! Check ControllerManager configuration!\n");
			return false;
		} // if
		numberOfSensors = controller->getNumberOfSensors();

		// create an instance of the coordinate system entity (ID=10) for each
		// sensor in the environment with ID 1
		for (int i=0; i < numberOfSensors; i++) {
			WorldDatabase::createEntity(10, 1);
		} // for
//----------------------------------------------------------------------------//
// Snippet-4-2 - END                                                          //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-4-3 - BEGIN                                                        //
//----------------------------------------------------------------------------//
	void updateCoordinateSystems() {
		TransformationData trackedUserTrans, sensorTrans;
		// get the list of coordinate system entities (entity type ID = 10)
		EntityType* coordinateSystemType = WorldDatabase::getEntityTypeWithId(10);
		const std::vector<Entity*>& entities = coordinateSystemType->getInstanceList();

		// map the tracked user transformation to the first entity
		if (entities.size() > 0) {
			trackedUserTrans = localUser->getTrackedUserTransformation();
			trackedUserTrans.position += COORDINATE_SYSTEM_CENTER;
			entities[0]->setEnvironmentTransformation(trackedUserTrans);
		} // if
		// map the tracked sensor transformations to the remaining entities
		for (int i=0; i < numberOfSensors; i++) {
			sensorTrans = localUser->getTrackedSensorTransformation(i);
			sensorTrans.position += COORDINATE_SYSTEM_CENTER;
			if (i+1 < entities.size()) {
				entities[i+1]->setEnvironmentTransformation(sensorTrans);
			} // if
		} // for
	} // updateCoordinateSystems
//----------------------------------------------------------------------------//
// Snippet-4-3 - END                                                          //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-4-4 - BEGIN                                                        //
//----------------------------------------------------------------------------//
		updateCoordinateSystems();
//----------------------------------------------------------------------------//
// Snippet-4-4 - END                                                          //
//----------------------------------------------------------------------------//
