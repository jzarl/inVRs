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


#ifndef LATTICEGEOMETRY_H_INCLUDED
#define LATTICEGEOMETRY_H_INCLUDED

#include "Lattice.h"

/*******************************************************************************
 * @class LatticeGeometry
 * @brief The LatticeGeometry is a base class for implementations of the
 *        lattice visualization.
 *
 * This class is a lattice visualization base class for the used scene graph.
 */
class LatticeGeometry
{
    public:
       	/**
         * Constructor.
         * @param lattice
         */
        LatticeGeometry(Lattice& lattice);

       	/**
         * Destructor.
         */
        virtual ~LatticeGeometry();

        /**
         * Updates the geometry after changing any values like a single lattice
         * point or the amount of divisions.
         * @param all is true if inner subdivisions of the lattice should be
         *        rendered or false if only the user set divisions should be
         *        shown
         */
        virtual void updateGeometry(const bool all) = 0;


    protected:
        Lattice& lattice;

};

#endif // LATTICEGEOMETRY_H_INCLUDED
