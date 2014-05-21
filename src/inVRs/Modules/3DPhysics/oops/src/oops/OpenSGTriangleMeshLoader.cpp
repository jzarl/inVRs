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

#include "oops/OpenSGTriangleMeshLoader.h"
#include <assert.h>
#include <OpenSG/OSGSceneFileHandler.h>
#include <OpenSG/OSGGeoFunctions.h>

#include <OpenSG/OSGTriangleIterator.h>
#if OSG_MAJOR_VERSION >= 2
#include <OpenSG/OSGGeoPnt3fProperty.h>
#endif

#include "oops/Geometries.h"

OSG_USING_NAMESPACE

// Object Oriented Physics Simulation
namespace oops
{

OpenSGTriangleMeshLoader::~OpenSGTriangleMeshLoader()
{

} // ~OpenSGTriangleMeshLoader

void OpenSGTriangleMeshLoader::loadMesh(std::string fileName, TriangleMesh* object)
{
	int i, j;
	int nPositions, nTriangles, indices[3];
	Vec3f point, normal, debugNormalError;
	NodePtr obj, geometryNode;
	GeometryPtr geometry;
#if OSG_MAJOR_VERSION >= 2
	GeoPnt3fPropertyRecPtr pos;
#else
	GeoPositions3fPtr pos;
#endif
	TriangleIterator it;
	TriangleMeshData *data = new TriangleMeshData();
//	dVector3 *tempVec;

	printf("OpenSGTriangleMeshLoader::loadMesh(): Trying to load file %s!\n", fileName.c_str());
#if OSG_MAJOR_VERSION >= 2
	obj = OSG::SceneFileHandler::the()->read(fileName.c_str());
#else
	obj = SceneFileHandler::the().read(fileName.c_str());
#endif
	assert(obj != NullFC);
	
	geometryNode = getGeometryNode(obj);
	assert(geometryNode != NullFC);
	geometry = GeometryPtr::dcast(geometryNode->getCore());

// TODO: Check if this two methods work correctly	
// 	createSharedIndex(geometry);
// 	createVertexNormals(geometry);

#if OSG_MAJOR_VERSION >= 2
	pos = dynamic_cast<GeoPnt3fProperty *>(geometry->getPositions());
#else
	pos = GeoPositions3fPtr::dcast(geometry->getPositions());
#endif
#if OPENSG_GEOPROPERTY_HAVE_SIZE_FUNCTION
	nPositions = pos->size();
#else
	nPositions = pos->getSize();
#endif
	data->nVertices = nPositions;
	data->vertices = new dVector3[nPositions];

	for (i=0; i < nPositions; i++)
	{
		point = (Vec3f)pos->getValue(i);
		data->vertices[i][0] = point[0];
		data->vertices[i][1] = point[1];
		data->vertices[i][2] = point[2];
// 		printf("Point (%3i): %2.4f %2.4f %2.4f\n", i, point[0], point[1], point[2]);
 	} // for

	it = TriangleIterator(geometryNode);

// TODO: Find a better way to do this *g*
	nTriangles = 0;
	for(it.seek(0); !it.isAtEnd(); ++it)
	{
		nTriangles++;
	} // for
	data->nTriangles = nTriangles;
	data->triangles = new TriMeshTriangle[nTriangles];
	data->normals = new dVector3[nTriangles];

// 	printf("\nNumber of TRIs = %i\n\n", nTriangles);
	
	j=0;
	for(it.seek(0); !it.isAtEnd(); ++it)
	{
		for (i=0; i < 3; i++)
		{
			indices[i] = it.getPositionIndex(i);
 			data->triangles[j].index[i] = indices[i];
		} // for
		normal = it.getNormal(0);
		data->normals[j][0] = normal[0];
		data->normals[j][1] = normal[1];
		data->normals[j][2] = normal[2];
// 		printf("Indices (%3i): %3i %3i %3i   |   Normal: %2.4f %2.4f %2.4f\n", j, indices[0], indices[1], indices[2],
// 			normal[0], normal[1], normal[2]);
		j++;

		debugNormalError = (it.getNormal(0) + it.getNormal(1) + it.getNormal(2));
		debugNormalError.normalize();
		debugNormalError *= -1;
		debugNormalError += normal;
		if (fabs(debugNormalError.length()) > 0.00001)
		{ 
// 			printf("Normal-Error = %f\n", debugNormalError.length());
// 			assert(false);
		} // if
	} // for

	object->setData(data);

} // loadMesh

NodePtr OpenSGTriangleMeshLoader::getGeometryNode(NodePtr n)
{
	NodeCorePtr core;
	NodePtr result;
	int i;
	int nChildren = n->getNChildren();
	
	core = n->getCore();
	if(!strcmp(core->getTypeName(), "Geometry\0"))
		return n;

	for (i=0; i < nChildren; i++)
	{
		result = getGeometryNode(n->getChild(i));
		if (result != NullFC)
			return result;
	} // for
	return NullFC;
} // getGeometryCore

} // oops
