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


#ifndef OPENSGDEFORMACTIONMANAGER_H_INCLUDED
#define OPENSGDEFORMACTIONMANAGER_H_INCLUDED

#include <deque>
#include <vector>

#include <gmtl/Vec.h>
#include <gmtl/Matrix.h>
#include <gmtl/AABox.h>

#include <OpenSG/OSGNode.h>
#include <OpenSG/OSGSimpleSceneManager.h>

#include "Lattice.h"
#include "OpenSGModelPointManager.h"
#include "Actions/DAction.h"
#include "Actions/DeformAction.h"
#include "OpenSGLatticeActionDeque.h"
#include "OpenSGLatticeGeometry.h"

using std::deque;
using std::vector;
using std::cout;
using std::endl;

/**
 * @class OpenSGDeformActionManager
 *
 * @brief This class is a singleton which manages all lattices and actions for
 *        all nodes of the scene graph.
 *
 * The idea of this class is to manage all actions on the lattice
 * centralized, so that the user does not have to handle it (e.g. node creation
 * when inserting a lattice).
 *
 * The OpenSGDeformActionManager provides the following deforming actions:
 * First of all there are functions to insert and remove a lattice. After
 * deforming the lattice executeFFD needs to be called in order to interpolate
 * model points according to the lattice.
 * Lattice deformation actions performed on the entire lattice are: @see bend,
 * @see twist, @see taper, and @see deform. Then @see setPoint can be used to
 * displace a single lattice point, functions to select more lattice points and
 * giving access to the selection are implemented.
 *
 * OpenSGDeformActionManager provides the possibility to undo actions:
 * InsertLattice: Undo supported
 * RemoveLattice: Only supported if this remove has been done after insert or
 *                execute
 * Execute: Undo only supported if last action was insert or execute (does
 *          not affect modelpoints (visually)
 * Deform: Undoing actions are supported as long as an action implements the
 *         deformInverse function and sets isInvertible to true. If one action
 *         is not invertible, undoing will do nothing for any further undo
 *         calls.
 *
 * There are some restrictions which are implemented in this class:
 * First of all, a node may have exactly one lattice. Next, if a lattice should
 * be inserted, a check, whether a lattice exists in the subtree, is made. If
 * a lattice has been found, no lattice will be inserted.
 * The most important restriction is that no node in the subScene, having a
 * lattice, must not be changed in the following way:
 * Do not change the amount of model points of a node which is affected by the
 * lattice.
 * Do not change the position of a node in the subScene which has a lattice
 * superimposed, except the movement of a node does not affect the AABB or the
 * lattice gets reset.
 * If the entire subScene should be moved, get the lattice node and move that
 * instead of the subScene.
 */
class OpenSGDeformActionManager
{
    typedef vector<OpenSGLatticeActionDeque* > Davec;

    public:
       	/**
         * Create and get singleton instance of the static
         * OpenSGDeformActionManager.
         */
        static OpenSGDeformActionManager* GetInstance()
        {
            if (osgDamInstance == 0)
            {
                osgDamInstance = new OpenSGDeformActionManager();
                osgDamInstance->latticeExists = false;
                osgDamInstance->doOptimize = true;
                osgDamInstance->latticeNodeName = std::string("LatticeNode");
            }
            return osgDamInstance;
        }

       	/**
         * Destroy the instance of OpenSGDeformActionManager and clean up
         * allocated memory of all classes in use.
         */
        static void Destroy();

        /**
         * Toggle the visualization for internal subdivisions.
         */
        void setShowAll(OSG::NodePtr subScene, bool showAll);

        /**
         * Toggles the execution calculation to be either performed on the
         * lattice solely or on both, the lattice and the model, by setting
         * @param doExecute If doExecute is true, the deformation will call
         *        executeFFD for each deformation action done by the user, but
         *        no execute actions will be inserted into the action deque
         *        until executeFFD is called explicitly.
         * If instant execution is toggled @see executeFFD is called, previous
         * deformations to the lattice are applied to the model, and the lattice
         * is reset to the AABB. Then, if doExecute is set to true, the
         * @see OpenSGModelPointManager is assigned to create a copy of all
         * model points of the subScene, called a savepoint, which is necessary
         * for the FFD calculation, especially if the deformation should not be
         * applied to the model at the end.
         * Setting doExecute to true and later back to false will not delete the
         * created savepoint (backup of the modelpoints).
         */
        void setInstantExecution(OSG::NodePtr subScene, bool doExecute);

       	/**
         * @param doOptimize is true by default. Setting to false causes the
         *        manager to keep each action.
         * doOptimize will merge certain actions, like: setting a point to three
         * different positions in a row could be merged to one setPoint call.
         * At the moment optimization is not in use.
         */
        void setOptimize(bool doOptimize);

        /**
         * Set the position of a single lattice cell point. The lattice cell
         * point index refers to the internal subdivisions of a lattice.
         */
        void setPoint(OSG::NodePtr subScene, const gmtl::Vec3i index,
            const gmtl::Vec3f position);

        /**
         * Set the position of a vector of lattice cell points to different
         * positions. Index and position vector have to be of the same length.
         * @param indices of the cell vertices
         * new @param positions for the cell vertices
         */
        void setPoints(OSG::NodePtr subScene, const vector<gmtl::Vec3i>& indices,
            const vector<gmtl::Vec3f>& positions);

        /**
         * Add the same vector to all lattice cell points.
         */
        void setPoints(OSG::NodePtr subScene, const vector<gmtl::Vec3i>& indices,
            const gmtl::Vec3f addPosition);

        /**
         * @return cellDivisions Is the number of the set lattice
         * divisions * internal subdivisions (3 per default)
         */
        gmtl::Vec3i getCellDivisions(OSG::NodePtr subScene);

        /**
         * @return a copy of the current selection indices
         */
        vector<gmtl::Vec3i> getSelection(OSG::NodePtr subScene);

        /**
         * Calculate and
         * @return axis aligned bounding box for the given subScene
         */
        gmtl::AABoxf getAabb(OSG::NodePtr subScene);

        /**
         * @return the hit point of the intersection test
         */
        bool getHitPoint(OSG::SimpleSceneManager* mgr, OpenSGLatticeActionDeque*
            daDeque, int x, int y, gmtl::Vec3f& hitPoint);

        /**
         * @return selection index of the nearest neighbour of the given point
         */
        gmtl::Vec3i getNearestNeighbourIndex(gmtl::Vec3f& selection,
            OSG::NodePtr subScene);

        /**
         * @return number of currently managed action deques
         */
        int getNumOfDeques() const;

        /**
         * @return a pointer to the action deque for the given subScene
         */
        OpenSGLatticeActionDeque* getDaDeque(OSG::NodePtr subScene);

        /**
         * Evaluates the position of a cell corner with the given index
         * @return true if intersecion was successful
         */
        bool getPoint(OSG::NodePtr subScene, const gmtl::Vec3i& index,
            gmtl::Vec3f& position);

        /**
         * @return a pointer to the action deque with the given index
         */
        const OpenSGLatticeActionDeque* getDequeByIndex(int index) const;

        /**
         * @return optimization flag
         */
        bool getOptimize();

        /**
         * @return doExecute
         */
        bool isInstantExecution(OSG::NodePtr subScene);

        /**
         * Do optimization of all action deques
         */
        int optimize();

       	/**
         * Insert a new lattice above the given subScene if the subScene node
         * and none of its children or parents have a lattice inserted, a new
         * lattice will be created and the given node will be managed by
         * OpenSGDeformActionManager.
         * If the subScene node is manged already, this method verifies that the
         * last action was removeLattice and no node under subScene has a
         * lattice inserted before inserting.
         * The parameters
         * @param height
         * @param width
         * @param length set the lattice divisions in each dimension.
         * @param dim determines the polynomial degree.
         * @param isMasked: If set to true, only model vertices of the subscene
         *        which are inside the given aabb are affected.
         *        The default value for isMasked is false. In this case the AABB
         *        is ignored and found internally, so that all model vertices of
         *        the subscene reside in the bounding box.
         * @param epsilon is a threshold which extends the AABB in each
         *        dimension before calculating the relatice model vertex
         *        coordinates. This is necessary because of possible floating
         *        point imprecisions. The default value should be sufficient,
         *        but needs to be increased if any assertions regarding cell
         *        indices occur.
         */
        bool insertLattice(OSG::NodePtr subScene, int height, int width, int length,
            size_t dim = 3, bool isMasked = false,
            gmtl::AABoxf aabb = gmtl::AABoxf(gmtl::Vec3f(0, 0, 0),
                                             gmtl::Vec3f(0, 0, 0)),
            float epsilon = 0.0001f);

        /**
         * Removes the lattice from the given node if the lattice is exactly one
         * hierachy above, as inserted by insertLattice.
         */
        bool removeLattice(OSG::NodePtr subScene);

        /**
         * Executes the free-form deformation on all affected model vertices.
         * @param subScene model which should be deformed
         */
        void executeFfd(OSG::NodePtr subScene);

        /**
         * Bends an object with bending angle in rad at the given center of
         * bend along the bending axis.
         * @param center of bend has to be in range [0, 1]:
         *        0: bottom
         *        0.5: center
         *        1: top of the bending axis.
         * The bend operation is not invertible.
         */
        void bend(OSG::NodePtr subScene, const float bendFactor, const float center,
            const int axis);

        /**
         * Twist an object with the given twistFactor in rad along the given
         * axis. A twistFactor of 2\pi defines a full twist.
         * The twist operation is invertible.
         */
        void twist(OSG::NodePtr subScene, const float twistFactor, const int axis);

        /**
         * Taper an object with the given taperFactor along the given axis. The
         * top of the lattice is set to the center. The basis of the lattice is
         * scaled along the choosen axis, determined by the
         * @param taperFactor
         *        1: no scale
         *        <1: scale down
         *        >1: scale up
         * The taper operation is not invertible.
         */
        void taper(OSG::NodePtr subScene, const float taperFactor, const int axis);

        /**
         * Deforms the lattice with a customized matrix (dMatrix). Actually each
         * point of the lattice is multiplied with the given matrix, using the
         * GMTL method xform.
         * @param subScene
         * @param dMatrix
         * @param center is true per default and does center the nodes to the
         *        origin for symmetric deformation
         * The customized deformation is invertible.
         */
        void deform(OSG::NodePtr subScene, const gmtl::Matrix44f dMatrix,
            const bool center = true);

        /**
         * Reverts the last action if possible
         */
        bool undo(OSG::NodePtr subScene);

        /**
         * Updates the visualization of the lattice geometry, mainly called
         * internally if anything on the lattice changes (color, position,...)
         */
        void update(OpenSGLatticeActionDeque& daDeque);

        /**
         * Selects the lattice points and changes the color for visualization
         * @param selectPoints are the indices of the lattice points
         * @param subScene
         * @param append is true per default and appends the selection to the
         *        last selection
         */
        void selectLatticeCellPoints(const vector<gmtl::Vec3i>& selectPoints,
            OSG::NodePtr subScene, bool append = true);

        /**
         * Tries to select a lattice point for the given mouse coordinates x, y
         * by performing the OpenSG intersection test. If the subScene gets hit,
         * the nearest lattice cell point gets calculated and selected.
         * @param mgr
         * @param subScene
         * @param append determines whether the selected point should replace or
         *        be added to the current selection
         * @return true if a lattice cell point can be selected
         */
        bool selectLatticeCellPoint(OSG::SimpleSceneManager* mgr, OSG::NodePtr subScene,
            int x, int y, bool append = true);

        /**
         * Clears the selection and updates the visualization by reseting the
         * color for the lattice corners.
         */
        void unselectLatticeCellPoints(OSG::NodePtr subScene);

        /**
         * Tries to unselect a lattice point by performing the same intersection
         * test as @see selectLatticeCellPoint does, but removes the lattice
         * corner from the selection list if the hit succeeds.
         * @return true if unselecting was successful
         */
        bool unselectLatticeCellPoint(OSG::SimpleSceneManager* mgr, OSG::NodePtr subScene,
            int x, int y);

        /**
         * Print all actions with all paramteres for the entire action deque.
         * Output includes also nodes which had a lattice in the past.
         */
        void dump();

        /**
         * Print all actions with all paramteres for the given subscene.
         */
        void dump(OSG::NodePtr subScene);

    protected:
        ~OpenSGDeformActionManager()
        {
        }
        OpenSGDeformActionManager()
        {
        }
        OpenSGDeformActionManager(const OpenSGDeformActionManager& dam);
        OpenSGDeformActionManager& operator=(OpenSGDeformActionManager const&);

        bool latticeExists;
        bool doOptimize;
        static OpenSGDeformActionManager* osgDamInstance;
        Davec deformNodes;
        std::string latticeNodeName;

        bool hasSubSceneLattice(OSG::NodePtr subScene);
        bool hasParentLattice(OSG::NodePtr subScene);
        bool isDeformActionAllowed(OpenSGLatticeActionDeque* daDeque);
        bool isInsertActionAllowed(OpenSGLatticeActionDeque* daDeque);
        bool isRemoveActionAllowed(OpenSGLatticeActionDeque* daDeque);
        bool isInvertible(int fromInd, int toInd,
            OpenSGLatticeActionDeque& daDeque);
        bool undoDaction(const DAction* dAction, OSG::NodePtr subScene,
            OpenSGLatticeActionDeque& daDeque);
        bool undoDeformAction(OSG::NodePtr subScene, DeformAction& deformAction,
            OpenSGLatticeActionDeque& daDeque);
        void handleAction(OSG::NodePtr subScene, DeformAction* dAction,
            OpenSGLatticeActionDeque* daDeque);

        Lattice* insert(OSG::NodePtr subScene, const int height, const int width,
            const int length, size_t dim, const bool isMasked,
            const gmtl::AABoxf& aabb, float epsilon,
            OpenSGLatticeActionDeque& daDeque);
        bool remove(OSG::NodePtr subScene);
        gmtl::AABoxf execute(OSG::NodePtr subScene,
            OpenSGLatticeActionDeque& daDeque, bool resetAabb = false);

        void createSavePoint(OSG::NodePtr subScene);
        void deleteSavePoint(OSG::NodePtr subScene);
        void restoreSavePoint(OSG::NodePtr subScene);

#if OSG_MAJOR_VERSION >= 2
		OSG::Action::ResultE findLattice(OSG::Node* node);
#else
		OSG::Action::ResultE findLattice(OSG::NodePtr& node);
#endif
        
};

#endif // OPENSGDEFORMACTIONMANAGER_H_INCLUDED
