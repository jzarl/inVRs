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


#include "GlobalDeformAction.h"

#include "gmtl/Vec.h"
#include "gmtl/Matrix.h"

GlobalDeformAction::GlobalDeformAction(gmtl::Matrix44f dMatrix, bool center) :
    Base(), dMatrix(dMatrix), center(center)
{
    invertible = true;
}

GlobalDeformAction::~GlobalDeformAction()
{
}

void GlobalDeformAction::setMatrix(const gmtl::Matrix44f dMatrix)
{
    this->dMatrix = dMatrix;
}

const gmtl::Matrix44f GlobalDeformAction::getMatrix() const
{
    return dMatrix;
}

bool GlobalDeformAction::isCenter()
{
    return center;
}

void GlobalDeformAction::deform(Lattice& lattice)
{
    gmtl::Vec3f cellPoint, move, min, max;

    if (center)
        lattice.centerOrigin(min, max, move);

    gmtl::Vec3i cellDivisions = lattice.getCellDivisions();
    Lattice::Vector3d& cellPoints = lattice.accessCellPoints();

    for (int h = 0; h <= cellDivisions[0]; ++h)
        for (int w = 0; w <= cellDivisions[1]; ++w)
            for (int l = 0; l <= cellDivisions[2]; ++l)
            {
                if (center)
                    lattice.moveTo(cellPoints[h][w][l], cellPoint, move);
                gmtl::xform(cellPoint, dMatrix, cellPoint);
                if (center)
                    lattice.moveBack(cellPoint, cellPoints[h][w][l], move);
            }
}

void GlobalDeformAction::deformInverse(Lattice& lattice)
{
    gmtl::Matrix44f invdMatrix;
    gmtl::invert(invdMatrix, dMatrix);
    gmtl::Vec3f cellPoint, move, min, max;

    if (center)
        lattice.centerOrigin(min, max, move);

    gmtl::Vec3i cellDivisions = lattice.getCellDivisions();
    Lattice::Vector3d& cellPoints = lattice.accessCellPoints();

    for (int h = 0; h <= cellDivisions[0]; ++h)
        for (int w = 0; w <= cellDivisions[1]; ++w)
            for (int l = 0; l <= cellDivisions[2]; ++l)
            {
                if (center)
                    lattice.moveTo(cellPoints[h][w][l], cellPoint, move);
                gmtl::xform(cellPoint, dMatrix, cellPoint);
                if (center)
                    lattice.moveBack(cellPoint, cellPoints[h][w][l], move);
            }
}

void GlobalDeformAction::printDA(std::ostream& out) const
{
    out << "GlobalDeformAction(" << std::endl;
    for (int i = 0; i < 4; ++i)
    {
        out << "      [" << dMatrix[i][0] << "][" << dMatrix[i][1] << "][";
        out << dMatrix[i][2] << "][" << dMatrix[i][3] << "]" << std::endl;
    }
    if (center)
        out << "      , true)";
    else
        out << "      , false";
}
