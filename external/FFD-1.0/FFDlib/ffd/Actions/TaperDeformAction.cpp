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


#include "TaperDeformAction.h"

#include "gmtl/Vec.h"

#include <iostream>

using std::cout;
using std::endl;

TaperDeformAction::TaperDeformAction(float taperFactor, int axis) :
    Base(), taperFactor(taperFactor)
{
    this->axis = axis;
    initAxis();
}

TaperDeformAction::~TaperDeformAction()
{
}

float TaperDeformAction::getTaperFactor() const
{
    return taperFactor;
}

int TaperDeformAction::getAxis() const
{
    return axis;
}

void TaperDeformAction::deform(Lattice& lattice)
{
    float taper;
    gmtl::Vec3f cellPoint, move, min, max;

    gmtl::Vec3i cellDivisions = lattice.getCellDivisions();
    Lattice::Vector3d& cellPoints = lattice.accessCellPoints();

    lattice.centerOrigin(min, max, move);
    taper = (max[z] - min[z]) / taperFactor;


    if (sign > 0)
    {
        for (int h = 0; h <= cellDivisions[0]; ++h)
            for (int w = 0; w <= cellDivisions[1]; ++w)
                for (int l = 0; l <= cellDivisions[2]; ++l)
                {
                    lattice.moveTo(cellPoints[h][w][l], cellPoint, move);
                    // f(z) = (max z - z) / (max z - min z)
                    // z' = z

                    // x' = f(z) * x
                    cellPoint[x] *= ((max[z] - cellPoint[z]) / taper);
                    // y' = f(z) * y
                    cellPoint[y] *= ((max[z] - cellPoint[z]) / taper);

                    lattice.moveBack(cellPoint, cellPoints[h][w][l], move);
                }
    }
    else
    {
        for (int h = 0; h <= cellDivisions[0]; ++h)
            for (int w = 0; w <= cellDivisions[1]; ++w)
                for (int l = 0; l <= cellDivisions[2]; ++l)
                {
                    lattice.moveTo(cellPoints[h][w][l], cellPoint, move);
                    // f(z) = (max z - z) / (max z - min z)
                    // z' = z

                    // x' = f(z) * x
                    cellPoint[x] *= (((max[z]) + cellPoint[z]) / taper);
                    // y' = f(z) * y
                    cellPoint[y] *= (((max[z]) + cellPoint[z]) / taper);

                    lattice.moveBack(cellPoint, cellPoints[h][w][l], move);
                }
    }
}

void TaperDeformAction::deformInverse(Lattice& lattice)
{
}

void TaperDeformAction::initAxis()
{
    if (axis < 0)
    {
        sign = -1;
        axis *= sign;
    }
    else
        sign = 1;

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

void TaperDeformAction::printDA(std::ostream& out) const
{
    out << "TaperDeformAction(" << taperFactor << ", " << axis << ")";
}

