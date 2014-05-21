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

#ifndef _ENVIRONMENT_H
#define _ENVIRONMENT_H

#include "Entity.h"
#include "Tile.h"
#include "../Configuration.h"
#include "../DataTypes.h"

class WorldDatabaseCreateEntityEvent;

/******************************************************************************
 * @class AbstractEntityCreationCB
 * @brief Abstract callback class for entity creation
 */
class INVRS_SYSTEMCORE_API AbstractEntityCreationCB {
public:

	/**
	 *
	 */
	virtual ~AbstractEntityCreationCB() {
	}

	/**
	 *
	 */
	virtual void call(Entity* newEntity) = 0;
}; // AbstractEntityCreationFunctor

/******************************************************************************
 * @class AbstractEntityDeletionCB
 * @brief Abstract callback class for entity deletion
 */
typedef AbstractEntityCreationCB AbstractEntityDeletionCB;

/******************************************************************************
 * @class EntityCreationCB
 * @brief Callback class for entity creation
 */
template<class TClass> class EntityCreationCB : public AbstractEntityCreationCB {
public:
	/**
	 *
	 */
	EntityCreationCB(TClass* object, void(TClass::*function)(Entity*)) {
		this->object = object;
		this->function = function;
	}

	/**
	 *
	 */
	virtual ~EntityCreationCB() {
	}

	/**
	 *
	 */
	virtual void call(Entity* newEntity) {
		(*object.*function)(newEntity);
	}

private:
	TClass* object;
	void (TClass::*function)(Entity*);
}; // EntityCreationCB

/******************************************************************************
 * @class EntityDeletionCB
 * @brief Callback class for entity deletion
 */
template<class TClass> class EntityDeletionCB : public AbstractEntityDeletionCB {
public:

	/**
	 *
	 */
	EntityDeletionCB(TClass* object, void(TClass::*function)(Entity*)) {
		this->object = object;
		this->function = function;
	}

	/**
	 *
	 */
	virtual ~EntityDeletionCB() {
	}

	/**
	 *
	 */
	virtual void call(Entity* newEntity) {
		(*object.*function)(newEntity);
	}

private:
	TClass* object;
	void (TClass::*function)(Entity*);
}; // EntityDeletionCB


/******************************************************************************
 * @class Environment
 * @brief This class is used for structuring the Virtual Environment.
 *
 * The virtual world is splitted up into several Environments which itself
 * consist of Tiles and Entities.
 * Tiles are fixed elements inside an Environment and are arranged in form of a
 * grid. Therefore a Tile always has to have a certain size to match into this
 * grid. The size of one Element and therefore also the minimum size of a Tile
 * is defined by the horizontal and vertical spacing (<code>xSpacing</code> /
 * <code>ySpacing</code>). The size of an Tile always has to be an integer
 * multiple of this minimum size.
 * Entities are elements which can be fixed or movable. They can be positioned
 * arbitrarily in the Environment. Entities can be added or removed from an
 * Environment during runtime.
 * To manage the IDs of the Entities inside an Environment each Environment has
 * it's own IdPool (<code>globalEnvironmentIdPool</code>). The IDs below 4096
 * of this IdPool are reserved for the Entities which are initially positioned
 * in an Environment defined by the configuration file. The IDs above 4096 are
 * used for Entities which are created during runtime. Each user in the system
 * gets his own IdPool with again 4096 free IDs which he can use to assign to
 * created Entities on runtime. These IDs are managed by the
 * <code>localEnvironmentIdPool</code> which is created in the
 * <code>getFreeEntityId</code> method.
 */
class INVRS_SYSTEMCORE_API Environment {
public:
	/**
	 * Constructor initializes the Environment.
	 * It sets the ID, the spacing and the size of the Environment. It also
	 * initializes the Pointer to the SceneGraphInterface and the
	 * <code>globalEnvironmentIdPool</code> used for Entity-IDs inside the
	 * environment.
	 * @param xSpacing horizontal size of an element in the Tile-grid
	 * @param ySpacing vertical size of an element in the Tile-grid
	 * @param initialXSize initial horizontal size of the Environment in
	 *                     spacing-coordinates
	 * @param initialZSize initial vertical size of the Environment in
	 *                     spacing-coordinates
	 * @param environmentId ID of the Environment
	 */
	Environment(int xSpacing, int zSpacing, int initialXSize, int initialZSize,
			unsigned environmentId);

	/**
	 * Destructor deletes all Entities and Tiles in the Environment.
	 * The destructor removes all Entities from the Environment and also
	 * deletes this Entities. It also deletes all Tiles inside the Tile-grid.
	 * At last it removes itself from the SceneGraphInterface.
	 */
	virtual ~Environment();

	/**
	 * Adds an entry-point to the Environment. The position has to be passed
	 * in environment coordinates. The passed view direction defines the
	 * initial orientation of the user when he enters the environment at this
	 * point.
	 * @param position position of the entry point in environment coordinates
	 * @param direction view direction for this entry point
	 */
	void addEntryPoint(gmtl::Vec3f position, gmtl::Vec3f direction);

	/**
	 *  Deletes the entry-point with the passed index from the environment.
	 *  @param index index of the entry-point
	 */
	void deleteEntryPoint(unsigned index);

	/**
	 * Sets the passed Tile to the passed position.
	 * The method is called by the WorldDatabase when the Environment is loaded.
	 * The passed position is in spacing-coordinates. The method searches for
	 * other Tiles in the Tile grid and deletes the passed Tile if it conflicts
	 * with another Tile. Otherwise it adds the Tile to the tile-grid and adds
	 * the Tile to the tile-grid and adds the Tile to the SceneGraph.
	 * NOTE: When calling this method by hand the Tile is only added in the
	 *       local copy of the Environment. When a multi-user application is
	 *       used then be sure to also call this method on all other hosts
	 *       (e.g. by sending an Event).
	 * @param x,z position of the new Tile in the Environment in spacing coord.
	 * @param tile the Tile that sould be entered
	 * @return true, if the Tile could be added
	 */
	bool setTileAtGridPosition(int x, int z, Tile* tile);

	/**
	 * Removes a Tile at the passed position.
	 * The method removes the Tile-entries in the Tile Grid and removes the
	 * graphical representation of the Tile from the SceneGraph. The passed
	 * positions arguments are the coordinates of the Tile in the Tile-Grid
	 * (in spacing coordinates). The Tile itself is not deleted and returned
	 * by the method.
	 * NOTE: When calling this method by hand the Tile is only removed in the
	 *       local copy of the Environment. When a multi-user application is
	 *       used then be sure to also call this method on all other hosts
	 *       (e.g. by sending an Event).
	 * @param x,z position of the Tile in the Tile-Grid
	 * @return Tile that is removed, NULL if not found
	 */
	Tile* removeTileAtGridPosition(int x, int z);

	/**
	 * Creates and adds a new Entity to the Environment.
	 * The method creates a new Entity of the passed EntityType and adds it to
	 * the Environment. If the new Entity could be added the method sends an
	 * <code>WorldDatabaseCreateEntityEvent</code> to all other hosts which adds
	 * the new Entity also on this hosts. This method should always be used when
	 * a new Entity is added to an Environment.
	 * @param entType EntityType of which a new instance should be created
	 * @param initialTransfInEnv initial Entity-Transformation in Environment-
	 *                           coordinates
	 * @return new Entity in Environment, NULL if an error occured
	 */
	unsigned createEntity(EntityType* entType, TransformationData initialTransfInEnv,
			AbstractEntityCreationCB* callback = NULL);

	/**
	 * @todo document
	 */
	bool destroyEntity(Entity* entity, AbstractEntityDeletionCB* callback = NULL);

	/**
	 * @todo document
	 */
	bool unregisterEntityCreationCallback(unsigned environmentBasedId);

	/**
	 * @todo document
	 */
	bool unregisterEntityDeletionCallback(unsigned environmentBasedId);

	/**
	 * Allows to enlarge the Environment.
	 * The method enlarges the Tile-Grid of the Environment in both directions
	 * according to the passed parameters. The parameters are in
	 * spacing-coordinates. If the parameters are negative values then the
	 * Environment is enlarged in the negative direction (left or up). If the
	 * Environment is enlarged in negative direction then the coordinates of
	 * the Tiles in the Environment change so that the Tile in the upper left
	 * corner is at Position (0,0).
	 * NOTE: The method just enlarges the Environment but does not check if the
	 *       enlarged environment overlaps with another environment. It also
	 *       can not adopt Tiles from other Environments.
	 * @param xDirection number of Tiles added in horizontal direction
	 * @param zDirection number of Tiles added in vertical direction
	 */
	void enlargeEnvironment(int xDirection, int zDirection);

	// TODO: add extra method for getting the tile that was inital at position 0,0
	// 	void shrinkEnvironment(int xDirection, int yDirection, std::vector<Entity*>* lostEntities, std::vector<TileInstance*>* lostTileInstances);

	/**
	 * Converts the passed Transformation from world coordinates to Environment
	 * coordinates.
	 * @param worldTransformation Transformation in world coordinates
	 * @return passed Transformation in Environment coordinates
	 */
	TransformationData convertWorldToEnvironment(TransformationData worldTransformation);

	/**
	 * Converts the passed Transformation from Environment coordinates to world
	 * coordinates.
	 * @param environmentTransformation Transformation in Environment coordinates
	 * @return passed Transformation in world coordinates
	 */
	TransformationData convertEnvironmentToWorld(TransformationData environmentTransformation);

	/**
	 * Does a ray intersection test with all Entities in the Environment and
	 * returns the Entity which is closest to the passed position and hit by
	 * the ray and the distance from the passed position to the Entity. The
	 * return value indicates if the ray hit an Entity or not.
	 * <br/>
	 * Note: coordinates are world-coordinates.
	 * @param position start position of the ray
	 * @param direction direction of the ray
	 * @param intDist pointer where to write the calculated distance to
	 * @param intersectedEntity pointer where to write the resulting Entity to
	 * @return true if an Entity was hit, false otherwise
	 */
	bool rayIntersect(gmtl::Vec3f position, gmtl::Vec3f direction, float* intDist,
			Entity** intersectedEntity);


	/**
	 * Shows or hides the graphical representation of the Environment
	 * @param visible true when the environment should be displayed.
	 */
	void setVisible(bool visible);

	/**
	 * Sets the horizontal position of the Environment in spacing coordinates.
	 * The passed position is used as the upper left corner of the Environment.
	 * All Tiles and Entities in this Environment are transformed as well.
	 * NOTE: Be careful when moving the Environment during runtime to not
	 *       overlap another Environment!
	 * @param xPosition new horizontal position of the Environment
	 */
	void setXPosition(int xPosition);

	/**
	 * Sets the vertical position of the Environment in spacing coordinates.
	 * The passed position is used as the upper left corner of the Environment.
	 * All Tiles and Entities in this Environment are transformed as well.
	 * NOTE: Be careful when moving the Environment during runtime to not
	 *       overlap another Environment!
	 * @param zPosition new vertical position of the Environment
	 */
	void setZPosition(int zPosition);
	
	/**
	 * Sets the name of the XML-file where the Environmet should be stored.
	 * @param filename of the XML-file
	 */
	void setXmlFilename(std::string filename);

	/**
	 * Returns the Id of the Environment
	 * @return Id of the Environment
	 */
	unsigned short getId();

	/**
	 * Returns the Transformation of the entry point with the passed index.
	 * The returned Transformation is in Environment coordinates.
	 * @return Transformation of an entry point in Environment coordinates
	 */
	TransformationData getStartTransformation(unsigned index);

	/**
	 * Returns the Transformation of the entry point with the passed index.
	 * The returned Transformation is in World coordinates.
	 * @return Transformation of an entry point in World coordinates
	 */
	TransformationData getWorldStartTransformation(unsigned index);

	/**
	 * Provides the number of available start transformations.
	 * @return number of start transformations for environment
	 */
	unsigned getNumberOfStartTransformations();

	/**
	 * Returns the horizontal spacing. The spacing is the size of one Element
	 * in the Tile grid. A Tile always has to have a size which is an integer
	 * multiple of the spacing.
	 * @return horizontal spacing of the Tile grid.
	 */
	int getXSpacing();

	/**
	 * Returns the vertical spacing. The spacing is the size of one Element
	 * in the Tile grid. A Tile always has to have a size which is an integer
	 * multiple of the spacing.
	 * @return vertical spacing of the Tile grid.
	 */
	int getZSpacing();

	/**
	 * Returns the horizontal size of the Environment in spacing coordinates.
	 * @return horizontal size of the Environment in spacing coordinates
	 */
	int getXSize();

	/**
	 * Returns the vertical size of the Environment in spacing coordinates.
	 * @return vertical size of the Environment in spacing coordinates
	 */
	int getZSize();

	/**
	 * Returns the horizontal position of the Environment in the world in
	 * spacing coordinates.
	 * @return horizontal position of the Environment in spacing coordinates
	 */
	int getXPosition();

	/**
	 * Returns the vertical position of the Environment in the world in
	 * spacing coordinates.
	 * @return vertical position of the Environment in spacing coordinates
	 */
	int getZPosition();

	/**
	 * Returns the Transformation of the Environment in world coordinates.
	 * The orientation is always the identity rotation, the position is
	 * generated out of the horizontal position of the Environment in spacing
	 * coordinates multiplied with the horizontal spacing.
	 * @return Transformation of the Environment in world coordinates.
	 */
	TransformationData getWorldTransformation();

	/**
	 * Returns the Tile which is positioned at the passed coordinates.
	 * The coordinates are interpreted as coordinates in the Tile Grid.
	 * @param x,z position in the Tile Grid
	 * @return Tile which is located at the position of the Tile Grid
	 */
	Tile* getTileAtGridPosition(int x, int z);

	/**
	 * Returns the Tile which is positioned at the passed world coordinates.
	 * It first calculates if the world position lies within the Environment
	 * and if so it calculates the matching position in the Tile Grid and
	 * returns the Tile positioned there. If the world position is not inside
	 * this Environment then the method returns NULL.
	 * @param x,z world position of the searched Tile
	 * @return Tile at the world position if in this Environment, NULL otherwise
	 */
	Tile* getTileAtWorldPosition(float x, float z);

	/**
	 * Returns the position of a Tile from the current Environment in world
	 * coordinates. It therefore tries to find the Tile in the Tile-Grid and
	 * then calculates it's position in world coordinates. If the Tile could
	 * not be found in the Tile Grid the method prints a Warning and returns
	 * the Vector (0,0,0).
	 * @param tle Tile from which the world position is requested
	 * @return world position of the Tile if found, otherwise (0,0,0)
	 */
	gmtl::Vec3f getWorldPositionOfTile(Tile* tle);

	/**
	 * Returns the Entity with the passed environmentBasedId.
	 * The method first checks if the Entity is originally created in this
	 * Environment. If so it checks if the Entity can be found in the EntityMap.
	 * If it is not in the EntityMap the method searches through all Entities
	 * which are currently inside the Environment and checks if the passed id
	 * matches. If no matching Entity was found the method returns NULL.
	 * NOTE: If the method returns an Entity then this Entity must not reside
	 *       inside this Environment. It could also be that the Entity was
	 *       originally created in this Environment but has moved to another
	 *       Environment.
	 * @param envBasedId environmentBasedId of the searched Entity
	 * @return Entity with the passed environmentBasedId if it is found in
	 *         this Environment
	 */
	Entity* getEntityByEnvironmentBasedId(unsigned envBasedId);

	/**
	 * Returns a the pointer to the Tile-Grid.
	 * WARNING: the method returns the pointer so every change also changes data
	 * in the Environment!!!
	 * @return the pointer to the Tile Grid
	 */
	Tile** getTileGrid();

	/**
	 * Returns a pointer to the Entity-list.
	 * WARNING: the method returns a pointer to the list so every change also
	 * changes data in the Environment!!!
	 * @return pointer to Entity-list
	 */
	const std::vector<Entity*>& getEntityList();

	/**
	 * Returns the name of the XML-file where the Environment is stored.
	 * @return filname of the XML-File
	 */
	std::string getXmlFilename();

	/**
	 * Dumps the Environment including the Tile Grid and all entities.
	 */
	void dump();

	/**
	 * Dumps the Tile Grid of the Environment.
	 */
	void dumpTileGrid();

	/**
	 * Pointer for SceneGraphInterface to store Environment-specific data
	 */
	SceneGraphAttachment* sceneGraphAttachment;

private:

	friend class Entity;
	friend class EntityType;
	friend class SystemCore;
	friend class WorldDatabase;
	friend class WorldDatabaseSyncEvent;
	friend class WorldDatabaseCreateEntityEvent;
	friend class WorldDatabaseDestroyEntityEvent;

	/**
	 * Adds the passed Entity to the Environment.
	 * The method can be used to add an already existing Entity to the
	 * Environment which was originally created in another Environment.
	 * It sets the Environment-pointer of the passed Entity, adds
	 * the Entity to the list of local Entities.
	 * NOTE: The method does not remove the Entity from another Environment
	 *       neither correct the Transformation of the passed Entity.
	 * @param entity Entity to add to the Environment
	 * @return true if no error occured
	 */
	bool addEntity(Entity* entity);

	/**
	 * Removes an Entity from the Environment.
	 * The method removes the passed Entity from the entityList and removes its
	 * graphical representation from the SceneGraph. The Entity-object itself
	 * is not deleted.
	 * @param entity Entity that should be removed from the Environment
	 * @return true if the Entity could be removed
	 */
	bool removeEntity(Entity* entity);

	/**
	 * Returns the name of the globalEnvironmentIdPool.
	 * @return name of the globalEnvironmentIdPool
	 */
	std::string getGlobalEnvironmentIdPoolName();

	/**
	 * Sets the localEnvironmentIdPool.
	 * @param newlocalPool pointer to the new localEnvironmentIdPool
	 */
	void setLocalEnvironmentIdPool(IdPool* newlocalPool);

	/**
	 * Updates the Environment Transformation.
	 * The method is called when the position of the Environment is changed.
	 * It updates the TransformationData which stores the Environment
	 * transformation.
	 */
	void updatePosition();

	/**
	 * Adds the passed Entity to the Environment.
	 * The method can be used to add a new but already created Entity to the
	 * Environment. It sets the Environment-pointer of the passed Entity, adds
	 * the Entity to the list of local Entities and attaches the Entity to the
	 * SceneGraph.
	 * NOTE: The method does not remove the Entity from another Environment
	 *       neither correct the Transformation of the passed Entity. It should
	 *       only be used to initially add existing Entities to an Environment
	 *       as it is done by the <code>WorldDatabaseCreateEntityEvent</code>
	 * @param entity Entity to add to the Environment
	 * @return true if no error occured
	 */
	bool addNewEntity(Entity* newEntity);

	/**
	 * Removes an Entity from the EntityMap.
	 * The method is called by the destructor of an Entity which unregisters
	 * itself from the Environment before it enters the happy hunting ground.
	 * @param entity Entity which should be removed.
	 */
	void removeDeletedEntity(Entity* entity);

	/**
	 * Returns the next a Entity ID in this Environment from the local id pool.
	 * @return free Entity ID
	 */
	unsigned short getFreeEntityId();

	/// ID of the Environment
	unsigned short environmentId;

	/// World-Transformation of the Environment
	TransformationData envTransformation;

	/// position and size in spacing-units
	int posX, posZ, sizeX, sizeZ;
	/// minimum size of a tile
	int xSpacing, zSpacing;
	/// offset information -> used for enlargement or shrink of environment
	int xOffset, zOffset;

	/// entry points for users in Environment
	std::vector<TransformationData> entryPoints;
	/// grid where all Tiles are stored
	Tile** tileGrid;
	/// list of all Entities positioned inside the Environment
	std::vector<Entity*> entityList;
	/// stores all Entities with EnvironmentBasedID = this->environmentId.xxx
	std::map<unsigned short, Entity*> entityMap;

	std::map<unsigned, AbstractEntityCreationCB*> entityCreationCallback;

	std::map<unsigned, AbstractEntityDeletionCB*> entityDeletionCallback;

	/// Pointer to the SceneGraphInterface if existing
	SceneGraphInterface* sgIF;

	/// name of the globalEnvironmentIdPool
	std::string globalEnvironmentIdPoolName;
	/// global IdPool for Environment
	IdPool globalEnvironmentIdPool;
	/// local IdPool for Environment
	IdPool* localEnvironmentIdPool;
	/// IdPool used at load time
	IdPool* loadTimeEnvironmentIdPool;

	/// name of the XML-file where the Environment is stored
	std::string xmlFilename;
};

#endif // _ENVIRONMENT_H
