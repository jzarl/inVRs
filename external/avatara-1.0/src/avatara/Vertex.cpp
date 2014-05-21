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
/// \file Vertex.cpp
/// 3d Vector implementation.
//_______________________________________________________


//___________________________________________________________________________
//
//  Includes
//___________________________________________________________________________
//

#include "avatara/Vertex.h"
#include "avatara/Debug.h"

using namespace Algebra;

namespace Avatara
{
  //___________________________________________________________________________
  //
  //  Methods
  //___________________________________________________________________________
  //

  //-------------------------------------------------------
  /// Initializes vertex.
  //-------------------------------------------------------
  Vertex::Vertex(void)
    : pos(), normal(), noOfBones(0), pBoneIndices(0), pWeights(0)
  {
  }


  //-------------------------------------------------------
  /// Destructor.
  //-------------------------------------------------------
  Vertex::~Vertex(void)
  {
    if (pBoneIndices != 0)
      delete[] pBoneIndices;
    if (pWeights != 0)
      delete[] pWeights;
  }


  //-------------------------------------------------------
  /// Initializes vertex (deep copy).
  //-------------------------------------------------------
  Vertex::Vertex(const Vertex& v)
  {
    Set(v);
  }


  //-------------------------------------------------------
  /// Assignment operator (deep copy).
  //-------------------------------------------------------
  Vertex& Vertex::operator=(const Vertex &v)
  {
    if (this == &v)
      return *this;
    Set(v);
    return *this;
  }


  //-------------------------------------------------------
  /// Set vertex (deep copy).
  //-------------------------------------------------------
  void Vertex::Set(const Vertex& v)
  {
    this->pos = v.pos;
    this->normal = v.normal;
    this->noOfBones = v.noOfBones;

    if (noOfBones > 0)
    {
      int i;

      pBoneIndices = new int[noOfBones];
      pWeights = new float[noOfBones];
      for (i=0; i<noOfBones; i++)
      {
        pBoneIndices[i] = v.pBoneIndices[i];
        pWeights[i] = v.pWeights[i];
      }
    }
    else
    {
      pBoneIndices = 0;
      pWeights = 0;
    }
  }


  //-------------------------------------------------------
  /// Position of Vertex.
  //-------------------------------------------------------
  Vector& Vertex::Position()
  {
    return pos;
  }


  //-------------------------------------------------------
  /// Position of Vertex.
  //-------------------------------------------------------
  const Vector& Vertex::Position() const
  {
    return pos;
  }


  //-------------------------------------------------------
  /// Normal of Vertex.
  //-------------------------------------------------------
  Vector& Vertex::Normal()
  {
    return normal;
  }


  //-------------------------------------------------------
  /// Normal of Vertex.
  //-------------------------------------------------------
  const Vector& Vertex::Normal() const
  {
    return normal;
  }


  //-------------------------------------------------------
  /// Allocate resources for bone indices.
  ///
  /// \param n  Number of affecting bones.
  //-------------------------------------------------------
  void Vertex::AllocateBoneList(int n)
  {
    ASSERT(n>0);
    if (pBoneIndices != 0)
      delete[] pBoneIndices;
    if (pWeights != 0)
      delete[] pWeights;

    noOfBones = n;
    pBoneIndices = new int[n];
    pWeights = new float[n];
  }


  //-------------------------------------------------------
  /// Returns number of affecting bones.
  //-------------------------------------------------------
  int Vertex::NoOfBones() const
  {
    return noOfBones;
  }


  //-------------------------------------------------------
  /// Get index (in skeleton) of affecting bone.
  /// \param n  Index of affecting bone (0 <= n < no. of affecting bones).
  /// \return   Index of affecting bone in skeleton.
  //-------------------------------------------------------
  int Vertex::GetBoneIndex(int n) const
  {
    ASSERT(n>=0);
    ASSERT(n<noOfBones);
    return pBoneIndices[n];
  }


  //-------------------------------------------------------
  /// Set index (in skeleton) of affecting bone.
  /// \param n  Index of affecting bone (0 <= n < no. of affecting bones).
  /// \param boneIndex  Index of affecting bone in skeleton.
  //-------------------------------------------------------
  void Vertex::SetBoneIndex(int n, int boneIndex)
  {
    ASSERT(n>=0);
    ASSERT(n<noOfBones);
    pBoneIndices[n] = boneIndex;
  }


  //-------------------------------------------------------
  /// Get weight of bone.
  /// \param n  Index of affecting bone (0 <= n < no. of affecting bones).
  /// \return   Weight of bone.
  //-------------------------------------------------------
  float Vertex::GetBoneWeight(int n) const
  {
    ASSERT(n>=0);
    ASSERT(n<noOfBones);
    return pWeights[n];
  }


  //-------------------------------------------------------
  /// Set weight of affecting bone.
  /// \param n  Index of affecting bone (0 <= n < no. of affecting bones).
  /// \param weight  Weight of bone.
  //-------------------------------------------------------
  void Vertex::SetBoneWeight(int n, float weight)
  {
    ASSERT(n>=0);
    ASSERT(n<noOfBones);
    pWeights[n] = weight;
  }

}
