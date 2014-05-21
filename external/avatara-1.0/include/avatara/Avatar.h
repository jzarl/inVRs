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
 * rlander(rlander@inVRs.org):                                               *
 *  Added statements for dll-support under windows                           *
 *                                                                           *
 *                                                                           *
\*---------------------------------------------------------------------------*/

//_____________________________________________________________
//
/// \file Avatar.h
/// Avatar definition.
//_____________________________________________________________
//

#ifndef _DR_AVATAR_H
  #define _DR_AVATAR_H

#include "avatara/Model.h"
#include "avatara/Texture.h"
#include "avatara/Animation.h"
#include "avatara/AvataraTimer.h"
#include "avatara/AvataraDllExports.h"


namespace Avatara
{
  //-------------------------------------------------------
  /// Specfies how object should be painted.
  /// Wire frame, solid or textured.
  //-------------------------------------------------------
  enum DrawType
  {
    WireFrame = 0,
    Solid = 1,
    Textured = 2
  };


  //-------------------------------------------------------
  /// A 3-dimensional avatar.
  /// A avatar consists of a Model (3d mesh + skeleton),
  /// a texture for the model, and a set of animations.
  /// Multiple Avatars can share the same Model, Texture or
  /// AnimationSet, which means that the Model, Texture and
  /// AnimationSet exist only once in memory.
  ///
  /// Avatar features:
  /// - Mesh skinning (smooth deformations of mesh).
  /// - Smooth animations (interpolation of keyframes).
  /// - Smooth transitions between different animations.
  /// - Control speed of animations.
  /// - Inverse kinematics for right arm.
  /// - Setting the look direction of head.
  /// - Foreward/backward bending of upper body.
  ///
  /// All of these features can function simultaneous.
  /// Example: The avatar can grab and look at a certain point
  /// while walking and bending its upper body.
  //-------------------------------------------------------
  class AVATARA_API Avatar
  {
  public:
    Avatar();
    virtual ~Avatar();

    void SetModel(Model* pModel);
    void SetTexture(Texture* pTexture);
    void SetAnimations(AnimationSet* pAnimationSet);
    void Draw(DrawType drawType);
    void DrawSkeleton(bool drawAxis);
    Algebra::Vector SkinVertex(const Vertex &vertex) const;
    void SetPose(const Pose& pose);
    void SetRestPose();
    void LookTarget(const Algebra::Vector& target);
    void LookVector(const Algebra::Vector& lookVector);
    void TurnHead(bool enable);
    void ArmTarget(const Algebra::Vector& target);
    void MoveArm(bool enable);
    void Bow(float angle);

    void SetHandRotation(float angle);
    float GetHandRotation();
    void SetAnimation(std::string name, int transitionTime = 0);
    void SetAnimation(int index, int transitionTime = 0);
    void SetAnimationTime(int time);
    int GetAnimationTime() const;
    void StartAnimation();
    void StopAnimation();
    void SmoothAnimation(bool enable);
    void SetSpeed(float s);

    const static float maxHeadPitch;      ///< Max. angle for head pitch (up-down).
    const static float maxHeadYaw;        ///< Max. angle for head yaw (left-right).
    const static float maxArmYaw;         ///< Max. angle for arm yaw (=reaching to the left).
    const static float minArmYaw;         ///< Min. angle for arm yaw (=reaching behind).
    const static float maxUpperArmPitch;  ///< Max. pitch for upper arm (max. up movement).
    const static float minUpperArmPitch;  ///< Min. pitch for upper arm (max. down movement).
    const static float maxLowerArmPitch;  ///< Max. pitch for lower arm (max. up movement).
    const static float minLowerArmPitch;  ///< Min. pitch for lower arm (max. down movement).
    const static int   minFrameTime;      ///< Minimal time per frame (e.g. 20 ms => max. 50 fps)


  protected:
    Avatar(const Avatar&);                // forbid use of copy constructor
    Avatar& operator=(const Avatar&);     // forbid use of assignment operator
    void CalculatePose();
    void UpdateAnimationPose();
    void UpdateMatrixStack();
    Algebra::Matrix ComputeBoneMatrix(int boneID) const;
    Algebra::Matrix BonePose(int boneID) const;
    void CalculateHeadPose();
    void CalculateArmPose();
    void CalculateBowPose();
    void FreeMatrixStacks();

    Model* pModel;                        ///< Pointer to model.
    Texture* pTexture;                    ///< Pointer to texture.
    AnimationSet* pAnimationSet;          ///< Pointer to animation set.
    Utilities::Timer timer;               ///< Animation timer.
    const Animation* pCurrentAnimation;   ///< Pointer to current animation.

    bool recalculatePose;                 ///< Flag: Recalculate current pose.
    bool animationRunning;                ///< Flag: Animation is running.
    bool smoothAnimation;                 ///< Flag: Smooth animation (interpolation) enabled.
    int headBoneID;                       ///< ID of head bone.
    Algebra::Vector lookTarget;           ///< Position of look target.
    float handRotation;                   ///< Rotation of right hand
    bool turnHead;                        ///< Flag: Head movement on.
    int upperArmBoneID;                   ///< ID of upper arm bone (right).
    Algebra::Vector armTarget;            ///< Position of target for right hand.
    bool moveArm;                         ///< Flag: Arm IK on.
    int spineBoneID;                      ///< ID of spine bone.
    float bowAngle;                       ///< Angle to bow upper part of body.

    Pose restPose;                        ///< Rest pose of model.
    Pose currentPose;                     ///< Current pose of avatar.
    Algebra::Matrix* pBoneMatrices;       ///< Matrix stack of current pose.
  };

} // namespace Avatara

#endif
