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

#ifndef _OPENSGRENDERER_H
#define _OPENSGRENDERER_H

#include "Interfaces/RendererInterface.h"
#include <OpenSG/OSGTransform.h>
#include <OpenSG/OSGNode.h>
#include <OpenSG/OSGSimpleMaterial.h>
#include "Geometries.h"
#ifndef INVRS_BUILD_TIME
#include "configOops.h"
#endif

// Object Oriented Physics Simulation
namespace oops
{

class RigidBody;

class OpenSGRenderer : public RendererInterface
{
protected:
	RigidBody* object;
	OSG::NodePtr rootNode;
	OSG::NodePtr renderNode;
	OSG::NodePtr matGroupNode;
	OSG::TransformPtr renderCore;
	OSG::SimpleMaterialPtr material;
	bool isVisible;
	bool changeVisibility; 
	bool objectActive;

	OSG::NodePtr buildSceneGraph(Geometry* geom);
	OSG::NodePtr buildSphere(Sphere* sphere);
	OSG::NodePtr buildBox(Box* box);
	OSG::NodePtr buildCapsule(Capsule* ccylinder);
	OSG::NodePtr buildTriMesh(TriangleMeshData* data);
	OSG::NodePtr buildCompositeGeo(CompositeGeometry* geo);
	OSG::NodePtr buildCylinder(Cylinder* cylinder);
	OSG::NodePtr buildHeightField(HeightField* hfield);

/*	void buildTriMeshInternal(TriangleMeshData* data);*/
	void ccylinderLoop(TriangleMeshData* data, int index, 
		float capRadius, float posY);

public:
	OpenSGRenderer(OSG::NodePtr root);
	virtual ~OpenSGRenderer();
	virtual RendererInterface* clone();

	virtual void setObject(RigidBody* object);
	virtual void render();
	virtual void setVisible(bool visible);
}; // OpenSGRenderer

} // oops

#endif //_OPENSGRENDERER_H
