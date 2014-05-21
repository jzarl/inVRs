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
/// \file Bone.h
/// Bone.
//_____________________________________________________________

#ifndef _DR_BONE_H
  #define _DR_BONE_H


#include "avatara/Vector.h"
#include "avatara/Matrix.h"
#include <string>


namespace Avatara
{

  //-------------------------------------------------------
  /// Bone structure.
  //-------------------------------------------------------
  struct AVATARA_API Bone
  {
    int parentID;                         ///< Parent-bone id (-1 == no parent).
    std::string name;                     ///< Name of bone.
    //Algebra::Vector head;               ///< Head point.
    //Algebra::Vector tail;               ///< Tail point.
    //float roll;                         ///< Roll angle.
    float length;                         ///< The length of this bone.
    Algebra::Matrix boneToWorldMatrix;    ///< The bone-to-world transformation matrix.
    Algebra::Matrix worldToBoneMatrix;    ///< The world-to-bone transformation matrix.
    Algebra::Matrix offsetMatrix;         ///< The offset to parent bone.
  };

} // namespace Avatara


#endif
