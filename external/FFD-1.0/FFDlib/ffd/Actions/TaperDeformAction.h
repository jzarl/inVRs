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


#ifndef TAPERDEFORMACTION_H_INCLUDED
#define TAPERDEFORMACTION_H_INCLUDED

#include "DeformAction.h"

/*******************************************************************************
 * @class TaperDeformAction
 * @brief Implements the taper parameters and deformation methods. This
 *        deformation is not invertible.
 */
class TaperDeformAction : public DeformAction
{
    typedef DeformAction Base;

    public:
        /**
         * @param taperFactor determines the scaling of the basis:
         *        1  -> no scale
         *        <1 -> down scale
         *        >1 -> up scale
         *        The top is always set to 0, which is the center of the
         *        scaling plane.
         * @param axis: taper is performed along this axis ([X=1, Y=2, Z=3]).
         *        Values outside of the range are set to the Z-Axis. Negative
         *        axis values cause a taper deformation in the opposite
         *        direction.
         */
        TaperDeformAction(float taperFactor, int axis);
        virtual ~TaperDeformAction();

        float getTaperFactor() const;
        int getAxis() const;

        virtual void deform(Lattice& lattice);
        virtual void deformInverse(Lattice& lattice);

    protected:
        float taperFactor;
        int axis, sign, x, y, z;
        void initAxis();
        virtual void printDA(std::ostream& out) const;
};

#endif // TAPERDEFORMACTION_H_INCLUDED
