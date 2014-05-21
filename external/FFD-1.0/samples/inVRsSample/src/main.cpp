/*---------------------------------------------------------------------------*\
 *           interactive networked Virtual Reality system (inVRs)            *
 *                                                                           *
 *    Copyright (C) 2005-2009 by the Johannes Kepler University, Linz        *
 *                                                                           *
 *                            www.inVRs.org                                  *
 *                                                                           *
 *              contact: canthes@inVRs.org, rlander@inVRs.org                *
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *                                License                                    *
 *                                                                           *
 * This library is free software; you can redistribute it and/or modify it   *
 * under the terms of the GNU Library General Public License as published    *
 * by the Free Software Foundation, version 2.                               *
 *                                                                           *
 * This library is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Library General Public License for more details.                          *
 *                                                                           *
 * You should have received a copy of the GNU Library General Public         *
 * License along with this library; if not, write to the Free Software       *
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 *
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *                           Project: FFD                                    *
 *                                                                           *
 * The FFD library was developed during a practical at the Johannes Kepler   *
 * University, Linz in 2009 by Marlene Hochrieser                            *
 *                                                                           *
\*---------------------------------------------------------------------------*/

#include <iostream>

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
#include <inVRs/SystemCore/Configuration.h>
#include <inVRs/SystemCore/SystemCore.h>
#include <inVRs/SystemCore/Timer.h>
#include <inVRs/SystemCore/WorldDatabase/WorldDatabase.h>
#include <inVRs/SystemCore/EventManager/EventFactory.h>

// FFD includes
#include <inVRs/Modules/Deformation/InVRsDeformActionManager.h>
#include <inVRs/Modules/Deformation/Deformation.h>

OSG_USING_NAMESPACE   // Activate the OpenSG namespace
using std::cout;
using std::endl;
//------------------------------------------------------------------------------
// Global Variables:
//------------------------------------------------------------------------------
SimpleSceneManager *mgr;  // the SimpleSceneManager to manage applications
NetworkInterface* network;            // an interface to the network module
Deformation* deformation;
InVRsDeformActionManager* invrsDam;

float lastTimeStamp;                  // remember the time
Entity* entity;
Entity* entity1;
Entity* entity2;
Lattice* lattice;
bool all = false;
bool instantExecute;
int ax = 1;
float bendFactor, twistFactor, taperFactor, bendCenter, move, epsilon;
int dim, height, width, length;

//------------------------------------------------------------------------------
// Forward Declarations:
//------------------------------------------------------------------------------
int setupGLUT(int *argc, char *argv[]);
void cleanup();
void printUsage();

// The following methods are called by the SystemCore to notify the application
// when CoreComponents, Interfaces or Modules are initialized.
void initModules(ModuleInterface* module);

//------------------------------------------------------------------------------
// The main method
//------------------------------------------------------------------------------
int main(int argc, char **argv)
{
	osgInit(argc, argv);
	int winid = setupGLUT(&argc, argv);

    //printd_severity(WARNING);
	GLUTWindowPtr gwin = GLUTWindow::create();
	gwin->setId(winid);
	gwin->init();

    if (argc > 1)
    {
        if (!Configuration::loadConfig("config/general.xml")) {
            printf("Error: could not load config-file!\n");
            return -1;
        }
        else if (!Configuration::loadConfig("config/generalClient.xml")) {
            printf("Error: could not load config-file!\n");
            return -1;
        }
    }
    else
    {
        if (!Configuration::loadConfig("config/general.xml")) {
            printf("Error: could not load config-file!\n");
            return -1;
        }
    }

	SystemCore::registerModuleInitCallback(initModules);

	std::string systemCoreConfigFile = Configuration::getString(
			"SystemCore.systemCoreConfiguration");
	std::string modulesConfigFile = Configuration::getString(
            "Modules.modulesConfiguration");
	std::string outputInterfaceConfigFile = Configuration::getString(
			"Interfaces.outputInterfaceConfiguration");
	std::string inputInterfaceConfigFile = Configuration::getString(
			"Interfaces.inputInterfaceConfiguration");

	if (!SystemCore::configure(systemCoreConfigFile, outputInterfaceConfigFile,
        inputInterfaceConfigFile, modulesConfigFile)) {
		printf("Error: failed to setup SystemCore!\n");
		return -1;
	}

    bendFactor = Configuration::getFloat("DeformationSample.bend");
    taperFactor = Configuration::getFloat("DeformationSample.taper");
    twistFactor = Configuration::getFloat("DeformationSample.twist");
    bendCenter = Configuration::getFloat("DeformationSample.bendCenter");
    instantExecute = Configuration::getBool("DeformationSample.instantExecution");
    move = Configuration::getFloat("DeformationSample.move");
    epsilon = Configuration::getFloat("DeformationSample.epsilon");
    dim = Configuration::getInt("DeformationSample.dim");
    height = Configuration::getInt("DeformationSample.height");
    width = Configuration::getInt("DeformationSample.width");
    length = Configuration::getInt("DeformationSample.length");

	NodePtr root = Node::create();
	beginEditCP(root);
		root->setCore(Group::create());
        OpenSGSceneGraphInterface* sgIF =
            dynamic_cast<OpenSGSceneGraphInterface*>(OutputInterface::getSceneGraphInterface());
        if (!sgIF) {
            printf("Error: Failed to get OpenSGSceneGraphInterface!\n");
            printf("Please check if the OutputInterface configuration is correct!\n");
            return -1;
        }
		NodePtr scene = sgIF->getNodePtr();
		root->addChild(scene);
    endEditCP(root);

	// set our transformation to the start transformation

	mgr = new SimpleSceneManager;  // create the SimpleSceneManager
	mgr->setWindow(gwin);          // tell the manager what to manage
	mgr->setRoot(root);            // attach the scenegraph to the  root node
	mgr->showAll();                // show the whole scene

	lastTimeStamp = timer.getTime();   // initialize timestamp;

    // try to connect to network first command line argument is {hostname|IP}:port
    if (argc > 1) {
        printf("Trying to connect to %s\n", argv[1]);
        network->connect(argv[1]);
    }

	SystemCore::synchronize();   // synchronize both VEs

    invrsDam = InVRsDeformActionManager::GetInstance();
    if(invrsDam == 0)
        printf("ERROR: Failed to get INVRsDeformActionManager instance!");

    entity = WorldDatabase::getEntityWithEnvironmentId(1, 1);
    if (entity == 0)
        printf("ERROR: Failed to get entity!");

    printUsage();
	glutMainLoop(); // GLUT main loop
	return 0;
}

Action::ResultE printSceneGraph(NodePtr& node)
{
    std::cout << "Node type: ";

    GeometryPtr gcore = GeometryPtr::dcast(node->getCore());
    if (gcore != NullFC)
        std::cout << "Geometry ";
    else
    {
        TransformPtr tcore = TransformPtr::dcast(node->getCore());
        if (tcore != NullFC)
            std::cout << "Transform ";
        else
        {
            GroupPtr core = GroupPtr::dcast(node->getCore());
            if (core != NullFC)
                std::cout << "Group ";
            else
                std::cout << "Unknown ";
        }
    }
    std::cout << ", name: ";
    if (getName(node))
        std::cout << getName(node);

    std::cout << ", ptr: " << node;

    if (node->getParent() != NullFC)
    {
        std::cout << "  => PARENT: ";
        if (getName(node->getParent()))
            std::cout << getName(node->getParent()) << " ";
        std::cout << node->getParent();
    }

    std::cout << std::endl;

    return Action::Continue;
}

void display(void)
{
	SystemCore::step();  //update the system core, needed for event handling

	float currentTimeStamp;
	float dt; // time difference between currentTimestamp and lastTimestamp

	currentTimeStamp = timer.getTime(); //get current time
	dt = currentTimeStamp - lastTimeStamp;

	deformation->step(dt);              // process deformation pipe
	lastTimeStamp = currentTimeStamp;

	mgr->redraw(); // redraw the window
} // display

void reshape(int w, int h) {
	mgr->resize(w, h);
	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
    if (state == 1)
        mgr->mouseButtonRelease(button, x, y);
    else
        mgr->mouseButtonPress(button, x, y);
    glutPostRedisplay();
} // mouse

void motion(int x, int y)
{
    mgr->mouseMove(x, y);
	glutPostRedisplay();
} // motion


void printUsage()
{
    cout << endl;
    cout << "*************** *  USAGE  * *****************" << endl;
    cout << "-------------- render options ---------------" << endl;
    cout << "1: render wireframe" << endl;
    cout << "2: render vertices" << endl;
    cout << "3: render polygons" << endl;
    cout << "4: toggle lattice division rendering" << endl;
    cout << "e: toggle instant execution" << endl;
    cout << "------------- lattice options ---------------" << endl;
    cout << "r: remove lattice" << endl;
    cout << "i: insert lattice by using the AABB" << endl;
    cout << "j: insert masking lattice" << endl;
    cout << "x: execute FFD" << endl;
    cout << "------------- deformation -------------------" << endl;
    cout << "t: taper deformation" << endl;
    cout << "g: twist deformation" << endl;
    cout << "b: bend deformation" << endl;
    cout << "d: global deformation" << endl;
    cout << "----------- lattice point selection ---------" << endl;
    cout << "m: add point to selection" << endl;
    cout << "n: remove point from selection" << endl;
    cout << "l: unselect all points" << endl;
    cout << "--------- lattice point deformation ---------" << endl;
    cout << "+: move all points + 0.5f along the set axis" << endl;
    cout << "-: move all points - 0.5f along the set axis" << endl;
    cout << "-------------- misc settings ----------------" << endl;
    cout << "5: toggle model selection" << endl;
    cout << "6: toggle axis direction" << endl;
    cout << "7: select x-axis" << endl;
    cout << "8: select y-axis" << endl;
    cout << "9: select z-axis" << endl;
    cout << "u: undo last action if possible" << endl;
    cout << "o: dump action deque" << endl;
    cout << "h: display this help" << endl;
    cout << "--------------------------------------------------" << endl;
}

void keyboard(unsigned char k, int x, int y)
{
    switch(k)
    {
        case 27:
        {
            OSG::osgExit();
            cleanup();
            exit(0);
        }
        case '1':
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
        }
        case '2':
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            break;
        }
        case '3':
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
        }
        case '5':
        {
            (entity == entity1) ? (entity = entity2) : (entity = entity1);
            OpenSGModel* osgmodel = dynamic_cast<OpenSGModel *>
                (entity->getVisualRepresentation());

            if (osgmodel != 0)
            {
                cout << "Selected entity is: ";
                if (getName(osgmodel->getNodePtr()))
                    cout << getName(osgmodel->getNodePtr());
                cout << " " << osgmodel << endl;
            }
            break;
        }
        case '6':
        {
            ax *= -1;
            break;
        }
        case '7':
        {
            ax = 1;
            break;
        }
        case '8':
        {
            ax = 2;
            break;
        }
        case '9':
        {
            ax = 3;
            break;
        }
        case 'h':
        {
            printUsage();
            break;
        }
        case 'i':
        {
            invrsDam->insertLattice(entity, height, width, length, dim);
            invrsDam->setInstantExecution(entity, instantExecute);
            break;
        }
        case 'j':
        {
            OpenSGModel* osgmodel = dynamic_cast<OpenSGModel *>
                (entity->getVisualRepresentation());

            if (osgmodel != 0)
            {
                gmtl::AABoxf aabb;
                Pnt3f aabbMin, aabbMax;
                osgmodel->getNodePtr()->getVolume(true).getBounds(aabbMin,
                    aabbMax);

                for (int i = 0; i < 3; i++)
                {
                    aabbMin[i] /= 2;
                    aabbMax[i] /= 2;
                }
                aabb.setMin(gmtl::Vec3f(aabbMin[0], aabbMin[1], aabbMin[2]));
                aabb.setMax(gmtl::Vec3f(aabbMax[0], aabbMax[1], aabbMax[2]));

                invrsDam->insertLattice(entity, height, width, length, dim,
                    true, aabb);
                invrsDam->setInstantExecution(entity, instantExecute);
            }
            break;
        }
        case 'p':
        {
            OpenSGModel* osgmodel = dynamic_cast<OpenSGModel *>
                (entity->getVisualRepresentation());

            traverse(osgmodel->getNodePtr(), osgTypedFunctionFunctor1CPtrRef
                <Action::ResultE, NodePtr>(printSceneGraph));
            break;
        }
        default:
            break;
    }

    if (invrsDam != 0)
    {
        switch(k)
        {
            case '4':
                {
                    all = !all;
                    invrsDam->setShowAll(entity, all);
                    break;
                }
            case 'e':
            {
                instantExecute = !instantExecute;
                invrsDam->setInstantExecution(entity, instantExecute);
                break;
            }
            case 'r':
            {
                invrsDam->removeLattice(entity);
                break;
            }
            case 't':
            {
                invrsDam->taper(entity, taperFactor, ax);
                break;
            }
            case 'g':
            {
                invrsDam->twist(entity, gmtl::Math::deg2Rad(twistFactor), ax);
                break;
            }
            case 'b':
            {
                invrsDam->bend(entity, gmtl::Math::deg2Rad(bendFactor),
                    bendCenter, ax);
                break;
            }
            case 'd':
            {
                gmtl::Matrix44f dMatrix;
                dMatrix[0][0] = 0.5f;
                dMatrix[1][1] = 2.0f;
                dMatrix[2][2] = 1.5f;

                invrsDam->deform(entity, dMatrix, true);
                break;
            }
            case 'l':
            {
                invrsDam->unselectLatticeCellPoints(entity);
                break;
            }
            case 'm':
            {
                invrsDam->selectLatticeCellPoint(mgr, entity, x, y, true);
                break;
            }
            case 'n':
            {
                invrsDam->unselectLatticeCellPoint(mgr, entity, x, y);
                break;
            }
            case '+':
            {
                gmtl::Vec3f pos;
                vector<gmtl::Vec3i> selectPoints = invrsDam->getSelection(entity);
                for (size_t i = 0; i < selectPoints.size(); ++i)
                {
                    invrsDam->getPoint(entity, selectPoints[i], pos);
                    if (ax == 1)
                        pos[0] += move;
                    else if (ax == 2)
                        pos[1] += move;
                    else
                        pos[2] += move;
                    invrsDam->setPoint(entity, selectPoints[i], pos);
                }
                break;
            }
            case '-':
            {
                gmtl::Vec3f pos;
                vector<gmtl::Vec3i> selectPoints = invrsDam->getSelection(entity);
                for (size_t i = 0; i < selectPoints.size(); ++i)
                {
                    invrsDam->getPoint(entity, selectPoints[i], pos);
                    if (ax == 1)
                        pos[0] -= move;
                    else if (ax == 2)
                        pos[1] -= move;
                    else
                        pos[2] -= move;
                    invrsDam->setPoint(entity, selectPoints[i], pos);
                }
                break;
            }
            case 'u':
            {
                invrsDam->undo(entity);
                break;
            }
            case 'x':
            {
                invrsDam->executeFfd(entity);
                break;
            }
            case 'o':
            {
                invrsDam->dump(entity);
                break;
            }
            default:
                break;
        }
    }
} // keyboard

void initOpenGL()
{
    glPointSize(4.0f);
    glLineWidth(1.0f);
}

int setupGLUT(int *argc, char *argv[])
{
	glutInit(argc, argv);
	glutInitDisplayMode(GLUT_RGB| GLUT_DEPTH | GLUT_DOUBLE);
	int winid = glutCreateWindow("inVRs - FFD sample");

    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    initOpenGL();
    glutIdleFunc(display);

	return winid;
} // setupGLUT

void cleanup()
{
    InVRsDeformActionManager::Destroy();
	SystemCore::cleanup();
	osgExit();
}

void initModules(ModuleInterface* module)
{
    if (module->getName() == "Network") {
        network = (NetworkInterface*)module;
    }
	else if (module->getName() == "Deformation") {
	    deformation = (Deformation*)module;
	}
}
