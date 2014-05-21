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

#include "HeightMap.h"

#include <memory.h>
#include <assert.h>
#include <math.h>

#include <OpenSG/OSGTriangleIterator.h>
#if OSG_MAJOR_VERSION >= 2
#include <OpenSG/OSGBoxVolume.h>
#else //OpenSG1:
#include <OpenSG/OSGDynamicVolume.h>
#endif
#include <OpenSG/OSGSceneFileHandler.h>

#include <inVRs/SystemCore/DebugOutput.h>


OSG_USING_NAMESPACE

#define MIN2(a,b)	((a)<(b)?(a):(b))
#define MAX2(a,b)	((a)>(b)?(a):(b))

// change of axes!!:
const Vec3f HeightMap::up = Vec3f(0, 1, 0);
const Vec3f HeightMap::down = Vec3f(0, -1, 0);

const Char8* HeightMap::GEOMETRY_CORE_NAME = "Geometry\0";
const float HeightMap::RANGE = 0.0001f;

HeightMap::HeightMap() {
	height = NULL;
	normals = NULL;
	bInitialized = NULL;

	gaussSmoothness = 3.0f;
	gaussRectEps = 0.001f;
}

HeightMap::~HeightMap() {
	if (height)
		delete[] height;
	if (normals)
		delete[] normals;
	if (bInitialized)
		delete[] bInitialized;
}

HeightMap::HeightMap(HeightMap* h) {
	int i, j;

	xSamples = h->xSamples;
	ySamples = h->ySamples;
	x0 = h->x0;
	y0 = h->y0;
	z0 = h->z0;
	xlen = h->xlen;
	ylen = h->ylen;
	dx = h->dx;
	dy = h->dy;
	applyGaussBlurFilter = h->applyGaussBlurFilter;

	for (i = 0; i < 4; i++) {
		weights[i] = h->weights[i];
		indices[i] = h->indices[i];
	} // for

	if (h->height && h->normals && h->bInitialized) // always come together
	{
		height = new Real32[xSamples * ySamples];
		normals = new Vec3f[xSamples * ySamples];
		bInitialized = new char[xSamples * ySamples];
		for (i = 0; i < ySamples; i++) {
			for (j = 0; j < xSamples; j++) {
				height[i * xSamples + j] = h->height[i * xSamples + j];
				normals[i * xSamples + j] = h->normals[i * xSamples + j];
				bInitialized[i * xSamples + j] = h->bInitialized[i * xSamples + j];
			} // for
		} // for
	} // if

	gaussSmoothness = 3.0f;
	gaussRectEps = 0.001f;
} // HeightMap


bool HeightMap::setup(Real32 x0, Real32 y0, Real32 z0, Real32 xlen, Real32 ylen, int xSamples,
		int ySamples, bool filter) {
	int i, j;
	this->xSamples = xSamples;
	this->ySamples = ySamples;
	this->x0 = x0;
	this->y0 = y0;
	this->z0 = z0;
	this->xlen = xlen;
	this->ylen = ylen;
	applyGaussBlurFilter = filter;

	assert(xSamples> 1);
	assert(ySamples> 1);
	assert(xlen> 0);
	assert(ylen> 0);

	dx = xlen / (float)(xSamples - 1);
	dy = ylen / (float)(ySamples - 1);

	if (height)
		delete[] height;
	if (normals)
		delete[] normals;

	height = new Real32[xSamples * ySamples];
	normals = new Vec3f[xSamples * ySamples];
	bInitialized = new char[xSamples * ySamples];

	for (i = 0; i < ySamples; i++) {
		for (j = 0; j < xSamples; j++) {
			height[i * xSamples + j] = z0;
			normals[i * xSamples + j] = up;
			bInitialized[i * xSamples + j] = 0;
		}
	}

	return true;
}

bool HeightMap::setup(OSG::NodeRefPtr sampleMdl, int xSamples, int ySamples, bool filter) {
#if OSG_MAJOR_VERSION >= 2
	BoxVolume worldVol;
#else //OpenSG1:
	DynamicVolume worldVol;
#endif
	Vec3f minV, maxV;
	Real32 xlen, ylen;
	sampleMdl->getWorldVolume(worldVol);
	worldVol.getBounds(minV, maxV);
	xlen = maxV.x() - minV.x();
	ylen = maxV.z() - minV.z();
	return this->setup(minV.x(), minV.z(), minV.y() - 1, xlen, ylen, xSamples, ySamples, filter);
}

bool HeightMap::setup(const char* filename) {
	FILE* f;
	FILEHEADER_HMP1 h;
	size_t res;
	bool ret;

	assert(sizeof(float)*3 == sizeof(Vec3f));
	assert(sizeof(float) == sizeof(Real32));

	if (!filename)
		return false;
	f = fopen(filename, "rb");
	if (!f) {
		printd(ERROR, "HeightMap::setup(): failed to open file %s\n", filename);
		return false;
	}

	memset(&h, 0, sizeof(FILEHEADER_HMP1));
	res = fread(&h, sizeof(FILEHEADER_HMP1), 1, f);

	if (h.typeID != 'HMP1') {
		if (h.typeID == '1PMH')
			printd(ERROR, "HeightMap::setup(): file %s has wrong endianess\n", filename);
		else
			printd(ERROR, "HeightMap::setup(): %s has incorrect header\n", filename);
		fclose(f);
		return false;
	}

	x0 = h.x0;
	y0 = h.y0;
	z0 = h.z0;
	dx = h.dx;
	dy = h.dy;
	xSamples = h.xSamples;
	ySamples = h.ySamples;
	xlen = h.xLen;
	ylen = h.yLen;
	applyGaussBlurFilter = false;

	height = new float[xSamples * ySamples];
	normals = new Vec3f[xSamples * ySamples];
	bInitialized = new char[xSamples * ySamples];

	ret = true;
	res += fread(height, sizeof(float) * xSamples * ySamples, 1, f);
	res += fread(normals, 3 * sizeof(float) * xSamples * ySamples, 1, f);
	res += fread(bInitialized, sizeof(char) * xSamples * ySamples, 1, f);
	fclose(f);
	if (res != 4) {
		printd(ERROR, "HeightMap::setup(): could not read from %s\n", filename);
		ret = false;
	}
	return ret;
}

bool HeightMap::generateHeightMap(OSG::NodeRefPtr model) {
	doIntersectionTests(model);

	Int32 len, normalsXLen, normalsYLen;
	Real32* gaussKernel = createGaussKernel(3, &len, 0.001);
	Real32 *preHeight, *preNormals[3], *postNormals[3], temp;
	Int32 i, j, k;
	char* bInitializedNormals;

	preHeight = new Real32[(xSamples) * (ySamples)];
	memcpy(preHeight, height, sizeof(Real32) * xSamples * ySamples);

	normalsXLen = xSamples + len;
	normalsYLen = ySamples + len;
	for (k = 0; k < 3; k++) {
		preNormals[k] = new Real32[(normalsXLen) * (normalsYLen)];
		postNormals[k] = new Real32[(normalsXLen) * (normalsYLen)];
	}
	bInitializedNormals = new char[(normalsXLen) * (normalsYLen)];

	if (applyGaussBlurFilter) {
		for (i = 0; i < normalsYLen; i++)
			for (j = 0; j < normalsXLen; j++) {
				preNormals[0][i * normalsXLen + j] = up.x();
				preNormals[1][i * normalsXLen + j] = up.y();
				preNormals[2][i * normalsXLen + j] = up.z();
				postNormals[0][i * normalsXLen + j] = up.x();
				postNormals[1][i * normalsXLen + j] = up.y();
				postNormals[2][i * normalsXLen + j] = up.z();
				bInitializedNormals[i * normalsXLen + j] = -1;
			}

		for (i = 0; i < ySamples; i++)
			for (j = 0; j < xSamples; j++) {
				for (k = 0; k < 3; k++) {
					preNormals[k][(i + len / 2) * (xSamples + len) + (j + len / 2)] = normals[i
							* xSamples + j][k];
				}
				bInitializedNormals[(i + len / 2) * (xSamples + len) + (j + len / 2)]
						= bInitialized[i * xSamples + j];
			}

		for (i = 0; i < ySamples; i++)
			for (j = 0; j < xSamples; j++) {
				//height[i*xSamples+j] = Conv2D(embeddedHeight, gaussKernel, xSamples-1, ySamples-1, len/2-1, len/2-1, 0, 0, -len/2, -len/2, j, i);

				if (bInitialized[i * xSamples + j]) {
					temp = conv2D(preHeight, xSamples - 1, ySamples - 1, gaussKernel, len / 2 - 1,
							-len / 2, len / 2 - 1, -len / 2, j, i, bInitialized);
					height[i * xSamples + j] = temp;

					for (k = 0; k < 3; k++) {
						temp = conv2D(preNormals[k], normalsXLen - 1, normalsYLen - 1, gaussKernel,
								len / 2 - 1, -len / 2, len / 2 - 1, -len / 2, j + len / 2, i + len
										/ 2, bInitializedNormals);
						postNormals[k][(i + len / 2) * (xSamples + len) + (j + len / 2)] = temp;
					}
				}
			}
	}

//	 static char buffer[255];
//	 static int idx = 0;
//
//	 sprintf(buffer, "vorher%d.ppm", idx);
//	 dumpPPM(preHeight, xSamples, ySamples, 0.2f, 0.3, buffer);
//	 sprintf(buffer, "nachher%d.ppm", idx);
//	 dumpPPM(height, xSamples, ySamples, 0.2f, 0.3, buffer);
//	 sprintf(buffer, "vecvorher%d.ppm", idx);
//	 dumpPPMVec3f(normals, xSamples, ySamples, 0.6f, 0.4, buffer);

	if (applyGaussBlurFilter) {
		for (i = 0; i < ySamples; i++)
			for (j = 0; j < xSamples; j++) {
				for (k = 0; k < 3; k++) {
					normals[i * xSamples + j][k] = postNormals[k][(i + len / 2) * (xSamples + len)
							+ (j + len / 2)];
				}
				normals[i * xSamples + j].normalize();
			}
	}

//	 sprintf(buffer, "vecnachher%d.ppm", idx);
//	 dumpPPMVec3f(normals, xSamples, ySamples, 0.6f, 0.4, buffer);
//	 idx++;

	delete[] gaussKernel;
	delete[] preHeight;
	for (k = 0; k < 3; k++) {
		delete[] preNormals[k];
		delete[] postNormals[k];
	}
	delete[] bInitializedNormals;

	return true;
}

bool HeightMap::generateHeightMap(OSG::NodeRefPtr model, int xSamples, int ySamples, int height, int zRot,
		bool filter) {
	NodeRefPtr transNode;
	transNode = Node::create();
	TransformRefPtr trans;
	trans = Transform::create();
	Matrix m;
	m.setTranslate(0, height, 0);
	m.setRotate(Quaternion(Vec3f(0, 1, 0), zRot * PI / 180));
#if OSG_MAJOR_VERSION < 2
	beginEditCP(trans, Transform::MatrixFieldMask);
#endif
	trans->setMatrix(m);
#if OSG_MAJOR_VERSION < 2
	endEditCP(trans, Transform::MatrixFieldMask);
	beginEditCP(transNode, Node::CoreFieldMask, Node::ChildrenFieldMask);
#endif
	transNode->setCore(trans);
	transNode->addChild(model);
#if OSG_MAJOR_VERSION < 2
	endEditCP(transNode, Node::CoreFieldMask, Node::ChildrenFieldMask);
#endif

	setup(transNode, xSamples, ySamples, filter);
	return generateHeightMap(transNode);
}

void HeightMap::dumpHeight(const char* filename, float* minHeight, float* maxHeight) {
	float localMinH, localMaxH;

	if (!minHeight || !maxHeight) {
		getMinAndMaxHeightValues(&localMinH, &localMaxH);
		if (!minHeight)
			minHeight = &localMinH;
		if (!maxHeight)
			maxHeight = &localMaxH;
	}

	dumpPPM(height, xSamples, ySamples, 1.0f / (*maxHeight - *minHeight), -*minHeight * (1.0f
			/ (*maxHeight - *minHeight)), filename);
}

OSG::NodeTransitPtr HeightMap::exportGeometry(char* filename, char* normalsArrowModelFile) {
	NodeRefPtr node, box, arrow, tempArrow;
	int i, j;
	GroupRefPtr core;
	float dx, dy;
	Matrix4f transfMat;
	Matrix4f scaleMat;
	Matrix4f rotMat;
	float h;
	float hmin, hmax, eps;

	node = Node::create();
	core = Group::create();
	dx = xlen / (xSamples - 1);
	dy = ylen / (ySamples - 1);

	hmin = -1;
	hmax = 1;
	eps = 0.5f / (hmax - hmin);
	getMinAndMaxHeightValues(&hmin, &hmax);

#if OSG_MAJOR_VERSION >= 2
	arrow = NULL;
	if (normalsArrowModelFile)
		arrow = SceneFileHandler::the()->read(normalsArrowModelFile);
#else //OpenSG1:
	arrow = NullFC;
	if (normalsArrowModelFile)
		arrow = SceneFileHandler::the().read(normalsArrowModelFile);
#endif

#if OSG_MAJOR_VERSION < 2
	beginEditCP(node);
	beginEditCP(core);
#endif
	for (i = 0; i < ySamples; i++) {
		// 			printf("newline\n");
		for (j = 0; j < xSamples; j++) {
			if (!bInitialized[i * xSamples + j])
				continue;
			h = height[i * xSamples + j] - hmin + eps;
			NodeRefPtr transfNode;
			transfNode = Node::create();
			TransformRefPtr transfCore;
			transfCore = Transform::create();
			box = makeBox(dx, h, dy, 1, 1, 1);
			transfMat.setIdentity();
			transfMat.setTranslate(0.5 * dx + (float)i * dy, 0.5 * h, 0.5 * dy + (float)j * dy);
#if OSG_MAJOR_VERSION < 2
			beginEditCP(transfCore);
#endif
			transfCore->setMatrix(transfMat);
#if OSG_MAJOR_VERSION < 2
			endEditCP(transfCore);
			beginEditCP(transfNode);
#endif
			transfNode->addChild(box);
			transfNode->setCore(transfCore);
#if OSG_MAJOR_VERSION < 2
			endEditCP(transfNode);
#endif
			node->addChild(transfNode);

			if (arrow) {
				transfNode = Node::create();
				transfCore = Transform::create();
				tempArrow = cloneTree(arrow);
				// 					tempArrow = deepClone(arrow);

				rotMat.setIdentity();
				Vec3f tempVec[2];
				Vec3f tempNormal = normals[j * xSamples + i];
				if (tempNormal.enclosedAngle(up) > 0.001) {
					tempVec[0] = tempNormal.cross(up);
					tempVec[1] = tempVec[0].cross(tempNormal);
					// 						rotMat.setValue(tempVec[0], tempNormal, tempVec[1]);
					tempVec[0].normalize();
					tempVec[1].normalize();
					tempNormal.normalize();
					rotMat.setValue(tempVec[0][0], tempVec[0][1], tempVec[0][2], 0, tempNormal[0],
							tempNormal[1], tempNormal[2], 0, tempVec[1][0], tempVec[1][1],
							tempVec[1][2], 0, 0, 0, 0, 1);
					// 						printf("det: %0.03f\n", rotMat.det());
					rotMat.invert3();
				}

				transfMat.setIdentity();
				transfMat.setTranslate(0.5 * dx + (float)i * dy, h, 0.5 * dy + (float)j * dy);
				scaleMat.setIdentity();
				scaleMat.setScale(0.04f, 0.06f, 0.04f);
				// 					rotMat.setRotate(getNormalRotation(normals[i*xSamples+j]));
				// 					printf("(%0.03f, %0.03f, %0.03f)\n", tempNormal[0], tempNormal[1], tempNormal[2]);
				Vec3f test = Vec3f(0, 1, 0);
#if OSG_MAJOR_VERSION >= 2
				rotMat.mult(test,test);
#else
				rotMat.multMatrixVec(test);
#endif
				// 					printf("after: (%0.03f, %0.03f, %0.03f)\n",test[0], test[1], test[2]);
				// 					rotMat.setRotate(Quaternion(Vec3f(1, 0, 0), 0.5f*3.1459f));
				rotMat.mult(scaleMat);
				transfMat.mult(rotMat);
				// 					scaleMat.mult(rotMat);
				// // 					rotMat.mult(transfMat);
				// 					scaleMat.mult(transfMat);
				// 					transfMat.mult(scaleMat);
#if OSG_MAJOR_VERSION < 2
				beginEditCP(transfCore);
#endif
				transfCore->setMatrix(transfMat);
				// 						transfCore->setMatrix(scaleMat);
#if OSG_MAJOR_VERSION < 2
				endEditCP(transfCore);
				beginEditCP(transfNode);
#endif
				transfNode->addChild(tempArrow);
				transfNode->setCore(transfCore);
#if OSG_MAJOR_VERSION < 2
				endEditCP(transfNode);
#endif
				node->addChild(transfNode);
			}
		}
	}
#if OSG_MAJOR_VERSION < 2
	endEditCP(core);
#endif
	node->setCore(core);
#if OSG_MAJOR_VERSION >= 2
	SceneFileHandler::the()->write(node, filename);
#else //OpenSG1:
	endEditCP(node);
	SceneFileHandler::the().write(node, filename);
#endif
	return NodeTransitPtr(node);
}

NodeCoreTransitPtr HeightMap::makeGeometryPtr() {
#if OSG_MAJOR_VERSION >= 2
	GeoUInt8PropertyRecPtr type = GeoUInt8Property::create();
	GeoUInt32PropertyRecPtr length = GeoUInt32Property::create();
	GeoUInt32PropertyRecPtr indices = GeoUInt32Property::create();
	GeoPnt3fPropertyRecPtr pos = GeoPnt3fProperty::create();
	//GeoColor3fPropertyRecPtr colors = GeoColor3fProperty::create();
	GeoVec3fPropertyRecPtr norms = GeoVec3fProperty::create();
#else //OpenSG1:
	GeoPTypesPtr type = GeoPTypesUI8::create();
	GeoPLengthsPtr length = GeoPLengthsUI32::create();
	GeoIndicesUI32Ptr indices = GeoIndicesUI32::create();
	GeoPositions3fPtr pos = GeoPositions3f::create();
	//	GeoColors3fPtr colors = GeoColors3f::create();
	GeoNormals3fPtr norms = GeoNormals3f::create();
#endif
	SimpleMaterialRefPtr mat;
	mat = SimpleMaterial::create();
	GeometryRefPtr geo;
	geo = Geometry::create();
	Pnt3f vertex;

#if OSG_MAJOR_VERSION >= 2
	type->addValue(GL_QUADS);
	length->addValue((xSamples - 1) * (ySamples - 1) * 4);

	for (int x = 0; x < xSamples; x++)
		for (int y = 0; y < ySamples; y++) {
			vertex[0] = (float)x * dx + x0;
			vertex[1] = height[y * xSamples + x];
			vertex[2] = (float)y * dy + y0;
			pos->addValue(vertex);
		}

//	for(int x = 0; x < xSamples; x++)
//	for(int y = 0; y < ySamples; y++)
//	colors->addValue(Color3f(1, 1, 1));
	for (int x = 0; x < xSamples; x++)
		for (int y = 0; y < ySamples; y++) {
			norms->addValue(normals[y * xSamples + x]);
		}

	for (int x = 0; x < xSamples - 1; x++)
		for (int z = 0; z < ySamples - 1; z++) {
			indices->addValue(z * xSamples + x);
			indices->addValue((z + 1) * xSamples + x);
			indices->addValue((z + 1) * xSamples + x + 1);
			indices->addValue(z * xSamples + x + 1);
		}

	mat->setDiffuse(Color3f(1, 1, 1));

	geo->setTypes(type);
	geo->setLengths(length);
	geo->setIndices(indices);
	geo->setPositions(pos);
	geo->setNormals(norms);
	//		geo->setColors(colors);
	geo->setMaterial(mat);
#else //OpenSG1:
	beginEditCP(type, GeoPTypesUI8::GeoPropDataFieldMask);
	type->addValue(GL_QUADS);
	endEditCP(type, GeoPTypesUI8::GeoPropDataFieldMask);

	beginEditCP(length, GeoPLengthsUI32::GeoPropDataFieldMask);
	length->addValue((xSamples - 1) * (ySamples - 1) * 4);
	endEditCP(length, GeoPLengthsUI32::GeoPropDataFieldMask);

	beginEditCP(pos, GeoPositions3f::GeoPropDataFieldMask);
	for (int x = 0; x < xSamples; x++)
		for (int y = 0; y < ySamples; y++) {
			vertex[0] = (float)x * dx + x0;
			vertex[1] = height[y * xSamples + x];
			vertex[2] = (float)y * dy + y0;
			pos->addValue(vertex);
		}
	endEditCP(pos, GeoPositions3f::GeoPropDataFieldMask);

//	beginEditCP(colors, GeoColors3f::GeoPropDataFieldMask);
//	for(int x = 0; x < xSamples; x++)
//	for(int y = 0; y < ySamples; y++)
//	colors->addValue(Color3f(1, 1, 1));
//	endEditCP(colors, GeoColors3f::GeoPropDataFieldMask);
	beginEditCP(norms, GeoNormals3f::GeoPropDataFieldMask);
	for (int x = 0; x < xSamples; x++)
		for (int y = 0; y < ySamples; y++) {
			norms->addValue(normals[y * xSamples + x]);
		}
	endEditCP(norms, GeoNormals3f::GeoPropDataFieldMask);

	beginEditCP(indices, GeoIndicesUI32::GeoPropDataFieldMask);
	for (int x = 0; x < xSamples - 1; x++)
		for (int z = 0; z < ySamples - 1; z++) {
			indices->addValue(z * xSamples + x);
			indices->addValue((z + 1) * xSamples + x);
			indices->addValue((z + 1) * xSamples + x + 1);
			indices->addValue(z * xSamples + x + 1);
		}
	endEditCP(indices, GeoIndicesUI32::GeoPropDataFieldMask);

	beginEditCP(mat);
	mat->setDiffuse(Color3f(1, 1, 1));
	endEditCP(mat);
	beginEditCP(geo, Geometry::TypesFieldMask | Geometry::LengthsFieldMask
			| Geometry::IndicesFieldMask | Geometry::PositionsFieldMask
			| Geometry::NormalsFieldMask |
	//		Geometry::ColorsFieldMask |
			Geometry::MaterialFieldMask);

	geo->setTypes(type);
	geo->setLengths(length);
	geo->setIndices(indices);
	geo->setPositions(pos);
	geo->setNormals(norms);
	//		geo->setColors(colors);
	geo->setMaterial(mat);
	endEditCP(geo, Geometry::TypesFieldMask | Geometry::LengthsFieldMask
			| Geometry::IndicesFieldMask | Geometry::PositionsFieldMask
			| Geometry::NormalsFieldMask |
	//		Geometry::ColorsFieldMask |
			Geometry::MaterialFieldMask);
#endif

	return NodeCoreTransitPtr(geo);
}

void HeightMap::adjustZ(float scale, float off) {
	int i;
	for (i = 0; i < xSamples * ySamples; i++) {
		height[i] = height[i] * scale + off;
	}
}

bool HeightMap::writeToFile(const char* filename) {
	FILE* f;
	FILEHEADER_HMP1 h;
	size_t res;
	bool ret;

	assert(sizeof(float)*3 == sizeof(Vec3f));
	assert(sizeof(float) == sizeof(Real32));

	if (!filename)
		return false;
	f = fopen(filename, "wb");
	if (!f) {
		printd(WARNING, "HeightMap::writeToFile(): failed to open file %s for writing\n", filename);
		return false;
	}

	ret = true;
	h.typeID = 'HMP1';
	h.x0 = x0;
	h.y0 = y0;
	h.z0 = z0;
	h.xLen = xlen;
	h.yLen = ylen;
	h.xSamples = xSamples;
	h.ySamples = ySamples;
	h.dx = dx;
	h.dy = dy;

	res = fwrite(&h, sizeof(FILEHEADER_HMP1), 1, f);
	res += fwrite(height, sizeof(float) * xSamples * ySamples, 1, f);
	res += fwrite(normals, 3 * sizeof(float) * xSamples * ySamples, 1, f);
	res += fwrite(bInitialized, sizeof(char) * xSamples * ySamples, 1, f);
	fclose(f);
	if (res != 4) {
		printd(WARNING, "HeightMap::writeToFile(): file %s incomplete\n", filename);
		// 		remove(filename);
		ret = false;
	}
	return ret;
}

void HeightMap::writeToPPM(const char* filename, float scale, float off, bool findSelf) {
	int i;
	float maxHeight = 0, minHeight = 0;
	if (findSelf) {
		minHeight = height[0];
		maxHeight = height[0];
		for (i = 1; i < xSamples * ySamples; i++) {
			if (height[i] < minHeight)
				minHeight = height[i];
			if (height[i] > maxHeight)
				maxHeight = height[i];
		}
		dumpPPM(height, xSamples, ySamples, 1.0f / (maxHeight - minHeight), -minHeight / (maxHeight
				- minHeight), filename);
	} else {
		dumpPPM(height, xSamples, ySamples, scale, off, filename);
	}
}

void HeightMap::setGaussFilterCoefficients(float smoothness, float rectEps) {
	gaussSmoothness = 1.0f / sqrtf(smoothness);
	gaussRectEps = rectEps;
}

void HeightMap::getMinAndMaxHeightValues(float* minHeight, float* maxHeight) {
	int i;
	float localMinH, localMaxH;

	if (!minHeight && !maxHeight)
		return;

	// search for first initialized sample to initialize localMinH and localMaxH
	for (i = 0; i < xSamples * ySamples; i++) {
		if (bInitialized[i]) {
			localMinH = height[i];
			localMaxH = height[i];
			break;
		}
	}

	if (i == xSamples * ySamples) {
		if (minHeight)
			*minHeight = z0;
		if (maxHeight)
			*maxHeight = z0;
		return;
	}

	for (i = 0; i < xSamples * ySamples; i++) {
		if (!bInitialized[i])
			continue;
		if (height[i] < localMinH)
			localMinH = height[i];
		else if (height[i] > localMaxH)
			localMaxH = height[i];
	}

	if (minHeight)
		*minHeight = localMinH;
	if (maxHeight)
		*maxHeight = localMaxH;
}

Real32 HeightMap::getZ(Real32 x, Real32 y) {
	Real32 ret;
	int i;

	if (!getWeightsAndIndices(x, y))
		return z0;

	ret = 0;
	for (i = 0; i < 4; i++)
		ret += weights[i] * height[indices[i]];

	return ret;
}

float HeightMap::getHeight(float x, float z) {
	return getZ(x, z);
}

void HeightMap::getNormal(Real32 x, Real32 y, Vec3f* dst) {
	int i;

	if (!getWeightsAndIndices(x, y)) {
		*dst = up;
		return;
	}

	(*dst).setValues(0, 0, 0);
	for (i = 0; i < 4; i++)
		*dst += weights[i] * normals[indices[i]];

	return;
}

gmtl::Vec3f HeightMap::getNormal(float x, float z) {
	Vec3f tmp;
	getNormal(x, z, &tmp);
	return gmtl::Vec3f(tmp.x(), tmp.y(), tmp.z());
}

Quaternion HeightMap::getNormalRotation(Real32 x, Real32 y) {
	Vec3f normal;
	getNormal(x, y, &normal);
	return getNormalRotation(normal);
}

Quaternion HeightMap::getNormalRotation(const Vec3f& normal) {
	Quaternion ret;
	Real32 angle;
	Vec3f rotaxis;
	Vec3f planeNormal;

	angle = normal.enclosedAngle(up);

	rotaxis = up.cross(normal);
	planeNormal = rotaxis.cross(up);
	planeNormal.normalize();
	if (planeNormal.dot(normal) > 0.0)
		angle = -angle;
	ret.setValueAsAxisRad(rotaxis, angle);
	return ret;
}

void HeightMap::doIntersectionTests(NodeRefPtr model) {
	NodeCoreRefPtr core;
	int i, j, k;
	uint32_t nChildren, n;
	TriangleIterator it;
	Pnt3f pos[3], p;
	Vec3f normal[3], dir, uvw, uvs, res;
	Matrix4f modelTransform, invmodelTransformT;
	Vec3f v[2];
	Vec3f rhs;
	Vec3f row[3];
	Matrix4f m;

	core = model->getCore();
	if (!strcmp(core->getTypeName(), GEOMETRY_CORE_NAME)) {
		model->getToWorld(modelTransform);

		dir = down;
		invmodelTransformT.invertFrom(modelTransform);
		invmodelTransformT.transpose();
		it = TriangleIterator(model);

		for (it.seek(0); !it.isAtEnd(); ++it) {
			for (k = 0; k < 3; k++) {
#if OSG_MAJOR_VERSION >= 2
				modelTransform.mult(it.getPosition(k), pos[k]);
				invmodelTransformT.mult(it.getNormal(k), normal[k]);
#else
				modelTransform.multMatrixPnt(it.getPosition(k), pos[k]);
				invmodelTransformT.multMatrixVec(it.getNormal(k), normal[k]);
#endif
			}

			for (i = 0; i < 2; i++) {
				v[i] = pos[i] - pos[2];
			}

			m.setIdentity();
			for (i = 0; i < 3; i++) {
				row[i][0] = v[0][i];
				row[i][1] = v[1][i];
				row[i][2] = -dir[i];
			}
			m.setValue(row[0], row[1], row[2]);
			m.transpose();

			if (!m.invert3()) {
				continue;
			}

			for (i = 0; i < ySamples; i++) {
				for (j = 0; j < xSamples; j++) {
					p = Pnt3f(x0 + (float)j * dx, 0, y0 + (float)i * dy);

					// create intersection of line/triangle
					for (k = 0; k < 3; k++) {
						rhs[k] = p[k] - pos[2][k];
					}
#if OSG_MAJOR_VERSION >= 2
					m.mult(rhs, uvs);
#else
					m.multMatrixVec(rhs, uvs);
#endif
					// test if intersection of line/triangle lays inside the triangle
					if ((uvs.x() < (0 - RANGE)) || (uvs.y() < (0 - RANGE))) {
						continue;
					}
					if (uvs.x() + uvs.y() > (1 + RANGE)) {
						continue;
					}

					res.setValues(p[0], p[1], p[2]);
					res += uvs[2] * dir;
					if (!bInitialized[i * xSamples + j] || (height[i * xSamples + j] < res[1])) {
						height[i * xSamples + j] = res[1];

						// interpolation of normal
						uvw[0] = uvs[0];
						uvw[1] = uvs[1];
						uvw[2] = 1 - uvs[0] - uvs[1];
						res.setValues(0, 0, 0);
						for (k = 0; k < 3; k++)
							res += normal[k] * uvw[k];

						res.normalize();
						normals[i * xSamples + j] = res;
						bInitialized[i * xSamples + j] = -1;
					}
				}
			}
		}
	}

	nChildren = model->getNChildren();
	for (n = 0; n < nChildren; n++)
	{
		NodeRefPtr child;
		child = model->getChild(n);
		doIntersectionTests(child);
	//		the following line was used in the lndf app, but that's definitely not in the sense of gauss filtering
	// 		generateHeightMap(model->getChild(n));
	}

	return;
}

Real32* HeightMap::createGaussKernel(Real32 sigma, Int32* length, Real32 eps) {
	Real32 sigmasqr = sigma * sigma;
	Real32* ret;
	Real32 d, x, y, r, divisor, oneBySigmaSqr, vol;
	int i, j;

	*length = 2 * (int)(sqrt(-sigmasqr * log(eps * PI * sigmasqr)) + 1);
	ret = new Real32[*length * *length];

	divisor = 1 / (PI*sigmasqr);
	oneBySigmaSqr = 1.0f / sigmasqr;
	d = (Real32)*length / (Real32)(*length - 1);
	vol = 0;
	for (i = 0; i < *length; i++) {
		y = -(Real32)(*length / 2) + d * (Real32)i;
		for (j = 0; j < *length; j++) {
			x = -(Real32)(*length / 2) + d * (Real32)j;
			r = x * x + y * y;
			r *= -oneBySigmaSqr;
			ret[i * *length + j] = exp(r) * divisor;
			vol += ret[i * *length + j];
		}
	}

	vol = 1.0f / vol;
	for (i = 0; i < *length; i++)
		for (j = 0; j < *length; j++) {
			ret[i * *length + j] *= vol;
		}

	return ret;
}

Real32 HeightMap::conv1D(Real32* fun, Real32* filterKrnl, Int32 fmax, Int32 filterMax,
		Int32 filterStart, Int32 x, Real32* areaBelowFilter, char* bInitalized) {
	Real32 ret = 0;
	int i;
	int iStart, iEnd;

	iStart = MAX2(x-fmax, filterStart);
	iEnd = MIN2(x, filterMax);
	*areaBelowFilter = 0;

	for (i = iStart; i <= iEnd; i++) {
		if (!bInitalized[x - i])
			continue;
		ret += fun[x - i] * filterKrnl[i - filterStart];
		*areaBelowFilter += filterKrnl[i - filterStart];
	}

	return ret;
}

Real32 HeightMap::conv2D(Real32* fun, Int32 funMaxX, Int32 funMaxY, Real32* filterKrnl,
		Int32 filterMaxX, Int32 filterStartX, Int32 filterMaxY, Int32 filterStartY, Int32 x,
		Int32 y, char* bInitialized) {
	Real32 ret = 0;
	Real32 temp, vol = 0;
	int j;
	int jStart, jEnd;
	int flen = funMaxX + 1;
	int glen = filterMaxX - filterStartX + 1;

	jStart = MAX2(y-funMaxY, filterStartY);
	jEnd = MIN2(y, filterMaxY);

	for (j = jStart; j <= jEnd; j++) {
		//ret += Conv1D(&f[(y-j-fstarty)*flen], &g[(j-gstarty)*glen], fxmax, gxmax, fstartx, gstartx, x);

		ret += conv1D(&fun[(y - j) * flen], &filterKrnl[(j - filterStartY) * glen], funMaxX,
				filterMaxX, filterStartX, x, &temp, &bInitialized[(y - j) * flen]);
		vol += temp;
	}
	if (vol < 0.001)
		return ret;
	return ret / vol;
}

bool HeightMap::getWeightsAndIndices(Real32 x, Real32 y) {
	Real32 xSample, ySample, dfxSample, dfySample;
	int fxSample, fySample;

	x -= x0;
	y -= y0;

	xSample = x / dx;
	ySample = y / dy;

	// x,y must lay inside the triangle
	if ((xSample <= 0) || (xSample >= (float)(xSamples - 1)) || (ySample <= 0) || (ySample
			>= (float)(ySamples - 1)))
		return false;

	fxSample = (int)floor(xSample);
	fySample = (int)floor(ySample);
	dfxSample = xSample - (float)fxSample;
	dfySample = ySample - (float)fySample;

	indices[0] = (int)fxSample + xSamples * (int)fySample;
	indices[1] = (int)fxSample + 1 + xSamples * (int)fySample;
	indices[2] = (int)fxSample + xSamples * ((int)fySample + 1);
	indices[3] = (int)fxSample + 1 + xSamples * ((int)fySample + 1);

	weights[0] = (1 - dfxSample) * (1 - dfySample);
	weights[1] = dfxSample * (1 - dfySample);
	weights[2] = (1 - dfxSample) * dfySample;
	weights[3] = dfxSample * dfySample;

	return true;
}

void HeightMap::dumpPPM(Real32* data, int xLen, int yLen, Real32 scale, Real32 off,
		const char* file) {
	FILE* f = fopen(file, "w");
	int c;
	if (!f)
		return;
	fprintf(f, "P3\n#Heightmap debug output\n%d %d\n255\n", xLen, yLen);
	int i, j;
	for (i = 0; i < yLen; i++)
		for (j = 0; j < xLen; j++) {
			c = (unsigned)((data[i * xLen + j] * scale + off) * 255.0f);
			if (c < 0)
				c = 0;
			if (c > 255)
				c = 255;
			fprintf(f, "%d\n%d\n%d\n", c, c, c);
		}
	fclose(f);
}

void HeightMap::dumpPPMVec3f(Vec3f* data, int xLen, int yLen, Real32 scale, Real32 off,
		const char* file) {
	FILE* f = fopen(file, "w");
	int c;
	Real32 toff;
	if (!f)
		return;
	fprintf(f, "P3\n#Heightmap debug output\n%d %d\n255\n", xLen, yLen);
	int i, j, k;
	for (i = 0; i < yLen; i++)
		for (j = 0; j < xLen; j++) {
			for (k = 0; k < 3; k++) {
				toff = (k == 1 ? -0.2 : off);
				c = (unsigned)((data[i * xLen + j][k] * scale + toff) * 255.0f);
				if (c < 0)
					c = 0;
				if (c > 255)
					c = 255;
				fprintf(f, "%d\n", c);
			}
		}
	fclose(f);
}

