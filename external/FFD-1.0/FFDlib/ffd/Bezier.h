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

/*---------------------------------------------------------------------------*\
 *                                Changes                                    *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
\*---------------------------------------------------------------------------*/


#ifndef BEZIER_H_INCLUDED
#define BEZIER_H_INCLUDED

#include <vector>

#include <gmtl/Vec.h>

using std::vector;

/*******************************************************************************
 * @namespace Bezier
 * @brief Implements functions to calculate the Bezier interpolation.
 */
namespace Bezier
{
    typedef vector<vector<vector<gmtl::Vec3f> > > Vector3d;

    /**
     * calculateBernstein3d calculates the result of a trivariate Bernstein
     * polynomial for the given coordinate stu.
     *
     * @param stu has to be within range [0...1]
     * @param controlPoints are the corners of the cell in which a vertex
     *        resides in
     * @param dim + 1 describes the degree of the Bernstein polynomial for all
     *        three dimensions, therefore the size of controlPoints is
     *        (dim + 1)^3
     * @return transformed stu within range [0...1]
     */
    const gmtl::Vec3f calculateBernstein3d(const gmtl::Vec3f& stu,
                       const Vector3d& controlPoints, const unsigned int& dim);

    /**
     * Calculates the binomial coefficient for "n choose k"
     */
    unsigned int calculateBinomialCoefficient(const unsigned int n,
                        const unsigned int k);

    /**
     * Calculates the Bernstein basis polynomial
     */
    float calculateBernsteinPolynomial(const unsigned int i,
        const unsigned int n, const float t);
}

#endif // BEZIER_H_INCLUDED
