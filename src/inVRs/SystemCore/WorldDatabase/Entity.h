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

#ifndef _ENTITY_H
#define _ENTITY_H

#include <map>
#include <iostream>

#include "EntityType.h"
#include "AttachmentKey.h"
#include "../DataTypes.h"
#include "../../OutputInterface/SceneGraphInterface.h"
#include "../../OutputInterface/SceneGraphAttachment.h"

class Environment;

/******************************************************************************
 * @class Entity
 *
 * The class Entity represents an object in the virtual world. It can be
 * identified via two Ids, either the environmentBasedId or the typeBasedId.
 * The environmentBasedId is a 32bit Integer where the upper 16 bit store
 * the Id of the Environment it was built in and the lower 16 bit store the
 * Id of the Entity in this Environment.
 * The typeBasedId is also a 32bit Integer where the upper 16 bit store the
 * Id of the EntityType it belongs to and the lower 16 bit store the instance
 * Id of the Entity in the EntityType.
 * An Entity contains the Transformation of the object in the Environment it
 * resides. It can be set fixed or movable to allow users to manipulate it
 * or not. It also provides a method to change the Environment it belongs to
 * if it is moved from one Environment into another.
 */
class INVRS_SYSTEMCORE_API Entity {
public:
	/**
	 * Constructor sets the Ids of the Entity, initializes the boolean fixed
	 * with the one from the EntityType and clones the model from the
	 * EntityType it is an instance from.
	 * @param id ID of the Entity inside the Environment
	 * @param environmentId ID of the Environment the Entity should be built in
	 * @param instanceId ID of the Entity instance of the EntityType
	 * @param type EntityType from which the Entity is an instance of
	 */
	Entity(unsigned short id, unsigned short environmentId, unsigned short instanceId,
			EntityType *type);

	/**
	 * Destructor removes the Entity from the instanceList of the EntityType.
	 */
	virtual ~Entity();

	/**
	 * Moves the Entity from one Environment to another. It does so by calling the
	 * removeEntity()-method of the current Environment, correcting the
	 * Environment-Transformation of the Entity to match to the new Environment
	 * and calling the addEntity-method of the new Environment.
	 * @param newEnvironment Environment to which the Entity should be migrated
	 */
	virtual void changeEnvironment(Environment* newEnvironment);

	/**
	 * The method dumps information about the Entity.
	 */
	virtual void dump();

	/**
	 * Defines if the Entity is movable or not. The default value is taken from
	 * the EntityType the Entity is an instance of.
	 * @param fixed true if the Entity should not be movable, false otherwise
	 */
	virtual void setFixed(bool fixed);

	/**
	 * Sets the Transformation of the Entity in Environment-coordinates!!!
	 * It also calls the update-method to write the new Entity-Transformation
	 * to the SceneGraph.
	 * @param trans the Transformation of the Entity in Environment-coordinates
	 */
	virtual void setEnvironmentTransformation(TransformationData trans);

	/**
	 * Sets the Transformation of the Entity in World-coordinates!!!
	 * It also calls the update-method to write the new Entity-Transformation
	 * to the SceneGraph.
	 * @param trans the Transformation of the Entity in World-coordinates
	 */
	virtual void setWorldTransformation(TransformationData trans);

	/**
	 * Stores the passed attachment under the passed key.
	 * @param key Key with which the attachment should be identified
	 * @param attachment Attachment to store
	 */
	void setAttachment(AttachmentKey key, void* attachment);

	/**
	 * Returns if the Entity is fixed or movable.
	 * @return true if the Entity is not movable, false otherwise
	 */
	bool isFixed();

	/**
	 * Returns the instanceId of the Entity. The instanceId is a unique ID inside
	 * the associated EntityType. It is part of the typeBasedId. This can be used
	 * to identify the Entity as one of many of the same EntityType.
	 * @return unique instanceId of the Entity inside the associated EntityType.
	 */
	unsigned short getInstanceId();

	/**
	 * Returns the TypeBasedId of the Entity.
	 * This Id consists of:
	 *    upper 16 bit: Id of the EntityType the Entity belongs to
	 *    lower 16 bit: Id of the instance of the Entity inside the EntityType
	 * @return typeBasedId of the Entity
	 */
	unsigned int getTypeBasedId();

	/**
	 * Returns the EnvironmentBasedId of the Entity.
	 * This id consists of:
	 *    upper 16 bit: Id of the Environment it was built in
	 *    lower 16 bit: Id of the Entity in the Environment (as configured in
	 * 					the config file)
	 * NOTE: the environmentBasedId never changes over time, so the part of the
	 *       ID containing the Environment-ID belongs always to the Environment
	 *       the Entity was built in which must not be the same as the
	 *       Environment to which the Entity currently belongs to. This concept
	 *       allows to access Entities according to their IDs set in the
	 *       config file.
	 * @return environmentBasedId of the Entity
	 */
	unsigned int getEnvironmentBasedId();

	/**
	 * Returns the EntityType from which the Entity is an instance of.
	 * @return EntityType of the Entity
	 */
	EntityType* getEntityType();

	/**
	 * Returns the Environment to which the Entity currently belongs to.
	 * At startup the Entity is created in an Environment. Over time the Entity
	 * can be moved from one Environment to another one. This method always
	 * returns the Environment where the Entity can currently be found.
	 * @return Environment the Entity belongs to
	 */
	Environment* getEnvironment();

	/**
	 * Returns the Transformation of the Entity in Environment-coordinates.
	 * @return Transformation in Environment-coordinates
	 */
	TransformationData getEnvironmentTransformation();

	/**
	 * Returns the Transformation of the Entity in World-coordinates.
	 * @return Transformation in World-coordinates
	 */
	TransformationData getWorldTransformation();

	/**
	 * Returns the visual representation of the Entity. The method returns the
	 * Model used as the visual representation of the Entity.
	 * @return Model used for Entity.
	 */
	ModelInterface* getVisualRepresentation();

	/**
	 * Updates the visual representation of the Entity to the current model of
	 * the EntityType.
	 */
	void updateVisualRepresentation();

	/**
	 * Returns the attachment stored with the passed key.
	 * @param key Key for the attachment to search
	 * @return AttachmentKey stored with the passed key
	 */
	void* getAttachment(AttachmentKey key);

	//****************************//
	// PUBLIC DEPRECATED METHODS: //
	//****************************//

	//	virtual void setTranslation(gmtl::Vec3f translation);
	//	virtual void setRotation(gmtl::Quatf rotation);
	//	virtual void setScale(gmtl::Vec3f scale);
	// 	virtual gmtl::Vec3f getTranslation();
	// 	virtual gmtl::Quatf getRotation();
	// 	virtual gmtl::Vec3f getScale();

protected:
	friend class Environment;

	/**
	 * Updates the graphical representation of the Entity.
	 * The method calls the update-method from the SceneGraphInterface to
	 * update the current Entity-Transformation in the SceneGraph.
	 */
	virtual void update();

	/**
	 * Sets the Environment the Entity belongs to. It is called by the
	 * Environment-class and initially sets the pointer to the Environment the
	 * Entity is created in. The method is not intended to be used when the
	 * Environment is changed, therefore call the <code>changeEnvironment</code>
	 * method.
	 * @param env Environment the Entity belongs to
	 */
	virtual void setEnvironment(Environment* env);

public:
	/// Pointer for SceneGraphInterface to store Entity-specific data
	//TODO: change to protected (or private) member
	SceneGraphAttachment* sceneGraphAttachment;

protected:
	/// Defines if the Entity is fixed or movable
	bool fixed;

	///	unique ID consisting of:
	///     upper short: ID of the Environment the Entity was built in
	///     lower short: ID of the Entity inside the Environment (configured in
	///                  the config-file)
	unsigned int environmentBasedId;

	/// unique ID consisting of:
	///     upper short: ID of the EntityType of the Entity
	///     lower short: instance ID of the Entity in the EntityType
	unsigned int typeBasedId;

	/// Transformation of the entity in env space
	/// (= matrix from entity space to env space)
	TransformationData trans;

	/// Pointer to the EntityType the Entity is an instance of
	EntityType *type;
	/// Pointer to the Environment the Entity is insied
	Environment *env;
	/// Pointer to the SceneGraphInterface used for rendering
	SceneGraphInterface *sgIF;
	/// Pointer to the graphical model representing the Entity
	ModelInterface* visualRepresentation;

	/// Map for Modules to store Entity-specific data
	std::map<AttachmentKey, void*> attachmentMap;
};

#endif // _ENTITY_H
