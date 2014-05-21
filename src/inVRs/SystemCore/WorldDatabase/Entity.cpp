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

#include "Entity.h"

#include <assert.h>

#include "WorldDatabase.h"
//#include "../SystemCore.h"
#include "../../OutputInterface/OutputInterface.h"
#include "../DebugOutput.h"

Entity::Entity(unsigned short id, unsigned short environmentId, unsigned short instanceId,
		EntityType *type) {
	unsigned short typeId;

	assert(type);
	this->type = type;
	typeId = type->getId();

	this->environmentBasedId = join(environmentId, id);
	this->typeBasedId = join(typeId, instanceId);

	fixed = type->fixed;

	// environmentBasedId id must be unique
	assert(type->getEntityByEnvironmentBasedId(environmentBasedId) == NULL);

	trans = identityTransformation();
	env = NULL;

	visualRepresentation = NULL;
	sgIF = OutputInterface::getSceneGraphInterface();
	if (sgIF) {
		if (!type->modelTemplate)
			printd(ERROR, "Entity constructor: (%u, %u) modelTemplate of EntityType not set!\n",
					type->getId(), instanceId);
		else
			visualRepresentation = type->modelTemplate->clone();
	} // if

	//	sceneGraphInterfacePrivateData = NULL;
	sceneGraphAttachment = NULL;
} // Entity

Entity::~Entity() {
	Environment * env;
	unsigned short environmentId, entityId;

	// remove Entity from EntityMap in Environment
	split(environmentBasedId, environmentId, entityId);
	env = WorldDatabase::getEnvironmentWithId(environmentId);
	if (env)
		env->removeDeletedEntity(this);

	type->removeInstance(this);

	if (sceneGraphAttachment != NULL) {
		delete sceneGraphAttachment;
		sceneGraphAttachment = NULL;
	} // if

	delete visualRepresentation;
} // ~Entity

void Entity::changeEnvironment(Environment* newEnvironment) {
	Environment* currentEnv = env;
	TransformationData newEnvTransf, currentWorldTransf, invEnvTransf, newInEnvTransf;
	unsigned short envId, entId;

	assert(newEnvironment);
	if (currentEnv) {
		currentWorldTransf = getWorldTransformation();
		newEnvTransf = newEnvironment->getWorldTransformation();
		invert(invEnvTransf, newEnvTransf);
		multiply(newInEnvTransf, invEnvTransf, currentWorldTransf);

		if (!currentEnv->removeEntity(this)) {
			printd(ERROR,
					"Entity::changeEnvironment(): environment does not contain requested entity\n");
		}
		trans = newInEnvTransf;
		newEnvironment->addEntity(this);
		split(environmentBasedId, envId, entId);
		printd(
				INFO,
				"Entity::changeEnvironment(): entity with Id (env: %u / id: %u) changed from environment %u to environment %u\n",
				envId, entId, currentEnv->getId(), newEnvironment->getId());
		update();
	} // if
	else {
		printd(WARNING, "Entity::changeEnvironment(): environment not set\n");
		newEnvironment->addEntity(this);
	} // else
} // changeEnvironment

void Entity::dump() {
	printd(INFO, "Entity::dump(): entityTransform  Entity - TYPE: %u ID: %u\n", type->getId(),
			(environmentBasedId & 0xFFFF));
} // dump

void Entity::setFixed(bool fixed) {
	this->fixed = fixed;
} // setFixed

void Entity::setEnvironmentTransformation(TransformationData trans) {
	this->trans = trans;
	update();
} // setEnvironmentTransformation

void Entity::setWorldTransformation(TransformationData trans) {
	TransformationData envTrans = env->getWorldTransformation();
	trans.position[0] -= envTrans.position[0];
	trans.position[2] -= envTrans.position[2];
	this->trans = trans;
	update();
} // setWorldTransformation

void Entity::setAttachment(AttachmentKey key, void* attachment) {
	attachmentMap[key] = attachment;
} // setAttachment

bool Entity::isFixed() {
	return fixed;
} // isFixed

unsigned short Entity::getInstanceId() {
	unsigned short entTypeId;
	unsigned short entInstId;
	split(typeBasedId, entTypeId, entInstId);
	return entInstId;
} // getInstanceId

unsigned int Entity::getTypeBasedId() {
	return typeBasedId;
} // getTypeBasedId

unsigned int Entity::getEnvironmentBasedId() {
	return environmentBasedId;
} // getEnvironmentBasedId

EntityType* Entity::getEntityType() {
	return type;
} // getEntityType

Environment* Entity::getEnvironment() {
	return env;
} // getEnvironment

TransformationData Entity::getEnvironmentTransformation() {
	return trans;
} // getEnvironmentTransformation

TransformationData Entity::getWorldTransformation() {
	TransformationData ret;
	TransformationData transfOfEnv;

	if (!env) {
		printd(WARNING, "Entity::getWorldTransformation(): environment not set\n");
		return identityTransformation();
	} // if

	transfOfEnv = env->getWorldTransformation();
	multiply(ret, transfOfEnv, trans);
	return ret;
} // getTransformation

ModelInterface* Entity::getVisualRepresentation() {
	return visualRepresentation;
} // getVisualRepresentation

void Entity::updateVisualRepresentation() {
	if(sgIF){
		sgIF->detachEntity(this);
		sceneGraphAttachment = NULL;		
		visualRepresentation = type->modelTemplate->clone();
		sgIF->attachEntity(this);
	} 
} // updateVisualRepresentation

void* Entity::getAttachment(AttachmentKey key) {
	return attachmentMap[key];
} // getAttachment


void Entity::update() {
	if (sgIF)
		sgIF->updateEntity(this);
} // update

void Entity::setEnvironment(Environment* env) {
	this->env = env;
	update();
} // setEnvironment

//****************************//
// PUBLIC DEPRECATED METHODS: //
//****************************//

// /** The method sets the translation-part of the Entity-Transformation
// * It also calls the update-method to write the current
// * Entity-Transformation to the SceneGraph
// * @param tranlation Translation of the Entity in Environment-coordinates
// */
//void Entity::setTranslation(gmtl::Vec3f translation)
//{
//	trans.position = translation;
//	update();
//} // setTranslation

// /** The method sets the rotation part of the Entity-Transformation
// * It also calls the update-method to write the current
// * Entity-Transformation to the SceneGraph
// * @param rotation Rotation of the Entity
// */
//void Entity::setRotation(gmtl::Quatf rotation)
//{
//	trans.orientation = rotation;
//	update();
//}

// /** The method sets the scale-part of the Entity-Transformation
// * It also calls the update-method to write the current
// * Entity-Transformation to the SceneGraph
// * @param scale Scale part of the Entity
// */
//void Entity::setScale(gmtl::Vec3f scale)
//{
//	trans.scale = scale;
//	update();
//} // setScale

// /** The method returns the Translation-part of the Transformation in Environment-coordinates
// * @return Translation of the Entity in Environment-coordinates
// */
//gmtl::Vec3f Entity::getTranslation()
//{
//	return trans.position;
//} // getTranslation

// /** The method returns the Rotation-part of the Transformation
// * @return Rotation of the Entity
// */
// gmtl::Quatf Entity::getRotation()
// {
// 	return trans.orientation;
// } // getRotation

// /** The method returns the Scale-part of the Transformation
// * @return Scale of the Entity
// */
// gmtl::Vec3f Entity::getScale()
// {
// 	return trans.scale;
// } // getScale
