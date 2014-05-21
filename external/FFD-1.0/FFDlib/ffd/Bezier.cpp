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
 *                           Project: FFD                                    *
 *                                                                           *
 * The FFD library was developed during a practical at the Johannes Kepler   *
 * University, Linz in 2009 by Marlene Hochrieser                            *
 *                                                                           *
\*---------------------------------------------------------------------------*/


#include "Bezier.h"

#include <gmtl/Math.h>

const gmtl::Vec3f Bezier::calculateBernstein3d(const gmtl::Vec3f& stu,
                   const Vector3d& controlPoints, const unsigned int& dim)
{
    gmtl::Vec3f bernstein;
    float bernsteinPoly;

    for (unsigned int i = 0; i <= dim; ++i)
    {
        for (unsigned int j = 0; j <= dim; ++j)
        {
            for (unsigned int k = 0; k <= dim; ++k)
            {
                bernsteinPoly = calculateBernsteinPolynomial(dim, i, stu[0]);
                bernsteinPoly *= calculateBernsteinPolynomial(dim, j, stu[1]);
                bernsteinPoly *= calculateBernsteinPolynomial(dim, k, stu[2]);

                for (unsigned int c = 0; c < 3; ++c)
                    bernstein[c] += controlPoints[i][j][k][c] * bernsteinPoly;
            }
        }
    }
    return bernstein;
}

unsigned int Bezier::calculateBinomialCoefficient(const unsigned int n,
                    const unsigned int k)
{
    unsigned int binomialCoefficient = 1;

    if (k == 0)
        return binomialCoefficient;

    if (2 * k > n)
        binomialCoefficient = calculateBinomialCoefficient(n, n - k);
    else
    {
        binomialCoefficient = n;
        for (unsigned int i = 2; i <= k; ++i)
        {
            binomialCoefficient = binomialCoefficient * (n + 1 - i);
            binomialCoefficient = binomialCoefficient / i;
        }
    }

    return binomialCoefficient;
}

float Bezier::calculateBernsteinPolynomial(const unsigned int n,
              const unsigned int k, const float t)
{
    return (calculateBinomialCoefficient(n, k) * gmtl::Math::pow(t, k) *
            gmtl::Math::pow((1.0 - t), (n - k)));
}
