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

#include "SimplePhysicsEntityTypeFactory.h"

#include <assert.h>

#ifdef INVRS_BUILD_TIME
#include "oops/Simulation.h"
#else
#include <inVRs/Modules/3DPhysics/oops/Simulation.h>
#endif
#include <inVRs/SystemCore/DebugOutput.h>
#include <inVRs/SystemCore/Platform.h>

using namespace oops;

SimplePhysicsEntityTypeFactory::SimplePhysicsEntityTypeFactory(oops::XMLLoader* xmlLoader) {
	this->xmlLoader = xmlLoader;
} // SimplePhysicsEntityTypeFactory

//**************************************************//
// PUBLIC METHODS INHERITED FROM: EntityTypeFactory //
//**************************************************//

EntityType* SimplePhysicsEntityTypeFactory::create(std::string className,
		unsigned short id) {
	if (className == "SimplePhysicsEntityType")
		return new SimplePhysicsEntityType(id);

	return NULL;
} // create

bool SimplePhysicsEntityTypeFactory::parseXML(EntityType* entType, const XmlElement* element) {
	assert(element != NULL);

	SimplePhysicsEntityType* entityType = dynamic_cast<SimplePhysicsEntityType*>(entType);
	if (!entityType) {
		printd(ERROR,
				"SimplePhysicsEntityTypeFactory::parseXML(): INTERNAL ERROR: invalid EntityType passed!\n");
		return false;
	} // if

	if (!element->hasSubElement("simplePhysicsEntityType.rigidBody")) {
		printd(ERROR,
				"SimplePhysicsEntityTypeFactory::parseXML(): missing elements <simplePhysicsEntityType> and/or <rigidBody> in <implementationClass>! Please fix your entityType configuration!\n");
		return false;
	} // if
	const XmlElement* subElement = element->getSubElement("simplePhysicsEntityType.rigidBody");

	RigidBody* rigidBody = xmlLoader->loadRigidBody(subElement);
	if (!rigidBody) {
		printd(ERROR,
				"SimplePhysicsEntityTypeFactory::parseXML(): Error at parsing RigidBody! Please fix your EntityType configuration file!\n");
		return false;
	} // if
	entityType->rigidBody = rigidBody;

	return true;
/*
	if ((xml->getNodeType() != EXN_ELEMENT)	|| strcmp("implementationClass", xml->getNodeName())) {
		printd(ERROR, "SimplePhysicsEntityTypeFactory::parseXML(): expecting an element type with name implementationClass (name is %s)\n", xml->getNodeName());
		return false;
	} // if

	SimplePhysicsEntityType* entityType = (SimplePhysicsEntityType*)entType;
	bool finished = false;
	RigidBody* rigidBody= NULL;
	std::string type;
	const std::string configVersion = "1.0";
	std::string version = configVersion;

	while (!finished && xml && xml->read()) {
		switch (xml->getNodeType()) {
			case EXN_ELEMENT:
				if (!strcmp("simplePhysicsEntityType", xml->getNodeName())) {
					if (xml->getAttributeValue("version")) {
						version = xml->getAttributeValue("version");
						if (version != configVersion)
							printd(WARNING, "SimplePhysicsEntityTypeFactory::parseXML(): unexpected config file version! Expected %s, found %s!\n", configVersion.c_str(), version.c_str());

					} // if
					rigidBody = xmlLoader->loadRigidBody(xml);
					assert(rigidBody);
					entityType->rigidBody = rigidBody;
				} // if
				break;
			case EXN_ELEMENT_END:
				if (!strcmp("implementationClass", xml->getNodeName()))
					finished = true;
				break;
			default:
				break;
		} // switch
	} // while

	return true;
*/
} // parseXML
