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
 *                           Project: Avatara                                *
 *                                                                           *
 * The Avatara library was developed during a practical at the Johannes      *
 * Kepler University, Linz in 2005 by Helmut Garstenauer                     *
 * (helmut@digitalrune.com) and Martin Garstenauer (martin@digitalrune.com)  *
 *                                                                           *
\*---------------------------------------------------------------------------*/

//_____________________________________________________________
//
/// \file OSGViewer.cpp
/// OpenSG avatar viewer.
/// This file is based on the original OpenSG Tutorial Example
/// "Multiindex Geometry". It displays a moving OSGAvatar.
//_____________________________________________________________
//

#include "avatara/OSGAvatar.h"
#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGSimpleGeometry.h>
#include <OpenSG/OSGGLUTWindow.h>
#include <OpenSG/OSGSimpleSceneManager.h>
#include <OpenSG/OSGBaseFunctions.h>
#include <OpenSG/OSGTransform.h>
#include <OpenSG/OSGGroup.h>
#include <OpenSG/OSGGeometry.h>
#include <OpenSG/OSGImage.h>
#include <inVRs/SystemCore/Platform.h>
#include <iostream>

OSG_USING_NAMESPACE;
using namespace Avatara;
using namespace std;

//___________________________________________________________________________
//
// Function prototypes
//___________________________________________________________________________
//

void InitAvatar();
#if OSG_MAJOR_VERSION >= 2
NodeRecPtr BuildScene();
#else //OpenSG1:
NodePtr BuildScene();
#endif
void PrintUsageInfo();
int setupGLUT( int *argc, char *argv[] );
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void keyboard(unsigned char k, int x, int y);
void display();


//___________________________________________________________________________
//
// Global variables
//___________________________________________________________________________
//

#if OSG_MAJOR_VERSION >= 2
TransformRecPtr trans;         ///<The pointer to the transformation.
SimpleSceneManager::ObjRefPtr mgr;    ///<The SimpleSceneManager to manage simple applications
#else
TransformPtr trans;         ///<The pointer to the transformation.
SimpleSceneManager *mgr;    ///<The SimpleSceneManager to manage simple applications
#endif
OSGAvatar avatar;           ///<OpenSG Avatar.
Model model;                ///<Avatar model.
Animation aniWalk;          ///<Avatar animation "Walk".
Animation aniWave0;         ///<Avatar animation "Waving 0".
Animation aniWave1;          ///<Avatar animation "Waving 1".
Animation aniWave2;          ///<Avatar animation "Waving 2".
AnimationSet animationSet;  ///<Animation set for all avatar animations.


//___________________________________________________________________________
//
// Main
//___________________________________________________________________________
//

int main(int argc, char **argv)
{
	osgInit(argc,argv);                  // OSG init
	int winid = setupGLUT(&argc, argv);  // GLUT init

#if OSG_MAJOR_VERSION >= 2
	// the connection between GLUT and OpenSG
	GLUTWindowRecPtr gwin= GLUTWindow::create();
	gwin->setGlutId(winid);
#else
	// the connection between GLUT and OpenSG
	GLUTWindowPtr gwin= GLUTWindow::create();
	gwin->setId(winid);
#endif
	gwin->init();

	InitAvatar();                        // OSGAvatar init

#if OSG_MAJOR_VERSION >= 2
	mgr = SimpleSceneManager::create();        // create the SimpleSceneManager
#else
	mgr = new SimpleSceneManager;        // create the SimpleSceneManager
#endif
	mgr->setWindow(gwin );               // set the window for the manager
	mgr->setRoot  (BuildScene());        // set the scene for the manager
	mgr->showAll();                      // show the whole scene

	PrintUsageInfo();

	glutMainLoop();                      // GLUT main loop
	return 0;
}


//---------------------------------------------------------
/// Loads and sets avatar data (model, animations).
//---------------------------------------------------------
void InitAvatar()
{
  //
  // load model
  //
  bool success = false;
  success = model.Load("../example/HumanAvatar/HumanHigh.mdl");
  if(!success)
    cerr << "Couldn't load model!" << endl;

  //
  // Load animations
  //
  success = aniWalk.Load("../example/HumanAvatar/Walk.ani");
  if(!success)
    cerr << "Couldn't load animation!" << endl;

  success = aniWave0.Load("../example/HumanAvatar/Waving0.ani");
  if(!success)
    cerr << "Couldn't load animation!" << endl;

  success = aniWave1.Load("../example/HumanAvatar/Waving1.ani");
  if(!success)
    cerr << "Couldn't load animation!" << endl;

  success = aniWave2.Load("../example/HumanAvatar/Waving2.ani");
  if(!success)
    cerr << "Couldn't load animation!" << endl;

  animationSet.Add("Walk", &aniWalk);
  animationSet.Add("Waving0", &aniWave0);
  animationSet.Add("Waving1", &aniWave1);
  animationSet.Add("Waving2", &aniWave2);
  avatar.SetModel(&model);
  avatar.SetAnimations(&animationSet);
  avatar.SetAnimation("Walk");
  avatar.SmoothAnimation(true);
  avatar.SetSpeed(1);
  avatar.StartAnimation();
}


//---------------------------------------------------------
/// Builds a scene with the one node containing the avatar.
/// The avatar texture is loaded here.
/// An animation is set and started.
//---------------------------------------------------------
#if OSG_MAJOR_VERSION >= 2
NodeRecPtr BuildScene()
#else //OpenSG1:
NodePtr BuildScene()
#endif
{
  //
  // load texture
  //
#if OSG_MAJOR_VERSION >= 2
  ImageRecPtr image = Image::create();
#else //OpenSG1:
  ImagePtr image = Image::create();
#endif
  string imageUrl = "../example/HumanAvatar/Human.jpg";
  image->read(imageUrl.c_str());

  //
  // put the avatar geometry into a node
  //
#if OSG_MAJOR_VERSION >= 2
  NodeRecPtr n = Node::create();
#else //OpenSG1:
  NodePtr n = Node::create();
  beginEditCP(n, Node::CoreFieldMask);
  {
#endif
    n->setCore(avatar.MakeAvatarGeo(image));   // make geometry with texture
#if OSG_MAJOR_VERSION >= 2
#else //OpenSG1:
  }
  endEditCP  (n, Node::CoreFieldMask);
#endif

  return n;
}

//---------------------------------------------------------
/// Prints the keybord shortcuts
//---------------------------------------------------------
void PrintUsageInfo() {
  cout << "Keyboard commands:" << endl;
  cout << " 1 .. Start Walk Animation" << endl;
  cout << " 2 .. Start Waving0 Animation" << endl;
  cout << " 3 .. Start Waving1 Animation" << endl;
  cout << " 4 .. Start Waving2 Animation" << endl;
}

//___________________________________________________________________________
//
// GLUT callback functions
//___________________________________________________________________________
//

//---------------------------------------------------------
/// Display function to redraw the scene.
//---------------------------------------------------------
void display( void )
{
  avatar.UpdateAvatarGeo();         // !!!update avatar geometry - don't miss this line!!!
  mgr->redraw();
}


// react to size changes
void reshape(int w, int h)
{
    mgr->resize(w, h);
    glutPostRedisplay();
}

// react to mouse button presses
void mouse(int button, int state, int x, int y)
{
    if (state)
        mgr->mouseButtonRelease(button, x, y);
    else
        mgr->mouseButtonPress(button, x, y);

    glutPostRedisplay();
}

// react to mouse motions with pressed buttons
void motion(int x, int y)
{
    mgr->mouseMove(x, y);
    glutPostRedisplay();
}

// react to keys
void keyboard(unsigned char k, int x, int y)
{
  switch(k)
  {
  case 27:
    OSG::osgExit();
    exit(0);
    break;
  case '1':
    avatar.SetAnimation("Walk", 500);
    break;
  case '2':
    avatar.SetAnimation("Waving0", 500);
    break;
  case '3':
    avatar.SetAnimation("Waving1", 500);
    break;
  case '4':
    avatar.SetAnimation("Waving2", 500);
    break;
  }
}

// setup the GLUT library which handles the windows for us
int setupGLUT(int *argc, char *argv[])
{
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    int winid = glutCreateWindow("OpenSG");

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);

    // call the redraw function whenever there's nothing else to do
    glutIdleFunc(display);

    return winid;
}
