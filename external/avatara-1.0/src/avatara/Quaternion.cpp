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
/// \file Quaternion.cpp
/// Quaternion implementation.
/// Code is base on
/// "Ken Shoemake: Animating Rotation with Quaternion Curves"
/// (Siggraph Proceedings '85, Volume 19, Number 3, pp. 245-254).
//_______________________________________________________

//___________________________________________________________________________
//
//  Includes
//___________________________________________________________________________
//

#include "avatara/Quaternion.h"
#include "avatara/Debug.h"

#include <cmath>

using namespace std;

namespace Algebra
{
  //___________________________________________________________________________
  //
  //  Constants
  //___________________________________________________________________________
  //

  //-------------------------------------------------------
  /// Identity quaternion.
  //-------------------------------------------------------
  const Quaternion IdentityQuaternion(1.0, Vector(0.0, 0.0, 0.0));


  //___________________________________________________________________________
  //
  //  Methods
  //___________________________________________________________________________
  //

  //-------------------------------------------------------
  /// Constructor.
  /// Initializes quaternion.
  //-------------------------------------------------------
  Quaternion::Quaternion(void)
    : w(1.0), v(0.0, 0.0, 0.0)
  {
  }


  //-------------------------------------------------------
  /// Constructor.
  /// Initializes elements with specified values.
  //-------------------------------------------------------
  Quaternion::Quaternion(float w, const Vector &v)
  {
    Set(w, v);
  }


  //-------------------------------------------------------
  /// Constructor.
  /// Initializes elements with specified values.
  //-------------------------------------------------------
  Quaternion::Quaternion(float w, float x, float y, float z)
  {
    Set(w, x, y, z);
  }


  //-------------------------------------------------------
  /// Constructor.
  /// Initializes elements to a certain rotation.
  //-------------------------------------------------------
  Quaternion::Quaternion(const Vector& axis, float theta)
  {
    Set(axis, theta);
  }


  //-------------------------------------------------------
  /// Constructor.
  /// Initializes elements according to Euler angles.
  /// Warning: Code is not tested.
  //-------------------------------------------------------
  Quaternion::Quaternion(float roll, float pitch, float yaw)
  {
    Set(roll, pitch, yaw);
  }


  //-------------------------------------------------------
  /// Copy-Constructor.
  /// Initializes quaternion as copy of other.
  //-------------------------------------------------------
  Quaternion::Quaternion(const Quaternion &q)
  {
    Set(q);
  }


  //-------------------------------------------------------
  /// Constructor.
  /// Initializes elements according to rotation
  /// matrix.
  //-------------------------------------------------------
  Quaternion::Quaternion(const Matrix &m)
  {
    Set(m);
  }


  //-------------------------------------------------------
  /// Destructor.
  //-------------------------------------------------------
  Quaternion::~Quaternion(void)
  {
  }


  //-------------------------------------------------------
  /// Returns reference of w.
  //-------------------------------------------------------
  float& Quaternion::W()
  {
    return w;
  }


  //-------------------------------------------------------
  /// Returns reference of x.
  //-------------------------------------------------------
  float& Quaternion::X()
  {
    return v.X();
  }


  //-------------------------------------------------------
  /// Returns reference of y.
  //-------------------------------------------------------
  float& Quaternion::Y()
  {
    return v.Y();
  }


  //-------------------------------------------------------
  /// Returns reference of z.
  //-------------------------------------------------------
  float& Quaternion::Z()
  {
    return v.Z();
  }


  //-------------------------------------------------------
  /// Returns reference of v.
  //-------------------------------------------------------
  Vector& Quaternion::V()
  {
    return v;
  }


  //-------------------------------------------------------
  /// Returns reference of w.
  //-------------------------------------------------------
  const float& Quaternion::W() const
  {
    return w;
  }


  //-------------------------------------------------------
  /// Returns reference of x.
  //-------------------------------------------------------
  const float& Quaternion::X() const
  {
    return v.X();
  }


  //-------------------------------------------------------
  /// Returns reference of y.
  //-------------------------------------------------------
  const float& Quaternion::Y() const
  {
    return v.Y();
  }


  //-------------------------------------------------------
  /// Returns reference of z.
  //-------------------------------------------------------
  const float& Quaternion::Z() const
  {
    return v.Z();
  }


  //-------------------------------------------------------
  /// Returns reference of v.
  //-------------------------------------------------------
  const Vector& Quaternion::V() const
  {
    return v;
  }


  //-------------------------------------------------------
  /// Set elements to specified values.
  //-------------------------------------------------------
  Quaternion& Quaternion::Set(float w, const Vector &v)
  {
    this->w = w;
    this->v = v;
    return *this;
  }


  //-------------------------------------------------------
  /// Set elements to specified values.
  //-------------------------------------------------------
  Quaternion& Quaternion::Set(float w, float x, float y, float z)
  {
    this->w = w;
    v.Set(x, y, z);
    return *this;
  }


  //-------------------------------------------------------
  /// Set elements equal to given quaternion.
  //-------------------------------------------------------
  Quaternion& Quaternion::Set(const Quaternion &q)
  {
    w = q.w;
    v = q.v;
    return *this;
  }


  //-------------------------------------------------------
  /// Set elements to the rotation given by the angle theta
  /// and the rotation axis.
  //-------------------------------------------------------
  Quaternion& Quaternion::Set(const Vector& axis, float theta)
  {
    theta /= 2;
    Vector normalizedAxis(axis);
    normalizedAxis.Normalize();
    this->w = cos(theta);
    this->v = sin(theta) * normalizedAxis;
    return *this;
  }


  //-------------------------------------------------------
  /// Set elements according to Euler angles.
  /// Warning: Code is not tested.
  //-------------------------------------------------------
  Quaternion& Quaternion::Set(float roll, float pitch, float yaw)
  {
    float cr = cos(roll / 2);
    float cp = cos(pitch / 2);
    float cy = cos(yaw / 2);

    float sr = sin(roll / 2);
    float sp = sin(pitch / 2);
    float sy = sin(yaw / 2);

    float cpcy = cp * cy;
    float spsy = sp * sy;

    w = cr * cpcy + sr * spsy;
    v.X() = sr * cpcy - cr * spsy;
    v.Y() = cr * sp * cy + sr * cp * sy;
	  v.Z() = cr * cp * sy - sr * sp * cy;

    return *this;
  }


  //-------------------------------------------------------
  /// Set elements equal to given rotation matrix.
  //-------------------------------------------------------
  Quaternion& Quaternion::Set(const Matrix& m)
  {
    float w2 = 0.25f * (m.M(0, 0) + m.M(1, 1) + m.M(2, 2) + m.M(3, 3));

    if (w2 > EPSILON)
    {
      w = sqrt(w2);
      v.X() = (m.M(2, 1) - m.M(1, 2)) / (4 * w);
      v.Y() = (m.M(0, 2) - m.M(2, 0)) / (4 * w);
      v.Z() = (m.M(1, 0) - m.M(0, 1)) / (4 * w);
    }
    else
    {
      w = 0.0;
      float x2 = -0.5f * (m.M(1, 1) + m.M(2, 2));
      if (x2 > EPSILON)
      {
        v.X() = sqrt(x2);
        v.Y() = m.M(1, 0) / (2 * v.X());
        v.Z() = m.M(2, 0) / (2 * v.X());
      }
      else
      {
        v.X() = 0.0;
        float y2 = 0.5f * (1 - m.M(2, 2));
        if (y2 > EPSILON)
        {
          v.Y() = sqrt(y2);
          v.Z() = m.M(2, 1) / (2 * v.Y());
        }
        else
        {
          v.Y() = 0.0;
          v.Z() = 1.0;
        }
      }
    }
    return *this;
  }


  //-------------------------------------------------------
  /// Set elements equal to a certain rotation.
  /// The rotation is given by a initial vector and
  /// one rotated (target) vector.
  //-------------------------------------------------------
  Quaternion& Quaternion::Set(const Vector& v1, const Vector& v2)
  {
    using namespace std;

    ASSERT(v1.LengthSquare() > EPSILON);
    ASSERT(v2.LengthSquare() > EPSILON);

    Vector startVector(v1);
    Vector rotatedVector(v2);
    startVector.Normalize();
    rotatedVector.Normalize();
    Vector axis = CrossProduct(startVector, rotatedVector);
    if (axis.Length() > EPSILON)
    {
      float sinTheta = axis.Length();
      float cosTheta = DotProduct(startVector, rotatedVector);
      float angle;
      if (cosTheta >= 0.0)
        angle = asin(sinTheta);                 // 1st or 4th quadrant
      else
        angle = (float) M_PI - asin(sinTheta);  // 2nd or 3rd quadrant
      axis.Normalize();
      w = cos(angle / 2);
      v = sin(angle / 2) * axis;
    }
    else
    {
      // vector have same direction
      if (startVector == rotatedVector)
      {
        // rotation of 0�
        w = 1.0;
        v = Vector(0, 0, 0);
      }
      else
      {
        // rotation of 180�
        w = 0.0;
        // any axis normal to startVector will do
        if (startVector == UnitVectorX)
          v = CrossProduct(UnitVectorY, startVector);
        else
          v = CrossProduct(UnitVectorX, startVector);
      }
    }
    return *this;
  }


  //-------------------------------------------------------
  /// Assignment operator.
  /// Set elements to specified values.
  //-------------------------------------------------------
  Quaternion& Quaternion::operator=(const Matrix &m)
  {
    Set(m);
    return *this;
  }


  //-------------------------------------------------------
  /// Assignment operator.
  /// Perfoms a deep-copy.
  //-------------------------------------------------------
  Quaternion& Quaternion::operator=(const Quaternion &q)
  {
    if (this == &q)
      return *this;
    Set(q);
    return *this;
  }


  //-------------------------------------------------------
  /// Equality operator ==.
  //-------------------------------------------------------
  bool Quaternion::operator==(const Quaternion& q) const
  {
    if (this == &q)
      return true;

    return (w == q.w && v == q.v);
  }


  //-------------------------------------------------------
  /// Inequality operator !=.
  //-------------------------------------------------------
  bool Quaternion::operator!=(const Quaternion& q) const
  {
    return !(*this == q);
  }


  //-------------------------------------------------------
  /// Quaternion subtraction.
  //-------------------------------------------------------
  Quaternion Quaternion::operator-() const
  {
    return Quaternion(-w, -v);
  }


  //-------------------------------------------------------
  /// Quaternion addition.
  //-------------------------------------------------------
  Quaternion Quaternion::operator+(const Quaternion& q) const
  {
    return Quaternion(w + q.w, v + q.v);
  }


  //-------------------------------------------------------
  /// Quaternion addition.
  //-------------------------------------------------------
  Quaternion Quaternion::operator-(const Quaternion& q) const
  {
    return Quaternion(w - q.w, v - q.v);
  }


  //-------------------------------------------------------
  /// Quaternion multiplication.
  //-------------------------------------------------------
  Quaternion Quaternion::operator*(const Quaternion& q) const
  {
    return Quaternion(w * q.w - DotProduct(v, q.v),
                      CrossProduct(v, q.v) + w * q.v + q.w * v);
  }


  //-------------------------------------------------------
  /// Quaternion division.
  //-------------------------------------------------------
  Quaternion Quaternion::operator/(const Quaternion& q) const
  {
    Quaternion inverse = q.Inverse();
    return *this * inverse;
  }


  //-------------------------------------------------------
  /// Quaternion division.
  //-------------------------------------------------------
  Quaternion Quaternion::operator/(const float& f) const
  {

    return Quaternion(w / f, v / f);
  }


  //-------------------------------------------------------
  /// Operator +=.
  //-------------------------------------------------------
  void Quaternion::operator+=(const Quaternion& q)
  {
    w += q.w;
    v += q.v;
  }


  //-------------------------------------------------------
  /// Operator -=.
  //-------------------------------------------------------
  void Quaternion::operator-=(const Quaternion& q)
  {
    w -= q.w;
    v -= q.v;
  }


  //-------------------------------------------------------
  /// Operator *=.
  //-------------------------------------------------------
  void Quaternion::operator*=(const Quaternion& q)
  {
    w = w * q.w - DotProduct(v, q.v);
    v = CrossProduct(v, q.v) + w * q.v + q.w * v;
  }


  //-------------------------------------------------------
  /// Operator /=.
  //-------------------------------------------------------
  void Quaternion::operator/=(const float & f)
  {
    w /= f;
    v /= f;
  }


  //-------------------------------------------------------
  /// Returns magnitude of quaternion.
  //-------------------------------------------------------
  float Quaternion::Length() const
  {
    using namespace std;
    return sqrt(w * w + v.LengthSquare());
  }


  //-------------------------------------------------------
  /// Returns square magnitude of vector.
  /// Faster to compute than real length.
  //-------------------------------------------------------
  float Quaternion::LengthSquare() const
  {
    return w * w + v.LengthSquare();
  }


  //-------------------------------------------------------
  /// Normalizes quaternion.
  //-------------------------------------------------------
  Quaternion& Quaternion::Normalize()
  {
    using namespace std;

    float length = 1.0;
    float squareLength = w * w + v.LengthSquare();
    if (squareLength > EPSILON)
      length = sqrt(squareLength);
    w /= length;
    v /= length;
    return *this;
  }


  //-------------------------------------------------------
  /// Calculate inverted quaternion.
  //-------------------------------------------------------
  Quaternion Quaternion::Inverse() const
  {
    Quaternion q(w, v);
    return q.Invert();
  }


  //-------------------------------------------------------
  /// Invert this quaternion.
  //-------------------------------------------------------
  Quaternion& Quaternion::Invert()
  {
    float squareMagnitude = LengthSquare();
    v = -v;
    w /= squareMagnitude;
    v /= squareMagnitude;
    return *this;
  }


  //-------------------------------------------------------
  /// Build rotation matrix.
  //-------------------------------------------------------
  Matrix Quaternion::RotationMatrix() const
  {
    float values[4][4];

	  float x2 = v.X() * v.X();
	  float y2 = v.Y() * v.Y();
	  float z2 = v.Z() * v.Z();
    float xy = 2 * v.X() * v.Y();
    float xz = 2 * v.X() * v.Z();
    float yz = 2 * v.Y() * v.Z();
    float sx = 2 * w * v.X();
    float sy = 2 * w * v.Y();
    float sz = 2 * w * v.Z();

    // according to Watt, p.489
    values[0][0] = 1-2*(y2+z2);
	  values[0][1] = xy-sz;
	  values[0][2] = sy+xz;
    values[0][3] = 0.0;

	  values[1][0] = xy+sz;
	  values[1][1] = 1-2*(x2+z2);
	  values[1][2] = -sx+yz;
    values[1][3] = 0.0;

	  values[2][0] = -sy+xz;
	  values[2][1] = sx+yz;
	  values[2][2] = 1-2*(x2+y2);
    values[2][3] = 0.0;

    values[3][0] = 0.0;
    values[3][1] = 0.0;
    values[3][2] = 0.0;
    values[3][3] = 1;

    return Matrix(values);
  }


  //-------------------------------------------------------
  /// Linear interpolation.
  /// Linear interpolation of two quaternions.
  ///
  ///  \param q end quaternion.
  ///  \param u factor.
  ///
  ///  Precondition:
  ///  - \a u ... [0,1].
  ///  - Quaternions are normalized.
  ///
  ///  \return Interpolated quaternion.
  //-------------------------------------------------------
  Quaternion Quaternion::Lerp(const Quaternion& q, float u) const
  {
    ASSERT(0.0 <= u && u <= 1);
    if (this == &q)
      return Quaternion(q);

    const Quaternion& startQuat = *this;  // start (reference to this quaternion)
    Quaternion endQuat = q;               // end (copy of q)
    Quaternion slerpQuat;                 // inbetween (interpolated quaternion)

    //
    // Determine angle between quaternions
    //
    float cosOmega = DotProduct(startQuat, endQuat);

    // Check if we are moving along the shortest arc of interpolation.
    if (cosOmega < 0.0)
    {
      // We are moving along the wrong arc.
      // --> Invert second quaternion
      endQuat = -endQuat;
    }

    float f1 = 1.0f - u;
    float f2 = u;
    slerpQuat.w = f1 * startQuat.w + f2 * endQuat.w;
    slerpQuat.v = f1 * startQuat.v + f2 * endQuat.v;
    return slerpQuat;
}


  //-------------------------------------------------------
  /// Spherical linear interpolation.
  /// Spherical linear interpolation of two quaternions.
  ///
  ///  \param q end quaternion.
  ///  \param u factor.
  ///
  ///  Precondition:
  ///  - \a u ... [0,1].
  ///  - Quaternions are normalized.
  ///
  ///  \return Interpolated quaternion.
  //-------------------------------------------------------
  Quaternion Quaternion::Slerp(const Quaternion& q, float u) const
  {
    ASSERT(0.0 <= u && u <= 1)

    if (this == &q)
      return Quaternion(q);

    const Quaternion& startQuat = *this;  // start
    Quaternion endQuat = q;               // end
    Quaternion slerpQuat;                 // inbetween (interpolated quaternion)

    //
    // Determine angle between quaternions
    //
    float cosOmega = DotProduct(startQuat, endQuat);

    // Check if we are moving along the shortest arc of interpolation.
    if (cosOmega < 0.0)
    {
      // We are moving along the wrong arc.
      // --> Invert second quaternion
      cosOmega = -cosOmega;
      endQuat = -endQuat;
    }

    float f1;
    float f2;

    if ((1.0f - cosOmega) > EPSILON)
    {
      // Default case:
      //   omega != 0
      // => sin(omega) != 0
      float omega = acos(cosOmega);
      float sinOmega = sin(omega);

      f1 = sin((1.0f - u) * omega) / sinOmega;
      f2 = sin(u * omega) / sinOmega;
    }
    else
    {
      // omega == 0�
      // Mathematically:    limes  (sin ax / sin x) = a
      //                   x --> 0
      // Logically: The two quaternions are so close together that we can
      //            use linear interpolation.
      f1 = 1.0f - u;
      f2 = u;
    }

    slerpQuat.w = f1 * startQuat.w + f2 * endQuat.w;
    slerpQuat.v = f1 * startQuat.v + f2 * endQuat.v;
    return slerpQuat;
  }


  //-------------------------------------------------------
  /// Dot-product.
  /// Returns the dot-product (inner product) of
  /// two quaternions.
  //-------------------------------------------------------
  float DotProduct(const Quaternion &q1, const Quaternion& q2)
  {
    float result;
    result = q1.w * q2.w + DotProduct(q1.v, q2.v);
    return result;
  }
}
