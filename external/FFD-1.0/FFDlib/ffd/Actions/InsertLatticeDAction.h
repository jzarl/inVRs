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


#ifndef INSERTLATTICEDEFORMACTION_H_INCLUDED
#define INSERTLATTICEDEFORMACTION_H_INCLUDED

#include "gmtl/AABox.h"
#include "gmtl/Vec.h"

#include "DAction.h"

/*******************************************************************************
 * @class InsertLatticeDAction
 * @brief Implements an action for lattice insertion with all needed parameters.
 */
class InsertLatticeDAction : public DAction
{
    typedef DAction Base;

    public:
        InsertLatticeDAction(int hDiv, int wDiv, int lDiv, size_t dim,
            bool masked, gmtl::Vec3f aabbMin, gmtl::Vec3f aabbMax,
            float epsilon);
        virtual ~InsertLatticeDAction();

        const gmtl::AABoxf getAabb() const;
        const int getHdiv() const;
        const int getWdiv() const;
        const int getLdiv() const;
        const size_t getDim() const;
        const float getEpsilon() const;
        const bool isMasked() const;

    protected:
        int hDiv, wDiv, lDiv;
        size_t dim;
        bool masked;
        gmtl::Vec3f aabbMin, aabbMax;
        float epsilon;
        virtual void printDA(std::ostream& out) const;
};

#endif // INSERTLATTICEDEFORMACTION_H_INCLUDED
