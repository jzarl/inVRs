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


#include "BendDeformAction.h"

#include <gmtl/Math.h>
#include <gmtl/Matrix.h>
#include <gmtl/Xforms.h>
#include <gmtl/Vec.h>

BendDeformAction::BendDeformAction(float bendFactor, float center, int axis) :
    Base(), bendFactor(bendFactor)
{
    if (center > 1.0f)
        this->center = 1.0f;
    else if (center < 0.0f)
        this->center = 0.0f;
    else
        this->center = center;

    if ((axis > 2) || (axis < -2))
        this->axis = 0;
    else
        this->axis = axis;
    initAxis();

    invertible = false;
}

BendDeformAction::~BendDeformAction()
{
}

float BendDeformAction::getBendFactor() const
{
    return bendFactor;
}

float BendDeformAction::getCenter() const
{
    return center;
}

int BendDeformAction::getAxis() const
{
    return axis;
}

void BendDeformAction::deform(Lattice& lattice)
{
    float theta, cos_theta, sin_theta, y_adjust, tmp;
    gmtl::Vec3f cellPoint, move, min, max;
    gmtl::Matrix44f dMatrix;

    gmtl::Vec3i cellDivisions = lattice.getCellDivisions();
    Lattice::Vector3d& cellPoints = lattice.accessCellPoints();
    gmtl::Vec3f ldl = lattice.getLatticeDividedLength();

    float bf = (bendFactor / (ldl[y] * lattice.getLatticeDivisions()[y]));
    lattice.centerOrigin(min, max, move);

    float centerOfBend = min[y] + (max[y] - min[y]) * center;

    for (int h = 0; h <= cellDivisions[0]; ++h)
        for (int w = 0; w <= cellDivisions[1]; ++w)
            for (int l = 0; l <= cellDivisions[2]; ++l)
            {
                lattice.moveTo(cellPoints[h][w][l], cellPoint, move);

                // y^ = y_min if y <= y_min
                if (cellPoint[y] <= min[y])
                    y_adjust = min[y];

                // y^ = y_max if y >= y_max
                else if (cellPoint[y] >= max[y])
                    y_adjust = max[y];

                // y^ = y if y_min < y < y_max
                else
                    y_adjust = cellPoint[y];

                // theta = k * (y^ - y0)
                theta = bf * (y_adjust - centerOfBend);
                cos_theta = cos(theta);
                sin_theta = sin(theta);

                // x' = x
                // y'
                if (cellPoint[y] > max[y])
                    // y' = y0 - S_theta * (z - 1/k) + C_theta * (y - y_max)
                    tmp = centerOfBend -
                            sin_theta * (cellPoint[z] - 1 / bf) +
                            cos_theta * (cellPoint[y] - max[y]);

                else if (cellPoint[y] < min[y])
                    // y' = y0 - S_theta * (z - 1/k) + C_theta * (y - y_min)
                    tmp = centerOfBend -
                            sin_theta * (cellPoint[z] - 1 / bf) +
                            cos_theta * (cellPoint[y] - min[y]);
                else
                    // y' = y0 - S_theta * (z - 1/k)
                    tmp = centerOfBend -
                            sin_theta * (cellPoint[z] - 1 / bf);

                // z'
                if (cellPoint[y] > max[y])
                    // z' = C_theta * (z - 1/k) + 1 / k + S_theta * (y - y_min)
                    cellPoint[z] =  cos_theta * (cellPoint[z] - 1 / bf) + 1 / bf
                            + sin_theta * (cellPoint[y] - max[y]);

                else if (cellPoint[y] < min[y])
                    // z' = C_theta * (z - 1/k) + 1 / k + S_theta * (y - y_max)
                    cellPoint[z] =  cos_theta * (cellPoint[z] - 1 / bf) + 1 / bf
                            + sin_theta * (cellPoint[y] - min[y]);
                else
                    // z' = C_theta * (z - 1/k) + 1 / k
                    cellPoint[z] =  cos_theta * (cellPoint[z] - 1 / bf) + 1 / bf;

                // assign y'
                cellPoint[y] = tmp;

                lattice.moveBack(cellPoint, cellPoints[h][w][l], move);
            }
}

void BendDeformAction::deformInverse(Lattice& lattice)
{
}

void BendDeformAction::initAxis()
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
        x = 1;
        y = 0;
        z = 2;
    }
    else
    {
        x = 2;
        y = 1;
        z = 0;
        axis = 3;
    }
}

void BendDeformAction::printDA(std::ostream& out) const
{
    out << "BendDeformAction(" << gmtl::Math::rad2Deg(bendFactor) << " deg, ";
    if (axis == 1)
        out << "X)";
    else if (axis == 2)
        out << "Y)";
    else
        out << "Z)";
}
