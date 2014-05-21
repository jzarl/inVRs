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
/// \file Vector.h
/// 3d Vector definition.
//_______________________________________________________

#ifndef _DR_VECTOR_H
#define _DR_VECTOR_H

#include "avatara/AvataraDllExports.h"

//___________________________________________________________________________
//
//  Constants
//___________________________________________________________________________
//

/// Epsilon-value for comparisons (f==0)
#define EPSILON  1e-6

namespace Algebra
{

  //___________________________________________________________________________
  //
  //  Classes
  //___________________________________________________________________________
  //

  //-------------------------------------------------------
  /// Vector.
  /// Definition of a 3-dimensional vector.
  //-------------------------------------------------------
  class AVATARA_API Vector
  {
  private:
    float x;  ///< x-element of vector
    float y;  ///< y-element of vector
    float z;  ///< z-element of vector

  public:
    Vector(void);
    Vector(const float v[3]);
    Vector(float x, float y, float z);
    Vector(const Vector &vector);
    ~Vector(void);

    float& X();
    float& Y();
    float& Z();
    const float& X() const;
    const float& Y() const;
    const float& Z() const;

    Vector& Set(const float v[3]);
    Vector& Set(float x, float y, float z);
    Vector& Set(const Vector &vector);
    void ToArray(float v[3]) const;
    Vector& operator=(const float v[3]);
    Vector& operator=(const Vector &vector);
    const float& operator[](int i) const;
    float& operator[](int i);

    bool operator==(const Vector& vector) const;
    bool operator!=(const Vector& vector) const;

    Vector operator-() const;
    Vector operator+(const Vector& vector) const;
    Vector operator-(const Vector& vector) const;
    Vector operator*(const float & s) const;
    float operator*(const Vector & vector) const;
    Vector operator/(const float & s) const;
    void operator+=(const Vector& vector);
    void operator-=(const Vector& vector);
    void operator*=(const float & s);
    void operator/=(const float & s);

    float Length() const;
    float LengthSquare() const;
    Vector& Normalize();

    friend Vector operator*(const float &s, const Vector &v);
    friend float DotProduct(const Vector &v1, const Vector& v2);
    friend Vector CrossProduct(const Vector &v1, const Vector& v2);
  };


  //-------------------------------------------------------
  //  Unit vectors
  //-------------------------------------------------------
  extern const Vector UnitVectorX;
  extern const Vector UnitVectorY;
  extern const Vector UnitVectorZ;

  //-------------------------------------------------------
  //  Friends of Vector
  //-------------------------------------------------------
  Vector operator*(const float &s, const Vector &v);
  float DotProduct(const Vector &v1, const Vector& v2);
  Vector CrossProduct(const Vector &v1, const Vector& v2);

}

#endif // _DR_VECTOR_H
