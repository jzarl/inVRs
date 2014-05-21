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

//_______________________________________________________
//
/// \file Vertex.h
/// 3d Vertex definition.
//_______________________________________________________

#ifndef _DR_VERTEX_H
#define _DR_VERTEX_H

#include "avatara/Vector.h"

namespace Avatara
{

  //___________________________________________________________________________
  //
  //  Classes
  //___________________________________________________________________________
  //

  //-------------------------------------------------------
  /// 3-dimensional vertex.
  //-------------------------------------------------------
  class AVATARA_API Vertex
  {
  public:
    Vertex();
    ~Vertex();
    Vertex(const Vertex& v);
    Vertex& operator=(const Vertex &v);
    void Set(const Vertex& v);

    Algebra::Vector& Position();
    const Algebra::Vector& Position() const;
    Algebra::Vector& Normal();
    const Algebra::Vector& Normal() const;
    void AllocateBoneList(int n);
    int NoOfBones() const;
    int GetBoneIndex(int n) const;
    void SetBoneIndex(int n, int boneIndex);
    float GetBoneWeight(int n) const;
    void SetBoneWeight(int n, float weight);

  private:
    Algebra::Vector pos;      ///< Position.
    Algebra::Vector normal;   ///< Normal vector.
    int noOfBones;            ///< Number of bones to which this vertex belongs.
    int* pBoneIndices;        ///< Array of indices of affecting bones.
    float* pWeights;          ///< Array of bone weights.
  };
}

#endif // _DR_VERTEX_H
