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

#include "oops/GeometryFactories.h"
#include "oops/Simulation.h"
#include "oops/XMLLoader.h"
#include <inVRs/SystemCore/DebugOutput.h>
#include <inVRs/SystemCore/XMLTools.h>
//#include <irrXML.h>
#include <assert.h>

//using namespace irr;
//using namespace io;

namespace oops {

//*****************************************************************************

Geometry* BoxFactory::create(std::string className, const XmlElement* element) {
	if (className != "box")
		return NULL;

	gmtl::Vec3f size = gmtl::Vec3f(0, 0, 0);
	Box* result= NULL;

	if (element->hasSubElement("boxSize")) {
		size[0] = element->getAttributeValueAsFloat("boxSize.x");
		size[1] = element->getAttributeValueAsFloat("boxSize.y");
		size[2] = element->getAttributeValueAsFloat("boxSize.z");
	} // if
	else {
		printd(ERROR,
				"BoxFactory::create(): missing element <boxSize> in geometry element <box> found! Please check your configuration!\n");
		return NULL;
	} // else

	result = new Box(size);
	return result;

} // create

//*****************************************************************************

Geometry* SphereFactory::create(std::string className, const XmlElement* element) {
	if (className != "sphere")
		return NULL;

	float radius = 0;
	Sphere* result= NULL;

	if (element->hasSubElement("sphereRadius")) {
		radius = element->getAttributeValueAsFloat("sphereRadius.value");
	} // if
	else {
		printd(ERROR,
				"SphereFactory::create(): missing element <sphereRadius> in geometry element <sphere> found! Please check your configuration!\n");
		return NULL;
	} // else

	result = new Sphere(radius);
	return result;
} // create

//*****************************************************************************

Geometry* PlaneFactory::create(std::string className, const XmlElement* element) {
	if (className != "plane")
		return NULL;

	gmtl::Vec3f normal = gmtl::Vec3f(0, 0, 0);
	float offset = 0;
	Plane* result= NULL;

	if (element->hasSubElement("planeOffset")) {
		offset = element->getAttributeValueAsFloat("planeOffset.value");
	} // if
	else {
		printd(ERROR,
				"PlaneFactory::create(): missing element <planeOffset> in geometry element <plane> found! Please check your configuration!\n");
		return NULL;
	} // else

	if (element->hasSubElement("planeNormal")) {
		normal[0] = element->getAttributeValueAsFloat("planeNormal.x");
		normal[1] = element->getAttributeValueAsFloat("planeNormal.y");
		normal[2] = element->getAttributeValueAsFloat("planeNormal.z");
	} // if
	else {
		printd(ERROR,
				"PlaneFactory::create(): missing element <planeNormal> in geometry element <plane> found! Please check your configuration!\n");
		return NULL;
	} // else

	result = new Plane(normal, offset);
	return result;
} // create

//*****************************************************************************

Geometry* CylinderFactory::create(std::string className, const XmlElement* element) {
	if (className != "cylinder")
		return NULL;

	float radius = 0;
	float length = 0;
	Cylinder* result= NULL;

	if (element->hasSubElement("cylinderLength")) {
		length = element->getAttributeValueAsFloat("cylinderLength.value");
	} // if
	else {
		printd(ERROR,
				"CylinderFactory::create(): missing element <cylinderLength> in geometry element <cylinder> found! Please check your configuration!\n");
		return NULL;
	} // else

	if (element->hasSubElement("cylinderRadius")) {
		radius = element->getAttributeValueAsFloat("cylinderRadius.value");
	} // if
	else {
		printd(ERROR,
				"CylinderFactory::create(): missing element <cylinderRadius> in geometry element <cylinder> found! Please check your configuration!\n");
		return NULL;
	} // else


	result = new Cylinder(length, radius);
	return result;
} // create

//*****************************************************************************

Geometry* CapsuleFactory::create(std::string className, const XmlElement* element) {
	if (className != "capsule")
		return NULL;

	float radius = 0;
	float length = 0;
	Capsule* result= NULL;

	if (element->hasSubElement("capsuleLength")) {
		length = element->getAttributeValueAsFloat("capsuleLength.value");
	} // if
	else {
		printd(ERROR,
				"CapsuleFactory::create(): missing element <capsuleLength> in geometry element <capsule> found! Please check your configuration!\n");
		return NULL;
	} // else

	if (element->hasSubElement("capsuleRadius")) {
		radius = element->getAttributeValueAsFloat("capsuleRadius.value");
	} // if
	else {
		printd(ERROR,
				"CapsuleFactory::create(): missing element <capsuleRadius> in geometry element <capsule> found! Please check your configuration!\n");
		return NULL;
	} // else


	result = new Capsule(length, radius);
	return result;
} // create

//*****************************************************************************

CompositeGeometryFactory::CompositeGeometryFactory(XMLLoader* xmlLoader) {
	this->xmlLoader = xmlLoader;
} // CompositeGeometryFactory


Geometry* CompositeGeometryFactory::create(std::string className, const XmlElement* element) {
	if (className != "compositeGeometry")
		return NULL;

	CompositeGeometryComponent* component= NULL;
	std::vector<CompositeGeometryComponent*> geoList;
	std::vector<CompositeGeometryComponent*>::iterator geoListIt;
	std::string geometryType, material;
	TransformationData offset = identityTransformation();
	CompositeGeometry* result= NULL;

	bool success = true;

	std::vector<const XmlElement*> subGeometries = element->getSubElements("subGeometry");
	std::vector<const XmlElement*>::iterator it;
	const XmlElement* subGeometry;
	for (it = subGeometries.begin(); it != subGeometries.end(); ++it) {
		subGeometry = *it;

		// load mass value
		if (!subGeometry->hasAttribute("mass")) {
			printd(ERROR,
					"CompositeGeometryFactory::create(): missing attribute mass in element <subGeometry> found! Please fix your configuration file!\n");
			success = false;
			continue;
		} // if
		component = new CompositeGeometryComponent;
		component->mass = subGeometry->getAttributeValueAsFloat("mass");

		// read offsetToCenter (if set)
		if (subGeometry->hasSubElement("offsetToCenter")) {
			readTransformationDataFromXmlElement(offset,
					subGeometry->getSubElement("offsetToCenter"));
		} // if

		// read geometry
		if (subGeometry->hasSubElement("geometry")) {
			component->geometry = xmlLoader->loadGeometry(subGeometry->getSubElement("geometry"));
			if (!component->geometry) {
				printd(ERROR,
						"CompositeGeometryFactory::create(): Error loading sub-geometry!\n");
				success = false;
				delete component;
				continue;
			} // if
		} // if
		component->offset = offset.position;
		component->rotation = offset.orientation;
		geoList.push_back(component);
		component = NULL;
		offset = identityTransformation();
	} // for

	if (success) {
		result = new CompositeGeometry(geoList);
	} // if
	else {
		// free all registered geometries
		for (geoListIt = geoList.begin(); geoListIt != geoList.end(); ++geoListIt) {
			delete (*geoListIt)->geometry;
			delete (*geoListIt);
		} // for
	} // else

	return result;
} // create

//*****************************************************************************

HeightFieldFactory::HeightFieldFactory(XMLLoader* xmlLoader) {
	this->xmlLoader = xmlLoader;
} // HeightFieldFactory

Geometry* HeightFieldFactory::create(std::string className, const XmlElement* element) {
	if (className != "heightField")
		return NULL;

	HeightFieldLoader* heightFieldLoader = xmlLoader->getHeightFieldLoader();
	std::string fileName = "";
	HeightField* result= NULL;

	// check for HeightFieldLoader
	if (!heightFieldLoader) {
		printd(ERROR,
				"HeightFieldFactory::create(): Unable to load HeightField because no HeightFieldLoader was found! Please register HeightFieldLoader before loading the configuration!\n");
		return NULL;
	} // if

	if (element->hasAttribute("heightFieldFile.url")) {
		fileName = element->getAttributeValue("heightFieldFile.url");
		result = new HeightField(heightFieldLoader, fileName);
	} // if
	else {
		printd(ERROR,
				"HeightFieldFactory::create(): missing or invalid subelement <heightFieldFile> in element <heightField>! Please fix your configuration!\n");
		return NULL;
	} // else

	return result;
} // create 

//*****************************************************************************

TriangleMeshFactory::TriangleMeshFactory(XMLLoader* xmlLoader) {
	this->xmlLoader = xmlLoader;
} // TriangleMeshLoader

Geometry* TriangleMeshFactory::create(std::string className, const XmlElement* element) {
	if (className != "triangleMesh")
		return NULL;

	TriangleMeshLoader* triangleMeshLoader = xmlLoader->getTriangleMeshLoader();
	std::string fileName = "";
	TriangleMesh* result= NULL;

	// check for TriangleMeshLoader
	if (!triangleMeshLoader) {
		printd(ERROR,
				"TriangleMeshFactory::create(): Unable to load TriangleMesh because no TriangleMeshLoader was found! Please register TriangleMeshLoader before loading the configuration!\n");
		return NULL;
	} // if

	if (element->hasAttribute("triangleMeshFile.url")) {
		fileName = element->getAttributeValue("triangleMeshFile.url");
		result = new TriangleMesh(triangleMeshLoader, fileName);
	} // if
	else {
		printd(ERROR,
				"TriangleMeshFactory::create(): missing or invalid subelement <triangleMeshFile> in element <triangleMesh>! Please fix your configuration!\n");
		return NULL;
	} // else

	return result;
} // create

//*****************************************************************************

} // oops
