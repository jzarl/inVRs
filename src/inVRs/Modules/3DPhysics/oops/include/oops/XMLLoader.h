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

#ifndef _XMLLOADER_H
#define _XMLLOADER_H

#include "Interfaces/RigidBodyFactory.h"
#include "JointFactories.h"
#include "GeometryFactories.h"
#include "Interfaces/ArticulatedBodyFactory.h"
#include "Interfaces/TransformationWriterFactory.h"
#include "Interfaces/RendererFactory.h"
#include "Interfaces/TriangleMeshLoader.h"
#include "Interfaces/HeightFieldLoader.h"
#include "RigidBody.h"
#include "ArticulatedBody.h"

#include <inVRs/SystemCore/XmlElement.h>
//#include <irrXML.h>

//using namespace irr;
//using namespace io;

namespace oops {

class XMLLoader {
public:
	XMLLoader();
	virtual ~XMLLoader();

	// methods to register factories for SimulationObjects
	void registerRigidBodyFactory(RigidBodyFactory* factory);
	void registerGeometryFactory(GeometryFactory* factory);
	void registerJointFactory(JointFactory* factory);
	void registerArticulatedBodyFactory(ArticulatedBodyFactory* factory);

	// methods to load Objects by registered factories from file
	RigidBody* loadRigidBody(std::string fileName);
	ArticulatedBody* loadArticulatedBody(std::string fileName);

//	// methods to load Objects by registered factories from xml-stream
//	RigidBody* loadRigidBody(IrrXMLReader* xml);
//	ArticulatedBody* loadArticulatedBody(IrrXMLReader* xml);
//	Joint* loadJoint(IrrXMLReader* xml);
//	Geometry* loadGeometry(IrrXMLReader* xml);

	// methods to load Objects by registered factories from xml-element
	RigidBody* loadRigidBody(const XmlElement* element);
	ArticulatedBody* loadArticulatedBody(const XmlElement* element);
	Joint* loadJoint(const XmlElement* element);
	Geometry* loadGeometry(const XmlElement* element);

	// setter for TransformationWriter, Renderer and TriMeshLoader factories
	void setTransformationWriterFactory(
			TransformationWriterFactory* transWriterFactory);
	void setRendererFactory(RendererFactory* rendererFactory);
	void setTriangleMeshLoader(TriangleMeshLoader* loader);
	void setHeightFieldLoader(HeightFieldLoader* loader);

	// getter for TransformationWriter, Renderer and TriMeshLoader factories
	TransformationWriterFactory* getTransformationWriterFactory();
	RendererFactory* getRendererFactory();
	TriangleMeshLoader* getTriangleMeshLoader();
	HeightFieldLoader* getHeightFieldLoader();

protected:

//	RigidBody* loadRigidBody(std::string className, IrrXMLReader* xml);
//	Geometry* loadGeometry(std::string className, IrrXMLReader* xml);
//	Joint* loadJoint(std::string className, IrrXMLReader* xml);
//	ArticulatedBody* loadArticulatedBody(std::string className,
//			IrrXMLReader* xml);

	RigidBody* loadRigidBody(std::string className, const XmlElement* element);
	Geometry* loadGeometry(std::string className, const XmlElement* element);
	Joint* loadJoint(std::string className, const XmlElement* element);
	ArticulatedBody* loadArticulatedBody(std::string className,
			const XmlElement* element);

	// factories to build SimulationObjects
	std::vector<RigidBodyFactory*> rigidBodyFactoryList;
	std::vector<JointFactory*> jointFactoryList;
	std::vector<GeometryFactory*> geometryFactoryList;
	std::vector<ArticulatedBodyFactory*> articulatedBodyFactoryList;

	// factories to build TransformationWriter, Renderer or TriMeshLoader
	TransformationWriterFactory* transWriterFactory;
	RendererFactory* rendererFactory;
	TriangleMeshLoader* triMeshLoader;
	HeightFieldLoader* heightFieldLoader;

}; // XMLLoader

} // oops

#endif // _XMLLOADER_H
