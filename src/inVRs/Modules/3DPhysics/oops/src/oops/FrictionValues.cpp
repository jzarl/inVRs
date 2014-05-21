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

#include "oops/FrictionValues.h"
#include <cstdio>

namespace oops
{

namespace materials
{

MATERIAL getMaterial(std::string name)
{
	if (name == "ALUMINIUM")
		return ALUMINIUM;
	if (name == "STEEL")
		return STEEL;
	if (name == "WOOD")
		return WOOD;
	if (name == "CAST_IRON")
		return CAST_IRON;
	if (name == "GLASS")
		return GLASS;
	if (name == "DIAMOND")
		return DIAMOND;
	if (name == "RUBBER_TIRE")
		return RUBBER_TIRE;
	if (name == "ASPHALT")
		return ASPHALT;
	if (name == "LEATHER")
		return LEATHER;
	if (name == "BRICK")
		return BRICK;
	if (name == "ASPHALT_WET")
		return ASPHALT_WET;
	if (name == "SNOW")
		return SNOW;
	printf("getMaterial(): WARNING: unknown Material %s, using default Material STEEL!\n", name.c_str());
	return STEEL;
} // getMaterial

} // materials

// lower value = column
// higher value = row
/*
row: 0 -> +0
	 1 -> +1
	 2 -> +3
	 3 -> +6
	 4 -> +10
	 5 -> +15
*/

float getFriction(materials::MATERIAL mat1, materials::MATERIAL mat2)
{
	if (mat1 == materials::UNDEFINED || mat2 == materials::UNDEFINED)
		return DEFAULT_FRICTION;

	float result;
	int i, temp;
	int matrixIndex1, matrixIndex2;
	int index1 = ((int)mat1) - 1;
	int index2 = ((int)mat2) - 1;

	if (index1 > index2)
	{
		temp = index1;
		index1 = index2;
		index2 = temp;
	} // if

	matrixIndex2 = 0;
	for (i=1; i <= index2; i++)
	{
		matrixIndex2 += i;
	} // for
	result = materials::FRICTION[matrixIndex2 + index1];

	if (result == 0)
	{
		matrixIndex2 += index2;
		matrixIndex1 = 0;
		for (i=1; i <= index1; i++)
		{
			matrixIndex1 += i;
		} // for
		matrixIndex1 += index1;
	
		result = (materials::FRICTION[matrixIndex1] + materials::FRICTION[matrixIndex2])/2.0f;
	} // if
	return result;
} // getFriction

} // oops
