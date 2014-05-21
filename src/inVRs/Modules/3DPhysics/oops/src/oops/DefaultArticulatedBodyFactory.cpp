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

#include "oops/DefaultArticulatedBodyFactory.h"
#include <inVRs/SystemCore/DebugOutput.h>
#include <inVRs/SystemCore/XMLTools.h>
#include <gmtl/AxisAngle.h>
#include <gmtl/Generate.h>
#include <inVRs/SystemCore/DataTypes.h>
#include <assert.h>

#include "oops/Simulation.h"
#include "oops/ArticulatedBody.h"
#include "oops/Interfaces/TransformationWriterFactory.h"
#include "oops/Interfaces/RendererFactory.h"
#include "oops/Geometries.h"
#include "oops/RigidBody.h"
#include "oops/Joints.h"
#include "oops/Interfaces/TriangleMeshLoader.h"
#include "oops/XMLLoader.h"

namespace oops {

//*****************************************************************************

DefaultArticulatedBodyFactory::DefaultArticulatedBodyFactory(XMLLoader* xmlLoader) {
	this->xmlLoader = xmlLoader;
} // DefaultArticulatedBodyFactory

DefaultArticulatedBodyFactory::~DefaultArticulatedBodyFactory() {
} // ~DefaultArticulatedBodyFactory

ArticulatedBody* DefaultArticulatedBodyFactory::create(std::string className,
		const XmlElement* element) {
	if (className != "articulatedBody")
		return NULL;

//TODO
	printd(ERROR,
			"DefaultArticulatedBodyFactory::create(): NOT UPDATED TO NEW XML CONFIG FILE LOADING YET!\n");
	return NULL;

/*
	ArticulatedBody* result;
	IrrXMLReader* xml = (IrrXMLReader*)args;

	result = loadArticulatedBody(xml);
	return result;
*/
} // create
/*
ArticulatedBody* DefaultArticulatedBodyFactory::loadArticulatedBody(IrrXMLReader* xml) {
	std::string name;
	ArticulatedBody* result = new ArticulatedBody;
	//TransformationData transformation = identityTransformation();
	bool finished = false;
	const std::string configVersion ="1.0";
	std::string version = configVersion;

	if (xml && !strcmp("articulatedBody", xml->getNodeName()) && xml->getAttributeValue("version")) {
		version = xml->getAttributeValue("version");
		if (version != configVersion)
			printd(WARNING, "ArticulatedBodyFactory::loadArticulatedBody(): unexpected config file version! Expected %s, found %s!\n", configVersion.c_str(), version.c_str());
	} // if


	while (!finished && xml && xml->read()) {
		switch (xml -> getNodeType()) {
		case EXN_ELEMENT:
			if (!strcmp("name", xml->getNodeName()))
				result->name = xml->getAttributeValue("value");
			// DEPRECATED: Transformation is set externally like done with RigidBodies!!!
			//else if (!strcmp("transformation", xml->getNodeName()))
			//	readTransformationDataFromXML(transformation, xml,
			//	"transformation");
			else if (!strcmp("subBodies", xml->getNodeName()))
				parseSubBodies(xml, result);
			else if (!strcmp("joints", xml->getNodeName()))
				parseJoints(xml, result);
			else if (!strcmp("collisionRules", xml->getNodeName()))
				parseCollisionRules(xml, result);
			else
				printd(WARNING, "ArticulatedBodyFactory::loadArticulatedBody(): WARNING: unknown element %s found!\n", xml->getNodeName());
			break;

		case EXN_ELEMENT_END:
			if (!strcmp("articulatedBody", xml->getNodeName())) {
				//result->setTransformation(transformation);
				finished = true;
			} // if
			else
				printd(WARNING, "ArticulatedBodyFactory::loadArticulatedBody(): WARNING: unknown end-element %s found!\n", xml->getNodeName());
			break;

		default:
			break;
		} // switch

	} // while

	if (!finished)
		printd(ERROR, "ArticulatedBodyFactory::loadArticulatedBody(): ERROR while loading of ArticulatedBody with name %s!\n", result->name.c_str());

	return result;
} // loadArticulatedBody


bool DefaultArticulatedBodyFactory::parseSubBodies(IrrXMLReader* xml, ArticulatedBody* artBody) {
	bool finished = false;
	RigidBody* rigidBody;
	std::string objectName, finalName, mainBodyString;
	bool fixed = false;
	bool mainBody = false;
	int id = -1;
	TransformationData objectOffset = identityTransformation();
	// 	TransformationData subResult;

	while (!finished && xml->read()) {
		switch (xml -> getNodeType()) {
		case EXN_ELEMENT:
			if (!strcmp("subBody", xml->getNodeName())) {
				id = xml->getAttributeValueAsInt("id");
				if (xml->getAttributeValue("mainBody")) {
					mainBodyString = xml->getAttributeValue("mainBody");
					convert(mainBodyString, mainBody);
				} // if
			} // if
			else if (!strcmp("offsetToObjectCenter", xml->getNodeName()))
				readTransformationDataFromXML(objectOffset, xml, "offsetToObjectCenter");
			else if (!strcmp("rigidBodyDeclaration", xml->getNodeName())) {
				rigidBody = xmlLoader->loadRigidBody(xml);
				rigidBody->setID(id);
			} // if
			else
				printd(WARNING, "ArticulatedBodyFactory::parseObjectsGlobal(): WARNING: unknown element %s found!\n", xml->getNodeName());
			break;

		case EXN_ELEMENT_END:
			if (!strcmp("subBody", xml->getNodeName())) {
				finalName = artBody->name + "." + rigidBody->getName();
				rigidBody->setName(finalName);

//				 multiply(subResult, artBody->transformation, objectOffset);
//				 object->setTransformation(subResult);

				// 	Correct RigidBody-Transformation is built by ArticulatedBody::setTransformation
				rigidBody->setTransformation(objectOffset);
				artBody->rigidBodyList.push_back(rigidBody);
				artBody->rigidBodyMap[id] = rigidBody;
				if (mainBody) {
					artBody->mainBody = rigidBody;
					invert(objectOffset);
					artBody->invMainBodyOffset = objectOffset;
				} // if

				id = -1;
				objectName = "";
				fixed = false;
				rigidBody = NULL;
				objectOffset = identityTransformation();
				mainBody = false;
				mainBodyString = "";
			} // if
			else if (!strcmp("subBodies", xml->getNodeName()))
				finished = true;
			else if (!strcmp("rigidBodyDeclaration", xml->getNodeName())) {
			// Nothing to do here
			} // else if
			else
				printd(WARNING, "ArticulatedBodyFactory::parseObjectsGlobal(): WARNING: unknown end-element %s found!\n", xml->getNodeName());
			break;

		default:
			break;
		} // switch
	} // while

	if (!finished)
		printd(ERROR, "ArticulatedBodyFactory::parseObjectsGlobal(): ERROR while loading of Element <objects>!\n");

	return finished;
} // parseObjectsGlobal

bool DefaultArticulatedBodyFactory::parseJoints(IrrXMLReader* xml, ArticulatedBody* artBody) {
	bool finished = false;
	std::string jointName, finalName;
	unsigned id;
	unsigned object1ID, object2ID;
	RigidBody *object1, *object2;
	Joint* joint= NULL;
	const std::string configVersion = "1.0.1";
	std::string version = configVersion;

	while (!finished && xml->read()) {
		switch (xml -> getNodeType()) {
		case EXN_ELEMENT:
			if (!strcmp("joint", xml->getNodeName())) {
				if (xml->getAttributeValue("version")) {
					version = xml->getAttributeValue("version");
					if (version.compare(configVersion) > 0)
						printd(WARNING, "ArticulatedBodyFactory::parseJoints(): unexpected Joint-config version! Expected %s, found %s!\n", configVersion.c_str(), version.c_str());
				} // if
			} // if
			else if (!strcmp("jointProperties", xml->getNodeName())) {
				id = xml->getAttributeValueAsInt("jointId");
				jointName = xml->getAttributeValue("name");
				object1ID = xml->getAttributeValueAsInt("object1Id");
				object2ID = xml->getAttributeValueAsInt("object2Id");
				object1 = artBody->rigidBodyMap[object1ID];
				object2 = artBody->rigidBodyMap[object2ID];
				assert(object1);
				assert(object2);
				finalName = artBody->name + "." + jointName;
				// ASSUMING THAT NOW THE JOINT-ELEMENT HAS TO BE PARSED!!!
				joint = xmlLoader->loadJoint(xml);
				if (!joint) {
					printd(ERROR, "ArticulatedBodyFactory::parseJointsGlobal(): ERROR: could not build joint with name %s\n", finalName.c_str());
					break;
				} // if
				joint->setID(id);
				joint->setName(finalName);
				joint->setObject1(object1);
				joint->setObject2(object2);
				artBody->jointList.push_back(joint);
				artBody->jointMap[id] = joint;
			} // if
			else
				printd(WARNING, "ArticulatedBodyFactory::parseJointsGlobal(): WARNING: unknown element %s found!\n", xml->getNodeName());

			break;

		case EXN_ELEMENT_END:
			if (!strcmp("joints", xml->getNodeName()))
				finished = true;
			else if (!strcmp("joint", xml->getNodeName())) {
				id = 0;
				object1ID = 0;
				object2ID = 0;
				joint = NULL;
				object1 = NULL;
				object2 = NULL;
				jointName = "";
			} // else if
			else
				printd(WARNING, "ArticulatedBodyFactory::parseJointsGlobal(): WARNING: unknown end-element %s found!\n", xml->getNodeName());
			break;

		default:
			break;
		} // switch
	} // while

	if (!finished)
		printd(ERROR, "ArticulatedBodyFactory::parseJointsGlobal(): ERROR while loading of Element <joints>!\n");

	return finished;
} // parseJointsGlobal

bool DefaultArticulatedBodyFactory::parseCollisionRules(IrrXMLReader* xml, ArticulatedBody* artBody) {
	bool finished = false;
	unsigned id1, id2;
	RigidBody *object1, *object2;

	while (!finished && xml->read()) {
		switch (xml -> getNodeType()) {
		case EXN_ELEMENT:
			if (!strcmp("ignoreCollision", xml->getNodeName())) {
				id1 = xml->getAttributeValueAsInt("object1Id");
				id2 = xml->getAttributeValueAsInt("object2Id");
				object1 = artBody->rigidBodyMap[id1];
				object2 = artBody->rigidBodyMap[id2];
				assert(object1 && object2);
				object1->dontCollideWithRigidBody(object2);
			} // if
			else if (!strcmp("doCollision", xml->getNodeName())) {
				id1 = xml->getAttributeValueAsInt("object1Id");
				id2 = xml->getAttributeValueAsInt("object2Id");
				object1 = artBody->rigidBodyMap[id1];
				object2 = artBody->rigidBodyMap[id2];
				assert(object1 && object2);
				object1->collideWithRigidBody(object2);
			} // else if
			else
				printd(WARNING, "ArticulatedBodyFactory::parseCollisionRules(): WARNING: unknown element %s found!\n", xml->getNodeName());

			break;

		case EXN_ELEMENT_END:
			if (!strcmp("collisionRules", xml->getNodeName()))
				finished = true;
			else
				printd(WARNING, "ArticulatedBodyFactory::parseCollisionRules(): WARNING: unknown end-element %s found!\n", xml->getNodeName());
			break;

		default:
			break;
		} // switch
	} // while

	if (!finished)
		printd(ERROR, "ArticulatedBodyFactory::parseCollisionRules(): ERROR while loading of Element <collisionRules>!\n");

	return finished;
} // parseCollisionRules
*/
} // oops
