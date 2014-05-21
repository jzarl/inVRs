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
/// \file Quaternion.h
/// Quaternion definition.
//_______________________________________________________

#ifndef _DR_QUATERNION_H
#define _DR_QUATERNION_H

#include "avatara/Matrix.h"


namespace Algebra
{

  //___________________________________________________________________________
  //
  //  Classes
  //___________________________________________________________________________
  //

  //-------------------------------------------------------
  /// Quaternion.
  /// A quaternion is a sophisticated way to describe
  /// 3-dimensional rotations. It describes a rotation
  /// around an axis. Quaternions have several advantages over
  /// simple rotations matrices.
  //-------------------------------------------------------
  class Quaternion
  {
  private:
    float w;  ///< Real part of #Quaternion.
    Vector v; ///< Imaginary part of #Quaternion.

  public:
    Quaternion(void);
    Quaternion(float w, const Vector &v);
    Quaternion(const Vector& axis, float theta);
    Quaternion(float w, float x, float y, float z);
    Quaternion(float roll, float pitch, float yaw);
    Quaternion(const Quaternion &q);
    Quaternion(const Matrix &m);
    ~Quaternion(void);

    float& W();
    float& X();
    float& Y();
    float& Z();
    Vector& V();
    const float& W() const;
    const float& X() const;
    const float& Y() const;
    const float& Z() const;
    const Vector& V() const;

    Quaternion& Set(float w, const Vector &v);
    Quaternion& Set(float w, float x, float y, float z);
    Quaternion& Set(const Quaternion& q);
    Quaternion& Set(const Matrix& m);
    Quaternion& Set(const Vector& v1, const Vector& v2);
    Quaternion& Set(float roll, float pitch, float yaw);
    Quaternion& Set(const Vector& axis, float theta);
    Quaternion& operator=(const Matrix& m);
    Quaternion& operator=(const Quaternion& q);

    bool operator==(const Quaternion& q) const;
    bool operator!=(const Quaternion& q) const;

    Quaternion operator-() const;
    Quaternion operator+(const Quaternion& q) const;
    Quaternion operator-(const Quaternion& q) const;
    Quaternion operator*(const Quaternion& q) const;
    Quaternion operator/(const Quaternion& q) const;
    Quaternion operator/(const float& f) const;
    void operator+=(const Quaternion& q);
    void operator-=(const Quaternion& q);
    void operator*=(const Quaternion& q);
    void operator/=(const Quaternion& q);
    void operator/=(const float& f);

    float Length() const;
    float LengthSquare() const;
    Quaternion& Normalize();
    Quaternion Inverse() const;
    Quaternion& Invert();
    Matrix RotationMatrix() const;
    Quaternion Lerp(const Quaternion& q, float u) const;
    Quaternion Slerp(const Quaternion& q, float u) const;

    friend float DotProduct(const Quaternion &q1, const Quaternion& q2);
  };


  //-------------------------------------------------------
  /// Identity quaternion (= unit-quaternion, no rotation).
  //-------------------------------------------------------
  extern const Quaternion IdentityQuaternion;


  //-------------------------------------------------------
  //  Friends of Quaternion
  //-------------------------------------------------------
  float DotProduct(const Vector &v1, const Vector& v2);

}

#endif // _DR_QUATERNION_H
