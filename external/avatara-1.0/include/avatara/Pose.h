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

//_______________________________________________________
//
/// \file Pose.h
/// Description of pose of a skeleton and bones.
//_______________________________________________________

#ifndef _DR_POSE_H
  #define _DR_POSE_H

#include "avatara/Quaternion.h"

namespace Avatara
{
  //-------------------------------------------------------
  /// Definition of a pose of a single bone (position, size, rotation).
  //-------------------------------------------------------
  class AVATARA_API BoneKey
  {
  public:
    Algebra::Quaternion quaternion;   ///< Rotation of bone
    Algebra::Vector location;         ///< Translation of bone
    Algebra::Vector scale;            ///< Scaling of bone

    BoneKey();
    BoneKey(const Algebra::Quaternion & quat,
            const Algebra::Vector & loc,
            const Algebra::Vector & s);
    ~BoneKey();

    void Set(const Algebra::Quaternion & quat,
             const Algebra::Vector & loc,
             const Algebra::Vector & s);
    void Set(const BoneKey& boneKey);
    BoneKey(const BoneKey & bk);
    BoneKey& operator=(const BoneKey& boneKey);
    void Interpolate(const BoneKey& target, float u);
  };


  //-------------------------------------------------------
  /// Definition of a single pose of a skeleton.
  /// A pose consists of time (current animation-time, [ms])
  /// and the position of all bones (position, size, rotation).
  //-------------------------------------------------------
  class AVATARA_API Pose
  {
  private:
    int noOfBones;        ///< Number of bones in skeleton.
    BoneKey* pBoneKeys;   ///< Position of each bone.

  public:
    int time;             ///< Time of this pose [ms].

    Pose();
    ~Pose();
    Pose(const Pose & pose);

    void AllocateBoneKeys(int n);
    const BoneKey & GetBoneKey(int boneID) const;
    void SetBoneKey(int boneID, const BoneKey & boneKey);
    void Set(const Pose& pose);
    Pose& operator=(const Pose& pose);
    void Interpolate(const Pose& target, int time);
  };

}


#endif // _DR_POSE_H
