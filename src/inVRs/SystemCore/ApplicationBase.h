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

#ifndef APPLICATIONBASE_H_
#define APPLICATIONBASE_H_

#include "SystemCore.h"
#include "CommandLineArguments.h"
#include "ProfilingHelper.h"

#include "UserDatabase/User.h"

#include "../InputInterface/ControllerManagerInterface.h"
#include "ComponentInterfaces/NetworkInterface.h"
#include "ComponentInterfaces/NavigationInterface.h"
#include "ComponentInterfaces/InteractionInterface.h"

/**
 *
 *
 */
class INVRS_SYSTEMCORE_API ApplicationBase {

/******************************************************************************
 * Methods to overload / implement in derived classes:
 *****************************************************************************/
protected:
	/**
	 *
	 */
	virtual bool preInit(const CommandLineArgumentWrapper& args);

	/**
	 *
	 */
//	virtual SceneGraphInterface* createSceneGraphInterface(const CommandLineArgumentWrapper& args) = 0;

	/**
	 *
	 */
	virtual std::string getConfigFile(const CommandLineArgumentWrapper& args) = 0;

	/**
	 *
	 */
	virtual bool init(const CommandLineArgumentWrapper& args) = 0;

	/**
	 *
	 */
	virtual void run() = 0;

	/**
	 *
	 */
	virtual void initCoreComponentCallback(CoreComponents comp);

	/**
	 *
	 */
	virtual void initInputInterfaceCallback(ModuleInterface* moduleInterface);

	/**
	 *
	 */
	virtual void initOutputInterfaceCallback(ModuleInterface* moduleInterface);

	/**
	 *
	 */
	virtual void initModuleCallback(ModuleInterface* module);

	/**
	 *
	 */
	virtual bool disableAutomaticModuleUpdate();

	/**
	 *
	 */
	virtual void manualModuleUpdate(float dt);

	/**
	 *
	 */
	virtual void display(float dt) = 0;

	/**
	 *
	 */
	virtual void cleanup() = 0;

	/// Pointer to the SceneGraphInterface
	SceneGraphInterface* sceneGraphInterface;
	/// Pointer to the ControllerManager
	ControllerManagerInterface* controllerManager;
	/// Pointer to the Network Module
	NetworkInterface* networkModule;
	/// Pointer to the Navigation Module
	NavigationInterface* navigationModule;
	/// Pointer to the Interaction Module
	InteractionInterface* interactionModule;

	/// Pointer to the local user
	User* localUser;
	/// Pointer to the user's camera
	CameraTransformation* activeCamera;

/******************************************************************************
 * Methods to call from derived classes
 *****************************************************************************/
public:

	/**
	 *
	 */
	ApplicationBase();

	/**
	 *
	 */
	virtual ~ApplicationBase();

	/**
	 *
	 */
	bool start(int argc, char** argv);

	/**
	 *
	 */
	void globalCleanup();

	/**
	 *
	 */
	void globalUpdate();

/******************************************************************************
 * Internal methods
 *****************************************************************************/
private:
	friend class SystemCore;

	/**
	 *
	 */
	bool _init(const CommandLineArgumentWrapper& args);

	/**
	 *
	 */
	bool _loadConfiguration(std::string configFile);

	/**
	 *
	 */
	bool _configureSystemCore();

	/**
	 *
	 */
	static void _initCoreComponent(CoreComponents comp);

	/**
	 *
	 */
	static void _initInputInterface(ModuleInterface* moduleInterface);

	/**
	 *
	 */
	static void _initOutputInterface(ModuleInterface* moduleInterface);

	/**
	 *
	 */
	static void _initModule(ModuleInterface* module);

	/**
	 *
	 */
	void _updateModules(float dt);

	/**
	 *
	 */
	void _updateController(float dt);

    /**
	 *
	 */
	static void _handleUnexpectedTermination(int errorCode);

	/// Pointer to log file
	FILE* _logFile;
	/// Variable storing if ApplicationBase was cleaned up already
	bool _isCleanedUp;
	/// Time when last step was executed
	float _lastTime;
	/// Time between two frames (used when maximum framerate is set)
	float _frameDuration;
	/// Time until next frame (used when maximum framerate is set)
	float _timeToNextFrame;

	/// Pointer to application instance
	static ApplicationBase* _instance;
	/// List of all active tracking pipes
	std::vector<TransformationPipe*> _trackingPipes;

	/// Profiling class to find out slow parts of the inVRs application
	inVRsUtilities::ProfilingHelper _appProfiler;
}; // ApplicationBase

//ApplicationBase* createApplication(int argc, char** argv);

#endif /* APPLICATIONBASE_H_ */
