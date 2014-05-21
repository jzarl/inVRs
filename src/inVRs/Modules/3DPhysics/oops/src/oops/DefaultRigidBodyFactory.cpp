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

#include "oops/DefaultRigidBodyFactory.h"
#include <inVRs/SystemCore/DebugOutput.h>
#include <inVRs/SystemCore/XMLTools.h>
#include <gmtl/AxisAngle.h>
#include <assert.h>
//#include <irrXML.h>

#include "oops/Interfaces/TransformationWriterFactory.h"
#include "oops/Interfaces/RendererFactory.h"
#include "oops/Simulation.h"
#include "oops/XMLLoader.h"

//using namespace irr;
//using namespace io;

namespace oops {

//*****************************************************************************

DefaultRigidBodyFactory::DefaultRigidBodyFactory(XMLLoader* xmlLoader) {
	this->xmlLoader = xmlLoader;
} // DefaultRigidBodyFactory

DefaultRigidBodyFactory::~DefaultRigidBodyFactory() {

} // ~DefaultRigidBodyFactory

RigidBody* DefaultRigidBodyFactory::create(std::string className, const XmlElement* element) {
	if (className != "rigidBody")
		return NULL;

	TransformationWriterFactory* transformationFactory =
			xmlLoader->getTransformationWriterFactory();
	RendererFactory* rendererFactory = xmlLoader->getRendererFactory();
	RigidBody* object= NULL;
	Geometry* geometry= NULL;
	TransformationWriterInterface* transformWriter= NULL;
	RendererInterface* renderer= NULL;
	std::string objectName, geometryType, material, finalName;
	bool fixed = false;
	bool visible = false;
	bool gravity = true;
	float mass = 1;
	TransformationData offset = identityTransformation();
	gmtl::Vec3f normal(0, 0, 0);
	gmtl::AxisAnglef axisAngle;
	material = "";
	const XmlElement* subElement;

	if (element->hasAttribute("name.value")) {
		objectName = element->getAttributeValue("name.value");
	} // if
	else {
		printd(ERROR,
				"DefaultRigidBodyFactory::create(): missing or invalid element <name> in element <rigidBody> found! Please fix your configuration file!\n");
		return NULL;
	} // else

	if (element->hasAttribute("fixed.value")) {
		fixed = element->getAttributeValueAsInt("fixed.value");
	} // if

	if (element->hasAttribute("mass.value")) {
		mass = element->getAttributeValueAsFloat("mass.value");
	} // if
	else if (!fixed) {
		printd(WARNING,
				"DefaultRigidBodyFactory::create(): missing or invalid element <mass> in element <rigidBody> found! Using default mass of 1!\n");
	} // else

	if (element->hasAttribute("influencedByGravity.value")) {
		gravity = element->getAttributeValueAsInt("influencedByGravity.value");
	} // if

	if (element->hasSubElement("offsetToDestination")) {
		subElement = element->getSubElement("offsetToDestination");
		readTransformationDataFromXmlElement(offset, subElement);
	} // if

	if (element->hasSubElement("geometry")) {
		geometry = xmlLoader->loadGeometry(element->getSubElement("geometry"));
		if (!geometry) {
			printd(ERROR,
					"DefaultRigidBodyFactory::create(): error at loading geometry! Please check your configuration!\n");
			return NULL;
		} // if
	} // if
	else {
		printd(ERROR,
				"DefaultRigidBodyFactory::create(): missing or invalid element <geometry> in element <rigidBody> found! Please fix your configuration file!\n");
		return NULL;
	} // else

	if (element->hasSubElement("transformationWriter")) {
		if (transformationFactory) {
			transformWriter = transformationFactory->getTransformationWriter(
							element->getAttributeValue("transformationWriter.target"));
		} // if
		else {
			printd(WARNING,
					"DefaultRigidBodyFactory::create(): No transformationWriter factory registered! <transformationWriter> element in <rigidBody> will be ignored!\n");
		} // else
	} // if

	if (element->hasSubElement("renderer")) {
		renderer = rendererFactory->getRenderer();
		visible = element->getAttributeValueAsInt("renderer.visible");
		renderer->setVisible(visible);
	} // if

	object = new RigidBody(geometry, transformWriter, renderer);
	object->setName(objectName);
	object->setMass(mass);
	object->setFixed(fixed);
	object->setOffsetTransformation(offset);
	object->setGravityMode(gravity);
	object->finalizeIntialization();

	return object;
} // create

//*****************************************************************************

} // oops
