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

#ifndef OUTPUTINTERFACE_H_
#define OUTPUTINTERFACE_H_

#include <string>
#include <map>
#include <memory>

#include "SceneGraphInterface.h"
#include "OutputInterfaceSharedLibraryExports.h"
#include "../SystemCore/ComponentInterfaces/ModuleInterface.h"
#include "../SystemCore/XmlConfigurationLoader.h"

typedef void (*outputInterfaceCallback)(ModuleInterface*);

/******************************************************************************
 *
 */
class INVRS_OUTPUTINTERFACE_API OutputInterface {
public:
	/**
	 * @TODO: write doxygen documentation
	 */
	static bool init(std::string outputInterfaceConfigFile);

	/**
	 * @TODO: write doxygen documentation
	 */
	static bool configure(std::string outputInterfaceConfigFile);

	/**
	 * @TODO: write doxygen documentation
	 */
	static void cleanup();

	/**
	 * Registers a ModuleInterface. All registered modules will be configured
	 * through <code>loadConfigs()</code>. Typically a module calls this method
	 * in their constructor.
	 * @param modIntf module which will be registered
	 */
	static bool registerModuleInterface(ModuleInterface* modIntf);

	/**
	 * Unregisters a ModuleInterface. Typically a module calls this method in
	 * their destructor.
	 * @param modIntf module which will be registered
	 */
	static bool unregisterModuleInterface(ModuleInterface* modIntf);

	/**
	 * Register a function to be called on initialisation of a module.
	 * The callback function will be called after a new module has been loaded
	 * (i.e. it has been registered with the OutputInterface), but before the
	 * module's loadConfig method has been called.
	 * The callback function has to take one argument of type
	 * <code>ModuleInterface *</code>, and have a return type of
	 * <code>void</code>.
	 * All registered callbacks are called on each new module.
	 * @param function the callback function.
	 */
	static void registerModuleInitCallback(outputInterfaceCallback function);

	/**
	 * @TODO: write doxygen documentation
	 */
	static ModuleInterface* getModuleByName(std::string name, std::string fileName = "");

	/**
	 * @TODO: write doxygen documentation
	 */
	static bool isModuleLoaded(std::string name);

	/**
	 *
	 */
	static SceneGraphInterface* getSceneGraphInterface();

private:
	struct ModulesConfigData {
		std::string moduleName;
		std::string moduleConfigPath;
		std::string moduleLibName;
	}; // ModulesConfigData

	/**
	 * @TODO: write doxygen documentation
	 */
	static bool parseOutputInterfaceConfig(std::string path,
			std::vector<ModulesConfigData>& modulesConfigurations);

	/**
	 * @TODO: write doxygen documentation
	 */
	static bool initializeModules(std::vector<ModulesConfigData>& modulesConfigurations);

	/**
	 * @TODO: write doxygen documentation
	 */
	static bool configureModules(std::vector<ModulesConfigData>& modulesConfigurations);


	static bool isInitialized;
	static bool isConfigured;
	static std::vector<ModulesConfigData> modulesConfigurations;
	static std::map<std::string, ModuleInterface*> moduleMap;
	static std::vector<outputInterfaceCallback> moduleInitCallbacks;
	static SceneGraphInterface* sceneGraphInterface;

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

}; // OutputInterface

#endif /* OUTPUTINTERFACE_H_ */
