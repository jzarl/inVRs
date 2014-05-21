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

#include "ArticulatedPhysicsEntityTypeFactory.h"

#include <assert.h>

#ifdef INVRS_BUILD_TIME
#include "oops/Simulation.h"
#else
#include <inVRs/Modules/3DPhysics/oops/Simulation.h>
#endif
#include <inVRs/SystemCore/DebugOutput.h>
#include <inVRs/SystemCore/Platform.h>

using namespace oops;

ArticulatedPhysicsEntityTypeFactory::ArticulatedPhysicsEntityTypeFactory(
		oops::XMLLoader* xmlLoader) {
	this->xmlLoader = xmlLoader;
} // ArticulatedPhysicsEntityTypeFactory

EntityType* ArticulatedPhysicsEntityTypeFactory::create(std::string className,
		void* args) {
	if (className == "ArticulatedPhysicsEntityType")
		return new ArticulatedPhysicsEntityType((uint64_t)args);

	return NULL;
} // create

/*
bool ArticulatedPhysicsEntityTypeFactory::parseXML(EntityType* entType,
		IrrXMLReader* xml) {
	assert(xml != NULL);
	assert(entType != NULL);

	if ((xml->getNodeType() != EXN_ELEMENT) || strcmp("implementationClass",
			xml->getNodeName())) {
		printd(
				ERROR,
				"ArticulatedPhysicsEntityTypeFactory::parseXML(): expecting an element type with name implementationClass (name is %s)\n",
				xml->getNodeName());
		return false;
	} // if

	ArticulatedPhysicsEntityType* entityType = 
		(ArticulatedPhysicsEntityType*)entType;
	bool finished = false;
	ArticulatedBody* articulatedBody = NULL;
	std::string type;
	unsigned rigidBodyId;
	unsigned succeedingEntity;
	const std::string configVersion = "1.0";
	std::string version = configVersion;

	while (!finished && xml && xml->read()) {
		switch (xml->getNodeType()) {
		case EXN_ELEMENT:
			if (!strcmp("articulatedPhysicsEntityType", xml->getNodeName())) {
				if (xml->getAttributeValue("version")) {
					version = xml->getAttributeValue("version");
					if (version != configVersion)
						printd(WARNING, "ArticulatedPhysicsEntityTypeFactory::parseXML(): unexpected config file version! Expected %s, found %s!\n", configVersion.c_str(), version.c_str());
					
				} // if
				articulatedBody = xmlLoader->loadArticulatedBody(xml);
				assert(articulatedBody);
				entityType->articulatedBody = articulatedBody;
			} // if
			else if (!strcmp("rigidBodyMapping", xml->getNodeName())) {
				rigidBodyId = xml->getAttributeValueAsInt("subBodyId");
				succeedingEntity
						= xml->getAttributeValueAsInt("succeedingEntity");
				entityType->setRigidBodyMapping(rigidBodyId, succeedingEntity);
			} // else if
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
} // parseXML
*/
