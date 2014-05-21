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

#include "oops/Geometries.h"
#include "oops/Simulation.h"
#include "oops/RigidBody.h"
#include "oops/Interfaces/TriangleMeshLoader.h"
#include "oops/OopsMath.h"
#include "oops/Interfaces/HeightFieldLoader.h"
#include <gmtl/Generate.h>
#include <gmtl/MatrixOps.h>
#include <gmtl/Xforms.h>
#include <assert.h>

// Object Oriented Physics Simulation
namespace oops
{

//*****************************************************************************

Geometry::Geometry()
{
	id = 0;
	geom = 0;
	material = materials::STEEL;
	owner = NULL;
	isPartOfComposite = false;
} // Geometry

Geometry::~Geometry()
{
	if (geom && !isPartOfComposite)
	{
// 		fprintf(stderr, "Geometry::~Geometry(): geom ID = %u, geom type = %u (should be %u)\n", *((unsigned*)&geom), (unsigned)(dGeomGetClass(geom)), (unsigned)dPlaneClass);
		dGeomDestroy(geom);
	} // if
	geom = 0;
} // Geometry

void Geometry::cloneData(Geometry *dst)
{
// TODO: Check if the id should be copied!!!
// 	result->id = this->id;
	dst->material = this->material;
} // cloneData

void Geometry::finalizeInitialization()
{
} // finalizeInitialization

RigidBody* Geometry::getOwner()
{
	return owner;
} // getOwner

dGeomID Geometry::getODEGeometry()
{
	return geom;
} // getODEGeometry

void Geometry::setMaterial(materials::MATERIAL mat)
{
	this->material = mat;
} // setMaterial

void Geometry::setId(unsigned int id)
{
	this->id = id;
} // setId

materials::MATERIAL Geometry::getMaterial()
{
	return material;
} // getMaterial

unsigned int Geometry::getId()
{
	return id;
} // getId

bool Geometry::setContactParam(dContact& contact, Geometry* geom2, bool firstGeometry)
{
	return false;
} // setContactParam

void Geometry::attachGeom(dBodyID body)
{
	dGeomSetBody(geom, body);
} // attachGeom

void Geometry::positionGeometry(gmtl::Vec3f pos, gmtl::Quatf ori)
{
	dQuaternion quat;
	dGeomSetPosition(geom, pos[0], pos[1], pos[2]);
	quat[0] = ori[3];
	quat[1] = ori[0];
	quat[2] = ori[1];
	quat[3] = ori[2];
	dGeomSetQuaternion(geom, quat);
} // positionGeometry

void Geometry::destroy()
{
	if (geom && !isPartOfComposite)
	{
		dGeomDestroy(geom);
		geom = 0;
	} // if
} // destroy

//*****************************************************************************

Box::Box(gmtl::Vec3f size)
{
	this->size = size;
} // Box

Geometry* Box::clone()
{
	Box* result = new Box(this->size);
	Geometry::cloneData(result);
	return result;
} // clone

void Box::build(dSpaceID space)
{
	geom = dCreateBox(space, size[0], size[1], size[2]);
	dGeomSetData(geom, this);

	//TODO: Document: when space == 0 then it is assumed that geometry is part of a CompositeGeometry
	if (!space)
		isPartOfComposite = true;
} // build

gmtl::Vec3f Box::getSize()
{
	return size;
} // getSize

dMass& Box::setMass(float mass, dMass& m)
{
	dMassSetBoxTotal(&m, mass, size[0], size[1], size[2]);
	return m;
} // setMass

//*****************************************************************************

Sphere::Sphere(float radius)
{
	this->radius = radius;
} // Sphere

Geometry* Sphere::clone()
{
	Sphere* result = new Sphere(this->radius);
	Geometry::cloneData(result);
	return result;
} // clone

void Sphere::build(dSpaceID space)
{
	geom = dCreateSphere(space, radius);
	dGeomSetData(geom, this);

	if (!space)
		isPartOfComposite = true;
} // build

float Sphere::getRadius()
{
	return radius;
} // getRadius

dMass& Sphere::setMass(float mass, dMass& m)
{
	dMassSetSphereTotal(&m, mass, radius);
	return m;
} // setMass

//*****************************************************************************

Plane::Plane(gmtl::Vec3f normal, float offset)
{
	this->normal = normal;
	this->offset = offset;
} // Plane

Geometry* Plane::clone()
{
	Plane* result = new Plane(this->normal, this->offset);
	Geometry::cloneData(result);
	return result;
} // clone

void Plane::build(dSpaceID space)
{
	geom = dCreatePlane(space, normal[0], normal[1], normal[2], offset);
	dGeomSetData(geom, this);
// 	fprintf(stderr, "Plane::build(): created Plane geom (ID = %u, class = %u)\n", *((unsigned*)&geom), (unsigned)dGeomGetClass(geom));

	if (!space)
		isPartOfComposite = true;
} // build

gmtl::Vec3f Plane::getNormal()
{
	return normal;
} // getNormal

float Plane::getOffset()
{
	return offset;
} // getOffset

dMass& Plane::setMass(float mass, dMass& m)
{
	assert(false);
	return m;
} // setMass

//*****************************************************************************

Cylinder::Cylinder(float length, float radius)
{
	this->radius = radius;
	this->length = length;
} // Cylinder

Geometry* Cylinder::clone()
{
	Cylinder* result = new Cylinder(this->length, this->radius);
	Geometry::cloneData(result);
	return result;
} // clone

void Cylinder::build(dSpaceID space)
{
	geom = dCreateCylinder(space, radius, length);
	dGeomSetData(geom, this);

	if (!space)
		isPartOfComposite = true;
/*
	dQuaternion quat;

// Cylinder is built in no space because it is wrapped into geomTransform
	dGeomID cylinderGeom = dCreateCylinder(0, radius, length);
// Rotate cylinder so that it matches with graphical representation
	dQFromAxisAndAngle(quat, 1, 0, 0, M_PI/2.f);
	dGeomSetQuaternion(cylinderGeom, quat);
// GeometryTransform hides rotation by 90 degrees to simulation
	geom = dCreateGeomTransform(space);
	dGeomTransformSetGeom(geom, cylinderGeom);
// Use the TransformGeo for collision instead of the cylinderGeom
	dGeomTransformSetInfo(geom, 1);
*/
} // build

float Cylinder::getRadius()
{
	return radius;
} // getRadius

float Cylinder::getLength()
{
	return length;
} // getLength

// TODO: CHECK IF DIRECTION 2 IN dMassSetCylinderTotal IS CORRECT!!!!!
dMass& Cylinder::setMass(float mass, dMass& m)
{
	dMassSetCylinderTotal(&m, mass, 2, radius, length);
	return m;
} // setMass

//*****************************************************************************

Capsule::Capsule(float length, float radius)
{
	this->radius = radius;
	this->length = length;
} // Capsule

Geometry* Capsule::clone()
{
	Capsule* result = new Capsule(this->length, this->radius);
	Geometry::cloneData(result);
	return result;
} // clone

void Capsule::build(dSpaceID space)
{
/* USE THIS CODE TO INITIALLY ROTATE CAPSULE
	dQuaternion quat;
// Cylinder is built in no space because it is wrapped into geomTransform
	dGeomID cylinderGeom = dCreateCCylinder(0, radius, length);
// Rotate cylinder so that it matches with graphical representation
	dQFromAxisAndAngle(quat, 1, 0, 0, M_PI/2.f);
	dGeomSetQuaternion(cylinderGeom, quat);
// GeometryTransform hides rotation by 90 degrees to simulation
	geom = dCreateGeomTransform(space);
	dGeomTransformSetGeom(geom, cylinderGeom);
// Use the TransformGeo for collision instead of the cylinderGeom
	dGeomTransformSetInfo(geom, 1);
*/
	geom = dCreateCCylinder(space, radius, length);
	dGeomSetData(geom, this);

	if (!space)
		isPartOfComposite = true;
} // build

float Capsule::getRadius()
{
	return radius;
} // getRadius

float Capsule::getLength()
{
	return length;
} // getLength

// TODO: CHECK IF DIRECTION 2 IN dMassSetCappedCylinderTotal IS CORRECT!!!!!
dMass& Capsule::setMass(float mass, dMass& m)
{
	dMassSetCappedCylinderTotal(&m, mass, 2, radius, length);
	return m;
} // setMass

//*****************************************************************************

CompositeGeometry::CompositeGeometry(const std::vector<CompositeGeometryComponent*> &geoms)
{
	int i;
	geometryList.clear();
	for (i=0; i < (int)geoms.size(); i++)
		geometryList.push_back(geoms[i]);

	geometryOffsetsCorrected = false;
} // CompositeGeometry

CompositeGeometry::~CompositeGeometry()
{
	for (int i=0; i < (int)geometryList.size(); i++) {
		delete geometryList[i]->geometry;
		delete geometryList[i];
	} // for
	geometryList.clear();
} // ~CompositeGeometry

Geometry* CompositeGeometry::clone()
{
	int i;
	CompositeGeometryComponent *srcEntry, *dstEntry;
	std::vector<CompositeGeometryComponent*> clonedGeometryList;

	for (i=0; i < (int)geometryList.size(); i++)
	{
		srcEntry = geometryList[i];
		dstEntry = new CompositeGeometryComponent;
		dstEntry->geometry = srcEntry->geometry->clone();
		dstEntry->offset = srcEntry->offset;
		dstEntry->rotation = srcEntry->rotation;
		dstEntry->mass = srcEntry->mass;
		clonedGeometryList.push_back(dstEntry);
	} // for

	CompositeGeometry* result = new CompositeGeometry(clonedGeometryList);
	result->geometryOffsetsCorrected = this->geometryOffsetsCorrected;
	Geometry::cloneData(result);
	return result;
} // clone

void CompositeGeometry::finalizeInitialization()
{
	if (!geometryOffsetsCorrected)
		correctGeometryOffsets();
} // setOwner

void CompositeGeometry::correctGeometryOffsets()
{
	CompositeGeometryComponent *srcEntry, *dstEntry;
	std::vector<CompositeGeometryComponent*> tempGeometryList;
	dGeomID* tempGeometryTransforms;
	dMass tempGeomMass;
	int i, numGeoms;
	dMass m;
	Geometry* subGeom;
	dGeomID subGeomID;
	dMatrix3 matrix;
	gmtl::Vec3f centerOfMassCheck;
	const float COM_THRESHOLD = 0.00001f;

	// STEP 1: clone the geometries
	numGeoms = geometryList.size();
	assert(numGeoms > 0);
	for (i=0; i < numGeoms; i++)
	{
		srcEntry = geometryList[i];
		dstEntry = new CompositeGeometryComponent;
		dstEntry->geometry = srcEntry->geometry->clone();
		dstEntry->offset = srcEntry->offset;
		dstEntry->rotation = srcEntry->rotation;
		dstEntry->mass = srcEntry->mass;
		tempGeometryList.push_back(dstEntry);
	} // for

	tempGeometryTransforms = new dGeomID[numGeoms];
	dMassSetZero(&tempGeomMass);

	// STEP 2: build the cloned geometries
	for (i=0; i < numGeoms; i++)
		tempGeometryList[i]->geometry->build(0);

	// STEP 3: create global mass for composition of all geometries
	for (i=0; i < numGeoms; i++)
	{
		subGeom = tempGeometryList[i]->geometry;

		tempGeometryTransforms[i] = dCreateGeomTransform(0);
		dGeomTransformSetCleanup(tempGeometryTransforms[i], 1);

		subGeom->setMass(tempGeometryList[i]->mass, m);

		subGeomID = subGeom->getODEGeometry();
		dGeomTransformSetGeom(tempGeometryTransforms[i], subGeomID);

		dGeomSetRotation(subGeomID, convert(tempGeometryList[i]->rotation, matrix));
		dMassRotate(&m, matrix);
		dGeomSetPosition(subGeomID, tempGeometryList[i]->offset[0], tempGeometryList[i]->offset[1], tempGeometryList[i]->offset[2]);
		dMassTranslate(&m, tempGeometryList[i]->offset[0], tempGeometryList[i]->offset[1], tempGeometryList[i]->offset[2]);

		dMassAdd(&tempGeomMass, &m);
//		dGeomSetData(tempGeometryTransforms[i], this);
	} // for

	// STEP 4: Check for center of mass
	printf("CompositeGeometry: CENTER OF MASS = %f %f %f\n", tempGeomMass.c[0], tempGeomMass.c[1], tempGeomMass.c[2]);
	centerOfMassCheck = gmtl::Vec3f(tempGeomMass.c[0], tempGeomMass.c[1], tempGeomMass.c[2]);
	if (gmtl::length(centerOfMassCheck) > COM_THRESHOLD)
	{
		fprintf(stderr, "INFO: CENTER OF MASS for Composite Geom has to be about (0,0,0)!\n");
		fprintf(stderr, "       current value = %f %f %f\n", tempGeomMass.c[0], tempGeomMass.c[1], tempGeomMass.c[2]);
		fprintf(stderr, "       fixing offsets!\n");

		// STEP 5: correct rigid body offset (to match to center of mass)
		TransformationData rigidBodyOffset = owner->getOffsetTransformation();
		rigidBodyOffset.position[0] -= tempGeomMass.c[0];
		rigidBodyOffset.position[1] -= tempGeomMass.c[1];
		rigidBodyOffset.position[2] -= tempGeomMass.c[2];
		owner->setOffsetTransformation(rigidBodyOffset);

		// STEP 5: correct subgeometry offsets (to match to center of mass)
		for (i=0; i < numGeoms; i++)
		{
			geometryList[i]->offset[0] -= tempGeomMass.c[0];
			geometryList[i]->offset[1] -= tempGeomMass.c[1];
			geometryList[i]->offset[2] -= tempGeomMass.c[2];
		} // for
	} // if

	// STEP 6: delete temporary geometries and free reserved memory
	for (int i=0; i < (int)tempGeometryList.size(); i++) {
		delete tempGeometryList[i]->geometry;
		delete tempGeometryList[i];
		dGeomDestroy(tempGeometryTransforms[i]);
		tempGeometryTransforms[i] = 0;
	} // for
	tempGeometryList.clear();
	delete[] tempGeometryTransforms;

	geometryOffsetsCorrected = true;
} // correctGeometryOffsets

void CompositeGeometry::attachGeom(dBodyID body)
{
	int i;
	for (i=0; i < (int)geometryList.size(); i++)
		dGeomSetBody(geometryTransforms[i], body);
} // attachGeom

void CompositeGeometry::positionGeometry(gmtl::Vec3f pos, gmtl::Quatf ori)
{
	int i;
	dQuaternion quat;
	quat[0] = ori[3];
	quat[1] = ori[0];
	quat[2] = ori[1];
	quat[3] = ori[2];

	for (i=0; i < (int)geometryList.size(); i++)
	{
		dGeomSetPosition(geometryTransforms[i], pos[0], pos[1], pos[2]);
		dGeomSetQuaternion(geometryTransforms[i], quat);
	} // for
} // positionGeometry

void CompositeGeometry::build(dSpaceID space)
{
	int i, numGeoms;
	dMass m;
	Geometry* subGeom;
	dGeomID subGeomID;
	dMatrix3 matrix;
	gmtl::Vec3f centerOfMassCheck;
	const float COM_THRESHOLD = 0.00001f;

	numGeoms = geometryList.size();
	assert(numGeoms > 0);
	geometryTransforms = new dGeomID[numGeoms];
	dMassSetZero(&geomMass);

	for (i=0; i < numGeoms; i++)
		geometryList[i]->geometry->build(0);

	for (i=0; i < numGeoms; i++)
	{
		subGeom = geometryList[i]->geometry;

		geometryTransforms[i] = dCreateGeomTransform(space);
		dGeomTransformSetCleanup(geometryTransforms[i], 1);

		subGeom->setMass(geometryList[i]->mass, m);

		subGeomID = subGeom->getODEGeometry();
		dGeomTransformSetGeom(geometryTransforms[i], subGeomID);

		dGeomSetRotation(subGeomID, convert(geometryList[i]->rotation, matrix));
		dMassRotate(&m, matrix);
		dGeomSetPosition(subGeomID, geometryList[i]->offset[0], geometryList[i]->offset[1], geometryList[i]->offset[2]);
		dMassTranslate(&m, geometryList[i]->offset[0], geometryList[i]->offset[1], geometryList[i]->offset[2]);

		dMassAdd(&geomMass, &m);
		dGeomSetData(geometryTransforms[i], this);
	} // for

	// move the center of mass to (0,0,0)
	printf("CompositeGeometry: CENTER OF MASS = %f %f %f\n", geomMass.c[0], geomMass.c[1], geomMass.c[2]);
	centerOfMassCheck = gmtl::Vec3f(geomMass.c[0], geomMass.c[1], geomMass.c[2]);
	if (gmtl::length(centerOfMassCheck) > COM_THRESHOLD)
	{
		// To fix center of mass:
		// add COM offset positive to obj-position, negative to obj-center
		// add COM negative to geom-positions
		fprintf(stderr, "ERROR: CENTER OF MASS for Composite Geom has to be about (0,0,0)!\n");
		fprintf(stderr, "       current value = %f %f %f\n", geomMass.c[0], geomMass.c[1], geomMass.c[2]);
		fprintf(stderr, "       INTERNAL ERROR: OFFSET SHOULD HAVE BEEN FIXED AUTOMATICALLY!\n");
		assert(false);
	} // if

	for (i=0; i < numGeoms; i++)
	{
		dGeomSetPosition(geometryList[i]->geometry->getODEGeometry(),
				geometryList[i]->offset[0] - geomMass.c[0],
				geometryList[i]->offset[1] - geomMass.c[1],
				geometryList[i]->offset[2] - geomMass.c[2]);
	} // for
	dMassTranslate(&geomMass, -geomMass.c[0], -geomMass.c[1], -geomMass.c[2]);
} // build

dMass& CompositeGeometry::setMass(float mass, dMass& m)
{
	// NOTE: THE MASS IS NOT CALCULATED HERE BUT IS A SUM OF ALL SUBMASSES!!!
	m = geomMass;
	return m;
} // setMass

std::vector<CompositeGeometryComponent*>& CompositeGeometry::getGeometryList()
{
	return geometryList;
} // getGeometryList

void CompositeGeometry::destroy()
{
	int i;
	for (i=0; i < (int)geometryList.size(); i++) {
		geometryList[i]->geometry->destroy();
		dGeomDestroy(geometryTransforms[i]);
		geometryTransforms[i] = 0;
	} // for
	delete[] geometryTransforms;

	if (geom && !isPartOfComposite) {
		dGeomDestroy(geom);
		geom = 0;
	} // if
} // destroy

//*****************************************************************************

HeightField::HeightField(HeightFieldLoader* loader, std::string fileName)
{
	loader->loadHeightField(fileName, this);

	heightfieldData = dGeomHeightfieldDataCreate();
	dGeomHeightfieldDataBuildSingle(heightfieldData, data->heightValues,
			3, data->sizeX, data->sizeY, data->resolutionX, data->resolutionY,
			data->scale, data->offset, 0.1f, 0);

//TODO:	dGeomHeightfieldDataSetBounds(...);
} // HeightField

HeightField::HeightField(HeightField* src)
{
	this->data = new HeightFieldData;
	this->data->offset = src->data->offset;
	this->data->offsetX = src->data->offsetX;
	this->data->offsetY = src->data->offsetY;
	this->data->resolutionX = src->data->resolutionX;
	this->data->resolutionY = src->data->resolutionY;
	this->data->scale = src->data->scale;
	this->data->sizeX = src->data->sizeX;
	this->data->sizeY = src->data->sizeY;
	this->data->heightValues = new float[this->data->resolutionX*this->data->resolutionY];
	memcpy(this->data->heightValues, src->data->heightValues,
			sizeof(float)*this->data->resolutionX*this->data->resolutionY);

	heightfieldData = dGeomHeightfieldDataCreate();
	dGeomHeightfieldDataBuildSingle(heightfieldData, data->heightValues,
			3, data->sizeX, data->sizeY, data->resolutionX, data->resolutionY,
			data->scale, data->offset, 0.1f, 0);
} // HeightField

Geometry* HeightField::clone()
{
	HeightField* result = new HeightField(this);
	Geometry::cloneData(result);
	return result;
} // clone

void HeightField::setData(HeightFieldData* data)
{
	this->data = data;
} // setData

HeightFieldData* HeightField::getData()
{
	return data;
} // getData

void HeightField::build(dSpaceID space)
{
	geom = dCreateHeightfield(space, heightfieldData, 1);
	dGeomSetData(geom, this);

	if (!space)
		isPartOfComposite = true;
} // build

dMass& HeightField::setMass(float mass, dMass& m)
{
	printf("WARNING: HeightField::setMass(): can not set mass for HeightField!\n");
	return m;
} // setMass


//*****************************************************************************

TriangleMesh::TriangleMesh(TriangleMeshLoader* loader,
	std::string fileName)
{
	loader->loadMesh(fileName, this);
	meshData = dGeomTriMeshDataCreate();

	dGeomTriMeshDataBuildSimple(meshData, (dReal*)data->vertices, data->nVertices,
		(int*)data->triangles, data->nTriangles*3);
//  	dGeomTriMeshDataBuildSimple1(meshData, (dReal*)data->vertices, data->nVertices,
//  		(int*)data->triangles, data->nTriangles*3, (int*)data->normals);
} // TriangleMesh

TriangleMesh::TriangleMesh(TriangleMesh* src)
{
	int i;

	this->data = new TriangleMeshData;
	this->data->nTriangles = src->data->nTriangles;
	this->data->nVertices = src->data->nVertices;
	this->data->vertices = new dVector3[data->nVertices];
	this->data->triangles = new TriMeshTriangle[data->nTriangles];
	this->data->normals = new dVector3[data->nTriangles];

	for (i=0; i < (int)data->nVertices; i++)
	{
		this->data->vertices[i][0] = src->data->vertices[i][0];
		this->data->vertices[i][1] = src->data->vertices[i][1];
		this->data->vertices[i][2] = src->data->vertices[i][2];
	} // for

	for (i=0; i < (int)data->nTriangles; i++)
	{
		this->data->triangles[i] = src->data->triangles[i];
		this->data->normals[i][0] = src->data->normals[i][0];
		this->data->normals[i][1] = src->data->normals[i][1];
		this->data->normals[i][2] = src->data->normals[i][2];
	} // for

	meshData = dGeomTriMeshDataCreate();
	dGeomTriMeshDataBuildSimple(meshData, (dReal*)data->vertices, data->nVertices,
		(int*)data->triangles, data->nTriangles*3);
} // TriangleMesh

Geometry* TriangleMesh::clone()
{
	TriangleMesh* result = new TriangleMesh(this);
	Geometry::cloneData(result);
	return result;
} // clone

void TriangleMesh::build(dSpaceID space)
{
	geom = dCreateTriMesh(space, meshData, 0, 0, 0);
	dGeomSetData(geom, this);

	if (!space)
		isPartOfComposite = true;
} // build

TriangleMesh::~TriangleMesh()
{
	delete[] data->vertices;
	delete[] data->triangles;
	delete[] data->normals;
	delete data;
} // TriangleMesh

void TriangleMesh::setData(TriangleMeshData* data)
{
	this->data = data;
} // setData

TriangleMeshData* TriangleMesh::getData()
{
	return data;
} // getData

void TriangleMesh::dumpData()
{
	int i;
	for (i=0; i < data->nVertices; i++)
	{
		printf("Point (%3i): %2.4f %2.4f %2.4f\n", i, data->vertices[i][0],
			data->vertices[i][1], data->vertices[i][2]);
	} // for
	for (i=0; i < data->nTriangles; i++)
	{
		printf("Indices (%3i): %3i %3i %3i   |   Normal: %2.4f %2.4f %2.4f\n",
			i, data->triangles[i].index[0], data->triangles[i].index[1],
			data->triangles[i].index[2], data->normals[i][0], data->normals[i][1],
			data->normals[i][2]);
	} // for
} // dumpData

dMass& TriangleMesh::setMass(float mass, dMass& m)
{
	if (geom)
	{
		// TODO: configure density
		float DENSITY = 1.f;
		dMassSetTrimesh(&m, DENSITY, geom);
		if (dMassCheck(&m))
			return m;
	} // if
// Set Mass according to Bounding Box
	dReal aabb[6];
	dGeomGetAABB(geom, aabb);
	dMassSetBoxTotal(&m, mass, aabb[1]-aabb[0], aabb[3]-aabb[2], aabb[5]-aabb[4]);
	return m;
} // setMass

void TriangleMesh::update(TransformationData trans)
{
	int i;
	gmtl::Matrix44f mat;
	dMatrix4 odeMat;
	transformationDataToMatrix(trans, mat);
	for (i=0; i < 16; i++)
		odeMat[i] = (float)mat[i/4][i%4];

	dGeomTriMeshSetLastTransform(geom, odeMat);
} // update

//*****************************************************************************

} // oops
