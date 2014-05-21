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

#ifndef _HEIGHTMAPINTERFACE_H
#define _HEIGHTMAPINTERFACE_H

#ifdef WIN32
	#ifdef INVRSHEIGHTMAP_EXPORTS
	#define INVRS_HEIGHTMAP_API __declspec(dllexport)
	#else
	#define INVRS_HEIGHTMAP_API __declspec(dllimport)
	#endif
#else
#define INVRS_HEIGHTMAP_API
#endif

#include <gmtl/Vec.h>
#include <inVRs/SystemCore/ClassFactory.h>

/**
 * this class provides an math library / scene management independent access to a heightmap implementation
 */
class INVRS_HEIGHTMAP_API HeightMapInterface {
public:
	virtual ~HeightMapInterface() {}

	virtual float getHeight(float x, float z) = 0;
	virtual gmtl::Vec3f getNormal(float x, float z) = 0;

	virtual void adjustZ(float scale, float offset) = 0;

};

#endif
