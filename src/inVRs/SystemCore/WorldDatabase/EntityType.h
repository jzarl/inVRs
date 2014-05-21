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

#ifndef _ENTITYTYPE_H
#define _ENTITYTYPE_H

#include <string>
#include <iostream>

#include <irrXML.h>

#include "../IdPool.h"
#include "../DataTypes.h"
#include "../../OutputInterface/SceneGraphInterface.h"

using namespace irr;
using namespace io;

class Entity;
class Event;

/******************************************************************************
 * @class EntityType
 *
 * The class EntityType describes a single virtual object. From this object
 * there can exist multiple instances. It can be identified via an unique ID.
 * To create instances of the EntityType the class registers its own
 * instance-IDs from an IdPool. To be able to do so the class HAS TO be given
 * a name via the <code>setName</code> method. Setting the name without calling
 * the <code>setName</code> method will lead to an uninitialized IdPool which
 * results in an undefined behaviour. Usually the setName method is called by
 * the WorldDatabase. Anyway keep this in mind when implementing your own
 * EntityType.
 */
class INVRS_SYSTEMCORE_API EntityType {
public:
	/**
	 * Constructor initializes the members and sets the passed ID.
	 * @param id ID of the EntityType
	 */
	EntityType(unsigned short id);

	/**
	 * Destructor deletes all instances of the EntityType.
	 */
	virtual ~EntityType();

	/**
	 * The method dumps the EntityType's Id and name on the std:out
	 */
	virtual void dump();

	/**
	 * Sets the name of the EntityType and initalizes the globalEntityIdPool.
	 * The method sets the name of the EntityType and if it is called the
	 * first time it initializes the globalEntityIdPool.
	 * @param entName new name of the EntityType
	 */
	void setName(std::string entName);

	/**
	 * Defines if the EntityType should be fixed.
	 * @param fixed true if the EntityType should not be movable
	 */
	void setFixed(bool fixed);

	/**
	 * Sets the Visual representation of the EntityType. The method sets the
	 * passed Model to be used as template for new Entity-instances created by
	 * this class.
	 * @param model Model-template used for new Entity-instances
	 */
	virtual void setModel(ModelInterface* model);

	/**
	 * Sets the XML file where the EntityType should be stored.
	 * @param filename Filename of the XML-file
	 */
	void setXmlFilename(std::string filename);

	/**
	 * Returns the ID of the EntityType.
	 * @return ID of the EntityType
	 */
	unsigned short getId();

	/**
	 * Returns the name of the EntityType.
	 * @return name of the EntityType
	 */
	std::string getName();

	/**
	 * Returns the number of instances of the EntityType.
	 * @return number of EntityType instances
	 */
	unsigned getNumberOfInstances();

	/**
	 * Returns whether the EntityType is fixed.
	 * @return true if the EntityType is fixed
	 */
	bool isFixed();

	/**
	 * Returns the Entity of this EntityType which has the passed instanceId. It
	 * therefore iterates over all instances and checks the Entities for the
	 * passed Id.
	 * @param instanceId instanceId of the searched Entity (=lower 16 bit of
	 *					 Entity::typeInstanceId)
	 * @return Entity if found, NULL otherwise
	 */
	virtual Entity* getEntityByInstanceId(unsigned short instanceId);

	/**
	 * Returns the Entity of this EntityType which has the passed
	 * environmentBasedId. It therefore iterates over all instances and checks if
	 * one Entity has the passed environmentBasedId.
	 * @param envBasedId environmentBasedId of the searched Entity
	 * @return Entity if found, NULL otherwise
	 */
	virtual Entity* getEntityByEnvironmentBasedId(unsigned envBasedId);

	/**
	 * Returns the visual representation of the EntityType. The returned
	 * ModelInterface is used as template for the Entity-instances created by the
	 * class.
	 * @return Model-template for this EntityType, NULL if not set
	 */
	virtual ModelInterface* getModel();

	/**
	 * Returns the filename where the EntityType is stored.
	 * @return filname 
	 */
	std::string getXmlFilename();

	/**
	 * Returns a pointer to a list of instances. Do not change to content of the
	 * list! And of course don't delete it.
	 */
	const std::vector<Entity*>& getInstanceList();



protected:
	friend class Entity;
	friend class Environment;
	friend class WorldDatabase;
	friend class SystemCore;
	friend class WorldDatabaseSyncEvent;
	friend class WorldDatabaseCreateEntityEvent;

	/**
	 * Creates a new instance of itself. The method creates a new Entity instance
	 * where the instanceId is requested from the localEntityIdPool.
	 * @param idOfEnv ID of the destination Environment
	 * @param idInEnv ID of the Entity in the Environment
	 * @return new Entity instance of current EntityType
	 */
	virtual Entity* createInstance(unsigned short idOfEnv, unsigned short idInEnv);

	/**
	 * Creates a new instance of itself at load time. The method creates a new
	 * Entity instance where the instanceId is requested from the
	 * loadTimeEntityIdPool.
	 * @param idOfEnv ID of the destination Environment
	 * @param idInEnv ID of the Entity in the Environment
	 * @return new Entity instance of the current EntityType
	 */
	virtual Entity* createInstanceAtLoadTime(unsigned short idOfEnv, unsigned short idInEnv);

	/**
	 * Creates a new instance of itself with the passed IDs. The method creates a
	 * new Entity instance with the passed IDs.
	 * @param instId ID of the Entity instance
	 * @param idOfEnv ID of the destination Environment
	 * @param idInEnv ID of the Entity in the Environment
	 * @return new Entity instance of the current EntityType
	 */
	virtual Entity* createInstanceUnchecked(unsigned short instId, unsigned short idOfEnv,
			unsigned short idInEnv);

	/**
	 * Removes an instance from the list.
	 * The method is called from the Entity destructor and removes the passed entity
	 * from the list of instances.
	 * @param entity Entity which should be removed
	 */
	virtual void removeInstance(Entity* entity);

	/**
	 * Deletes all instances of the EntityType. The method deletes all created
	 * instances of this EntityType. It is mainly called by the destructor and the
	 * <code>WorldDatabase::clearWorld</code> method.
	 */
	virtual void clearInstances();

	/**
	 * Creates the EntityCreationEvent for a new instance.
	 */
	virtual Event* createEntityCreationEvent(Environment* env, unsigned environmentBasedId,
			TransformationData trans);

	/**
	 * Sets the model used for the instances.
	 * @param modelTemplate Model which is used for instances
	 */
	void setModelTemplate(ModelInterface* modelTemplate);

	/**
	 * Sets the localEntityIdPool. The method is called when connecting to other
	 * users. After the localEntityIdPool is approved from all other users the
	 * new pool is set with this method.
	 * @param newlocalPool approved localEntityIdPool
	 */
	void setLocalEntityIdPool(IdPool* newlocalPool);

	/**
	 * Returns the name of the globalEntityIdPool. This is used when connecting to
	 * other users to allocate the localEntityIdPool.
	 * @return name of the globalEntityIdPool
	 */
	std::string getGlobalEntityIdPoolName();


	/**
	 * Returns the next Instance ID available from the local id pool.
	 * @return free instance ID
	 */
	unsigned short getFreeInstanceId();

	/**
	 * Returns the next free ID available from the id pool of the passed
	 * Environment
	 * @return free id in environment
	 */
	unsigned short getFreeIdInEnvironment(Environment* env);

	/// ID of the EntityType
	unsigned short id;
	/// name of the EntityType
	std::string name;
	/// boolean flag indicating that the model can't be moved
	bool fixed;

	/// name of the globalEntityIdPool (set in <code>setName</code> method)
	std::string nameGlobalEntityIdPool;
	/// globalEntityIdPool for EntityType
	IdPool globalEntityIdPool;
	/// localEntityIdPool for creation of new Entities
	IdPool* localEntityIdPool;
	/// loadTimeEntityIdPool for creation of Entities on startup
	IdPool* loadTimeEntityIdPool;

	/// list of Entity instances of this EntityType
	std::vector<Entity*> instances;

	/// model used as template for the Entity instances
	ModelInterface* modelTemplate;

	/// name of the XML-file where the EntityType is stored
	std::string xmlFilename;
};
#endif // _ENTITYTYPE_H
