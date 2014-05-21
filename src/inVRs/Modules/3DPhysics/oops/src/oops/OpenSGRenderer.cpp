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

#include "oops/OpenSGRenderer.h"

#include <OpenSG/OSGMatrix.h>
#include <OpenSG/OSGQuaternion.h>
#include <OpenSG/OSGSimpleGeometry.h>
#include <OpenSG/OSGGeoFunctions.h>
#include <OpenSG/OSGMaterialGroup.h>

#if OSG_MAJOR_VERSION >= 2
#include <OpenSG/OSGGeoUInt8Property.h>
#include <OpenSG/OSGGeoUInt32Property.h>
#include <OpenSG/OSGGeoPnt3fProperty.h>
#include <OpenSG/OSGGeoVec3fProperty.h>
#endif

#include "oops/RigidBody.h"
#include "oops/Geometries.h"

OSG_USING_NAMESPACE

// convert(gmtl::Vec3f &src, OSG::Vec3f &dst) was removed from DataTypes.h in r2194:
void convert(const gmtl::Vec3f &src, OSG::Vec3f &dst)
{
	dst[0] = src[0];
	dst[1] = src[1];
	dst[2] = src[2];
}
// convert(gmtl::Quatf &src, OSG::Quaternion &dst) was removed from DataTypes.h in r2194:
void convert(const gmtl::Quatf &src, OSG::Quaternion &dst)
{
	dst.setValueAsQuat(src.getData());
}

// Object Oriented Physics Simulation
namespace oops
{

OpenSGRenderer::OpenSGRenderer(OSG::NodePtr root)
 : object(0)
{
	renderNode = Node::create();
	renderCore = Transform::create();

	beginEditCP(renderNode, Node::CoreFieldMask);
		renderNode->setCore(renderCore);
	endEditCP(renderNode, Node::CoreFieldMask);

	setVisible(false);

	material = SimpleMaterial::create();

	beginEditCP(material);
		material->setDiffuse(Color3f(0.8, 0.8, 0.8));
	endEditCP(material);

	MaterialGroupPtr matGroup = MaterialGroup::create();
	beginEditCP(matGroup);
		matGroup->setMaterial(material);
	endEditCP(matGroup);

	matGroupNode = Node::create();
	beginEditCP(matGroupNode, Node::CoreFieldMask | Node::ChildrenFieldMask);
		matGroupNode->setCore(matGroup);
		matGroupNode->addChild(renderNode);
	endEditCP(matGroupNode, Node::CoreFieldMask | Node::ChildrenFieldMask);

	beginEditCP(root, Node::ChildrenFieldMask);
		root->addChild(matGroupNode);
	endEditCP(root, Node::ChildrenFieldMask);

	objectActive = false;
	this->rootNode = root;
} // OpenSGRenderer

OpenSGRenderer::~OpenSGRenderer()
{
	beginEditCP(rootNode, Node::ChildrenFieldMask);
		rootNode->subChild(matGroupNode);
	endEditCP(rootNode, Node::ChildrenFieldMask);
} // ~OpenSGRenderer

RendererInterface* OpenSGRenderer::clone()
{
	OpenSGRenderer* newRenderer = new OpenSGRenderer(this->rootNode);
	newRenderer->setVisible(isVisible);
	return newRenderer;
} // clone

void OpenSGRenderer::setObject(RigidBody* object)
{
	if(this->object) {
		// remove this object from the scene graph
		beginEditCP(renderNode, Node::ChildrenFieldMask);
			const UInt32 index = 0;
			renderNode->subChild(index);
		endEditCP(renderNode, Node::ChildrenFieldMask);
		// replace by different node
	} // if

	NodePtr geometryNode;
	this->object = object;
	Geometry* geom = object->getGeometry();
	geometryNode = buildSceneGraph(geom);

	beginEditCP(renderNode, Node::ChildrenFieldMask);
		renderNode->addChild(geometryNode);
		renderNode->invalidateVolume();
	endEditCP(renderNode, Node::ChildrenFieldMask);
} // setObject

void OpenSGRenderer::setVisible(bool visible)
{
	beginEditCP(renderNode);
		renderNode->setActive(visible);
	endEditCP(renderNode);
	changeVisibility = true;
	isVisible = visible;
} // setVisible

void OpenSGRenderer::render()
{
	if (changeVisibility) {
		beginEditCP(renderNode);
			renderNode->setActive(isVisible);
		endEditCP(renderNode);
		changeVisibility = false;
	} // if

	if (!isVisible)
		return;

	OSG::Matrix m;
	OSG::Quaternion quat;
	TransformationData trans = object->getTransformation();

	m.setIdentity();
	m.setTranslate(trans.position[0], trans.position[1], trans.position[2]);
	quat.setValueAsQuat(trans.orientation.getData());
	m.setRotate(quat);
	beginEditCP(renderCore, OSG::Transform::MatrixFieldMask);
		renderCore->setMatrix(m);
	endEditCP(renderCore, OSG::Transform::MatrixFieldMask);

	if ((!object->isActive() && objectActive) || (object->isActive() && !objectActive))
	{
		objectActive = object->isActive();
		beginEditCP(material);
			if (objectActive)
				material->setDiffuse(Color3f(0.2, 0.2, 1.0));
			else
				material->setDiffuse(Color3f(0.8, 0.8, 0.8));
		endEditCP(material);
	} // if
} // render

NodePtr OpenSGRenderer::buildSceneGraph(Geometry* geom)
{
	if (dynamic_cast<Sphere*>(geom))
		return buildSphere((Sphere*)geom);

	if (dynamic_cast<Box*>(geom))
		return buildBox((Box*)geom);

	if (dynamic_cast<Capsule*>(geom))
		return buildCapsule((Capsule*)geom);

	if (dynamic_cast<TriangleMesh*>(geom))
		return buildTriMesh(((TriangleMesh*)geom)->getData());

	if (dynamic_cast<Cylinder*>(geom))
		return buildCylinder((Cylinder*)geom);

	if (dynamic_cast<CompositeGeometry*>(geom))
		return buildCompositeGeo((CompositeGeometry*)geom);

	if (dynamic_cast<HeightField*>(geom))
		return buildHeightField((HeightField*)geom);

	printf("OpenSGRenderer::buildSceneGraph(): ERROR: unknown geometry-type!\n");
	return NullFC;
} // buildSceneGraph

NodePtr OpenSGRenderer::buildSphere(Sphere* sphere)
{
	float radius = sphere->getRadius();

	NodePtr sphereNode = Node::create();
	GeometryPtr sphereGeo = makeSphereGeo(2, radius);
	beginEditCP(sphereNode, Node::CoreFieldMask);
		sphereNode->setCore(sphereGeo);
	endEditCP(sphereNode, Node::CoreFieldMask);

	return sphereNode;
} // buildSphere

NodePtr OpenSGRenderer::buildBox(Box* box)
{
	gmtl::Vec3f size = box->getSize();

	NodePtr boxNode = Node::create();
	GeometryPtr boxGeo = makeBoxGeo(size[0], size[1], size[2], 1, 1, 1);
	beginEditCP(boxNode, Node::CoreFieldMask);
		boxNode->setCore(boxGeo);
	endEditCP(boxNode, Node::CoreFieldMask);

	return boxNode;
} // buildBox

void OpenSGRenderer::ccylinderLoop(TriangleMeshData* data, int index, float capRadius, float posY)
{
	int i;
	float posX, posZ;

	for (i=0; i < 16; i++)
	{
		posX = capRadius * sin( (i*360.f/16.f) * M_PI/180.f);
		posZ = capRadius * cos( (i*360.f/16.f) * M_PI/180.f);
		data->vertices[index+i][0] = posX;
		data->vertices[index+i][1] = posY;
		data->vertices[index+i][2] = posZ;
	} // for
} // ccylinderLoop

NodePtr OpenSGRenderer::buildCapsule(Capsule* ccylinder)
{
	NodePtr result;
	int i, j, index, vertexIndex;
	float capRadius;
	float posY;
	float length = ccylinder->getLength();
	float radius = ccylinder->getRadius();
	TriangleMeshData* data = new TriangleMeshData;
	data->nVertices = 98;
	data->nTriangles = 192;
	data->triangles = new TriMeshTriangle[data->nTriangles];
	data->vertices = new dVector3[data->nVertices];

	data->vertices[0][0] = 0;
	data->vertices[0][1] = -length/2 - radius;
	data->vertices[0][2] = 0;

	data->vertices[97][0] = 0;
	data->vertices[97][1] = length/2 + radius;
	data->vertices[97][2] = 0;

	index = 1;
	capRadius = radius * sin(30.f*M_PI/180.f);
	posY = -length/2 - radius * cos(30.f*M_PI/180.f);
	ccylinderLoop(data, index, capRadius, posY);

	index += 16;
	capRadius = radius * sin(60.f*M_PI/180.f);
	posY = -length/2 - radius * cos(60.f*M_PI/180.f);
	ccylinderLoop(data, index, capRadius, posY);

	index += 16;
	capRadius = radius;
	posY = -length/2;
	ccylinderLoop(data, index, capRadius, posY);

	index += 16;
	capRadius = radius;
	posY = length/2 ;
	ccylinderLoop(data, index, capRadius, posY);

	index += 16;
	capRadius = radius * sin(60.f*M_PI/180.f);
	posY = length/2 + radius * cos(60.f*M_PI/180.f);
	ccylinderLoop(data, index, capRadius, posY);

	index += 16;
	capRadius = radius * sin(30.f*M_PI/180.f);
	posY = length/2 + radius * cos(30.f*M_PI/180.f);
	ccylinderLoop(data, index, capRadius, posY);

	index = 0;
	for (i=0; i < 16; i++)
	{
		data->triangles[index+i].index[0] = 0;
		data->triangles[index+i].index[2] = i+1;
		if (i==15)
			data->triangles[index+i].index[1] = 1;
		else
			data->triangles[index+i].index[1] = i+2;
	} // for

	index += 16;
	vertexIndex = 1;
	for (j=0; j < 5; j++)
	{
		for (i=0; i < 16; i++)
		{
			data->triangles[index+2*i].index[0] = vertexIndex+i;
			if (i==15)
				data->triangles[index+2*i].index[1] = vertexIndex;
	//			data->triangles[index+2*i].index[1] = 1;
			else
				data->triangles[index+2*i].index[1] = vertexIndex + 1 + i;
	// 			data->triangles[index+2*i].index[1] = 2+i;

			data->triangles[index+2*i].index[2] = vertexIndex + 16 + i;
	// 		data->triangles[index+2*i].index[2] = 17+i;

			data->triangles[index+2*i+1].index[0] = data->triangles[index+2*i].index[1];
			data->triangles[index+2*i+1].index[2] = data->triangles[index+2*i].index[2];
			if (i==15)
				data->triangles[index+2*i+1].index[1] = vertexIndex + 16;
	//			data->triangles[index+2*i+1].index[2] = 17;
			else
				data->triangles[index+2*i+1].index[1] = vertexIndex + 16 + 1 + i;
	//			data->triangles[index+2*i+1].index[2] = 18+i;
		} // for
		vertexIndex += 16;
		index += 32;
	} // for

	for (i=0; i < 16; i++)
	{
		data->triangles[index+i].index[0] = 97;
		data->triangles[index+i].index[2] = 97-(i+1);
		if (i==15)
			data->triangles[index+i].index[1] = 96;
		else
			data->triangles[index+i].index[1] = 97-(i+2);
	} // for

	result = buildTriMesh(data);
	delete(data);

	return result;
} // buildCapsule

NodePtr OpenSGRenderer::buildCylinder(Cylinder* cylinder)
{
	float length = cylinder->getLength();
	float radius = cylinder->getRadius();

	Matrix m;
	Quaternion quat;
	quat.setValueAsAxisDeg(Vec3f(1,0,0), 90);
	m.setIdentity();
	m.setRotate(quat);

	NodePtr cylinderTransNode = Node::create();
	TransformPtr cylinderTrans = Transform::create();
	beginEditCP(cylinderTrans);
		cylinderTrans->setMatrix(m);
	endEditCP(cylinderTrans);

	NodePtr cylinderNode = Node::create();
	GeometryPtr cylinderGeo = makeCylinderGeo(length, radius, 16, 1, 1, 1);
	beginEditCP(cylinderNode, Node::CoreFieldMask);
		cylinderNode->setCore(cylinderGeo);
	endEditCP(cylinderNode, Node::CoreFieldMask);

	beginEditCP(cylinderTransNode);
		cylinderTransNode->setCore(cylinderTrans);
		cylinderTransNode->addChild(cylinderNode);
	endEditCP(cylinderTransNode);


	return cylinderTransNode;
} // buildCylinder

NodePtr OpenSGRenderer::buildTriMesh(TriangleMeshData* data)
{
	int i;
	Vec3f vec;

	NodePtr triMeshNode = Node::create();
	GeometryPtr triMeshGeom = OSG::Geometry::create();

#if OSG_MAJOR_VERSION >= 2
	GeoUInt8PropertyRecPtr type = GeoUInt8Property::create();
	GeoUInt32PropertyRecPtr length = GeoUInt32Property::create();
	GeoPnt3fPropertyRecPtr pos = GeoPnt3fProperty::create();
	GeoUInt32PropertyRecPtr indices = GeoUInt32Property::create();
	type->addValue(GL_TRIANGLES);
	length->addValue(3 * data->nTriangles);
	for (i=0; i < data->nVertices; i++)
	{
		vec = Vec3f(data->vertices[i][0], data->vertices[i][1], data->vertices[i][2]);
		pos->addValue(vec);
	} // for
	for (i=0; i < data->nTriangles; i++)
	{
		indices->addValue(data->triangles[i].index[0]);
		indices->addValue(data->triangles[i].index[1]);
		indices->addValue(data->triangles[i].index[2]);
	} // for
#else //OpenSG1:
	GeoPTypesPtr type = GeoPTypesUI8::create();
	beginEditCP(type, GeoPTypesUI8::GeoPropDataFieldMask);
		type->addValue(GL_TRIANGLES);
	endEditCP(type, GeoPTypesUI8::GeoPropDataFieldMask);

	GeoPLengthsPtr length = GeoPLengthsUI32::create();
	beginEditCP(length, GeoPLengthsUI32::GeoPropDataFieldMask);
		length->addValue(3 * data->nTriangles);
	endEditCP(length, GeoPLengthsUI32::GeoPropDataFieldMask);

	GeoPositions3fPtr pos = GeoPositions3f::create();
	beginEditCP(pos, GeoPositions3f::GeoPropDataFieldMask);
		for (i=0; i < data->nVertices; i++)
		{
			vec = Vec3f(data->vertices[i][0], data->vertices[i][1], data->vertices[i][2]);
			pos->addValue(vec);
		} // for
	endEditCP(pos, GeoPositions3f::GeoPropDataFieldMask);

	GeoIndicesUI32Ptr indices = GeoIndicesUI32::create();
	beginEditCP(indices, GeoIndicesUI32::GeoPropDataFieldMask);
		for (i=0; i < data->nTriangles; i++)
		{
			indices->addValue(data->triangles[i].index[0]);
			indices->addValue(data->triangles[i].index[1]);
			indices->addValue(data->triangles[i].index[2]);
		} // for
	endEditCP(indices, GeoIndicesUI32::GeoPropDataFieldMask);
#endif

#if OSG_MAJOR_VERSION < 2
	beginEditCP(triMeshGeom, OSG::Geometry::TypesFieldMask |
		OSG::Geometry::LengthsFieldMask | OSG::Geometry::IndicesFieldMask |
		OSG::Geometry::PositionsFieldMask);
#endif
		triMeshGeom->setTypes(type);
		triMeshGeom->setLengths(length);
		triMeshGeom->setIndices(indices);
		triMeshGeom->setPositions(pos);
#if OSG_MAJOR_VERSION < 2
	endEditCP(triMeshGeom, OSG::Geometry::TypesFieldMask |
		OSG::Geometry::LengthsFieldMask | OSG::Geometry::IndicesFieldMask |
		OSG::Geometry::PositionsFieldMask);
#endif

	calcVertexNormals(triMeshGeom);

	beginEditCP(triMeshNode, Node::CoreFieldMask);
		triMeshNode->setCore(triMeshGeom);
	endEditCP(triMeshNode, Node::CoreFieldMask);

	return triMeshNode;
} // buildTriMesh

NodePtr OpenSGRenderer::buildCompositeGeo(CompositeGeometry* geo)
{
	int i;
	Matrix m;
	Quaternion osgQuat;
	Vec3f osgVec3f;
	std::vector<CompositeGeometryComponent*> list = geo->getGeometryList();
	Geometry* subGeom;
	NodePtr resultNode;
	NodePtr subGeoNode;
	NodePtr subGeoTransNode;
	TransformPtr subGeoTransCore;

	resultNode = Node::create();
	beginEditCP(resultNode, Node::CoreFieldMask);
		resultNode->setCore(Group::create());
	endEditCP(resultNode, Node::CoreFieldMask);

	for (i=0; i < (int)list.size(); i++)
	{
		subGeom = list[i]->geometry;
		subGeoNode = buildSceneGraph(subGeom);
		subGeoTransNode = Node::create();
		subGeoTransCore = Transform::create();
		m.setIdentity();
		convert(list[i]->offset, osgVec3f);
		m.setTranslate(osgVec3f);
		convert(list[i]->rotation, osgQuat);
		m.setRotate(osgQuat);
		beginEditCP(subGeoTransCore, Transform::MatrixFieldMask);
			subGeoTransCore->setMatrix(m);
		endEditCP(subGeoTransCore, Transform::MatrixFieldMask);
		beginEditCP(subGeoTransNode, Node::CoreFieldMask | Node::ChildrenFieldMask);
			subGeoTransNode->setCore(subGeoTransCore);
			subGeoTransNode->addChild(subGeoNode);
		endEditCP(subGeoTransNode, Node::CoreFieldMask | Node::ChildrenFieldMask);

		beginEditCP(resultNode, Node::ChildrenFieldMask);
			resultNode->addChild(subGeoTransNode);
		endEditCP(resultNode, Node::ChildrenFieldMask);
	} // for
	return resultNode;
} // buildCompositeGeo

OSG::NodePtr OpenSGRenderer::buildHeightField(HeightField* hfield)
{
	int i, j;
	float dx, dy;
	Vec3f vertex;
	HeightFieldData* data = hfield->getData();

	NodePtr heightFieldNode = Node::create();
	GeometryPtr heightFieldGeom = OSG::Geometry::create();

#if OSG_MAJOR_VERSION >= 2
	GeoUInt8PropertyRecPtr type = GeoUInt8Property::create();
	GeoUInt32PropertyRecPtr length = GeoUInt32Property::create();
	type->addValue(GL_QUADS);
	length->addValue(4*(data->resolutionX-1)*(data->resolutionY-1));
#else //OpenSG1:
	GeoPTypesPtr type = GeoPTypesUI8::create();
	beginEditCP(type, GeoPTypesUI8::GeoPropDataFieldMask);
		type->addValue(GL_QUADS);
	endEditCP(type, GeoPTypesUI8::GeoPropDataFieldMask);

	GeoPLengthsPtr length = GeoPLengthsUI32::create();
	beginEditCP(length, GeoPLengthsUI32::GeoPropDataFieldMask);
		length->addValue(4*(data->resolutionX-1)*(data->resolutionY-1));
	endEditCP(length, GeoPLengthsUI32::GeoPropDataFieldMask);
#endif

	dx = (data->sizeX+1) / data->resolutionX;
	dy = (data->sizeY+1) / data->resolutionY;

#if OSG_MAJOR_VERSION >= 2
	GeoPnt3fPropertyRecPtr pos = GeoPnt3fProperty::create();
#else //OpenSG1:
	GeoPositions3fPtr pos = GeoPositions3f::create();
	beginEditCP(pos, GeoPositions3f::GeoPropDataFieldMask);
#endif
		for (j=0; j < data->resolutionY; j++)
		{
			for (i=0; i < data->resolutionX; i++)
			{
				vertex[0] = dx*i + data->offsetX;
				vertex[2] = dy*j + data->offsetY;
				vertex[1] = data->heightValues[j*data->resolutionX+i];
				pos->addValue(vertex);
			} // for
		} // for
#if OSG_MAJOR_VERSION < 2
	endEditCP(pos, GeoPositions3f::GeoPropDataFieldMask);
#endif

#if OSG_MAJOR_VERSION >= 2
	GeoUInt32PropertyRecPtr indices = GeoUInt32Property::create();
#else
	GeoIndicesUI32Ptr indices = GeoIndicesUI32::create();
	beginEditCP(indices, GeoIndicesUI32::GeoPropDataFieldMask);
#endif
		for (j=0; j < (data->resolutionY-1); j++)
		{
			for (i=0; i < (data->resolutionX-1); i++)
			{
// Quad-indices: counter-clockwise:
//      i
//    j a--d
//      |  |
//      b--c
				indices->addValue(j*data->resolutionX+i);
				indices->addValue((j+1)*data->resolutionX+i);
				indices->addValue((j+1)*data->resolutionX+(i+1));
				indices->addValue(j*data->resolutionX+(i+1));
			} // for
		} // for
#if OSG_MAJOR_VERSION < 2
	endEditCP(indices, GeoIndicesUI32::GeoPropDataFieldMask);
#endif

#if OSG_MAJOR_VERSION < 2
	beginEditCP(heightFieldGeom, OSG::Geometry::TypesFieldMask |
		OSG::Geometry::LengthsFieldMask | OSG::Geometry::IndicesFieldMask |
		OSG::Geometry::PositionsFieldMask);
#endif

		heightFieldGeom->setTypes(type);
		heightFieldGeom->setLengths(length);
		heightFieldGeom->setIndices(indices);
		heightFieldGeom->setPositions(pos);

#if OSG_MAJOR_VERSION < 2
	endEditCP(heightFieldGeom, OSG::Geometry::TypesFieldMask |
		OSG::Geometry::LengthsFieldMask | OSG::Geometry::IndicesFieldMask |
		OSG::Geometry::PositionsFieldMask);
#endif

	calcVertexNormals(heightFieldGeom);

	beginEditCP(heightFieldNode, Node::CoreFieldMask);
		heightFieldNode->setCore(heightFieldGeom);
	endEditCP(heightFieldNode, Node::CoreFieldMask);

	return heightFieldNode;
} // buildHeightField

} // oops
