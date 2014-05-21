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

#ifndef _SYSTEMCORE_H
#define _SYSTEMCORE_H

#include <list>

//#include "ComponentInterfaces/ModuleInterface.h"
#include "../OutputInterface/SceneGraphInterface.h"
//#include "EventManager/EventManager.h"

#include "XmlConfigurationLoader.h"

enum CoreComponents {
	WORLDDATABASE, USERDATABASE, TRANSFORMATIONMANAGER, EVENTMANAGER
};

typedef void (*moduleInitCallback)(ModuleInterface*);
typedef void (*coreComponentInitCallback)(CoreComponents);
//typedef void (*interfacesInitCallback)(ModuleInterface*);

#ifndef WIN32
typedef void* plugin_t;
#else
#include <windows.h>
#include <vector>
typedef HINSTANCE plugin_t;
#endif

//forward declaration to avoid OpenSG dependency in header
template<class T>
class SyncPipe;
typedef SyncPipe<Event*> EventPipe;

/******************************************************************************
 * @class SystemCore
 * This class holds information about modules and their configuration.
 */
class INVRS_SYSTEMCORE_API SystemCore {
public:

	/**
	 * Initializes all classes from the inVRs system core. This method initializes
	 * the WorldDatabase, UserDatabase, EventManager, RequestListener and
	 * TransformationManager in the written order. Only call this method if you want
	 * to use all features of the inVRs system core. Otherwise you can call the init-
	 * methods of those classes by yourself.
	 */
	static void init();

	/**
	 * Parse the SystemCoreConfiguration file.
	 *
	 * @param systemCoreConfigFile the path to the systemCore configuration file.
	 *
	 * This method may be called only once.
	 */
	static bool configure(std::string systemCoreConfigFile);

	/**
	 * Parse the SystemCoreConfiguration file.
	 *
	 * @param systemCoreConfigFile the path to the systemCore configuration file.
	 * @param outputInterfaceConfigFile the path to the output interface
	 *        configuration file.
	 *
	 * This method may be called only once.
	 */
	static bool configure(std::string systemCoreConfigFile, std::string outputInterfaceConfigFile);

	/**
	 * Parse the user configuration file which assigns a configuration file to each module.
	 * For each registered module, <code>loadConfig()</code> will be invoked.
	 *
	 * @param systemCoreConfigFile the path to the systemCore configuration file.
	 * @param outputInterfaceConfigFile the path to the output interface
	 *        configuration file.
	 * @param inputInterfaceConfigFile the path to the input interface
	 *        configuration file.
	 * @param modulesConfigFile the path to the modules configuration file.
	 *
	 * This method may be called only once.
	 *
	 * Note: The user configuration file also holds information unrelated to the modules
	 * (e.g. User and Avatar information).
	 * @see User::loadConfig(std::string)
	 */
	static bool configure(std::string systemCoreConfigFile, std::string outputInterfaceConfigFile,
			std::string inputInterfaceConfigFile, std::string modulesConfigFile);


	/**
	 * Unregister all currently loaded modules, and close the associated plugins.
	 * If you use any module, i.e. if you called SystemCore::configure() before,
	 * you should call this method during cleanup.
	 * Call this method before WorldDatabase::cleanup() and UserDatabase::cleanup().
	 * @see WorldDatabase::cleanup(), UserDatabase::cleanup()
	 */
	static void cleanup();

	/**
	 * Registers a ModuleInterface. All registered modules will be configured through <code>loadConfigs()</code>
	 * Typically a module calls this method in their constructor.
	 * @param modIntf module which will be registered
	 */
	static bool registerModuleInterface(ModuleInterface* modIntf);

	/**
	 * Unregisters a ModuleInterface. Typically a module calls this method in their destructor.
	 * @param modIntf module which will be registered
	 */
	static bool unregisterModuleInterface(ModuleInterface* modIntf);

	/**
	 * Register a function to be called on initialisation of a module.
	 * The callback function will be called after a new module has been loaded (i.e. it has
	 * been registered with the SystemCore), but before the module's loadConfig method
	 * has been called.
	 * The callback function has to take one argument of type <code>ModuleInterface *</code>,
	 * and have a return type of <code>void</code>.
	 * All registered callbacks are called on each new module.
	 *
	 * @param function the callback function.
	 */
	static void registerModuleInitCallback(moduleInitCallback function);

	/**
	 * Register a function to be called on initialisation of a CoreComponent.
	 * The callback function will be called after a new components has been loaded
	 * (i.e. it has been registered with the SystemCore), but before the
	 * components loadConfig method has been called.
	 * The callback function has to take one argument of type <code>CoreComponents</code>,
	 * and have a return type of <code>void</code>.
	 * All registered callbacks are called on each new component.
	 *
	 * @param function the callback function.
	 */
	static void registerCoreComponentInitCallback(coreComponentInitCallback function);

	/**
	 * @TODO: write doxygen documentation
	 * Note: Depends on the Network module being loaded.
	 */
	static void synchronize();

	/**
	 * @TODO: write doxygen documentation
	 * Note: Must not be called before synchronize().
	 */
	static void step();

	/**
	 * Sets the SceneGraphInterface. This method can be called only once. Also invokes <code>init()</code> of the SceneGraphInterface.
	 * At the moment there is no way to remove the SceneGraphInterface (as there is no need for that). Simply call <code>delete</code> on it when everything else has been released.
	 * @param sgInt SceneGraphInterface singelton
	 */
//	static void setSceneGraphInterface(SceneGraphInterface* sgInt);

	/**
	 * Returns a reference to a list of all registered modules. The content of the list might change (whenever another module
	 * is added or removed (which is not supported at the moment)).
	 * @return reference to list of all registered modules.
	 */
	static const std::map<std::string, ModuleInterface*>& getModuleMap();

	/**
	 * Returns the module with the specified name. If the module is not loaded yet the method tries
	 * to load a suitable plugin.
	 * @param name name of the module
	 * @param fileName optional path of the plugin file
	 * @return pointer to the module with the name <code>name</code> or <code>NULL</code> in case of failure
	 *
	 * Note: The file name is completed using the function getFullPath() (in file UtilityFunctions.h)
	 */
	static ModuleInterface* getModuleByName(std::string name, std::string fileName = "");

	/**
	 * Method for getting access to the SceneGraphInterface singelton.
	 * @return returns the SceneGraphInterface singelton
	 */
//	static SceneGraphInterface* getSceneGraphInterface();

	/**
	 * Query, wether a module has already been loaded.
	 * @param name The name of the module.
	 * @return True, if the module is loaded; false otherwise.
	 */
	static bool isModuleLoaded(std::string name);

#ifdef WIN32
	static void addPreloadModule(HMODULE dllModuleHandle);
	static void initPreloadModules();
#endif

private:

	struct ModulesConfigData {
		std::string moduleName;
		std::string moduleConfigPath;
		std::string moduleLibPath;
	}; // ModulesConfigData

	/**
	 *
	 */
	static bool initModules(std::string modulesConfigFile,
			std::vector<ModulesConfigData>& modulesConfigurations);

	/**
	 *
	 */
	static bool configureCoreComponents(std::string configFile);

	/** The method returns a pointer to the vector of all registered
	 * Modules. WARNING: The method returns a pointer to the list so
	 * every change also changes the data of the SystemCore! Also take care
	 * when working multithreaded!
	 * @return pointer to the vector of registered Modules
	 */
	static bool loadPlugin(std::string filename);

	/**
	 * @TODO: write doxygen documentation
	 */
	static std::string getLibraryFileName(std::string libraryName);

	static SceneGraphInterface* sgInt;
	static std::list<plugin_t> pluginlist;
	static std::map<std::string, ModuleInterface*> moduleMap;
	static std::vector<moduleInitCallback> moduleCallbacks;
	static std::vector<coreComponentInitCallback> coreComponentCallbacks;
	static EventPipe* eventPipe;
	static bool initialized;

	/**
	 * @TODO: write doxygen documentation
	 */
	static bool parseModuleConfig(std::string path,
			std::vector<ModulesConfigData>& modulesConfigurations);

	/**
	 * @TODO: write doxygen documentation
	 */
	static bool initializeModules(std::vector<ModulesConfigData>& modulesConfigurations);

	/**
	 * @TODO: write doxygen documentation
	 */
	static bool configureModules(std::vector<ModulesConfigData>& modulesConfigurations,
			ModuleInterface::CONFIGURATION_ORDER configurationOrder);

	/**
	 * @TODO: write doxygen documentation
	 */
	static void sendCoreComponentCallback(CoreComponents component);

	/**
	 * Frees the memory for the loaded plugins
	 */
	static void cleanupPlugins();


#ifdef WIN32
	static std::vector<HMODULE> dllsPreloadList;
#endif

public:

	class ModuleLoadAccessor {
	private:
		friend class InputInterface;
		friend class OutputInterface;

		static bool loadModule(std::string name, std::string fileName);
	}; // ModuleLoadAccessor

//*****************************************************************************
// Configuration loading
//*****************************************************************************
private:
	static XmlConfigurationLoader systemCoreXmlConfigLoader;
	static XmlConfigurationLoader moduleXmlConfigLoader;

	class SystemCoreConverterToV1_0a4 : public XmlConfigurationConverter {
	public:
		SystemCoreConverterToV1_0a4();
		virtual bool convert(XmlDocument* document, const Version& version, std::string configFile);
	}; // SystemCoreConverterToV1_0a4

	class ModuleConverterToV1_0a4 : public XmlConfigurationConverter {
	public:
		ModuleConverterToV1_0a4();
		virtual bool convert(XmlDocument* document, const Version& version, std::string configFile);
	}; // ModuleConverterToV1_0a4

}; // SystemCore

#endif
