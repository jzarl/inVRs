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

#ifndef _HTMPHEIGHTFIELDLOADER_H
#define _HTMPHEIGHTFIELDLOADER_H

#include "Interfaces/HeightFieldLoader.h"

// Object Oriented Physics Simulation
namespace oops
{

#pragma pack(1)
struct FILEHEADER_HMP1
{
	unsigned typeID;
	float x0;
	float y0;
	float z0;
	float dx;
	float dy;
	int xSamples;
	int ySamples;
	float xLen;
	float yLen;
};
#pragma pack()

class HtmpHeightFieldLoader : public HeightFieldLoader
{
public:
	virtual ~HtmpHeightFieldLoader();
	virtual void loadHeightField(std::string fileName, HeightField* object);
}; // OpenSGTriangleMeshLoader

} // oops

#endif // _HTMPHEIGHTFIELDLOADER_H
