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

/*---------------------------------------------------------------------------*\
 *                                Changes                                    *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
\*---------------------------------------------------------------------------*/

//_____________________________________________________________
//
/// \file ViewerApp.h
/// ViewerApp definition.
//_____________________________________________________________
//

#ifndef _DR_VIEWERAPP_H
  #define _DR_VIEWERAPP_H


#include "avatara/Avatar.h"


namespace AvataraViewer
{
  //----------------------------------------------------------
  /// Avatara Viewer Application class.
  /// ViewerApp contains application logic, callback functions
  /// for glut, ...
  /// Class is static-only.
  /// (Only static methods can be callback functions.)
  //----------------------------------------------------------
  class ViewerApp
  {
  public:
    static int Run(int argc, char **argv);

  private:
    static const std::string applicationTitle;  ///< title of application

    static int winWidth;                ///< Width of glut window.
    static int winHeight;               ///< Height of glut window

    static float distance;              ///< Camera distance
    static Algebra::Vector cameraX;     ///< x vector of camera coordinate system
    static Algebra::Vector cameraY;     ///< y vector of camera coordinate system
    static Algebra::Vector cameraZ;     ///< z vector of camera coordinate system

    static bool animate;                ///< flag: animate model
    static bool smoothAnimation;        ///< flag: smooth animation (= interpolation bewteen keyframes)
    static float speedFactor;           ///< Animation speed (0.5 = half the speed, 1 = original, ...)
    static Avatara::DrawType drawType;  ///< draw type for avatar (wireframe,
                                        /// solid or textured)
    static bool drawBones;              ///< flag: draw Skeleton
    static bool drawBoneCoordinates;    ///< flag: draw coordinate crosses of bones
    static bool turnHead;               ///< flag: make model look to target
    static bool moveArm;                ///< flag: turn Arm IK on
    static float bowAngle;              ///< angle of upper torso bowing

    static int mouseX;                  ///< x coordinate of mouse position
    static int mouseY;                  ///< y coordinate of mouse position
    static bool isLeftButtonDown;       ///< Flag: Left mouse button pressed?
    static bool isRightButtonDown;      ///< Flag: Left mouse button pressed?

    static Avatara::Model model;        ///< Model.
    static Avatara::Avatar avatar;      ///< Avatar.
    static Avatara::Texture texture;    ///< Texture.
    static Avatara::Animation anim;     ///< Animation.
    static Avatara::AnimationSet animationSet;      ///< Animation set.
    static bool hasTexture;             ///< true, if texture is available.
    static bool hasAnimation;           ///< true, if animation is available.
    static Algebra::Vector target;      ///< target point to look at and reach for


    // ----- Constructor, Assignment operator and Copy Constructor
    ViewerApp();
    ~ViewerApp();
    ViewerApp(const ViewerApp&);
    ViewerApp& operator=(const ViewerApp&);

    // ----- methods
    static void ExitOnError(bool isError, std::string errorMessage);
    static void InitGlut();
    static void InitApp(int argc, char **argv);
    static void InitGL();

    static void GlutDisplayFunc();
    static void GlutReshapeFunc(int winWidth, int winHeight);
    static void GlutKeyboardFunc (unsigned char key, int x, int y);
    static void GlutSpecialKeyboardFunc(int key, int x, int y);
    static void GlutMouseButtonFunc (int button, int state, int x, int y);
    static void GlutMouseMotionFunc (int x, int y);
    static void GlutIdleFunc();
    static void SetViewingTransformation();
    static void DrawScene();
  };

}
#endif
