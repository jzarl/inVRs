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


#ifndef LATTICE_H_INCLUDED
#define LATTICE_H_INCLUDED

#include <vector>

#include <gmtl/Vec.h>
#include <gmtl/VecOps.h>
#include <gmtl/AABox.h>
#include <gmtl/Matrix.h>
#include <gmtl/Xforms.h>

#include "Bezier.h"

using std::vector;
using std::string;

/*******************************************************************************
 * @class Lattice
 * @brief The Lattice class implements the lattice datastructure. It provides
 *        functionality to deform the lattice itself and calculates the Bezier
 *        interpolation for a given model vertex which resides inside the
 *        lattice.
 *
 * This lattice is described as a parallelpiped with a certain amount of
 * subdivisions which are refered to by the member latticeDivisions.
 * latticeDivisions can be set by the user.
 * Internally the lattice holds more subdivisions, these are necessary to
 * calculate the Bezier interpolation. The amount of these subdivisions are
 * always the same for all dimensions and are defined by the variable dim.
 * The resulting subdivisions (latticeDivisions * dim) are called cells and the
 * cell divisions are refered to as latticeCellDivision.
 * All points describing a single cell are the control points.
 *
 * The calculateBernstein3d function assumes that an assigned point is within
 * range [0...1]. The Lattice class takes care about this precondition and
 * normalizes a given point for the cell it resides in. The normalized
 * coordinate is called STU coordinate. After calulating the interpolation,
 * the stu coordinate is transformed into the absolute coordinate with respect
 * to the origin which is the lattices' origin.
 */
class Lattice
{
    public:
        typedef vector<vector<gmtl::Vec3f> > Vector2d;
        typedef vector<vector<gmtl::Vec3i> > IndexVector2d;
        typedef vector<vector<vector<gmtl::Vec3f> > > Vector3d;

        /*
        enum
        {
            dim = 3
        };
        */

    	/**
         * Creates a default lattice with one divisions in all dimensions and
         * standard colors. The constructor does only create the lattice
         * datastructure but not the geometry, which depends on the scene graph.
         * The AABB defines the volume for the lattice.
         * @param aabbMin: minimum of axis aligned bounding box
         * @param aabbMax: maximum of axis aligned bounding box
         * @param divisions: amount of [height|width|length] divisions
         */
        Lattice(gmtl::Vec3i divisions, gmtl::AABoxf aabb, size_t dim = 3,
            float epsilon = 0.0001f);

       	/**
         * Destructor.
         */
        ~Lattice() {}

        /**
         * Set the position of a certain cell point
         * @param index of the cell point
         * @return true if access with given index was valid
         */
        bool setCellPoint(const gmtl::Vec3i& index, const gmtl::Vec3f& pos);

        /**
         * Set the position of a certain lattice point.
         * CAUTION: This function does not include interpolation of cell points
         *          between set lattice points and its neighbours (yet)!
         *          Generally setCellPoint is convenient for setting lattice
         *          coordinates.
         * @param index of the lattice point
         * @return true if access with given index was valid
         */
        bool setLatticePoint(const gmtl::Vec3i& index, const gmtl::Vec3f& pos);

        /**
         * Set the lattice divisions. Recalculates the lattice cell corners.
         */
        void setLatticeDivisions(const gmtl::Vec3i divisions);

        /**
         * Set the lattice bounds. Does recalculate the length of divisions.
         * Any lattice visualization will need an update after calling this.
         */
        void setBounds(const gmtl::AABoxf aabb);

        /**
         * Get dimension for internal subdivisions, which is the degree of
         * polynom.
         */
        size_t getDim();

        /**
         * Get the position of a certain cell point
         * @param index of the cell point
         * @return true if access with given index was valid
         */
        bool getCellPoint(const gmtl::Vec3i& index, gmtl::Vec3f& pos) const;

        /**
         * Get the position of a certain cell point of the initial lattice
         * @param index of the cell point
         * @return true if access with given index was valid
         */
        bool getLastCellPoint(const gmtl::Vec3i& index, gmtl::Vec3f& pos) const;

        /**
         * Get the position of a certain lattice point
         * @param index of the lattice point
         * @return true if access with given index was valid
         */
        bool getLatticePoint(const gmtl::Vec3i& index, gmtl::Vec3f& pos) const;

        /**
         * Get the position of a certain lattice point of the initial lattice
         * @param index of the lattice point
         * @return true if access with given index was valid
         */
        bool getLastLatticePoint(const gmtl::Vec3i& index, gmtl::Vec3f& pos)
            const;

        /**
         * Gives access to the cell points.
         * @return reference to the vector holding all cell points
         */
        const Vector3d& getCellPoints() const;

        /**
         * Get amount of [height|width|length] divsions.
         */
        const gmtl::Vec3i getLatticeDivisions() const;

        /**
         * Get length of a [height|width|length] divsion part.
         */
        const gmtl::Vec3f getLatticeDividedLength() const;

        /**
         * Get amount of [height|width|length] divsion parts of a single cell.
         */
        const gmtl::Vec3i getCellDivisions() const;

        /**
         * Get length of [height|width|length] divsions for a single cell.
         */
        const gmtl::Vec3f getCellDividedLength() const;

        /**
         * Returns the control points for a given model point relative to the
         * lattice origin
         */
        void getLatticeRelativeControlPoints(const gmtl::Vec3i index,
            Vector3d& controlPoints);

        /**
         * @return index of the nearest model point to the given position
         */
        gmtl::Vec2i getNearestNeighbour(gmtl::Vec3f position);

        /**
         * @return index of the nearest lattice cell point to the given position
         */
        gmtl::Vec3i getNearestNeighbourLattice(gmtl::Vec3f position);

        /**
         * Gives full access to the cell points.
         * @return reference to the vector holding all cell points
         */
        Vector3d& accessCellPoints();

        /**
         * Calculates the model point -to-> cell assignment. The cell is the
         * lattice cell, as defined by construction, here.
         * Assignment is always done on the lastCellPoints, not on deformed
         * lattice cellPoints.
         */
        void assignModelPointsToLatticeCells(Vector2d& modelPoints);

        /**
         * Copy current cell points to lastCellPoints.
         */
        void shiftLastCellPoints();

        /**
         * Calculates STU coordinates which can be used for a bezier calculation
         * . STU coordinates are relative to the lattice cell in which the point
         * resides and normalized in respect to the cell division length.
         */
        void transformStuRelative(gmtl::Vec3f& stu, const gmtl::Vec3i index);

        /**
         * Calculates the absolute coordinate for a given STU regarding the cell
         * and the lattice origin.
         */
        void transformStuAbsolute(gmtl::Vec3f& stu, const gmtl::Vec3i index);

        /**
         * Executes FFD on each modelpoint by transforming the modelpoint to its
         * STU representation, executing the FFD, and transforming it back to
         * its absolute value.
         */
        void executeFFD(Vector2d& modelPointsSavepoint, Vector2d& modelPoints);

        /**
         * Calculates the center of the AABB und give a vector move which
         * holds the difference from the lattice position to the centered.
         */
        void centerOrigin(gmtl::Vec3f& min, gmtl::Vec3f& max,
                gmtl::Vec3f& move) const;

        /**
         * Moves a point about vector move, starting from the original position.
         */
        void moveTo(const gmtl::Vec3f& cellPointOrigPos,
                gmtl::Vec3f& cellPoint, const gmtl::Vec3f& move);

        /**
         * Moves a point about vector move back to its original position.
         */
        void moveBack(const gmtl::Vec3f& cellPoint,
                gmtl::Vec3f& cellPointOrigPos, const gmtl::Vec3f& move) const;

    private:
        gmtl::Vec3f aabbMin;
        gmtl::Vec3f aabbMax;
        const int maxDivisions;
        int numOfCells;
        gmtl::Vec3f distances;
        gmtl::Vec3i cellDivisions;
        gmtl::Vec3f cellDividedLength;
        gmtl::Vec3i latticeDivisions;
        gmtl::Vec3f latticeDividedLength;
        Vector3d cellPoints;
        Vector3d lastCellPoints;
        IndexVector2d modelPointIndex;
        Vector3d controlPoints;
        Vector2d* mp;
        float epsilon;
        size_t dim;

        gmtl::Vec3f getDistancePerAxis(const gmtl::Vec3f& min,
                const gmtl::Vec3f& max) const;

        void calculateCellPoints();

        bool checkCellIndex(const gmtl::Vec3i& index) const;

        bool checkLatticeIndex(const gmtl::Vec3i& index) const;

        void printPoint(string name, gmtl::Vec3f point);
        void printPoint(string name, gmtl::Vec3i point);
        void printIndex(int from, int to);
        void printAllPoints();
};

#endif // LATTICE_H_INCLUDED
