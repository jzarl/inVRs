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

#include "oops/HtmpHeightFieldLoader.h"
#include "oops/Geometries.h"

#include <string>
#include <stdio.h>
#include <assert.h>

#include <inVRs/SystemCore/DebugOutput.h>

// Object Oriented Physics Simulation
namespace oops
{

HtmpHeightFieldLoader::~HtmpHeightFieldLoader()
{
} // ~HtmpHeightFieldLoader

void HtmpHeightFieldLoader::loadHeightField(std::string fileName, HeightField* object)
{
	FILE* file;
	FILEHEADER_HMP1 header;
	size_t res;
	unsigned correctHeader = 0;
	unsigned wrongHeader = 0;
	float x0, y0, z0, dx, dy, xLen, yLen;
	int xSamples, ySamples;
	HeightFieldData* data;
	
	if(fileName.empty())
	{
		printd(ERROR, "HtmpHeightFieldLoader::loadHeightField(): missing filename!\n");
		return;
	} // if
	file = fopen(fileName.c_str(), "rb");
	if(!file)
	{
		printd(ERROR, "HtmpHeightFieldLoader::loadHeightField(): failed to open file %s\n", fileName.c_str());
		return;
	} // if

	memset(&header, 0, sizeof(FILEHEADER_HMP1));
	res = fread(&header, sizeof(FILEHEADER_HMP1), 1, file);

	correctHeader = ('H'<<24) | ('M'<<16) | ('P'<<8) | ('1');
	wrongHeader = ('1'<<24) | ('P'<<16) | ('M'<<8) | ('H');
	
	if(header.typeID != correctHeader)
	{
		if(header.typeID == wrongHeader)
			printd(ERROR, "HtmpHeightFieldLoader::loadHeightField(): file %s has wrong endianess\n", fileName.c_str());
		else
			printd(ERROR, "HtmpHeightFieldLoader::loadHeightField(): %s has incorrect header\n", fileName.c_str());
		fclose(file);
		return;
	} // if

	x0 = header.x0;
	y0 = header.y0;
	z0 = header.z0;
	dx = header.dx;
	dy = header.dy;
	xSamples = header.xSamples;
	ySamples = header.ySamples;
	xLen = header.xLen;
	yLen = header.yLen;

	printd(INFO, "HtmpHeightFieldLoader::loadHeightField(): found following data:\n");
	printd(INFO, "\tx0 = %f\n", x0);
	printd(INFO, "\ty0 = %f\n", y0);
	printd(INFO, "\tz0 = %f\n", z0);
	printd(INFO, "\tdx = %f\n", dx);
	printd(INFO, "\tdy = %f\n", dy);
	printd(INFO, "\txLen = %f\n", xLen);
	printd(INFO, "\tyLen = %f\n", yLen);
	printd(INFO, "\txSamples = %i\n", xSamples);
	printd(INFO, "\tySamples = %i\n", ySamples);

	data = new HeightFieldData;
 	data->resolutionX = xSamples;
 	data->resolutionY = ySamples;
	data->offsetX = x0;
	data->offsetY = y0;
 	data->offset = 0;
 	data->scale = 1;
 	data->sizeX = xLen;
 	data->sizeY = yLen;
 	data->heightValues = new float[xSamples*ySamples];
 	res += fread(data->heightValues, sizeof(float)*xSamples*ySamples, 1, file);

 	object->setData(data);
 	
//	assert(false);
	fclose(file);
	
	if(res != 2)
	{
		printd(ERROR, "HeightFieldLoader::loadHeightMao(): could not read from %s\n", fileName.c_str());
	} // if
} // loadHeightField

} // oops
