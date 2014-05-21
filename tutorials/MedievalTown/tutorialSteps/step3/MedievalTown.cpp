// GMTL includes
#include <gmtl/Matrix.h>
#include <gmtl/MatrixOps.h>
#include <gmtl/External/OpenSGConvert.h>
// OpenSG includes
#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGGLUTWindow.h>
#include <OpenSG/OSGSimpleSceneManager.h>
// inVRs includes
#include <inVRs/OutputInterface/OutputInterface.h>
#include <inVRs/OutputInterface/SceneGraphNodeInterface.h>
#include <inVRs/OutputInterface/OpenSGSceneGraphInterface/OpenSGSceneGraphInterface.h>
#include <inVRs/InputInterface/InputInterface.h>
#include <inVRs/InputInterface/ControllerManager/ControllerManager.h>
#include <inVRs/InputInterface/ControllerManager/GlutMouseDevice.h>
#include <inVRs/InputInterface/ControllerManager/GlutCharKeyboardDevice.h>
#include <inVRs/SystemCore/Configuration.h>
#include <inVRs/SystemCore/SystemCore.h>
#include <inVRs/SystemCore/Timer.h>
#include <inVRs/SystemCore/TransformationManager/TransformationManager.h>
#include <inVRs/SystemCore/WorldDatabase/WorldDatabase.h>
#include <inVRs/Modules/Navigation/Navigation.h>
#include <inVRs/Modules/Interaction/Interaction.h>
#include <inVRs/tools/libraries/HeightMap/HeightMapModifier.h>
#include <inVRs/tools/libraries/HeightMap/HeightMapManager.h>
#include <inVRs/tools/libraries/Skybox/Skybox.h>
#include <inVRs/tools/libraries/CollisionMap/CheckCollisionModifier.h>

OSG_USING_NAMESPACE   // Activate the OpenSG namespace

//------------------------------------------------------------------------------
// Global Variables:
//------------------------------------------------------------------------------
SimpleSceneManager *mgr;  // the SimpleSceneManager to manage applications
bool grabMouse = true;                // is the mouse stuck in the window ?

float lastTimeStamp;                  // remember the time
User* localUser = NULL;               // our local user
ControllerManager *controllerManager; // the unit handling input devices
Controller* controller;               // an abstract input device
Navigation *navigation;               // the inVRs navigation module
Interaction* interaction;             // the inVRs interaction module

CameraTransformation* camera = NULL;  // the transformation of the camera
gmtl::Matrix44f camMatrix;            // a matrix representing the above

Skybox skybox;                        // scene surroundings

NetworkInterface* network;            // an interface to the network module
AvatarInterface* avatar = NULL;       // an interface to a user representation

float windMillSpeed = 0;              // the rotational speed of a windmill

//------------------------------------------------------------------------------
// Forward Declarations:
//------------------------------------------------------------------------------
// forward declaration so we can have the interesting parts upfront
int setupGLUT(int *argc, char *argv[]);

// forward declaration to cleanup the used modules and databases
void cleanup();

// The following methods are called by the SystemCore to notify the application
// when CoreComponents, Interfaces or Modules are initialized.
void initCoreComponents(CoreComponents comp);
void initInputInterface(ModuleInterface* moduleInterface);
void initModules(ModuleInterface* module);

//------------------------------------------------------------------------------
// The main method
//------------------------------------------------------------------------------
int main(int argc, char **argv) {
	osgInit(argc, argv);                 // initialize OpenSG
	int winid = setupGLUT(&argc, argv);  // initialize GLUT

	// the connection between GLUT and OpenSG is established
	GLUTWindowPtr gwin = GLUTWindow::create();
	gwin->setId(winid);
	gwin->init();

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
// Snippet-2-4 - BEGIN                                                        //
//----------------------------------------------------------------------------//
	// register callbacks
	InputInterface::registerModuleInitCallback(initInputInterface);
	SystemCore::registerModuleInitCallback(initModules);
//----------------------------------------------------------------------------//
// Snippet-2-4 - END                                                          //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-3-3 - BEGIN                                                        //
//----------------------------------------------------------------------------//
	SystemCore::registerCoreComponentInitCallback(initCoreComponents);
//----------------------------------------------------------------------------//
// Snippet-3-3 - END                                                          //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-1-2 - BEGIN                                                        //
//----------------------------------------------------------------------------//
	std::string systemCoreConfigFile = Configuration::getString(
			"SystemCore.systemCoreConfiguration");
	std::string outputInterfaceConfigFile = Configuration::getString(
			"Interfaces.outputInterfaceConfiguration");

//	// !!!!!! Remove in tutorial part 2, Snippet-2-1 - BEGIN
//	if (!SystemCore::configure(systemCoreConfigFile, outputInterfaceConfigFile)) {
//		printf("Error: failed to setup SystemCore!\n");
//		return -1;
//	}
//	// !!!!!! Remove - END
//----------------------------------------------------------------------------//
// Snippet-1-2 - END                                                          //
//----------------------------------------------------------------------------//

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
// Snippet-3-1 - BEGIN                                                        //
//----------------------------------------------------------------------------//
	// generate or load and configure height maps of the used tiles
	HeightMapManager::generateTileHeightMaps();
//----------------------------------------------------------------------------//
// Snippet-3-1 - END                                                          //
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

	NodePtr root = Node::create();
	beginEditCP(root);
		root->setCore(Group::create());

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
// Snippet-2-16 - BEGIN                                                       //
//----------------------------------------------------------------------------//
		// add the SkyBox to the scene
		root->addChild(skybox.getNodePtr());
//----------------------------------------------------------------------------//
// Snippet-2-16 - END                                                         //
//----------------------------------------------------------------------------//

	endEditCP(root);

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

	mgr = new SimpleSceneManager;  // create the SimpleSceneManager
	mgr->setWindow(gwin);          // tell the manager what to manage
	mgr->setRoot(root);            // attach the scenegraph to the  root node
	mgr->showAll();                // show the whole scene
	mgr->getCamera()->setNear(0.1);

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
// Snippet-5-2                                                                //
//----------------------------------------------------------------------------//

	glutMainLoop(); // GLUT main loop
	return 0;
}

void display(void) {

//----------------------------------------------------------------------------//
// Snippet-5-3                                                                //
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
// Snippet-2-17 - BEGIN                                                       //
//----------------------------------------------------------------------------//
	skybox.setupRender(camera->getPosition()); // attach the SkyBox to the camera
//----------------------------------------------------------------------------//
// Snippet-2-17 - END                                                         //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-4-2                                                                //
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
// Snippet-6-1                                                                //
//----------------------------------------------------------------------------//

	mgr->redraw(); // redraw the window
} // display

void reshape(int w, int h) {
	mgr->resize(w, h); 	// react to size changes

//----------------------------------------------------------------------------//
// Snippet-2-9 - BEGIN                                                        //
//----------------------------------------------------------------------------//
	// the mouse device must be aware of the window size in pixel
	GlutMouseDevice::setWindowSize(w, h);
//----------------------------------------------------------------------------//
// Snippet-2-9 - END                                                          //
//----------------------------------------------------------------------------//

	glutPostRedisplay();
}


void mouse(int button, int state, int x, int y) {
//	// !!!!!! Remove in tutorial part 2, Snippet-2-10 - BEGIN
//	// react to mouse button presses
//	if (state) {
//		mgr->mouseButtonRelease(button, x, y);
//	} else {
//		mgr->mouseButtonPress(button, x, y);
//	}
//	// !!!!!! Remove - END

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

	 glutPostRedisplay();
} // mouse

void motion(int x, int y) {

//	// !!!!!! Remove in tutorial part 2, Snippet 2-11 - BEGIN
//	// react to mouse motions with pressed buttons
//	mgr->mouseMove(x, y);
//	// !!!!!! Remove - END

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

	glutPostRedisplay();
} // motion

void keyboard(unsigned char k, int x, int y) {

//----------------------------------------------------------------------------//
// Snippet-2-12 - BEGIN                                                       //
//----------------------------------------------------------------------------//
	// notify keyboard device about GLUT message
	GlutCharKeyboardDevice::cbGlutKeyboard(k, x, y);
//----------------------------------------------------------------------------//
// Snippet-2-12 - END                                                         //
//----------------------------------------------------------------------------//

	// react to keys
	switch (k) {
	// escape-button pressed
	case 27: {
		cleanup();
		exit(0);
	} break;

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

	default: {
	} break;
	} // switch
} // keyboard

void keyboardUp(unsigned char k, int x, int y) {

//----------------------------------------------------------------------------//
// Snippet-2-14 - BEGIN                                                       //
//----------------------------------------------------------------------------//
	GlutCharKeyboardDevice::cbGlutKeyboardUp(k, x, y);
//----------------------------------------------------------------------------//
// Snippet-2-14 - END                                                         //
//----------------------------------------------------------------------------//

} // keyboardUp


int setupGLUT(int *argc, char *argv[]) {
	// setup the GLUT library which handles the windows for us
	glutInit(argc, argv);
	glutInitDisplayMode(GLUT_RGB| GLUT_DEPTH | GLUT_DOUBLE);

	int winid = glutCreateWindow("inVRs Tutorial: Medieval Town");

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUp);
	glutPassiveMotionFunc(motion);
	glutIdleFunc(display);

	return winid;
} // setupGLUT

void cleanup() {

//----------------------------------------------------------------------------//
// Snippet-1-4 - BEGIN                                                        //
//----------------------------------------------------------------------------//
	SystemCore::cleanup(); // clean up SystemCore and registered components
//----------------------------------------------------------------------------//
// Snippet-1-4 - END                                                          //
//----------------------------------------------------------------------------//

	osgExit();
}

//----------------------------------------------------------------------------//
// Snippet-3-2 - BEGIN                                                        //
//----------------------------------------------------------------------------//
void initCoreComponents(CoreComponents comp) {
	// register factory for HeightMapModifier as soon as the
	// TransformationManager is initialized
	if (comp == TRANSFORMATIONMANAGER) {
		TransformationManager::registerModifierFactory
			(new HeightMapModifierFactory());
	// register factory for CheckCollisionModifier
		TransformationManager::registerModifierFactory
			(new CheckCollisionModifierFactory);
	}
}
//----------------------------------------------------------------------------//
// Snippet-3-2 - END                                                          //
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

