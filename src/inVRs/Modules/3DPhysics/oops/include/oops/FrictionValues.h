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
 *                                Changes                                    *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
\*---------------------------------------------------------------------------*/

#ifndef _FRICTIONVALUES_H
#define _FRICTIONVALUES_H
#include <string>

namespace oops
{

namespace materials
{

#define DEFAULT_FRICTION	0.5f
#define N_MATERIALS 		12

enum MATERIAL {
	UNDEFINED = 0,
	ALUMINIUM = 1,
	STEEL = 2,
	WOOD = 3,
	CAST_IRON = 4,
	GLASS = 5,
	DIAMOND = 6,
	RUBBER_TIRE = 7,
	ASPHALT = 8,
	LEATHER = 9,
	BRICK = 10,
	ASPHALT_WET = 11,
	SNOW = 12
}; // oops

/*
	|	1	|	2	|	3	|	4	|	5	|	6	|	7	|	8	|	9 	|	10	|	11	|	12	|
1	|  1.4	|	-	|	-	|	-	|	-	|	-	|	-	|	-	|	-	|	-	|	-	|	-	|
2	|  0.55 |  0.6  |	-	|	-	|	-	|	-	|	-	|	-	|	-	|	-	|	-	|	-	|
3	|  0.4  |  0.4  |  0.4  |	-	|	-	|	-	|	-	|	-	|	-	|	-	|	-	|	-	|
4	|   -   |  0.4  |  0.5  |  0.5  |	-	|	-	|	-	|	-	|	-	|	-	|	-	|	-	|
5	|  0.6  |  0.6  |   -   |  0.7  |  0.7  |	-	|	-	|	-	|	-	|	-	|	-	|	-	|
6	|  0.15 |  0.15 |   -   |  0.15 |   -   |  0.1  |	-	|	-	|	-	|	-	|	-	|	-	|
7	|   -   |	-	|   -   |   -	|   -   |   -   |   1   |	-	|	-	|	-	|	-	|	-	|
8	|   -   |	-	|  0.62 |   -   |   -   |   -   |  0.8  |  0.7  |	-	|	-	|	-	|	-	|
9	|  0.6  |  0.6	|  0.4  |  0.6  |   -   |   -   |   -   |   -   |  0.5  |	-	|	-	|	-	|
10	|   -   |	-	|  0.6  |   -   |   -   |   -   |   -   |  0.6  |   -   |  0.7  |	-	|	-	|
11	|   -	|	-	|  0.4	|	-	|	-	|	-	|  0.4	|	-	|	-	|  0.5  |  0.5	|	-	|
12	|  0.1  |  0.1	|  0.2	|  0.1	|   -	|   -	|  0.2	|	-	|  0.1	|	-	|	-	|  0.1	|

*/

const float FRICTION[] = {
/* ALUMINIUM */		1.40f,
/* STEEL */			0.55f, 0.60f,
/* WOOD */			0.40f, 0.40f, 0.40f, 
/* CAST_IRON */		0.00f, 0.40f, 0.50f, 0.50f,
/* GLASS */			0.60f, 0.60f, 0.00f, 0.70f, 0.70f,
/* DIAMOND */		0.15f, 0.15f, 0.00f, 0.15f, 0.00f, 0.10f,
/* RUBBER_TIRE */	0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 4.00f,
/* ASPHALT */		0.00f, 0.00f, 0.62f, 0.00f, 0.00f, 0.00f, 3.20f, 0.70f,
/* LEATHER */		0.60f, 0.60f, 0.40f, 0.60f, 0.00f, 0.00f, 0.00f, 0.00f, 0.50f,
/* BRICK */			0.00f, 0.00f, 0.60f, 0.00f, 0.00f, 0.00f, 0.00f, 0.60f, 0.00f, 0.70f,
/* ASPHALT_WET */	0.00f, 0.00f, 0.40f, 0.00f, 0.00f, 0.00f, 1.60f, 0.00f, 0.00f, 0.50f, 0.50f,
/* SNOW */			0.10f, 0.10f, 0.20f, 0.10f, 0.00f, 0.00f, 0.80f, 0.00f, 0.10f, 0.00f, 0.00f, 0.10f
};


MATERIAL getMaterial(std::string name);

} // materials

float getFriction(materials::MATERIAL mat1, materials::MATERIAL mat2);

} // oops

#endif // _FRICTIONVALUES_H
