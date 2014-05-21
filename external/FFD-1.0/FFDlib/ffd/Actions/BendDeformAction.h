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


#ifndef BENDDEFORMACTION_H_INCLUDED
#define BENDDEFORMACTION_H_INCLUDED

#include "DeformAction.h"

/*******************************************************************************
 * @class BendDeformAction
 * @brief Implements the bending deformation. The range of the bend is set to
 *        the AABB minimum and maximum.
 *        The bend deformation is not invertible.
 */
class BendDeformAction : public DeformAction
{
    typedef DeformAction Base;

    public:
        /**
         * @param bendFactor: bending angle in radiants, should be negative
         *        to bend in the opposite direction
         * @param center of bending has to be in range [0.0f, 1.0f] and sets the
         *        center of the bending to a point between bottom and top of the
         *        lattice at the given bending axis
         * @param axis bend along this given axis ([X=1, Y=2, Z=3]). Values
         *        outside of the range are set Z.
         */
        BendDeformAction(float bendFactor, float center, int axis);
        virtual ~BendDeformAction();

        float getBendFactor() const;
        float getCenter() const;
        int getAxis() const;

        virtual void deform(Lattice& lattice);
        virtual void deformInverse(Lattice& lattice);

    protected:
        float bendFactor, center;
        int axis, sign, x, y, z;
        void initAxis();
        virtual void printDA(std::ostream& out) const;
};

#endif // BENDDEFORMACTION_H_INCLUDED
