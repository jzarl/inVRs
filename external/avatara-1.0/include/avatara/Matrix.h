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
/// \file Matrix.h
/// 4x4-Matrix definition.
//_______________________________________________________

#ifndef _DR_MATRIX_H
#define _DR_MATRIX_H


//___________________________________________________________________________
//
//  Includes
//___________________________________________________________________________
//

#include "avatara/Vector.h"
#include <stdexcept>


namespace Algebra
{
  //___________________________________________________________________________
  //
  //  Classes
  //___________________________________________________________________________
  //

  //-------------------------------------------------------
  /// 4x4-matrix.
  /// Defintion of a 4x4-matrix.
  //-------------------------------------------------------
  class AVATARA_API Matrix
  {
  private:
    const static int size = 4;  ///< Dimension of #Matrix.

    /// Matrix elements.
    /// Aligned in row major order.
    /// Attention: OpenGL uses column major order.
    float values[4][4];

  public:
    Matrix(void);
    Matrix(const float m[4][4]);
    Matrix(const Matrix &matrix);
    ~Matrix(void);

    float& M(int row, int column);
    const float& M(int row, int column) const;

    void Set(const float m[4][4]);
    void Set(const Matrix& matrix);
    void ToArray(float m[4][4]) const;
    Matrix& operator=(const float m[4][4]);
    Matrix& operator=(const Matrix& matrix);
    bool operator==(const Matrix& matrix) const;
    bool operator!=(const Matrix& matrix) const;
    Matrix operator*(const Matrix& matrix) const;
    Vector operator*(const Vector& vector) const;
    Matrix operator*(const float& s) const;
    void operator*=(const Matrix& matrix);
    Matrix Inverse() const;
    Matrix& Invert();
    void PostMultiply(const Matrix& matrix);
    void SetScale(const Vector& s);
    void SetTranslation(const Vector& v);
    void SetRotation(const Vector& v, float phi);
    void Transform(const Vector& v, Vector& result) const;
    friend Matrix operator*(const float & s, const Matrix m);
  };


  /// Values of identity matrix.
  /// The following array is only used to initialize Matrix::IdentityMatrix.
  static const float s_afIdentity[4][4] =
  {
    {1.0, 0.0, 0.0, 0.0},
    {0.0, 1.0, 0.0, 0.0},
    {0.0, 0.0, 1.0, 0.0},
    {0.0, 0.0, 0.0, 1.0}
  };

  //-------------------------------------------------------
  /// Identity matrix.
  //-------------------------------------------------------
  const Matrix IdentityMatrix(s_afIdentity);


  //-------------------------------------------------------
  //  Friends of Matrix
  //-------------------------------------------------------
  Matrix operator*(const float &s, const Matrix m);


  //-------------------------------------------------------
  /// Exception "Matrix is not invertible".
  /// Matrix is thrown by Algebra::Matrix.Inverse or
  /// Algebra::Matrix.Invert when matrix is singular.
  //-------------------------------------------------------
  class NotInvertibleException
  {
  private:
    const static char * szDescription;
  public:
    const char *what( ) const throw();
  };

}

#endif // _DR_MATRIX_H
