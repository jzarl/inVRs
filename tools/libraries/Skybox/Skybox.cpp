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


#include "Skybox.h"
#include <inVRs/SystemCore/DebugOutput.h>
#include <iostream>

#if OSG_MAJOR_VERSION >= 2
#include <OpenSG/OSGTypedGeoVectorProperty.h>
#define NullFC NULL
#endif


OSG_USING_NAMESPACE

Skybox::Skybox()
: xSize(0.0),
ySize(0.0),
zSize(0.0),
farClipDistance(0.0),
transf(NullFC),
transfCore(NullFC),
skybox(NullFC),
skyboxCore(NullFC),
initialized(false),
sortKey(0)
{
	asCubemap = NullFC;
}

Skybox::~Skybox() {

}

bool Skybox::init(float xSize, float ySize, float zSize, float farClipDist, std::string texNegY,
		std::string texPosY, std::string texNegX, std::string texPosX, std::string texNegZ,
		std::string texPosZ, int sortKey) {
	float eps;
	bool ret;


	// set sort key to render always in background
	this->sortKey = sortKey;
//	 for(i=0;i<6;i++)
//	 {
//	 textures[i] = TextureChunk::create();
//	}

	ret = setTextures(texNegY, texPosY, texNegX, texPosX, texNegZ, texPosZ);

	skybox = Node::create();
	skyboxCore = Group::create();
#if OSG_MAJOR_VERSION < 2
	beginEditCP(skybox, Node::CoreFieldMask);
#endif
	skybox->setCore(skyboxCore);
#if OSG_MAJOR_VERSION < 2
	endEditCP(skybox, Node::CoreFieldMask);
#endif

	eps = 1.001f;


//	 createSide(SIDE_NEGX,
//	 Pnt3f(-0.5f, -0.5f*eps, 0.5f*eps),
//	 Pnt3f(-0.5f, 0.5f*eps, 0.5f*eps),
//	 Pnt3f(-0.5f, 0.5f*eps, -0.5f*eps),
//	 Pnt3f(-0.5f, -0.5f*eps, -0.5f*eps));

	createSide(SIDE_NEGX, Pnt3f(-0.5f, -0.5f * eps, -0.5f * eps), Pnt3f(-0.5f, 0.5f * eps, -0.5f
			* eps), Pnt3f(-0.5f, 0.5f * eps, 0.5f * eps), Pnt3f(-0.5f, -0.5f * eps, 0.5f * eps));

	createSide(SIDE_POSX, Pnt3f(0.5f, -0.5f * eps, 0.5f * eps),
			Pnt3f(0.5f, 0.5f * eps, 0.5f * eps), Pnt3f(0.5f, 0.5f * eps, -0.5f * eps), Pnt3f(0.5f,
					-0.5f * eps, -0.5f * eps));

	createSide(SIDE_POSY, Pnt3f(0.5f * eps, 0.5f, -0.5f * eps),
			Pnt3f(0.5f * eps, 0.5f, 0.5f * eps), Pnt3f(-0.5f * eps, 0.5f, 0.5f * eps), Pnt3f(-0.5f
					* eps, 0.5f, -0.5f * eps));

	createSide(SIDE_NEGY, Pnt3f(-0.5f * eps, -0.5f, -0.5f * eps), Pnt3f(0.5f * eps, -0.5f, -0.5f
			* eps), Pnt3f(0.5f * eps, -0.5f, 0.5f * eps), Pnt3f(-0.5f * eps, -0.5f, 0.5f * eps));


//	 createSide(SIDE_POSZ,
//	 Pnt3f(0.5f*eps, -0.5f*eps, 0.5f),
//	 Pnt3f(0.5f*eps, 0.5f*eps, 0.5f),
//	 Pnt3f(-0.5f*eps, 0.5f*eps, 0.5f),
//	 Pnt3f(-0.5f*eps, -0.5f*eps, 0.5f)
//	 );


	createSide(SIDE_POSZ, Pnt3f(-0.5f * eps, -0.5f * eps, 0.5f), Pnt3f(-0.5f * eps, 0.5f * eps,
			0.5f), Pnt3f(0.5f * eps, 0.5f * eps, 0.5f), Pnt3f(0.5f * eps, -0.5f * eps, 0.5f));

	createSide(SIDE_NEGZ, Pnt3f(0.5f * eps, -0.5f * eps, -0.5f), Pnt3f(0.5f * eps, 0.5f * eps,
			-0.5f), Pnt3f(-0.5f * eps, 0.5f * eps, -0.5f), Pnt3f(-0.5f * eps, -0.5f * eps, -0.5f));

	transf = Node::create();
	transfCore = Transform::create();
#if OSG_MAJOR_VERSION < 2
	beginEditCP(transf, Node::CoreFieldMask | Node::ChildrenFieldMask);
#endif
	transf->setCore(transfCore);
	transf->addChild(skybox);
#if OSG_MAJOR_VERSION < 2
	endEditCP(transf, Node::CoreFieldMask | Node::ChildrenFieldMask);
#endif

	setSize(xSize, ySize, zSize, farClipDist);
	initialized = true;
	return ret;
}

void Skybox::setSize(float xSize, float ySize, float zSize, float farClipDist) {
	float s;

	this->xSize = xSize;
	this->ySize = ySize;
	this->zSize = zSize;
	this->farClipDistance = farClipDist;

	scale.setIdentity();
	s = 2.0f * farClipDist / sqrt(xSize * xSize + ySize * ySize + zSize * zSize);
	scale.setScale(xSize * s * 0.99, ySize * s * 0.99, zSize * s * 0.99);
}

bool Skybox::setTextures(std::string texNegY, std::string texPosY, std::string texNegX,
		std::string texPosX, std::string texNegZ, std::string texPosZ) {
	int i;
	bool ret = true;

	texPaths[SIDE_NEGZ] = texNegZ;
	texPaths[SIDE_POSZ] = texPosZ;
	texPaths[SIDE_NEGX] = texNegX;
	texPaths[SIDE_POSX] = texPosX;
	texPaths[SIDE_NEGY] = texNegY;
	texPaths[SIDE_POSY] = texPosY;

	//for(i=0;i<6;i++)
	//{
	//	ret = LoadTexture(textures[i], texPaths[i]) && ret;
	//}

	for (i = 0; i < 6; i++) {
		textures[i] = textureManager.loadTexture(texPaths[i]);
		if (textures[i] == NullFC)
			ret = false;
	}

	return ret;
}

//bool Skybox::LoadTexture(TextureChunkPtr dst, std::string path)
//{
//	ImagePtr img = Image::create();
//	beginEditCP(img);
//		img->read((Char8*)path.c_str());
//	endEditCP(img);
//    beginEditCP(dst);
//		dst->setImage(img);
//		dst->setMinFilter(GL_LINEAR);
//		dst->setMagFilter(GL_LINEAR);
//		#ifndef BIGENDIAN
//		dst->setWrapS(GL_CLAMP_TO_EDGE);
//		dst->setWrapT(GL_CLAMP_TO_EDGE);
//		#else
//		dst->setWrapS(GL_CLAMP_TO_EDGE_SGIS);
//		dst->setWrapT(GL_CLAMP_TO_EDGE_SGIS);
//		#endif
//		// f?r onyx: GL_CLAMP_TO_EDGE_SGIS (=0x812F)
//		dst->setEnvMode(GL_REPLACE);
//    endEditCP(dst);
//	return true;
//}

GeometryRefPtr Skybox::createQuad(Pnt3f v0, Pnt3f v1, Pnt3f v2, Pnt3f v3, TextureObjChunkRefPtr texture) {
#if OSG_MAJOR_VERSION >= 2
	GeoUInt8PropertyRecPtr type = GeoUInt8Property::create();
	GeoUInt32PropertyRecPtr length = GeoUInt32Property::create();
	GeoPnt3fPropertyRecPtr pos = GeoPnt3fProperty::create();
	GeoUInt32PropertyRecPtr indices = GeoUInt32Property::create();
	GeoVec2fPropertyRecPtr texc = GeoVec2fProperty::create();
#else //OpenSG1:
	GeoPTypesPtr type = GeoPTypesUI8::create();
	GeoPLengthsPtr length = GeoPLengthsUI32::create();
	GeoPositions3fPtr pos = GeoPositions3f::create();
	GeoIndicesUI32Ptr indices = GeoIndicesUI32::create();
	GeoTexCoords2fPtr texc = GeoTexCoords2f::create();
#endif
	GeometryRefPtr geo;
	geo = Geometry::create();
	ChunkMaterialRefPtr mat;
	mat = ChunkMaterial::create();

#if OSG_MAJOR_VERSION >= 2
	mat->addChunk(texture);
	mat->setSortKey(sortKey);

	type->addValue(GL_QUADS);

	length->addValue(4);

	pos->addValue(v0);
	pos->addValue(v1);
	pos->addValue(v2);
	pos->addValue(v3);

	texc->addValue(Vec2f(0, 0));
	texc->addValue(Vec2f(0, 1));
	texc->addValue(Vec2f(1, 1));
	texc->addValue(Vec2f(1, 0));

	indices->addValue(0);
	indices->addValue(1);
	indices->addValue(2);
	indices->addValue(3);

	geo->setTypes(type);
	geo->setLengths(length);
	geo->setIndices(indices);
	geo->setPositions(pos);
	geo->setTexCoords(texc);
	geo->setMaterial(mat);
#else //OpenSG1:
	beginEditCP(mat);
		mat->addChunk(texture);
		mat->setSortKey(sortKey);
	endEditCP(mat);

	beginEditCP(type, GeoPTypesUI8::GeoPropDataFieldMask);
	type->addValue(GL_QUADS);
	endEditCP(type, GeoPTypesUI8::GeoPropDataFieldMask);

	beginEditCP(length, GeoPLengthsUI32::GeoPropDataFieldMask);
	length->addValue(4);
	endEditCP(length, GeoPLengthsUI32::GeoPropDataFieldMask);

	beginEditCP(pos, GeoPositions3f::GeoPropDataFieldMask);
	pos->addValue(v0);
	pos->addValue(v1);
	pos->addValue(v2);
	pos->addValue(v3);
	endEditCP(pos, GeoPositions3f::GeoPropDataFieldMask);

	beginEditCP(texc, GeoTexCoords2f::GeoPropDataFieldMask);
	texc->addValue(Vec2f(0, 0));
	texc->addValue(Vec2f(0, 1));
	texc->addValue(Vec2f(1, 1));
	texc->addValue(Vec2f(1, 0));
	endEditCP(texc, GeoTexCoords2f::GeoPropDataFieldMask);

	beginEditCP(indices, GeoIndicesUI32::GeoPropDataFieldMask);
	indices->addValue(0);
	indices->addValue(1);
	indices->addValue(2);
	indices->addValue(3);
	endEditCP(indices, GeoIndicesUI32::GeoPropDataFieldMask);

	beginEditCP(geo, Geometry::TypesFieldMask | Geometry::LengthsFieldMask
			| Geometry::IndicesFieldMask | Geometry::PositionsFieldMask
			| Geometry::TexCoordsFieldMask | Geometry::MaterialFieldMask);
	geo->setTypes(type);
	geo->setLengths(length);
	geo->setIndices(indices);
	geo->setPositions(pos);
	geo->setTexCoords(texc);
	geo->setMaterial(mat);
	endEditCP(geo, Geometry::TypesFieldMask | Geometry::LengthsFieldMask
			| Geometry::IndicesFieldMask | Geometry::PositionsFieldMask
			| Geometry::TexCoordsFieldMask | Geometry::MaterialFieldMask);
#endif

	return geo;
}

void Skybox::createSide(SIDE s, Pnt3f v0, Pnt3f v1, Pnt3f v2, Pnt3f v3) {
	side[s] = Node::create();
#if OSG_MAJOR_VERSION < 2
	beginEditCP(side[s], Node::CoreFieldMask);
#endif
	side[s]->setCore(createQuad(v0, v1, v2, v3, textures[s]));
#if OSG_MAJOR_VERSION < 2
	endEditCP(side[s], Node::CoreFieldMask);
	beginEditCP(skybox, Node::ChildrenFieldMask);
#endif
	skybox->addChild(side[s]);
#if OSG_MAJOR_VERSION < 2
	endEditCP(skybox, Node::ChildrenFieldMask);
#endif
}

NodeTransitPtr Skybox::getNodePtr() {
	if (!initialized) {
		printd(ERROR, "Skybox::getNodePtr(): this skybox instance is uninitialized\n");
#if OSG_MAJOR_VERSION >= 2
		return NodeTransitPtr(NULL);
#else
		return NullFC;
#endif
	}
	return NodeTransitPtr(transf);
}

void Skybox::setupRender(Vec3f camPos) {
	Matrix4f camTransl, final;

	if (!initialized) {
		printd(ERROR, "Skybox::setupRender(): this skybox instance is uninitialized\n");
		return;
	}

	camTransl.setIdentity();
	camTransl.setTranslate(camPos);

	final.setIdentity();
	final.multLeft(scale);
	final.multLeft(camTransl);

#if OSG_MAJOR_VERSION < 2
	beginEditCP(transfCore, Transform::MatrixFieldMask);
#endif
	transfCore->setMatrix(final);
#if OSG_MAJOR_VERSION < 2
	endEditCP(transfCore, Transform::MatrixFieldMask);
#endif
}

void Skybox::setupRender(gmtl::Vec3f camPos) {
	Vec3f osgCamPos;
	osgCamPos[0] = camPos[0];
	osgCamPos[1] = camPos[1];
	osgCamPos[2] = camPos[2];
	setupRender(osgCamPos);
}

CubeTextureObjChunkRefPtr Skybox::getSkyboxCubemap() {
#ifdef BIGENDIAN
	printd(ERROR, "Skybox::getSkyboxCubemap(): cubemaps are not supported\n");
	return NullFC;
#endif
	if (asCubemap)
		return asCubemap;

	asCubemap = CubeTextureObjChunk::create();
#if OSG_MAJOR_VERSION >= 2
	ImageRefPtr img;
#else
	ImagePtr img;
#endif
	img = Image::create();

#if OSG_MAJOR_VERSION < 2
	beginEditCP(asCubemap);
#endif
	img->read((Char8*)texPaths[SIDE_NEGZ].c_str());
	asCubemap->setImage(img); // is negative z
	img->read((Char8*)texPaths[SIDE_POSZ].c_str());
	asCubemap->setPosZImage(img);
	img->read((Char8*)texPaths[SIDE_NEGX].c_str());
	asCubemap->setNegXImage(img);
	img->read((Char8*)texPaths[SIDE_POSX].c_str());
	asCubemap->setPosXImage(img);
	img->read((Char8*)texPaths[SIDE_NEGY].c_str());
	asCubemap->setNegYImage(img);
	img->read((Char8*)texPaths[SIDE_POSY].c_str());
	asCubemap->setPosYImage(img);
#if OSG_MAJOR_VERSION < 2
	endEditCP(asCubemap);
#endif

	return asCubemap;
}

void SkyboxTextureManager::initializeTexture(std::string path, TextureObjChunkRefPtr tex) {
	tex->setMinFilter(GL_LINEAR);
	tex->setMagFilter(GL_LINEAR);
	tex->setWrapS(GL_CLAMP_TO_EDGE);
	tex->setWrapT(GL_CLAMP_TO_EDGE);
#if OSG_MAJOR_VERSION < 2
	// 2013-03-09 ZaJ: not available in OpenSG2. -> not neccessary any more?
	tex->setEnvMode(GL_REPLACE);
#endif
}
