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


#include "SetPointDeformAction.h"

SetPointDeformAction::SetPointDeformAction(gmtl::Vec3i index,
    gmtl::Vec3f position) : Base(), index(index), position(position)
{
    invertible = true;
}

SetPointDeformAction::~SetPointDeformAction()
{
}

const gmtl::Vec3i SetPointDeformAction::getIndex() const
{
    return index;
}

const gmtl::Vec3f SetPointDeformAction::getPosition() const
{
    return position;
}

void SetPointDeformAction::deform(Lattice& lattice)
{
    lattice.getCellPoint(index, lastPosition);
    lattice.setCellPoint(index, position);
}

void SetPointDeformAction::deformInverse(Lattice& lattice)
{
    lattice.setCellPoint(index, lastPosition);
}

void SetPointDeformAction::printDA(std::ostream& out) const
{
    out << "SetPointDeformAction([";
    out <<  position[0] << "][";
    out <<  position[1] << "][";
    out <<  position[2] << "], lastPosition[";
    out <<  lastPosition[0] << "][";
    out <<  lastPosition[1] << "][";
    out <<  lastPosition[2] << "], index[";
    out <<  index[0] << "][";
    out <<  index[1] << "][";
    out <<  index[2] << "])";
}
