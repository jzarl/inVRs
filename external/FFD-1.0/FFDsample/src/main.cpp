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

#include <gmtl/Vec.h>
#include <gmtl/AABox.h>

#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGGLUTWindow.h>
#include <OpenSG/OSGSimpleSceneManager.h>
#include <OpenSG/OSGSceneFileHandler.h>
#include <OpenSG/OSGNode.h>
#include <OpenSG/OSGGroup.h>
#include <OpenSG/OSGSimpleAttachments.h>

// include the OpenSGDeformActionManager for deformation
#include <ffd/OpenSGDeformActionManager.h>

using namespace std;
OSG_USING_NAMESPACE

SimpleSceneManager *mgr;
//NodePtr scene, model1, model2, model;
NodePtr scene, model;
NodePtr chairNodes[7];
int chairIndex = 0;

OpenSGDeformActionManager* osgdam;
bool all = false;
bool doExecute = false;
int ax = 1;
vector<gmtl::Vec3i> selectPoints;
vector<gmtl::Vec3f> selectPositions;

NodePtr createScenegraph()
{
    scene = Node::create();
    setName(scene, "Scene");

    /*
    model1 = SceneFileHandler::the().read("data/high_res_box.wrl");

    if (model1 == NullFC)
    {
        cout << "Unable to load model1" << endl;
        return NullFC;
    }
    setName(model1, "Highresbox");

    model2 = SceneFileHandler::the().read("data/box.wrl");
    if (model2 == NullFC)
    {
        cout << "Unable to load model2" << endl;
        return NullFC;
    }
    setName(model2, "Box");
    */

#if OSG_MAJOR_VERSION >= 2
	model = SceneFileHandler::the()->read("data/char.wrl");
#else
	model = SceneFileHandler::the().read("data/chair.wrl");
#endif 

    if (model == NullFC)
    {
        cout << "Unable to load model1" << endl;
        return NullFC;
    }

    beginEditCP(scene, Node::CoreFieldMask | Node::ChildrenFieldMask);
        scene->setCore(Group::create());
        //scene->addChild(model1);
        //scene->addChild(model2);
        scene->addChild(model);
    endEditCP(scene, Node::CoreFieldMask | Node::ChildrenFieldMask);

    //model = model1;
    return scene;
}

#if OSG_MAJOR_VERSION >= 2
Action::ResultE assignChairNodes(Node* const node)
#else
Action::ResultE assignChairNodes(NodePtr& node)
#endif

{
    TransformPtr tcore = TransformPtr::dcast(node->getCore());
    if (tcore != NullFC)
    {
        if (getName(node))
        {
            std::cout << "Found geometry node: " << getName(node) << endl;
            if (strcmp(getName(node), "Seat") == 0)
                chairNodes[0] = node;
            else if (strcmp(getName(node), "Back") == 0)
                chairNodes[1] = node;
            else if (strcmp(getName(node), "Leg_BR") == 0)
                chairNodes[2] = node;
            else if (strcmp(getName(node), "Leg_BL") == 0)
                chairNodes[3] = node;
            else if (strcmp(getName(node), "Leg_FR") == 0)
                chairNodes[4] = node;
            else if (strcmp(getName(node), "Leg_FL") == 0)
                chairNodes[5] = node;
        }
    }

    return Action::Continue;
}

#if OSG_MAJOR_VERSION >= 2
Action::ResultE printSceneGraph(Node* const node)
#else
Action::ResultE printSceneGraph(NodePtr& node)
#endif
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

void reshape(int w, int h)
{
    mgr->resize(w, h);
    glutPostRedisplay();
}

void display(void)
{
    mgr->idle();
    mgr->redraw();
}

void mouse(int button, int state, int x, int y)
{
    if (state == 1)
        mgr->mouseButtonRelease(button, x, y);
    else
        mgr->mouseButtonPress(button, x, y);
    glutPostRedisplay();
}

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
    cout << "p: print scenegraph" << endl;
    cout << "h: display this help" << endl;
    cout << "--------------------------------------------------" << endl;
}

void keyboard(unsigned char k, int x, int y)
{
    switch(k)
    {
        case 27:
        {
            OpenSGDeformActionManager::Destroy();
            osgExit();
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
            chairIndex = (chairIndex + 1) % 6;
            model = chairNodes[chairIndex];
            cout << "Selected model is: ";
            if (getName(model))
                cout << getName(model);
            cout << " " << model << endl;
            break;
        }
        case '6':
        {
            ax *= -1;
            break;
        }
        case '7':
        {
            if (ax < 0)
                ax = -1;
            else
                ax = 1;
            break;
        }
        case '8':
        {
            if (ax < 0)
                ax = -2;
            else
                ax = 2;
            break;
        }
        case '9':
        {
            if (ax < 0)
                ax = -3;
            else
                ax = 3;
            break;
        }
        case 'p':
        {
#if OSG_MAJOR_VERSION >= 2
			traverse(scene, boost::bind(printSceneGraph,_1));
#else
            traverse(scene, osgTypedFunctionFunctor1CPtrRef
                <Action::ResultE, NodePtr>(printSceneGraph));
#endif 
            break;
        }
        case 'h':
        {
            printUsage();
            break;
        }
        case 'i':
        {
            gmtl::AABoxf aabb;
            osgdam = OpenSGDeformActionManager::GetInstance();
            if (osgdam != 0)
            {
                osgdam->insertLattice(model, 2, 2, 2, 3);
            }
            break;
        }
        case 'j':
        {
            osgdam = OpenSGDeformActionManager::GetInstance();
            if(osgdam != 0)
            {
                gmtl::AABoxf aabb;
                Pnt3f aabbMin, aabbMax;
#if OSG_MAJOR_VERSION >= 2
				model->editVolume(true).getBounds(aabbMin, aabbMax);
#else
				model->getVolume(true).getBounds(aabbMin, aabbMax);
#endif               

                for (int i = 0; i < 3; i++)
                {
                    aabbMin[i] = aabbMin[i] - 1.0f;
                    aabbMax[i] = aabbMax[i]/2 + 1.0f;
                }
                aabb = gmtl::AABoxf(gmtl::Vec3f(aabbMin[0], aabbMin[1], aabbMin[2]),
                                    gmtl::Vec3f(aabbMax[0], aabbMax[1], aabbMax[2]));

                osgdam->insertLattice(model, 2, 2, 2, 3, true, aabb);
            }
            break;
        }
        case 'q':
        {
            NodePtr tmp = model;

            while (tmp != NullFC)
            {
                TransformPtr tcore = TransformPtr::dcast(tmp->getCore());
                if (tcore != NullFC)
                {
                    Matrix mm = tcore->getMatrix();
                    Matrix tm = Matrix::identity();

                    tm.setScale(1,2,1);
                    mm.mult(tm);

                    beginEditCP(tcore, Transform::MatrixFieldMask);
                        tcore->setMatrix(mm);
                    endEditCP(tcore, Transform::MatrixFieldMask);

                    mm = tcore->getMatrix();
                    break;
                }
                tmp = tmp->getParent();
            }
            break;
        }
        default:
            break;
    }
    if (osgdam != 0)
    {
        switch(k)
        {
            case '4':
            {
                all = !all;
                osgdam->setShowAll(model, all);
                break;
            }
            case 'e':
            {
                doExecute = !doExecute;
                osgdam->setInstantExecution(model, doExecute);
                break;
            }
            case 'r':
            {
                osgdam->removeLattice(model);
                break;
            }
            case 't':
            {
                osgdam->taper(model, 1.0f, ax);
                break;
            }
            case 'g':
            {
                osgdam->twist(model, gmtl::Math::deg2Rad(5.0f), ax);
                break;
            }
            case 'b':
            {
                osgdam->bend(model, gmtl::Math::deg2Rad(5.0f), 0.5f, ax);
                break;
            }
            case 'v':
            {
                osgdam->bend(model, -gmtl::Math::deg2Rad(5.0f), 0.0f, ax);
                break;
            }
            case 'c':
            {
                osgdam->bend(model, -gmtl::Math::deg2Rad(5.0f), 1.0f, ax);
                break;
            }
            case 'f':
            {
                osgdam->bend(model, -gmtl::Math::deg2Rad(5.0f), 0.5f, ax);
                break;
            }
            case 'd':
            {
                gmtl::Matrix44f dMatrix;
                dMatrix[0][0] = 0.5f;
                dMatrix[1][1] = 1.0f;
                dMatrix[2][2] = -1.5f;
                osgdam->deform(model, dMatrix, false);
                break;
            }
            case 'l':
            {
                selectPoints.clear();
                selectPositions.clear();
                osgdam->unselectLatticeCellPoints(model);
                break;
            }
            case 'm':
            {
                osgdam->selectLatticeCellPoint(mgr, model, x, y, true);
                break;
            }
            case 'n':
            {
                osgdam->unselectLatticeCellPoint(mgr, model, x, y);
                break;
            }
            case '-':
            {
                gmtl::Vec3f pos;
                selectPositions.clear();
                selectPoints = osgdam->getSelection(model);
                for (size_t i = 0; i < selectPoints.size(); ++i)
                {
                    osgdam->getPoint(model, selectPoints[i], pos);
                    if (ax == 1)
                        pos[0] -= 0.5f;
                    else if (ax == 2)
                        pos[1] -= 0.5f;
                    else
                        pos[2] -= 0.5f;
                    selectPositions.push_back(pos);
                }
                osgdam->setPoints(model, selectPoints, selectPositions);
                break;
            }
            case '+':
            {
                gmtl::Vec3f pos;
                selectPositions.clear();
                selectPoints = osgdam->getSelection(model);
                for (size_t i = 0; i < selectPoints.size(); ++i)
                {
                    osgdam->getPoint(model, selectPoints[i], pos);
                    if (ax == 1)
                        pos[0] += 0.5f;
                    else if (ax == 2)
                        pos[1] += 0.5f;
                    else
                        pos[2] += 0.5f;
                    selectPositions.push_back(pos);
                }
                osgdam->setPoints(model, selectPoints, selectPositions);
                break;
            }
            case 'u':
            {
                osgdam->undo(model);
                break;
            }
            case 'x':
            {
                osgdam->executeFfd(model);
                break;
            }
            case 'o':
            {
                osgdam->dump(model);
                break;
            }
            default:
                break;
        }
    }
}

void motion(int x, int y)
{
    mgr->mouseMove(x, y);
    glutPostRedisplay();
}

void initOpenGL()
{
    glPointSize(5.0f);
    glLineWidth(1.5f);
}

int setupGLUT(int *argc, char *argv[])
{
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE | GLUT_ALPHA);
    int winid = glutCreateWindow("FFD OpenSG");

    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    initOpenGL();
    glutIdleFunc(display);

    return winid;
}

int main(int argc, char **argv)
{
    osgInit(argc, argv);

    int winid = setupGLUT(&argc, argv);
    GLUTWindowPtr gwin= GLUTWindow::create();

#if OSG_MAJOR_VERSION >= 2
	 gwin->setGlutId(winid);
#else
	 gwin->setId(winid);
#endif
   
    gwin->setSize(600, 600);
    gwin->init();
	glutPositionWindow(400, 400);

    scene = createScenegraph();

#if OSG_MAJOR_VERSION >= 2
	 traverse(scene, boost::bind(assignChairNodes,_1));
#else
	 traverse(scene, osgTypedFunctionFunctor1CPtrRef
        <Action::ResultE, NodePtr>(assignChairNodes));
#endif 
   
    chairNodes[6] = model;

    mgr = new SimpleSceneManager;
    mgr->setWindow(gwin);
    mgr->setRoot(scene);

#if OSG_MAJOR_VERSION >= 2
	CameraPtr pc = mgr->getCamera();
    beginEditCP(pc, CameraBase::NearFieldMask | CameraBase::FarFieldMask);
    	pc->setNear(-2000);
    	pc->setFar(2000);
    endEditCP(pc, CameraBase::NearFieldMask | CameraBase::FarFieldMask);
#else //OpenSG1:
	PerspectiveCameraPtr pc = mgr->getCamera();
	
    beginEditCP(pc, PerspectiveCamera::NearFieldMask |
                    PerspectiveCamera::FarFieldMask);
    	pc->setNear(-2000);
    	pc->setFar(2000);
    endEditCP(pc, PerspectiveCamera::NearFieldMask |
                    PerspectiveCamera::FarFieldMask);
#endif

    mgr->showAll();

    Navigator* nav = mgr->getNavigator();
    nav->setFrom(nav->getFrom() + Vec3f(0.0f, 50.0f, 0.0f));

    printUsage();
    glutMainLoop();

    return 0;
}
