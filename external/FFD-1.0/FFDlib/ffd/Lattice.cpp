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


#include "Lattice.h"

#include <assert.h>
#include <iostream>
#include <math.h>
#include <string>
using std::cout;
using std::endl;

Lattice::Lattice(gmtl::Vec3i divisions, gmtl::AABoxf aabb, size_t dim, float
    epsilon) :
                    aabbMin(aabb.getMin()),
                    aabbMax(aabb.getMax()),
                    maxDivisions(20),
                    dim(dim == 0 ? 3 : dim),
                    epsilon(epsilon >= 0.0f ? epsilon : 0.0001f)
{
    for (int i = 0; i < 3; i++)
    {
        aabbMin[i] -= epsilon;
        aabbMax[i] += epsilon;
    }
    distances = getDistancePerAxis(aabbMin, aabbMax);
    setLatticeDivisions(divisions);

    controlPoints.resize(dim + 1);
    for (int i = 0; i <= dim; ++i)
    {
        controlPoints[i].resize(dim + 1);
        for (int j = 0; j <= dim; ++j)
            controlPoints[i][j].resize(dim + 1);
    }
}

bool Lattice::setCellPoint(const gmtl::Vec3i& index, const gmtl::Vec3f& pos)
{
    if(checkCellIndex(index))
    {
        cellPoints[index[0]][index[1]][index[2]] = pos;
        return true;
    }
    return false;
}

bool Lattice::setLatticePoint(const gmtl::Vec3i& index, const gmtl::Vec3f& pos)
{
    if(checkLatticeIndex(index))
    {
        cellPoints[index[0] * dim][index[1] * dim][index[2] * dim] = pos;
        return true;
    }
    return false;
}

void Lattice::setLatticeDivisions(const gmtl::Vec3i divisions)
{
    // if amount of divsions is not valid set it to 1
    if ((divisions[0] > 0) && (divisions[0] < maxDivisions) &&
        (divisions[1] > 0) && (divisions[1] < maxDivisions) &&
        (divisions[2] > 0) && (divisions[2] < maxDivisions))
    {
        cellDivisions = gmtl::Vec3i(divisions[0] * dim,
                                    divisions[1] * dim,
                                    divisions[2] * dim);
        latticeDivisions = divisions;
    }
    else
    {
        cellDivisions = gmtl::Vec3i(dim, dim, dim);
        latticeDivisions = gmtl::Vec3i(1, 1, 1);
    }

    cellPoints.resize(cellDivisions[0] + 1);
    lastCellPoints.resize(cellDivisions[0] + 1);
    for (int w = 0; w <= cellDivisions[0]; ++w)
    {
        cellPoints[w].resize(cellDivisions[1] + 1);
        lastCellPoints[w].resize(cellDivisions[1] + 1);
        for (int l = 0; l <= cellDivisions[1]; ++l)
        {
            lastCellPoints[w][l].resize(cellDivisions[2] + 1);
            cellPoints[w][l].resize(cellDivisions[2] + 1);
        }
    }

    numOfCells = cellDivisions[0] * cellDivisions[1] * cellDivisions[2];

    calculateCellPoints();
}

void Lattice::setBounds(const gmtl::AABoxf aabb)
{
    aabbMin = aabb.getMin();
    aabbMax = aabb.getMax();

    for (int i = 0; i < 3; i++)
    {
        aabbMin[i] -= epsilon;
        aabbMax[i] += epsilon;
    }

    distances = getDistancePerAxis(aabbMin, aabbMax);
    calculateCellPoints();
}

size_t Lattice::getDim()
{
    return dim;
}

bool Lattice::getCellPoint(const gmtl::Vec3i& index, gmtl::Vec3f& pos) const
{
    if(checkCellIndex(index))
    {
        pos = cellPoints[index[0]][index[1]][index[2]];
        return true;
    }
    return false;
}

bool Lattice::getLastCellPoint(const gmtl::Vec3i& index, gmtl::Vec3f& pos)
    const
{
    if(checkCellIndex(index))
    {
        pos = lastCellPoints[index[0]][index[1]][index[2]];
        return true;
    }
    return false;
}

bool Lattice::getLatticePoint(const gmtl::Vec3i& index, gmtl::Vec3f& pos) const
{
    if(checkLatticeIndex(index))
    {
        pos = cellPoints[index[0] * dim][index[1] * dim][index[2] * dim];
        return true;
    }
    return false;
}

bool Lattice::getLastLatticePoint(const gmtl::Vec3i& index, gmtl::Vec3f& pos)
    const
{
    if(checkLatticeIndex(index))
    {
        pos = lastCellPoints[index[0] * dim][index[1] * dim][index[2] * dim];
        return true;
    }
    return false;
}

const Lattice::Vector3d& Lattice::getCellPoints() const
{
    return cellPoints;
}

const gmtl::Vec3i Lattice::getLatticeDivisions() const
{
    return latticeDivisions;
}

const gmtl::Vec3f Lattice::getLatticeDividedLength() const
{
    return latticeDividedLength;
}

const gmtl::Vec3i Lattice::getCellDivisions() const
{
    return cellDivisions;
}

const gmtl::Vec3f Lattice::getCellDividedLength() const
{
    return cellDividedLength;
}

void Lattice::getLatticeRelativeControlPoints(
                    const gmtl::Vec3i index, Vector3d& controlPoints)
{
    for (int i = 0; i < 3; ++i)
        assert(index[i] >= 0);

    const float ldl0(latticeDividedLength[0]);
    const float ldl1(latticeDividedLength[1]);
    const float ldl2(latticeDividedLength[2]);

    const size_t hh(index[0] * dim), ww(index[1] * dim), ll(index[2] * dim);
    for (int h = 0; h <= dim; ++h)
        for (int w = 0; w <= dim; ++w)
            for (int l = 0; l <= dim; ++l)
            {
                gmtl::Vec3f& temp(controlPoints[h][w][l]);
                temp = cellPoints[hh + h][ww + w][ll + l];
                temp -= cellPoints[hh][ww][ll];

                temp[0] /= ldl0;
                temp[1] /= ldl1;
                temp[2] /= ldl2;
            }
}

gmtl::Vec3i Lattice::getNearestNeighbourLattice(gmtl::Vec3f position)
{
    float dist, distL;
    gmtl::Vec3i ind;
    gmtl::Vec3f diff = position - cellPoints[0][0][0];

    distL = gmtl::lengthSquared(diff);


    for (size_t h = 0; h < cellPoints.size(); ++h)
        for (size_t w = 0; w < cellPoints[h].size(); ++w)
            for (size_t l = 0; l < cellPoints[w].size(); ++l)
        {
            diff = position - cellPoints[h][w][l];
            dist = gmtl::lengthSquared(diff);
            if (dist < distL)
            {
                distL = dist;
                ind[0] = h;
                ind[1] = w;
                ind[2] = l;
            }

        }
    return ind;
}

gmtl::Vec2i Lattice::getNearestNeighbour(gmtl::Vec3f position)
{
    float dist, distL;
    gmtl::Vec2i ind;
    gmtl::Vec3f diff = position - (*mp)[0][0];

    distL = gmtl::lengthSquared(diff);
    ind[0] = 0;
    ind[1] = 0;

    for (size_t model = 0; model < mp->size(); ++model)
        for (size_t point = 0; point < ((*mp)[model]).size(); ++point)
        {
            diff = position - (*mp)[model][point];
            dist = gmtl::lengthSquared(diff);
            if (dist < distL)
            {
                distL = dist;
                ind[0] = model;
                ind[1] = point;
            }

        }
    return ind;
}

Lattice::Vector3d& Lattice::accessCellPoints()
{
    return cellPoints;
}

void  Lattice::assignModelPointsToLatticeCells(Vector2d &modelPoints)
{
    mp = &modelPoints;

    modelPointIndex.resize(modelPoints.size());
    for (size_t model = 0; model < modelPoints.size(); ++model)
    {
        modelPointIndex[model].resize(modelPoints[model].size());
        for (size_t point = 0; point < modelPoints[model].size(); ++point)
        {
            gmtl::Vec3f distToMin;
            for (int i = 0; i < 3; ++i)
            {
                // if a point lies outside of the lattice minimum correct the
                // point to be a bit larger than the lattice minimum.
                if ((modelPoints[model][point][i]) <= lastCellPoints[0][0][0][i])
                    modelPoints[model][point][i] = lastCellPoints[0][0][0][i] + epsilon;

                // if a point lies outside of the lattice maximum correct
                // the point to be a bit smaller than the lattice maximum.
                else if ((modelPoints[model][point][i]) >=
                        lastCellPoints[cellDivisions[0]]
                                      [cellDivisions[1]]
                                      [cellDivisions[2]][i])
                    modelPoints[model][point][i] =
                        lastCellPoints[cellDivisions[0]]
                                      [cellDivisions[1]]
                                      [cellDivisions[2]][i] - epsilon;
            }

            // get distance to lattice origin
            distToMin = getDistancePerAxis(modelPoints[model][point],
                lastCellPoints[0][0][0]);

            for (int i = 0; i < 3; ++i)
            {
                assert(distToMin[i] >= 0.0f);

                // calculate the index for each model point in each dimension
                // get index regarding the lattice subdivisions
                modelPointIndex[model][point][i] =
                    floor(distToMin[i] / latticeDividedLength[i]);

                if (modelPointIndex[model][point][i] >= latticeDivisions[i])
                {
                    cout << "index " << modelPointIndex[model][point][i];
                    cout << " >= division " << latticeDivisions[i] << endl;
                }
                assert(modelPointIndex[model][point][i] < latticeDivisions[i]);
            }
        }
    }
}

void Lattice::shiftLastCellPoints()
{
    for (int h = 0; h <= dim; ++h)
        for (int w = 0; w <= dim; ++w)
            for (int l = 0; l <= dim; ++l)
                lastCellPoints[h][w][l] = cellPoints[h][w][l];
}

void  Lattice::transformStuRelative(gmtl::Vec3f& stu, const gmtl::Vec3i index)
{
    for (int i = 0; i < 3; ++i)
        assert(index[i] >= 0);

    gmtl::Vec3f point;
    // use the undeformed (last) lattice
    bool valid = getLastLatticePoint(index, point);
    assert(valid);

    for (int i = 0; i < 3; ++i)
        stu[i] = (stu[i] - point[i]) / latticeDividedLength[i];
}

void  Lattice::transformStuAbsolute(gmtl::Vec3f& stu, const gmtl::Vec3i index)
{
    for (int i = 0; i < 3; ++i)
        assert(index[i] >= 0);

    gmtl::Vec3f point;
    // use the deformed lattice
    bool valid = getLatticePoint(index, point);
    assert(valid);

    // TODO: recalculate latticeDividedLength for current lattice ?
    for (int i = 0; i < 3; ++i)
        stu[i] = point[i] + stu[i] * latticeDividedLength[i];
}

void  Lattice::executeFFD(Vector2d &modelPointsSavepoint, Vector2d &modelPoints)
{
    gmtl::Vec3f stu;

    for (size_t model = 0; model < modelPoints.size(); ++model)
    {
        for (size_t point = 0; point < modelPoints[model].size(); ++point)
        {
            stu = modelPointsSavepoint[model][point];

            transformStuRelative(stu, modelPointIndex[model][point]);
            getLatticeRelativeControlPoints(
                                modelPointIndex[model][point], controlPoints);
            stu = Bezier::calculateBernstein3d(stu, controlPoints, dim);
            transformStuAbsolute(stu, modelPointIndex[model][point]);

            modelPoints[model][point] = stu;
        }
    }
}

void Lattice::centerOrigin(gmtl::Vec3f& min, gmtl::Vec3f& max,
                gmtl::Vec3f& move) const
{
    for (int i = 0; i < 3; ++i)
    {
        min[i] = distances[i] / -2.0f;
        max[i] = distances[i] / 2.0f;
        move[i] = aabbMin[i] * -1.0f + min[i];
    }
}

void Lattice::moveTo(const gmtl::Vec3f& cellPointOrigPos,
                gmtl::Vec3f& cellPoint, const gmtl::Vec3f& move)
{
    for (int i = 0; i < 3; ++i)
        cellPoint[i] = cellPointOrigPos[i] + move[i];
}

void Lattice::moveBack(const gmtl::Vec3f& cellPoint,
                gmtl::Vec3f& cellPointOrigPos, const gmtl::Vec3f& move) const
{
    for (int i = 0; i < 3; ++i)
        cellPointOrigPos[i] = cellPoint[i] - move[i];
}

gmtl::Vec3f Lattice::getDistancePerAxis(const gmtl::Vec3f& min,
                                              const gmtl::Vec3f& max) const
{
    gmtl::Vec3f dist;
    for (size_t i = 0; i < 3; ++i)
    {
        if (min[i] < max[i])
            dist[i] = max[i] - min[i];
        else
            dist[i] = min[i] - max[i];

        assert(dist[i] >= 0.0f);
    }
    return dist;
}

void Lattice::calculateCellPoints()
{
    // calulate divided part length for each dimension
    for (int i = 0; i < 3; ++i)
    {
        cellDividedLength[i] = distances[i] / cellDivisions[i];
        latticeDividedLength[i] = distances[i] / latticeDivisions[i];
    }

    // calculate verticePoint coordinates
    for (int h = 0; h <= cellDivisions[0]; ++h)
        for (int w = 0; w <= cellDivisions[1]; ++w)
            for (int l = 0; l <= cellDivisions[2]; ++l)
            {
                // add points for points list
                cellPoints[h][w][l] = (gmtl::Vec3f
                        (aabbMin[0] + h * cellDividedLength[0],
                         aabbMin[1] + w * cellDividedLength[1],
                         aabbMin[2] + l * cellDividedLength[2]));
                // initialize lastCellPoinst as cellPoints
                lastCellPoints[h][w][l] = cellPoints[h][w][l];
            }
}

bool Lattice::checkCellIndex(const gmtl::Vec3i& index) const
{
    for (int i = 0; i < 3; i++)
        if(!((index[i] >= 0) && (index[i] <= cellDivisions[i])))
            return false;
    return true;
}

bool Lattice::checkLatticeIndex(const gmtl::Vec3i& index) const
{
    for (int i = 0; i < 3; i++)
        if(!((index[i] >= 0) && (index[i] <= latticeDivisions[i])))
            return false;
    return true;
}

void Lattice::printPoint(string name, gmtl::Vec3f point)
{
    cout << name << point[0] << ", " << point[1] << ", " << point[2] << endl;
}

void Lattice::printPoint(string name, gmtl::Vec3i point)
{
    cout << name << point[0] << ", " << point[1] << ", " << point[2] << endl;
}

void Lattice::printIndex(int from, int to)
{
    cout << endl;
    int c = 0;
    for (size_t m = 0; m < modelPointIndex.size(); ++m)
        for (size_t p = 0; p < modelPointIndex[m].size(); ++p)
        {
            if (c >= to)
                return;

            if (c >= from)
            {
                printPoint("indexarray: ", modelPointIndex[m][p]);
                printPoint("modelpoint: ", (*mp)[m][p]);
            }
            ++c;
        }
}

void Lattice::printAllPoints()
{
    for (int h = 0; h <= cellDivisions[0]; ++h)
        for (int w = 0; w <= cellDivisions[1]; ++w)
            for (int l = 0; l <= cellDivisions[2]; ++l)
                printPoint("cellPoints: ", cellPoints[h][w][l]);
}
