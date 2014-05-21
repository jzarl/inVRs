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

#include "CollisionMap.h"

#include <irrXML.h>

#include "CollisionLineSet.h"
#include <inVRs/SystemCore/DebugOutput.h>
#include <inVRs/SystemCore/Configuration.h>
#include <inVRs/SystemCore/XMLTools.h>

using namespace irr;
using namespace io;

CollisionMap::CollisionMap() {

} // CollisionMap

CollisionMap::CollisionMap(CollisionObject* user) {
	this->user = user;
	printd(INFO, "Found a user with type %s\n", user->getType().c_str());
} // CollisionMap

CollisionMap::~CollisionMap() {
	std::vector<CollisionObject*>::iterator it;
	for (it = objects.begin(); it != objects.end(); ++it)
		delete (*it);
	objects.clear();
} // ~CollisionMap

bool CollisionMap::loadCollisionWorld(std::string cfgFile, CollisionLineSetFactory* factory) {
	unsigned collMapId, tileType;
	float yRotation = 0;
	std::string fileType, fileName;
	TransformationData trans;
	CollisionLineSet* collLineSet;
	IrrXMLReader * xml;

	if (!fileExists(cfgFile)) {
		printd(ERROR, "CollisionMap::loadCollisionWorld(): ERROR: could not find config-file %s\n",
				cfgFile.c_str());
		return false;
	} // if

	xml = createIrrXMLReader(cfgFile.c_str());

	printd(INFO, "CollisionMap::loadCollisionWorld(): loading from file: %s\n", cfgFile.c_str());
	while (xml && xml->read()) {
		switch (xml -> getNodeType()) {
		case EXN_ELEMENT:
			if (!strcmp("collisionMapConfig", xml->getNodeName())) {
			} // if
			else if (!strcmp("collisionMaps", xml->getNodeName())) {
			} // else if
			else if (!strcmp("collisionMap", xml->getNodeName())) {
				collMapId = xml->getAttributeValueAsInt("id");
			} // else if
			else if (!strcmp("trans", xml->getNodeName())) {
				yRotation = xml->getAttributeValueAsFloat("yRotation");
			} // else if
			else if (!strcmp("file", xml->getNodeName())) {
				fileType = xml->getAttributeValue("type");
			} // else if
			else if (!strcmp("fileName", xml->getNodeName())) {
				fileName = xml->getAttributeValue("name");
			} // else if
			else if (!strcmp("mapping", xml->getNodeName())) {
			} // else if
			else if (!strcmp("tile", xml->getNodeName())) {
				tileType = xml->getAttributeValueAsInt("type");
				collMapId = xml->getAttributeValueAsInt("collisionMapId");
				tileMap[tileType] = objectMap[collMapId];
			} // else if
			break;
		case EXN_ELEMENT_END:
			if (!strcmp("collisionMap", xml->getNodeName())) {
				// TODO: also allow other collision primitives, not only lineSets
				trans = identityTransformation();
				setQuatfAsAxisAngleDeg(trans.orientation, 0, 1, 0, yRotation);
				// TODO: add extra path for CollisionMaps
				collLineSet = loadCollisionLineSet(Configuration::getPath("Tiles") + fileName,
						trans, factory);
				if (collLineSet)
					objectMap[collMapId] = collLineSet;
			} // if
			break;
		default:
			break;
		} // switch
	} // while

	delete xml;
	return true;
} // loadCollisionWorld

std::vector<CollisionData*>& CollisionMap::checkCollision(TransformationData trans, std::vector<
		CollisionData*> &dst) {
	std::vector<CollisionObject*>::iterator it;

	user->setTransformation(trans);
	for (it = objects.begin(); it != objects.end(); ++it) {
		(*it)->checkCollision(user, dst);
	} // for
	return dst;
} // checkCollision

CollisionObject* CollisionMap::getTileCollisionMap(unsigned tileId) {
	return tileMap[tileId];
} // getTileCollisionMap

CollisionLineSet* CollisionMap::loadCollisionLineSet(std::string vrmlFile, TransformationData trans, CollisionLineSetFactory* factory) {

	if (!fileExists(vrmlFile)) {
		printd(ERROR, "CollisionMap::loadCollisionLineSet(): cannot read from file %s\n",
				vrmlFile.c_str());
		return NULL;
	}

	CollisionLineSet *object = factory->create("CollisionLineSet", vrmlFile);
	object->setTransformation(trans);
	objects.push_back(object);
	return object;
} // loadCollisionLineSet
