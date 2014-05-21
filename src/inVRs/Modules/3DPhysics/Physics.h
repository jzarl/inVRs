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

#ifndef _PHYSICS_H
#define _PHYSICS_H

#include <OpenSG/OSGLock.h>
#include <OpenSG/OSGBaseThread.h>
#include "PhysicsObjectInterface.h"
#include "PhysicsObjectManagerFactory.h"
#include "SynchronisationModelFactory.h"
#include "SimulationStepListenerInterface.h"
#include "SystemThreadListenerInterface.h"
#include "UserData.h"
#include "3DPhysicsSharedLibraryExports.h"
#include <inVRs/SystemCore/Timer.h>
#ifdef INVRS_BUILD_TIME
#include "oops/Simulation.h"
#include "oops/XMLLoader.h"
#else
#include <inVRs/Modules/3DPhysics/oops/Simulation.h>
#include <inVRs/tools/libraries/oops/XMLLoader.h>
#endif
#include <inVRs/SystemCore/ComponentInterfaces/ModuleInterface.h>
#include <inVRs/SystemCore/EventManager/EventManager.h>
#include <inVRs/SystemCore/XmlConfigurationLoader.h>

//using namespace oops;

enum MESSAGE_TYPES {
	MSGTYPE_SYNC = 1,
	MSGTYPE_CLIENTINPUT = 2
};

/** inVRs module for multi-user physics simulation.
 * The Physics class is an inVRs module for physics simulation in a networked
 * multi-user virtual environment. The physics simulation is based on the
 * oops library which is an object oriented wrapper to the Open Dynamics Engine
 * (ODE). The distribution of the simulation is managed by a plugable
 * ObjectManager which handles the synchronisation of the physics data. The
 * plugin-mechanism allows the module to be used in any network structure.
 */
class INVRS_3DPHYSICS_API Physics : public ModuleInterface
{
public:

//******************//
// PUBLIC METHODS: *//
//******************//

	/** Constructor initializes basic variables.
	 * The constructor registers the module at the SystemCore, creates a new
	 * instance of the oops::simulation class and configures this class. It
	 * furthermore registers the PhysicsEventFactory to the EventManager and
	 * creates the default PhysicsObjectManagerFactories.
	 */
	Physics();

	/** Destructor cleans up the Module.
	 * The destructor deletes all registered PhysicsObjectManagerFactories
	 * and deletes the instance of the oops::simulation class.
	 */
	virtual ~Physics();

	/** Loads the configuration for the Module from a config file.
	 * The method parses the passed XML file and configures the Module according
	 * to the data in the config file.
	 * @param configFile URL of the configFile which should be loaded
	 * @return true if the loading of the configuration succeeded, false otherwise
	 */
	virtual bool loadConfig(std::string configFile);

	/** Cleans up the Module.
	 * The method deletes all registered PhysicsObjects and the ObjectManager
	 */
	virtual void cleanup();

	/** Returns the name of the Module.
	 * @return Name of the Module (Physics)
	 */
	virtual std::string getName();

	/** Returns a SynchronisePhysicsEvent.
	 * The event is used to synchronise the current state of the Module to new
	 * users.
	 * @return SynchronisePhysicsEvent which includes all relevant Physics data
	 */
	virtual Event* createSyncEvent();

	/** Registers a new PhysicsObjectManagerFactory to the Module.
	 * The passed factory is stored in the Module and is used to build the
	 * apropriate PhysicsObjectManager in the loadConfig-method if this Manager
	 * is configured in the XML-file. The factory will be deleted by the Physics
	 * Module when the destructor is called.
	 * @param factory PhysicsObjectManagerFactory which should be registered
	 */
	void registerPhysicsObjectManagerFactory(PhysicsObjectManagerFactory* factory);

	/** Registers a new SynchronisationModelFactory to the Module.
	 * The passed factory is stored in the Module and is used to build the
	 * appropriate SynchronisationModel in the loadConfig-method if this Model
	 * is configured in the XML-file. The factory will be deleted by the Physics
	 * Module when the destructor is called.
	 * @param factory SynchronisationModelFactory which should be registered
	 */
	void registerSynchronisationModelFactory(SynchronisationModelFactory* factory);

	/** Initializes the Physics Module.
	 * The method has to be called after the loadConfig method was initiated.
	 * Therefore it is used to initialize data that needs this or other Modules
	 * to be configured already. In the moment it only requests an EventPipe
	 * from the EventManager.
	 */
	void init();

	/** Starts the Physics Simulation in an own Thread.
	 * The method creates a new Thread and starts the static Physics::run-method
	 * which runs the physics simulation.
	 */
	void start();

	/** Starts the Physics Simulation in an own Thread after a
	 * StartPhysicsSimulationEvent is received. This is used to synchronise
	 * multiple participants.
	 */
	void startOnEvent();

	/** Kills the Physics Simulation Thread.
	 * The method kills the Physics Simulation Thread and waits until the
	 * Thread has terminated before it returns.
	 */
	void kill();

	/** Runs the Physics Simulation in the same Thread.
	 * The method determines how much time elapsed since the last call and runs
	 * the according number of simulation steps. The simulation itself is done
	 * in the same thread as the method was called. This allows to run the
	 * Physics Simulation in single threaded mode.
	 * NOTE: If you want to run the simulation in the same Thread as the rest
	 *       then don't call the start method!
	 */
	void simulate();

	/** Adds a new PhysicsObject to the Simulation.
	 * The method forwards the call to the used objectManager.
	 * @return true if the object could be added, false otherwise
	 */
	bool addPhysicsObject(PhysicsObjectInterface* object);

	/** Removes a PhysicsObject from the Simulation.
	 * The method forwards the call to the used objectManager.
	 * @return true if the object could be removed, false otherwise
	 */
	bool removePhysicsObject(PhysicsObjectInterface* object);

	/** Removes a PhysicsObject from the Simulation.
	 * The method forwards the call to the used objectManager.
	 * @return true if the object could be removed, false otherwise
	 */
	PhysicsObjectInterface* removePhysicsObject(PhysicsObjectID physicsObjectID);

	/** Registers a SystemThreadListener.
	 * The method adds the passed listener to the systemThreadListener list.
	 * These listener will then be notified every time the <code>update</code>
	 * method is called. NOTE: The method has to be called from the system
	 * thread! There is no Lock which allows this registration from another
	 * thread! It is suggested you register the listener in the constructor
	 * of the implemented class and check on the callback if the object is
	 * already added to the simulation.
	 * @param listener SystemThreadListener which should be added
	 * @return true if the listener could be registered, false if not
	 */
	bool addSystemThreadListener(SystemThreadListenerInterface* listener);

	/** Removes a registered SystemThreadListener.
	 * The method removes the passed listener from the systemThreadListener
	 * list. NOTE: The method has to be called from the system thread! It is
	 * suggested that the method is called by the destructor of the implemented
	 * class.
	 * @param listener SystemThreadListener which should be removed
	 * @return true if the listener could be removed, false otherwise
	 */
	bool removeSystemThreadListener(SystemThreadListenerInterface* listener);

	/** Registers a SimulationStepListener.
	 * The method adds the passed listener to the simulationStepListener list.
	 * These listener will then be notified every time the <code>step</code>
	 * method is called.
	 * @param listener SimulationStepListener which should be added
	 * @return true if the listener could be registered, false if not
	 */
	bool addSimulationStepListener(SimulationStepListenerInterface* listener);

	/** Removes a registered SimulationStepListener.
	 * The method removes the passed listener from the simulationStepListener
	 * list.
	 * @param listener SimulationStepListener which should be removed
	 * @return true if the listener could be removed, false otherwise
	 */
	bool removeSimulationStepListener(SimulationStepListenerInterface* listener);

	/** Update call for system thread.
	 * The method call notifies all registered systemThreadListener.
	 * @param dt elapsed time since last system thread call
	 */
	void update(float dt);

	/** Returns the simulation instance.
	 * The method returns the instance of the oops::Simulation class
	 * @return instance of the oops::Simulation class
	 */
	oops::Simulation* getSimulation();

	/** Returns the XMLLoader used for loading physics objects.
	 * The method returns the XMLLoader instance which is used internally for
	 * loading RigidBodies, ArticulatedBodies, Joints and Geometries.
	 * @return instance of the oops::XMLLoader class
	 */
	oops::XMLLoader* getXMLLoader();

	/** Returns the current simulation time.
	 * The method returns the current simulation time.
	 * @return current simulation time
	 */
	unsigned getSimulationTime();

	/** Returns the duration of one simulation step in seconds.
	 * @return size of one simulation step
	 */
	float getStepSize();

	/** Returns if the physics module acts as server for some objects
	 */
	bool isPhysicsServer();

	/** Returns if the physics module does physics calculations (either server
	 * or client side)
	 */
	bool doesPhysicsCalculations();

//******************************//
//* DEPRECATED PUBLIC METHODS: *//
//******************************//

/*
	void setTriangleMeshLoader(TriangleMeshLoader* loader);
	void setRendererFactory(RendererFactory* factory);
	void setGravity(gmtl::Vec3f gravity);
	void setObjectsVisible(bool visible);
	static void setServerMode(bool server);
	static bool isServer();
	static void syncToServer(ExtendedBinaryMessage* msg);
 	static void getAvatarCollisions(PhysicsAvatar* avatarList, std::vector<ContactData*>& contacts);
	void removePhysicsObject(PhysicsObject* object);
	void handleMessages();
*/

//*******************//
//* PUBLIC MEMBERS: *//
//*******************//

	static inVRsUtilities::Timer physicsTimer;

protected:

	friend class ChangedUserDataEvent;
	friend class PhysicsObjectManager;
	friend class SynchronisePhysicsEvent;
	friend class SynchronisationModel;
	friend class StartSimulationEvent;
	friend class ChangeSynchronisationModelEvent;

//*********************//
// PROTECTED METHODS: *//
//*********************//

	/** Parses the Element <userData> from the xml-configuration.
	 * @param xml Pointer to the XML reader
	 * @return true if element could be parsed correctly
	 */
//	bool parseUserData(IrrXMLReader* xml);

	/** Returns a new PhysicsObjectManager of the passed class-type.
	 * The method asks all registered PhysicsObjectManagerFactories to create
	 * a new PhysicsObjectManager with the passed className. If a matching
	 * factory is found the new Manager is returned.
	 * @param className name of the PhysicsObjectManager which should be created
	 * @param args arguments given to the factory
	 * @return new PhysicsObjectManager if factory is found, NULL otherwise
	 */
	PhysicsObjectManager* createPhysicsObjectManager(std::string className, ArgumentVector* args);

	/** Returns a new SynchronisationModel of the passed class-type.
	 * The method asks all registered SynchronisationModelFactories to create
	 * a new SynchronisationModel with the passed className. If a matching
	 * factory is found the new Model is returned.
	 * @param className name of the SynchronisationModel which should be created
	 * @param args arguments passed to the factory
	 * @return new SynchronisationModel if factory is found, NULL otherwise
	 */
	SynchronisationModel* createSynchronisationModel(std::string className, ArgumentVector* args);

	/** Runs the physics simulation until it is killed.
	 * The method runs in an own thread and loops until the Physics::kill method
	 * is called. It does the Physics simulation by calling the step-method
	 * after the number of seconds defined as stepSize are passed.
	 * @param args is not used, only needed to match to the Threading-interface
	 */
	static void run(void* args);

	/** Runs a simulation step.
	 * The method handles the incoming Events and Messages, notifies the
	 * SimulationStepListener, synchronises the PhysicsObjectManager and run
	 * the virtual simulation step.
	 */
	void step();

	/** Handles all incoming Physics-Events.
	 * The method reads all incoming Events from the EventPipe and executes
	 * them.
	 */
	void handleEvents();

	/** Notifies all registered SimulationStepListener that a new step occurs.
	 * The method calls the step-method of all registered
	 * SimulationStepListener.
	 * @param dt the passed timestep, which is equal to the stepSize
	 */
	void handleSimulationStepListener(float dt);

	void setStartTime(double time);

	/** Changes the used SynchronisationModel during runtime
	 * The method is called by the PhysicsChangeSynchronisationModelEvent in
	 * order to change the used SynchronisationModel during runtime
	 * @param modelName Name of the new SynchronisationModel
	 * @param args Arguments for the Model
	 */
	void setSynchronisationModel(std::string modelName, ArgumentVector* args);

//*********************//
// PROTECTED MEMBERS: *//
//*********************//

	static volatile double startTime;

	/// Module instance stored to be accessed by run-method
	static Physics* singleton;
	/// bool indicating if the physics thread is running
	static volatile bool running;
	/// bool indicating that the physics thread should be killed
	static volatile bool shutdown;
	/// bool indicating that the simulation should start when event is received
	static volatile bool waitForEvent;

	/// Used to ignore method calls after cleanup is called
	bool alreadyCleanedUp;

#if OSG_MAJOR_VERSION >= 2
	/// Lock for simulation step so that no PhysicsObject can be removed
	OSG::LockRefPtr simulationStepLock;
	/// Lock for systemThreadListener-list
	OSG::LockRefPtr systemThreadListenerLock;
	/// Lock for simulationStepListener-list
	OSG::LockRefPtr simulationStepListenerLock;
#else
	/// Lock for simulation step so that no PhysicsObject can be removed
	OSG::Lock* simulationStepLock;
	/// Lock for systemThreadListener-list
	OSG::Lock* systemThreadListenerLock;
	/// Lock for simulationStepListener-list
	OSG::Lock* simulationStepListenerLock;
#endif
	/// Simulation instance running the underlying simulation
	oops::Simulation* simulation;
	/// XMLLoader used for loading physics objects from file
	oops::XMLLoader* xmlLoader;
	/// number of seconds between two simulation steps
	float stepSize;

	/// time when the simulation is started -> needed to synchronise simulationTimes!!!
	double simulationStartTime;

	/// step counter since simulation start.
	/// will be increased every simulation step, unsigned seems to be
	/// enough since the simulation can run about 50 days with a simulation
	/// rate of 1000 steps per second, where ODE surely crashes before ;-)
	unsigned simulationTime;

	/// timestamp of last simulation call (used in Physics::simulate)
	float simLastTime;
	/// time until next simulation step (used in Physics::simulate)
	float simTimeToNextStep;
	/// time since last FPS output, done every sec (used in Physics::simulate)
	float simFpsTime;
	/// FPS counter (used in Physics::simulate)
	unsigned simFpsCounter;

	/// Load of the Physics Thread (used in Physics::run)
	float simulationLoad;

	/// factories for PhysicsObjectManager
	std::vector<PhysicsObjectManagerFactory*> objectManagerFactories;
	/// actually used objectManager
	PhysicsObjectManager* objectManager;

	/// list for all registered SystemThreadListener
	std::vector<SystemThreadListenerInterface*> systemThreadListener;
	/// list for all registered SimulationStepListener
	std::vector<SimulationStepListenerInterface*> simulationStepListener;

	/// EventPipe for incoming Events
	EventPipe* incomingEvents;

	/// factories for SynchronisationModels
	std::vector<SynchronisationModelFactory*> synchronisationModelFactories;
	/// Model which handles synchronisation between servers and clients
	SynchronisationModel* synchronisationModel;

//	/// Lock for updating the connected Users
// 	OSG::Lock* connectedUserLock;
	/// list with the IDs of all directly (network link) connected Users
//	std::vector<unsigned> connectedUsers;
	/// map with the relevant data for each user
//	std::map<unsigned, UserData*> userDataMap;

	/// Pointer to physics Thread, used to not start Thread two times
#if OSG_MAJOR_VERSION >= 2
	OSG::ThreadRefPtr physicsThread;
#else
	OSG::Thread* physicsThread;
#endif

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

}; // Physics

#endif // _PHYSICS_H
