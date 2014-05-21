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
/// \file Matrix.cpp
/// Matrix implementation.
//_______________________________________________________

#include "avatara/Matrix.h"
#include "avatara/Debug.h"

#include <cmath>
#include <algorithm>

using namespace std;


namespace Algebra
{
  //___________________________________________________________________________
  //
  //  Methods
  //___________________________________________________________________________
  //

  //-------------------------------------------------------
  /// Constructor.
  /// Initializes each coefficent with 0.0.
  //-------------------------------------------------------
  Matrix::Matrix(void)
  {
    values[0][0] = 0.0; values[0][1] = 0.0; values[0][2] = 0.0; values[0][3] = 0.0;
    values[1][0] = 0.0; values[1][1] = 0.0; values[1][2] = 0.0; values[1][3] = 0.0;
    values[2][0] = 0.0; values[2][1] = 0.0; values[2][2] = 0.0; values[2][3] = 0.0;
    values[3][0] = 0.0; values[3][1] = 0.0; values[3][2] = 0.0; values[3][3] = 0.0;
  }


  //-------------------------------------------------------
  /// Constructor.
  /// Initializes coefficients with specified values.
  //-------------------------------------------------------
  Matrix::Matrix(const float m[4][4])
  {
    Set(m);
  }


  //-------------------------------------------------------
  /// Copy-Constructor.
  /// Initializes matrix as copy of other matrix.
  //-------------------------------------------------------
  Matrix::Matrix(const Matrix &matrix)
  {
    Set(matrix);
  }


  //-------------------------------------------------------
  /// Destructor.
  //-------------------------------------------------------
  Matrix::~Matrix(void)
  {
  }


  //-------------------------------------------------------
  /// Access element.
  /// Returns reference of coefficient at [row][column].
  //-------------------------------------------------------
  float& Matrix::M(int row, int column)
  {
    using namespace std;
    ASSERT(0 <= row && row < 4);
    ASSERT(0 <= column && column < 4);

    return values[row][column];
  }


  //-------------------------------------------------------
  /// Access element.
  /// Returns reference of coefficient at [row][column].
  //-------------------------------------------------------
  const float& Matrix::M(int row, int column) const
  {
    using namespace std;
    ASSERT(0 <= row && row < 4);
    ASSERT(0 <= column && column < 4);

    return values[row][column];
  }


  //-------------------------------------------------------
  /// Set coefficients to specified values.
  //-------------------------------------------------------
  void Matrix::Set(const float m[4][4])
  {
    for(int y=0; y<4; y++)
      for(int x=0; x<4; x++)
        values[y][x] = m[y][x];
  }


  //-------------------------------------------------------
  /// Set coefficients equal to given matrix.
  //-------------------------------------------------------
  void Matrix::Set(const Matrix &matrix)
  {
    for(int y=0; y<4; y++)
      for(int x=0; x<4; x++)
        values[y][x] = matrix.values[y][x];
  }


  //-------------------------------------------------------
  /// Returns array of coefficients.
  //-------------------------------------------------------
  void Matrix::ToArray(float m[4][4]) const
  {
    for(int y=0; y<4; y++)
      for(int x=0; x<4; x++)
        m[y][x] = values[y][x];
  }


  //-------------------------------------------------------
  /// Assignment operator.
  //-------------------------------------------------------
  Matrix& Matrix::operator=(const float m[4][4])
  {
    Set(m);
    return *this;
  }


  //-------------------------------------------------------
  /// Assignment operator.
  /// Performs a deep-copy.
  //-------------------------------------------------------
  Matrix& Matrix::operator=(const Matrix &matrix)
  {
    if (this == &matrix)
      return *this;
    Set(matrix);
    return *this;
  }


  //-------------------------------------------------------
  /// Equality operator ==.
  //-------------------------------------------------------
  bool Matrix::operator==(const Matrix& matrix) const
  {
    if (this == &matrix)
      return true;

    for(int y=0; y<4; y++)
      for(int x=0; x<4; x++)
        if (values[y][x] != matrix.values[y][x])
          return false;

    return true;
  }


  //-------------------------------------------------------
  /// Inequality operator !=.
  //-------------------------------------------------------
  bool Matrix::operator!=(const Matrix& matrix) const
  {
    return !(*this == matrix);
  }


  //-------------------------------------------------------
  /// Matrix-Multiplication.
  //-------------------------------------------------------
  Matrix Matrix::operator*(const Matrix &matrix) const
  {
    Matrix result;

    for(int y=0; y<4; y++)
      for(int x=0; x<4; x++)
        for(int i=0; i<4; i++)
          result.values[y][x] += values[y][i] * matrix.values[i][x];

    return result;
  }


  //-------------------------------------------------------
  /// Matrix-Vector-Multiplication.
  /// Returns transformed vector:
  /// v' = M * v.
  //-------------------------------------------------------
  Vector Matrix::operator*(const Vector &vector) const
  {
    Vector result;
    Transform(vector, result);
    return result;
  }


  //-------------------------------------------------------
  /// Matrix-Scalar-Multiplication.
  /// Returns s * M, where s is a scalar.
  //-------------------------------------------------------
  Matrix Matrix::operator*(const float &s) const
  {
    Matrix result;

    for(int y=0; y<4; y++)
      for(int x=0; x<4; x++)
        result.values[y][x] *= s * values[y][x];
    return result;
  }


  //-------------------------------------------------------
  /// Multiply other matrix to current matrix.
  /// this = this * matrix.
  /// Same as #PostMultiply.
  //-------------------------------------------------------
  void Matrix::operator*=(const Matrix &matrix)
  {
    PostMultiply(matrix);
  }


  //-------------------------------------------------------
  /// Inverse matrix.
  /// Calculate inverted matrix.
  //-------------------------------------------------------
  Matrix Matrix::Inverse() const
  {
    Matrix m(values);
    return m.Invert();
  }


  //-------------------------------------------------------
  /// Invert this matrix.
  /// Throws NotInvertibleException if Matrix is
  /// not regular.
  //-------------------------------------------------------
  Matrix& Matrix::Invert()
  {
    Matrix temp = *this;
    Matrix inverse(IdentityMatrix);

    for(int i=0; i<4; i++)
    {
      // Look for row with max pivot
      float fMax = fabs(temp.M(i, i));
      int nMaxRow = i;
      for(int j=i+1; j<4; j++)
      {
        if(fabs(temp.M(j, i)) > fMax)
        {
          fMax = fabs(temp.M(j, i));
          nMaxRow = j;
        }
      }

      // Swap rows if necessary
      if(nMaxRow != i)
      {
        for(int k=0; k<4; k++)
        {
          std::swap(temp.M(i, k), temp.M(nMaxRow, k));
          std::swap(inverse.M(i, k), inverse.M(nMaxRow, k));
        }
      }

      float fTemp = temp.M(i, i);
      // Check if matrix is invertible
      if(fTemp == 0.0)
        throw new NotInvertibleException();

      for(int k=0; k<4; k++)
      {
        temp.M(i, k) /= fTemp;
        inverse.M(i, k) /= fTemp;
      }

      // Subtract current row from remaining rows
      for(int j=0; j<4; j++)
      {
        // Subtract from all but current row
        if(j != i)
        {
          float fTemp = temp.M(j, i);
          for(int k=0; k<4; k++)
          {
            temp.M(j, k) -= temp.M(i, k) * fTemp;
            inverse.M(j, k) -= inverse.M(i, k) * fTemp;
          }
        }
      }
    }

    Set(inverse);
    return *this;
  }


  //-------------------------------------------------------
  /// Post multiply another matrix.
  /// Multiplies to current.
  /// this = this * matrix
  //-------------------------------------------------------
  void Matrix::PostMultiply(const Matrix &matrix)
  {
    Matrix result;

    for(int y=0; y<4; y++)
      for(int x=0; x<4; x++)
        for(int i=0; i<4; i++)
          result.values[y][x] += values[y][i] * matrix.values[i][x];

    Set(result);
  }


  //-------------------------------------------------------
  /// Set scale part of matrix to 's'.
  //-------------------------------------------------------
  void Matrix::SetScale(const Vector& s)
  {
    values[0][0] = s[0];
    values[1][1] = s[1];
    values[2][2] = s[2];
  }


  //-------------------------------------------------------
  /// Set translation part of matrix to 'v'.
  //-------------------------------------------------------
  void Matrix::SetTranslation(const Vector &v)
  {
    values[0][3] = v[0];
    values[1][3] = v[1];
    values[2][3] = v[2];
  }


  //-------------------------------------------------------
  /// Set a certain rotation.
  /// Rotation of phi radials around an arbitrary
  /// axis/vector.
  ///
  /// Precondition:
  /// - v must be normalized.
  ///
  /// All other elements are unchanged.
  //-------------------------------------------------------
  void Matrix::SetRotation(const Vector &v, float phi)
  {
    float x = v.X();
	  float y = v.Y();
	  float z = v.Z();
	  float x2 = x * x;
	  float y2 = y * y;
	  float z2 = z * z;
    float xy = x * y;
    float xz = x * z;
    float yz = y * z;
	  float co = cos(phi);
	  float si = sin(phi);
    float xsi = x * si;
    float ysi = y * si;
    float zsi = z * si;
    float oneMinusCo = 1.0f - co;

    Set(IdentityMatrix);

	  values[0][0] = x2+co*(1.0f-x2);
	  values[0][1] = xy*oneMinusCo-zsi;
	  values[0][2] = xz*oneMinusCo+ysi;
	  values[1][0] = xy*oneMinusCo+zsi;
	  values[1][1] = y2+co*(1.0f-y2);
	  values[1][2] = yz*oneMinusCo-xsi;
	  values[2][0] = xz*oneMinusCo-ysi;
	  values[2][1] = yz*oneMinusCo+xsi;
	  values[2][2] = z2+co*(1.0f-z2);
  }


  //-------------------------------------------------------
  /// Transform vector.
  /// Transforms vector v:
  /// result = Matrix * v.
  //-------------------------------------------------------
  void Matrix::Transform(const Vector &v, Vector &result) const
  {
    // homogeneous vector
    float hv[4] = {v[0], v[1], v[2], 1.0};
    float afResult[4] = {0};

    for(int y=0; y<4; y++)
    {
      for(int x=0; x<4; x++)
        afResult[y] += values[y][x] * hv[x];
    }
    if (afResult[3] != 1.0)
      for(int y=0; y<3; y++)
        afResult[y] /= afResult[3];

    result = afResult;
  }


  //-------------------------------------------------------
  /// Matrix-Scalar-Multiplication.
  /// result = scalar * matrix.
  //-------------------------------------------------------
  Matrix operator*(const float &s, const Matrix m)
  {
    Matrix result;

    for(int y=0; y<4; y++)
      for(int x=0; x<4; x++)
        result.values[y][x] *= s * m.values[y][x];
    return result;
  }


  //-------------------------------------------------------
  // NotInvertibleException
  //-------------------------------------------------------
  const char * NotInvertibleException::szDescription
    = "Matrix is not invertible.";                          ///< Description of exception.
  const char * NotInvertibleException::what() const throw() /// Returns description of exception.
  {
    return this->szDescription;
  }

}
