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


#ifndef OPENSGMODELPOINTMANAGER_H_INCLUDED
#define OPENSGMODELPOINTMANAGER_H_INCLUDED

#include <vector>

#include <assert.h>

#include <gmtl/Vec.h>
#include <gmtl/VecOps.h>
#include <gmtl/AABox.h>
#include <gmtl/Containment.h>

#include <OpenSG/OSGNode.h>
#include <OpenSG/OSGGeometry.h>
#include <OpenSG/OSGTransform.h>
#include <OpenSG/OSGMatrix.h>
#include <OpenSG/OSGGeoFunctions.h>
#if OSG_MAJOR_VERSION < 2
#include <OpenSG/OSGSimpleAttachments.h>
#endif

#include "ModelPointManager.h"

using std::vector;

/*******************************************************************************
 * @class OpenSGModelPointManager
 * @brief Implements the ModelPointManager for the OpenSG scene graph.
 *
 * The model point collection and assignment functions assume that the lattice
 * is to alter all model points of all models starting with the assigned OpenSG
 * node.
 * OpenSGModelPointManager provides functions with the postfix "Masked". These
 * functions give more control on which points may be altered. Only point lying
 * exactly inside a user-defined axis aligned box are affected.
 */
class OpenSGModelPointManager : public ModelPointManager
{
    public:
        typedef vector<vector<bool> > Vector2dMask;

       	/**
         * Constructor.
         * @param node is the root point for all implemented functions which
         *        require scene graph traversal.
         *        The lattice geometry node has to be above this node.
         */
        OpenSGModelPointManager(OSG::NodePtr node);

       	/**
         * Destructor.
         */
        virtual ~OpenSGModelPointManager();

        /**
         * @return true if only model points inside a user-defined axis aligned
         *         box are affected
         */
        bool getMasked();

        /**
         * @return the current axis aligned box for affected model points
         */
        gmtl::AABoxf getAffectedVolume();

        /**
         * Read model vertices:
         * Fills the modelPoints array with points of all models for the sub
         * scenegraph.
         */
        void collectModelPoints();

        /**
         * Read masked model vertices:
         * Fills the modelPoints array with all points of all models for
         * the sub scene graph, but performs a check on each point against the
         * given AABB. If a point is inside the box, the mask flag for the
         * appropriate point is set true.
         */
        void collectModelPointsMasked(const gmtl::AABoxf& aabb);

        /**
         * Write model vertices:
         * Overwrites the points of the model with modelPoints.
         */
        void assignModelPoints();

        /**
         * Write masked model vertices:
         * Overwrites the points of the model with modelPoints, using the
         * appropriate mask.
         */
        void assignModelPointsMasked();

        /**
         * Creates a copy of the current model points, which are the modelpoints
         * of the last collect call. Only one copy per instance is allowed.
         */
        bool createSavepoint();

        /**
         * Restore the savepoint.
         */
        bool restoreSavepoint();

        /**
         * Remove the savepoint.
         */
        bool deleteSavepoint();

    private:
        /**
         * Disable the copy constructor in order to avoid different instances
         * with access to the same model points.
         */
        OpenSGModelPointManager(const OpenSGModelPointManager&);
        OpenSGModelPointManager& operator=(const OpenSGModelPointManager&);

        const OSG::NodePtr node;
        size_t numOfGeometryNodes;
        size_t model;
        bool isMasked;
        OSG::Matrix tm;
        Vector2dMask modelPointsAffected;
        gmtl::AABoxf affectedVolume;

#if OSG_MAJOR_VERSION >= 2
		OSG::Action::ResultE getNumOfGeometryNodes(OSG::Node* const node);
        OSG::Action::ResultE collectGeometryNodePoints(OSG::Node* const node);
        OSG::Action::ResultE collectGeometryNodePointsMasked(OSG::Node* const node);
        OSG::Action::ResultE assignGeometryNodePoints(OSG::Node* const node);
        OSG::Action::ResultE assignGeometryNodePointsMasked(OSG::Node* const node);
        OSG::Action::ResultE recalculateNormals(OSG::Node* const node);
        OSG::Action::ResultE leave(OSG::Node* const node, OSG::Action::ResultE result);
#else
		OSG::Action::ResultE getNumOfGeometryNodes(OSG::NodePtr& node);
        OSG::Action::ResultE collectGeometryNodePoints(OSG::NodePtr& node);
        OSG::Action::ResultE collectGeometryNodePointsMasked(OSG::NodePtr& node);
        OSG::Action::ResultE assignGeometryNodePoints(OSG::NodePtr& node);
        OSG::Action::ResultE assignGeometryNodePointsMasked(OSG::NodePtr& node);
        OSG::Action::ResultE recalculateNormals(OSG::NodePtr& node);
        OSG::Action::ResultE leave(OSG::NodePtr& node, OSG::Action::ResultE result);
#endif 
        
};

#endif // OPENSGMODELPOINTMANAGER_H_INCLUDED
