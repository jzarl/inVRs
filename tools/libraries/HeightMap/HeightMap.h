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

#ifndef HEIGHMAP_H
#define HEIGHMAP_H

// fix problems with Visual Studio 2008
#ifdef WIN32
#include <stdio.h>
#endif

#include <OpenSG/OSGSimpleGeometry.h>
#include <OpenSG/OSGSimpleSceneManager.h>
#include <OpenSG/OSGSceneFileHandler.h>
#include <OpenSG/OSGTriangleIterator.h>

#include "HeightMapInterface.h"

#if OSG_MAJOR_VERSION < 2
namespace OSG {
	typedef NodePtr NodeTransitPtr;
	typedef NodeCorePtr NodeCoreTransitPtr;
}
#endif

#ifndef PI
#define PI 3.14159265
#endif

#pragma pack(1)
struct FILEHEADER_HMP1 {
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

class INVRS_HEIGHTMAP_API HeightMap : public HeightMapInterface {
public:

	HeightMap();
	HeightMap(HeightMap* h); // copy-constructor
	virtual ~HeightMap();

	bool setup(OSG::Real32 x0, OSG::Real32 y0, OSG::Real32 z0, OSG::Real32 xlen, OSG::Real32 ylen, int xSamples,
			int ySamples, bool filter = false);
	bool setup(OSG::NodeRefPtr sampleMdl, int xSamples, int ySamples, bool filter = false);
	bool setup(const char* filename);

	bool generateHeightMap(OSG::NodeRefPtr model);
	bool generateHeightMap(OSG::NodeRefPtr model, int xSamples, int ySamples, int height, int zRot,
			bool filter = false);
	void dumpHeight(const char* filename, float* minHeight = NULL, float* maxHeight = NULL);
	OSG::NodeTransitPtr exportGeometry(char* filename, char* normalsArrowModelFile = NULL);

	OSG::NodeCoreTransitPtr makeGeometryPtr();
	void adjustZ(float scale, float off); // h <- h*scale + off

	bool writeToFile(const char* filename);
	void writeToPPM(const char* filename, float scale, float off, bool findSelf);

	void setGaussFilterCoefficients(float smoothness, float rectEps);

	void getMinAndMaxHeightValues(float* minHeight = NULL, float* maxHeight = NULL);
	OSG::Real32 getZ(OSG::Real32 x, OSG::Real32 y);

	// required by HeightMapInterface
	virtual float getHeight(float x, float z);
	void getNormal(OSG::Real32 x, OSG::Real32 y, OSG::Vec3f* dst);
	virtual gmtl::Vec3f getNormal(float x, float z);

	OSG::Quaternion getNormalRotation(OSG::Real32 x, OSG::Real32 y);
	static OSG::Quaternion getNormalRotation(const OSG::Vec3f& normal);


protected:

	void doIntersectionTests(OSG::NodeRefPtr model);

	static OSG::Real32 conv1D(OSG::Real32* fun, OSG::Real32* filterKrnl, OSG::Int32 fmax, OSG::Int32 filterMax,
			OSG::Int32 filterStart, OSG::Int32 x, OSG::Real32* areaBelowFilter, char* bInitalized);
	static OSG::Real32 conv2D(OSG::Real32* fun, OSG::Int32 funMaxX, OSG::Int32 funMaxY, OSG::Real32* filterKrnl,
			OSG::Int32 filterMaxX, OSG::Int32 filterStartX, OSG::Int32 filterMaxY, OSG::Int32 filterStartY, OSG::Int32 x,
			OSG::Int32 y, char* bInitialized);
	static OSG::Real32* createGaussKernel(OSG::Real32 sigma, OSG::Int32* length, OSG::Real32 eps);

	bool getWeightsAndIndices(OSG::Real32 x, OSG::Real32 y);

	static void dumpPPM(OSG::Real32* data, int xLen, int yLen, OSG::Real32 scale, OSG::Real32 off,
			const char* file = "heightdump.ppm");
	static void dumpPPMVec3f(OSG::Vec3f* data, int xLen, int yLen, OSG::Real32 scale, OSG::Real32 off,
			const char* file = "heightdumpvec.ppm");

	static const OSG::Vec3f up;
	static const OSG::Vec3f down;
	static const OSG::Char8* GEOMETRY_CORE_NAME;
	static const float RANGE; // Accuracy of intersection

	OSG::Real32* height;
	OSG::Vec3f* normals;
	char* bInitialized;
	int xSamples;
	int ySamples;
	OSG::Real32 x0;
	OSG::Real32 y0;
	OSG::Real32 z0;
	OSG::Real32 xlen;
	OSG::Real32 ylen;
	OSG::Real32 dx;
	OSG::Real32 dy;
	bool applyGaussBlurFilter;

	float gaussSmoothness;
	float gaussRectEps;

	// fr getWeightsAndIndices()
	OSG::Real32 weights[4];
	int indices[4];
};

#endif //HEIGHMAP_H
