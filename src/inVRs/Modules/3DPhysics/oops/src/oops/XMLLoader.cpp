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

#include "oops/XMLLoader.h"

#include <memory>

#include "oops/DefaultRigidBodyFactory.h"
#include "oops/DefaultArticulatedBodyFactory.h"

#include <inVRs/SystemCore/XMLTools.h>
#include <inVRs/SystemCore/DebugOutput.h>

// disable deprecation warning for std::auto_ptr when std::unique_ptr is not available.
#ifndef HAS_CXX11_UNIQUE_PTR
// Only do this on GCC versions that support it:
# if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 402
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
#endif

namespace oops {

XMLLoader::XMLLoader() {
	rigidBodyFactoryList.push_back(new DefaultRigidBodyFactory(this));
	articulatedBodyFactoryList.push_back(new DefaultArticulatedBodyFactory(this));

	geometryFactoryList.push_back(new BoxFactory);
	geometryFactoryList.push_back(new SphereFactory);
	geometryFactoryList.push_back(new PlaneFactory);
	geometryFactoryList.push_back(new CylinderFactory);
	geometryFactoryList.push_back(new CapsuleFactory);
	geometryFactoryList.push_back(new CompositeGeometryFactory(this));
	geometryFactoryList.push_back(new HeightFieldFactory(this));
	geometryFactoryList.push_back(new TriangleMeshFactory(this));

	jointFactoryList.push_back(new HingeJointFactory);
	jointFactoryList.push_back(new BallJointFactory);
	jointFactoryList.push_back(new SliderJointFactory);
	jointFactoryList.push_back(new UniversalJointFactory);
	jointFactoryList.push_back(new Hinge2JointFactory);
	jointFactoryList.push_back(new FixedJointFactory);
	jointFactoryList.push_back(new AMotorJointFactory);

	transWriterFactory = NULL;
	rendererFactory = NULL;
	triMeshLoader = NULL;
	heightFieldLoader = NULL;

} // XMLLoader

XMLLoader::~XMLLoader() {
	unsigned i;

	for (i=0; i < rigidBodyFactoryList.size(); i++)
		delete rigidBodyFactoryList[i];
	rigidBodyFactoryList.clear();

	for (i=0; i < articulatedBodyFactoryList.size(); i++)
		delete articulatedBodyFactoryList[i];
	articulatedBodyFactoryList.clear();

	for (i=0; i < geometryFactoryList.size(); i++)
		delete geometryFactoryList[i];
	geometryFactoryList.clear();

	for (i=0; i < jointFactoryList.size(); i++)
		delete jointFactoryList[i];
	jointFactoryList.clear();
} // XMLLoader

//****************************************************
// methods to register factories for SimulationObjects
//****************************************************
void XMLLoader::registerRigidBodyFactory(RigidBodyFactory* factory) {
	rigidBodyFactoryList.push_back(factory);
} // registerRigidBodyFactory

void XMLLoader::registerGeometryFactory(GeometryFactory* factory) {
	geometryFactoryList.push_back(factory);
} // registerGeometryFactory

void XMLLoader::registerJointFactory(JointFactory* factory) {
	jointFactoryList.push_back(factory);
} // registerJointFactory

void XMLLoader::registerArticulatedBodyFactory(ArticulatedBodyFactory* factory) {
	articulatedBodyFactoryList.push_back(factory);
} // registerArticulatedBodyFactory

//**********************************************************
// methods to load Objects by registered factories from file
//**********************************************************
RigidBody* XMLLoader::loadRigidBody(std::string fileName) {
	RigidBody* result= NULL;

	// load xml document via xmlConfigLoader
	std::auto_ptr<const XmlDocument> document(XmlDocument::loadXmlDocument(fileName));
	if (!document.get()) {
		fprintf(stderr,
				"XMLLoader::loadRigidBody(): error at loading of configuration file %s!\n",
				fileName.c_str());
		return NULL;
	} // if

	result = loadRigidBody(document->getRootElement());

	return result;
} // loadRigidBody

ArticulatedBody* XMLLoader::loadArticulatedBody(std::string fileName) {
	ArticulatedBody* result= NULL;

	// load xml document via xmlConfigLoader
	std::auto_ptr<const XmlDocument> document(XmlDocument::loadXmlDocument(fileName));
	if (!document.get()) {
		fprintf(stderr,
				"XMLLoader::loadArticulatedBody(): error at loading of configuration file %s!\n",
				fileName.c_str());
		return NULL;
	} // if

	result = loadArticulatedBody(document->getRootElement());

	return result;
} // loadArticulatedBody

//************************************************************
// methods to  SimulationObjects by registered factories
//************************************************************

RigidBody* XMLLoader::loadRigidBody(const XmlElement* element) {
	std::string className;
	RigidBody* result = NULL;

	className = element->getName();
	result = loadRigidBody(className, element);

	return result;
} // loadRigidBody

ArticulatedBody* XMLLoader::loadArticulatedBody(const XmlElement* element) {
	std::string className;
	ArticulatedBody* result = NULL;

	className = element->getName();
	result = loadArticulatedBody(className, element);

	return result;
} // loadArticulatedBody

Joint* XMLLoader::loadJoint(const XmlElement* element) {
	std::string className;
	Joint* result = NULL;

	className = element->getName();
	result = loadJoint(className, element);

	return result;
} // loadJoint

Geometry* XMLLoader::loadGeometry(const XmlElement* element) {
	std::string className;
	Geometry* result = NULL;
	std::string material = "";
	const XmlElement* subElement;

	// parse material
	if (element->hasAttribute("material")) {
		material = element->getAttributeValue("material");
	} // if

	if (!element->hasSubElements()) {
		printd(ERROR,
				"XMLLoader::loadGeometry(): invalid empty <geometry> element found! Please fix your configuration!\n");
		return NULL;
	} // if
	subElement = element->getAllSubElements().at(0);

	// read name of subelement and load geometry
	className = subElement->getName();
	result = loadGeometry(className, subElement);

	// set material
	if (result && material.length() > 0) {
		result->setMaterial(materials::getMaterial(material));
	} // if

	return result;
} // loadGeometry

RigidBody* XMLLoader::loadRigidBody(std::string className, const XmlElement* element) {
	int i;
	RigidBody* result = NULL;
	for (i=0; i < (int)rigidBodyFactoryList.size(); i++) {
		result = rigidBodyFactoryList[i]->create(className, element);
		if (result)
			break;
	} // for
	return result;
} // loadRigidBody

Geometry* XMLLoader::loadGeometry(std::string className, const XmlElement* element) {
	int i;
	Geometry* result= NULL;
	for (i=0; i < (int)geometryFactoryList.size(); i++) {
		result = geometryFactoryList[i]->create(className, element);
		if (result)
			break;
	} // for
	return result;
} // loadGeometry

Joint* XMLLoader::loadJoint(std::string className, const XmlElement* element) {
	int i;
	Joint* result= NULL;
	for (i=0; i < (int)jointFactoryList.size(); i++) {
		result = jointFactoryList[i]->create(className, element);
		if (result)
			break;
	} // for
	return result;
} // loadJoint

ArticulatedBody* XMLLoader::loadArticulatedBody(std::string className, const XmlElement* element) {
	int i;
	ArticulatedBody* result= NULL;
	for (i=0; i < (int)articulatedBodyFactoryList.size(); i++) {
		result = articulatedBodyFactoryList[i]->create(className, element);
		if (result)
			break;
	} // for
	return result;
} // loadArticulatedBody

//**********************************************************************
// setter for TransformationWriter, Renderer and TriMeshLoader factories
//**********************************************************************
void XMLLoader::setTransformationWriterFactory(
		TransformationWriterFactory* transWriterFactory) {
	XMLLoader::transWriterFactory = transWriterFactory;
} // setTransformationWriterFactory

void XMLLoader::setRendererFactory(RendererFactory* rendererFactory) {
	XMLLoader::rendererFactory = rendererFactory;
} // setRendererFactory

void XMLLoader::setTriangleMeshLoader(TriangleMeshLoader* loader) {
	XMLLoader::triMeshLoader = loader;
} // setTriangleMeshLoader

void XMLLoader::setHeightFieldLoader(HeightFieldLoader* loader) {
	XMLLoader::heightFieldLoader = loader;
} // setHeightFieldLoader

//**********************************************************************
// getter for TransformationWriter, Renderer and TriMeshLoader factories
//**********************************************************************
TransformationWriterFactory* XMLLoader::getTransformationWriterFactory() {
	return transWriterFactory;
} // getTransformationWriterFactory

RendererFactory* XMLLoader::getRendererFactory() {
	return rendererFactory;
} // getRendererFactory

TriangleMeshLoader* XMLLoader::getTriangleMeshLoader() {
	return triMeshLoader;
} // getTriangleMeshLoader

HeightFieldLoader* XMLLoader::getHeightFieldLoader() {
	return heightFieldLoader;
} // getHeightFieldLoader


} // oops
