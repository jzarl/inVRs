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


#ifndef MODELPOINTMANAGER_H_INCLUDED
#define MODELPOINTMANAGER_H_INCLUDED

#include <vector>

#include <gmtl/Vec.h>

using std::vector;

/*******************************************************************************
 * @class ModelPointManager
 * @brief This class holds all model point data and gives read and write access
 *        to them.
 *
 * This class is the base for the used scene graph and gives access to the
 * model points and the savepoint of model points.
 */
class ModelPointManager
{
    public:
        typedef vector<vector<gmtl::Vec3f> > Vector2d;

       	/**
         * Constructor.
         */
        ModelPointManager();

       	/**
         * Destructor.
         */
        virtual ~ModelPointManager();

       	/**
         * Gives read and write access to the modelPoints array.
         */
        Vector2d& accessModelPoints();

       	/**
         * Gives read access to the saved modelPoints array.
         */
        Vector2d& accessModelPointsSavepoint();


    protected:
        Vector2d modelPoints;
        Vector2d modelPointsSavepoint;

};

#endif // MODELPOINTMANAGER_H_INCLUDED
