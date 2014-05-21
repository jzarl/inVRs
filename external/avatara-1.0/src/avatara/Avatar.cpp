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
/// \file Avatar.cpp
/// Avatar definition.
//_____________________________________________________________

#include "avatara/Avatar.h"
#include "avatara/Debug.h"
#ifdef WIN32
  #include <windows.h>
#endif
#if defined(MACOS_X) || defined(__APPLE_CC__)
  #include <OpenGL/gl.h>
#else
  #include <GL/gl.h>
#endif
#include <cmath>

using namespace Algebra;
using namespace Utilities;
using namespace std;


//-------------------------------------------------------
/// Rendering and animation of OpenGL-based, 3-dimensional
/// avatars.
//-------------------------------------------------------
namespace Avatara
{
  const float Avatar::maxHeadPitch = static_cast<float> (45.*M_PI/180.);      // up-down
  const float Avatar::maxHeadYaw = static_cast<float> (70.*M_PI/180.);        // left-right
  const float Avatar::maxArmYaw = static_cast<float> (140.*M_PI/180.);        // reaching to the left
  const float Avatar::minArmYaw = static_cast<float> (-55.*M_PI/180.);        // reaching behind
  const float Avatar::maxUpperArmPitch = static_cast<float> (90.*M_PI/180.);  // up
  const float Avatar::minUpperArmPitch = static_cast<float> (-80.*M_PI/180.); // down
  const float Avatar::maxLowerArmPitch = static_cast<float> (160.*M_PI/180.); // up
  const float Avatar::minLowerArmPitch = static_cast<float> (-5.*M_PI/180.);  // down
  const int   Avatar::minFrameTime = 0;  // don't limit fps


  //-------------------------------------------------------
  /// Initializes empty avatar object.
  //-------------------------------------------------------
  Avatar::Avatar() : currentPose()
  {
    pModel = 0;
    pTexture = 0;
    pAnimationSet = 0;
    pCurrentAnimation = 0;
    recalculatePose = false;
    animationRunning = false;
    smoothAnimation = false;
    headBoneID = -1;
    pBoneMatrices = 0;
    upperArmBoneID = -1;
    turnHead = false;
    moveArm = false;
    spineBoneID = -1;
    bowAngle = 0.;
    handRotation = 0.0f;
  }


  //-------------------------------------------------------
  /// Cleans up resources.
  //-------------------------------------------------------
  Avatar::~Avatar()
  {
    FreeMatrixStacks();
  }


  //-------------------------------------------------------
  /// Frees all matrix stacks.
  //-------------------------------------------------------
  void Avatar::FreeMatrixStacks()
  {
    if (pBoneMatrices != 0)
      delete[] pBoneMatrices;
  }


  //-------------------------------------------------------
  /// Sets model for this avatar.
  ///
  /// \param pModel Pointer to model object.
  //-------------------------------------------------------
  void Avatar::SetModel(Model* pModel)
  {
    ASSERT(pModel != 0);

    // free resources
    FreeMatrixStacks();

    this->pModel = pModel;

    int noOfBones = pModel->GetSkeleton().GetNoOfBones();
    if (noOfBones != 0)
    {
      //
      // allocate memory for poses and matrix stacks
      //
      restPose.AllocateBoneKeys(noOfBones);
      currentPose.AllocateBoneKeys(noOfBones);
      pBoneMatrices = new Matrix[noOfBones];

      // currentPose == restPose
      UpdateMatrixStack();

      // save IDs for special bones
      headBoneID = pModel->GetSkeleton().GetBoneID("Head");
      upperArmBoneID = pModel->GetSkeleton().GetBoneID("ArmUp.R");
      spineBoneID = pModel->GetSkeleton().GetBoneID("Spine");
    }
  }


  //-------------------------------------------------------
  /// Sets texture for this avatar.
  ///
  /// \param pTexture Pointer to texture object.
  //-------------------------------------------------------
  void Avatar::SetTexture(Texture* pTexture)
  {
    ASSERT(pTexture != 0);
    this->pTexture = pTexture;
  }


  //-------------------------------------------------------
  /// Sets animation set for this avatar.
  ///
  /// \param pAnimationSet Pointer to animation set object.
  //-------------------------------------------------------
  void Avatar::SetAnimations(AnimationSet* pAnimationSet)
  {
    ASSERT(pAnimationSet != 0);
    this->pAnimationSet = pAnimationSet;
  }


  //-----------------------------------------------------------
  /// Draws avatar.
  ///
  /// \param drawType Wire frame, solid or textured (default).
  //-----------------------------------------------------------
  void Avatar::Draw(DrawType drawType=Textured)
  {
    ASSERT(pModel != NULL)

    //
    // Update pose of model
    //
    CalculatePose();

    //
    // skin vertices and store position vector of skinned vertices
    //
    Vector *pSkinnedVectors = new Vector[pModel->GetNoOfVertices()];
    for(int idx=0; idx<pModel->GetNoOfVertices(); idx++)
      pSkinnedVectors[idx] = SkinVertex(pModel->GetVertex(idx));

    // save texture mode
    GLboolean texturingEnabled = glIsEnabled(GL_TEXTURE_2D);

    //
    // set texture - or not
    //
    if(drawType == Textured && pTexture != 0)
    {
      ASSERT(pTexture->GetTextureID() != 0)
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, pTexture->GetTextureID());
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);//GL_DECAL);
    }
    else
      glDisable(GL_TEXTURE_2D);

    //
    // Draw faces
    //
    if (drawType == Solid || drawType == Textured)
    {
      glBegin(GL_TRIANGLES);
      {
        for(int i=0; i<pModel->GetNoOfFaces(); i++)
        {
          Face face = pModel->GetFace(i);

          //
          // set shading model
          //
          if(face.shading == Flat)
            glShadeModel(GL_FLAT);
          else
            glShadeModel(GL_SMOOTH);

          //
          // draw vertices
          //
          for(int j=0; j<3; j++)
          {
            Vertex vertex = pModel->GetVertex(face.pVertexIndices[j]);
            Vector skinnedPos = pSkinnedVectors[face.pVertexIndices[j]];
            Color col = face.pColors[j];
            TextureCoord tc = face.pTextureCoords[j];

            // use face normals for flat shading and vertex normals for
            // smooth shading
            if (face.shading == Flat)
            {
              glNormal3f(face.normal.X(), face.normal.Y(), face.normal.Z());
            }
            else
            {
              glNormal3f(vertex.Normal().X(), vertex.Normal().Y(), vertex.Normal().Z());
            }

            glColor4f(col.r/255.0f, col.g/255.0f, col.b/255.0f, col.a/255.0f);
            glTexCoord2f(tc.u, tc.v);
            Vector pos = skinnedPos;
            glVertex3f(pos.X(), pos.Y(), pos.Z());
          }
        }
      }
      glEnd();
    }

    //
    // draw lines for wire frame
    //
    if (drawType == WireFrame)
    {
      glDisable(GL_LIGHTING);
      for(int i=0; i<pModel->GetNoOfFaces(); i++)
      {
        glBegin(GL_LINE_LOOP);
        {
          Face face = pModel->GetFace(i);
          Vector pos0 = pSkinnedVectors[face.pVertexIndices[0]];
          Vector pos1 = pSkinnedVectors[face.pVertexIndices[1]];
          Vector pos2 = pSkinnedVectors[face.pVertexIndices[2]];

          glColor4f(1.0, 0.0, 0.0, 1.0);
          glVertex3f(pos0.X(), pos0.Y(), pos0.Z());
          glVertex3f(pos1.X(), pos1.Y(), pos1.Z());
          glVertex3f(pos2.X(), pos2.Y(), pos2.Z());

        }
        glEnd();
      }

      glEnable(GL_LIGHTING);
    }

    //
    // restore texture mode
    //
    if(texturingEnabled)
      glEnable(GL_TEXTURE_2D);
    else
      glDisable(GL_TEXTURE_2D);

    delete [] pSkinnedVectors;
  }


  //-------------------------------------------------------
  /// Draws skeleton of avatar.
  ///
  /// \param drawAxis True if coordinate crosses for bones
  ///                 should be drawn.
  //-------------------------------------------------------
  void Avatar::DrawSkeleton(bool drawAxis)
  {
    const Skeleton& skeleton = pModel->GetSkeleton();

    if (skeleton.GetNoOfBones() <= 0)
      return;

    //
    // draw lines for Bones and points for joints
    //
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glPointSize(6.0);
    for(int i=0; i<skeleton.GetNoOfBones(); i++)
    {
      const Bone& b = skeleton.GetBone(i);

      //
      // calculate pose-transformed coordinates
      //
      Vector head(0.0, 0.0, 0.0);
      Vector tail(b.length, 0.0, 0.0);
      if (pModel->GetVersion() == 1)
        tail.Set(0.0, b.length, 0.0);

      head = pBoneMatrices[i] * head;
      tail = pBoneMatrices[i] * tail;


      if (drawAxis)
      {
        //
        // Draw coordinate cross
        //
        Vector x(15.0, 0.0, 0.0);
        Vector y(0.0, 15.0, 0.0);
        Vector z(0.0, 0.0, 15.0);
        x = pBoneMatrices[i] * x;
        y = pBoneMatrices[i] * y;
        z = pBoneMatrices[i] * z;

        glLineWidth(1.5);
        glBegin(GL_LINES);
        {
          glColor3f(1.0, 0.0, 0.0);
          glVertex3f(head.X(), head.Y(), head.Z());
          glVertex3f(x.X(), x.Y(), x.Z());

          glColor3f(0.0, 1.0, 0.0);
          glVertex3f(head.X(), head.Y(), head.Z());
          glVertex3f(y.X(), y.Y(), y.Z());

          glColor3f(0.0, 0.0, 1.0);
          glVertex3f(head.X(), head.Y(), head.Z());
          glVertex3f(z.X(), z.Y(), z.Z());
        }
        glEnd();
      }

      glLineWidth(3.0);
      glBegin(GL_LINES);
      {
        glColor4f(0.0, 0.0, 1.0, 1.0);
        glVertex3f(head.X(), head.Y(), head.Z());
        glVertex3f(tail.X(), tail.Y(), tail.Z());
      }
      glEnd();

      //
      // Draw point for head joint
      //
      glBegin(GL_POINTS);
      {
        glColor4f(0.0, 1.0, 0.0, 1.0);
        glVertex3f(head.X(), head.Y(), head.Z());
      }
      glEnd();
    }
    glLineWidth(1.0);
    glPointSize(1.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
  }


  //-------------------------------------------------------
  /// Calculates vertex position of posing model.
  ///
  /// \param vertex Vertex to be skinned.
  /// \return Vertex position after skinning process.
  //-------------------------------------------------------
  Vector Avatar::SkinVertex(const Vertex &vertex) const
  {
    // return if model has no bones
    if (pBoneMatrices == 0 || pModel->GetSkeleton().GetNoOfBones() == 0)
      return vertex.Position();

    // skin vertex if is affected by one or more bones
    if (vertex.NoOfBones() > 0)
    {
      // vertex belongs to some bones
      Vector finalVector(0.0, 0.0, 0.0);
      float totalWeight=0.0;

      //
      // Sum up all bone-weights
      //
      for(int i=0; i<vertex.NoOfBones(); i++)
        totalWeight += vertex.GetBoneWeight(i);

      //
      // Apply all bones and sum up weighted results.
      //
      for(int i=0; i<vertex.NoOfBones(); i++)
      {
        Vector v;
        v = pBoneMatrices[vertex.GetBoneIndex(i)]
            * pModel->GetSkeleton().GetBone(vertex.GetBoneIndex(i)).worldToBoneMatrix
            * vertex.Position();

        finalVector += v * vertex.GetBoneWeight(i) / totalWeight;
      }
      return finalVector;
    }
    else
      return vertex.Position();
  }


  //-------------------------------------------------------
  /// Sets current pose.
  ///
  /// \param pose New current pose.
  //-------------------------------------------------------
  void Avatar::SetPose(const Pose& pose)
  {
    currentPose = pose;
    recalculatePose = true;
  }


  //-------------------------------------------------------
  /// Sets current pose to rest pose.
  //-------------------------------------------------------
  void Avatar::SetRestPose()
  {
    currentPose = restPose;
    recalculatePose = true;
  }


  //-------------------------------------------------------
  /// Sets look vector for head movement.
  ///
  /// \param lookVector Direction vector for head.
  //-------------------------------------------------------
  void Avatar::LookVector(const Vector& lookVector)
  {
    // set look target to: root vertex of head-bone + lookVector
    LookTarget(lookVector + pBoneMatrices[headBoneID] * Vector(0., 0., 0));
  }


  //-------------------------------------------------------
  /// Sets look target for head movement.
  ///
  /// \param target Target position for look.
  //-------------------------------------------------------
  void Avatar::LookTarget(const Vector& target)
  {
    if (lookTarget != target)
    {
      lookTarget = target;
      recalculatePose = true;
    }
  }

  //-------------------------------------------------------
  /// Turn head movement on/off.
  ///
  /// \param enable True to turn on.
  //-------------------------------------------------------
  void Avatar::TurnHead(bool enable)
  {
    if (enable != turnHead)
    {
      turnHead = enable;
      recalculatePose = true;
    }
  }


  //-------------------------------------------------------
  /// Sets target for arm IK.
  ///
  /// \param target Target position for arm movement.
  //-------------------------------------------------------
  void Avatar::ArmTarget(const Vector& target)
  {
    if (armTarget != target)
    {
      armTarget = target;
      recalculatePose = true;
    }
  }

  //-------------------------------------------------------
  /// Turn arm IK on/off.
  ///
  /// \param enable True to turn on.
  //-------------------------------------------------------
  void Avatar::MoveArm(bool enable)
  {
    if (enable != moveArm)
    {
      moveArm = enable;
      recalculatePose = true;
    }
  }


  //-------------------------------------------------------
  /// Calculate new current pose.
  //-------------------------------------------------------
  void Avatar::CalculatePose()
  {
    // make calculations if recalculate flag is set or
    // if animation is running
    if (recalculatePose || animationRunning)
    {
      if (animationRunning)
      {
        UpdateAnimationPose();
        UpdateMatrixStack();
      }

      if (bowAngle < -EPSILON || EPSILON < bowAngle)
      {
        CalculateBowPose();
        UpdateMatrixStack();
      }

      if (turnHead)
        CalculateHeadPose();
      if (moveArm)
        CalculateArmPose();
      if (turnHead || moveArm)
        UpdateMatrixStack();

      recalculatePose = false;
    }
  }


  //-------------------------------------------------------
  /// Calculates new animation pose.
  //-------------------------------------------------------
  void Avatar::UpdateAnimationPose()
  {
    ASSERT(pCurrentAnimation != 0);

    // get current animation time
    int time = timer.GetTimeInMilliseconds();
    int animationLength = pCurrentAnimation->GetAnimationLength();

    // check if enough time has elapsed
    if (time - currentPose.time > minFrameTime)
    {
      //
      // update pose
      //
      if (animationLength != 0)
      {
        if (time >= animationLength)
          time %= animationLength;
      }
      timer.SetTimeInMilliseconds(time);

      if (smoothAnimation == false)
      {
        // Jerky animation
        // (jump from key-frame to key-frame)
        currentPose = pCurrentAnimation->GetPrecPose(time);
      }
      else
      {
        //
        // Smooth animation
        //
        const Pose& nextPose = pCurrentAnimation->GetSuccPose(time);

        // consider that animations are cyclic
        // --> nextPose.time is possibly less than currentPose.time
        if (time < currentPose.time)
        {
          // adjust time for cyclic animation
          currentPose.time -= animationLength;
        }
        currentPose.Interpolate(nextPose, time);
      }
    }
  }


  //-------------------------------------------------------
  /// Update PosedBone-To-World matrices for new pose.
  //-------------------------------------------------------
  void Avatar::UpdateMatrixStack()
  {
    int i;
    int noOfBones = pModel->GetSkeleton().GetNoOfBones();

    // calculate bone-to-world-matrix of each bone
    // (from root-bone to last bones).
    for (i = 0; i < noOfBones; i++)
      pBoneMatrices[i] = ComputeBoneMatrix(i);
  }


  //-----------------------------------------------------------
  /// Calculates bone to world transformation including posing.
  ///
  /// BoneToWorldMatrix calculates the matrix which applies a
  /// pose to a point in the coordinate system of the bone (head
  /// joint) and transforms this point back to the coordinate
  /// system of the skeleton/world.
  ///
  /// Precondition:
  /// - BoneToWorldMatrix of parent bone must be computed and
  ///   stored in #pBoneMatrices.
  ///
  /// \param boneID ID of current bone.
  /// \return Transformation matrix.
  //-----------------------------------------------------------
  Matrix Avatar::ComputeBoneMatrix(int boneID) const
  {
    const Bone & bone = pModel->GetSkeleton().GetBone(boneID);

    //
    // Matrix of parent bone
    //
    Matrix parentMatrix(IdentityMatrix);
    if (bone.parentID != -1)
      parentMatrix = pBoneMatrices[bone.parentID];

    //
    // Bone offset to parent
    //
    Matrix offset = bone.offsetMatrix;

    //
    // Pose matrix
    //
    Matrix pose = BonePose(boneID);

    // Cumulate matrices.
    // Read from right-to-left:
    // (Assume: Vertex is given in coordinate system of current bone.)
    // pose ......................... Apply pose to vertex (movement of bone).
    // offset ....................... Apply offset to transform vertex to parent-bone-system.
    // parentMatrix ................. Transform vertex from parent to world coordinates.
    return parentMatrix * offset * pose;
  }


  //-------------------------------------------------------
  /// Returns pose matrix of bone.
  ///
  /// The pose matrix describes the transformation of a bone
  /// from its rest pose to its current pose.
  ///
  /// (Oversimplified: Vertices are given in a rest pose of
  /// the model. To move a vertex according to its bones, we
  /// need to calculate the pose matrix of each bone.)
  ///
  /// \param boneID ID of bone.
  /// \return Pose matrix.
  //-------------------------------------------------------
  Matrix Avatar::BonePose(int boneID) const
  {
    const BoneKey & boneKey = currentPose.GetBoneKey(boneID);

    // Scale bone
    Matrix scale(IdentityMatrix);
    scale.SetScale(boneKey.scale);

    // Rotate bone
    Quaternion q = boneKey.quaternion;
    q.Normalize();
    Matrix rotation = q.RotationMatrix();

    // Translate bone
    Matrix translation(IdentityMatrix);
    translation.SetTranslation(boneKey.location);

    return translation * rotation * scale;
  }


  //-------------------------------------------------------
  /// Calculates head position.
  ///
  /// Head is turned so that it is looking to the specified
  /// target.
  //-------------------------------------------------------
  void Avatar::CalculateHeadPose()
  {
    if (headBoneID == -1)
      return;

    Matrix pose = BonePose(headBoneID);

    // get Head-to-World matrix: We can get this matrix from the posed-bone-to
    // world matrix stack by eliminating the influence of the pose.
    Matrix HeadToWorld = pBoneMatrices[headBoneID] * pose.Inverse();

    // transform the look target into the coordinate system of the bone
    Vector target =  HeadToWorld.Inverse() * lookTarget;

    if (pModel->GetVersion() >= 2)
    {
      // 3dsmax model
      // Axes: Z in viewing direction, X up, Y to right shoulder

      // cancel if target is behind the head
      if (target.Z() < 0)
        return;

      //-----------------------------------------
      // first turn the head left or right: yaw
      //-----------------------------------------

      //
      // look at model from above and eliminate x-coordinate
      // get yaw angle from dot product
      //
      Vector yzPlaneTarget(target.Y(), target.Z(), 0.f);
      yzPlaneTarget.Normalize();
      float yaw = acos(DotProduct(yzPlaneTarget, UnitVectorY));
      // restrict head movement
      if (yaw > maxHeadYaw)
        yaw = maxHeadYaw;
      // change sign of angle if necessary - because dotproduct is always
      // positive
      if (0 < yzPlaneTarget.X())
        yaw = -yaw;
      Matrix yawMatrix(IdentityMatrix);
      yawMatrix.SetRotation(UnitVectorX, yaw);


      //-----------------------------------------
      // then turn the head up or down: pitch
      //-----------------------------------------

      // apply yaw-rotation to the target - so that it lies in the yz-plane
      target = yawMatrix.Inverse() * target;

      //
      // get pitch vector from dot product
      //
      target.Normalize();
      float pitch = acos(DotProduct(target, UnitVectorZ));
      // restrict head movement
      if (pitch > maxHeadPitch)
        pitch = maxHeadPitch;
      // change sign of angle if necessary - because dotproduct is always
      // positive
      if (target.X() < 0)
        pitch = -pitch;
      Matrix pitchMatrix(IdentityMatrix);
      pitchMatrix.SetRotation(UnitVectorY, pitch);

      //
      // set new boneKey for head: apply yaw and pitch rotation
      //
      BoneKey headKey = currentPose.GetBoneKey(headBoneID);
      headKey.quaternion = yawMatrix * pitchMatrix;
      currentPose.SetBoneKey(headBoneID, headKey);

    }
    else // (pModel->GetVersion() == 1)
    {

      // Blender model
      // Axes: Y in viewing direction, Z up, X to right soulder

      // cancel if target is behind the head
      if (target.Y() < 0)
        return;

      //-----------------------------------------
      // first turn the head left or right: yaw
      //-----------------------------------------

      //
      // look at model from above and eliminate z-coordinate
      // get yaw angle from dot product
      //
      Vector xyPlaneTarget(target.X(), target.Y(), 0.f);
      xyPlaneTarget.Normalize();
      float yaw = acos(DotProduct(xyPlaneTarget, UnitVectorY));
      // restrict head movement
      if (yaw > maxHeadYaw)
        yaw = maxHeadYaw;
      // change sign of angle if necessary - because dotproduct is always
      // positive
      if (0 < xyPlaneTarget.X())
        yaw = -yaw;
      Matrix yawMatrix(IdentityMatrix);
      yawMatrix.SetRotation(UnitVectorZ, yaw);


      //-----------------------------------------
      // then turn the head up or down: pitch
      //-----------------------------------------

      // apply yaw-rotation to the target - so that it lies in the yz-plane
      target = yawMatrix.Inverse() * target;

      //
      // get pitch vector from dot product
      //
      target.Normalize();
      float pitch = acos(DotProduct(target, UnitVectorY));
      // restrict head movement
      if (pitch > maxHeadPitch)
        pitch = maxHeadPitch;
      // change sign of angle if necessary - because dotproduct is always
      // positive
      if (target.Z() < 0)
        pitch = -pitch;
      Matrix pitchMatrix(IdentityMatrix);
      pitchMatrix.SetRotation(UnitVectorX, pitch);

      //
      // set new boneKey for head: apply yaw and pitch rotation
      //
      BoneKey headKey = currentPose.GetBoneKey(headBoneID);
      headKey.quaternion = yawMatrix * pitchMatrix;
      currentPose.SetBoneKey(headBoneID, headKey);

    }
  }


  //-------------------------------------------------------
  /// Calculates Arm position.
  ///
  /// Bone keys for upper arm and lower arm are calculated
  /// with inverse kinematics (IK) to position the right
  /// hand at a target.
  /// See Avatara.doc for more information.
  //-------------------------------------------------------
  void Avatar::CalculateArmPose()
  {
    if (upperArmBoneID == -1)
      return;

    // get ID of lower arm bone
    int lowerArmBoneID = upperArmBoneID + 1;
    int handBoneID = lowerArmBoneID + 1;

    Matrix pose = BonePose(upperArmBoneID);

    // get Upperarm-to-World matrix: We can get this matrix from the posed-bone-to
    // world matrix stack by eliminating the influence of the pose.
    Matrix upperArmToWorld = pBoneMatrices[upperArmBoneID] * pose.Inverse();

     // transform the target into the coordinate system of the bone
    Vector target =  upperArmToWorld.Inverse() * armTarget;

    if (pModel->GetVersion() >= 2)
    {
      // 3dsmax model
      // Axes: X along bone, Z up, Y backwards

      //---------------------------------------------
      // first turn the upper arm left or right: yaw
      // (rotation of arm around Y-axis of upperarm)
      //---------------------------------------------

      //
      // look at model from above and eliminate Y-coordinate
      // get yaw angle from dot product
      //
      Vector zxPlaneTarget(target.Z(), target.X(), 0.f);
      zxPlaneTarget.Normalize();
      float yaw = acos(DotProduct(zxPlaneTarget, UnitVectorY));
      // change sign of angle if necessary - because dotproduct is always
      // positive
      if (0 < zxPlaneTarget.X())
        yaw = -yaw;

      //
      // restrict head movement
      //
      if (yaw < minArmYaw)
        yaw = minArmYaw;
      if (maxArmYaw < yaw)
        yaw = maxArmYaw;

      Matrix yawMatrix(IdentityMatrix);
      yawMatrix.SetRotation(UnitVectorY, yaw);


      //-----------------------------------------
      // IK calculations
      //-----------------------------------------

      // apply yaw-rotation to the target - so that it lies in the xy-plane
      target = yawMatrix.Inverse() * target;
      float x = target.X();
      float y = target.Y();

      //
      // calculate lengths of arm bones
      //
      Bone uArmBone = pModel->GetSkeleton().GetBone(upperArmBoneID);
      Bone lArmBone = pModel->GetSkeleton().GetBone(lowerArmBoneID);
      Bone handBone = pModel->GetSkeleton().GetBone(handBoneID);
      float uArmLen = uArmBone.length;
      float lArmLen = lArmBone.length;
      float handLen = handBone.length;

      // add half hand length to lower arm
      // this approximation works if lower arm and hand bone are in one line
      lArmLen += handLen/2;

      // limit movement to the y-positive space
      if (x <= 0)
        x = 1;      // y = 0 could lead to divide-by-zero exception

      // check if target is far away and out of reach
      if ((x*x+y*y) > (uArmLen+lArmLen)*(uArmLen+lArmLen))
      {
        // target is out of reach: calculate a target in the same
        // direction which can be reached.

        float targetAngle = atan(y/x);
        x = cos(targetAngle) * (uArmLen+lArmLen);
        y = sin (targetAngle) * (uArmLen+lArmLen);
        // limit movement to the y-positive space
        if (x <= 0)
          x = 1;
      }

      // check if target is to close and out of reach
      // this is only possible if upper arm is longer than lower arm
      if ((uArmLen>lArmLen) && (x*x+y*y) < (uArmLen-lArmLen)*(uArmLen-lArmLen))
      {
        // target is out of reach: calculate a target in the same
        // direction which can be reached.

        float targetAngle = atan(y/x);
        x = cos(targetAngle) * (uArmLen-lArmLen);
        y = sin (targetAngle) * (uArmLen-lArmLen);
        // limit movement to the y-positive space
        if (x <= 0)
          x = 1;
      }

      // calculate arm angle of lower arm
      float lArmAngle = acos((x*x+y*y-uArmLen*uArmLen-lArmLen*lArmLen)/
        (2*uArmLen*lArmLen));

      // check for numerical problems: acos returns wrong value if
      // numerator (Z�hler) and denominator (Nenner) of fraction are
      // nearly equal
      if ((x*x+y*y-uArmLen*uArmLen-lArmLen*lArmLen)
        > (2*uArmLen*lArmLen))
        lArmAngle = 0;
      else if ((x*x+y*y-uArmLen*uArmLen-lArmLen*lArmLen)
        < -(2*uArmLen*lArmLen))
        lArmAngle = static_cast<float> (M_PI);

      // calculate arm angle of upper arm
      float uArmAngle = atan(y/x) -
        acos((lArmLen*lArmLen-uArmLen*uArmLen-(x*x+y*y))/
        (-2*uArmLen*sqrt(x*x+y*y)) );

      // check for numerical problems: acos returns wrong value if
      // numerator (Z�hler) and denominator (Nenner) of fraction are
      // nearly equal
      if ((lArmLen*lArmLen-uArmLen*uArmLen-(x*x+y*y))
        > (2*uArmLen*sqrt(x*x+y*y)))
        uArmAngle = atan(x/y);
      else if ((lArmLen*lArmLen-uArmLen*uArmLen-(x*x+y*y))
        < -(2*uArmLen*sqrt(x*x+y*y)))
        uArmAngle = atan(y/x);

      //
      // limit movement of arm
      //
      if (uArmAngle < minUpperArmPitch)
        uArmAngle = minUpperArmPitch;
      if (maxUpperArmPitch < uArmAngle)
        uArmAngle = maxUpperArmPitch;
      if (lArmAngle < minLowerArmPitch)
        lArmAngle = minLowerArmPitch;
      if (maxLowerArmPitch < lArmAngle)
        lArmAngle = maxLowerArmPitch;


      //
      // set new bone key for upper arm
      //
      Matrix uArmMatrix(IdentityMatrix);
      uArmMatrix.SetRotation(UnitVectorZ, uArmAngle);
      BoneKey upperArmKey = currentPose.GetBoneKey(upperArmBoneID);
      upperArmKey.quaternion = yawMatrix * uArmMatrix;
      currentPose.SetBoneKey(upperArmBoneID, upperArmKey);

      //
      // set new bone key for lower arm
      //
      Matrix lArmMatrix(IdentityMatrix);
      lArmMatrix.SetRotation(UnitVectorZ, lArmAngle);
      BoneKey lowerArmKey = currentPose.GetBoneKey(lowerArmBoneID);
      lowerArmKey.quaternion = lArmMatrix ;
      currentPose.SetBoneKey(lowerArmBoneID, lowerArmKey);

      BoneKey handKey = currentPose.GetBoneKey(handBoneID);
      handKey.quaternion = Quaternion(UnitVectorX, handRotation);
      currentPose.SetBoneKey(handBoneID, handKey);

    }
    else // (pModel->GetVersion() == 1)
    {

      // Blender model
      // Axes: Y along bone, Z up, X backwards

      //---------------------------------------------
      // first turn the upper arm left or right: yaw
      // (rotation of arm around z-axis of upperarm)
      //---------------------------------------------

      //
      // look at model from above and eliminate z-coordinate
      // get yaw angle from dot product
      //
      Vector xyPlaneTarget(target.X(), target.Y(), 0.f);
      xyPlaneTarget.Normalize();
      float yaw = acos(DotProduct(xyPlaneTarget, UnitVectorY));
      // change sign of angle if necessary - because dotproduct is always
      // positive
      if (0 < xyPlaneTarget.X())
        yaw = -yaw;

      //
      // restrict head movement
      //
      if (yaw < minArmYaw)
        yaw = minArmYaw;
      if (maxArmYaw < yaw)
        yaw = maxArmYaw;

      Matrix yawMatrix(IdentityMatrix);
      yawMatrix.SetRotation(UnitVectorZ, yaw);


      //-----------------------------------------
      // IK calculations
      //-----------------------------------------

      // apply yaw-rotation to the target - so that it lies in the yz-plane
      target = yawMatrix.Inverse() * target;
      float y = target.Y();
      float z = target.Z();

      //
      // calculate lengths of arm bones
      //
      Bone uArmBone = pModel->GetSkeleton().GetBone(upperArmBoneID);
      Bone lArmBone = pModel->GetSkeleton().GetBone(lowerArmBoneID);
      Bone handBone = pModel->GetSkeleton().GetBone(handBoneID);
      float uArmLen = uArmBone.length;
      float lArmLen = lArmBone.length;
      float handLen = handBone.length;

      // add half hand length to lower arm
      // this approximation works if lower arm and hand bone are in one line
      lArmLen += handLen/2;

      // limit movement to the y-positive space
      if (y <= 0)
        y = 1;      // y = 0 could lead to divide-by-zero exception

      // check if target is far away and out of reach
      if ((y*y+z*z) > (uArmLen+lArmLen)*(uArmLen+lArmLen))
      {
        // target is out of reach: calculate a target in the same
        // direction which can be reached.

        float targetAngle = atan(z/y);
        y = cos(targetAngle) * (uArmLen+lArmLen);
        z = sin (targetAngle) * (uArmLen+lArmLen);
        // limit movement to the y-positive space
        if (y <= 0)
          y = 1;
      }

      // check if target is to close and out of reach
      // this is only possible if upper arm is longer than lower arm
      if ((uArmLen>lArmLen) && (y*y+z*z) < (uArmLen-lArmLen)*(uArmLen-lArmLen))
      {
        // target is out of reach: calculate a target in the same
        // direction which can be reached.

        float targetAngle = atan(z/y);
        y = cos(targetAngle) * (uArmLen-lArmLen);
        z = sin (targetAngle) * (uArmLen-lArmLen);
        // limit movement to the y-positive space
        if (y <= 0)
          y = 1;
      }

      // calculate arm angle of lower arm
      float lArmAngle = acos((y*y+z*z-uArmLen*uArmLen-lArmLen*lArmLen)/
        (2*uArmLen*lArmLen));

      // check for numerical problems: acos returns wrong value if
      // numerator (Z�hler) and denominator (Nenner) of fraction are
      // nearly equal
      if ((y*y+z*z-uArmLen*uArmLen-lArmLen*lArmLen)
        > (2*uArmLen*lArmLen))
        lArmAngle = 0;
      else if ((y*y+z*z-uArmLen*uArmLen-lArmLen*lArmLen)
        < -(2*uArmLen*lArmLen))
        lArmAngle = static_cast<float> (M_PI);

      // calculate arm angle of upper arm
      float uArmAngle = atan(z/y) -
        acos((lArmLen*lArmLen-uArmLen*uArmLen-(y*y+z*z))/
        (-2*uArmLen*sqrt(y*y+z*z)) );

      // check for numerical problems: acos returns wrong value if
      // numerator (Z�hler) and denominator (Nenner) of fraction are
      // nearly equal
      if ((lArmLen*lArmLen-uArmLen*uArmLen-(y*y+z*z))
        > (2*uArmLen*sqrt(y*y+z*z)))
        uArmAngle = atan(z/y);
      else if ((lArmLen*lArmLen-uArmLen*uArmLen-(y*y+z*z))
        < -(2*uArmLen*sqrt(y*y+z*z)))
        uArmAngle = atan(z/y);

      //
      // limit movement of arm
      //
      if (uArmAngle < minUpperArmPitch)
        uArmAngle = minUpperArmPitch;
      if (maxUpperArmPitch < uArmAngle)
        uArmAngle = maxUpperArmPitch;
      if (lArmAngle < minLowerArmPitch)
        lArmAngle = minLowerArmPitch;
      if (maxLowerArmPitch < lArmAngle)
        lArmAngle = maxLowerArmPitch;


      //
      // set new bone key for upper arm
      //
      Matrix uArmMatrix(IdentityMatrix);
      uArmMatrix.SetRotation(UnitVectorX, uArmAngle);
      BoneKey upperArmKey = currentPose.GetBoneKey(upperArmBoneID);
      upperArmKey.quaternion = yawMatrix * uArmMatrix;
      currentPose.SetBoneKey(upperArmBoneID, upperArmKey);

      //
      // set new bone key for lower arm
      //
      Matrix lArmMatrix(IdentityMatrix);
      lArmMatrix.SetRotation(UnitVectorX, lArmAngle);
      BoneKey lowerArmKey = currentPose.GetBoneKey(lowerArmBoneID);
      lowerArmKey.quaternion = lArmMatrix ;
      currentPose.SetBoneKey(lowerArmBoneID, lowerArmKey);

      BoneKey handKey = currentPose.GetBoneKey(handBoneID);
      handKey.quaternion = Quaternion(UnitVectorY, handRotation);
      currentPose.SetBoneKey(handBoneID, handKey);
    }
  }


  //-------------------------------------------------------
  /// Bow upper part of body.
  ///
  /// Positive angle -> lean forward.
  /// Negative angle -> lean back.
  ///
  /// \param angle Bow angle in radians.
  //-------------------------------------------------------
  void Avatar::Bow(float angle)
  {
    bowAngle = angle;
    recalculatePose = true;
  }


  //-------------------------------------------------------
  /// Calculates bow pose.
  /// Add bow of upper body to current pose.
  //-------------------------------------------------------
  void Avatar::CalculateBowPose()
  {
    if (spineBoneID == -1)
      return;

    //
    // get bonekey for spine and apply rotation
    //
    if (pModel->GetVersion() > 1)
    {
      // 3dsmax model
      BoneKey spineKey = currentPose.GetBoneKey(spineBoneID);
      spineKey.quaternion.Set(UnitVectorY, -bowAngle);
      currentPose.SetBoneKey(spineBoneID, spineKey);
    }
    else // (pModel->GetVersion() == 1)
    {
      // Blender Model
      BoneKey spineKey = currentPose.GetBoneKey(spineBoneID);
      spineKey.quaternion.Set(UnitVectorX, bowAngle);
      currentPose.SetBoneKey(spineBoneID, spineKey);
    }
  }


  //-------------------------------------------------------
  /// Sets current animation.
  ///
  /// Animation will begin after the transition time.
  /// (The avatar is smoothly interpolated from the current pose
  /// to the first pose (key frame) of the next animation.
  /// Transition time specifies the duration of this interpolation.
  ///
  /// By default transition time is 0, which means that there
  /// is no interpolation.)
  ///
  /// \param name Name of animation.
  /// \param transitionTime Time for transition.
  //-------------------------------------------------------
  void Avatar::SetAnimation(string name, int transitionTime)
  {
    ASSERT(pAnimationSet != 0);
    pCurrentAnimation = pAnimationSet->GetAnimation(name);
    SetAnimationTime(-transitionTime);
    ASSERT(pCurrentAnimation != 0);
  }

  //-------------------------------------------------------
  /// Sets current animation.
  ///
  /// Animation will begin after the transition time.
  /// (The avatar is smoothly interpolated from the current pose
  /// to the first pose (key frame) of the next animation.
  /// Transition time specifies the duration of this interpolation.
  ///
  /// By default transition time is 0, which means that there
  /// is no interpolation.)
  ///
  /// \param index Index of animation in animation set.
  /// \param transitionTime Time for transition.
  //-------------------------------------------------------
  void Avatar::SetAnimation(int index, int transitionTime)
  {
    ASSERT(pAnimationSet != 0);
    pCurrentAnimation = pAnimationSet->GetAnimation(index);
    SetAnimationTime(-transitionTime);
    ASSERT(pCurrentAnimation != 0);
  }


  //-------------------------------------------------------
  /// Sets animation time.
  ///
  /// Time of current pose and time of animation timer will
  /// be set to #time.
  /// (In this way we can jump to a certain frame in an
  /// animation.)
  ///
  /// \param time Time in ms.
  //-------------------------------------------------------
  void Avatar::SetAnimationTime(int time)
  {
    currentPose.time = time;
    timer.SetTimeInMilliseconds(time);
  }


  //-------------------------------------------------------
  /// Returns current animation time.
  ///
  /// \return Time in ms.
  //-------------------------------------------------------
  int Avatar::GetAnimationTime() const
  {
    return timer.GetTimeInMilliseconds();
  }


  //-------------------------------------------------------
  /// Starts/resumes animation.
  /// (Animation time is not changed.)
  //-------------------------------------------------------
  void Avatar::StartAnimation()
  {
    ASSERT(pCurrentAnimation != 0);
    animationRunning = true;
    timer.Start();
  }


  //-------------------------------------------------------
  /// Stops/pauses animation.
  /// (Animation time is not changed.)
  //-------------------------------------------------------
  void Avatar::StopAnimation()
  {
    animationRunning = false;
    timer.Stop();
  }


  //-------------------------------------------------------
  /// Turns smooth animation on/off.
  ///
  /// Smooth animation means that the model is interpolated
  /// between the keyframes of an animation.
  /// Turned off 'smooth animation' will result in a jerky,
  /// in case there aren't enough key-frames per animation.
  ///
  /// \param enable True to turn on.
  //-------------------------------------------------------
  void Avatar::SmoothAnimation(bool enable)
  {
    smoothAnimation = enable;
  }


  //-------------------------------------------------------
  /// Set speed of animation.
  /// Examples:
  /// -  1.0 ... original speed.
  /// -  0.5 ... half speed.
  /// -  2.0 ... double speed.
  ///
  /// \param s Speed factor.
  //-------------------------------------------------------
  void Avatar::SetSpeed(float s)
  {
    timer.SetSpeed(s);
  }


  //-------------------------------------------------------
  /// Sets angle of hand rotation.
  /// \param angle Angle of rotation [rad]
  //-------------------------------------------------------
  void Avatar::SetHandRotation(float angle)
  {
    handRotation = angle;
  }


  //-------------------------------------------------------
  /// Gets angle of hand rotation.
  /// \return Angle of rotation [rad].
  //-------------------------------------------------------
  float Avatar::GetHandRotation()
  {
    return handRotation;
  }
}
