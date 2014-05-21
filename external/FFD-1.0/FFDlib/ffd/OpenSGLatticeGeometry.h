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


#ifndef OPENSGLATTICEGEOMETRY_H_INCLUDED
#define OPENSGLATTICEGEOMETRY_H_INCLUDED

#include <gmtl/External/OpenSGConvert.h>

#include <OpenSG/OSGNode.h>
#include <OpenSG/OSGGroup.h>
#include <OpenSG/OSGTransform.h>
#include <OpenSG/OSGGeometry.h>
#include <OpenSG/OSGSimpleGeometry.h>

#if OSG_MAJOR_VERSION >= 2
#include <OpenSG/OSGGeoIndices.h>
#endif

#include "Lattice.h"
#include "LatticeGeometry.h"

/*******************************************************************************
 * @class OpenSGLatticeGeometry
 * @brief Implements the (update and) visualization of the lattice with OpenSG.
 *        Colors for lattice lines and model points can be changed here, the
 *        default values are:
 *          Point color is green - [0,1,0]
 *          Point for user defined subdivisions is light green - [0, 0.5, 0]
 *          Line color is gray - [0.5, 0.5, 0.5]
 *          Selection color is red - [1.0, 0.0, 0.0]
 */
class OpenSGLatticeGeometry : public LatticeGeometry
{
    typedef LatticeGeometry Base;

    public:
    	/**
         * Constructor.
         * Creates a default lattice standard colors. The constructor does only
         * create the lattice datastructure but not the geometry type.
         * @param &lattice is a reference to a lattice description
         */
        OpenSGLatticeGeometry(Lattice& lattice);

        /**
         * Desctructor.
         */
        virtual ~OpenSGLatticeGeometry();

        /**
         * Set the color for lattice points and lines
         */
        void setColor(const OSG::Color3f pointColor, const OSG::Color3f divPointColor,
            const OSG::Color3f lineColor, const OSG::Color3f selectionColor);

        /**
         * Set the colors for the given indices selected.
         * @param indices
         * @param append selection selection or replace current selection
         */
        void setSelected(const vector<gmtl::Vec3i> indices, bool append = false);
        void setSelected(const gmtl::Vec3i indices, bool append);

        /**
         * Set all cellPoints unselected by changing the color back to the
         * default.
         */
        void setUnselected();

        /**
         * Returns a vector holding all indices of the current selection.
         * The indices are sorted by x, then y, then z
         */
        vector<gmtl::Vec3i> getSelection() const;

        /**
         * Returns a pointer to the lattice geometry
         */
        OSG::GeometryPtr getGeometryPtr() const;

        /**
         * Removes the point with given index from the selection.
         */
        bool removeSelected(gmtl::Vec3i index);

        /**
         * Creates a geometry for OSG and returns the GeometryPtr which is ready
         * to use. Uses GL_POINTS and GL_LINES, normals, and one colors for
         * lines and one for points.
         */
        OSG::GeometryPtr createGeometry(const bool all = true);

        /**
         * Updates the geometry after changing any values like a single lattice
         * point or the amount of divisions
         */
        virtual void updateGeometry(const bool all = true);

    private:
        vector <gmtl::Vec3f> verticeLinePoints;
        vector <gmtl::Vec3i> selectedPoints;
        size_t numOfLatticePoints;
        size_t numOfLatticeLinePoints;
        gmtl::Vec3i cellDivisions;
        OSG::GeometryPtr latticeGeo;
#if OSG_MAJOR_VERSION >= 2
        OSG::GeoUInt8PropertyRecPtr latticeTypes;
        OSG::GeoUInt32PropertyRecPtr numOfPointsPerType;
        OSG::GeoPnt3fPropertyRecPtr latticePoints;
        OSG::GeoColor3fPropertyRecPtr latticeColors;
        OSG::GeoUInt32PropertyRecPtr latticeIndices;
#else //OpenSG1:
        OSG::GeoPTypesPtr latticeTypes;
        OSG::GeoPLengthsPtr numOfPointsPerType;
        OSG::GeoPositions3fPtr latticePoints;
        OSG::GeoColors3fPtr latticeColors;
        OSG::GeoIndicesUI32Ptr latticeIndices;
#endif
        OSG::Color3f pointColor;
        OSG::Color3f lineColor;
        OSG::Color3f divPointColor;
        OSG::Color3f selectionColor;
        int step;
        bool showAll;
        size_t dim;

        void changeColor();
        void calcNumOfPoints(const bool all);
        void sortSelection();

        struct compareIndex3 : public std::binary_function<const gmtl::Vec3i&,
            const gmtl::Vec3i&, bool>
        {
            inline bool operator()(const gmtl::Vec3i& left,
                const gmtl::Vec3i right)
            {
                if (left[0] < right[0])
                    return true;
                else if (left[0] == right[0])
                {
                    if (left[1] < right[1])
                        return true;
                    else if (left[1] == right[1])
                        if (left[2] < right[2])
                            return true;
                }
                return false;
            }
        };

};

#endif // OPENSGLATTICEGEOMETRY_H_INCLUDED
