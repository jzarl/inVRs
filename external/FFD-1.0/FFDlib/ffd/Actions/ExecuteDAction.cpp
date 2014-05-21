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


#include "ExecuteDAction.h"

ExecuteDAction::ExecuteDAction(gmtl::Vec3f aabbMin, gmtl::Vec3f aabbMax) :
    Base(), aabbMin(aabbMin), aabbMax(aabbMax)
{
}

ExecuteDAction::~ExecuteDAction()
{
}

const gmtl::AABoxf ExecuteDAction::getAabb() const
{
    return gmtl::AABoxf(aabbMin, aabbMax);
}

void ExecuteDAction::printDA(std::ostream& out) const
{
    out << "ExecuteDAction ";
    out << aabbMin[0] << ", " << aabbMin[1] << ", " << aabbMin[2] << "), (";
    out << aabbMax[0] << ", " << aabbMax[1] << ", " << aabbMax[2] << ")]";
}