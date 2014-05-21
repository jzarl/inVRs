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


#ifndef SETPOINTDEFORMACTION_H_INCLUDED
#define SETPOINTDEFORMACTION_H_INCLUDED

#include <gmtl/Vec.h>

#include "DeformAction.h"

/*******************************************************************************
 * @class SetPointDeformAction
 * @brief Sets the position of a single lattice corner. The corner is
 *        references by an internally managed index.
 */
class SetPointDeformAction : public DeformAction
{
    typedef DeformAction Base;

    public:
        SetPointDeformAction(gmtl::Vec3i index, gmtl::Vec3f position);
        virtual ~SetPointDeformAction();

        const gmtl::Vec3i getIndex() const;
        const gmtl::Vec3f getPosition() const;

        virtual void deform(Lattice& lattice);
        virtual void deformInverse(Lattice& lattice);

    protected:
        gmtl::Vec3i index;
        gmtl::Vec3f position;
        gmtl::Vec3f lastPosition;
        virtual void printDA(std::ostream& out) const;
};


#endif // SETPOINTDEFORMACTION_H_INCLUDED
