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
/// \file Vector.cpp
/// 3d Vector implementation.
//_______________________________________________________


//___________________________________________________________________________
//
//  Includes
//___________________________________________________________________________
//

#include "avatara/Vector.h"
#include "avatara/Debug.h"

#include <cmath>

using namespace std;

//-------------------------------------------------------
/// Algebraic structures and operations.
/// Includes classes and function for algebraic operations,
/// like vectors, matrices, quaternions, ...
//-------------------------------------------------------
namespace Algebra
{

  //___________________________________________________________________________
  //
  //  Constants
  //___________________________________________________________________________
  //

  //-------------------------------------------------------
  // Unit vectors
  //-------------------------------------------------------
  const Vector UnitVectorX(1.0, 0.0, 0.0);  ///< Unit vector along x-axis.
  const Vector UnitVectorY(0.0, 1.0, 0.0);  ///< Unit vector along y-axis.
  const Vector UnitVectorZ(0.0, 0.0, 1.0);  ///< Unit vector along z-axis.



  //___________________________________________________________________________
  //
  //  Methods
  //___________________________________________________________________________
  //

  //-------------------------------------------------------
  /// Constructor.
  /// Initializes each element with 0.0.
  //-------------------------------------------------------
  Vector::Vector(void)
  {
    x = 0.0;
    y = 0.0;
    z = 0.0;
  }


  //-------------------------------------------------------
  /// Constructor.
  /// Initializes elements with specified values.
  //-------------------------------------------------------
  Vector::Vector(const float v[3])
  {
    x = v[0];
    y = v[1];
    z = v[2];
  }


  //-------------------------------------------------------
  /// Constructor.
  /// Initializes elements with specified values.
  //-------------------------------------------------------
  Vector::Vector(float x, float y, float z)
  {
    this->x = x;
    this->y = y;
    this->z = z;
  }


  //-------------------------------------------------------
  /// Copy-Constructor.
  /// Initializes vector as copy of other.
  //-------------------------------------------------------
  Vector::Vector(const Vector &vector)
  {
    x = vector.x;
    y = vector.y;
    z = vector.z;
  }


  //-------------------------------------------------------
  /// Destructor.
  //-------------------------------------------------------
  Vector::~Vector(void)
  {
  }


  //-------------------------------------------------------
  /// X element of vector.
  /// Returns reference of element 0.
  //-------------------------------------------------------
  float& Vector::X()
  {
    return x;
  }


  //-------------------------------------------------------
  /// Y element of vector.
  /// Returns reference of element 1.
  //-------------------------------------------------------
  float& Vector::Y()
  {
    return y;
  }


  //-------------------------------------------------------
  /// Z element of vector.
  /// Returns reference of element 2.
  //-------------------------------------------------------
  float& Vector::Z()
  {
    return z;
  }


  //-------------------------------------------------------
  /// X element of vector.
  /// Returns reference of element 0.
  //-------------------------------------------------------
  const float& Vector::X() const
  {
    return x;
  }


  //-------------------------------------------------------
  /// Y element of vector.
  /// Returns reference of element 1.
  //-------------------------------------------------------
  const float& Vector::Y() const
  {
    return y;
  }


  //-------------------------------------------------------
  /// Z element of vector.
  // Returns reference of element 2.
  //-------------------------------------------------------
  const float& Vector::Z() const
  {
    return z;
  }


  //-------------------------------------------------------
  /// Set elements to specified values.
  //-------------------------------------------------------
  Vector& Vector::Set(const float v[3])
  {
    x = v[0];
    y = v[1];
    z = v[2];
    return *this;
  }


  //-------------------------------------------------------
  /// Set elements to specified values.
  //-------------------------------------------------------
  Vector& Vector::Set(float x, float y, float z)
  {
    this->x = x;
    this->y = y;
    this->z = z;
    return *this;
  }


  //-------------------------------------------------------
  /// Set elements equal to given vector.
  //-------------------------------------------------------
  Vector& Vector::Set(const Vector &vector)
  {
    x = vector.x;
    y = vector.y;
    z = vector.z;
    return *this;
  }


  //-------------------------------------------------------
  /// Returns array of elements.
  //-------------------------------------------------------
  void Vector::ToArray(float v[3]) const
  {
    v[0] = x;
    v[1] = y;
    v[2] = z;
  }


  //-------------------------------------------------------
  /// Assignment operator.
  /// Set elements to specified values.
  //-------------------------------------------------------
  Vector& Vector::operator=(const float v[3])
  {
    Set(v);
    return *this;
  }


  //-------------------------------------------------------
  /// Assignment operator.
  /// Performs a deep-copy.
  //-------------------------------------------------------
  Vector& Vector::operator=(const Vector &vector)
  {
    if (this == &vector)
      return *this;
    Set(vector);
    return *this;
  }


  //-------------------------------------------------------
  /// Index operator.
  /// Returns reference of element i.
  //-------------------------------------------------------
  const float& Vector::operator[](int i) const
  {
    ASSERT(0 <= i && i < 3);
    switch(i)
    {
    case 0:
      return x;
    case 1:
      return y;
    case 2:
      return z;
    default:
      return x;
    }
  }


  //-------------------------------------------------------
  /// Index operator.
  /// Returns reference of element i.
  //-------------------------------------------------------
  float& Vector::operator[](int i)
  {
    ASSERT(0 <= i && i < 3);
    switch(i)
    {
    case 0:
      return x;
    case 1:
      return y;
    case 2:
      return z;
    default:
      return x;
    }
  }


  //-------------------------------------------------------
  /// Equality operator==.
  //-------------------------------------------------------
  bool Vector::operator==(const Vector& vector) const
  {
    if (this == &vector)
      return true;

    return (x == vector.x
            && y == vector.y
            && z == vector.z);
  }


  //-------------------------------------------------------
  /// Inequality operator !=.
  //-------------------------------------------------------
  bool Vector::operator!=(const Vector& vector) const
  {
    return !(*this == vector);
  }


  //-------------------------------------------------------
  /// Unary operator -.
  //-------------------------------------------------------
  Vector Vector::operator-() const
  {
    return Vector(-x, -y, -z);
  }


  //-------------------------------------------------------
  /// Binary operator +.
  //-------------------------------------------------------
  Vector Vector::operator+(const Vector& vector) const
  {
    return Vector(x + vector.x,
                  y + vector.y,
                  z + vector.z);
  }


  //-------------------------------------------------------
  /// Binary operator +.
  //-------------------------------------------------------
  Vector Vector::operator-(const Vector& vector) const
  {
    return Vector(x - vector.x,
                  y - vector.y,
                  z - vector.z);
  }


  //-------------------------------------------------------
  /// Binary operator *.
  /// Vector' = Vector * Scalar.
  //-------------------------------------------------------
  Vector Vector::operator*(const float & s) const
  {
    return Vector(x * s,
                  y * s,
                  z * s);
  }


  //-------------------------------------------------------
  /// Binary operator * (inner vector product).
  /// Same as #DotProduct.
  /// Scalar = Vector1 * Vector2.
  //-------------------------------------------------------
  float Vector::operator*(const Vector & vector) const
  {
    return DotProduct(*this, vector);
  }


  //-------------------------------------------------------
  /// Binary operator /.
  /// Vector' = Vector / Scalar.
  //-------------------------------------------------------
  Vector Vector::operator/(const float & s) const
  {
    return Vector(x / s,
                  y / s,
                  z / s);
  }


  //-------------------------------------------------------
  /// Operator +=.
  //-------------------------------------------------------
  void Vector::operator+=(const Vector& vector)
  {
    x += vector.x;
    y += vector.y;
    z += vector.z;
  }


  //-------------------------------------------------------
  /// Operator -=.
  //-------------------------------------------------------
  void Vector::operator-=(const Vector& vector)
  {
    x -= vector.x;
    y -= vector.y;
    z -= vector.z;
  }


  //-------------------------------------------------------
  /// Operator *=.
  //-------------------------------------------------------
  void Vector::operator*=(const float & s)
  {
    x *= s;
    y *= s;
    z *= s;
  }


  //-------------------------------------------------------
  /// Operator /=.
  //-------------------------------------------------------
  void Vector::operator/=(const float & s)
  {
    x /= s;
    y /= s;
    z /= s;
  }


  //-------------------------------------------------------
  /// Returns length of vector.
  //-------------------------------------------------------
  float Vector::Length() const
  {
    return sqrt(x*x + y*y + z*z);
  }


  //-------------------------------------------------------
  /// Returns square length of vector.
  /// Faster to compute than real length.
  //-------------------------------------------------------
  float Vector::LengthSquare() const
  {
    return x*x + y*y + z*z;
  }


  //-------------------------------------------------------
  /// Normalizes vector.
  /// If the length of the vector is approximately 0.0
  /// the vector is set to {0.0,0.0,0.0}.
  /// #EPSILON value is used for
  /// comparision against 0.0.
  //-------------------------------------------------------
  Vector& Vector::Normalize()
  {
    float length = Length();
    if (length > EPSILON)
    {
      x /= length;
      y /= length;
      z /= length;
    }
    else
    {
      x = 0.0;
      y = 0.0;
      z = 0.0;
    }
    return *this;
  }


  //-------------------------------------------------------
  /// Binary operator *.
  /// Vector = Scalar * Vector.
  //-------------------------------------------------------
  Vector operator*(const float &s, const Vector &v)
  {
    return Vector(v.x * s,
                  v.y * s,
                  v.z * s);
  }


  //-------------------------------------------------------
  /// Dot-product.
  /// Returns the dot-product (inner product) of
  /// two vectors.
  //-------------------------------------------------------
  float DotProduct(const Vector &v1, const Vector& v2)
  {
    float result;
    result = v1.x*v2.x
             + v1.y*v2.y
             + v1.z*v2.z;
    return result;
  }


  //-------------------------------------------------------
  /// Cross-product (vector-product).
  /// Returns the cross-product (outer product) of
  /// two vectors.
  /// v3 = v1 x v2.
  //-------------------------------------------------------
  Vector CrossProduct(const Vector &v1, const Vector& v2)
  {
    Vector result;
    result.x = v1.y*v2.z - v1.z*v2.y;
    result.y = v1.z*v2.x - v1.x*v2.z;
    result.z = v1.x*v2.y - v1.y*v2.x;
    return result;
  }

}
