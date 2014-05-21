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

//_______________________________________________________
//
/// \file Pose.cpp
/// Description of pose of a skeleton and bones.
//_______________________________________________________

#include "avatara/Pose.h"
#include "avatara/Debug.h"

using namespace Algebra;

namespace Avatara
{

  //-------------------------------------------------------
  /// Initializes bone key (no rotation, no translation, no
  /// scaling).
  //-------------------------------------------------------
  BoneKey::BoneKey()
  {
    // quaterion is unit-quaternion with no rotation
    // location = (0, 0, 0)
    scale = Vector(1.0, 1.0, 1.0);
  }


  //-------------------------------------------------------
  /// Destructor.
  //-------------------------------------------------------
  BoneKey::~BoneKey()
  {
  }


  //-------------------------------------------------------
  /// Initializes bone key with certain rotation, translation,
  /// and scaling.
  //-------------------------------------------------------
  BoneKey::BoneKey(const Quaternion & quat, const Vector & loc, const Vector & s)
  {
    Set(quat, loc, s);
  }


  //-------------------------------------------------------
  /// Set elements to specified values.
  //-------------------------------------------------------
  void BoneKey::Set(const Quaternion & quat, const Vector & loc, const Vector & s)
  {
    quaternion = quat;
    location = loc;
    scale = s;
  }


  //-------------------------------------------------------
  /// Set elements equal to given bone key.
  //-------------------------------------------------------
  void BoneKey::Set(const BoneKey& boneKey)
  {
    quaternion = boneKey.quaternion;
    location = boneKey.location;
    scale = boneKey.scale;
  }

  //-------------------------------------------------------
  /// Copy-Constructor.
  //-------------------------------------------------------
  BoneKey::BoneKey(const BoneKey & bk)
  {
    Set(bk);
  }

  //-------------------------------------------------------
  /// Assignment operator.
  /// Set elements equal to given bone key.
  //-------------------------------------------------------
  BoneKey& BoneKey::operator=(const BoneKey& boneKey)
  {
    if (this == &boneKey)
      return *this;

    Set(boneKey);
    return *this;
  }


  //-------------------------------------------------------
  /// Interpolate between this pose and target pose.
  /// \param target   target (final) pose.
  /// \param u        interpolation factor [0...1]
  //-------------------------------------------------------
  void BoneKey::Interpolate(const BoneKey& target, float u)
  {
    // interpolate scale-vector
    scale = target.scale * u + scale * (1.0f - u);

    // interpolate location
    location = target.location * u + location * (1.0f - u);

    // interploate rotation
    quaternion = quaternion.Slerp(target.quaternion, u);
  }




  //-------------------------------------------------------
  /// Initializes an empty pose.
  //-------------------------------------------------------
  Pose::Pose()
  {
    noOfBones = 0;
    time = 0;
    pBoneKeys = 0;
  }


  //-------------------------------------------------------
  /// Copy-constructor.
  //-------------------------------------------------------
  Pose::Pose(const Pose& pose)
  {
    Set(pose);
  }


  //-------------------------------------------------------
  /// Destructor.
  /// Frees allocated memory for array of bone keys.
  //-------------------------------------------------------
  Pose::~Pose()
  {
    if (pBoneKeys != NULL)
      delete[] pBoneKeys;
  }


  //-------------------------------------------------------
  /// Allocates new array of bone keys.
  /// Old bone keys are deleted.
  /// \param n  Number of bone keys.
  //-------------------------------------------------------
  void Pose::AllocateBoneKeys(int n)
  {
    ASSERT(n>0);
    if (pBoneKeys != NULL)
    {
      delete[] pBoneKeys;
      pBoneKeys = NULL;
    }

    noOfBones = n;
    pBoneKeys = new BoneKey[n];
  }


  //-------------------------------------------------------
  /// Returns bone key.
  /// \param boneID   Index of bone.
  //-------------------------------------------------------
  const BoneKey& Pose::GetBoneKey(int boneID) const
  {
    ASSERT(boneID < noOfBones);
    ASSERT(pBoneKeys != 0);
    return pBoneKeys[boneID];
  }


  //-------------------------------------------------------
  /// Set bone key.
  /// \param boneID   Index of bone.
  /// \param boneKey  Key, which will be set.
  //-------------------------------------------------------
  void Pose::SetBoneKey(int boneID, const BoneKey & boneKey)
  {
    ASSERT(boneID < noOfBones);
    ASSERT(pBoneKeys != 0);
    pBoneKeys[boneID] = boneKey;
  }


  //-------------------------------------------------------
  /// Set certain pose.
  //-------------------------------------------------------
  void Pose::Set(const Pose& pose)
  {
    time = pose.time;
    noOfBones = pose.noOfBones;

    if (noOfBones == 0)
    {
      if (pBoneKeys != NULL)
      {
        delete[] pBoneKeys;
        pBoneKeys = NULL;
      }
      return;
    }

    AllocateBoneKeys(noOfBones);

    // Copy bone-keys
    int i;
    for (i = 0; i < noOfBones; i++)
      pBoneKeys[i] = pose.pBoneKeys[i];
  }


  //-------------------------------------------------------
  /// Assignment operator.
  /// Set pose equal to given pose.
  //-------------------------------------------------------
  Pose& Pose::operator=(const Pose& pose)
  {
    if (this == &pose)
      return *this;

    Set(pose);
    return *this;
  }


  //-------------------------------------------------------
  /// Interpolate between this pose and target pose.
  /// The parameter #time determines the interpolation factor:
  /// The interpolated pose lies between time this pose and the
  /// time of the target pose.
  ///
  /// \param target Target pose.
  /// \param time   Time [ms] of interpolated frame.
  //-------------------------------------------------------
  void Pose::Interpolate(const Pose& target, int time)
  {
    ASSERT(this->time < time);
    ASSERT(pBoneKeys != 0);

    // Determine interpolation factor
    float u;

    if (time <= target.time)
    {
      u = (float) (time - this->time) / (target.time - this->time);
      this->time = time;
    }
    else
    {
      // Exception (If only one keyframe is available - next target-frame is actually previous frame.)
      u = 1.0f;
      this->time = target.time;
    }

    // Interpolate bone keys
    int i;
    for (i = 0; i < noOfBones; i++)
      pBoneKeys[i].Interpolate(target.pBoneKeys[i], u);
  }
}
