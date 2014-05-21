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

#include <stdlib.h>
#include <memory>
#include <assert.h>

#include <gmtl/QuatOps.h>
#include <gmtl/Matrix.h>
#include <gmtl/VecOps.h>
#include <gmtl/AxisAngle.h>
#include <gmtl/MatrixOps.h>
#include <gmtl/Generate.h>

#include <irrXML.h>

#include <inVRs/OutputInterface/OutputInterface.h>
#include <inVRs/OutputInterface/SceneGraphInterface.h>

#include "User.h"
#include "UserDatabase.h"
#include "VirtualHandCursorModel.h"
#include "../Timer.h"
#include "../Configuration.h"
#include "../DebugOutput.h"
#include "../WorldDatabase/WorldDatabase.h"
#include "../XMLTools.h"
#include "../UtilityFunctions.h"

// disable deprecation warning for std::auto_ptr when std::unique_ptr is not available.
#ifndef HAS_CXX11_UNIQUE_PTR
// Only do this on GCC versions that support it:
# if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 402
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
#endif

XmlConfigurationLoader User::userXmlConfigLoader;
XmlConfigurationLoader User::cursorTransXmlConfigLoader;
XmlConfigurationLoader User::userTransXmlConfigLoader;
static const std::string DEFAULT_USER_TRANSFORMATION_MODEL="HeadPositionUserTransformationModel";

std::vector<CursorTransformationModelFactory*>	User::cursorTransformationModelFactories;
std::vector<UserTransformationModelFactory*> 	User::userTransformationModelFactories;

UserSetupData::UserSetupData() {
	name = "noname";
	id = 0;
	avatarFile = "";
	cursorRepresentationFile = "";
	initialCursorModel = "";
	cursorModelArguments = NULL;
	initialUserTransformationModel = "";
	userTransformationModelArguments = NULL;
	networkId.processId = 0;
	networkId.address.ipAddress = 0;
	networkId.address.portTCP = 0;
	networkId.address.portUDP = 0;
} // UserSetupData

User::User() {
	UserSetupData setupData;
	double tempTime = timer.getSystemTime();
	setupData.id = (rand() + (unsigned)((uint64_t)this)) ^ (unsigned)((uint64_t)(*((unsigned*)&tempTime)));
	setupData.cursorModelArguments = NULL;
	commonInit(&setupData);
} // User

User::User(UserSetupData* setupData) {
	commonInit(setupData);
	// for the local user the netid will be set during init() of the network module
} // User

User::~User() {
	int i;
	if (camera)
		delete camera;

	if (avatar) {
		WorldDatabase::removeAvatar(avatar);
		delete avatar;
	} // if

	for (i = 0; i < (int)associatedEntities.size(); i++)
		delete associatedEntities[i];

	if (cursor)
		delete cursor;

	if (cursorTransformationModel)
		delete cursorTransformationModel;
	cursorTransformationModel = 0;

	if (cursorTransformationModelArguments)
		delete cursorTransformationModelArguments;
	cursorTransformationModelArguments = 0;

	if (userTransformationModel)
		delete userTransformationModel;
	userTransformationModel = 0;

	if (userTransformationModelArguments)
		delete userTransformationModelArguments;
	userTransformationModelArguments = 0;
} // ~User

bool User::loadConfig(std::string configFile) {
	bool success = true;
	std::string configFileConcatenatedPath = getConcatenatedPath(configFile, "UserConfiguration");
	printd(INFO, "User::loadConfig(): loading configuration %s\n", configFile.c_str());

	// load xml document via xmlConfigLoader
	std::auto_ptr<const XmlDocument> document(
			userXmlConfigLoader.loadConfiguration(configFileConcatenatedPath));
	if (!document.get()) {
		printd(ERROR,
				"User::loadConfig(): error at loading of configuration file %s!\n",
				configFile.c_str());
		return false;
	} // if

	// loading avatar
	std::string avatarConfigFile = document->getAttributeValue("userDatabase.avatar.configFile");
	if (avatarConfigFile.size() > 0) {
		printd(INFO, "User::loadConfig(): trying to load avatar configuration file %s!\n",
				avatarConfigFile.c_str());
		avatar = WorldDatabase::loadAvatar(avatarConfigFile);
		if (!avatar) {
			printd(ERROR, "UsersDatabase::loadConfig(): error loading avatar!\n");
			success = false;
		} // if
		else {
			avatar->setOwner(this);
			this->avatarConfigFile = avatarConfigFile;
		} // else
	} // if

	if (cursor != NULL) {
		detachCursorRepresentation();
	} // if

	// loading CursorRepresentation
	const XmlElement* element = document->getElement("userDatabase.cursorRepresentation");
	if (element) {
		configFile = element->getAttributeValue("configFile");
		printd(INFO,
				"User::loadConfig(): trying to load cursorRepresentation configuration file %s!\n",
				configFile.c_str());
		cursor = UserDatabase::loadCursorRepresentation(configFile);
		if (!cursor) {
			printd(ERROR, "UserDatabase::loadConfig(): error loading cursorRepresentation!\n");
			success = false;
		} // if
		else {
			this->cursorRepresentationConfigFile = configFile;
			attachCursorRepresentation();
		} // else
	} // if

	// loading UserTransformationModel
	element = document->getElement("userDatabase.userTransformationModel");
	if (element) {
		configFile = element->getAttributeValue("configFile");
		success = loadUserTransformationModelConfig(configFile, this) && success;
	} // if

	// loading CursorTransformationModel
	element = document->getElement("userDatabase.cursorTransformationModel");
	if (element) {
		configFile = element->getAttributeValue("configFile");
		success = loadCursorTransformationModelConfig(configFile, this) && success;
	} // if

	return success;
} // loadConfig

void User::cleanup() {
	int i;
	for (i = 0; i < (int)cursorTransformationModelFactories.size(); i++)
		delete cursorTransformationModelFactories[i];

	cursorTransformationModelFactories.clear();

	for (i = 0; i < (int)userTransformationModelFactories.size(); i++)
		delete userTransformationModelFactories[i];

	userTransformationModelFactories.clear();
} // cleanup

void User::registerCursorTransformationModelFactory(CursorTransformationModelFactory* factory) {
	cursorTransformationModelFactories.push_back(factory);
} // registerCursorTransformationModelFactory

void User::registerUserTransformationModelFactory(UserTransformationModelFactory* factory) {
	userTransformationModelFactories.push_back(factory);
} // registerUserTransformationModelFactory


void User::pickUpEntity(Entity* entity, TransformationData offset) {
	AssociatedEntity* associatedEntity = new AssociatedEntity;
	associatedEntity->entity = entity;
	associatedEntity->pickingOffset = offset;
	associatedEntities.push_back(associatedEntity);
} // pickUpEntity

bool User::dropEntity(unsigned int entityId) {
	AssociatedEntity* associatedEntity = NULL;
	std::vector<AssociatedEntity*>::iterator it;

	for (it = associatedEntities.begin(); it != associatedEntities.end(); ++it) {
		if ((*it)->entity->getTypeBasedId() == entityId) {
			associatedEntity = *it;
			break;
		} // if
	} // for
	if (associatedEntity) {
		associatedEntities.erase(it);
		delete associatedEntity;
		return true;
	} // if
	return false;
} // dropEntity

void User::setCursorTransformationModel(std::string modelName, ArgumentVector* arguments) {
	int i;
	CursorTransformationModel* newModel = NULL;

	if (modelName.length() == 0) {
		printd(WARNING, "User::setCursorTransformationModel(): empty string passed\n");
		return;
	} // if

	for (i = 0; i < (int)cursorTransformationModelFactories.size(); i++) {
		newModel = cursorTransformationModelFactories[i]->create(modelName, arguments);
		if (newModel) {
			break;
		} // if
	} // for

	if (newModel) {
		if (cursorTransformationModelArguments)
			delete cursorTransformationModelArguments;
		cursorTransformationModelArguments = arguments;
		if (cursorTransformationModel)
			delete cursorTransformationModel;
		cursorTransformationModel = newModel;
		printd(INFO, "User::setCursorTransformationModel(): setting model to %s\n",
				modelName.c_str());
	} else {
		printd(WARNING,
			"User::setCursorTransformationModel(): couldn't find factory to create model %s\n",
			modelName.c_str());
	} // else
} // setCursorTransformationModel

void User::setUserTransformationModel(std::string modelName, ArgumentVector* arguments) {
	int i;
	UserTransformationModel* newModel = NULL;

	if (modelName.length() == 0) {
		printd(WARNING, "User::setUserTransformationModel(): empty string passed\n");
		return;
	} // if

	for (i = 0; i < (int)userTransformationModelFactories.size(); i++) {
		newModel = userTransformationModelFactories[i]->create(modelName, arguments);
		if (newModel)
			break;
	} // for
	if (newModel) {
		if (userTransformationModelArguments)
			delete userTransformationModelArguments;
		userTransformationModelArguments = arguments;
		if (userTransformationModel)
			delete userTransformationModel;
		userTransformationModel = newModel;
		printd(INFO, "User::setUserTransformationModel(): setting model to %s\n",
				modelName.c_str());
	} else {
		printd(WARNING,
			"User::setUserTransformationModel(): couldn't find factory to create model %s\n",
			modelName.c_str());
	} // if
} // setUserTransformationModel

void User::setHeadTransformation(TransformationData head) {
//	this->head = head;
	this->sensorTransformation[0] = head;
} // setHeadTransformation

void User::setHandTransformation(TransformationData hand) {
//	this->hand = hand;
	this->sensorTransformation[1] = hand;

	// TODO: implement this externally (e.g. in Modifier since it is a Transformation!)
	// if (avatar)
	// avatar->setHandTransformation(getUserHandTransformation());
} // setHandTransformation

void User::setSensorTransformation(unsigned short sensorIndex, TransformationData transformation) {
	if (sensorIndex < MAX_NUM_SENSORS) {
		sensorTransformation[sensorIndex] = transformation;
	} else {
		printd(ERROR, "User::setSensorTransformation(): sensor index %i out of range (max: %i)!\n",
				sensorIndex, MAX_NUM_SENSORS-1);
	} // else
} // setSensorTransformation

void User::setNavigatedTransformation(TransformationData navigatedTrans) {
	this->navigatedTransformation = navigatedTrans;
} // setNavigatedTransformation

void User::setCursorTransformation(TransformationData &trans) {
	cursorTransformation = trans;
	// TODO: Check if this is the right place to set the Transformation in the SceneGraph (correct Thread???)
	if (cursor && cursor->getModel()) {
		cursor->setTransformation(cursorTransformation);
//		SceneGraphInterface* sgIF = OutputInterface::getSceneGraphInterface();
//		if (sgIF) {
//			sgIF->updateModel(cursor->getModel(), trans);
//		} // if
	} // if
} // setCursorTransformation

void User::setMoving(bool moving) {
	this->moving = moving;
} // setMoving

void User::setNetworkId(NetworkIdentification* netId) {
	assert(netId);
	networkId = *netId;
}

unsigned User::getId() {
	return id;
} // getId

std::string User::getName() {
	return name;
} // getName

std::string User::getAvatarConfigFile() {
	return avatarConfigFile;
} // getAvatarConfigFile

CursorTransformationModel* User::getCursorTransformationModel() {
	return cursorTransformationModel;
} // getCursorTransformationModel

ArgumentVector* User::getCursorTransformationModelArguments() {
	return cursorTransformationModelArguments;
} // getCursorTransformationModelArguments

UserTransformationModel* User::getUserTransformationModel() {
	return userTransformationModel;
} // getUserTransformationModel

ArgumentVector* User::getUserTransformationModelArguments() {
	return userTransformationModelArguments;
} // getUserTransformationModelArguments

TransformationData User::getNavigatedTransformation() {
	return navigatedTransformation;
} // getNavigatedTransformation

TransformationData User::getCursorTransformation() {
	return cursorTransformation;
} // getCursorTransformation

TransformationData User::getWorldHeadTransformation() {
	TransformationData result;
//	return multiply(result, navigatedTransformation, head);
	return multiply(result, navigatedTransformation, sensorTransformation[0]);
} // getWorldHeadTransformation

TransformationData User::getWorldHandTransformation() {
	TransformationData result;
//	return multiply(result, navigatedTransformation, hand);
	return multiply(result, navigatedTransformation, sensorTransformation[1]);
} // getWorldHandTransformation

TransformationData User::getWorldSensorTransformation(unsigned short index) {
	TransformationData result;
	if (index < MAX_NUM_SENSORS) {
		multiply(result, navigatedTransformation, sensorTransformation[index]);
	} // if
	else {
		printd(WARNING,
				"User::getWorldSensorTransformation(): sensor with index %i out of range (indices range from 0 to %i)!\n",
				index, MAX_NUM_SENSORS-1);
		result = identityTransformation();
	} // else
	return result;
} // getWorldSensorTransformation

TransformationData User::getWorldUserTransformation() {
	TransformationData result;
	updateUserTransformation();

	return multiply(result, navigatedTransformation, userTransformation);
} // getWorldUserTransformation

TransformationData User::getTrackedHeadTransformation() {
//	return head;
	return sensorTransformation[0];
} // getTrackedHeadTransformation

TransformationData User::getTrackedHandTransformation() {
//	return hand;
	return sensorTransformation[1];
} // getTrackedHandTransformation

TransformationData User::getTrackedSensorTransformation(unsigned short index) {
	if (index < MAX_NUM_SENSORS)
		return sensorTransformation[index];
	else {
		printd(WARNING,
				"User::getTrackedSensorTransformation(): sensor index %i out of range (indices range from 0 to %i)!\n",
				index, MAX_NUM_SENSORS-1);
		return identityTransformation();
	} // else
} // getTrackedSensorTransformation

TransformationData User::getTrackedUserTransformation() {
	updateUserTransformation();
	return userTransformation;
} // getTrackedUserTransformation

TransformationData User::getUserHeadTransformation() {
	TransformationData result;
	TransformationData headTransformation = sensorTransformation[0];
	TransformationData inverseUserTrans;

	updateUserTransformation();
	invert(inverseUserTrans, userTransformation);

	return multiply(result, inverseUserTrans, headTransformation);
} // getUserHeadTransformation

TransformationData User::getUserHandTransformation() {
	TransformationData result;
	TransformationData handTransformation = sensorTransformation[1];
	TransformationData inverseUserTrans;

	updateUserTransformation();
	invert(inverseUserTrans, userTransformation);

	return multiply(result, inverseUserTrans, handTransformation);
} // getUserHandTransformation

TransformationData User::getUserSensorTransformation(unsigned short index) {
	TransformationData result;
	TransformationData inverseUserTrans;

	if (index < MAX_NUM_SENSORS) {
		updateUserTransformation();
		invert(inverseUserTrans, userTransformation);
		multiply(result, inverseUserTrans, sensorTransformation[index]);
	} // if
	else {
		printd(WARNING,
				"User::getSensorTransformation(): sensor index %i out of range (indices range from 0 to %i)!\n",
				index, MAX_NUM_SENSORS-1);
		result = identityTransformation();
	} // else
	return result;
} // getUserSensorTransformation

AvatarInterface* User::getAvatar() {
	return avatar;
} // getAvatar

CursorRepresentationInterface* User::getCursor() {
	return cursor;
} // getCursor

CameraTransformation* User::getCamera() {
	return camera;
} // getCamera

unsigned User::getNumberOfAssociatedEntities() {
	return associatedEntities.size();
} // getNumberOfAssociatedEntities

Entity* User::getAssociatedEntity(unsigned int entityId) {
	AssociatedEntity* associatedEntity = NULL;
	std::vector<AssociatedEntity*>::iterator it;

	for (it = associatedEntities.begin(); it != associatedEntities.end(); ++it) {
		if ((*it)->entity->getTypeBasedId() == entityId) {
			associatedEntity = *it;
			break;
		} // if
	} // for
	if (associatedEntity)
		return associatedEntity->entity;

	return NULL;
} // getAssociatedEntity

Entity* User::getAssociatedEntityByIndex(unsigned index) {
	if (index >= associatedEntities.size())
		return NULL;

	return associatedEntities[index]->entity;
} // getAssociatedEntityByIndex

TransformationData User::getAssociatedEntityOffset(unsigned int entityId) {
	AssociatedEntity* associatedEntity = NULL;
	std::vector<AssociatedEntity*>::iterator it;

	for (it = associatedEntities.begin(); it != associatedEntities.end(); ++it) {
		if ((*it)->entity->getTypeBasedId() == entityId) {
			associatedEntity = *it;
			break;
		} // if
	} // for
	if (associatedEntity)
		return associatedEntity->pickingOffset;

	printd(
			WARNING,
			"User::getPickedEntityOffset(): user: %s hasn't picked up an entity with id %u, carrying at the moment %i entities!\n",
			name.c_str(), entityId, associatedEntities.size());
	// 	std::cerr << "User::getPickedEntityOffset(): user: " << name << " haven't picked up an entity with id " << entityId << ", carrying at the moment " << carriedEntities.size() << " entities\n";
	return identityTransformation();
} // getPickedEntityOffset

NetworkIdentification User::getNetworkId() {
	return networkId;
}

bool User::isMoving() {
	return moving;
} // isMoving

void User::commonInit(UserSetupData* setupData) {
	// initialize configuration loaders
	if (!userXmlConfigLoader.hasConverters()) {
		userXmlConfigLoader.registerConverter(new UserConverterToV1_0a4);
	} // if

	if (!userTransXmlConfigLoader.hasConverters()) {
		userTransXmlConfigLoader.registerConverter(new UserTransConverterToV1_0a4);
	} // if

	if (!cursorTransXmlConfigLoader.hasConverters()) {
		cursorTransXmlConfigLoader.registerConverter(new CursorTransConverterToV1_0a4);
	} // if

	cursorTransformationModel = NULL;
	cursorTransformationModelArguments = NULL;
	userTransformationModel = NULL;
	userTransformationModelArguments = NULL;
	moving = false;
	name = setupData->name;
	id = setupData->id;

	// load avatar
	if (setupData->avatarFile.size() != 0) {
		avatar = WorldDatabase::loadAvatar(setupData->avatarFile);
		if (avatar) {
			avatar->setOwner(this);
			this->avatarConfigFile = setupData->avatarFile;
		} else {
			printd(ERROR, "User::commonInit(): failed to load avatar file %s\n",
					setupData->avatarFile.c_str());
		} // else
	} // if
	else {
		this->avatarConfigFile = "";
		avatar = NULL;
	} // else

	// load cursor representation
	if (setupData->cursorRepresentationFile.size() != 0) {
		cursor = UserDatabase::loadCursorRepresentation(setupData->cursorRepresentationFile);
		if (cursor) {
			this->cursorRepresentationConfigFile = setupData->cursorRepresentationFile;
		} else {
			printd(ERROR, "User::commonInit(): failed to load cursorRepresentation file %s\n",
					setupData->cursorRepresentationFile.c_str());
		} // else
	} // if
	else {
		this->cursorRepresentationConfigFile = "";
		cursor = NULL;
	} // else

	camera = new CameraTransformation;
	userTransformation = identityTransformation();
	navigatedTransformation = identityTransformation();
	cursorTransformation = identityTransformation();

	//TODO: add argument-vector to setupData to allow dynamic configuration!
	if (setupData->initialCursorModel.size() > 0)
		setCursorTransformationModel(setupData->initialCursorModel, setupData->cursorModelArguments);
	if (setupData->initialUserTransformationModel.size() > 0) {
		setUserTransformationModel(setupData->initialUserTransformationModel,
				setupData->userTransformationModelArguments);
	} else {
		setUserTransformationModel(DEFAULT_USER_TRANSFORMATION_MODEL, NULL);
	} // else

//	cursor = new CursorRepresentation();

	this->networkId = setupData->networkId;

	for (int i=0; i < MAX_NUM_SENSORS; i++) {
		sensorTransformation[i] = identityTransformation();
	} // for
} // commonInit

void User::updateUserTransformation() {
	userTransformationModel->generateUserTransformation(userTransformation, this);
/*
	// use user's head sensor for estimation of user transformation
	userTransformation = sensorTransformation[0];
	// remove the head sensor orientation (assuming that user is oriented in
	// direction of the screen)
	userTransformation.orientation = gmtl::QUAT_IDENTITYF;
	// remove the head sensor height (assuming that the tracking system has its
	// origin at the floor)
	userTransformation.position[1] = 0;
*/
} // updateUserTransformation

void User::attachCursorRepresentation() {
	SceneGraphInterface* sgIF = OutputInterface::getSceneGraphInterface();
	if (sgIF && cursor != NULL) {
		ModelInterface* cursorModel = cursor->getModel();
		if (cursorModel) {
			sgIF->attachModel(cursorModel, cursorTransformation);
		} // if
		else {
			printd(WARNING, "User::attachCursorRepresentation(): cursorRepresentation has no model!\n");
		} // else
	} // if
} // attachCursorRepresentation

void User::detachCursorRepresentation() {
	SceneGraphInterface* sgIF = OutputInterface::getSceneGraphInterface();
	if (sgIF && cursor != NULL) {
		ModelInterface* cursorModel = cursor->getModel();
		if (cursorModel) {
			sgIF->detachModel(cursorModel);
		} // if
	} // if
} // attachCursorRepresentation

void User::updateCursor(float dt) {
	if (cursor) {
		cursor->update(dt);
	} // if
} // updateCursor


//*****************************************************************************
// Configuration loading
//*****************************************************************************

bool User::loadCursorTransformationModelConfig(std::string configFile, User* user) {
	std::string configFileConcatenatedPath;
	ArgumentVector* arguments = NULL;

	if (!Configuration::containsPath("CursorTransformationModelConfiguration")
			&& Configuration::containsPath("CursorModelConfiguration")) {
		printd(WARNING,
				"User::loadCursorTransformationModelConfig(): Deprecated path entry CursorModelConfiguration found in general configuration file! Use CursorTransformationModelConfiguration instead!\n");
		configFileConcatenatedPath = getConcatenatedPath(configFile, "CursorModelConfiguration");
	} else {
		configFileConcatenatedPath = getConcatenatedPath(configFile, "CursorTransformationModelConfiguration");
	} // else

	// load xml document via xmlConfigLoader
	std::auto_ptr<const XmlDocument> document(
			cursorTransXmlConfigLoader.loadConfiguration(configFileConcatenatedPath));
	if (!document.get()) {
		printd(ERROR,
				"User::loadCursorTransformationModelConfig(): error at loading of configuration file %s!\n",
				configFile.c_str());
		return false;
	} // if

	const XmlElement* element = document->getElement("cursorTransformationModel.model");
	if (!element) {
		printd(ERROR, "User::loadCursorTransformationModelConfig(): Unable to find xml node <model> in config file %s!\n",
				configFile.c_str());
		return false;
	} // if
	std::string cursorTransformationModelName = element->getAttributeValue("name");

	if (element->hasSubElements()) {
		arguments = readArgumentsFromXmlElement(element);
	} // if
	user->setCursorTransformationModel(cursorTransformationModelName, arguments);

	return true;
} // loadCursorTransformationModelConfig

bool User::loadUserTransformationModelConfig(std::string configFile, User* user) {
	std::string configFileConcatenatedPath;
	ArgumentVector* arguments = NULL;

	configFileConcatenatedPath = getConcatenatedPath(configFile, "UserTransformationModelConfiguration");

	// can not automatically update file because it may be included in the userDatabase.xml file
	// load xml document via xmlConfigLoader
	std::auto_ptr<const XmlDocument> document(userTransXmlConfigLoader.loadConfiguration(configFileConcatenatedPath));
	if (!document.get()) {
		printd(ERROR,
				"User::loadUserTransformationModelConfig(): error at loading of configuration file %s!\n",
				configFile.c_str());
		return false;
	} // if

	const XmlElement* element = document->getElement("userTransformationModel.model");
	if (!element) {
		printd(ERROR, "User::loadUserTransformationModelConfig(): Unable to find xml node <model> in config file %s!\n",
				configFile.c_str());
		return false;
	} // if
	std::string userTransformationModelName = element->getAttributeValue("name");

	if (element->hasSubElements()) {
		arguments = readArgumentsFromXmlElement(element);
	} // if
	user->setUserTransformationModel(userTransformationModelName, arguments);

	return true;
} // loadUserTransformationModelConfig


User::UserConverterToV1_0a4::UserConverterToV1_0a4() :
	XmlConfigurationConverter("0.0", "1.0a4") {
} // ConverterToV1_0a4

bool User::UserConverterToV1_0a4::convert(XmlDocument* document, const Version& version,
		std::string configFile) {
	bool success = true;

	updateDocument(document, XmlConfigurationLoader::XmlDtdUrl + "userDatabase_v1.0a4.dtd",
			destinationVersion, "userDatabase");

	XmlElement* element = document->getElement("userDatabase.cursorModel");
	if (element) {
		element->setName("cursorTransformationModel");
	} // if
	document->renameAttributes("userDatabase.avatar.config", "configFile");
	document->renameAttributes("userDatabase.cursorTransformationModel.config", "configFile");
	document->renameAttributes("userDatabase.cursorRepresentation.config", "configFile");

	// change entry of userTransformationModel since this is parsed in a separate file
	// in later versions. In this case the configFile will be set to the userDatabase
	// config file which contains the definition. The converter of the userTransformationModel
	// loader will then convert the userDatabase config file to the new userTransformationModel
	// config file!
	element = document->getElement("userDatabase.userTransformationModel");
	if (element) {
		element->deleteAllAttributes();
		element->deleteAllSubElements();
		XmlAttribute* newAttribute = new XmlAttribute("configFile", configFile);
		element->addAttribute(newAttribute);
	} // if

	return success;
} // convert

User::CursorTransConverterToV1_0a4::CursorTransConverterToV1_0a4() :
	XmlConfigurationConverter("0.0", "1.0a4") {
} // CursorTransConverterToV1_0a4

bool User::CursorTransConverterToV1_0a4::convert(XmlDocument* document, const Version& version,
		std::string configFile) {

	bool success = true;
	XmlElement* modelNode = document->getElement("cursorModel");
	if (!modelNode) {
		printd(ERROR,
				"CursorTransConverterToV1_0a4::convert(): could not convert file because root node is not matching: found <%s>, expected <cursorModel>! Please upgrade to a current xml file version\n",
				document->getRootElement()->getName().c_str());
		return false;
	} // if

	// rename cursorModel node to model
	modelNode->setName("model");

	// create new document root node and attach model node
	XmlElement* newDocumentRoot = new XmlElement("cursorTransformationModel");
	newDocumentRoot->addSubElement(modelNode);

	// replace old document root with new one (old one must not be deleted since it is
	// now a child of the new root element)
	document->replaceRootElement(newDocumentRoot);

	updateDocument(document, XmlConfigurationLoader::XmlDtdUrl + "cursorTransformationModel_v1.0a4.dtd",
			destinationVersion, "cursorTransformationModel");

	// update argument vector to current version
	XmlElement* element = document->getElement("cursorTransformationModel.model.arguments");
	if (element) {
		success = XmlArgumentVectorLoader::get().updateXmlElement(element, version,
				destinationVersion, configFile);
	} // if

	return success;
} // convert


User::UserTransConverterToV1_0a4::UserTransConverterToV1_0a4() :
	XmlConfigurationConverter("0.0", "1.0a4") {
} // UserTransConverterToV1_0a4

bool User::UserTransConverterToV1_0a4::convert(XmlDocument* document, const Version& version,
		std::string configFile) {

	bool success = true;
	// Part of config file was in userDatabase configuration previously
	// so we load the userDatabase config file and remove all unnecessary nodes!!!

	// find user node which is parent of userTransformationModel node
	XmlElement* parent = document->getElement("user");
	if (!parent) {
		printd(ERROR,
				"UserTransConverterToV1_0a4::convert(): could not convert file because of missing node <user> in UserDatabase configuration file! Please upgrade to a current xml file version\n");
		return false;
	} // if

	// detach userTransformationModel node from user node
	XmlElement* element = parent->removeSubElement("userTransformationModel");
	if (!element) {
		printd(ERROR,
				"UserTransConverterToV1_0a4::convert(): could not convert file because of missing node <userTransformationModel> in UserDatabase configuration file! Please upgrade to a current xml file version\n");
		return false;
	} // if

	// create new root node
	XmlElement* newDocumentRoot = new XmlElement("userTransformationModel");
	// attach element to new root node
	newDocumentRoot->addSubElement(element);

	// rename userTransformationModel node to model
	// attribute name and argument vector can be te same
	element->setName("model");

	// convert argument vector to current version
	XmlElement* arguments = element->getSubElement("arguments");
	if (arguments) {
		success = XmlArgumentVectorLoader::get().updateXmlElement(arguments, version,
				destinationVersion, configFile);
	} // if

	// replace old root node with new one
	XmlElement* oldRootElement = document->replaceRootElement(newDocumentRoot);

	// delete old root element
	assert(oldRootElement);
	delete oldRootElement;

	updateDocument(document, XmlConfigurationLoader::XmlDtdUrl + "userTransformationModel_v1.0a4.dtd",
			destinationVersion, "userTransformationModel");

	return success;
} // convert

