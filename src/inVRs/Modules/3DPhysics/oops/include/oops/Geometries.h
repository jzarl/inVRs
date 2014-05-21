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

#ifndef _GEOMETRIES_H
#define _GEOMETRIES_H

#include <ode/ode.h>
#include <gmtl/VecOps.h>
#include <string>
#include <vector>
#include <inVRs/SystemCore/DataTypes.h>
#include "FrictionValues.h"
#ifndef INVRS_BUILD_TIME
#include "configOops.h"
#endif

// Object Oriented Physics Simulation
namespace oops {

class RigidBody;
class HeightFieldLoader;
class TriangleMeshLoader;

//*****************************************************************************

class Geometry {
protected:
	friend class RigidBody;
	unsigned int id;
	dGeomID geom;
	materials::MATERIAL material;
	RigidBody* owner;
	bool isPartOfComposite;

	virtual void cloneData(Geometry* dst);
	virtual void finalizeInitialization();

public:
	Geometry();
	virtual ~Geometry();

	virtual Geometry* clone() = 0;

	virtual RigidBody* getOwner();
	dGeomID getODEGeometry();
	void setId(unsigned int id);
	unsigned int getId();
	void setMaterial(materials::MATERIAL mat);
	materials::MATERIAL getMaterial();
	virtual bool setContactParam(dContact& contact, Geometry* geom2,
			bool firstGeometry);

	virtual void build(dSpaceID space) = 0;
	virtual void attachGeom(dBodyID body);
	virtual void positionGeometry(gmtl::Vec3f pos, gmtl::Quatf ori);
	virtual void destroy();
	virtual dMass& setMass(float mass, dMass& m) = 0;
}; // Geometry

//*****************************************************************************

class Box : public Geometry {
protected:
	gmtl::Vec3f size;

public:
	Box(gmtl::Vec3f size);
	virtual ~Box() {};
	virtual Geometry* clone();

	gmtl::Vec3f getSize();

	virtual void build(dSpaceID space);
	virtual dMass& setMass(float mass, dMass& m);
}; // Box

//*****************************************************************************

class Sphere : public Geometry {
protected:
	float radius;

public:
	Sphere(float radius);
	virtual ~Sphere() {};
	virtual Geometry* clone();

	float getRadius();

	virtual void build(dSpaceID space);
	virtual dMass& setMass(float mass, dMass& m);
}; // Sphere

//*****************************************************************************

class Plane : public Geometry {
protected:
	gmtl::Vec3f normal;
	float offset;

public:
	Plane(gmtl::Vec3f normal, float offset);
	virtual ~Plane() {};
	virtual Geometry* clone();

	gmtl::Vec3f getNormal();
	float getOffset();

	virtual void build(dSpaceID space);
	virtual dMass& setMass(float mass, dMass& m);
}; // Plane

//*****************************************************************************

class Cylinder : public Geometry {
protected:
	float length;
	float radius;

public:
	Cylinder(float length, float radius);
	virtual ~Cylinder() {};
	virtual Geometry* clone();

	float getLength();
	float getRadius();

	virtual void build(dSpaceID space);
	virtual dMass& setMass(float mass, dMass& m);
}; // Cylinder

//*****************************************************************************

class Capsule : public Geometry {
protected:
	float length;
	float radius;

public:
	Capsule(float length, float radius);
	virtual ~Capsule() {};
	virtual Geometry* clone();

	float getLength();
	float getRadius();

	virtual void build(dSpaceID space);
	virtual dMass& setMass(float mass, dMass& m);
}; // Capsule

//*****************************************************************************

struct CompositeGeometryComponent {
	Geometry* geometry;
	gmtl::Vec3f offset;
	gmtl::Quatf rotation;
	float mass;
}; // CompositeGeometryComponent

class CompositeGeometry : public Geometry {
protected:
	std::vector<CompositeGeometryComponent*> geometryList;
	dGeomID* geometryTransforms;
	dMass geomMass;
	bool geometryOffsetsCorrected;

	// DEPRECATED:
	//	gmtl::Vec3f calculateTranslation(gmtl::Vec3f pos, gmtl::Quatf ori);
	virtual void finalizeInitialization();
	void correctGeometryOffsets();

public:
	CompositeGeometry(const std::vector<CompositeGeometryComponent*> &geoms);
	virtual ~CompositeGeometry();
	virtual Geometry* clone();

	virtual void attachGeom(dBodyID body);
	virtual void positionGeometry(gmtl::Vec3f pos, gmtl::Quatf ori);
	std::vector<CompositeGeometryComponent*>& getGeometryList();
	virtual void build(dSpaceID space);
	virtual dMass& setMass(float mass, dMass& m);

	virtual void destroy();

}; // CompositeGeometry

//*****************************************************************************

struct HeightFieldData {
	float sizeX, sizeY, scale, offset, offsetX, offsetY;
	int resolutionX, resolutionY;
	float* heightValues;
}; // HeightFieldData

class HeightField : public Geometry {
protected:
	HeightField(HeightField* src);
	HeightFieldData* data;
	dHeightfieldDataID heightfieldData;
	// 	dReal heightfield_callback( void* pUserData, int x, int z );

public:
	HeightField(HeightFieldLoader* loader, std::string fileName);
	virtual ~HeightField() {};
	virtual Geometry* clone();

	void setData(HeightFieldData* data);
	HeightFieldData* getData();

	virtual void build(dSpaceID space);
	virtual dMass& setMass(float mass, dMass& m);
}; // HeightField

//*****************************************************************************

struct TriMeshTriangle {
	int index[3];
}; // TriMeshTriangle

struct TriangleMeshData {
	int nTriangles;
	int nVertices;
	dVector3 *vertices;
	TriMeshTriangle *triangles;
	dVector3 *normals;
}; // TriangleMeshData

class TriangleMesh : public Geometry {
protected:
	// Constructor for cloning TriangleMesh
	TriangleMesh(TriangleMesh* src);

	TriangleMeshData* data;
	dTriMeshDataID meshData;

public:
	TriangleMesh(TriangleMeshLoader* loader, std::string fileName);
	virtual ~TriangleMesh();
	virtual Geometry* clone();

	void setData(TriangleMeshData* data);
	TriangleMeshData* getData();
	void dumpData();

	virtual void build(dSpaceID space);
	virtual dMass& setMass(float mass, dMass& m);
	void update(TransformationData trans);

}; // TriangleMesh

//*****************************************************************************

} // oops

#endif // _GEOMETRIES_H
