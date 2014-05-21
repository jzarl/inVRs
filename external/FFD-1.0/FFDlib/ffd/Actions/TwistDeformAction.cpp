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


#include "TwistDeformAction.h"

#include "gmtl/Vec.h"

TwistDeformAction::TwistDeformAction(float twistFactor, int axis) :
    Base(), twistFactor(twistFactor)
{
    if ((axis > 2) || (axis < -2))
        this->axis = 0;
    else
        this->axis = axis;
    initAxis();

    invertible = true;
}

TwistDeformAction::~TwistDeformAction()
{
}

float TwistDeformAction::getTwistFactor() const
{
    return twistFactor;
}

int TwistDeformAction::getAxis() const
{
    return axis;
}

void TwistDeformAction::deform(Lattice& lattice)
{
    runDeform(false, lattice);
}

void TwistDeformAction::deformInverse(Lattice& lattice)
{
    runDeform(true, lattice);
}

void TwistDeformAction::runDeform(const bool inverse, Lattice& lattice)// const
{
    int invSign = 1;
    gmtl::Vec3f cellPoint, move, min, max;

    gmtl::Vec3i cellDivisions = lattice.getCellDivisions();
    Lattice::Vector3d& cellPoints = lattice.accessCellPoints();
    gmtl::Vec3f ldl = lattice.getLatticeDividedLength();

    float tf = twistFactor / (ldl[z] * lattice.getLatticeDivisions()[z]);
    lattice.centerOrigin(min, max, move);

    float tmpX;

    if (inverse)
        invSign = -1;

    for (int h = 0; h <= cellDivisions[0]; ++h)
    {
        for (int w = 0; w <= cellDivisions[1]; ++w)
        {
            for (int l = 0; l <= cellDivisions[2]; ++l)
            {
                lattice.moveTo(cellPoints[h][w][l], cellPoint, move);

                // x' = x * cos(twistFactor * z) - y  * sin(twistFactor * z)
                // x = x' * cos(twistFactor * z) + y' * sin(twistFactor * z)
                tmpX =
                    cellPoint[x] * cos(tf * cellPoint[z]) -
                    cellPoint[y] * sin(tf * cellPoint[z])
                    * invSign;

                // y'= x  *   sin(twistFactor * z) + y  * cos(twistFactor * z)
                // y = x' * - sin(twistFactor * z) + y' * cos(twistFactor * z)
                cellPoint[y] =
                    cellPoint[y] * cos(tf * cellPoint[z]) +
                    cellPoint[x] * sin(tf * cellPoint[z])
                    * invSign;
                // z' = z

                // assign x'
                cellPoint[x] = tmpX;

                lattice.moveBack(cellPoint, cellPoints[h][w][l], move);
            }
        }
    }
}

void TwistDeformAction::initAxis()
{
    if (axis < 0)
    {
        sign = -1;
        axis *= sign;
    }
    if (axis == 1)
    {
        x = 0;
        y = 1;
        z = 2;
    }
    else if (axis == 2)
    {
        x = 2;
        y = 0;
        z = 1;
    }
    else
    {
        x = 1;
        y = 2;
        z = 0;
        axis = 3;
    }
}

void TwistDeformAction::printDA(std::ostream& out) const
{
    out << "TwistDeformAction(" << gmtl::Math::rad2Deg(twistFactor) << " deg, ";
    if (axis == 1)
        out << "X)";
    else if (axis == 2)
        out << "Y)";
    else
        out << "Z)";
}
