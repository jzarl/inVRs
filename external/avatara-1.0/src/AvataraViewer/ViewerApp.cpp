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
/// \file ViewerApp.cpp
/// ViewerApp implementation.
//_____________________________________________________________
//

//___________________________________________________________________________
//
//  Includes
//___________________________________________________________________________
//

#include "ViewerApp.h"
#include "avatara/Debug.h"
#include "avatara/Utilities.h"

#include <cstdlib>
#if defined(MACOS_X) || defined(__APPLE_CC__)
  #include <glut.h>
#else
  #include <GL/glut.h>
#endif

#include <cmath>

using namespace std;
using namespace Avatara;
using namespace Algebra;


//-------------------------------------------------------
/// OpenGL/GLUT-based viewer application.
//-------------------------------------------------------
namespace AvataraViewer
{
  const string ViewerApp::applicationTitle("Avatara Viewer");
  int ViewerApp::winWidth = 800;
  int ViewerApp::winHeight = 800;
  float ViewerApp::distance = 300.0;
  Vector ViewerApp::cameraX = Vector(1.0, 0.0, 0.0);
  Vector ViewerApp::cameraY = Vector(0.0, 1.0, 0.0);
  Vector ViewerApp::cameraZ = Vector(0.0, 0.0, 1.0);
  bool ViewerApp::animate = false;
  bool ViewerApp::smoothAnimation = true;
  float ViewerApp::speedFactor = 1.f;
  DrawType ViewerApp::drawType = Textured;
  bool ViewerApp::drawBones = false;
  bool ViewerApp::drawBoneCoordinates = false;
  bool ViewerApp::turnHead = false;
  bool ViewerApp::moveArm = false;
  float ViewerApp::bowAngle = 0.;
  int ViewerApp::mouseX = 0;
  int ViewerApp::mouseY = 0;
  bool ViewerApp::isLeftButtonDown = false;
  bool ViewerApp::isRightButtonDown = false;
  Model ViewerApp::model;
  Avatar ViewerApp::avatar;
  Texture ViewerApp::texture;
  Animation ViewerApp::anim;
  AnimationSet ViewerApp::animationSet;
  bool ViewerApp::hasTexture = false;
  bool ViewerApp::hasAnimation = false;
  Vector ViewerApp::target(-50, -50, 50);


  //---------------------------------------------------------------
  /// Executes application logic.
  /// Initializes application and graphics and calls glutMainLoop.
  //---------------------------------------------------------------
  int ViewerApp::Run(int argc, char **argv)
  {
    glutInit(&argc, argv);           // initialize glut library
    InitApp(argc, argv);             // application specific initialization
    InitGlut();                      // initialize glut settings
    InitGL();                        // initialize opengl
    glutMainLoop ();                 // enter processing loop

    return 0;
  }


  //---------------------------------------------------------------
  /// Exit program on error.
  /// Prints error message and exits program.
  /// \param isError       IN: true, if error
  /// \param errorMessage  IN: error message
  //---------------------------------------------------------------
  void ViewerApp::ExitOnError(bool isError, string errorMessage)
  {
    if (isError)
    {
      cerr << errorMessage << endl;
      exit(1);
    }
  }


  //---------------------------------------------------------------
  /// Initializes Glut.
  /// Set Glut window and register callback functions.
  //---------------------------------------------------------------
  void ViewerApp::InitGlut()
  {
    // Specify display mode: Double-buffered, RGBA, Depth-Buffer, no Stencil-Buffer
    // no Accumulation-Buffer
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    //
    // Create a window
    //
    glutInitWindowSize (winWidth, winHeight);
    glutInitWindowPosition (0, 0);
    glutCreateWindow (applicationTitle.c_str());

    //
    // Register callback functions
    //
    glutDisplayFunc(GlutDisplayFunc);
    glutReshapeFunc(GlutReshapeFunc);
    glutKeyboardFunc(GlutKeyboardFunc);
    glutSpecialFunc(GlutSpecialKeyboardFunc);
    glutMouseFunc(GlutMouseButtonFunc);
    glutMotionFunc(GlutMouseMotionFunc);
    glutIdleFunc(GlutIdleFunc);
  }


  //---------------------------------------------------------------
  /// Initalizes application.
  /// Writes informational output to console.
  //---------------------------------------------------------------
  void ViewerApp::InitApp(int argc, char **argv)
  {
    cout << "Avatara Viewer " << endl;
    cout << "Copyright (c) 2004 Helmut Garstenauer & Martin Garstenauer" << endl << endl;
    cout << "USAGE: Viewer model texture [animation]" << endl;
    cout << "model ....... A *.mdl file (mandatory parameter)." << endl;
    cout << "texture ..... An uncompressed *.tga file (mandatory parameter)." << endl;
    cout << "animation ... A *.ani file (optional parameter)." << endl << endl;
    cout << "____________________________________________________" << endl << endl;
    cout << "  a ..... toggle animation " << endl;
    cout << "  s ..... toggle smooth animation " << endl;
    cout << "  d ..... change draw type (wire frame, solid, textured)" << endl;
    cout << "  b ..... toggle drawing of bones" << endl;
    cout << "  c ..... toggle drawing of bones coordinate systems" << endl;
    cout << "  r, t, z, h, f, g, h .... move target " << endl;
    cout << "  x, y .. bow" << endl;
    cout << "  l ..... toggle look direction" << endl;
    cout << "  m ..... toggle arm IK" << endl;
    cout << "  +,- ... change animation speed " << endl;
    cout << "  ESC ... quit " << endl;
    cout << "____________________________________________________" << endl << endl;

    //
    // load model
    //
    bool success = false;

    if (argc == 1)
      exit(0);

    if (argc >= 2)
    {
      cout << "Loading model \"" << argv[1] << "\" ...  ";
      success = model.Load(argv[1]);
      ExitOnError(!success, "Couldn't load model!");
      avatar.SetModel(&model);
      cout << "successful." << endl;
    }

    //
    // load texture
    //
    if (argc >= 3)
    {
      cout << "Loading texture \"" << argv[2] << "\" ... ";
      success = texture.LoadUncompressedTGA(argv[2]);
      ExitOnError(!success, "Couldn't load texture!");
      avatar.SetTexture(&texture);
      hasTexture = true;
      cout << "successful." << endl;
    }

    //
    // Load animations
    //
    if (argc >= 4)
    {
      cout << "Loading texture \"" << argv[3] << "\" ... ";
      success = anim.Load(argv[3]);
      ExitOnError(!success, "Couldn't load animation!");
      animationSet.Add("ANI", &anim);

      avatar.SetAnimations(&animationSet);
      avatar.SetAnimation(0, 500);
      avatar.SmoothAnimation(true);
      avatar.SetSpeed(speedFactor);
      hasAnimation = true;
      cout << "successful." << endl;
    }

    avatar.LookTarget(target);
    avatar.ArmTarget(target);
    //avatar.SetHandRotation(0.785);
  }


  //---------------------------------------------------------------
  /// Initializes OpenGL.
  /// Performes OpenGL initialization (lights, rendering, ...)
  //---------------------------------------------------------------
  void ViewerApp::InitGL()
  {
    // ----- set lights

    //
    // light 0
    //
    GLfloat light0_ambient[]  = {0.0, 0.0, 0.0, 1.0};
    GLfloat light0_diffuse[]  = {1.0, 1.0, 1.0, 1.0};
    GLfloat light0_specular[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat light0_position[] = {1.0, 1.0, 1.0, 0.0};
    glLightfv (GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glLightfv (GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv (GL_LIGHT0, GL_SPECULAR, light0_specular);
    glLightfv (GL_LIGHT0, GL_POSITION, light0_position);

    //
    // light 1
    //
    GLfloat light1_ambient[]  = {0.0, 0.0, 0.0, 1.0};
    GLfloat light1_diffuse[]  = {0.7, 0.7, 0.7, 1.0};
    GLfloat light1_specular[] = {0.7, 0.7, 0.7, 1.0};
    GLfloat light1_position[] = {0.0, 0.0, 1.0, 0.0};
    glLightfv (GL_LIGHT1, GL_AMBIENT, light1_ambient);
    glLightfv (GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
    glLightfv (GL_LIGHT1, GL_SPECULAR, light1_specular);
    glLightfv (GL_LIGHT1, GL_POSITION, light1_position);

    //
    // light 2
    //
    GLfloat light2_ambient[]  = {0.0, 0.0, 0.0, 1.0};
    GLfloat light2_diffuse[]  = {0.7, 0.7, 0.7, 1.0};
    GLfloat light2_specular[] = {0.7, 0.7, 0.7, 1.0};
    GLfloat light2_position[] = {1.0, 1.0, -1.0, 0.0};
    glLightfv (GL_LIGHT2, GL_AMBIENT, light2_ambient);
    glLightfv (GL_LIGHT2, GL_DIFFUSE, light2_diffuse);
    glLightfv (GL_LIGHT2, GL_SPECULAR, light2_specular);
    glLightfv (GL_LIGHT2, GL_POSITION, light2_position);


    // ----- set rendering flags

    glFrontFace (GL_CCW);          // clockwise rendering sequence
    glEnable (GL_LIGHTING);       // enable lighting
    glEnable (GL_LIGHT0);         // enable light 0
    glEnable (GL_LIGHT1);         // enable light 1
    glEnable (GL_LIGHT2);         // enable light 2
    glEnable (GL_COLOR_MATERIAL); // enable color material mode
    glEnable (GL_AUTO_NORMAL);    // automatic generation of normals
    // for glut and glu objects
    glEnable (GL_NORMALIZE);      // normalize normals
    glDepthFunc (GL_LESS);        // depth function
    glEnable (GL_DEPTH_TEST);     // use depth test

    // ----- textures
    if (hasTexture)
      texture.BuildGLTexture();
  }


  //---------------------------------------------------------------
  /// GlutDisplay callback function.
  /// Calls DrawScene.
  //---------------------------------------------------------------
  void ViewerApp::GlutDisplayFunc()
  {
    glClearColor (0.3f, 0.3f, 0.3f, 1.0f);  // set background color
    glClear (GL_COLOR_BUFFER_BIT|           // erase color buffer
      GL_DEPTH_BUFFER_BIT);                 // and z buffer

    //
    // set perspective projection matrix
    //
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective (90, 1, 10, 2000);

    // set viewing transformation
    SetViewingTransformation();

    glMatrixMode (GL_MODELVIEW);

    // draw scene
    DrawScene();

    glutSwapBuffers ();
  }


  //---------------------------------------------------------------
  /// GlutReshape callback function.
  /// Handles changes of the window size and position.
  //---------------------------------------------------------------
  void ViewerApp::GlutReshapeFunc(int winWidth, int winHeight)
  {
    ViewerApp::winWidth = winWidth;
    ViewerApp::winHeight = winHeight;
    glViewport(0, 0, winWidth, winHeight);    // adjust viewport
  }


  //-------------------------------------------------------
  /// GlutKeyboard callback function.
  /// Handles key presses.
  /// \param key  IN: ASCII value of key
  /// \param x,y  IN: mouse position (window relative)
  //-------------------------------------------------------
  void ViewerApp::GlutKeyboardFunc(unsigned char key, int /*x*/, int /*y*/)
  {
    Matrix rotation(IdentityMatrix);
    float moveSpeed = 3;
    switch(key)
    {
    case 27:                                        // Escape
      exit(0);
      break;
    case 'a': case 'A':                             // toggle animation
      if (hasAnimation)
      {
        animate=!animate;
        if (animate)
          avatar.StartAnimation();
        else
          avatar.StopAnimation();
        glutPostRedisplay();
      }
      break;
    case 's': case 'S':                             // change animation type:
                                                    // smooth (interpolation) <-> jerky (only keyframes)
      if (hasAnimation)
      {
        smoothAnimation = !smoothAnimation;
        avatar.SmoothAnimation(smoothAnimation);
      }
      break;
    case 'd': case 'D':                             // change draw type
      drawType = static_cast<DrawType> (drawType + 1);
      if (drawType > Textured) drawType = WireFrame;
      glutPostRedisplay();
      break;
    case 'b': case 'B':                             // toggle bones
      drawBones=!drawBones;
      glutPostRedisplay();
      break;
    case 'c': case 'C':                             // toggle bone coordinate system
      drawBoneCoordinates=!drawBoneCoordinates;
      glutPostRedisplay();
      break;
    case 'r': case 'R':                           // move target
      target += Vector(0., -moveSpeed, 0.);
      avatar.LookTarget(target);
      avatar.ArmTarget(target);
      glutPostRedisplay();
      break;
    case 't': case 'T':                           // move target
      target += Vector(0., 0., moveSpeed);
      avatar.LookTarget(target);
      avatar.ArmTarget(target);
      glutPostRedisplay();
      break;
    case 'z': case 'Z':                           // move target
      target += Vector(0., moveSpeed, 0.);
      avatar.LookTarget(target);
      avatar.ArmTarget(target);
      glutPostRedisplay();
      break;
    case 'f': case 'F':                           // move target
      target += Vector(-moveSpeed, 0., 0.);
      avatar.LookTarget(target);
      avatar.ArmTarget(target);
      glutPostRedisplay();
      break;
    case 'g': case 'G':                           // move target
      target += Vector(0., 0., -moveSpeed);
      avatar.LookTarget(target);
      avatar.ArmTarget(target);
      glutPostRedisplay();
      break;
    case 'h': case 'H':                           // move target
      target += Vector(moveSpeed, 0., 0.);
      avatar.LookTarget(target);
      avatar.ArmTarget(target);
      glutPostRedisplay();
      break;
    case 'l': case 'L':                           // toggle head movement
      turnHead = !turnHead;
      avatar.TurnHead(turnHead);
      glutPostRedisplay();
      break;
    case 'm': case 'M':                           // toggle arm IK
      moveArm = !moveArm;
      avatar.MoveArm(moveArm);
      glutPostRedisplay();
      break;
    case 'x': case 'X':                           // bow avatar backward
      bowAngle -= 2. * static_cast<float> (M_PI)/180.;
      avatar.Bow(bowAngle);
      DEBUG(bowAngle);
      glutPostRedisplay();
      break;
    case 'y': case 'Y':                           // bow avatar forward
      bowAngle += 2. * static_cast<float> (M_PI)/180.;
      avatar.Bow(bowAngle);
      DEBUG(bowAngle);
      glutPostRedisplay();
      break;
    case '+':                                     // faster animation
      speedFactor +=0.1;
      avatar.SetSpeed(speedFactor);
      glutPostRedisplay();
      break;
    case '-':                                     // slower animation
      speedFactor -= 0.1;
      if (speedFactor < 0.1) speedFactor = 0.1;
      avatar.SetSpeed(speedFactor);
      glutPostRedisplay();
      break;

    default: break;
    }
  }


  //---------------------------------------------------------------
  ///  GlutSpecialKeyboard callback function.
  ///  Is called when special keys are pressed (F1, Up, Down, ...).
  ///  \param key   IN: code of key (e.g. GLUT_KEY_UP)
  ///  \param x, y  IN: mouse position (window-relative)
  //---------------------------------------------------------------
  void ViewerApp::GlutSpecialKeyboardFunc(int key, int /*x*/, int /*y*/)
  {

    Matrix rotation(IdentityMatrix);

    switch(key)
    {
    case GLUT_KEY_UP:                         // rotate around x axis
      rotation.SetRotation(cameraX, -M_PI/180.0);
      cameraZ = rotation * cameraZ;
      cameraY = rotation * cameraY;
      break;
    case GLUT_KEY_DOWN:                       // rotate around x axis
      rotation.SetRotation(cameraX, M_PI/180.0);
      cameraZ = rotation * cameraZ;
      cameraY = rotation * cameraY;
      break;
    case GLUT_KEY_LEFT:                       // rotate around y axis
      rotation.SetRotation(cameraY, -M_PI/180.0);
      cameraX = rotation * cameraX;
      cameraZ = rotation * cameraZ;
      break;
    case GLUT_KEY_RIGHT:                      // rotate around y axis
      rotation.SetRotation(cameraY, M_PI/180.0);
      cameraX = rotation * cameraX;
      cameraZ = rotation * cameraZ;
      break;
    case GLUT_KEY_PAGE_UP:                    // zoom in
      distance -= 10.0;
      break;
    case GLUT_KEY_PAGE_DOWN:                  // zoom out
      distance += 10.0;
      break;
    case GLUT_KEY_HOME:                       // roll around z axis
      rotation.SetRotation(cameraZ, -M_PI/180.0);
      cameraX = rotation * cameraX;
      cameraY = rotation * cameraY;
      glutPostRedisplay();
      break;
    case GLUT_KEY_END:                        // roll around z axis
      rotation.SetRotation(cameraZ, M_PI/180.0);
      cameraX = rotation * cameraX;
      cameraY = rotation * cameraY;
      glutPostRedisplay();
      break;
    }

    glutPostRedisplay();
  }


  //---------------------------------------------------------------
  ///  GlutMouseButton callback function.
  ///  Handles mouse button clicks.
  ///  \param button  IN: clicked button (GLUT_LEFT_BUTTON,
  ///                      GLUT_RIGHT_BUTTON or GLUT_MIDDLE_BUTTON)
  ///  \param state   IN: press or release? (GLUT_UP or GLUT_DOWN)
  ///  \param x, y    IN: mouse postion (window-relative)
  //---------------------------------------------------------------
  void ViewerApp::GlutMouseButtonFunc (int button, int state, int x, int y)
  {
    if (button == GLUT_LEFT_BUTTON)
    {
      if (state == GLUT_DOWN)
      {
        mouseX = x;
        mouseY = y;
        isLeftButtonDown = true;
      }
      else
      {
        isLeftButtonDown = false;
      }
    }
    else if (button == GLUT_RIGHT_BUTTON)
    {
      if (state == GLUT_DOWN)
      {
        mouseX = x;
        mouseY = y;
        isRightButtonDown = true;
      }
      else
      {
        isRightButtonDown = false;
      }
    }
  }


  //---------------------------------------------------------------
  /// GlutMouseMotion callback function.
  /// Is called when mouse buttons are pressed and mouse cursor moves.
  /// \param x, y   IN: new mouse postion (window-relative)
  //---------------------------------------------------------------
  void ViewerApp::GlutMouseMotionFunc (int x, int y)
  {
    // if mouse is moved with pressed button then scene will be rotated

    //
    // calculate vector of mouse motion
    //
    int xDiff = mouseX-x;
    int yDiff = mouseY-y;

    if (isLeftButtonDown)
    {
      //
      // rotate scene around y-axis if mouse was moved horizontally
      //
      Matrix yRotation(IdentityMatrix);
      yRotation.SetRotation(cameraY, M_PI/180.0*xDiff);
      cameraX = yRotation * cameraX;
      cameraZ = yRotation * cameraZ;

      //
      // rotate scene around x-axis if mouse was moved vertically
      //
      Matrix xRotation(IdentityMatrix);
      xRotation.SetRotation(cameraX, M_PI/180.0*yDiff);
      cameraZ = xRotation * cameraZ;
      cameraY = xRotation * cameraY;

      //
      // normalize camera coordinates to avoid numerical problems
      //
      cameraX.Normalize();
      cameraY.Normalize();
      cameraZ.Normalize();
    }
    else if (isRightButtonDown)
    {
      distance += 2*yDiff;
    }

    glutPostRedisplay();

    //
    // save new mouse coords
    //
    mouseX = x;
    mouseY = y;
  }


  //---------------------------------------------------------------
  /// GlutIdle callback function.
  /// Is called when no events are pending.
  //---------------------------------------------------------------
  void ViewerApp::GlutIdleFunc()
  {
    // TODO: do not redisplay all the time - use timer if
    // necessary.
    glutPostRedisplay();
  }


  //-------------------------------------------------------
  /// Sets viewing transformation.
  //-------------------------------------------------------
  void ViewerApp::SetViewingTransformation()
  {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity ();

    gluLookAt	(cameraZ.X()*distance,
      cameraZ.Y()*distance,
      cameraZ.Z()*distance,
      0.0,
      0.0,
      0.0,
      cameraY.X(),
      cameraY.Y(),
      cameraY.Z());
  }


  //---------------------------------------------------------------
  /// Draws szene.
  //---------------------------------------------------------------
  void ViewerApp::DrawScene()
  {
    //
    // Draw coordinate cross
    //
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    {
      glColor3f(1.0, 0.0, 0.0);
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(300.0, 0.0, 0.0);
      glColor3f(0.0, 1.0, 0.0);
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(0.0, 300.0, 0.0);
      glColor3f(0.0, 0.0, 1.0);
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(0.0, 0.0, 300.0);
    }
    glEnd();
    glEnable(GL_LIGHTING);

    glPushMatrix();
//    glTranslatef(0,0,-100);
    glTranslatef(target.X(), target.Y(), target.Z());
    glColor3f(1., 1., 1.);
    glutSolidSphere(4., 5, 5);
    glPopMatrix();

    //
    // Draw model aspects
    //
    glPushMatrix();
    //glTranslatef(0,0,-100);

    avatar.Draw(drawType);

    if (drawBones)
      avatar.DrawSkeleton(drawBoneCoordinates);

    glPopMatrix();
  }
}
