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
/// \file Skeleton.cpp
/// Skeleton implementation.
//_____________________________________________________________

#include "avatara/Skeleton.h"
#include "avatara/Debug.h"
#include <iostream>
#include <cmath>

using namespace std;
using namespace Algebra;

namespace Avatara
{

  //-------------------------------------------------------
  /// Initializes empty skeleton object.
  //-------------------------------------------------------
  Skeleton::Skeleton()
  {
    noOfBones = 0;
    pBones = 0;
  }


  //-------------------------------------------------------
  /// Frees resources.
  //-------------------------------------------------------
  Skeleton::~Skeleton()
  {
    UnLoad();
  }


  //-------------------------------------------------------
  /// Sets number of bones.
  //-------------------------------------------------------
  void Skeleton::SetNoOfBones(int noOfBones)
  {
    this->noOfBones = noOfBones;
  }


  //-------------------------------------------------------
  /// Returns number of bones.
  //-------------------------------------------------------
  int Skeleton::GetNoOfBones() const
  {
    return noOfBones;
  }


  //-------------------------------------------------------
  /// Sets array of bones.
  /// Array will be freed in skeleton object!
  ///
  /// \param pBones Bone array.
  //-------------------------------------------------------
  void Skeleton::SetBoneArray(Bone* pBones)
  {
    UnLoad();
    ASSERT(pBones != 0);
    this->pBones = pBones;
  }


  //-------------------------------------------------------
  /// Returns bone with specified id.
  //-------------------------------------------------------
  const Bone & Skeleton::GetBone(int id) const
  {
    ASSERT(0 <= id && id < noOfBones);
    ASSERT(pBones != 0);
    return pBones[id];
  }


  //-------------------------------------------------------
  /// Returns ID of bone with specified name.
  ///
  /// \return ID of bone. -1 if no bone was found.
  //-------------------------------------------------------
  int Skeleton::GetBoneID(string name) const
  {
    ASSERT(pBones != 0);

    int i=0;
    for (i=0; i<noOfBones; i++)
    {
      if (pBones[i].name == name)
        return i;
    }
    PRINT("No bone with name " + name + " found!");
    return -1;
  }


  //-------------------------------------------------------
  /// Frees resources.
  //-------------------------------------------------------
  void Skeleton::UnLoad()
  {
    if(pBones != 0)
    {
      delete [] pBones;
      pBones = 0;
    }
  }


  //-------------------------------------------------------
  /// Calculates derived matrices of bones (offset, inverse).
  //-------------------------------------------------------
  void Skeleton::CalculateMatrices()
  {
    for (int boneID = 0; boneID < noOfBones; boneID++)
    {
      Bone& pBone = pBones[boneID];

      //
      // Compute inverse bone matrix
      //
      pBone.worldToBoneMatrix = pBone.boneToWorldMatrix.Inverse();

      //
      // Compute offset matrix
      //
      if (pBone.parentID == -1)
        pBone.offsetMatrix = pBone.boneToWorldMatrix;
      else
        pBone.offsetMatrix = pBones[pBone.parentID].worldToBoneMatrix * pBone.boneToWorldMatrix;
    }
  }


  //-------------------------------------------------------
  /// Returns the rotation matrix of the bone.
  ///
  /// Each bone defines its own coordinate system.
  /// The position of the coordinates depend on the
  /// coordinates of the bone (head, tail) and a 'roll' value.
  /// BoneRotation returns a rotation matrix which describes
  /// the rotation of the bone-coordinate-system relativ to
  /// its parent bone.
  /// See Avatara.doc for more information and illustrations.
  ///
  /// \param head The head vector.
  /// \param tail The tail vector.
  /// \param roll The roll in radiant.
  /// \return Rotation matrix.
  //-------------------------------------------------------
  Matrix BoneRotation(Vector head, Vector tail, float roll)
  {
    Vector boneVector(tail - head);
    boneVector.Normalize();

    // Axis of rotation
    Vector axis = CrossProduct(UnitVectorY, boneVector);
    Matrix rotation(IdentityMatrix);

    // check if axis exists (crossproduct might be 0!)
    if(axis.Length() > EPSILON)
    {
      // Rotation axis exists
      // calculate rotation
      axis.Normalize();
      float theta = acos(DotProduct(UnitVectorY, boneVector));
      rotation.SetRotation(axis, theta);
    }
    else
    {
      // Rotation axis does not exist.
      // boneVector either points in the same direction as y-unit-vector
      // or in the opposite direction.

      if(DotProduct(UnitVectorY, boneVector) < 0)
      {
        // boneVector points in opposite direction:
        rotation.M(0, 0) = -1.0;
        rotation.M(1, 1) = -1.0;
      }
    }

    // Roll matrix
    Matrix rollMatrix(IdentityMatrix);
    rollMatrix.SetRotation(boneVector, roll);

    return rollMatrix * rotation;
  }


  //-----------------------------------------------------------
  /// Calculates bone to world transformation.
  ///
  /// BoneToWorldMatrix calculates the matrix which transforms a
  /// vertex from the bone space to the world space.
  ///
  /// \param head The head vector (relative to tail of parent; in bone space of parent).
  /// \param tail The tail vector (relative to tail of parent; in bone space of parent).
  /// \param roll The roll value in radiant.
  /// \param pParent The parent bone (null if no parent exists).
  /// \return Transformation matrix.
  //-----------------------------------------------------------
  Matrix ComputeBoneToWorldMatrix(Vector head, Vector tail, float roll, const Bone* pParent)
  {
    //
    // Matrix of parent bone
    //
    Matrix parentMatrix(IdentityMatrix);
    if (pParent != 0)
      parentMatrix = pParent->boneToWorldMatrix;

    //
    // Translate to tail of parent bone.
    // (Remember: Child-bone is always given relative to tail of
    // parent-bone.)
    //
    Matrix lengthTranslation(IdentityMatrix);
    if(pParent != 0)
      lengthTranslation.SetTranslation(Vector(0.0, pParent->length, 0.0));

    //
    // Translate from tail of parent bone to head of current bone
    //
    Matrix rootTranslation(IdentityMatrix);
    rootTranslation.SetTranslation(head);

    //
    // Rotation of bone
    //
    Matrix rotation = BoneRotation(head, tail, roll);

    // Cumulate matrices.
    // Read from right-to-left:
    // (Assume: Vertex is given in coordinate system of current bone.)
    // rootTranslation * rotation ... Transform vertex to coordinates of parent bone
    //                                (relative to tip/joint of parent bone).
    // lengthTranslation ............ Translate vertex to coordinates of parent bone
    //                                (relative to root of parent bone).
    // parentMatrix ................. Transform vertex to world coordinates.
    return parentMatrix * lengthTranslation * rootTranslation * rotation;
  }

} // namespace Avatara

