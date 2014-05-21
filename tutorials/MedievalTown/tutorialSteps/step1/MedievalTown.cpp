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
// Snippet-2-4                                                                //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-3-3                                                                //
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
// Snippet-2-1                                                                //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-3-1                                                                //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-2-15                                                               //
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
// Snippet-2-16                                                               //
//----------------------------------------------------------------------------//

	endEditCP(root);

//----------------------------------------------------------------------------//
// Snippet-2-5                                                                //
//----------------------------------------------------------------------------//

	mgr = new SimpleSceneManager;  // create the SimpleSceneManager
	mgr->setWindow(gwin);          // tell the manager what to manage
	mgr->setRoot(root);            // attach the scenegraph to the  root node
	mgr->showAll();                // show the whole scene
	mgr->getCamera()->setNear(0.1);

//----------------------------------------------------------------------------//
// Snippet-2-6                                                                //
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
// Snippet-2-7                                                                //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-2-17                                                               //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-4-2                                                                //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-2-8                                                                //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-6-1                                                                //
//----------------------------------------------------------------------------//

	mgr->redraw(); // redraw the window
} // display

void reshape(int w, int h) {
	mgr->resize(w, h); 	// react to size changes

//----------------------------------------------------------------------------//
// Snippet-2-9                                                                //
//----------------------------------------------------------------------------//

	glutPostRedisplay();
}


void mouse(int button, int state, int x, int y) {
	// !!!!!! Remove in tutorial part 2, Snippet-2-10 - BEGIN
	// react to mouse button presses
	if (state) {
		mgr->mouseButtonRelease(button, x, y);
	} else {
		mgr->mouseButtonPress(button, x, y);
	}
	// !!!!!! Remove - END


//----------------------------------------------------------------------------//
// Snippet-2-10                                                               //
//----------------------------------------------------------------------------//

	 glutPostRedisplay();
} // mouse

void motion(int x, int y) {

	// !!!!!! Remove in tutorial part 2, Snippet 2-11 - BEGIN
	// react to mouse motions with pressed buttons
	mgr->mouseMove(x, y);
	// !!!!!! Remove - END


//----------------------------------------------------------------------------//
// Snippet-2-11                                                                //
//----------------------------------------------------------------------------//

	glutPostRedisplay();
} // motion

void keyboard(unsigned char k, int x, int y) {

//----------------------------------------------------------------------------//
// Snippet-2-12                                                               //
//----------------------------------------------------------------------------//

	// react to keys
	switch (k) {
	// escape-button pressed
	case 27: {
		cleanup();
		exit(0);
	} break;

//----------------------------------------------------------------------------//
// Snippet-2-13                                                               //
//----------------------------------------------------------------------------//

	default: {
	} break;
	} // switch
} // keyboard

void keyboardUp(unsigned char k, int x, int y) {

//----------------------------------------------------------------------------//
// Snippet-2-14                                                               //
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
// Snippet-3-2                                                                //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-2-2                                                                //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-2-3                                                                //
//----------------------------------------------------------------------------//

