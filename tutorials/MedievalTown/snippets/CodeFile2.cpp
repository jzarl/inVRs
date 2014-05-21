//----------------------------------------------------------------------------//
// Snippet-2-1 - BEGIN                                                        //
//----------------------------------------------------------------------------//
	// !!!!!! Remove part of Snippet-1-2 (right above)
	// in addition to the SystemCore config file, modules and interfaces config
	// files have to be loaded.
	std::string modulesConfigFile = Configuration::getString(
				"Modules.modulesConfiguration");
	std::string inputInterfaceConfigFile = Configuration::getString(
			"Interfaces.inputInterfaceConfiguration");

	if (!SystemCore::configure(systemCoreConfigFile, outputInterfaceConfigFile, inputInterfaceConfigFile,
			modulesConfigFile)) {
		printf("Error: failed to setup SystemCore!\n");
		printf("Please check if the Plugins-path is correctly set to the inVRs-lib directory in the ");
		printf("'final/config/general.xml' config file, e.g.:\n");
		printf("<path name=\"Plugins\" path=\"/home/guest/inVRs/lib\"/>\n");
		return -1;
	}
//----------------------------------------------------------------------------//
// Snippet-2-1 - END                                                          //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-2-2 - BEGIN                                                        //
//----------------------------------------------------------------------------//
void initInputInterface(ModuleInterface* moduleInterface) {
	// store ControllerManger and the Controller as soon as the ControllerManager
	// is initialized
	if (moduleInterface->getName() == "ControllerManager") {
		controllerManager = (ControllerManager*)moduleInterface;
		controller = (Controller*)controllerManager->getController();
	}
}
//----------------------------------------------------------------------------//
// Snippet-2-2 - END                                                          //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-2-3 - BEGIN                                                        //
//----------------------------------------------------------------------------//
void initModules(ModuleInterface* module) {
	// store the Navigation as soon as it is initialized
	if (module->getName() == "Navigation") {
		navigation = (Navigation*)module;
	}
	//------------------------------------------------------------------------//
	// Snippet-4-1                                                            //
	//------------------------------------------------------------------------//

	//------------------------------------------------------------------------//
	// Snippet-5-1                                                            //
	//------------------------------------------------------------------------//
}
//----------------------------------------------------------------------------//
// Snippet-2-3 - END                                                          //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-2-4 - BEGIN                                                        //
//----------------------------------------------------------------------------//
	// register callbacks
	InputInterface::registerModuleInitCallback(initInputInterface);
	SystemCore::registerModuleInitCallback(initModules);
//----------------------------------------------------------------------------//
// Snippet-2-4 - END                                                          //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-2-5 - BEGIN                                                        //
//----------------------------------------------------------------------------//
	// fetch users camera, it is used to tell the Navigator where we are
	localUser = UserDatabase::getLocalUser();
	if (!localUser) {
		printd(ERROR, "Error: Could not find localUser!\n");
		return -1;
	}

	camera = localUser->getCamera();
	if (!camera) {
		printd(ERROR, "Error: Could not find camera!\n");
		return -1;
	}

	avatar = localUser->getAvatar();
	if (!avatar) {
		printd(ERROR, "Error: Could not find avatar!\n");
		return -1;
	}
	avatar->showAvatar(false);

  // set our transformation to the start transformation
	TransformationData startTrans =
		WorldDatabase::getEnvironmentWithId(1)->getStartTransformation(0);
	localUser->setNavigatedTransformation(startTrans);
//----------------------------------------------------------------------------//
// Snippet-2-5 - END                                                          //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-2-6 - BEGIN                                                        //
//----------------------------------------------------------------------------//
	// Navigator is part of SimpleSceneManager and not of the inVRs framework
	Navigator *nav = mgr->getNavigator();
	nav->setMode(Navigator::NONE);     // turn off the navigator
	lastTimeStamp = timer.getTime();   // initialize timestamp;
	camMatrix = gmtl::MAT_IDENTITY44F; // initial setting of the camera matrix
//----------------------------------------------------------------------------//
// Snippet-2-6 - END                                                          //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-2-7 - BEGIN                                                        //
//----------------------------------------------------------------------------//
	float currentTimeStamp;
	Matrix osgCamMatrix;
	float dt; // time difference between currentTimestamp and lastTimestamp

	currentTimeStamp = timer.getTime(); //get current time
	dt = currentTimeStamp - lastTimeStamp;

	controller->update();    // poll/update associated devices
	navigation->update(dt);	 // update navigation

	// process transformations which belong to the pipes with priority 0x0E000000
	TransformationManager::step(dt, 0x0E000000);

	camera->getCameraTransformation(camMatrix); // get camera transformation
//----------------------------------------------------------------------------//
// Snippet-2-7 - END                                                          //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-2-8 - BEGIN                                                        //
//----------------------------------------------------------------------------//
	set(osgCamMatrix, camMatrix);      // convert gmtl matrix into OpenSG matrix

	Navigator* nav = mgr->getNavigator();
	nav->set(osgCamMatrix);            // plug new camera matrix into navigator

	TransformationManager::step(dt);   // process the remaining pipes

	lastTimeStamp = currentTimeStamp;
//----------------------------------------------------------------------------//
// Snippet-2-8 - END                                                          //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-2-9 - BEGIN                                                        //
//----------------------------------------------------------------------------//
	// the mouse device must be aware of the window size in pixel
	GlutMouseDevice::setWindowSize(w, h);
//----------------------------------------------------------------------------//
// Snippet-2-9 - END                                                          //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-2-10- BEGIN                                                        //
//----------------------------------------------------------------------------//
	// !!!!!! Remove part above
	// instead of calling the SimpleSceneManager we delegate the message to
	// our mouse device
	GlutMouseDevice::cbGlutMouse(button, state, x, y);
//----------------------------------------------------------------------------//
// Snippet-2-10 - END                                                         //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-2-11 - BEGIN                                                       //
//----------------------------------------------------------------------------//
	// !!!!!! Remove part above
	// instead of calling the SimpleSceneManager we delegate the message to
	// our mouse device
	GlutMouseDevice::cbGlutMouseMove(x, y);
//----------------------------------------------------------------------------//
// Snippet-2-11 - END                                                         //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-2-12 - BEGIN                                                       //
//----------------------------------------------------------------------------//
	// notify keyboard device about GLUT message
	GlutCharKeyboardDevice::cbGlutKeyboard(k, x, y);
//----------------------------------------------------------------------------//
// Snippet-2-12 - END                                                         //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-2-13 - BEGIN                                                       //
//----------------------------------------------------------------------------//
	// grab the mouse
	case 'm':
	case 'M': {
		grabMouse = !grabMouse;
		GlutMouseDevice::setMouseGrabbing(grabMouse);
	} break;
//----------------------------------------------------------------------------//
// Snippet-2-13 - END                                                         //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-2-14 - BEGIN                                                       //
//----------------------------------------------------------------------------//
	GlutCharKeyboardDevice::cbGlutKeyboardUp(k, x, y);
//----------------------------------------------------------------------------//
// Snippet-2-14 - END                                                         //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-2-15 - BEGIN                                                       //
//----------------------------------------------------------------------------//
	// generate and configure the SkyBox
	std::string skyPath = Configuration::getPath("Skybox");
	skybox.init(5,5,5, 1000, (skyPath+"lostatseaday/lostatseaday_dn.jpg").c_str(),
		(skyPath+"lostatseaday/lostatseaday_up.jpg").c_str(),
		(skyPath+"lostatseaday/lostatseaday_ft.jpg").c_str(),
		(skyPath+"lostatseaday/lostatseaday_bk.jpg").c_str(),
		(skyPath+"lostatseaday/lostatseaday_rt.jpg").c_str(),
		(skyPath+"lostatseaday/lostatseaday_lf.jpg").c_str());
//----------------------------------------------------------------------------//
// Snippet-2-15 - END                                                         //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-2-16 - BEGIN                                                       //
//----------------------------------------------------------------------------//
		// add the SkyBox to the scene
		root->addChild(skybox.getNodePtr());
//----------------------------------------------------------------------------//
// Snippet-2-16 - END                                                         //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-2-17 - BEGIN                                                       //
//----------------------------------------------------------------------------//
	skybox.setupRender(camera->getPosition()); // attach the SkyBox to the camera
//----------------------------------------------------------------------------//
// Snippet-2-17 - END                                                         //
//----------------------------------------------------------------------------//

