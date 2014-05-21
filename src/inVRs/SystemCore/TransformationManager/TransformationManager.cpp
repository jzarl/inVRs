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
#include <assert.h>

#include "TransformationManager.h"
#include "TransformationModifier.h"
#include "TransformationManagerEvents.h"
#include "TrackingDataWriter.h"
#include "../../InputInterface/ControllerInterface.h"
#include "../ModuleIds.h"
#include "../DebugOutput.h"
#include "../XMLTools.h"
#include "TrackingDataWriter.h"
#include "CameraTransformationWriter.h"
#include "TrackingOffsetModifier.h"
#include "UserTransformationWriter.h"
#include "TransformationDistributionModifier.h"
#include "ApplyNavigationModifier.h"
#include "AvatarTransformationWriter.h"
#include "EntityTransformationWriter.h"
#include "ApplyCursorTransformationModifier.h"
#include "CursorTransformationWriter.h"
#include "ApplyUserTransformationModifier.h"
#include "TransformationLoggerModifier.h"
#include "AssociatedEntityInterrupter.h"
#include "MultiPipeInterrupter.h"
#include "TargetPipeTransformationWriter.h"
#include "../SystemCore.h"
#include "../UserDatabase/UserDatabaseEvents.h"
#include "../UtilityFunctions.h"

// disable deprecation warning for std::auto_ptr when std::unique_ptr is not available.
#ifndef HAS_CXX11_UNIQUE_PTR
// Only do this on GCC versions that support it:
# if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 402
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
#endif

OSG_USING_NAMESPACE

#if OSG_MAJOR_VERSION >= 2
OSG::LockRefPtr 									TransformationManager::pipeListLock = NULL;
#else //OpenSG1:
OSG::Lock* 									TransformationManager::pipeListLock = NULL;
#endif

const unsigned 								TransformationManager::ANYPIPE = 0x01000000;
const unsigned 								TransformationManager::EQUALPIPE = 0x02000000;

bool 										TransformationManager::firstRun = true;
bool 										TransformationManager::initialized = false;

NetworkInterface* 							TransformationManager::network = NULL;
User* 										TransformationManager::localUser = NULL;
std::vector<TransformationPipe*> 			TransformationManager::pipes;
EventPipe* 									TransformationManager::eventPipe = NULL;
unsigned 									TransformationManager::interruptedPipePriority = 0;
std::vector<TransformationModifierFactory*>	TransformationManager::modifierFactories;
std::vector<TransformationMergerFactory*>	TransformationManager::mergerFactories;
TransformationLoggerModifierFactory*		TransformationManager::loggerModifierFactory = NULL;
std::vector<MergerTemplate*>				TransformationManager::mergerTemplates;
std::vector<MergerData*> 					TransformationManager::mergerOrderList;
std::vector<TransformationMerger*> 			TransformationManager::existingMerger;
std::vector<PipeConfiguration*> 			TransformationManager::pipeConfigurationsList;

XmlConfigurationLoader 						TransformationManager::xmlConfigLoader;


ModifierTemplate::ModifierTemplate() {
	// 	modifier = NULL;
	argVector = NULL;
}

ModifierTemplate::~ModifierTemplate() {
	// 	if(modifier) delete modifier;
	// 	if(argVector) delete argVector;
}

PipeConfiguration::PipeConfiguration() {
	timingData = NULL;
}

PipeConfiguration::~PipeConfiguration() {
	int i;

	for (i = 0; i < (int)modifierList.size(); i++) {
		if (modifierList[i].argVector)
			delete modifierList[i].argVector;
		modifierList[i].argVector = NULL;
	}
	if (timingData)
		delete timingData;
}

MergerTemplate::MergerTemplate() {
	merger = NULL;
	argVector = NULL;
} // MergerData

	MergerTemplate::~MergerTemplate() {
		if (merger)
			delete merger;
		if (argVector)
			delete argVector;
	} // ~MergerData

bool TransformationManager::init() {
	if (initialized) {
		printd(INFO, "TransformationManager::init(): already initialized!\n");
		return false;
	} // if

	printd(INFO, "TransformationManager::init()!\n");
	localUser = UserDatabase::getLocalUser();
	eventPipe = EventManager::getPipe(TRANSFORMATION_MANAGER_ID);

	interruptedPipePriority = 0;

	EventManager::registerEventFactory("TransformationManagerOpenPipeEvent",
			new TransformationManagerOpenPipeEvent::Factory());
	EventManager::registerEventFactory("TransformationManagerClosePipeEvent",
			new TransformationManagerClosePipeEvent::Factory());

	registerModifierFactory(new TrackingDataWriterFactory);
	registerModifierFactory(new CameraTransformationWriterFactory);
	registerModifierFactory(new TrackingOffsetModifierFactory);
	registerModifierFactory(new UserTransformationWriterFactory);
	registerModifierFactory(new TransformationDistributionModifierFactory);
	registerModifierFactory(new ApplyNavigationModifierFactory);
	registerModifierFactory(new AvatarTransformationWriterFactory);
	registerModifierFactory(new EntityTransformationWriterFactory);
	registerModifierFactory(new ApplyCursorTransformationModifierFactory);
	registerModifierFactory(new ApplyUserTransformationModifierFactory);
	registerModifierFactory(new CursorTransformationWriterFactory);
	registerModifierFactory(new AssociatedEntityInterrupterFactory);
	registerModifierFactory(new MultiPipeInterrupterFactory);
	registerModifierFactory(new TargetPipeTransformationWriterFactory);
	loggerModifierFactory = new TransformationLoggerModifierFactory;
	registerModifierFactory(loggerModifierFactory);

	// register Config file converters
	xmlConfigLoader.registerConverter(new ConverterToV1_0a4);

#if OSG_MAJOR_VERSION >= 2
	pipeListLock = OSG::dynamic_pointer_cast<OSG::Lock> (ThreadManager::the()->getLock(
		"TransformationManagerPipeListLock",false));
#else //OpenSG1:
	pipeListLock = dynamic_cast<Lock*> (ThreadManager::the()->getLock(
				"TransformationManagerPipeListLock"));
#endif

	initialized = true;
	return true;
} // init

void TransformationManager::cleanup() {
	printd(INFO, "TransformationManager::cleanup(): begin ...\n");

	// TODO: RemoveUserEvent should not be sent from TransformationManager!!!

	if (localUser) {
		UserDatabaseRemoveUserEvent *removeUserEvent = new UserDatabaseRemoveUserEvent(localUser);
		EventManager::sendEvent(removeUserEvent, EventManager::EXECUTE_REMOTE);
	}
#if OSG_MAJOR_VERSION >= 2
	pipeListLock->acquire();
#else //OpenSG1:
	pipeListLock->aquire();
#endif

	for (int i = pipes.size() - 1; i >= 0; i--) {
		printd(INFO, "TransformationManager::cleanup(): closing pipe with id %s\n",
				getUInt64AsString(pipes[i]->pipeId).c_str());
		delete pipes[i];
	}
	pipes.clear();
	pipeListLock->release();

	// localUserTrackingPipeList is also affected by previous call
	//	localUserTrackingPipeList.clear();

	for (std::vector<PipeConfiguration*>::iterator it = pipeConfigurationsList.begin(); it
			!= pipeConfigurationsList.end(); ++it)
		delete *it;

	pipeConfigurationsList.clear();

	for (std::vector<TransformationModifierFactory*>::iterator it = modifierFactories.begin(); it
			!= modifierFactories.end(); ++it)
		delete *it;

	modifierFactories.clear();

	printd(INFO, "TransformationManager::cleanup(): done!\n");
} // cleanup

TransformationPipe* TransformationManager::getPipe(unsigned srcId, unsigned dstId,
		unsigned pipeType, unsigned objectClass, unsigned objectType, unsigned objectId,
		unsigned priority, bool fromNetwork, User* user) {

	TransformationPipe* ret = NULL;
	uint64_t pipeId;
	std::vector<TransformationPipe*>::iterator it;
	if (!user)
		user = UserDatabase::getLocalUser();

	pipeId = packPipeId(srcId, dstId, pipeType, objectClass, objectType, objectId, fromNetwork);

#if OSG_MAJOR_VERSION >= 2
	pipeListLock->acquire();
#else //OpenSG1:
	pipeListLock->aquire();
#endif
	it = pipes.begin();
	while (it != pipes.end()) {
		if ((*it)->pipeId == pipeId && (*it)->getOwner() == user) {
			ret = *it;
			break;
		} // if
		else
			++it;
	} // while
	pipeListLock->release();

	if (!ret) {
		printd(
				INFO,
				"TransformationManager::getPipe(): PIPE with ID %s FROM USER WITH ID %i NOT FOUND!!!\n",
				getUInt64AsString(pipeId).c_str(), user->getId());
	} // if
	return ret;
} // getPipe

bool TransformationManager::loadConfig(std::string configFile) {
	if (!initialized) {
		printd(WARNING,
				"TransformationManager::loadConfig(): TransformationManager not initialized yet - initializing now!\n");
		init();
	} // if

	std::string configFileConcatenatedPath = getConcatenatedPath(configFile, "TransformationManagerConfiguration");
	printd(INFO, "TransformationManager::loadConfig(): loading configuration %s!\n",
			configFile.c_str());

	// load xml document via xmlConfigLoader
	std::auto_ptr<const XmlDocument> document(
			xmlConfigLoader.loadConfiguration(configFileConcatenatedPath));
	if (!document.get()) {
		printd(ERROR,
				"TransformationManager::loadConfig(): error at loading of configuration file %s!\n",
				configFile.c_str());
		return false;
	} // if

	bool success = true;

	// parse logFile and store value in loggerModifierFactory
	if (document->hasAttribute("transformationManager.logFile.name")) {
		std::string logFile = document->getAttributeValue("transformationManager.logFile.name");
		assert(loggerModifierFactory);
		loggerModifierFactory->setLogFile(logFile);
	} // if

	// parse mergerList
	success = parseTransformationMergers(document) && success;
	// parse pipeList
	success = parseTransformationPipes(document) && success;

	// TODO: support for timing should be added in future versions

	return success;
} // loadConfig


void TransformationManager::step(float dt, unsigned interruptAt) {

	if (interruptedPipePriority != 0) {
		execute(dt, interruptAt);
		return;
	} // if

	if (firstRun) {
		network = (NetworkInterface*)SystemCore::getModuleByName("Network");
		if (!network) {
			printd(INFO,
					"TransformationManager::init(): Module Network not found! TransformationManager will run in single-user mode!\n");
		} // if
		firstRun = false;
	} // if

	// Execute events
	if (eventPipe)
		executeEvents();

	if (network)
		handleNetworkMessages();

	execute(dt, interruptAt);
} // step

void TransformationManager::execute(float dt, unsigned interruptAt) {
	int i, pipeIdx;

	if (interruptedPipePriority != 0) {
		for (i = pipes.size() - 1; i >= 0; i--) {
			if (pipes[i]->priority < interruptedPipePriority)
				break;
		} // for
		pipeIdx = i;
	} // if
	else
		pipeIdx = pipes.size() - 1;

	for (i = pipeIdx; i >= 0; i--) {
		if (pipes[i]->priority < interruptAt)
			break;

		pipes[i]->timeToNextExecution -= dt;

		if (pipes[i]->size() == 0) {
			// skip empty pipes (this might delay the execution of a merger)
			continue;
		} // if

		if (pipes[i]->timeToNextExecution <= 0.0f) {
			pipes[i]->execute();
			pipes[i]->flush();
			pipes[i]->timeToNextExecution += pipes[i]->executionInterval;

			// TODO: check why this is reset to zero!!!
			if (pipes[i]->timeToNextExecution < 0.0f)
				pipes[i]->timeToNextExecution = 0.0f;
		} // if
	} // for
	interruptedPipePriority = interruptAt;
} // execute

TransformationPipe* TransformationManager::openPipe(unsigned srcId, unsigned dstId,
		unsigned pipeType, unsigned objectClass, unsigned objectType, unsigned objectId,
		unsigned priority, bool fromNetwork, User* user) {

	return openPipeInternal(srcId, dstId, pipeType, objectClass, objectType, objectId, priority,
			fromNetwork, user, false);
} // openPipe

TransformationPipe* TransformationManager::openMTPipe(unsigned srcId, unsigned dstId,
		unsigned pipeType, unsigned objectClass, unsigned objectType, unsigned objectId,
		unsigned priority, bool fromNetwork, User* user) {

	return openPipeInternal(srcId, dstId, pipeType, objectClass, objectType, objectId, priority,
			fromNetwork, user, true);
} // openPipe

void TransformationManager::closePipe(TransformationPipe* pipe) {
	std::vector<TransformationPipe*>::iterator it;

	if (pipe == NULL) {
		printd(WARNING, "TransformationManager::closePipe(): attemting to close NULL pointer\n");
		return;
	}

	bool foundPipe = false;

#if OSG_MAJOR_VERSION >= 2
	pipeListLock->acquire();
#else //OpenSG1:
	pipeListLock->aquire();
#endif
	for (it = pipes.begin(); it != pipes.end(); ++it) {
		if ((*it) == pipe) {
			printd(INFO, "TransformationManager::closePipe(): closing pipe with id %s\n",
					getUInt64AsString(pipe->pipeId).c_str());
			pipes.erase(it);
			printd(INFO, "TransformationManager::closePipe(): deleting Pipe.\n");
			delete pipe;
			printd(INFO, "TransformationManager::closePipe(): Pipe deleted.\n");
			foundPipe = true;
			break;
		} // if
	} // for
	pipeListLock->release();

	if (!foundPipe)
		printd(WARNING, "TransformationManager::closePipe(): couldn't find a pipe with id %s\n",
				getUInt64AsString(pipe->pipeId).c_str());
} // closePipe

void TransformationManager::closeAllPipesFromUser(User* user) {
	std::vector<TransformationPipe*> pipesFromUser;
	std::vector<TransformationPipe*>::iterator it;

	getAllPipesFromUser(user, &pipesFromUser);

	for (it = pipesFromUser.begin(); it != pipesFromUser.end(); ++it)
		closePipe(*it);

	loggerModifierFactory->dump();
}

uint64_t TransformationManager::packPipeId(unsigned srcId, unsigned dstId, unsigned pipeType,
		unsigned objectClass, unsigned objectType, unsigned objectId, bool fromNetwork) {
	uint64_t ret;

	assert(srcId<=0x7F);
	assert(dstId<=0x7F);
	assert(pipeType<=0xFF);
	assert(objectClass<=0xFF);
	assert(objectType<=0xFFFF);
	assert(objectId<=0xFFFF);

	ret = fromNetwork ? 1 : 0;
	ret |= (uint64_t)srcId << 1;
	ret |= (uint64_t)dstId << 8;
	ret |= (uint64_t)pipeType << 16;
	ret |= (uint64_t)objectClass << 24;
	ret |= (uint64_t)objectType << 32;
	ret |= (uint64_t)objectId << 48;

	return ret;
} // packPipeId

void TransformationManager::unpackPipeId(uint64_t id, unsigned* srcId, unsigned* dstId,
		unsigned* pipeType, unsigned* objectClass, unsigned* objectType, unsigned* objectId,
		bool* fromNetwork) {

	*fromNetwork = (id & 1) != 0;
	//	*srcId = (unsigned)(id >> 1) & 0x7F;
	//	*dstId = (unsigned)(id >> 8) & 0x7F;
	//	*pipeType = (unsigned)(id >> 16) & 0xFF;
	//	*objectClass = (unsigned)(id >> 24) & 0xFF;
	//	*objectType = (unsigned)(id >> 32) & 0xFFFF;
	//	*objectId = (unsigned)(id >> 48); //&0xFFFF;
	// Downcast from UInt64 to unsigned via 0xFFFFFFFF because of errors on windows system!
	*srcId = (0xFFFFFFFF & (id >> 1)) & 0x7F;
	*dstId = (0xFFFFFFFF & (id >> 8)) & 0x7F;
	*pipeType = (0xFFFFFFFF & (id >> 16)) & 0xFF;
	*objectClass = (0xFFFFFFFF & (id >> 24)) & 0xFF;
	*objectType = (0xFFFFFFFF & (id >> 32)) & 0xFFFF;
	*objectId = (0xFFFFFFFF & (id >> 48)); //&0xFFFF;
} // unpackPipeId

void TransformationManager::registerModifierFactory(TransformationModifierFactory* factory) {
	modifierFactories.push_back(factory);
} // registerModifierFactory

void TransformationManager::registerMergerFactory(TransformationMergerFactory* factory) {
	mergerFactories.push_back(factory);
} // registerMergerFactory

void TransformationManager::removeMerger(TransformationMerger* merger) {
	int i;
	for (i = 0; i < (int)existingMerger.size(); i++) {
		if (existingMerger[i] == merger)
			break;
	} // for
	if (i < (int)existingMerger.size())
		existingMerger.erase(existingMerger.begin() + i);
	else
		printd(WARNING,
				"TransformationManager::removeMerger(): Merger not found in TransformationManager!\n");
} // removeMerger

void TransformationManager::getAllPipesFromUser(User* user, std::vector<TransformationPipe*>* dst) {
	int i;

	dst->clear();
	for (i = 0; i < (int)pipes.size(); i++) {
		if (pipes[i]->getOwner() == user)
			dst->push_back(pipes[i]);
	}
}

unsigned& TransformationManager::getValueFromAttribute(const XmlElement* xml, std::string name,
		unsigned& dst) {
	std::string tempString = xml->getAttributeValue(name.c_str());
	if (stringEqualsAny(tempString))
		dst = TransformationManager::ANYPIPE;
	else if (tempString == "Equals" || tempString == "equals" || tempString == "EQUALS")
		dst = TransformationManager::EQUALPIPE;
	else
		dst = atoi(tempString.c_str());
	return dst;
} // getValueFromAttribute

void TransformationManager::executeEvents() {
	std::deque<Event*>* eventRecvQ = NULL;
	Event * event;

	eventRecvQ = eventPipe->makeCopyAndClear();
	for (int i = 0; i < (int)eventRecvQ->size(); i++) {
		event = (*eventRecvQ)[i];
		event->execute();
		delete event;
	} // for
	delete eventRecvQ;
} // executeEvents

void TransformationManager::handleNetworkMessages() {
	TransformationData netData;
	unsigned netUserId;
	uint64_t netPipeId;
	NetMessage* msg;
	std::vector<NetMessage*> msgList;
	std::vector<TransformationPipe*> pipesFromUser;
	User* remoteUser;
	int i;

	network->popAll(TRANSFORMATION_MANAGER_ID, &msgList);
	while (!msgList.empty()) {
		msg = msgList[0];
		decodeNetMsg(msg, &netData, &netUserId, &netPipeId);

		netPipeId |= 1; // set network bit
		remoteUser = UserDatabase::getUserById(netUserId);
		if (remoteUser == localUser)
			printd(
					WARNING,
					"TransformationManager::handleNetworkMessages(): Found an incoming transformation from a remote pipe whose owner is localUser!\n");
		pipesFromUser.clear();
		getAllPipesFromUser(remoteUser, &pipesFromUser);

		if (pipesFromUser.size() == 0)
			printd(INFO,
					"TransformationManager::step(): user %u does not have any pipes attached\n",
					netUserId);

		for (i = 0; i < (int)pipesFromUser.size(); i++) {
			if (pipesFromUser[i]->getPipeId() == netPipeId) {
				// 					printd(INFO, "TransformationManager::run(): found data for TransformationPipe with ID %s!\n", getUInt64AsString(netPipeId).c_str());
				pipesFromUser[i]->push_back(netData);
				break;
			} // if
		} // for

		if (i == (int)pipesFromUser.size()) {
			printd(INFO,
					"TransformationManager::step(): cannot find any pipe with id %s owned by user %u!\n",
					getUInt64AsString(netPipeId).c_str(), netUserId);
		} // if

		msgList.erase(msgList.begin());
		delete msg;
	} // while
} // handleNetworkMessages


void TransformationManager::registerMerger(int mergerId, unsigned srcId, unsigned dstId,
		unsigned pipeType, unsigned objectClass, unsigned objectType, unsigned objectId,
		unsigned bFromNetwork, unsigned addBeforeIdx) {
	MergerData* data = new MergerData;
	// 	data->index = modifierTemplates.size();
	data->index = addBeforeIdx;
	data->mergerId = mergerId;
	data->anymaskPipeId = generateAnyPipeMask(srcId, dstId, pipeType, objectClass, objectType,
			objectId, bFromNetwork);
	data->testPipeId = packPipeId(srcId, dstId, pipeType, objectClass, objectType, objectId,
			bFromNetwork != 0);
	mergerOrderList.push_back(data);
} // registerMerger

void TransformationManager::decodeNetMsg(NetMessage* msg, TransformationData* transf,
		unsigned* userId, uint64_t* pipeId) {
	msg->getUInt32(*userId);
	*pipeId = msg->getUInt64();
	*transf = readTransformationFrom(msg);
}

TransformationMerger* TransformationManager::createMerger(std::string name,
		ArgumentVector* attributes) {
	int i;
	TransformationMerger* result = NULL;
	for (i = 0; i < (int)mergerFactories.size(); i++) {
		result = mergerFactories[i]->create(name, attributes);
		if (result)
			break;
	} // for
	if (!result)
		printd(ERROR,
				"TransformationManager::createMerger(): ERROR: no Factory for Merger of type %s found!\n",
				name.c_str());

	return result;
} // createMerger

void TransformationManager::addMergerToPipe(MergerData* data, TransformationPipe* pipe) {
	int i, k;
	int numMergers = (int)existingMerger.size();
	TransformationMerger* mergerForPipe = NULL;
	for (i = 0; i < numMergers; i++) {
		if (existingMerger[i]->getId() == (int)data->mergerId && existingMerger[i]->addInputPipe(
					pipe)) {
			mergerForPipe = existingMerger[i];
			break;
		} // if
	} // for

	if (!mergerForPipe) {
		for (i = 0; i < (int)mergerTemplates.size(); i++) {
			if (mergerTemplates[i]->merger->getId() == (int)data->mergerId) {
				mergerForPipe = createMerger(mergerTemplates[i]->merger->getName(),
						mergerTemplates[i]->argVector);
				assert(mergerForPipe);

				TransformationPipe::MASK tempMask;
				for (k = 0; k < mergerTemplates[i]->merger->getInputMaskListSize(); k++) {
					tempMask = mergerTemplates[i]->merger->getInputMask(k);
					mergerForPipe->addInputMask(&tempMask);
				}
				for (k = 0; k < mergerTemplates[i]->merger->getOutputMaskListSize(); k++) {
					tempMask = mergerTemplates[i]->merger->getOutputMask(k);
					mergerForPipe->addOutputMask(&tempMask);
				}
				mergerForPipe->setId(mergerTemplates[i]->merger->getId());

				if (!mergerForPipe->addInputPipe(pipe)) {
					printd(
							ERROR,
							"TransformationManager::addMergerToPipe(): merger did not accept pipe! Are the input masks correct?\n");
					assert(false);
				}
				break;
			} // if
		} // if

		if (mergerForPipe)
			existingMerger.push_back(mergerForPipe);
		else {
			printd(
					ERROR,
					"TransformationManager::addMergerToPipe(): Merger with ID %i could not be created!\n",
					data->mergerId);
			assert(false);
		} // else
	} // if

	if (!pipe->setMerger(mergerForPipe))
		printd(
				ERROR,
				"TransformationManager::addMergerToPipe(): ERROR: Merger with ID %i could not be added to Pipe!!!\n",
				data->mergerId);
} // addMergerToPipe

void TransformationManager::readPipeMask(const XmlElement* xml, TransformationPipe::MASK* mask) {
	if (xml->hasAttribute("srcComponentName"))
		mask->individual.srcId = getModuleIdByName(xml->getAttributeValue("srcComponentName"));
	else
		getValueFromAttribute(xml, "srcComponent", mask->individual.srcId);
	if (xml->hasAttribute("dstComponentName"))
		mask->individual.dstId = getModuleIdByName(xml->getAttributeValue("dstComponentName"));
	else
		getValueFromAttribute(xml, "dstComponent", mask->individual.dstId);
	getValueFromAttribute(xml, "pipeType", mask->individual.pipeType);
	getValueFromAttribute(xml, "objectClass", mask->individual.objectClass);
	getValueFromAttribute(xml, "objectType", mask->individual.objectType);
	getValueFromAttribute(xml, "objectId", mask->individual.objectId);
	getValueFromAttribute(xml, "fromNetwork", mask->individual.bFromNetwork);
} // readPipeMask

uint64_t TransformationManager::generateAnyPipeMask(unsigned& srcId, unsigned& dstId,
		unsigned& pipeType, unsigned& objectClass, unsigned& objectType, unsigned& objectId,
		unsigned& bFromNetwork) {
	uint64_t anyPipeMask = (uint64_t)(-1);

	if (bFromNetwork & ANYPIPE) {
		anyPipeMask &= ~(uint64_t)1;
		bFromNetwork = 0;
	}
	if (srcId & ANYPIPE) {
		anyPipeMask &= ~(uint64_t)0xFE;
		srcId = 0;
	}
	if (dstId & ANYPIPE) {
		anyPipeMask &= ~(uint64_t)0xFE00;
		dstId = 0;
	}
	if (pipeType & ANYPIPE) {
		anyPipeMask &= ~(uint64_t)0xFF0000;
		pipeType = 0;
	}
	if (objectClass & ANYPIPE) {
		anyPipeMask &= ~(uint64_t)0xFF000000;
		objectClass = 0;
	}
	if (objectType & ANYPIPE) {
		anyPipeMask &= ~((uint64_t)0xFFFF << 32);
		objectType = 0;
	}
	if (objectId & ANYPIPE) {
		anyPipeMask &= ~((uint64_t)0xFFFF << 48);
		objectId = 0;
	}

	return anyPipeMask;
} // generateAnyPipeMask

TransformationPipe* TransformationManager::openPipeInternal(unsigned srcId, unsigned dstId,
		unsigned pipeType, unsigned objectClass, unsigned objectType, unsigned objectId,
		unsigned priority, bool fromNetwork, User* user, bool useMTPipe) {

	TransformationPipe* ret;
	int i, j;
	uint64_t pipeId;
	MergerData* mergerData = NULL;
	pipeId = packPipeId(srcId, dstId, pipeType, objectClass, objectType, objectId, fromNetwork);

	for (i = 0; i < (int)pipes.size(); i++) {
		if (pipes[i]->pipeId == pipeId && pipes[i]->getOwner() == user) {
			ret = pipes[i];
			printd(WARNING,
					"TransformationManager::openPipe(): found two pipes with same id / pipe already opened!\n");
			return NULL;
		}
	}

	if (user == NULL)
		user = localUser;

	if (useMTPipe)
		ret = new TransformationPipeMT(pipeId, user);
	else
		ret = new TransformationPipe(pipeId, user);

	ret->setFlushStrategy(TransformationPipe::FLUSHSTRATEGY_QUORUM, 10);

	for (i = 0; i < (int)mergerOrderList.size(); i++) {
		if ((mergerOrderList[i]->anymaskPipeId & pipeId) == mergerOrderList[i]->testPipeId) {
			mergerData = mergerOrderList[i];
			break;
		} // if
	} // for

	bool success = false;
	//	bool inOneGroup = false;
	for (i = 0; i < (int)pipeConfigurationsList.size(); i++) {
		PipeConfiguration* pipeConf;
		pipeConf = pipeConfigurationsList[i];

		if (pipeConf->pipeGroup.selects(pipeId)) {

			printd(INFO,
					"TransformationManager::openPipeInternal(): pipe %s matches configuration %d\n",
					getUInt64AsString(pipeId).c_str(), i);

			// allow one pipe to be in multiple groups (first configuration is used first!)
			//			if (inOneGroup) {
			//				printd(ERROR,
			//						"TransformationManager::openPipe(): pipe belongs to multiple groups!\n");
			//				delete ret;
			//				return NULL;
			//			} // if

			TransformationModifier* modifier;
			for (j = 0; j < (int)pipeConf->modifierList.size(); j++) {
				if (mergerData && ((int)mergerData->index == j)) {
					printd(INFO,
							"TransformationManager::openPipeInternal(): pipe %s, adding merger at stage %d\n",
							getUInt64AsString(pipeId).c_str(), j);
					addMergerToPipe(mergerData, ret);
				} // if

				modifier = pipeConf->modifierList[j].modifierFactory->create(
						pipeConf->modifierList[j].argVector, &pipeConf->pipeGroup);
				assert(modifier);

				modifier->setOutputMode(pipeConf->modifierList[j].outputMode);

				printd(INFO, "TransformationManager::openPipe(): pipe %s: adding modifier %s\n",
						getUInt64AsString(pipeId).c_str(), modifier->getClassName().c_str());
				ret->addStage(modifier);
			}

			if (pipeConf->timingData) {
				printd(INFO,
						"TransformationManager::openPipe(): Setting executionInterval %f to Pipe with ID %s\n",
						pipeConf->timingData->iterationsPerSecond,
						getUInt64AsString(pipeId).c_str());
			}

			//			inOneGroup = true;
			success = true;
			break;
		}
	}

	if (!success) {
		printd(WARNING,
				"TransformationManager::openPipe(): no pipe configuration for pipe with ID %s found:\n%s",
				getUInt64AsString(pipeId).c_str(), getTransformationPipeIdAsString(pipeId).c_str());
		delete ret;
		return NULL;
	} // if

#if OSG_MAJOR_VERSION >= 2
	pipeListLock->acquire();
#else //OpenSG1:
	pipeListLock->aquire();
#endif
	for (i = 0; i < (int)pipes.size(); i++) {
		if (pipes[i]->priority >= priority) {
			// pipe with same priority already in the vector?
			if (pipes[i]->priority == priority) {
				// 				printd(INFO, "TransformationManager::openPipe(): priority of pipe %s has been increased\n", getUInt64AsString(pipeId).c_str());;
				priority++;
				continue;
			}

			// add new pipe before pipe with higher priority value
			pipes.insert(pipes.begin() + i, ret);
			ret->priority = priority;
			break;
		}
	}
	if (i == (int)pipes.size()) {
		ret->priority = priority;
		pipes.push_back(ret);
	}
	pipeListLock->release();

	printd(INFO,
			"TransformationManager::openPipe(): opened new Pipe from user with id %u and with PipeID %s\n",
			user->getId(), getUInt64AsString(pipeId).c_str());

	return ret;
}

TransformationModifierFactory* TransformationManager::findFactory(std::string className) {
	int i;
	for (i = 0; i < (int)modifierFactories.size(); i++) {
		if (modifierFactories[i]->getClassName() == className)
			return modifierFactories[i];
	} // for

	return NULL;
} // findFactory


bool TransformationManager::parseTransformationMergers(
		const std::auto_ptr<const XmlDocument>& document) {
	bool success = true;

	// parse all mergers
	std::vector<const XmlElement*> elements = document->getElements("transformationManager.mergerList.merger");
	std::vector<const XmlElement*>::iterator elementIt;
	const XmlElement* mergerElement;
	std::string mergerType;
	int mergerId;
	TransformationMerger* merger;
	ArgumentVector* arguments;
	MergerTemplate* mergerTemplate;
	for (elementIt = elements.begin(); elementIt != elements.end(); ++elementIt) {
		mergerElement = *elementIt;
		if (!mergerElement->hasSubElement("inputPipe") || !mergerElement->hasSubElement("outputPipe")) {
			printd(ERROR,
					"TransformationManager::loadConfig(): Found merger with missing <inputPipe> or <outputPipe> element! Please fix your TransformationManager configuration!\n");
			success = false;
			continue;
		} // if
		mergerType = mergerElement->getAttributeValue("type");
		mergerId = mergerElement->getAttributeValueAsInt("id");

		// parse arguments
		if (mergerElement->hasSubElement("arguments")) {
			arguments = readArgumentsFromXmlElement(mergerElement);
		} // if
		else {
			arguments = NULL;
		} // else

		// create merger
		merger = createMerger(mergerType, arguments);
		if (!merger) {
			printd(ERROR,
					"TransformationManager::loadConfig(): Unable to create merger of type %s! Please fix your TransformationManager configuration!\n",
					mergerType.c_str());
			success = false;
			continue;
		} // if
		merger->setId(mergerId);

		// parse all input pipes
		std::vector<const XmlElement*> inputPipeElements =
			mergerElement->getSubElements("inputPipe");
		std::vector<const XmlElement*>::iterator inputPipeIt;
		const XmlElement* inputPipeElement;
		TransformationPipe::MASK* pipeMask;
		for (inputPipeIt = inputPipeElements.begin(); inputPipeIt != inputPipeElements.end();
				++inputPipeIt) {
			inputPipeElement = *inputPipeIt;
			pipeMask = new TransformationPipe::MASK;
			readPipeMask(inputPipeElement, pipeMask);
			merger->addInputMask(pipeMask);
		} // for

		// parse all output pipes
		std::vector<const XmlElement*> outputPipeElements =
			mergerElement->getSubElements("outputPipe");
		std::vector<const XmlElement*>::iterator outputPipeIt;
		const XmlElement* outputPipeElement;
		for (outputPipeIt = outputPipeElements.begin(); outputPipeIt != outputPipeElements.end();
				++outputPipeIt) {
			outputPipeElement = *outputPipeIt;
			pipeMask = new TransformationPipe::MASK;
			readPipeMask(outputPipeElement, pipeMask);
			merger->addOutputMask(pipeMask);
		} // for

		mergerTemplate = new MergerTemplate;
		mergerTemplate->merger = merger;
		mergerTemplate->argVector = arguments;
		mergerTemplates.push_back(mergerTemplate);
	} // for

	return success;
} // parseTransformationMergers

bool TransformationManager::parseTransformationPipes(
		const std::auto_ptr<const XmlDocument>& document) {
	bool success = true;

	std::vector<const XmlElement*> elements = document->getElements("transformationManager.pipeList.pipe");
	std::vector<const XmlElement*>::iterator elementIt;
	const XmlElement* pipeElement;
	TransformationPipe::MASK* pipeMask;
	PipeConfiguration* newPipeConfiguration;
	std::vector<const XmlElement*> modifierElements;
	std::vector<const XmlElement*>::iterator modifierElementIt;
	const XmlElement* modifierElement;
	for (elementIt = elements.begin(); elementIt != elements.end(); ++elementIt) {
		pipeElement = *elementIt;
		pipeMask = new TransformationPipe::MASK;
		readPipeMask(pipeElement, pipeMask);
		newPipeConfiguration = new PipeConfiguration();
		newPipeConfiguration->pipeGroup.setup(pipeMask);
		modifierElements = pipeElement->getSubElements("modifier");
		for (modifierElementIt = modifierElements.begin();
				modifierElementIt != modifierElements.end(); ++ modifierElementIt) {
			modifierElement = *modifierElementIt;
			success = parseTransformationModifier(modifierElement, pipeMask, newPipeConfiguration)
				&& success;
		} // for
		pipeConfigurationsList.push_back(newPipeConfiguration);
		delete pipeMask;
	} // for

	return success;
} // parseTransformationPipes

bool TransformationManager::parseTransformationModifier(const XmlElement* modifierElement,
		TransformationPipe::MASK* pipeMask, PipeConfiguration* newPipeConfiguration) {

	bool success = true;
	std::string modifierType;
	std::string outputMode = "";
	ArgumentVector* arguments = NULL;
	int mergerId;
	ModifierTemplate modifierTemplate;

	// read the type of the modifier
	modifierType = modifierElement->getAttributeValue("type");

	// read the outputMode (if set)
	if (modifierElement->hasAttribute("outputMode")) {
		outputMode = modifierElement->getAttributeValue("outputMode");
	} // if

	// read modifier arguments if existing
	if (modifierElement->hasSubElement("arguments")) {
		arguments = readArgumentsFromXmlElement(modifierElement);
	} // if

	if (modifierType == "Merger") {
		if (arguments->keyExists("id")) {
			mergerId = arguments->get("id", mergerId);
			registerMerger(mergerId, pipeMask->individual.srcId,
					pipeMask->individual.dstId, pipeMask->individual.pipeType,
					pipeMask->individual.objectClass, pipeMask->individual.objectType,
					pipeMask->individual.objectId, pipeMask->individual.bFromNetwork,
					newPipeConfiguration->modifierList.size());
		} // if
		else {
			printd(ERROR,
					"TransformationManager::parseTransformationModifier(): modifier Merger has no argument id! Please fix your TransformationManager configuration!\n");
			success = false;
		} // if
	} // if
	else {
		if (outputMode == "PASSTHROUGH") {
			modifierTemplate.outputMode = TransformationModifier::OUTPUT_PASSTHROUGH;
		} // if
		else {
			// default outputMode:
			modifierTemplate.outputMode = TransformationModifier::OUTPUT_RESULT;
		} // else

		modifierTemplate.argVector = arguments;
		modifierTemplate.modifierFactory = findFactory(modifierType);
		if (modifierTemplate.modifierFactory) {
			newPipeConfiguration->modifierList.push_back(modifierTemplate);
		} // if
		else {
			printd(ERROR,
					"TransformationManager::parseTransformationModifier(): couldn't find factory for modifier of type %s!\n",
					modifierType.c_str());
			success = false;
		} // else
	} // else

	return success;
} // parseTransformationModifier

//*****************************************************************************
// Configuration loading
//*****************************************************************************
TransformationManager::ConverterToV1_0a4::ConverterToV1_0a4() :
	XmlConfigurationConverter("0.0", "1.0a4") {
	} // ConverterToV1_0a4

bool TransformationManager::ConverterToV1_0a4::convert(XmlDocument* document,
		const Version& version, std::string configFile) {

	if (document->getRootElement()->getName() != "transformationManager") {
		printd(ERROR,
				"TransformationManager::ConverterToV1_0a4::convert(): could not convert file because of invalid root node <%s> in TransformationManager configuration file! Please upgrade to a current xml file version!\n",
				document->getRootElement()->getName().c_str());
		return false;
	} // if

	bool success = true;

	updateDocument(document, XmlConfigurationLoader::XmlDtdUrl + "transformationManager_v1.0a4.dtd",
			destinationVersion, "transformationManager");

	// change logFile attribute path to name
	document->renameAttributes("transformationManager.logFile.path", "name");

	// add mergerList element if missing
	XmlElement* rootElement = document->getRootElement();
	XmlElement* mergerListElement = rootElement->getSubElement("mergerList");
	if (!mergerListElement) {
		mergerListElement = new XmlElement("mergerList");
		rootElement->addSubElement(mergerListElement);
	} // if
	// add all merger elements below mergerList
	std::vector<XmlElement*> elements = rootElement->getSubElements("merger");
	for (int i=0; i < elements.size(); i++) {
		mergerListElement->addSubElement(elements[i]);
		rootElement->removeSubElement(elements[i]);
	} // for

	// add pipeList element if missing
	XmlElement* pipeListElement = rootElement->getSubElement("pipeList");
	if (!pipeListElement) {
		pipeListElement = new XmlElement("pipeList");
		rootElement->addSubElement(pipeListElement);
	} // if
	// add all pipe elements below pipeList
	elements = rootElement->getSubElements("pipe");
	for (int i=0; i < elements.size(); i++) {
		pipeListElement->addSubElement(elements[i]);
		rootElement->removeSubElement(elements[i]);
	} // for

	// fix order of sub elements
	std::vector<std::string> subElementOrder;
	subElementOrder.push_back("logFile");
	subElementOrder.push_back("mergerList");
	subElementOrder.push_back("pipeList");
	rootElement->fixSubElementOrder(subElementOrder);

	// fix all argument vectors
	elements = rootElement->getSubElements("mergerList.merger.arguments");
	for (int i=0; i < elements.size(); i++) {
		success = XmlArgumentVectorLoader::get().updateXmlElement(elements[i], version,
				destinationVersion, configFile) && success;
	} // for
	elements = rootElement->getSubElements("pipeList.pipe.modifier.arguments");
	for (int i=0; i < elements.size(); i++) {
		success = XmlArgumentVectorLoader::get().updateXmlElement(elements[i], version,
				destinationVersion, configFile) && success;
	} // for

	// rename srcModule, dstModule, ... to srcComponent, dstComponent, ...
	document->renameAttributes("transformationManager.pipeList.pipe.srcModule", "srcComponent");
	document->renameAttributes("transformationManager.pipeList.pipe.srcModuleName",
			"srcComponentName");
	document->renameAttributes("transformationManager.pipeList.pipe.dstModule", "dstComponent");
	document->renameAttributes("transformationManager.pipeList.pipe.dstModuleName",
			"dstComponentName");

	document->renameAttributes("transformationManager.mergerList.merger.inputPipe.srcModule",
			"srcComponent");
	document->renameAttributes("transformationManager.mergerList.merger.inputPipe.srcModuleName",
			"srcComponentName");
	document->renameAttributes("transformationManager.mergerList.merger.inputPipe.dstModule",
			"dstComponent");
	document->renameAttributes("transformationManager.mergerList.merger.inputPipe.dstModuleName",
			"dstComponentName");

	document->renameAttributes("transformationManager.mergerList.merger.outputPipe.srcModule",
			"srcComponent");
	document->renameAttributes("transformationManager.mergerList.merger.outputPipe.srcModuleName",
			"srcComponentName");
	document->renameAttributes("transformationManager.mergerList.merger.outputPipe.dstModule",
			"dstComponent");
	document->renameAttributes("transformationManager.mergerList.merger.outputPipe.dstModuleName",
			"dstComponentName");

	return success;
} // convert
