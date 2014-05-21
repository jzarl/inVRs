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
/// \file Skeleton.h
/// Skeleton definition.
//_____________________________________________________________

#ifndef _DR_SKELETON_H
  #define _DR_SKELETON_H

#include "avatara/Bone.h"
#include "avatara/Vector.h"
#include "avatara/Matrix.h"
#include <string>


namespace Avatara
{
  //-------------------------------------------------------
  /// Contains the bones of a skeleton.
  //-------------------------------------------------------
  class AVATARA_API Skeleton
  {
  public:
    Skeleton();
    ~Skeleton();

    void SetNoOfBones(int noOfBones);
    int GetNoOfBones() const;
    void SetBoneArray(Bone* pBones);
    const Bone& GetBone(int id) const;
    int GetBoneID(std::string name) const;
    void UnLoad();
    void CalculateMatrices();

  private:
    Skeleton(const Skeleton&);            // forbid use of copy constructor
    Skeleton& operator=(const Skeleton&); // forbid use of assignment operator

    int noOfBones;                    ///< Number of bones.
    Bone* pBones;                     ///< Array of bones, sorted by bone index.
  };


  //---------------------------------------------------------
  // Auxiliary functions.
  //---------------------------------------------------------
  Algebra::Matrix ComputeBoneToWorldMatrix(Algebra::Vector head, Algebra::Vector tail, float roll, const Bone* pParent);
  Algebra::Matrix BoneRotation(Algebra::Vector head, Algebra::Vector tail, float roll);

} // namespace Avatara

#endif
