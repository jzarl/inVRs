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

#ifndef _MODULEINTERFACE_H
#define _MODULEINTERFACE_H

#include <string>

//#include "../EventManager/Event.h"
#include "../Platform.h"

//class Entity;
class Event;

/**
 * Basic interface for modules.
 * Provides methods for initialisation and cleanup.
 */
class INVRS_SYSTEMCORE_API ModuleInterface {
public:
	virtual ~ModuleInterface() {};

	enum CONFIGURATION_ORDER {
		CONFIGURE_BEFORE_CORECOMPONENTS,
		CONFIGURE_AFTER_CORECOMPONENTS
	};

	/**
	 * Instruct the module to read its configuration from a file.
	 * @return true on success.
	 * @param configFile path of the XML-file.
	 */
	virtual bool loadConfig(std::string configFile) = 0;

	/**
	 * Instruct the module to free all memory allocated internally.
	 */
	virtual void cleanup() {};

	/**
	 * Updates the module every frame
	 */
	virtual void update(float dt) {};

	/**
	 * Return the unique name of the module.
	 */
	virtual std::string getName() = 0;

	/**
	 * Define wether the module should be configured before or after the
	 * core components are configured. By default the module is configured
	 * before the core components are configured.
	 */
	virtual CONFIGURATION_ORDER getConfigurationOrder() {
		return CONFIGURE_BEFORE_CORECOMPONENTS;
	} // getConfigurationOrder

	/**
	 * \todo write doku
	 */
	virtual Event* createSyncEvent() {
		return NULL;
	}

};

// calling convention of invrs' modules
#ifdef WIN32
#define INVRS_MODULE_CALL __stdcall
#else
#define INVRS_MODULE_CALL 
#endif

// modules function typedefs
typedef bool (INVRS_MODULE_CALL *registerModule_t)();
typedef bool (INVRS_MODULE_CALL *unregisterModule_t)();
typedef const char* (INVRS_MODULE_CALL *modulePluginVersion_t)();

/* internal macro to ensure interface-compatibility between SystemCore
 * and plugins. (Is checked on plugin-loading) */
//#define MODULEPLUGINVERSION "$Revision: 1622 $"
char const *const MODULEPLUGINVERSION = "$Revision: 1622 $";


#endif // _MODULEINTERFACE_H
