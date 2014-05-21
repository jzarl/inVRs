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

#include "WorldDatabase.h"

#include <assert.h>

#include <sstream>
#include <memory>
#include <ctype.h>

#include <gmtl/AxisAngle.h>
#include <gmtl/MatrixOps.h>
#include <gmtl/Generate.h>

#include "EntityType.h"
#include "EntityTypeFactory.h"
#include "WorldDatabaseEvents.h"
#include "../XMLTools.h"
#include "../DebugOutput.h"
//#include "../SystemCore.h"
#include "../../OutputInterface/OutputInterface.h"
#include "../EventManager/EventManager.h"
#include "../UtilityFunctions.h"

// disable deprecation warning for std::auto_ptr when std::unique_ptr is not available.
#ifndef HAS_CXX11_UNIQUE_PTR
// Only do this on GCC versions that support it:
# if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 402
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
#endif

using namespace irr;
using namespace io;

XmlConfigurationLoader WorldDatabase::avatarXmlConfigLoader;
XmlConfigurationLoader WorldDatabase::worldXmlConfigLoader;
XmlConfigurationLoader WorldDatabase::entityTypeXmlConfigLoader;
XmlConfigurationLoader WorldDatabase::tileXmlConfigLoader;
XmlConfigurationLoader WorldDatabase::envLayoutXmlConfigLoader;
XmlConfigurationLoader WorldDatabase::environmentXmlConfigLoader;

std::string WorldDatabase::worldConfigFile;
std::string WorldDatabase::envLayoutConfigFile;

std::vector<Tile*> WorldDatabase::tileList;
std::vector<EntityType*> WorldDatabase::entityTypeList;
std::vector<Environment*> WorldDatabase::environmentList;
std::map<unsigned int, Tile*> WorldDatabase::tileMap;
std::map<unsigned short, EntityType*> WorldDatabase::entityTypeIDMap;
std::map<std::string, EntityType*> WorldDatabase::entityTypeNameMap;
std::map<unsigned short, Environment*> WorldDatabase::environmentMap;

std::vector<EntityTypeFactory*> WorldDatabase::entityTypeFactories;
std::vector<AvatarFactory*> WorldDatabase::avatarFactories;

std::vector<AvatarInterface*> WorldDatabase::avatarList;

int WorldDatabase::xSpacing;
int WorldDatabase::zSpacing;

SceneGraphInterface* WorldDatabase::sgIF;

bool WorldDatabase::initialized = false;

WorldDatabase::PrivateAccessor WorldDatabase::privateAccessor;


void WorldDatabase::PrivateAccessor::addEntityType(EntityType* entityType) const {
	WorldDatabase::addEntityType(entityType);
} // addEntityType

void WorldDatabase::PrivateAccessor::deleteEntityType(EntityType* entityType) const {
	WorldDatabase::deleteEntityType(entityType);
} // deleteEntityType

void WorldDatabase::init() {
	if (initialized) {
		printd(INFO,
				"WorldDatabase::init(): WorldDatabase is already initialized! Call will be ignored!\n");
		return;
	} // if

	registerAvatarFactory(new SimpleAvatarFactory);
	registerEntityTypeFactory(new EntityTypeFactory);

	EventManager::registerEventFactory("WorldDatabaseSyncEvent",
			new WorldDatabaseSyncEvent::Factory());
	EventManager::registerEventFactory("WorldDatabaseCreateEntityEvent",
			new WorldDatabaseCreateEntityEvent::Factory());
	EventManager::registerEventFactory("WorldDatabaseDestroyEntityEvent",
			new WorldDatabaseDestroyEntityEvent::Factory());
	EventManager::registerEventFactory("WorldDatabaseReloadEnvironmentsEvent",
			new WorldDatabaseReloadEnvironmentsEvent::Factory());
	EventManager::registerEventFactory("WorldDatabaseCreateEnvironmentEvent",
			new WorldDatabaseCreateEnvironmentEvent::Factory());

	avatarXmlConfigLoader.registerConverter(new AvatarConverterToV1_0a4);
	worldXmlConfigLoader.registerConverter(new WorldConverterToV1_0a4);
	entityTypeXmlConfigLoader.registerConverter(new EntityTypeConverterToV1_0a4);
	tileXmlConfigLoader.registerConverter(new TileConverterToV1_0a4);
	envLayoutXmlConfigLoader.registerConverter(new EnvLayoutConverterToV1_0a4);
	environmentXmlConfigLoader.registerConverter(new EnvironmentConverterToV1_0a4);

	initialized = true;
} // init

void WorldDatabase::cleanup() {
	int i;

	entityTypeIDMap.clear();
	entityTypeNameMap.clear();
	tileMap.clear();

	clearWorld();

	for (i = 0; i < (int)tileList.size(); i++)
		delete tileList[i];
	tileList.clear();

	for (i = 0; i < (int)entityTypeList.size(); i++)
		delete entityTypeList[i];
	entityTypeList.clear();

	for (i = 0; i < (int)avatarList.size(); i++)
		delete avatarList[i];
	avatarList.clear();

	for (i = 0; i < (int)avatarFactories.size(); i++)
		delete avatarFactories[i];
	avatarFactories.clear();

	for (i = 0; i < (int)entityTypeFactories.size(); i++)
		delete entityTypeFactories[i];
	entityTypeFactories.clear();
} // cleanup

void WorldDatabase::clearWorld() {
	int i;

	environmentMap.clear();
	for (i = 0; i < (int)environmentList.size(); i++) {
		// if(sgIF) sgIF->detachEnvironment(environmentList[i]); must be done in environment destructor (because scenegraph interface expects an environment without children!
		delete environmentList[i];
	} // for
	environmentList.clear();

	for (i = 0; i < (int)entityTypeList.size(); i++)
		entityTypeList[i]->clearInstances();
} // clearWorld

bool WorldDatabase::loadEntityTypes(std::string entityCfg) {
	if (!initialized) {
		printd(WARNING,
				"WorldDatabase::loadEntityTypes(): WorldDatabase not initialized yet - initializing now!\n");
		init();
	} // if

	if (!sgIF) {
		sgIF = OutputInterface::getSceneGraphInterface();
	} // if

	std::string configFileConcatenatedPath = entityCfg;
	if (!Configuration::containsPath("EntityTypeConfiguration") &&
			Configuration::containsPath("EntityConfiguration")) {
		printd(WARNING,
				"WorldDatabase::loadEntityTypes(): Deprecated path entry EntityConfiguration found in general configuration file! Use EntityTypeConfiguration instead!\n");
		configFileConcatenatedPath = getConcatenatedPath(entityCfg, "EntityConfiguration");
	} else {
		configFileConcatenatedPath = getConcatenatedPath(entityCfg, "EntityTypeConfiguration");
	} // else
	printd(INFO, "WorldDatabase::loadEntityTypes(): loading configuration %s!\n",
			entityCfg.c_str());

	// load xml document via xmlConfigLoader
	std::auto_ptr<const XmlDocument> document(
			entityTypeXmlConfigLoader.loadConfiguration(configFileConcatenatedPath));
	if (!document.get()) {
		printd(ERROR,
				"WorldDatabase::loadEntityTypes(): error at loading of configuration file %s!\n",
				entityCfg.c_str());
		return false;
	} // if

	bool success = true;
	int entityTypeId;
	EntityType* ent = NULL;
	EntityTypeFactory* factory = NULL;
	ModelInterface* model = NULL;

	std::vector<const XmlElement*> entityTypeElements = document->getElements("entityTypes.entityType");
	std::vector<const XmlElement*>::iterator it;
	for (it = entityTypeElements.begin(); it != entityTypeElements.end(); ++it) {
		if (!(*it)->hasAttribute("typeId")) {
			printd(ERROR,
					"WorldDatabase::loadEntityTypes(): missing attribute typeId in <entityType> element found in config file %s!\n",
					entityCfg.c_str());
			success = false;
			break;
		} // if
		entityTypeId = (*it)->getAttributeValueAsInt("typeId");
		if ((*it)->hasAttribute("implementationClass")) {
			ent = getEntityType((*it)->getAttributeValue("implementationClass"),
					(unsigned short)entityTypeId, &factory);
		} else {
			ent = getEntityType("EntityType", (unsigned short)entityTypeId, &factory);
		} // else

		if (!ent) {
			printd(ERROR,
					"WorldDatabase::loadEntityTypes(): could not create EntityType with ID %i!\n",
					entityTypeId);
			success = false;
			break;
		} // if
		ent->setName((*it)->getAttributeValue("name"));
		if ((*it)->hasAttribute("fixed")) {
			ent->setFixed((*it)->getAttributeValueAsInt("fixed"));
		} // if

		const XmlElement* representationElement = (*it)->getSubElement("representation");
		if (representationElement) {
			model = readRepresentationFromXmlElement(representationElement, OBJECTTYPE_ENTITY);
			ent->setModel(model);
		} // if

		const XmlElement* implementationClassElement = (*it)->getSubElement("implementationClass");
		if (implementationClassElement && implementationClassElement->hasSubElements()) {
			if (!factory) {
				printd(ERROR,
						"WorldDatabase::loadEntityTypes(): found entry for implementation class for entity type %i but no factory!\n",
						entityTypeId);
				success = false;
				break;
			} // if

			factory->parseXML(ent, implementationClassElement);
		} // if
		ent->setXmlFilename(entityCfg);
		addEntityType(ent);
	} // for

	return success;
} // loadEntityTypes

bool WorldDatabase::loadTiles(std::string tileCfg) {
	if (!initialized) {
		printd(WARNING,
				"WorldDatabase::loadTiles(): WorldDatabase not initialized yet - initializing now!\n");
		init();
	} // if

	if (!sgIF) {
		sgIF = OutputInterface::getSceneGraphInterface();
	} // if

	std::string configFileConcatenatedPath = getConcatenatedPath(tileCfg, "TileConfiguration");
	printd(INFO, "WorldDatabase::loadTiles(): loading configuration %s!\n",
			tileCfg.c_str());

	// load xml document via xmlConfigLoader
	std::auto_ptr<const XmlDocument> document(
			tileXmlConfigLoader.loadConfiguration(configFileConcatenatedPath));
	if (!document.get()) {
		printd(ERROR,
				"WorldDatabase::loadTiles(): error at loading of configuration file %s!\n",
				tileCfg.c_str());
		return false;
	} // if

	bool success = true;

	std::vector<const XmlElement*> tileElements = document->getElements("tiles.tile");
	std::vector<const XmlElement*>::iterator tileIt;
	Tile* newTile;
	const XmlElement* tileElement;
	const XmlElement* tileSize;
	const XmlElement* tileAdjustment;
	const XmlElement* representation;
	ModelInterface* model;

	for (tileIt = tileElements.begin(); tileIt != tileElements.end(); ++tileIt) {
		tileElement = *tileIt;
		tileSize = tileElement->getSubElement("tileProperties.size");
		tileAdjustment = tileElement->getSubElement("tileProperties.adjustment");
		representation = tileElement->getSubElement("representation");
		if (!tileElement->hasAttribute("id")) {
			printd(ERROR,
					"WorldDatabase::loadTiles(): invalid <tile> element without attribute id found! Please fix your tile configuration file!\n");
			success = false;
			break;
		} // if
		if (!tileSize) {
			printd(ERROR,
					"WorldDatabase::loadTiles(): invalid <tile> element (id = %i) without sub elements <tileProperties><size> found! Please fix your tile configuration file!\n",
					tileElement->getAttributeValueAsInt("id"));
			success = false;
			break;
		} // if
		if (!tileAdjustment) {
			printd(ERROR,
					"WorldDatabase::loadTiles(): invalid <tile> element (id = %i) without sub elements <tileProperties><adjustment> found! Please fix your tile configuration file!\n",
					tileElement->getAttributeValueAsInt("id"));
			success = false;
			break;
		} // if
		if (!representation) {
			printd(ERROR,
					"WorldDatabase::loadTiles(): invalid <tile> element (id = %i) without sub element <representation> found! Please fix your tile configuration file!\n",
					tileElement->getAttributeValueAsInt("id"));
			success = false;
			break;
		} // if
		newTile = new Tile;
		newTile->setId((unsigned)tileElement->getAttributeValueAsInt("id"));
		newTile->setName(tileElement->getAttributeValue("name"));
		newTile->setXSize(tileSize->getAttributeValueAsInt("xSize"));
		newTile->setZSize(tileSize->getAttributeValueAsInt("zSize"));
		newTile->setHeight(tileAdjustment->getAttributeValueAsFloat("height"));
		newTile->setYRotation(tileAdjustment->getAttributeValueAsInt("yRotation"));
		model = readRepresentationFromXmlElement(representation, OBJECTTYPE_TILE);
		newTile->setModel(model);
		newTile->setXmlFilename(tileCfg);
		tileList.push_back(newTile);
		tileMap[newTile->getId()] = newTile;
	} // for

	return success;
} // loadTiles

bool WorldDatabase::loadEnvironmentLayout(std::string envLayCfg) {
	envLayoutConfigFile = envLayCfg;
	return loadEnvironmentLayout(envLayCfg, false);
} // loadEnvironmentLayout

bool WorldDatabase::loadWorldDatabase(std::string worldCfg) {
	WorldDatabase::worldConfigFile = worldCfg;

	if (!initialized) {
		printd(WARNING,
				"WorldDatabase::loadWorldDatabase(): WorldDatabase not initialized yet - initializing now!\n");
		init();
	} // if

	if (!sgIF) {
		sgIF = OutputInterface::getSceneGraphInterface();
	} // if

	std::string configFileConcatenatedPath = getConcatenatedPath(worldCfg, "WorldConfiguration");
	printd(INFO, "WorldDatabase::loadWorldDatabase(): loading configuration %s!\n",
			worldCfg.c_str());

	// load xml document via xmlConfigLoader
	std::auto_ptr<const XmlDocument> document(
			worldXmlConfigLoader.loadConfiguration(configFileConcatenatedPath));
	if (!document.get()) {
		printd(ERROR,
				"WorldDatabase::loadWorldDatabase(): error at loading of configuration file %s!\n",
				worldCfg.c_str());
		return false;
	} // if

	// get all attributes
	std::vector<const XmlAttribute*> entityTypes =
		document->getAttributes("worldDatabase.entityTypes.configFile");
	std::vector<const XmlAttribute*> tiles =
		document->getAttributes("worldDatabase.tiles.configFile");
	const XmlAttribute* environmentLayout =
		document->getAttribute("worldDatabase.environmentLayout.configFile");

	bool success = true;

	std::vector<const XmlAttribute*>::iterator it;
	for (it = entityTypes.begin(); it != entityTypes.end(); ++it) {
		success = loadEntityTypes((*it)->getValue()) && success;
	} // for
	for (it = tiles.begin(); it != tiles.end(); ++it) {
		success = loadTiles((*it)->getValue()) && success;
	} // for

	if (success && environmentLayout) {
		success = loadEnvironmentLayout(environmentLayout->getValue()) && success;
	} // if

	return success;
} // loadWorldDatabase

std::vector<std::string> WorldDatabase::saveEntityTypes() {
	std::vector<std::string> entityTypeFilenames;
	std::map<std::string, XmlDocument*> entityTypeDocuments;

	std::vector<EntityType *>::iterator it;
	for (it = entityTypeList.begin(); it != entityTypeList.end(); ++it) {
		if (entityTypeDocuments.find((*it)->getXmlFilename()) == entityTypeDocuments.end()) {
			XmlElement *xmlElemRoot = new XmlElement("entityTypes");
			xmlElemRoot->addAttribute(new XmlAttribute("version", "1.0a4"));

			XmlDocument *xmlDoc = XmlDocument::createXmlDocument(xmlElemRoot);
			XmlDtdReference *xmlDtdRef = new XmlDtdReference("entityTypes",
				      "http://dtd.inVRs.org/entityTypes_v1.0a4.dtd");
			xmlDoc->setDtd(xmlDtdRef);
			entityTypeDocuments[(*it)->getXmlFilename()] = xmlDoc;
		} // if

		XmlDocument *xmlDoc = entityTypeDocuments.find((*it)->getXmlFilename())->second;
		if (xmlDoc == NULL) {
			printd(ERROR,
					    "WorldDatabase::saveEntityTypes(): xmlDoc is NULL\n");
			return entityTypeFilenames;
		} // if
		
		XmlElement *xmlElemEntityTypes = xmlDoc->getElement("entityTypes");
		if (xmlElemEntityTypes == NULL) {
			printd(ERROR,
					    "WorldDatabase::saveEntityType(): xmlElemEntityTypes is NULL\n");
			return entityTypeFilenames;
		} // if 

		XmlElement *xmlElemEntityType = new XmlElement("entityType");
		xmlElemEntityType->addAttribute(new XmlAttribute("typeId", toString((*it)->getId())));
		xmlElemEntityType->addAttribute(new XmlAttribute("name" , (*it)->getName()));
		if ((*it)->isFixed())
			xmlElemEntityType->addAttribute(new XmlAttribute("fixed", "1"));
		else
			xmlElemEntityType->addAttribute(new XmlAttribute("fixed", "0"));

		ModelInterface *modelInterface = (*it)->getModel();
		
	
		if (modelInterface != NULL) {
			XmlElement *xmlElemRepresentation = createXmlElementFromModelInterface(modelInterface);	
			xmlElemEntityType->addSubElement(xmlElemRepresentation);
		} // if
		xmlElemEntityTypes->addSubElement(xmlElemEntityType);
      } // for
	
	std::map<std::string, XmlDocument *>::iterator docIt;
	for (docIt = entityTypeDocuments.begin(); docIt != entityTypeDocuments.end(); ++docIt) {
		(*docIt).second->dumpToFile(Configuration::getPath("EntityTypeConfiguration") + (*docIt).first);
		entityTypeFilenames.push_back((*docIt).first);
	} // for
	return entityTypeFilenames;

} // saveEntityType

std::vector<std::string> WorldDatabase::saveTiles() {
	std::vector<std::string> tileFilenames;	
	std::map<std::string, XmlDocument*> tileDocuments;

	std::vector<Tile *>::iterator it;
	for (it = tileList.begin(); it != tileList.end(); ++it) {
		if (tileDocuments.find((*it)->getXmlFilename()) == tileDocuments.end()) {
			XmlElement *xmlElemRoot = new XmlElement("tiles");
			xmlElemRoot->addAttribute(new XmlAttribute("version", "1.0a4"));

			XmlDocument *xmlDoc = XmlDocument::createXmlDocument(xmlElemRoot);
			XmlDtdReference *xmlDtdRef = new XmlDtdReference("tiles",
				    "http://dtd.inVRs.org/tiles_v1.0a4.dtd");
			xmlDoc->setDtd(xmlDtdRef);
			tileDocuments[(*it)->getXmlFilename()] = xmlDoc;
		} // if

		XmlDocument *xmlDoc = tileDocuments.find((*it)->getXmlFilename())->second;
		if (xmlDoc == NULL) {
			printd(ERROR,
					    "WorldDatabase::saveTiles(): xmlDoc is NULL\n");
			return tileFilenames;
		} // if
		
		XmlElement *xmlElemTiles = xmlDoc->getElement("tiles");
		if (xmlElemTiles == NULL) {
			printd(ERROR,
					    "WorldDatabase::saveTiles(): xmlElemTiles is NULL\n");
			return tileFilenames;
		} // if
	
		XmlElement *xmlElemTile = new XmlElement("tile");
		xmlElemTile->addAttribute(new XmlAttribute("id", toString((*it)->getId())));
		xmlElemTile->addAttribute(new XmlAttribute("name", (*it)->getName()));

		XmlElement *xmlElemTileProperties = new XmlElement("tileProperties");
		
		XmlElement *xmlElemSize = new XmlElement("size");
		xmlElemSize->addAttribute(new XmlAttribute("xSize", toString((*it)->getXSize())));
		xmlElemSize->addAttribute(new XmlAttribute("zSize", toString((*it)->getZSize())));
		xmlElemTileProperties->addSubElement(xmlElemSize);
	      
		XmlElement *xmlElemAdjustment = new XmlElement("adjustment");
		xmlElemAdjustment->addAttribute(new XmlAttribute("height", toString((*it)->getHeight())));
		xmlElemAdjustment->addAttribute(new XmlAttribute("yRotation", toString((*it)->getYRotation())));
		xmlElemTileProperties->addSubElement(xmlElemAdjustment);
		xmlElemTile->addSubElement(xmlElemTileProperties);

		XmlElement *xmlElemRepresentation = new XmlElement("representation");
		ModelInterface *modelInterface = (*it)->getModel();

		if (modelInterface != NULL) {
			XmlElement *xmlElemRepresentation = createXmlElementFromModelInterface(modelInterface);
		} // if
		
		xmlElemTile->addSubElement(xmlElemRepresentation);
		xmlElemTiles->addSubElement(xmlElemTile);
	} // for

	std::map<std::string, XmlDocument *>::iterator docIt;
	for (docIt = tileDocuments.begin(); docIt != tileDocuments.end(); ++docIt) {
		(*docIt).second->dumpToFile(Configuration::getPath("TileConfiguration") + (*docIt).first);
		tileFilenames.push_back((*docIt).first);
	} // for
	return tileFilenames;
} // saveTiles

void WorldDatabase::saveEnvironmentLayout() {
	XmlElement *xmlElemEnvLayoutRoot = new XmlElement("environmentLayout");
	xmlElemEnvLayoutRoot->addAttribute(new XmlAttribute("version", "1.0a4"));

	XmlDocument *xmlDocEnvLayout = XmlDocument::createXmlDocument(xmlElemEnvLayoutRoot);
	XmlDtdReference *xmlDtdRefEnvLayout = new XmlDtdReference("environmentLayout",
			"http://dtd.inVRs.org/environmentLayout_v1.0a4.dtd");
	xmlDocEnvLayout->setDtd(xmlDtdRefEnvLayout);

	XmlElement *xmlElemTileGrid = new XmlElement("tileGrid");
	xmlElemTileGrid->addAttribute(new XmlAttribute("xSpacing", toString(getXSpacing())));
	xmlElemTileGrid->addAttribute(new XmlAttribute("zSpacing", toString(getZSpacing())));
	xmlElemEnvLayoutRoot->addSubElement(xmlElemTileGrid);

	std::vector<Environment *>::iterator it;
	for (it = environmentList.begin(); it != environmentList.end(); ++it) {
		XmlElement *xmlElemEnvRoot = new XmlElement("environment");
		xmlElemEnvRoot->addAttribute(new XmlAttribute("version", "1.0a4"));

		XmlDocument *xmlDocEnv = XmlDocument::createXmlDocument(xmlElemEnvRoot);
		XmlDtdReference *xmlDtdRefEnv = new XmlDtdReference("environment",
				"http://dtd.inVRs.org/environment_v1.0a4.dtd");
		xmlDocEnv->setDtd(xmlDtdRefEnv);
		
		XmlElement *xmlElemTileMap = new XmlElement("tileMap");
		xmlElemTileMap->addAttribute(new XmlAttribute("xDimension", toString((*it)->getXSize())));
		xmlElemTileMap->addAttribute(new XmlAttribute("zDimension", toString((*it)->getZSize())));
		
		//TODO create tileMap 
		xmlElemTileMap->setContent("1");  
		xmlElemEnvRoot->addSubElement(xmlElemTileMap);

		for(int i = 0; i < (*it)->getNumberOfStartTransformations(); ++i) {
			XmlElement *xmlElemEntryPoint = new XmlElement("entryPoint");
			xmlElemEntryPoint->addAttribute(new XmlAttribute("xPos", toString(
							(*it)->getStartTransformation(i).position[0])));
			xmlElemEntryPoint->addAttribute(new XmlAttribute("yPos", toString(
							(*it)->getStartTransformation(i).position[1])));
			xmlElemEntryPoint->addAttribute(new XmlAttribute("zPos", toString(
							(*it)->getStartTransformation(i).position[2])));
		
			gmtl::Vec3f direction;
			gmtl::Vec3f front(0, 0, -1);
			direction = (*it)->getStartTransformation(i).orientation * front;
			xmlElemEntryPoint->addAttribute(new XmlAttribute("xDir", toString(
							direction[0])));
			xmlElemEntryPoint->addAttribute(new XmlAttribute("yDir", toString(
							direction[1])));
			xmlElemEntryPoint->addAttribute(new XmlAttribute("zDir", toString(
							direction[2])));	
			
			xmlElemEnvRoot->addSubElement(xmlElemEntryPoint);
		} // for

		std::vector<Entity *> entities = (*it)->getEntityList();
		std::vector<Entity *>::iterator entityIt;
		for(entityIt = entities.begin(); entityIt != entities.end(); ++entityIt) {
			XmlElement *xmlElemEntity = new XmlElement("entity");
			
			unsigned short envId, entInEnvId;
			split((*entityIt)->getEnvironmentBasedId(), envId, entInEnvId);
			xmlElemEntity->addAttribute(new XmlAttribute("id", toString(entInEnvId)));
			xmlElemEntity->addAttribute(new XmlAttribute("typeId", toString(
							(*entityIt)->getEntityType()->getId())));
  
			XmlElement *xmlElemTransformation = createXmlElementFromTransformationData(
					(*entityIt)->getEnvironmentTransformation());

			xmlElemEntity->addSubElement(xmlElemTransformation);
			xmlElemEnvRoot->addSubElement(xmlElemEntity);
		} // for

		xmlDocEnv->dumpToFile(Configuration::getPath(
					"EnvironmentConfiguration") + (*it)->getXmlFilename());

		XmlElement *xmlElemEnvironment = new XmlElement("environment");
		xmlElemEnvironment->addAttribute(new XmlAttribute("id", toString((*it)->getId())));
		xmlElemEnvironment->addAttribute(new XmlAttribute("configFile", (*it)->getXmlFilename()));
		xmlElemEnvironment->addAttribute(new XmlAttribute("xLoc", toString((*it)->getXPosition())));
		xmlElemEnvironment->addAttribute(new XmlAttribute("zLoc", toString((*it)->getZPosition())));
		xmlElemEnvLayoutRoot->addSubElement(xmlElemEnvironment);
	} // for

	xmlDocEnvLayout->dumpToFile(Configuration::getPath("EnvironmentConfiguration") + envLayoutConfigFile);	

} // saveEnvironmentLayout

void WorldDatabase::saveWorldDatabase() {
	std::vector<std::string> entityTypeFilenames = saveEntityTypes();
	std::vector<std::string> tileFilenames = saveTiles();
	saveEnvironmentLayout();

	XmlElement *xmlElemRoot = new XmlElement("worldDatabase");
	xmlElemRoot->addAttribute(new XmlAttribute("version", "1.0a4"));

	XmlDocument *xmlDoc = XmlDocument::createXmlDocument(xmlElemRoot);
	XmlDtdReference *xmlDtdRef = new XmlDtdReference("worldDatabase",
			"http://dtd.inVRs.org/worldDatabase_v1.0a4.dtd");
	xmlDoc->setDtd(xmlDtdRef);

	std::vector<std::string>::iterator it;
	for (it = entityTypeFilenames.begin(); it != entityTypeFilenames.end(); ++it) {
		XmlElement *xmlElemEntityTypes = new XmlElement("entityTypes");
		xmlElemEntityTypes->addAttribute(new XmlAttribute("configFile", (*it)));
		xmlElemRoot->addSubElement(xmlElemEntityTypes);
	} // for

	for (it = tileFilenames.begin(); it != tileFilenames.end(); ++it) {
		XmlElement *xmlElemTiles = new XmlElement("tiles");
		xmlElemTiles->addAttribute(new XmlAttribute("configFile", (*it)));
		xmlElemRoot->addSubElement(xmlElemTiles);
	} // for
	
	XmlElement *xmlElemEnvironmentLayout = new XmlElement("environmentLayout");
	xmlElemEnvironmentLayout->addAttribute(new XmlAttribute("configFile", envLayoutConfigFile));
	xmlElemRoot->addSubElement(xmlElemEnvironmentLayout);

	xmlDoc->dumpToFile(Configuration::getPath("WorldConfiguration") + worldConfigFile);
} // saveWorldDatabase

EntityType* WorldDatabase::getEntityTypeWithId(unsigned short id) {
	EntityType* result = entityTypeIDMap[id];
	if (!result) {
		printd(WARNING,
				"WorldDatabase::getEntityTypeWithId(): -> EntityType with type %u not in WDB\n",
				(unsigned)id);
	} // if
	return result;
} // getEntityTypeWithId

EntityType* WorldDatabase::getEntityTypeWithName(std::string name) {
	EntityType* result = entityTypeNameMap[name];
	if (!result) {
		printd(WARNING,
				"WorldDatabase::getEntityTypeWithId(): -> EntityType with type %s not in WDB\n",
				name.c_str());
	} // if
	return result;
} // getEntityTypeWithId

Entity* WorldDatabase::getEntityWithEnvironmentId(unsigned environmentBasedId) {
	unsigned short environmentId, entityId;
	split(environmentBasedId, environmentId, entityId);
	return getEntityWithEnvironmentId(environmentId, entityId);
} // getEntityWithEnvironmentId

Entity* WorldDatabase::getEntityWithEnvironmentId(unsigned short environmentId,
		unsigned short entityId) {
	int i;
	unsigned int environmentBasedId;
	Entity* entity = NULL;
	Environment *env = environmentMap[environmentId];

	environmentBasedId = join(environmentId, entityId);

	if (env)
		entity = env->getEntityByEnvironmentBasedId(environmentBasedId);

	if (!entity) {
		for (i = 0; i < (int)environmentList.size(); i++) {
			if (environmentList[i] != env)
				entity = environmentList[i]->getEntityByEnvironmentBasedId(environmentBasedId);
			if (entity)
				break;
		} // for
	} // if

	if (!entity) {
		printd(
				WARNING,
				"WorldDatabase::getEntityWithEnvironmentId(): -> Entity with ID %i starting in Environment with ID %i not in WDB\n",
				entityId, environmentId);
	} // if
	return entity;
} // getEntityWithEnvironmentId

Entity* WorldDatabase::getEntityWithTypeInstanceId(unsigned typeInstanceId) {
	unsigned short typeId, instanceId;
	split(typeInstanceId, typeId, instanceId);
	return getEntityWithTypeInstanceId(typeId, instanceId);
} // getEntityWithTypeInstanceId

Entity* WorldDatabase::getEntityWithTypeInstanceId(unsigned short entityTypeId,
		unsigned short instanceId) {
	EntityType* type = getEntityTypeWithId(entityTypeId);
	if (!type) {
		printd(WARNING,
				"WorldDatabase::getEntityWithTypeInstanceId(): EntityType with ID %u unknown!\n",
				entityTypeId);
		return NULL;
	} // if
	Entity* ret = type->getEntityByInstanceId(instanceId);
	if (!ret) {
		printd(
				INFO,
				"WorldDatabase::getEntityWithTypeInstanceId(): cannot find a entity with type based id %i and instance based id %i\n",
				entityTypeId, instanceId);
	} // if
	return ret;
} // getEntityWithTypeInstanceId

Tile* WorldDatabase::getTileWithId(int id) {
	Tile* result = tileMap[id];
	if (!result) {
		printd(WARNING, "WorldDatabase::getTileWithId(): -> Tile with type %i not in WDB!\n", id);
	} // if
	return result;
} // getTileWithId

Environment* WorldDatabase::getEnvironmentWithId(unsigned short environmentId) {
	Environment* result = NULL;
	if (environmentMap.find(environmentId) != environmentMap.end()) {
		result = environmentMap[environmentId];
	}
	return result;
} // getEnvironmentWithId

Environment* WorldDatabase::getEnvironmentAtWorldPosition(float x, float z) {
	int i;
	Environment* env;
	float startX, startZ;

	for (i = 0; i < (int)environmentList.size(); i++) {
		env = environmentList[i];
		startX = env->getXPosition() * xSpacing;
		startZ = env->getZPosition() * zSpacing;
		if (startX <= x && startX + (env->getXSize() * env->getXSpacing()) > x && startZ <= z
				&& startZ + (env->getZSize() * env->getZSpacing()) > z)
			return env;
	} // for
	return NULL;
} // getEnvironmentAtWorldPosition

unsigned WorldDatabase::createEntity(unsigned short entityTypeId,
		unsigned short startEnvironmentId, TransformationData startTrans,
		AbstractEntityCreationCB* callback) {
	EntityType* entityType = entityTypeIDMap[entityTypeId];
	Environment* startEnvironment = environmentMap[startEnvironmentId];
	if (!entityType) {
		printd(WARNING, "WorldDatabase::createEntity(): EntityType with id %u not found in WDB!\n",
				entityTypeId);
		return 0;
	} // if
	if (!startEnvironment) {
		printd(WARNING,
				"WorldDatabase::createEntity(): Environment with id %u not found in WDB!\n",
				startEnvironmentId);
		return 0;
	} // if
	return createEntity(entityType, startEnvironment, startTrans, callback);
} // createEntity

unsigned WorldDatabase::createEntity(EntityType* type, Environment* startEnv,
		TransformationData startTrans, AbstractEntityCreationCB* callback) {
	return startEnv->createEntity(type, startTrans, callback);
} // createEntity


bool WorldDatabase::createEnvironment(unsigned short id, int xPosition, int zPosition, int xSize, int zSize)
{
	// Step 1: check if environmentID is already used
	if (environmentMap.find(id) != environmentMap.end()) {
		printd(ERROR, "WorldDatabase::createEnvironment(): environment with ID %u already exsiting!\n", id);
		return false;
	} // if

	// Step 2: check if xPosition and zPosition are valid
	bool invalidPosition = false;
	if (xPosition % xSpacing != 0) {
		printd(ERROR, "WorldDatabase::createEnvironment(): environment xPosition %i is not a multiple of the xSpacing (use %i or %i for example)!\n", xPosition, xPosition/xSpacing, (xPosition/xSpacing)+xSpacing);
		invalidPosition = true;
	} // if
	if (zPosition % zSpacing != 0) {
		printd(ERROR, "WorldDatabase::createEnvironment(): environment zPosition %i is not a multiple of the zSpacing (use %i or %i for example)!\n", zPosition, zPosition/zSpacing, (zPosition/zSpacing)+zSpacing);
		invalidPosition = true;
	} // if
	if (invalidPosition)
		return false;

	// Step 3: check if environment superimposes other environments
	gmtl::Vec2i minPoint(xPosition, zPosition);
	gmtl::Vec2i maxPoint(xPosition+xSize*xSpacing, zPosition+zSize*zSpacing);
	gmtl::Vec2i otherMinPoint, otherMaxPoint;
	std::vector<Environment*>::iterator it;
	for (it = environmentList.begin(); it != environmentList.end(); ++it)
	{
		otherMinPoint = gmtl::Vec2i((*it)->getXPosition(), (*it)->getZPosition());
		otherMaxPoint = gmtl::Vec2i((*it)->getXPosition() + (*it)->getXSize()*xSpacing, (*it)->getZPosition() + (*it)->getZSize()*zSpacing);
		// Just formulating it that way for simplicity
		if ((maxPoint[0] <= otherMinPoint[0]) ||
			(maxPoint[1] <= otherMinPoint[1]) ||
			(minPoint[0] >= otherMaxPoint[0]) ||
			(minPoint[1] >= otherMaxPoint[1]))
			continue;
		printd(ERROR, "WorldDatabase::createEnvironment(): Cannot add new environment because it overlaps environment with ID %u!\n", (*it)->getId());
		return false;
	} // for

	// Step 4: create Environment
	int gridXPosition = xPosition / xSpacing;
	int gridZPosition = zPosition / zSpacing;
	WorldDatabaseCreateEnvironmentEvent* event = new WorldDatabaseCreateEnvironmentEvent(id,
			gridXPosition, gridZPosition, xSize, zSize, WORLD_DATABASE_ID);
	EventManager::sendEvent(event, EventManager::EXECUTE_GLOBAL);
	return true;
} // createEnvironment

bool WorldDatabase::destroyEntity(Entity* entity, AbstractEntityDeletionCB* callback) {
	Environment* startEnv;
	unsigned short envId, entInEnvId;

	if (!entity) {
		printd(WARNING, "WorldDatabase::destroyEntity(): passed Entity is NULL! Ignoring!\n");
		// TODO: check if we have to delete callback
		if (callback)
			delete callback;
		return false;
	} // if

	split(entity->getEnvironmentBasedId(), envId, entInEnvId);

	startEnv = WorldDatabase::getEnvironmentWithId(envId);
	if (!startEnv) {
		printd(
				ERROR,
				"WorldDatabase::destroyEntity(): unable to get environment for deletion of entity with ID (env/#) %i %i\n",
				envId, entInEnvId);
		return false;
	} // if

	return startEnv->destroyEntity(entity, callback);
} // destroyEntity

bool WorldDatabase::unregisterEntityCreationCallback(unsigned environmentBasedId) {
	unsigned short environmentId, entityId;
	split(environmentBasedId, environmentId, entityId);

	Environment* environment = getEnvironmentWithId(environmentId);
	if (!environment) {
		printd(
				WARNING,
				"WorldDatabase::unregisterEntityCreationCallback(): Could not find Environment with ID %u where callback should be registered!\n",
				environmentId);
		return false;
	} // if
	return environment->unregisterEntityCreationCallback(environmentBasedId);
} // unregisterEntityCreationCallback

bool WorldDatabase::unregisterEntityDeletionCallback(unsigned environmentBasedId) {
	unsigned short environmentId, entityId;
	split(environmentBasedId, environmentId, entityId);

	Environment* environment = getEnvironmentWithId(environmentId);
	if (!environment) {
		printd(
				WARNING,
				"WorldDatabase::unregisterEntityDeletionCallback(): Could not find Environment with ID %u where callback should be registered!\n",
				environmentId);
		return false;
	} // if
	return environment->unregisterEntityDeletionCallback(environmentBasedId);
} // unregisterEntityDeletionCallback

void WorldDatabase::registerEntityTypeFactory(EntityTypeFactory* entityTypeFactory) {
	entityTypeFactories.push_back(entityTypeFactory);
} // registerEntityTypeFactory

void WorldDatabase::registerAvatarFactory(AvatarFactory* avatarFactory) {
	avatarFactories.push_back(avatarFactory);
} // registerAvatarFactory

AvatarInterface* WorldDatabase::loadAvatar(std::string configFile) {
	if (!sgIF) {
		sgIF = OutputInterface::getSceneGraphInterface();
	} // if

	int i;
	AvatarInterface* avatar = NULL;
	std::string avatarType;

	std::string configFileConcatenatedPath = getConcatenatedPath(configFile, "AvatarConfiguration");
	printd(INFO, "WorldDatabase::loadAvatar(): loading configuration %s\n", configFile.c_str());

	// load xml document via xmlConfigLoader
	std::auto_ptr<const XmlDocument> document(
			avatarXmlConfigLoader.loadConfiguration(configFileConcatenatedPath, false));
	if (!document.get()) {
		printd(ERROR,
				"WorldDatabase::loadAvatar(): error at loading of configuration file %s!\n",
				configFile.c_str());
		return 0;
	} // if

	avatarType = document->getRootElement()->getName();
	avatarType[0] = toupper(avatarType[0]);
	for (i = 0; i < (int)avatarFactories.size(); i++) {
		// We have to pass the file instead of the document, because we may need to update the
		// configuration versions
		avatar = avatarFactories[i]->create(avatarType, configFile);
		if (avatar)
			break;
	} // for

	if (avatar) {
		ModelInterface* avatarModel;

		if (sgIF) {
			avatarModel = avatar->getModel();
			sgIF->attachModel(avatarModel, IdentityTransformation);
		} else
			printd(ERROR, "WorldDatabase::loadAvatar(): No SceneGraphInterface found!\n");

		avatarList.push_back(avatar);
	} // if
	else {
		printd(ERROR, "WorldDatabase::loadAvatar(): Unable to create avatar of type %s!\n",
				avatarType.c_str());
	} // else

	return avatar;
} // loadAvatar

void WorldDatabase::removeAvatar(AvatarInterface* avatarInt) {
	int i;

	if (!avatarInt)
		return;

	if (sgIF) {
		ModelInterface* avatarModel = avatarInt->getModel();
		if (avatarModel == NULL) {
			printd(WARNING, "WorldDatabase::removeAvatar(): WARNING: no model found\n");
			return;
		} // if

		sgIF->detachModel(avatarModel);
	} // if

	for (i = 0; i < (int)avatarList.size(); i++) {
		if (avatarList[i] == avatarInt) {
			avatarList.erase(avatarList.begin() + i);
			break;
		} // if
	} // for

	// Deletion of the Avatar has to be done by caller (mainly User-destructor)
	// 	delete avatarInt;
} // removeAvatar

void WorldDatabase::updateAvatars(float dt) {
	int i;
	for (i = 0; i < (int)avatarList.size(); i++)
		avatarList[i]->update(dt);
} // updateAvatars

int WorldDatabase::getXSpacing() {
	return xSpacing;
} // getXSpacing

int WorldDatabase::getZSpacing() {
	return zSpacing;
} // getYSpacing

const std::vector<EntityType*>& WorldDatabase::getEntityTypeList() {
	return entityTypeList;
} // getEntityTypeList

const std::vector<Tile*>& WorldDatabase::getTileList() {
	return tileList;
} // getTileList

const std::vector<Environment*>& WorldDatabase::getEnvironmentList() {
	return environmentList;
} // getEnvironmentList

void WorldDatabase::dumpEntityTypes() {
	int i;
	printd(INFO, "--[dumpEntities]---------------------------------------\n");
	for (i = 0; i < (int)entityTypeList.size(); i++)
		entityTypeList[i]->dump();

	printd(INFO, "--[dumpEntities end]-----------------------------------\n");
} // dumpEntities

void WorldDatabase::dumpTiles() {
	int i;
	printd(INFO, "--[dumpTiles]------------------------------------------\n");
	for (i = 0; i < (int)tileList.size(); i++)
		tileList[i]->dump();

	printd(INFO, "--[dumpTiles end]---------------------------------------\n");
} // dumpTiles

void WorldDatabase::dumpEnvironment() {
	int i;
	printd(INFO, "--[dumpEnvironments]-------------------------------------\n");
	for (i = 0; i < (int)environmentList.size(); i++)
		environmentList[i]->dump();

	printd(INFO, "--[dumpEnvironments end]---------------------------------\n");
} // dumpEnvironment

void WorldDatabase::dump() {
	printd(INFO, "--[dumpWorldDB]----------------------------------------\n");
	dumpEntityTypes();
	dumpTiles();
	dumpEnvironment();
	printd(INFO, "--[dumpWorldDB end]------------------------------------\n");
} // dump

const WorldDatabase::PrivateAccessor& WorldDatabase::getPrivateAccessor() {
	return privateAccessor;
} // getPrivateAccessor

void WorldDatabase::reloadEnvironments() {
	WorldDatabaseReloadEnvironmentsEvent* event = new WorldDatabaseReloadEnvironmentsEvent(
			WORLD_DATABASE_ID);
	EventManager::sendEvent(event, EventManager::EXECUTE_LOCAL);
} // reloadEnvironments


bool WorldDatabase::loadEnvironmentLayout(std::string envLayCfg, bool reload) {
	if (!initialized) {
		printd(WARNING,
				"WorldDatabase::loadEnvironmentLayout(): WorldDatabase not initialized yet - initializing now!\n");
		init();
	} // if

	if (!sgIF) {
		sgIF = OutputInterface::getSceneGraphInterface();
	} // if

	std::string configFileConcatenatedPath = getConcatenatedPath(envLayCfg, "EnvironmentConfiguration");
	printd(INFO, "WorldDatabase::loadEnvironmentLayout(): loading configuration %s!\n",
			envLayCfg.c_str());

	// load xml document via xmlConfigLoader
	std::auto_ptr<const XmlDocument> document(
			envLayoutXmlConfigLoader.loadConfiguration(configFileConcatenatedPath));
	if (!document.get()) {
		printd(ERROR,
				"WorldDatabase::loadEnvironmentLayout(): error at loading of configuration file %s!\n",
				envLayCfg.c_str());
		return false;
	} // if

	bool success = true;

	// load tileGrid settings
	const XmlElement* tileGrid = document->getElement("environmentLayout.tileGrid");
	std::vector<const XmlElement*> envElements = document->getElements("environmentLayout.environment");

	if (!tileGrid || !tileGrid->hasAttribute("xSpacing") || !tileGrid->hasAttribute("zSpacing")) {
		printd(ERROR,
				"WorldDatabase::loadEnvironmentLayout(): missing or invalid <tileGrid> element found in file %s! Please fix your environmentLayout configuration file!\n",
				envLayCfg.c_str());
		return false;
	} // if
	xSpacing = tileGrid->getAttributeValueAsInt("xSpacing");
	zSpacing = tileGrid->getAttributeValueAsInt("zSpacing");

	// load environments
	std::string envConfigFilePath = Configuration::getPath("EnvironmentConfiguration");
	Environment* env;
	int id;
	std::string envConfigFileName;
	std::vector<const XmlElement*>::iterator envIt;
	for (envIt = envElements.begin(); envIt != envElements.end(); ++envIt) {
		if (!(*envIt)->hasAttribute("id") || !(*envIt)->hasAttribute("configFile") ||
				!(*envIt)->hasAttribute("xLoc") || !(*envIt)->hasAttribute("zLoc")) {
			printd(ERROR,
					"WorldDatabase::loadEnvironmentLayout(): invalid <environment> element with missing attribute(s) id/configFile/xLoc/zLoc found in file %s! Please fix your environmentLayout configuration file!\n",
					envLayCfg.c_str());
			success = false;
			continue;
		} // if
		id = (*envIt)->getAttributeValueAsInt("id");
		envConfigFileName = (*envIt)->getAttributeValue("configFile");
		env	= loadEnvironment(envConfigFilePath + envConfigFileName, xSpacing, zSpacing, id, reload);
		if (!env)
		{
			printd(ERROR,
					"WorldDatabase::loadEnvironmentLayout(): could not load environment. Please fix your environmentLayout configuration file %s!\n",
					envLayCfg.c_str());
			success = false;
			continue;
		}
		env->setXPosition((*envIt)->getAttributeValueAsInt("xLoc"));
		env->setZPosition((*envIt)->getAttributeValueAsInt("zLoc"));
		env->setXmlFilename(envConfigFileName);
		if (!reload) {
			environmentList.push_back(env);
			environmentMap[(unsigned short)id] = env;
		} // if
		printd(INFO,
				"WorldDatabase::loadEnvironment(): successfully created environment with ID %i\n",
				id);
	} // for

	return success;
} // loadEnvironmentLayout

Environment* WorldDatabase::loadEnvironment(std::string envCfg, int xSpacing, int zSpacing,
		int environmentId, bool reload) {
	if (!initialized) {
		printd(WARNING,
				"WorldDatabase::loadEnvironmentLayout(): WorldDatabase not initialized yet - initializing now!\n");
		init();
	} // if

	if (!sgIF) {
		sgIF = OutputInterface::getSceneGraphInterface();
	} // if

	printd(INFO, "WorldDatabase::loadEnvironment(): loading configuration %s!\n",
			envCfg.c_str());

	// load xml document via xmlConfigLoader
	std::auto_ptr<const XmlDocument> document(environmentXmlConfigLoader.loadConfiguration(envCfg));
	if (!document.get()) {
		printd(ERROR,
				"WorldDatabase::loadEnvironment(): error at loading of configuration file %s!\n",
				envCfg.c_str());
		return NULL;
	} // if

	bool success = true;
	Environment* result = NULL;

	// create environment with size defined in tileMap node
	const XmlElement* tileMapElement = document->getElement("environment.tileMap");
	if (!tileMapElement || !tileMapElement->hasContent() ||
			!tileMapElement->hasAttribute("xDimension") ||
			!tileMapElement->hasAttribute("zDimension")) {
		printd(ERROR,
				"WorldDatabase::loadEnvironment(): missing, invalid or empty <tileMap> element found in file %s! Please fix your environment configuration file!\n",
				envCfg.c_str());
		return NULL;
	} // if
	if (reload) {
		result = getEnvironmentWithId(environmentId);
	}
	else {
		int xSize = tileMapElement->getAttributeValueAsInt("xDimension");
		int zSize = tileMapElement->getAttributeValueAsInt("zDimension");
		result = new Environment(xSpacing, zSpacing, xSize, zSize, environmentId);
		if (sgIF)
			sgIF->attachEnvironment(result);
	} // if

	// load entry points
	std::vector<const XmlElement*> elements = document->getElements("environment.entryPoint");
	std::vector<const XmlElement*>::iterator elementIt;
	for (elementIt = elements.begin(); elementIt != elements.end(); ++elementIt) {
		if (!addEntryPointToEnvironment(*elementIt, result)) {
			printd(ERROR,
					"WorldDatabase::loadEnvironment(): Unable to add entry-point to Environment!\n");
			success = false;
		} // if
		else {
			if (sgIF)
				sgIF->updateEnvironment(result);
		} // else
	} // for

	// load entities
	elements = document->getElements("environment.entity");
	for (elementIt = elements.begin(); elementIt != elements.end(); ++elementIt) {
		if (!createEntityInEnvironment(*elementIt, result, reload)) {
			printd(ERROR,
					"WorldDatabase::loadEnvironment(): Unable to create Entity in Environment!\n");
			success = false;
		} // if
	} // for

	// load tile map
	if (!createTilesInEnvironment(tileMapElement, result)) {
		printd(ERROR,
				"WorldDatabase::loadEnvironment(): Unable to create Entity in Environment!\n");
		success = false;
	} // if

	if (!success && result) {
		delete result;
		result = NULL;
	} // if
	return result;
} // loadEnvironment

bool WorldDatabase::addEntryPointToEnvironment(const XmlElement* entryPointElement,
		Environment* env) {
	bool success = true;
	gmtl::Vec3f startTrans, startDir;

	startTrans[0] = entryPointElement->getAttributeValueAsFloat("xPos");
	startTrans[1] = entryPointElement->getAttributeValueAsFloat("yPos");
	startTrans[2] = entryPointElement->getAttributeValueAsFloat("zPos");
	startDir[0] = entryPointElement->getAttributeValueAsFloat("xDir");
	startDir[1] = entryPointElement->getAttributeValueAsFloat("yDir");
	startDir[2] = entryPointElement->getAttributeValueAsFloat("zDir");
	env->addEntryPoint(startTrans, startDir);

	return success;
} // addEntryPointToEnvironment

bool WorldDatabase::createEntityInEnvironment(const XmlElement* entityElement, Environment* env,
		bool reload) {
	bool success = true;
	int entityTypeId, entityId = -1;
	unsigned short environmentId;
	EntityType *entityType;
	Entity* entity;
	TransformationData entityTransformation = IdentityTransformation;
//	TransformationData entityTransformation = identityTransformation();

	// obtain ids
	entityTypeId = entityElement->getAttributeValueAsInt("typeId");
	entityId = entityElement->getAttributeValueAsInt("id");
	environmentId = env->getId();

	// parse entity transformation
	if (!entityElement->hasSubElement("transformation")) {
		printd(ERROR,
				"WorldDatabase::createEntityInEnvironment(): missing subnode <transformation> in node <entity>! Please fix your environment configuration file!\n");
		return false;
	} // if
	success = readTransformationDataFromXmlElement(entityTransformation,
			entityElement->getSubElement("transformation"));
	if (!success) {
		printd(ERROR,
				"WorldDatabase::createEntityInEnvironment(): error parsing <transformation> node! Please fix your environment configuration file!\n");
		return false;
	} // if

	// obtain pointer to EntityType
	entityType = entityTypeIDMap[entityTypeId];
	if (entityType == NULL) {
		printd(ERROR,
				"WorldDatabase::createEntityInEnvironment(): couldn't find entity with type id %i\n",
				entityTypeId);
		return false;
	} // if

	// delete already existing entity (if world is reloaded)
	if (reload) {
		Entity* tmpEntity = getEntityWithEnvironmentId(environmentId, entityId);
		if (tmpEntity) {
			printd(INFO,
					"WorldDatabase::createEntityInEnvironment(): removing already existing Entity from Environment %u with ID %u\n",
					environmentId, entityId);
			WorldDatabaseDestroyEntityEvent* destroyEvent =
					new WorldDatabaseDestroyEntityEvent(tmpEntity, WORLD_DATABASE_ID);
			EventManager::sendEvent(destroyEvent, EventManager::EXECUTE_REMOTE);
			Environment* currentEnv = tmpEntity->getEnvironment();
			currentEnv->removeEntity(tmpEntity);
			delete tmpEntity;
		} // if
	} // if

	// create new entity, set its transformation, and add it to the environment
	entity = entityType->createInstanceAtLoadTime(environmentId, entityId);
	entity->setEnvironmentTransformation(entityTransformation);
	if (!env->addNewEntity(entity)) {
		printd(ERROR,
				"WorldDatabase::loadEnvironment(): Could not add Entity with ID %u to Environment %u\n",
				entityId, env->getId());
		return false;
	} // if
	else {
		printd(INFO,
				"WorldDatabase::loadEnvironment(): Added Entity with ID %u to Environment %u\n",
				entityId, env->getId());
	} // else

	// send a createEntity event to all remote users if the world was reloaded
	if (reload) {
		WorldDatabaseCreateEntityEvent* createEvent =
				new WorldDatabaseCreateEntityEvent(entity->getTypeBasedId(),
						entity->getEnvironmentBasedId(),
						entity->getEnvironmentTransformation(), WORLD_DATABASE_ID);
		EventManager::sendEvent(createEvent, EventManager::EXECUTE_REMOTE);
	} // if

	return success;
} // createEntityInEnvironment

bool WorldDatabase::createTilesInEnvironment(const XmlElement* tileMapElement, Environment* env) {
	bool success = true;
	std::stringstream ss(std::stringstream::in | std::stringstream::out);
	Tile* tile;

	ss << tileMapElement->getContent();
	for (int z = 0; z < env->getZSize(); z++) {
		for (int x = 0; x < env->getXSize(); x++) {
			int id = 0;
			ss >> id;
			if (id > 0) {
				tile = getTileWithId(id);
				if (tile) {
					env->setTileAtGridPosition(x, z, tile->clone());
				} // if
				else {
					printd(ERROR,
							"WorldDatabase::createTilesInEnvironment(): Unable to find tile with ID %i! Please check your tile and environment configuration files!\n",
							id);
					success = false;
				} // else
			} // if
			else if (id < 0) {
				printd(ERROR,
						"WorldDatabase::createTilesInEnvironment(): Invalid tile ID %i found in <tileMap> element! Please fix your environment configuration file!\n",
						id);
				success = false;
			} // else
		} // for
	} // for

	return success;
} // createTilesInEnvironment

EntityType* WorldDatabase::getEntityType(std::string className, unsigned short id,
		EntityTypeFactory** factory) {
	int i;
	EntityType* result = NULL;

	for (i = 0; i < (int)entityTypeFactories.size(); i++) {
		result = entityTypeFactories[i]->create(className, id);
		if (result) {
			if (factory)
				*factory = entityTypeFactories[i];
			return result;
		} // if
	} // for

	if (className != "EntityType") {
		printd(WARNING,
				"WorldDatabase::getEntityType(): couldn't find classFactory for entityTypeClass %s! Using default entityType instead!\n",
				className.c_str());
		result = getEntityType("EntityType", id, factory);
	} // if
	else {
		printd(ERROR,
				"WorldDatabase::getEntityType(): couldn't find classFactory for default entityTypeClass %s!\n",
				className.c_str());
		result = NULL;
		if (factory)
			*factory = NULL;
	} // else

	return result;
} // getEntityType

void WorldDatabase::executeEnvironmentReload() {
	loadEnvironmentLayout(envLayoutConfigFile, true);
} // executeEnvironmentReload

void WorldDatabase::addEntityType(EntityType* entityType) {
	assert(entityType);
	entityTypeList.push_back(entityType);
	entityTypeIDMap[entityType->getId()] = entityType;
	entityTypeNameMap[entityType->getName()] = entityType;
	printd(INFO, "WorldDatabase::addEntityType(): successfully added EntityType %s\n",
			entityType->getName().c_str());
} // addEntityType

void WorldDatabase::deleteEntityType(EntityType* entityType) {
	int i;
	std::vector<Entity* > entities;

	assert(entityType);
	
	entities = entityType->getInstanceList();
	for(i = 0; i < entities.size(); i++) {
		destroyEntity(entities[i]);	
	} // for

	for (i = 0; i < entityTypeList.size(); i++) {
		if (entityTypeList[i] == entityType) {	
			entityTypeList.erase(entityTypeList.begin() + i);
			break;
		} // if
	} // for
} // deleteEntityType


void WorldDatabase::addNewEnvironment(Environment* environment)
{
	assert(environmentMap[environment->getId()] == NULL);
	environmentMap[environment->getId()] = environment;
	environmentList.push_back(environment);
} // addNewEnvironment

//*****************************************************************************
// Configuration loading
//*****************************************************************************
WorldDatabase::AvatarConverterToV1_0a4::AvatarConverterToV1_0a4() :
	XmlConfigurationConverter("0.0", "1.0a4") {
} // AvatarConverterToV1_04

bool WorldDatabase::AvatarConverterToV1_0a4::convert(XmlDocument* document, const Version& version,
		std::string configFile) {
	if (document->getRootElement()->getName() != "avatar" &&
			document->getRootElement()->getName() != "config") {
		printd(ERROR,
				"AvatarConverterToV1_0a4::convert(): could not convert file because of invalid root node <%s> in Avatar configuration file! Please upgrade to a current xml file version!\n",
				document->getRootElement()->getName().c_str());
		return false;
	} // if

	XmlElement* avatarElement = NULL;

	// replace root node (if existing) and make <avatar> node as root
	if (document->getRootElement()->getName() != "avatar") {
		avatarElement = document->getRootElement()->getSubElement("avatar");
		if (!avatarElement) {
			printd(ERROR,
					"AvatarConverterToV1_0a4::convert(): could not convert file because of misssing node <avatar>! Please upgrade to a current xml file version!\n");
			return false;
		} // if
		document->getRootElement()->removeSubElement(avatarElement);
		XmlElement* oldRootElement = document->replaceRootElement(avatarElement);
		delete oldRootElement;
	} // if
	else {
		avatarElement = document->getRootElement();
	} // else

// NOT NEEDED YET, MAYBE WE CAN ADD THIS LATER IN ORDER TO ALLOW AUTOMATIC TRANSFORMATION
// OF CONFIG FILES TO NEWER VERSIONS
//	// enter correct version number in root node
//	XmlAttribute* versionAttribute = new XmlAttribute("version", "1.0a4");
//	document->getRootElement()->deleteAllAttributes();
//	document->getRootElement()->addAttribute(versionAttribute);

	// replace avatar node with type based node
	// get type of avatar and use it as new node name, afterwards delete all attributes
	std::string avatarType = avatarElement->getAttributeValue("type");
	avatarElement->setName(avatarType);
	avatarElement->deleteAllAttributes();

	// add version attribute to element
	XmlAttribute* versionAttribute = new XmlAttribute("version", "1.0a4");
	avatarElement->addAttribute(versionAttribute);

	return true;
} // convert

WorldDatabase::WorldConverterToV1_0a4::WorldConverterToV1_0a4() :
	XmlConfigurationConverter("0.0", "1.0a4") {
} // WorldConverterToV1_0a4

bool WorldDatabase::WorldConverterToV1_0a4::convert(XmlDocument* document, const Version& version,
		std::string configFile) {
	bool success = true;

	updateDocument(document, XmlConfigurationLoader::XmlDtdUrl + "worldDatabase_v1.0a4.dtd",
			destinationVersion, "worldDatabase");

	// rename elements and attributes
	document->renameElements("worldDatabase.entities", "entityTypes");
	document->renameAttributes("worldDatabase.entityTypes.file", "configFile");
	document->renameAttributes("worldDatabase.tiles.file", "configFile");
	document->renameAttributes("worldDatabase.environmentLayout.file", "configFile");

	return success;
} // convert

WorldDatabase::EntityTypeConverterToV1_0a4::EntityTypeConverterToV1_0a4() :
	XmlConfigurationConverter("0.0", "1.0a4") {
} // EntityTypeConverterToV1_0a4

bool WorldDatabase::EntityTypeConverterToV1_0a4::convert(XmlDocument* document,
		const Version& version, std::string configFile) {
	bool success = true;

	updateDocument(document, XmlConfigurationLoader::XmlDtdUrl + "entityTypes_v1.0a4.dtd",
			destinationVersion, "entityTypes");

	// update transformation nodes
	std::vector<XmlElement*> transformationNodes =
		document->getElements("entityTypes.entityType.representation.transformation");

	std::vector<XmlElement*>::iterator it;
	for (it = transformationNodes.begin(); it != transformationNodes.end(); ++it) {
		success = XmlTransformationLoader::get().updateXmlElement(*it, version, destinationVersion,
				configFile) && success;
	} // for

	//TODO: update implementationclass elements

	return success;
} // convert

WorldDatabase::TileConverterToV1_0a4::TileConverterToV1_0a4() :
	XmlConfigurationConverter("0.0", "1.0a4") {
} // TileConverterToV1_0a4

bool WorldDatabase::TileConverterToV1_0a4::convert(XmlDocument* document, const Version& version,
		std::string configFile) {
	bool success = true;

	updateDocument(document, XmlConfigurationLoader::XmlDtdUrl + "tiles_v1.0a4.dtd",
			destinationVersion, "tiles");

	document->renameAttributes("tiles.tile.typeId", "id");

	// update transformation nodes
	std::vector<XmlElement*> transformationNodes =
		document->getElements("tiles.tile.representation.transformation");

	std::vector<XmlElement*>::iterator it;
	for (it = transformationNodes.begin(); it != transformationNodes.end(); ++it) {
		success = XmlTransformationLoader::get().updateXmlElement(*it, version, destinationVersion,
				configFile) && success;
	} // for

	return success;
} // convert

WorldDatabase::EnvLayoutConverterToV1_0a4::EnvLayoutConverterToV1_0a4() :
	XmlConfigurationConverter("0.0", "1.0a4") {
} // EnvLayoutConverterToV1_0a4

bool WorldDatabase::EnvLayoutConverterToV1_0a4::convert(XmlDocument* document,
		const Version& version, std::string configFile) {
	bool success = true;

	updateDocument(document, XmlConfigurationLoader::XmlDtdUrl + "environmentLayout_v1.0a4.dtd",
			destinationVersion, "environmentLayout");

	document->renameElements("environmentLayout.map", "environment");
	document->renameAttributes("environmentLayout.environment.name", "configFile");

	return success;
} // convert

WorldDatabase::EnvironmentConverterToV1_0a4::EnvironmentConverterToV1_0a4() :
	XmlConfigurationConverter("0.0", "1.0a4") {
} // EnvironmentConverterToV1_0a4

bool WorldDatabase::EnvironmentConverterToV1_0a4::convert(XmlDocument* document,
		const Version& version, std::string configFile) {
	bool success = true;

	updateDocument(document, XmlConfigurationLoader::XmlDtdUrl + "environment_v1.0a4.dtd",
			destinationVersion, "environment");

	document->renameElements("environment.map", "tileMap");

	std::vector<XmlElement*> transformationElements =
		document->getElements("environment.entity.transformation");

	std::vector<XmlElement*>::iterator it;
	for (it = transformationElements.begin(); it != transformationElements.end(); ++it) {
		success = XmlTransformationLoader::get().updateXmlElement(*it, version, destinationVersion,
				configFile) && success;
	} // for

	return success;
} // convert

