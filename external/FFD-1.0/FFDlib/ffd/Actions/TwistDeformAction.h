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


#ifndef TWISTDEFORMACTION_H_INCLUDED
#define TWISTDEFORMACTION_H_INCLUDED

#include "DeformAction.h"

/*******************************************************************************
 * @class TwistDeformAction
 * @brief Does a global twist on the lattice. The twist deformation is
 *        invertible.
 */
class TwistDeformAction : public DeformAction
{
    typedef DeformAction Base;

    public:
        /**
         * @param twistFactor: twisting angle in radiants
         * @param axis bend along the given axis ([X=1, Y=2, Z=3]). Values
         *        outside of the range are set to the Z-Axis. Negative axis
         *        values cause a twist in the opposite direction.
         */
        TwistDeformAction(float twistFactor, int axis);
        virtual ~TwistDeformAction();

        float getTwistFactor() const;
        int getAxis() const;

        virtual void deform(Lattice& lattice);
        virtual void deformInverse(Lattice& lattice);

    protected:
        float twistFactor;
        int axis, sign, x, y, z;
        void runDeform(const bool inverse, Lattice& lattice);// const;
        void initAxis();
        virtual void printDA(std::ostream& out) const;
};

#endif // TWISTDEFORMACTION_H_INCLUDED
