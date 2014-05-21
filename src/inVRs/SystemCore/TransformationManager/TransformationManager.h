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

#ifndef _TRANSFORMATIONMANAGER_H
#define _TRANSFORMATIONMANAGER_H

#include <vector>
#include <memory>
#include <sstream>

#include "../UserDatabase/UserDatabase.h"
#include "../ComponentInterfaces/NavigationInterface.h"
#include "../ComponentInterfaces/NetworkInterface.h"
#include "../../InputInterface/ControllerManagerInterface.h"
#include "../NetMessage.h"
#include "../EventManager/EventFactory.h"
#include "../EventManager/Event.h"
#include "../EventManager/EventManager.h"
#include "TransformationPipe.h"
#include "TransformationPipeMT.h"
#include "TransformationModifierFactory.h"
#include "TransformationLoggerModifier.h"
#include "TransformationMerger.h"

class TransformationManagerEventFactory;
class ArgumentVector;
class AverageMergerFactory;

/**
 * For internal use by TransformationManager.
 * \todo make this a protected member-class of TransformationManager
 */
class INVRS_SYSTEMCORE_API ModifierTemplate {
public:
	ModifierTemplate();
	~ModifierTemplate();

	TransformationModifierFactory* modifierFactory;
	ArgumentVector* argVector;
	TransformationModifier::OUTPUT outputMode;
};

/**
 * For internal use by TransformationManager.
 * \todo make this a protected member-class of TransformationManager
 */
class INVRS_SYSTEMCORE_API TimingData {
public:
	TimingData() {
		iterationsPerSecond = 0;
		delay = 0;
	}

	float iterationsPerSecond;
	float delay;
}; // TimingData

/**
 * For internal use by TransformationManager.
 * \todo make this a protected member-class of TransformationManager
 */
class INVRS_SYSTEMCORE_API PipeConfiguration {
public:
	PipeConfiguration();
	~PipeConfiguration();

	PipeSelector pipeGroup;

	std::vector<ModifierTemplate> modifierList;
	TimingData* timingData;
}; // PipeConfiguration

/**
 * For internal use by TransformationManager.
 * \todo make this a protected member-class of TransformationManager
 */
class INVRS_SYSTEMCORE_API MergerTemplate {
public:
	MergerTemplate();
	~MergerTemplate();

	TransformationMerger* merger;
	ArgumentVector* argVector;
}; // MergerTemplate

/**
 * For internal use by TransformationManager.
 * \todo make this a protected member-class of TransformationManager
 */
struct MergerData {
	unsigned index; // is index of modifier-List
	unsigned mergerId;
	uint64_t anymaskPipeId;
	uint64_t testPipeId;
}; // MergerData

/**
 * The TransformationManager
 * \todo write this class documentation
 * ... step
 * ... ControllerManager connection
 * ... NetworkInterface
 */
class INVRS_SYSTEMCORE_API TransformationManager {
public:
	/// marks a field of the pipeId an "don't care"
	static const unsigned ANYPIPE;
	/// XXX \todo  what exactly does this do? (PipeSelector treats this the same as ANYPIPE?!)
	static const unsigned EQUALPIPE;

	/**
	 * Initialize the TransformationManager.
	 * This method registers the standard TransformationModifiersFactories and
	 * TransformationMergersFactories of the TransformationManager.
	 * Furthermore it creates the connection to the EventManager and (if available)
	 * to the ControllerManager and Network modules.
	 * @note Call this method after EventManager::init(), but before EventManager::start()!
	 */
	static bool init();

	/**
	 * Delete and clear all TransformationPipes, and Factories.
	 * Also, send a UserDatabaseRemoveUserEvent.
	 * @note Call this method before EventManager::stop()!
	 * \todo RemoveUserEvent should not be sent from TransformationManager!
	 */
	static void cleanup();

	/**
	 * Find an already existing TransformationPipe matching the parameters.
	 * @return the matching TransformationPipe, or <code>NULL</code>, if none is found.
	 * @see TransformationManager::openPipe()
	 */
	static TransformationPipe* getPipe(unsigned srcId, unsigned dstId, unsigned pipeType,
			unsigned objectClass, unsigned objectType, unsigned objectId, unsigned priority,
			bool fromNetwork, User* user = NULL); //NULL means local user

	/**
	 * Load the pipe configuration.
	 * This method parses an XML configuration file and sets up the
	 * pipes, modifiers and mergers accordingly.
	 */
	static bool loadConfig(std::string configFile);

	/**
	 * Handle Events and NetworkMessages, generate tracking input (if available),
	 * and call execute(dt,interruptAt).
	 */
	static void step(float dt, unsigned interruptAt = 0);

	/**
	 * Execute all non-empty pipes which are eligible for execution.
	 * Any pipe with a <code>timeToNextExecution</code> <= 0 is eligible for execution.
	 * \todo find out about interrupt stuff
	 * @param dt time since last execution
	 * @param interruptAt \todo what does interruptAt mean?
	 */
	static void execute(float dt, unsigned interruptAt = 0);

	/**
	 * Create and open a TransformationPipe matching the parameters.
	 * @return a matching TransformationPipe
	 * @return <code>NULL</code>, if the same pipe has already been opened.
	 * @return <code>NULL</code>, if no matching TransformationPipe is found.
	 * @param srcId source module, as defined in ModuleIds.h
	 * @param dstId destination module, as defined in ModuleIds.h
	 * @param pipeType can be used to discriminate between otherwise identical pipes (e.g. HeadSensor vs. HandSensor)
	 * @param objectClass specifies  Entity, Tile, etc. (currently always 0 for Entity)
	 * @param objectType With an objectClass of 0 (=Entity), use EntityType::getId().
	 * @param objectId With an objectClass of 0 (=Entity), use Entity::getId().
	 * @param priority Minimum priority of the pipe (higher value == higher priority). Use different priorities to define the execution-order.
	 * @param fromNetwork Distinguishes local (<code>false</code>) and remote
	 *        (<code>true</code>) end of the pipe.
	 *        If <code>true</code>, the source of the pipe is the Network.
	 * @param user (set to <code>NULL</code> for localUser; defaults to <code>NULL</code>)
	 */
	static TransformationPipe* openPipe(unsigned srcId, unsigned dstId, unsigned pipeType,
			unsigned objectClass, unsigned objectType, unsigned objectId, unsigned priority,
			bool fromNetwork, User* user = NULL); //NULL means local user

	/**
	 * Create and open a threadsafe TransformationPipe matching the parameters.
	 * Behaves exactly like openPipe(), but the resulting pipe is of type TransformationPipeMT.
	 * @see TransformationManager::openPipe()
	 */
	static TransformationPipe* openMTPipe(unsigned srcId, unsigned dstId, unsigned pipeType,
			unsigned objectClass, unsigned objectType, unsigned objectId, unsigned priority,
			bool fromNetwork, User* user = NULL); //NULL means local user

	/**
	 * Close and delete the given TransformationPipe.
	 */
	static void closePipe(TransformationPipe* pipe);

	/**
	 * Close all TransformationPipes belonging to the given User.
	 */
	static void closeAllPipesFromUser(User* user);

	/**
	 * Encode the parameters of a TransformationPipe into a 64-bit unsigned integer.
	 * @see TransformationManager::openPipe()
	 * \todo move this into TransformationPipe?
	 */
	static uint64_t packPipeId(unsigned srcId, unsigned dstId, unsigned pipeType,
			unsigned objectClass, unsigned objectType, unsigned objectId, bool fromNetwork);

	/**
	 * Decode the parameters of a TransformationPipe from a 64-bit unsigned integer value
	 * generated by the method packPipeId().
	 * @see TransformationManager::openPipe()
	 * \todo move this into TransformationPipe?
	 */
	static void unpackPipeId(uint64_t id, unsigned* srcId, unsigned* dstId, unsigned* pipeType,
			unsigned* objectClass, unsigned* objectType, unsigned* objectId, bool* fromNetwork);

	/**
	 * Register a factory for a new TransformationModifier-type.
	 */
	static void registerModifierFactory(TransformationModifierFactory* factory);

	/**
	 * Register a factory for a new TransformationMerger-type.
	 */
	static void registerMergerFactory(TransformationMergerFactory* factory);

	/**
	 * Remove and delete the given TransformationMerger.
	 * \todo more info on this method? is it deprecated like registerMerger?
	 */
	static void removeMerger(TransformationMerger* merger);

private:
	static bool firstRun;
	static bool initialized;
	static NetworkInterface* network;
//	static ControllerManagerInterface* contInt;
	static User* localUser;
#if OSG_MAJOR_VERSION >= 2
	//OpenSG 2.0 prevents getting a raw C Pointer, also threading/sync objects are well ref counted
	static OSG::LockRefPtr pipeListLock;
#else //OpenSG1:
	static OSG::Lock* pipeListLock;
#endif

	static std::vector<TransformationPipe*> pipes;
	static std::vector<TransformationModifierFactory*> modifierFactories;
	static std::vector<TransformationMergerFactory*> mergerFactories;
	static std::vector<MergerTemplate*> mergerTemplates;
	static std::vector<MergerData*> mergerOrderList;
	static std::vector<TransformationMerger*> existingMerger;
	static std::vector<PipeConfiguration*> pipeConfigurationsList;
	static EventPipe* eventPipe;
	static TransformationLoggerModifierFactory* loggerModifierFactory;
	static unsigned interruptedPipePriority;

	static void getAllPipesFromUser(User* user, std::vector<TransformationPipe*>* dst);
	static unsigned& getValueFromAttribute(const XmlElement* xml, std::string name, unsigned& dst);

	static void executeEvents();
	static void handleNetworkMessages();

	/// \todo check what this method does, maybe it's deprecated!
	static void registerMerger(int mergerId, unsigned srcId, unsigned dstId, unsigned pipeType,
			unsigned objectClass, unsigned objectType, unsigned objectId, unsigned bFromNetwork,
			unsigned addBeforeIdx);

	static void decodeNetMsg(NetMessage* msg, TransformationData* transf, unsigned* userId,
			uint64_t* pipeId);
//	static void generateTrackingPipeInput();

	static TransformationMerger* createMerger(std::string name, ArgumentVector* attributes);
	static void addMergerToPipe(MergerData* data, TransformationPipe* pipe);

	static void readPipeMask(const XmlElement* xml, TransformationPipe::MASK* mask);
	static uint64_t
			generateAnyPipeMask(unsigned& srcId, unsigned& dstId, unsigned& pipeType,
					unsigned& objectClass, unsigned& objectType, unsigned& objectId,
					unsigned& bFromNetwork);

	/// 2008-04-21 ZaJ: \todo replace openPipe() and openPipeMT() with this function?
	static TransformationPipe* openPipeInternal(unsigned srcId, unsigned dstId, unsigned pipeType,
			unsigned objectClass, unsigned objectType, unsigned objectId, unsigned priority,
			bool fromNetwork, User* user, bool useMTPipe = false);

	static TransformationModifierFactory* findFactory(std::string className);

	static bool parseTransformationMergers(const std::auto_ptr<const XmlDocument>& document);

	static bool parseTransformationPipes(const std::auto_ptr<const XmlDocument>& document);

	static bool parseTransformationModifier(const XmlElement* modifierElement,
			TransformationPipe::MASK* pipeMask, PipeConfiguration* newPipeConfiguration);


//*****************************************************************************
// Configuration loading
//*****************************************************************************
private:
	static XmlConfigurationLoader xmlConfigLoader;

	class ConverterToV1_0a4 : public XmlConfigurationConverter {
	public:
		ConverterToV1_0a4();
		virtual bool convert(XmlDocument* document, const Version& version, std::string configFile);
	}; // ConverterToV1_0a4

}; // TransformationManager


inline std::string getTransformationPipeIdAsString(uint64_t id) {
	std::stringstream ss(std::stringstream::in | std::stringstream::out);
	unsigned srcId, dstId, pipeType, objectClass, objectType, objectId;
	bool fromNetwork;

	TransformationManager::unpackPipeId(id, &srcId, &dstId, &pipeType, &objectClass, &objectType, &objectId, &fromNetwork);

	ss << "Source Module: " << getModuleNameById(srcId) << std::endl;
	ss << "Destination Module: " << getModuleNameById(dstId) << std::endl;
	ss << "Pipe Type: " << pipeType << std::endl;
	ss << "Object Class: " << objectClass << std::endl;
	ss << "Object Type: " << objectType << std::endl;
	ss << "Object Id: " << objectId << std::endl;
	ss << "From Network: " << (fromNetwork ? "true" : "false") << std::endl;

	return ss.str();
} // getTransformationPipeIdAsString

#endif // _TRANSFORMATIONMANAGER_H
