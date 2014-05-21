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

#include "BSpline.h"

#include <memory.h>
#include <math.h>
#include <stdio.h>

BSpline1D::BSpline1D() {
	deg = 0;
	nPoints = 0;
	pointDim = 0;
	points = NULL;
	knots = NULL;
}

BSpline1D::~BSpline1D() {
	if (points)
		delete[] points;
	if (knots)
		delete[] knots;
}

bool BSpline1D::init(int deg, int nPoints, float *points, int pointDim) {
	int i;

	if (nPoints <= deg)
		return false;
	if (this->points)
		delete[] this->points;
	if (knots)
		delete[] knots;
	this->points = new float[nPoints * pointDim];
	memcpy(this->points, points, sizeof(float) * nPoints * pointDim);

	this->deg = deg;
	this->pointDim = pointDim;
	this->nPoints = nPoints;

	knots = new float[nPoints + deg + 1];

	for (i = 0; i < deg; i++)
		knots[i] = 0;
	for (i = deg; i < nPoints + 1; i++)
		knots[i] = (float)i - deg;
	for (i = nPoints + 1; i < nPoints + 1 + deg; i++)
		knots[i] = knots[i - 1];

	return true;
}

void BSpline1D::eval(float t, float *res) {
	int i, j;
	float v;

	// adjust t:
	t = (float)(knots[nPoints] - knots[deg]) * t;

	memset(res, 0, sizeof(float) * pointDim);

	for (i = 0; i < nPoints; i++) {
		v = evalB(t, i, deg);
		for (j = 0; j < pointDim; j++)
			res[j] += v * points[i * pointDim + j];
	}
}

float BSpline1D::evalB(float t, int i, int d) {
	float l, r;

	if (d == 0) {
		if ((t >= knots[i]) && (t < knots[i + 1]))
			return 1.0f;
		else
			return 0;
	}

	if (knots[i + d] - knots[i])
		l = ((t - knots[i]) / (knots[i + d] - knots[i])) * evalB(t, i, d - 1);
	else
		l = 0;
	if (knots[i + d + 1] - knots[i + 1])
		r = ((knots[i + d + 1] - t) / (knots[i + d + 1] - knots[i + 1])) * evalB(t, i + 1, d - 1);
	else
		r = 0;
	return l + r;
}
