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


#ifndef GLOBALDEFORMACTION_H_INCLUDED
#define GLOBALDEFORMACTION_H_INCLUDED

#include <gmtl/Matrix.h>

#include "DeformAction.h"

/*******************************************************************************
 * @class GlobalDeformAction
 * @brief Implements a deformation with customized matrices. Actually the
 *        gmtl::xform function is called for each lattice cell corner after
 *        setting the lattice to the origin (center) optionally.
 *        This deformation is invertible.
 */
class GlobalDeformAction : public DeformAction
{
    typedef DeformAction Base;

    public:
        /**
         * @param dMatrix is a customized matrix which is applied to all
         *        lattice corners by calling gmtl::xform
         * @param center determines whether the lattice should be translated to
         *        the origin before deformation or not
         */
        GlobalDeformAction(gmtl::Matrix44f dMatrix,
            bool center);
        ~GlobalDeformAction();

        void setMatrix(const gmtl::Matrix44f dMatrix);
        const gmtl::Matrix44f getMatrix() const;
        bool isCenter();

        virtual void deform(Lattice& lattice);
        virtual void deformInverse(Lattice& lattice);

    protected:
        gmtl::Matrix44f dMatrix;
        bool center;
        virtual void printDA(std::ostream& out) const;
};

#endif // GLOBALDEFORMACTION_H_INCLUDED
