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

#ifndef _WORLDDATABASE_H
#define _WORLDDATABASE_H

#include <iostream>
#include <string>

#include <irrXML.h>

#include "Tile.h"
#include "Entity.h"
#include "Environment.h"
#include "SimpleAvatar.h"
#include "../Configuration.h"
#include "../../OutputInterface/SceneGraphInterface.h"
#include "../XmlConfigurationConverter.h"
#include "../XmlElement.h"

class EntityTypeFactory;

/******************************************************************************
 * The WorldDatabase acts as a DB for the logical objects of the VE. Entities,
 * Tiles and Environment data is stored in the database. User avatars are stored
 * as well in the WDB.
 * @author canthes,rlander,hbress
 */
class INVRS_SYSTEMCORE_API WorldDatabase {
public:
	/****************************************************************************
	 * This class provides access to internal methods of the WorldDatabase.
	 * WARNING: DON'T USE THIS CLASS IN NETWORKED APPLICATIONS BECAUSE IT
	 * DOES NOT COPE WITH SYNCHRONIZATION
	 */
	class INVRS_SYSTEMCORE_API PrivateAccessor {
	public:
		/**
		 * Adds the passed EntityType to the WorldDatabase
		 * @param entityType
		 */
		void addEntityType(EntityType* entityType) const;

		/**
		 * Deletes the passed EntityType and all Instances (Entities) from
		 * the WorldDatabse
		 * @param entityType
		 */
		void deleteEntityType(EntityType* entityType) const;
	}; // PrivateAccessor

public:
	/**
	 * Initializes the WorldDatabase.
	 * The method adds the default Avatar factory to the list of Avatar
	 * factories. It also adds the default EntityType factory to the list of
	 * EntityType factories. Furthermore it requests the SceneGraphInterface
	 * from the SystemCore.
	 */
	static void init();

	/**
	 * Cleans up the WorldDatabase.
	 * The method deletes all Environments including all Entities and Tiles
	 * inside of the Environments. It also deletes all Tiles in the tile list
	 * and all EntityTypes. Furthermore it deletes all registered Avatars and
	 * Avatar factories and all EntityType factories.
	 * @see SystemCore::cleanup(), UserDatabase::cleanup()
	 */
	static void cleanup();

	/**
	 * Removes every Tile, Entity and Environment from the world.
	 * The method removes every Environment from the WorldDatabase. This
	 * includes every Tile and Entity. The Tile- and EntityType-vectors are not
	 * cleared.
	 */
	static void clearWorld();

	/**
	 * The method loads EntityTypes into the WorldDatabase.
	 * The EntityTypes are parsed from a XML file passed as an argument and then
	 * stored in the WorldDatabase.
	 * @param entityCfg the URL of the XML-EntityType definition file
	 */
	static bool loadEntityTypes(std::string entityCfg);

	/**
	 * Loads tiles into the WorldDatabase.
	 * The Tiles are parsed from a XML file passed as an argument and then
	 * stored in the WorldDatabase.
	 * @param tileCfg the URL of the XML-Tile definition file
	 */
	static bool loadTiles(std::string tileCfg);

	/**
	 * Loads all Environments into the WorldDatabase.
	 * The method reads the layout of the environments from the passed XML-file,
	 * creates each environment by calling the loadEnvironment method and puts
	 * every environment in the environmentList.
	 * @param envLayCfg the URL of the XML-config file
	 */
	static bool loadEnvironmentLayout(std::string envLayCfg);

	/**
	 * Fills the WorldDatabase according to the passed config file.
	 * Entity-, Tile- and Environment- configuration files are parsed from the
	 * config-file and then loaded into the WorldDatabase by the according
	 * methods. After the method call the WorldDatabase is fully filled.
	 * @param worldCfg URL to XML-World definition
	 * @return true if the world was loaded successfully
	 */
	static bool loadWorldDatabase(std::string worldCfg);

	/**
	 * Saves the EntityTypes to the according xml-files in the filesystem.
	 * @return vector of names of the written files.
	 */
	static std::vector<std::string> saveEntityTypes();

	/**
	 * Saves the Tiles to the according xml-files in the filesystem.
	 * @return vector of names of the written files.
	 */
	static std::vector<std::string> saveTiles();

	/**
	 * Saves the EnvironmentLayout and Environments to the according
	 * xml-files in the filesystem.
	 */
	static void saveEnvironmentLayout();

	/**
	 * Saves the current WorldDatabase to xml-files.
	 * The Method calls saveEntityTypes(), saveTiles() and
	 * saveEnvironmentLayout() to save all data to the filesystem.
	 */
	static void saveWorldDatabase();

	/**
	 * TODO: document
	 */
	static bool reloadEnvironmentLayout();

	/**
	 * Returns the EntityType with the passed ID.
	 * @param id ID of the EntityType
	 * @return EntityType with the corresponding ID, NULL if not existing
	 */
	static EntityType* getEntityTypeWithId(unsigned short id);

	/**
	 * Returns the EntityType with the given name.
	 * @param name Name of the EntityType, as specified in the xml-configuration.
	 * @return EntityType with the corresponding name, NULL if not existing
	 */
	static EntityType* getEntityTypeWithName(std::string name);

	/**
	 * Returns the Entity with the corresponding environmentBasedId.
	 * @param environmentBasedId summarized EntityId from environmentId and entityId in environment
	 * @return Entity with the corresponding ID, NULL if not found
	 */
	static Entity* getEntityWithEnvironmentId(unsigned environmentBasedId);

	/**
	 * Returns The Entity from the Environment with the passed ID.
	 * @param environmentId ID of the Environment where the Entity was created
	 * @param entityId ID of the Entity inside the Environment
	 * @return Entity with the corresponding ID, NULL if not found
	 */
	static Entity* getEntityWithEnvironmentId(unsigned short environmentId,
			unsigned short entityId);

	/**
	 * Returns the Entity with the passed ID.
	 * @param typeInstanceId the typeInstanceId of the Entity
	 * @return Entity with the corresponding ID, NULL if not existing
	 */
	static Entity* getEntityWithTypeInstanceId(unsigned typeInstanceId);

	/**
	 * Returns the Entity with the passed ID.
	 * The passed ID is split up into the entityType-ID and the instance-ID of
	 * the Entity
	 * @param entityTypeId ID of the EntityType
	 * @param instanceId instance-ID of the Entity
	 * @return Entity with the corresponding ID, NULL if not existing
	 */
	static Entity* getEntityWithTypeInstanceId(unsigned short entityTypeId,
			unsigned short instanceId);

	/**
	 * Returns a Tile with the passed ID if found.
	 * @param id ID of the desired Tile
	 * @return Tile with the passed ID if exist, NULL otherwise
	 */
	static Tile* getTileWithId(int id);

	/**
	 * Returns the Environment with the passed ID.
	 * @param environmentId ID of the desired Environment
	 * @return Environment with the passed ID, NULL if not existing
	 */
	static Environment* getEnvironmentWithId(unsigned short environmentId);

	/**
	 * Returns the Environment at the passed position.
	 * The method iterates over each Environment and checks if the passed point
	 * is inside the Environment. If so it returns the Environment.
	 * @param x,z point in world coordinates
	 * @return Environment in which point lies, NULL if no Environment found
	 */
	static Environment* getEnvironmentAtWorldPosition(float x, float z);

	/**
	 * Creates a new Entity of passed Type in the passed environment.
	 * The method tries to find the desired EntityType and the initial
	 * Environment of the Entity and forwards the call to the createEntity
	 * method of the Environment.
	 * @param entityTypeId ID of the desired EntityType
	 * @param startEnvironmentId ID of the Environment the Entity should be built in
	 * @param startTrans initial Transformation of the Entity, default is IdentityTransformation
	 * @return environmentBasedId of new Entity
	 */
	static unsigned createEntity(unsigned short entityTypeId, unsigned short startEnvironmentId,
			TransformationData startTrans = identityTransformation(),
			AbstractEntityCreationCB* callback = NULL);

	/**
	 * Creates a new Entity of passed Type in the passed environment.
	 * The method tries to find the desired EntityType and the initial
	 * Environment of the Entity and forwards the call to the createEntity
	 * method of the Environment.
	 * @param entityType The desired EntityType
	 * @param startEnvironmentId The Environment the Entity should be built in
	 * @param startTrans initial Transformation of the Entity, default is IdentityTransformation
	 * @return environmentBasedId of new Entity
	 */
	static unsigned createEntity(EntityType* type, Environment* startEnv,
			TransformationData startTrans = identityTransformation(),
			AbstractEntityCreationCB* callback = NULL);

	/**
	 * Creates a new Environment
	 * The method tries to create a new environment with the passed ID
	 * and the passed size at the defined positions. The method will only
	 * be successful if following conditions are met:
	 * 1) The environment-ID has to be unique (no other environment has the
	 *    passed ID already)
	 * 2) The xPosition and zPosition values of the environment must be
	 *    multiples of the xSpacing and zSpacing values. This is essential in
	 *    order to allow the dynamic movement of environment borders
	 * 3) The environment does not superpose other environments
	 *
	 * NOTE: The xSize and zSize values define the size of the tile-grid and
	 *       not the overall Environment size. The overall size is calculated
	 *       by multiplying xSize and zSize with the xSpacing and zSpacing
	 *       values.
	 *
	 * @param id id of the new environment
	 * @param xPosition horizontal position of the environment
	 * @param zPosition vertical position of the environment
	 * @param xSize horizontal size of the environment (= horizontal grid size)
	 * @param zSize vertical size of the environment (= vertical grid size)
	 */
	static bool createEnvironment(unsigned short id, int xPosition, int zPosition, int xSize, int zSize);

	/** Destroys an existing Entity.
	 * The method creates a WorldDatabaseDestroyEntityEvent and sends it to all
	 * connected users including the local machine.
	 * @param entity Entity which should be destroyed
	 * @return true if the Event could be created and was sent, false if not
	 */
	static bool destroyEntity(Entity* entity, AbstractEntityDeletionCB* callback = NULL);

	/**
	 * @todo document
	 */
	static bool unregisterEntityCreationCallback(unsigned environmentBasedId);

	/**
	 * @todo document
	 */
	static bool unregisterEntityDeletionCallback(unsigned environmentBasedId);

	/**
	 * Registers an EntityTypeFactory in the WDB.
	 * All registered EntityTypeFactories will be delete'd automatically when <code>cleanup()</code> is invoked
	 * @author hbress
	 * @param entityTypeFactory entityTypeFactory that should be added
	 */
	static void registerEntityTypeFactory(EntityTypeFactory* entityTypeFactory);

	/**
	 * Registers an AvatarFactory in the WDB.
	 * All registered AvatarFactories will be delete'd automatically when <code>cleanup()</code> is invoked
	 * @author landi
	 * @param avatarFactory avatarFactory that should be added
	 */
	static void registerAvatarFactory(AvatarFactory* avatarFactory);

	/**
	 * Loads an Avatar from a XML-config file.
	 * The method scans the config file for the type of the Avatar and
	 * then calls the create-method of the registered avatar-
	 * factories which should return an Avatar object. Afterwards
	 * the avatar is added to the SceneGraph.
	 * @author landi
	 * @param configFile URL of the avatar XML file
	 * @return new avatar object
	 */
	static AvatarInterface* loadAvatar(std::string configFile);

	/**
	 * Removes an Avatar from the WorldDatabase.
	 * The method removes the Avatar-OpenSG-Node from the World and
	 * removes the Avatar-Object from the avatar list. The
	 * Avatar object itself is NOT deleted!
	 * @author landi
	 * @param avatarInt the Avatar object which should be removed
	 */
	static void removeAvatar(AvatarInterface* avatarInt);

	/**
	 * Updates all registered avatars.
	 * It iterates over all elements of the avatarList and calls their
	 * update-methods.
	 * @author landi
	 * @param dt elapsed time since last call
	 */
	static void updateAvatars(float dt);

	/**
	 * Returns the used horizontal spacing.
	 * @return horizontal spacing (number of units per tile)
	 */
	static int getXSpacing();

	/**
	 * Returns the used vertical spacing.
	 * @return vertical spacing (number of units per tile)
	 */
	static int getZSpacing();

	/**
	 * Returns a pointer to the EntityType-list.
	 * WARNING: the method returns a pointer to the list so
	 * every change also changes data in the WDB!!!
	 * @return pointer to EntityType-list
	 */
	static const std::vector<EntityType*>& getEntityTypeList();

	/**
	 * Returns a pointer to the Tile-list.
	 * WARNING: the method returns a pointer to the list so
	 * every change also changes data in the WDB!!!
	 * @return pointer to Tile-list
	 */
	static const std::vector<Tile*>& getTileList();

	/**
	 * Returns a pointer to the Environment-list.
	 * WARNING: the method returns a pointer to the list so
	 * every change also changes data in the WDB!!!
	 * @return pointer to Environment-list
	 */
	static const std::vector<Environment*>& getEnvironmentList();

	/**
	 * Dumps all EntityTypes.
	 */
	static void dumpEntityTypes();

	/**
	 * Dumps all Tiles.
	 */
	static void dumpTiles();

	/**
	 * Dumps all Environments.
	 */
	static void dumpEnvironment();

	/**
	 * Dumps all EntityTypes, Tiles and Environments.
	 */
	static void dump();

	/**
	 *
	 */
	static const WorldDatabase::PrivateAccessor& getPrivateAccessor();

	/**
	 * @deprecated
	 */
	static void reloadEnvironments();

private:

	friend class WorldDatabaseReloadEnvironmentsEvent;
	friend class WorldDatabaseCreateEnvironmentEvent;

	static bool loadEnvironmentLayout(std::string config, bool reload);

	/**
	 * Loads an Environment from a config file.
	 * The Environment is parsed from a XML file passed as an argument. The
	 * provided arguments are passed to the Environment constructor. Finally the
	 * built Environment is returned.
	 * @param envCfg the URL of the XML-Environment definition file
	 * @param xSpacing the horizontal Tile-spacing
	 * @param ySpacing the vertical Tile-spacing
	 * @param environmentId the ID of the new Environment
	 * @param reload defines if an existing environment should be reloaded or a new one should be created
	 * @return the new built Environment
	 */
	static Environment* loadEnvironment(std::string envCfg, int xSpacing, int ySpacing,
			int environmentId, bool reload = false);

	/**
	 * Parses an entry point from the XmlElement and adds it to the Environment
	 * @param entryPointElement
	 * @param env
	 * @return true if the entry-point could be parsed and added
	 */
	static bool addEntryPointToEnvironment(const XmlElement* entryPointElement, Environment* env);

	/**
	 * Creates a new Entity from the config element.
	 * The Entity element is parsed, created and added to the passed
	 * environment.
	 * @param entityElement
	 * @param env
	 * @param reload
	 * @return true if the entity was created successfully
	 */
	static bool createEntityInEnvironment(const XmlElement* entityElement, Environment* env,
			bool reload);

	/**
	 * Parses the tileMap element and adds all tiles to the passed environment
	 * @param tileMapElement
	 * @param env
	 * @return true if the tile map was loaded successfully
	 */
	static bool createTilesInEnvironment(const XmlElement* tileMapElement, Environment* env);

	/**
	 * Creates a new EntityType with the corresponding className.
	 * It iterates over all elements in the entityTypeFactoryList and tries
	 * to create a new EntityType with the passed className. If an EntityType
	 * was created then it sets the responsible factory to the passed pointer
	 * and returns the new EntityType.
	 * @author landi,hbress
	 * @param className name of the EntityType-class
	 * @param id id of the new EntityType
	 * @param factory optional parameter which can be used to return the factory
	 * 			which built the EntityType
	 * @return new EntityType, NULL if className was not found
	 */
	static EntityType* getEntityType(std::string className, unsigned short id,
			EntityTypeFactory** factory);

	/**
	 *
	 * @todo document
	 */
	static void executeEnvironmentReload();

	/**
	 * Adds the passed EntityType to the WDB
	 * @param entityType EntityType which should be added
	 */
	static void addEntityType(EntityType* entityType);

	/**
	 * Deletes the passed EntityType and all Instances (Entities) from
	 * the WorldDatabse
	 * @param entityType
	 */
	static void deleteEntityType(EntityType* entityType);

	/**
	 * Adds a new Environment to the WorldDatabase
	 * This method is called from the WorldDatabaseCreateEnvironmentEvent
	 * when a new Environment is created.
	 * @param environment Created environment which should be added to the WDB
	 */
	static void addNewEnvironment(Environment* environment);

	/// Configuration file for world configuration
	static std::string worldConfigFile;
	/// Configuration file for environment layout
	static std::string envLayoutConfigFile;

	/// List of all Tiles
	static std::vector<Tile*> tileList;
	/// List of all EntityTypes
	static std::vector<EntityType*> entityTypeList;
	/// List of all Environments
	static std::vector<Environment*> environmentList;

	/// Map from Tile ID to Tile
	static std::map<unsigned int, Tile*> tileMap;
	/// Map from EntityType ID to EntityType
	static std::map<unsigned short, EntityType*> entityTypeIDMap;
	/// Map from EntityType name to EntityType
	static std::map<std::string, EntityType*> entityTypeNameMap;
	/// Map from Environment ID to Environment
	static std::map<unsigned short, Environment*> environmentMap;

	/// List of all EntityTypeFactories
	static std::vector<EntityTypeFactory*> entityTypeFactories;
	/// List of all AvatarFactories
	static std::vector<AvatarFactory*> avatarFactories;

	/// List of all registered Avatars
	static std::vector<AvatarInterface*> avatarList;

	/// horizontal and vertical spacing (units per tile)
	static int xSpacing;
	static int zSpacing;

	/// Pointer to the used SceneGraphInterface
	static SceneGraphInterface* sgIF;

	/// Used to check if WDB is already initialized
	static bool initialized;

	/// Used to access WDB-internal functions
	static PrivateAccessor privateAccessor;

//*****************************************************************************
// Configuration loading
//*****************************************************************************
private:
	static XmlConfigurationLoader avatarXmlConfigLoader;
	static XmlConfigurationLoader worldXmlConfigLoader;
	static XmlConfigurationLoader entityTypeXmlConfigLoader;
	static XmlConfigurationLoader tileXmlConfigLoader;
	static XmlConfigurationLoader envLayoutXmlConfigLoader;
	static XmlConfigurationLoader environmentXmlConfigLoader;

	class AvatarConverterToV1_0a4 : public XmlConfigurationConverter {
	public:
		AvatarConverterToV1_0a4();
		virtual bool convert(XmlDocument* document, const Version& version, std::string configFile);
	}; // AvatarConverterToV1_0a4

	class WorldConverterToV1_0a4 : public XmlConfigurationConverter {
	public:
		WorldConverterToV1_0a4();
		virtual bool convert(XmlDocument* document, const Version& version, std::string configFile);
	}; // WorldConverterToV1_0a4

	class EntityTypeConverterToV1_0a4 : public XmlConfigurationConverter {
	public:
		EntityTypeConverterToV1_0a4();
		virtual bool convert(XmlDocument* document, const Version& version, std::string configFile);
	}; // EntityTypeConverterToV1_0a4

	class TileConverterToV1_0a4 : public XmlConfigurationConverter {
	public:
		TileConverterToV1_0a4();
		virtual bool convert(XmlDocument* document, const Version& version, std::string configFile);
	}; // TileConverterToV1_0a4

	class EnvLayoutConverterToV1_0a4 : public XmlConfigurationConverter {
	public:
		EnvLayoutConverterToV1_0a4();
		virtual bool convert(XmlDocument* document, const Version& version, std::string configFile);
	}; // EnvLayoutConverterToV1_0a4

	class EnvironmentConverterToV1_0a4 : public XmlConfigurationConverter {
	public:
		EnvironmentConverterToV1_0a4();
		virtual bool convert(XmlDocument* document, const Version& version, std::string configFile);
	}; // EnvironmentConverterToV1_0a4

};

#endif // _WORLDDATABASE_H
