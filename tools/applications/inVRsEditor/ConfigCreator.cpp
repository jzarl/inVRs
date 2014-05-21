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

#include "ConfigCreator.h"

#include <inVRs/SystemCore/XmlElement.h>
#include <inVRs/SystemCore/XmlDocument.h>

#if WIN32
# include <direct.h>
# define MKDIR(DIR) _mkdir((DIR).c_str())
#else
# include <sys/stat.h>
# include <sys/types.h>
# define MKDIR(DIR) mkdir((DIR).c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)
#endif

void ConfigCreator::createBasicConfig(std::string invrsPath, std::string projectPath) {
	MKDIR(projectPath + "/config");
	MKDIR(projectPath + "/config/systemcore");
	MKDIR(projectPath + "/config/systemcore/worlddatabase");
	MKDIR(projectPath + "/config/systemcore/worlddatabase/entity");
	MKDIR(projectPath + "/config/systemcore/worlddatabase/environment");
	MKDIR(projectPath + "/config/systemcore/worlddatabase/tile");
	MKDIR(projectPath + "/config/outputinterface");
	MKDIR(projectPath + "/config/systemcore/transformationmanager");
	MKDIR(projectPath + "/config/systemcore/userdatabase");

	MKDIR(projectPath + "/models");
	MKDIR(projectPath + "/models/tiles");
	MKDIR(projectPath + "/models/entities");
	MKDIR(projectPath + "/models/skybox");
	MKDIR(projectPath + "/models/highlighters");
	MKDIR(projectPath + "/models/avatars");
	MKDIR(projectPath + "/models/heightmaps");
	MKDIR(projectPath + "/models/collisionmaps");
	MKDIR(projectPath + "/models/cursors");

	createGeneralXml(projectPath + "/config/general.xml", invrsPath);
	createSystemCoreXml(projectPath + "/config/systemcore/systemCore.xml");
	createWorldDatabaseXml(projectPath + "/config/systemcore/worlddatabase/worldDatabase.xml");
	createEntitiesXml(projectPath + "/config/systemcore/worlddatabase/entity/entities.xml");
	createEnvironmentXml(projectPath + "/config/systemcore/worlddatabase/environment/environment.xml");
	createEnvironmentLayoutXml(projectPath + "/config/systemcore/worlddatabase/environment/environmentLayout.xml");
	createTilesXml(projectPath + "/config/systemcore/worlddatabase/tile/tiles.xml");
	createOutputInterfaceXml(projectPath + "/config/outputinterface/outputInterface.xml");
	createModifiersXml(projectPath + "/config/systemcore/transformationmanager/modifiers.xml");
	createUserDatabaseXml(projectPath + "/config/systemcore/userdatabase/userDatabase.xml");
}

void ConfigCreator::createGeneralXml(std::string path, std::string invrsPath) {
	XmlElement *xmlElemRoot = new XmlElement("generalConfig");
	xmlElemRoot->addAttribute(new XmlAttribute("version", "1.0a4"));

	XmlElement *xmlElemGeneral = new XmlElement("general");
	XmlElement *xmlElemInterfaces = new XmlElement("Interfaces");
	XmlElement *xmlElemOption = new XmlElement("option");
	xmlElemOption->addAttribute(new XmlAttribute("key", "outputInterfaceConfiguration"));
	xmlElemOption->addAttribute(new XmlAttribute("value", "outputInterface.xml"));
	xmlElemInterfaces->addSubElement(xmlElemOption);
	xmlElemGeneral->addSubElement(xmlElemInterfaces);

	XmlElement *xmlElemSystemCore = new XmlElement("SystemCore");
	xmlElemOption = new XmlElement("option");
	xmlElemOption->addAttribute(new XmlAttribute("key", "systemCoreConfiguration"));
	xmlElemOption->addAttribute(new XmlAttribute("value", "systemCore.xml"));
	xmlElemSystemCore->addSubElement(xmlElemOption);
	xmlElemGeneral->addSubElement(xmlElemSystemCore);

	XmlElement *xmlElemPaths = new XmlElement("paths");
	XmlElement *xmlElemPathsRoot = new XmlElement("root");
	xmlElemPathsRoot->addAttribute(new XmlAttribute("directory", "./"));
	xmlElemPaths->addSubElement(xmlElemPathsRoot);

	xmlElemPaths->addSubElement(createXmlElemPath("Plugins", invrsPath));

	xmlElemPaths->addSubElement(createXmlElemPath("SystemCoreConfiguration", "config/systemcore"));

	xmlElemPaths->addSubElement(createXmlElemPath("OutputInterfaceConfiguration", "config/outputinterface/"));

	xmlElemPaths->addSubElement(createXmlElemPath("WorldConfiguration", "config/systemcore/worlddatabase/"));
	xmlElemPaths->addSubElement(createXmlElemPath("EnvironmentConfiguration", "config/systemcore/worlddatabase/environment/"));
	xmlElemPaths->addSubElement(createXmlElemPath("EntityTypeConfiguration", "config/systemcore/worlddatabase/entity/"));
	xmlElemPaths->addSubElement(createXmlElemPath("TileConfiguration", "config/systemcore/worlddatabase/tile/"));

	xmlElemPaths->addSubElement(createXmlElemPath("TransformationManagerConfiguration", "config/systemcore/transformationmanager/"));

	xmlElemPaths->addSubElement(createXmlElemPath("UserConfiguration", "config/systemcore/userdatabase/"));

	xmlElemPaths->addSubElement(createXmlElemPath("Models", "models/"));
	xmlElemPaths->addSubElement(createXmlElemPath("Tiles", "models/tiles/"));
	xmlElemPaths->addSubElement(createXmlElemPath("Entities", "models/entities/"));
	xmlElemPaths->addSubElement(createXmlElemPath("Skybox", "models/skybox/"));
	xmlElemPaths->addSubElement(createXmlElemPath("Highlighters", "models/highlighters/"));
	xmlElemPaths->addSubElement(createXmlElemPath("Avatars", "models/avatars/"));
	xmlElemPaths->addSubElement(createXmlElemPath("HeightMaps", "models/heightmaps/"));
	xmlElemPaths->addSubElement(createXmlElemPath("CollisionMaps", "models/collisionmaps/"));
	xmlElemPaths->addSubElement(createXmlElemPath("Cursors", "models/cursors/"));

	xmlElemRoot->addSubElement(xmlElemGeneral);
	xmlElemRoot->addSubElement(xmlElemPaths);

	XmlDocument *xmlDoc = XmlDocument::createXmlDocument(xmlElemRoot);
	xmlDoc->setDtd(new XmlDtdReference("generalConfig", "http://dtd.inVRs.org/generalConfig_v1.0a4.dtd"));
	xmlDoc->dumpToFile(path);
} // createGeneralXml

void ConfigCreator::createSystemCoreXml(std::string path) {

	XmlElement *xmlElemRoot = new XmlElement("systemCore");
	xmlElemRoot->addAttribute(new XmlAttribute("version", "1.0a4"));

	XmlElement *xmlElemWorldDatabase = new XmlElement("worldDatabase");
	xmlElemWorldDatabase->addAttribute(new XmlAttribute("configFile", "worldDatabase.xml"));
	XmlElement *xmlElemUserDatabase = new XmlElement("userDatabase");
	xmlElemUserDatabase->addAttribute(new XmlAttribute("configFile", "userDatabase.xml"));
	XmlElement *xmlElemTransformationManager = new XmlElement("transformationManager");
	xmlElemTransformationManager->addAttribute(new XmlAttribute("configFile", "modifiers.xml"));

	xmlElemRoot->addSubElement(xmlElemWorldDatabase);
	xmlElemRoot->addSubElement(xmlElemUserDatabase);
	xmlElemRoot->addSubElement(xmlElemTransformationManager);

	XmlDocument *xmlDoc = XmlDocument::createXmlDocument(xmlElemRoot);
	xmlDoc->setDtd(new XmlDtdReference("systemCore", "http://dtd.inVRs.org/systemCore_v1.0a4.dtd"));
	xmlDoc->dumpToFile(path);
} // createSystemCoreXml

void ConfigCreator::createWorldDatabaseXml(std::string path) {
	XmlElement *xmlElemRoot = new XmlElement("worldDatabase");
	xmlElemRoot->addAttribute(new XmlAttribute("version", "1.0a4"));

	XmlElement *xmlElemEntityTypes = new XmlElement("entityTypes");
	xmlElemEntityTypes->addAttribute(new XmlAttribute("configFile", "entities.xml"));
	XmlElement *xmlElemTiles = new XmlElement("tiles");
	xmlElemTiles->addAttribute(new XmlAttribute("configFile", "tiles.xml"));
	XmlElement *xmlElemEnvironmentLayout = new XmlElement("environmentLayout");
	xmlElemEnvironmentLayout->addAttribute(new XmlAttribute("configFile", "environmentLayout.xml"));

	xmlElemRoot->addSubElement(xmlElemEntityTypes);
	xmlElemRoot->addSubElement(xmlElemTiles);
	xmlElemRoot->addSubElement(xmlElemEnvironmentLayout);

	XmlDocument *xmlDoc = XmlDocument::createXmlDocument(xmlElemRoot);
	xmlDoc->setDtd(new XmlDtdReference("worldDatabase", "http://dtd.inVRs.org/worldDatabase_v1.0a4.dtd"));
	xmlDoc->dumpToFile(path);
} // createWorldDatabaseXml

void ConfigCreator::createEntitiesXml(std::string path) {
	XmlElement *xmlElemRoot = new XmlElement("entityTypes");
	xmlElemRoot->addAttribute(new XmlAttribute("version", "1.0a4"));

	XmlDocument *xmlDoc = XmlDocument::createXmlDocument(xmlElemRoot);
	xmlDoc->setDtd(new XmlDtdReference("entityTypes", "http://dtd.inVRs.org/entityTypes_v1.0a4.dtd"));
	xmlDoc->dumpToFile(path);
} // createEntitiesXml

void ConfigCreator::createEnvironmentXml(std::string path) {
	XmlElement *xmlElemRoot = new XmlElement("environment");
	xmlElemRoot->addAttribute(new XmlAttribute("version", "1.0a4"));

	XmlElement *xmlElemTileMap = new XmlElement("tileMap");
	xmlElemTileMap->addAttribute(new XmlAttribute("xDimension", "1"));
	xmlElemTileMap->addAttribute(new XmlAttribute("zDimension", "1"));
	xmlElemTileMap->setContent("1");
	xmlElemRoot->addSubElement(xmlElemTileMap);

	XmlDocument *xmlDoc = XmlDocument::createXmlDocument(xmlElemRoot);
	xmlDoc->setDtd(new XmlDtdReference("environment", "http://dtd.inVRs.org/environment_v1.0a4.dtd"));
	xmlDoc->dumpToFile(path);
} // createEnvironmentXml

void ConfigCreator::createEnvironmentLayoutXml(std::string path) {
	XmlElement *xmlElemRoot = new XmlElement("environmentLayout");
	xmlElemRoot->addAttribute(new XmlAttribute("version", "1.0a4"));

	XmlElement *xmlElemTileGrid = new XmlElement("tileGrid");
	xmlElemTileGrid->addAttribute(new XmlAttribute("xSpacing", "100"));
	xmlElemTileGrid->addAttribute(new XmlAttribute("zSpacing", "100"));
	xmlElemRoot->addSubElement(xmlElemTileGrid);

	XmlElement *xmlElemEnvironment = new XmlElement("environment");
	xmlElemEnvironment->addAttribute(new XmlAttribute("id", "1"));
	xmlElemEnvironment->addAttribute(new XmlAttribute("configFile", "environment.xml"));
	xmlElemEnvironment->addAttribute(new XmlAttribute("xLoc", "0"));
	xmlElemEnvironment->addAttribute(new XmlAttribute("zLoc", "0"));
	xmlElemRoot->addSubElement(xmlElemEnvironment);

	XmlDocument *xmlDoc = XmlDocument::createXmlDocument(xmlElemRoot);
	xmlDoc->setDtd(new XmlDtdReference("environmentLayout", "http://dtd.inVRs.org/environmentLayout_v1.0a4.dtd"));
	xmlDoc->dumpToFile(path);
} // createEnvironmentLayoutXml

void ConfigCreator::createTilesXml(std::string path) {
	XmlElement *xmlElemRoot = new XmlElement("tiles");
	xmlElemRoot->addAttribute(new XmlAttribute("version", "1.0a4"));

	XmlElement *xmlElemTile = new XmlElement("tile");
	xmlElemTile->addAttribute(new XmlAttribute("id", "1"));
	xmlElemTile->addAttribute(new XmlAttribute("name", ""));

	XmlElement *xmlElemTileProperties = new XmlElement("tileProperties");
	XmlElement *xmlElemSize = new XmlElement("size");
	xmlElemSize->addAttribute(new XmlAttribute("xSize", "100"));
	xmlElemSize->addAttribute(new XmlAttribute("zSize", "100"));
	xmlElemTileProperties->addSubElement(xmlElemSize);

	XmlElement *xmlElemAdjustment = new XmlElement("adjustment");
	xmlElemAdjustment->addAttribute(new XmlAttribute("height", "0"));
	xmlElemAdjustment->addAttribute(new XmlAttribute("yRotation", "0"));
	xmlElemTileProperties->addSubElement(xmlElemAdjustment);
	xmlElemTile->addSubElement(xmlElemTileProperties);

	XmlElement *xmlElemRepresentation = new XmlElement("representation");
	xmlElemTile->addSubElement(xmlElemRepresentation);
	xmlElemRoot->addSubElement(xmlElemTile);

	XmlDocument *xmlDoc = XmlDocument::createXmlDocument(xmlElemRoot);
	xmlDoc->setDtd(new XmlDtdReference("tiles", "http://dtd.inVRs.org/tiles_v1.0a4.dtd"));
	xmlDoc->dumpToFile(path);
} // createTilesXml

void ConfigCreator::createOutputInterfaceXml(std::string path) {
	XmlElement *xmlElemRoot = new XmlElement("outputInterface");
	xmlElemRoot->addAttribute(new XmlAttribute("version", "1.0a4"));

	XmlElement *xmlElemModule = new XmlElement("module");
	xmlElemModule->addAttribute(new XmlAttribute("name", "OpenSGSceneGraphInterface"));
	xmlElemModule->addAttribute(new XmlAttribute("configFile", ""));
	xmlElemRoot->addSubElement(xmlElemModule);

	XmlDocument *xmlDoc = XmlDocument::createXmlDocument(xmlElemRoot);
	xmlDoc->setDtd(new XmlDtdReference("outputInterface", "http://dtd.inVRs.org/outputInterface_v1.0a4.dtd"));
	xmlDoc->dumpToFile(path);
} // createOutputInterfaceXml

void ConfigCreator::createModifiersXml(std::string path) {
	XmlElement *xmlElemRoot = new XmlElement("transformationManager");
	xmlElemRoot->addAttribute(new XmlAttribute("version", "1.0a4"));

	XmlElement *xmlElemMergerList = new XmlElement("mergerList");
	xmlElemRoot->addSubElement(xmlElemMergerList);

	XmlElement *xmlElemPipeList = new XmlElement("pipeList");
	xmlElemPipeList->addSubElement(xmlElemPipeList);

	XmlDocument *xmlDoc = XmlDocument::createXmlDocument(xmlElemRoot);
	xmlDoc->setDtd(new XmlDtdReference("transformationManager", "http://dtd.inVRs.org/transformationManager_v1.0a4.dtd"));
	xmlDoc->dumpToFile(path);
} // createModifiersXml

void ConfigCreator::createUserDatabaseXml(std::string path) {
	XmlElement *xmlElemRoot = new XmlElement("userDatabase");
	xmlElemRoot->addAttribute(new XmlAttribute("version", "1.0a4"));

	XmlDocument *xmlDoc = XmlDocument::createXmlDocument(xmlElemRoot);
	xmlDoc->setDtd(new XmlDtdReference("userDatabase", "http://dtd.inVRs.org/userDatabase_v1.0a4.dtd"));
	xmlDoc->dumpToFile(path);
} // createUserDatabaseXml

XmlElement* ConfigCreator::createXmlElemPath(std::string name, std::string dir) {
	XmlElement *xmlElem = new XmlElement("path");
	xmlElem->addAttribute(new XmlAttribute("name", name));
	xmlElem->addAttribute(new XmlAttribute("directory", dir));

	return xmlElem;
} // createXmlElemPath
