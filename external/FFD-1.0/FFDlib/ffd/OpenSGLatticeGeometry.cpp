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


#include "OpenSGLatticeGeometry.h"

#include <OpenSG/OSGGeoFunctions.h>
#include <OpenSG/OSGNode.h>
#include <OpenSG/OSGSimpleMaterial.h>
#if OSG_MAJOR_VERSION < 2
#include <OpenSG/OSGSimpleAttachments.h>
#endif

#include <gmtl/Vec.h>
#include <gmtl/VecOps.h>

#if OSG_MAJOR_VERSION >= 2
// quick'n'dirty hack to disable ...editCP statements in OpenSG2:
#define beginEditCP(foo...)
#define endEditCP(foo...)
#endif

OSG_USING_NAMESPACE

OpenSGLatticeGeometry::OpenSGLatticeGeometry(Lattice& lattice) : Base(lattice)
{
    pointColor = Color3f(0.0f, 1.0f, 0.0f);
    lineColor = Color3f(0.5f, 0.5f, 0.5f);
    divPointColor = Color3f(0.0f, 0.5f, 0.0f);
    selectionColor = Color3f(0.8f, 0.1f, 0.1f);
    dim = lattice.getDim();
    step = dim;
    showAll = false;
}

OpenSGLatticeGeometry::~OpenSGLatticeGeometry()
{
}

void OpenSGLatticeGeometry::setColor(const Color3f pointColor,
    const Color3f divPointColor, const Color3f lineColor,
    const Color3f selectionColor)
{
    this->pointColor = pointColor;
    this->divPointColor = divPointColor;
    this->lineColor = lineColor;
    this->selectionColor = selectionColor;
    changeColor();
}

void OpenSGLatticeGeometry::setSelected(const vector<gmtl::Vec3i> indices,
    bool append)
{
    if (!append)
        selectedPoints.clear();

    for (size_t i = 0; i < indices.size(); i++)
        setSelected(indices[i], true);

    changeColor();
}

void OpenSGLatticeGeometry::setSelected(const gmtl::Vec3i indices,
    bool append)
{
    if (!append)
        selectedPoints.clear();

    size_t sSize = selectedPoints.size();
    bool inserted = false;

    for (vector<gmtl::Vec3i>::iterator it = selectedPoints.begin();
        it != selectedPoints.end(); ++it)
    {
        if (indices[0] < (*it)[0])
        {
            selectedPoints.insert(it, indices);
            inserted = true;
            ++sSize;
            break;
        }
        if (indices[0] == (*it)[0])
        {
            if (indices[1] < (*it)[1])
            {
                selectedPoints.insert(it, indices);
                inserted = true;
                ++sSize;
                break;
            }
            if (indices[1] == (*it)[1])
            {
                if (indices[2] < (*it)[2])
                {
                    selectedPoints.insert(it, indices);
                    inserted = true;
                    ++sSize;
                    break;
                }
                if (indices[2] == (*it)[2])
                {
                    inserted = true;
                    break;
                }
            }
        }
    }
    if (!inserted)
    {
        selectedPoints.push_back(indices);
        ++sSize;
    }

    changeColor();
}

void OpenSGLatticeGeometry::setUnselected()
{
    selectedPoints.clear();
    changeColor();
}

vector<gmtl::Vec3i> OpenSGLatticeGeometry::getSelection() const
{
    return selectedPoints;
}

GeometryPtr OpenSGLatticeGeometry::getGeometryPtr() const
{
    return latticeGeo;
}

bool OpenSGLatticeGeometry::removeSelected(gmtl::Vec3i index)
{
    for(vector<gmtl::Vec3i>::iterator it = selectedPoints.begin();
        it != selectedPoints.end(); ++it)
    {
        if (*it == index)
        {
            selectedPoints.erase(it);
            changeColor();
            return true;
        }
    }
    return false;
}

GeometryPtr OpenSGLatticeGeometry::createGeometry(const bool all)
{
    showAll = all;
    latticeGeo = Geometry::create();
#if OSG_MAJOR_VERSION < 2
	// 2013-08-27 ZaJ: is this portable to OpenSG2? I've got no idea where setName is coming from...
    setName(latticeGeo, "LatticeGeometry");
#endif
#if OSG_MAJOR_VERSION >= 2
    latticeTypes = GeoUInt8Property::create();
    latticePoints = GeoPnt3fProperty::create();
    latticeIndices = GeoUInt32Property::create();
    latticeColors = GeoColor3fProperty::create();
    numOfPointsPerType = GeoUInt32Property::create();
#else // OpenSG1.8:
    latticeTypes = GeoPTypesUI8::create();
    latticePoints = GeoPositions3f::create();
    latticeIndices = GeoIndicesUI32::create();
    latticeColors = GeoColors3f::create();
    numOfPointsPerType = GeoPLengthsUI32::create();
#endif

    SimpleMaterialPtr mat = SimpleMaterial::create();
    beginEditCP(mat, SimpleMaterial::LitFieldMask);
        mat->setLit(false);
    endEditCP(mat, SimpleMaterial::LitFieldMask);

    latticeGeo->setMaterial(mat);

    // define OPEN_GL types for lattice
    beginEditCP(latticeTypes, GeoPTypesUI8::GeoPropDataFieldMask);
        latticeTypes->addValue(GL_POINTS);
        latticeTypes->addValue(GL_LINES);
    endEditCP(latticeTypes, GeoPTypesUI8::GeoPropDataFieldMask);

    calcNumOfPoints(all);

    // define number of points for each OPEN_GL type for lattice
    beginEditCP(numOfPointsPerType, GeoPLengthsUI32::GeoPropDataFieldMask);
        numOfPointsPerType->addValue(numOfLatticePoints);
        numOfPointsPerType->addValue(numOfLatticeLinePoints);
    endEditCP(numOfPointsPerType, GeoPLengthsUI32::GeoPropDataFieldMask);

    updateGeometry(all);

    return latticeGeo;
}

void OpenSGLatticeGeometry::updateGeometry(const bool all)
{
    showAll = all;
    calcNumOfPoints(all);

    // set number of points for each OPEN_GL type for lattice
    beginEditCP(numOfPointsPerType, GeoPLengthsUI32::GeoPropDataFieldMask);
        numOfPointsPerType->setValue(numOfLatticePoints, 0);
        numOfPointsPerType->setValue(numOfLatticeLinePoints, 1);
    endEditCP(numOfPointsPerType, GeoPLengthsUI32::GeoPropDataFieldMask);

    cellDivisions = lattice.getCellDivisions();
    const Lattice::Vector3d& cellPoints = lattice.getCellPoints();

    // create all used vertices for the lattice (gl_points)
    beginEditCP(latticePoints, GeoPositions3f::GeoPropDataFieldMask);
        latticePoints->clear();

        // vertices for gl_points
        for (int h = 0; h <= cellDivisions[0]; h+=step)
            for (int w = 0; w <= cellDivisions[1]; w+=step)
                for (int l = 0; l <= cellDivisions[2]; l+=step)
                    latticePoints->addValue(Pnt3f(cellPoints[h][w][l][0],
                                                  cellPoints[h][w][l][1],
                                                  cellPoints[h][w][l][2]));

        // vertices for gl_lines
        for (int h = 0; h < cellDivisions[0]; h+=step)
            for (int w = 0; w <= cellDivisions[1]; w+=step)
                for (int l = 0; l <= cellDivisions[2]; l+=step)
                {
                    latticePoints->addValue(Pnt3f(cellPoints[h][w][l][0],
                                                  cellPoints[h][w][l][1],
                                                  cellPoints[h][w][l][2]));
                    latticePoints->addValue(Pnt3f(cellPoints[h + step][w][l][0],
                                                  cellPoints[h + step][w][l][1],
                                                  cellPoints[h + step][w][l][2])
                                                  );
                }
        for (int h = 0; h <= cellDivisions[0]; h+=step)
            for (int w = 0; w < cellDivisions[1]; w+=step)
                for (int l = 0; l <= cellDivisions[2]; l+=step)
                {
                    latticePoints->addValue(Pnt3f(cellPoints[h][w][l][0],
                                                  cellPoints[h][w][l][1],
                                                  cellPoints[h][w][l][2]));
                    latticePoints->addValue(Pnt3f(cellPoints[h][w + step][l][0],
                                                  cellPoints[h][w + step][l][1],
                                                  cellPoints[h][w + step][l][2])
                                                  );
                }
        for (int h = 0; h <= cellDivisions[0]; h+=step)
            for (int w = 0; w <= cellDivisions[1]; w+=step)
                for (int l = 0; l < cellDivisions[2]; l+=step)
                {
                    latticePoints->addValue(Pnt3f(cellPoints[h][w][l][0],
                                                  cellPoints[h][w][l][1],
                                                  cellPoints[h][w][l][2]));
                    latticePoints->addValue(Pnt3f(cellPoints[h][w][l + step][0],
                                                  cellPoints[h][w][l + step][1],
                                                  cellPoints[h][w][l + step][2])
                                            );
                }
    endEditCP(latticePoints, GeoPositions3f::GeoPropDataFieldMask);

    // create all indices for lattice <-> vertex assignment
    beginEditCP(latticeIndices, GeoIndicesUI32::GeoPropDataFieldMask);
        latticeIndices->clear();
        for (size_t i = 0; i < numOfLatticePoints; ++i)
            latticeIndices->addValue(i);
        for (size_t i = 0; i < numOfLatticeLinePoints; ++i)
            latticeIndices->addValue(numOfLatticePoints + i);
    endEditCP(latticeIndices, GeoIndicesUI32::GeoPropDataFieldMask);

    // assign vertices, colors, indices to geometry
#if OSG_MAJOR_VERSION >= 2
	beginEditCP(latticeGeo);
#else
	beginEditCP(latticeGeo, Geometry::TypesFieldMask |
                            Geometry::LengthsFieldMask |
                            Geometry::PositionsFieldMask |
                            Geometry::IndicesFieldMask |
                            Geometry::ColorsFieldMask);	
#endif
        latticeGeo->setTypes(latticeTypes);
        latticeGeo->setLengths(numOfPointsPerType);
        latticeGeo->setIndices(latticeIndices);
        latticeGeo->setPositions(latticePoints);
        latticeGeo->setColors(latticeColors);
        calcVertexNormals(latticeGeo);
#if OSG_MAJOR_VERSION >= 2
	 endEditCP(latticeGeo);
#else
	 endEditCP(latticeGeo, Geometry::TypesFieldMask |
                          Geometry::LengthsFieldMask |
                          Geometry::PositionsFieldMask |
                          Geometry::IndicesFieldMask |
                          Geometry::ColorsFieldMask);
#endif
   

    changeColor();
}

void OpenSGLatticeGeometry::changeColor()
{
    size_t nextSel = 0;

    // create all used colors for the lattice
    beginEditCP(latticeColors, GeoColors3f::GeoPropDataFieldMask);
        // in case the geometry has changed
        latticeColors->clear();

        if (step == 1)
            for (int h = 0; h <= cellDivisions[0]; h+=step)
                for (int w = 0; w <= cellDivisions[1]; w+=step)
                    for (int l = 0; l <= cellDivisions[2]; l+=step)
                        if ((nextSel < selectedPoints.size()) &&
                            (h == selectedPoints[nextSel][0]) &&
                            (w == selectedPoints[nextSel][1]) &&
                            (l == selectedPoints[nextSel][2]))
                        {
                            latticeColors->addValue(selectionColor);
                            ++nextSel;
                        }
                        else if (h % dim || w % dim || l % dim)
                            latticeColors->addValue(divPointColor);
                        else
                            latticeColors->addValue(pointColor);
        else
            for (size_t i = 0; i < numOfLatticePoints; ++i)
                latticeColors->addValue(pointColor);

        for (size_t i = 0; i < numOfLatticeLinePoints; ++i)
            latticeColors->addValue(lineColor);

    endEditCP(latticeColors, GeoColors3f::GeoPropDataFieldMask);
}

void OpenSGLatticeGeometry::calcNumOfPoints(const bool all)
{
    gmtl::Vec3i divisions;

    if (all)
    {
        step = 1;
        divisions = cellDivisions;
    }
    else
    {
        step = dim;
        divisions = lattice.getLatticeDivisions();
    }
    numOfLatticePoints = (divisions[0] + 1) *
                         (divisions[1] + 1) *
                         (divisions[2] + 1);

    numOfLatticeLinePoints =  (divisions[0]) *
                              (divisions[1] + 1) *
                              (divisions[2] + 1);

    numOfLatticeLinePoints += (divisions[0] + 1) *
                              (divisions[1]) *
                              (divisions[2] + 1);

    numOfLatticeLinePoints += (divisions[0] + 1) *
                              (divisions[1] + 1) *
                              (divisions[2]);

    numOfLatticeLinePoints *= 2;
}

void OpenSGLatticeGeometry::sortSelection()
{
    std::sort(selectedPoints.begin(), selectedPoints.end(), compareIndex3());
}
