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

#include "EditorBase.h"
#include "EditorDataTypes.h"

#include <stdio.h>
#ifdef WIN32
# include <direct.h>
#else
# include <dirent.h>
#endif
#include <sys/stat.h>

#include <iostream>
#include <locale>
#include <vector>
#include <limits>

#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGSimpleSceneManager.h>
#include <OpenSG/OSGSceneFileHandler.h>
#if OSG_MAJOR_VERSION >= 2
#	include <OpenSG/OSGGeoIndices.h>
#	include <OpenSG/OSGBoxVolume.h>
#else //OpenSG1:
#	include <OpenSG/OSGDynamicVolume.h>
#endif

#include <inVRs/OutputInterface/OpenSGSceneGraphInterface/OpenSGSceneGraphInterface.h>
#include <inVRs/SystemCore/WorldDatabase/EntityTypeFactory.h>
#include <inVRs/OutputInterface/OutputInterface.h>
#include <inVRs/SystemCore/SystemCore.h>
#include <inVRs/SystemCore/Timer.h>
#include <inVRs/SystemCore/TransformationManager/TransformationManager.h>

EditorBase::EditorBase() {
	mSelection = new Selection();
} // EditorBase

EditorBase::~EditorBase() {
	delete mSelection;	
} // ~EditorBase

bool EditorBase::loadConfiguration(std::string filename) {
	//to avoid problems loading vrml files (decimal symbol)
	setlocale(LC_ALL, "en_US");

	if(!Configuration::loadConfig(filename)) {
		std::cout << "Error: could not load config-file!" << std::endl; 
		return false;
	} // if

	Configuration::printConfig();
	size_t found;
	std::string dir = Configuration::getPath("SystemCoreConfiguration");
	std::string root = Configuration::getPath("Root");	

	found = dir.find(root);
	dir = dir.substr(found + root.size());

	found = dir.find_last_of("/\\");
	dir = dir.substr(0, found);
	found = dir.find_last_of("/\\");
	dir = dir.substr(0, found);

	found = filename.find_last_of("/\\");
	filename = filename.substr(0, found);	

	found = filename.rfind(dir);
	dir = filename.substr(0, found);

	chdir(dir.c_str());

	std::string systemCoreConfigFile = Configuration::getString(
			"SystemCore.systemCoreConfiguration");
	std::string outputInterfaceConfigFile = Configuration::getString(
			"Interfaces.outputInterfaceConfiguration");

	std::cout << std::endl;
	std::cout << "systemCoreConfigFile: " << systemCoreConfigFile << std::endl;
	std::cout << "outputInterfaceConfigFile: " << outputInterfaceConfigFile << std::endl;
	std::cout << std::endl;

	if(!SystemCore::configure(systemCoreConfigFile, outputInterfaceConfigFile)) {
		std::cout << "Error: failed to setup SystemCore!" << std::endl;
		return false;
	} // if

	OpenSGSceneGraphInterface* sgIF = dynamic_cast<OpenSGSceneGraphInterface*>
		(OutputInterface::getSceneGraphInterface());

	if(!sgIF) {
		std::cout << "Error: Failed to get OpenSGSceneGraphInterface!";
		std::cout << std::endl;
		std::cout << "Please check if the OutputInterface configuration";
		std::cout << " is correct" << std::endl;
		return false;
	} // if

	osg::NodePtr scene = sgIF->getNodePtr();
	mRoot = osg::Node::create();

	beginEditCP(mRoot);
	mRoot->setCore(osg::Group::create());
	mRoot->addChild(scene);
	endEditCP(mRoot);
	return true;
} // loadConfiguration

void EditorBase::newConfiguration(std::string invrsPath, std::string projectPath) {
	ConfigCreator::createBasicConfig(invrsPath, projectPath);
} // newConfiguration

void EditorBase::newEntity(int id) {
	int envBasedId = 0;
	if(mSelection->type == ENTITYTYPE) {
		envBasedId = WorldDatabase::createEntity(mSelection->entityType->getId(), id);
	}else if(mSelection->type == ENTITY) {
		envBasedId = WorldDatabase::createEntity(mSelection->entity->getEntityType()->getId(), id);
	} // else if

	if(envBasedId) {
		update();
		Entity *entity = WorldDatabase::getEntityWithEnvironmentId(envBasedId);
		Selection *tmpSelection = new Selection();
		tmpSelection->type = ENTITY;
		tmpSelection->entity = entity;
		tmpSelection->location = id;

		setSelection(tmpSelection);
	} // if
} // newEntity

void EditorBase::newEntityType(std::string name) {
	int i = 0;
	EntityType *entType;

	do{
		entType = WorldDatabase::getEntityTypeWithId(++i); 
	}while(entType != NULL);


	EntityTypeFactory factory;
	entType = factory.create("EntityType", i);

	OpenSGSceneGraphInterface* sgIF = dynamic_cast<OpenSGSceneGraphInterface*>
		(OutputInterface::getSceneGraphInterface());

	ModelInterface *model = new OpenSGModel();
	TransformationData transData;
	transData.position = gmtl::Vec3f(0,0,0);
	transData.scale = gmtl::Vec3f(1,1,1);
	transData.orientation = gmtl::Quatf(0,0,0,0);
	transData.scaleOrientation = gmtl::Quatf(0,0,0,0);
	model->setModelTransformation(transData);
	entType->setName(name);
	entType->setModel(model);
	entType->setXmlFilename("entities.xml");

	WorldDatabase::getPrivateAccessor().addEntityType(entType);

	Selection *tmpSelection = new Selection();
	tmpSelection->type = ENTITYTYPE;
	tmpSelection->entityType = entType;
	tmpSelection->location = -1;

	setSelection(tmpSelection);
} // newEntityType

bool EditorBase::newEnvironment(int spacingX, int spacingZ, int sizeX, int sizeZ) {
	int id = 0;
	Environment *env;

	do{
		env = WorldDatabase::getEnvironmentWithId(++id);
	}while(env != NULL);

	if(!WorldDatabase::createEnvironment(id, spacingX, spacingZ, sizeX, sizeZ)) {
		return false;
	}

	TransformationManager::step(10);

	env = WorldDatabase::getEnvironmentWithId(id);

	if(env == NULL) {
		printd("EditorBase::newEnvironment Environment with id %i is NULL\n", id);
		return false;
	} // if
	env->setXmlFilename("environment" + toString(id) + ".xml");

	Selection *tmpSelection = new Selection();
	tmpSelection->type = ENVIRONMENT;
	tmpSelection->environment = env; 
	tmpSelection->location = id;
	setSelection(tmpSelection);

	return true;
} // newEnvironment

void EditorBase::deleteSelection() {
	if(mSelection->type == ENTITY) {
		WorldDatabase::destroyEntity(mSelection->entity);
	}else if(mSelection->type == ENTITYTYPE) {
		WorldDatabase::getPrivateAccessor().deleteEntityType(mSelection->entityType);
	} // else if
	update();
} // deleteSelection

void EditorBase::saveConfiguration() {
	WorldDatabase::saveWorldDatabase();
} // saveConfiguration

void EditorBase::addEntryPoint(gmtl::Vec3f position, gmtl::Vec3f direction) {
	if(mSelection->type == ENVIRONMENT) {
		mSelection->environment->addEntryPoint(position, direction);
	} // if
} // addEntryPoint

void EditorBase::deleteEntryPoint(unsigned index) {
	if(mSelection->type == ENVIRONMENT) {
		mSelection->environment->deleteEntryPoint(index);
	}
} // deleteEntryPoint

void EditorBase::setSelection(Selection *value) {
	mSelection = value;
	highlightObject();
	showPreview();
} // setSelection

void EditorBase::setSelectionTranslation(gmtl::Vec3f value) {
	if(mSelection->type == ENTITY) {
		TransformationData tmp = mSelection->entity->getEnvironmentTransformation();
		tmp.position = value;
		mSelection->entity->setEnvironmentTransformation(tmp);
	}else if(mSelection->type == ENTITYTYPE) {
		TransformationData tmp = mSelection->entityType->getModel()->getModelTransformation();
		tmp.position = value;
		mSelection->entityType->getModel()->setModelTransformation(tmp);
	} // else if
	update();
} // setSelectionTranslation

void EditorBase::setSelectionRotation(gmtl::AxisAnglef value) {
	if(mSelection->type == ENTITY) {
		TransformationData tmp = mSelection->entity->getEnvironmentTransformation();
		gmtl::set(tmp.orientation, value);
		mSelection->entity->setEnvironmentTransformation(tmp);
	}else if(mSelection->type == ENTITYTYPE) {
		TransformationData tmp = mSelection->entityType->getModel()->getModelTransformation();
		gmtl::set(tmp.orientation, value);
		mSelection->entityType->getModel()->setModelTransformation(tmp);
	} // else if 

	update();
} // setSelectionRotation

void EditorBase::setSelectionScale(gmtl::Vec3f value) {
	if(mSelection->type == ENTITY) {
		TransformationData tmp = mSelection->entity->getEnvironmentTransformation();
		tmp.scale = value;
		mSelection->entity->setEnvironmentTransformation(tmp);
	}else if(mSelection->type == ENTITYTYPE) {
		TransformationData tmp = mSelection->entityType->getModel()->getModelTransformation();
		tmp.scale = value;
		mSelection->entityType->getModel()->setModelTransformation(tmp);
	} // else if

	update();
} // setSelectionScale

void EditorBase::setSimpleSceneManagerMain(osg::SimpleSceneManager* simpleSceneManager) {
	mSimpleSceneManagerMain = simpleSceneManager;
} // setSimpleSceneManagerMain

void EditorBase::setSimpleSceneManagerPreview(osg::SimpleSceneManager* simpleSceneManager) {
	mSimpleSceneManagerPreview = simpleSceneManager;
} // setSimpleSceneManagerPreview

void EditorBase::setPositionX(int value) {
	if(mSelection->type == ENVIRONMENT) {
		mSelection->environment->setXPosition(value);
	} // if
	updateHighlight();
} // setPositionX

void EditorBase::setPositionZ(int value) {
	if(mSelection->type == ENVIRONMENT) {
		mSelection->environment->setZPosition(value);
	}
	updateHighlight();
} // setPositionZ

void EditorBase::setFixed(bool value) {
	if(mSelection->type == ENTITY) {
		mSelection->entity->setFixed(value);
	} else if(mSelection->type == ENTITYTYPE) {
		mSelection->entityType->setFixed(value);
	} // else if
} // setFixed

void EditorBase::setRepCopy(bool value) {
	if(mSelection->type == ENTITYTYPE) {
		mSelection->entityType->getModel()->setDeepCloneMode(value);
	} // if
} // setRepCopy

void EditorBase::setName(std::string value) {
	if(mSelection->type == ENTITYTYPE) {
		mSelection->entityType->setName(value);
	} // if
} // setName

void EditorBase::setModelFilename(std::string value) {
	OpenSGSceneGraphInterface* sgIF = dynamic_cast<OpenSGSceneGraphInterface*>
		(OutputInterface::getSceneGraphInterface());
	if(mSelection->type == ENTITYTYPE) {
		ModelInterface *model = sgIF->loadModel("VRML", Configuration::getPath("Entities") + value);
		TransformationData tmpTransformData = mSelection->entityType->
			getModel()->getModelTransformation();
		model->setModelTransformation(tmpTransformData);

		mSelection->entityType->setModel(model);

		std::vector<Entity *> entities = mSelection->entityType->getInstanceList();

		TransformationData transData;
		for(std::vector<Entity *>::iterator entityIt = entities.begin(); entityIt != entities.end();
				++entityIt) {
			(*entityIt)->updateVisualRepresentation();
		} // for
	} // if

	highlightObject();
	showPreview();
} // setModelFilename

void EditorBase::setXmlFilename(std::string value) {
	if(mSelection->type == ENTITYTYPE) {
		mSelection->entityType->setXmlFilename(value);
	} else if(mSelection->type == ENVIRONMENT) {
		mSelection->environment->setXmlFilename(value);
	} // else if
} // setXmlFilename

osg::NodePtr EditorBase::getRootNode() {
	return mRoot;
} // getRootNode

std::vector<Tile*> EditorBase::getTileList() {
	return WorldDatabase::getTileList();
} // getTileList

std::vector<EntityType*> EditorBase::getEntityTypeList() {
	return WorldDatabase::getEntityTypeList();
} // getEntityTypeList

std::vector<Environment*> EditorBase::getEnvironmentList() {
	return WorldDatabase::getEnvironmentList();
} // getEnvironmentList

EntityType* EditorBase::getEntityType(unsigned short id) {
	return WorldDatabase::getEntityTypeWithId(id);  
} // getEntityType

Entity* EditorBase::getEntity(unsigned short id) {
	return WorldDatabase::getEntityWithEnvironmentId(id);
} // getEntity

Environment* EditorBase::getEnvironment(unsigned short id) {
	return WorldDatabase::getEnvironmentWithId(id);
} // getEnvironment

Selection* EditorBase::getSelection() {
	return mSelection;
} // getSelection

gmtl::Vec3f EditorBase::getSelectionTranslation() {
	if(mSelection->type == ENTITY) {
		TransformationData tmp = mSelection->entity->getEnvironmentTransformation();
		return tmp.position;
	}else if(mSelection->type == ENTITYTYPE) {
		TransformationData tmp = mSelection->entityType->getModel()->getModelTransformation();
		return tmp.position;
	} // else if
	return gmtl::Vec3f(0,0,0);
} // getSelectionTranslation

gmtl::AxisAnglef EditorBase::getSelectionRotation() {
	if(mSelection->type == ENTITY) {
		TransformationData tmp = mSelection->entity->getEnvironmentTransformation();
		return gmtl::make<gmtl::AxisAnglef>(tmp.orientation);
	}else if(mSelection->type == ENTITYTYPE) {
		TransformationData tmp = mSelection->entityType->getModel()->getModelTransformation();
		return gmtl::make<gmtl::AxisAnglef>(tmp.orientation);
	} // else if
	return gmtl::AxisAnglef(0,0,0,0);
} // getSelectionRotation

gmtl::Vec3f EditorBase::getSelectionScale() {
	if(mSelection->type == ENTITY) {
		TransformationData tmp = mSelection->entity->getEnvironmentTransformation();
		return tmp.scale;
	}else if(mSelection->type == ENTITYTYPE) {
		TransformationData tmp = mSelection->entityType->getModel()->getModelTransformation();
		return tmp.scale;
	} // else if
	return gmtl::Vec3f(0,0,0);
} // getSelectionScale

bool EditorBase::getFixed() {
	if(mSelection->type == ENTITYTYPE) {
		return mSelection->entityType->isFixed();
	} else if(mSelection->type == ENTITY) {
		return mSelection->entity->isFixed();
	} // else if
	return false;
} // getFixed

bool EditorBase::getRepCopy() {
	if(mSelection->type == ENTITYTYPE) {
		return mSelection->entityType->getModel()->getDeepCloneMode();
	} // if
	return false;
}

std::string EditorBase::getName() {
	if(mSelection->type == ENTITYTYPE) {
		return mSelection->entityType->getName();
	} // if
	return "";
} // getName

int EditorBase::getPositionX() {
	if(mSelection->type == ENVIRONMENT) {
		return mSelection->environment->getXPosition();
	} // if
	return 0;
} // getPositionX

int EditorBase::getPositionZ() {
	if(mSelection->type == ENVIRONMENT) {
		return mSelection->environment->getZPosition();
	}
	return 0;
} // getPositionZ

std::string EditorBase::getModelFilename() {
	if(mSelection->type == ENTITYTYPE) {
		std::string tmp = mSelection->entityType->getModel()->getFilePath();
		size_t found;
		found = tmp.find_last_of("/\\");
		tmp = tmp.substr(found+1);

		return tmp;
	} // if
	return "";
} // getModelFilename

std::vector<std::string> EditorBase::getModelFilenames(std::string kind) {
	std::string path = Configuration::getPath("Models");
	std::vector<std::string> strings;

	path += kind;

#if WIN32
	HANDLE hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA ffd;

	path += "/*"; //for the files in the directory

	if((hFind = FindFirstFile(path.c_str(), &ffd)) != INVALID_HANDLE_VALUE) {
		do {
			if ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) || (ffd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)) {
				//ignore
			} else {
				strings.push_back(ffd.cFileName);
			}			
		} while (FindNextFile(hFind, &ffd) != 0);
		FindClose(hFind);
	}
	//for(std::vector<std::string>::iterator it = strings.begin(); it != strings.end(); ++it) {
	//	std::cout << "found file: " << *it << std::endl;
	//}
#else
	DIR *dp;
	struct dirent *dirp;
	struct stat filestat;
	std::string filepath;

	if((dp = opendir(path.c_str())) != NULL) {
		while((dirp = readdir(dp)) != NULL) {
			filepath = path + "/" + dirp->d_name;

			//skip directories and invalid entries
			if(stat(filepath.c_str(), &filestat)) continue;
			if(S_ISDIR(filestat.st_mode)) continue;

			strings.push_back(dirp->d_name);
		} // while
		closedir(dp);
	} // if
#endif
	return strings;
} // getModelFilenames

std::string EditorBase::getXmlFilename() {
	if(mSelection->type == ENTITYTYPE) {
		return mSelection->entityType->getXmlFilename();
	} else if(mSelection->type == ENVIRONMENT) {
		return mSelection->environment->getXmlFilename();
	} // else if

	return "";
} // getXmlFilename

void EditorBase::update() {
	float dt;
	float currentTimeStamp;

	currentTimeStamp = timer.getTime();
	dt = currentTimeStamp - lastTimeStamp;
	TransformationManager::step(dt);
	lastTimeStamp = currentTimeStamp;

	updateHighlight();
} // update

void EditorBase::highlightObject() {
	clearHighlight();

	if(mSelection->type == ENTITY) {
		ModelInterface* entityModel = mSelection->entity->getVisualRepresentation();
		OpenSGModel* osgModel = dynamic_cast<OpenSGModel*>(entityModel);
		osg::NodePtr node = osgModel->getNodePtr();
		highlight.push_back(node);
	}else if(mSelection->type == ENTITYTYPE) {
		std::vector<Entity *> entities = mSelection->entityType->getInstanceList();

		for(std::vector<Entity *>::iterator entityIt = entities.begin(); entityIt != entities.end();
				++entityIt) {
			ModelInterface *entityModel = (*entityIt)->getVisualRepresentation();
			OpenSGModel* osgModel = dynamic_cast<OpenSGModel*>(entityModel);
			highlight.push_back(osgModel->getNodePtr());
		} // for
	}else if(mSelection->type == ENTITYTYPES){
		std::vector<Entity *> entities = 
			(WorldDatabase::getEnvironmentWithId(mSelection->location))->getEntityList();	

		for(std::vector<Entity *>::iterator entityIt = entities.begin(); entityIt != entities.end();
				++entityIt) {
			ModelInterface *entityModel = (*entityIt)->getVisualRepresentation();
			OpenSGModel* osgModel = dynamic_cast<OpenSGModel*>(entityModel);
			highlight.push_back(osgModel->getNodePtr());
		} // for
	}else if(mSelection->type == ENVIRONMENT){
		highlight.resize(1);			
	}// else if
	highlightChanged();
} // highlight Object

void EditorBase::highlightChanged() {
	if(highlightMaterial == osg::NullFC) {
		highlightMaterial = osg::SimpleMaterial::create();

		beginEditCP(highlightMaterial);
		highlightMaterial->setDiffuse(osg::Color3f(0,1,0));
		highlightMaterial->setLit(false);
		endEditCP(highlightMaterial);
	} // if

	if(highlightNode.size() != highlight.size()) {
		highlightNode.resize(highlight.size());
		highlightPoints.resize(highlight.size());
		for(int i = 0; i < (int)highlight.size(); ++i) {
#if OSG_MAJOR_VERSION >= 2
			osg::GeoUInt8PropertyRecPtr types = osg::GeoUInt8Property::create();
			osg::GeoUInt32PropertyRecPtr lengths = osg::GeoUInt32Property::create();
			osg::GeoUInt32PropertyRecPtr indices = osg::GeoUInt32Property::create();
			highlightPoints[i] = osg::GeoPnt3fProperty::create();
#else
			osg::GeoPTypesPtr types = osg::GeoPTypesUI8::create();
			osg::GeoPLengthsPtr lengths = osg::GeoPLengthsUI32::create();
			osg::GeoIndicesUI32Ptr indices = osg::GeoIndicesUI32::create();
			highlightPoints[i] = osg::GeoPositions3f::create();
#endif // OSG_MAJOR_VERSION
			beginEditCP(types);
			types->push_back(GL_LINE_STRIP);
			types->push_back(GL_LINES);
			endEditCP(types);

			beginEditCP(lengths);
			lengths->push_back(10);
			lengths->push_back(6);
			endEditCP(lengths);

#if OSG_MAJOR_VERSION < 2
			beginEditCP(indices, osg::GeoIndicesUI32::GeoPropDataFieldMask );
#endif
			indices->addValue(0);
			indices->addValue(1);
			indices->addValue(3);
			indices->addValue(2);
			indices->addValue(0);
			indices->addValue(4);
			indices->addValue(5);
			indices->addValue(7);
			indices->addValue(6);
			indices->addValue(4);
			indices->addValue(1);
			indices->addValue(5);
			indices->addValue(2);
			indices->addValue(6);
			indices->addValue(3);
			indices->addValue(7);
#if OSG_MAJOR_VERSION < 2
			endEditCP(indices);
#endif

			beginEditCP(highlightPoints[i]);
			highlightPoints[i]->push_back(osg::Pnt3f(-1,-1,-1));
			highlightPoints[i]->push_back(osg::Pnt3f( 1,-1,-1));
			highlightPoints[i]->push_back(osg::Pnt3f(-1, 1,-1));
			highlightPoints[i]->push_back(osg::Pnt3f( 1, 1,-1));
			highlightPoints[i]->push_back(osg::Pnt3f(-1,-1, 1));
			highlightPoints[i]->push_back(osg::Pnt3f( 1,-1, 1));
			highlightPoints[i]->push_back(osg::Pnt3f(-1, 1, 1));
			highlightPoints[i]->push_back(osg::Pnt3f( 1, 1, 1));
			endEditCP(highlightPoints[i]);

			osg::GeometryPtr geo = osg::Geometry::create();
			beginEditCP(geo);
			geo->setTypes(types);
			geo->setLengths(lengths);
			geo->setIndices(indices);
			geo->setPositions(highlightPoints[i]);
			geo->setMaterial(highlightMaterial);
			endEditCP(geo);
			addRefCP(geo);

			highlightNode[i] = osg::Node::create();
			beginEditCP(highlightNode[i]);
			highlightNode[i]->setCore(geo);
			endEditCP(highlightNode[i]);
			addRefCP(highlightNode[i]);
		} // for 
	} // if

	for(int i = 0; i < (int)highlightNode.size(); ++i) {
		if(highlightNode[i] != osg::NullFC) {
			beginEditCP(mRoot);
			mRoot->addChild(highlightNode[i]);
			endEditCP(mRoot);
		} // if
	} // for
	updateHighlight();
} // highlightChanged 

void EditorBase::updateHighlight() {
	for(int i = 0; i < (int)highlightPoints.size(); ++i) {

#if OSG_MAJOR_VERSION >= 2
		osg::BoxVolume vol;	
#else //OpenSG1:
		osg::DynamicVolume vol;
#endif
		osg::Pnt3f min,max;
		min[0] = std::numeric_limits<float>::max();
		min[1] = std::numeric_limits<float>::max();
		min[2] = std::numeric_limits<float>::max();
		if(mSelection->type == ENVIRONMENT) {
			std::vector<Entity *> entities = mSelection->environment->getEntityList();

			for(std::vector<Entity *>::iterator entityIt = entities.begin(); entityIt != entities.end();
					++entityIt) {
				ModelInterface *entityModel = (*entityIt)->getVisualRepresentation();
				OpenSGModel* osgModel = dynamic_cast<OpenSGModel*>(entityModel);
				(osgModel->getNodePtr()->getWorldVolume(vol));

				osg::Pnt3f tmpMin, tmpMax;				
				vol.getBounds(tmpMin, tmpMax);

				for(int i = 0; i < 3; ++i){
					if(tmpMin[i] < min[i]) min[i] = tmpMin[i];
					if(tmpMax[i] > max[i]) max[i] = tmpMax[i];
				} // for
			} // for

		}else{
			highlight[i]->getWorldVolume(vol);

			vol.getBounds(min, max);
		}
		beginEditCP(highlightPoints[i]);
		highlightPoints[i]->setValue(osg::Pnt3f(min[0], min[1], min[2]), 0);
		highlightPoints[i]->setValue(osg::Pnt3f(max[0], min[1], min[2]), 1);
		highlightPoints[i]->setValue(osg::Pnt3f(min[0], max[1], min[2]), 2);
		highlightPoints[i]->setValue(osg::Pnt3f(max[0], max[1], min[2]), 3);
		highlightPoints[i]->setValue(osg::Pnt3f(min[0], min[1], max[2]), 4);
		highlightPoints[i]->setValue(osg::Pnt3f(max[0], min[1], max[2]), 5);
		highlightPoints[i]->setValue(osg::Pnt3f(min[0], max[1], max[2]), 6);
		highlightPoints[i]->setValue(osg::Pnt3f(max[0], max[1], max[2]), 7);
		endEditCP(highlightPoints[i]);

#if OSG_MAJOR_VERSION >= 2
#else //OpenSG1:
		beginEditCP(highlightNode[i]->getCore(), osg::Geometry::PositionsFieldMask);
		endEditCP(highlightNode[i]->getCore(), osg::Geometry::PositionsFieldMask);
#endif
	} // for
} // updateHighlight

void EditorBase::clearHighlight() {
	for(std::vector<osg::NodePtr>::iterator hNodeIt = highlightNode.begin(); 
			hNodeIt != highlightNode.end(); ++hNodeIt) {
		if((*hNodeIt)->getParent() != osg::NullFC) {
			beginEditCP(mRoot);
			mRoot->subChild(*hNodeIt);
			endEditCP(mRoot);
		} // if
	} // for

	highlight.clear();
	highlightNode.clear();
	highlightPoints.clear();
} // clearHighlight

void EditorBase::showPreview() {
	if(mSelection->type == ENTITYTYPE) {
		ModelInterface *entityTypeModel = mSelection->entityType->getModel();
		OpenSGModel *osgModel = dynamic_cast<OpenSGModel*>(entityTypeModel);
		osg::NodePtr node = osgModel->getNodePtr();
		mSimpleSceneManagerPreview->setRoot(node);
		mSimpleSceneManagerPreview->showAll();
	}else if(mSelection->type == ENTITY) {
		ModelInterface *entityTypeModel = mSelection->entity->getEntityType()->getModel();
		OpenSGModel *osgModel = dynamic_cast<OpenSGModel*>(entityTypeModel);
		osg::NodePtr node = osgModel->getNodePtr();
		mSimpleSceneManagerPreview->setRoot(node);
		mSimpleSceneManagerPreview->showAll();
	}else {
		osg::NodePtr node = osg::Node::create();
		beginEditCP(node);
		node->setCore(osg::Group::create());
		endEditCP(node);
		mSimpleSceneManagerPreview->setRoot(node);
	}// else
} // showPreview

