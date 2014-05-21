//----------------------------------------------------------------------------//
// Snippet-1-1 - BEGIN                                                        //
//----------------------------------------------------------------------------//
	// very first step: load the configuration of the file structures, basically
	// paths are set. The Configuration always has to be loaded first since each
	// module uses the paths set in the configuration-file
	if (!Configuration::loadConfig("config/general.xml")) {
		printf("Error: could not load config-file!\n");
		return -1;
	}
//----------------------------------------------------------------------------//
// Snippet-1-1 - END                                                          //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-1-2 - BEGIN                                                        //
//----------------------------------------------------------------------------//
	std::string systemCoreConfigFile = Configuration::getString(
			"SystemCore.systemCoreConfiguration");
	std::string outputInterfaceConfigFile = Configuration::getString(
			"Interfaces.outputInterfaceConfiguration");

	// !!!!!! Remove in tutorial part 2, Snippet-2-1 - BEGIN
	if (!SystemCore::configure(systemCoreConfigFile, outputInterfaceConfigFile)) {
		printf("Error: failed to setup SystemCore!\n");
		return -1;
	}
	// !!!!!! Remove - END
//----------------------------------------------------------------------------//
// Snippet-1-2 - END                                                          //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-1-3 - BEGIN                                                        //
//----------------------------------------------------------------------------//
		OpenSGSceneGraphInterface* sgIF =
			dynamic_cast<OpenSGSceneGraphInterface*>(OutputInterface::getSceneGraphInterface());
		if (!sgIF) {
			printf("Error: Failed to get OpenSGSceneGraphInterface!\n");
			printf("Please check if the OutputInterface configuration is correct!\n");
			return -1;
		}
		// retrieve root node of the SceneGraphInterface (method is OpenSG specific)
		NodePtr scene = sgIF->getNodePtr();

		root->addChild(scene);
//----------------------------------------------------------------------------//
// Snippet-1-3 - END                                                          //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-1-4 - BEGIN                                                        //
//----------------------------------------------------------------------------//
	SystemCore::cleanup(); // clean up SystemCore and registered components
//----------------------------------------------------------------------------//
// Snippet-1-4 - END                                                          //
//----------------------------------------------------------------------------//
