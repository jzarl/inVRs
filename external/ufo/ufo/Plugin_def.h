#ifndef UFO_PLUGIN_DEF_H
#define UFO_PLUGIN_DEF_H

#include "Plugin.h"
#include "UfoDB.h"

/**
 * @file Plugin definitions 
 *
 * When writing a plugin, you can use the 
 * macro MAKEPLUGIN(<em>uctype</em>,<em>type</em>,<em>subtype</em>,<em>factoryFunctio</em>n), with:
 * 	- <em>uctype</em> in {FLOCK, PILOT, BEHAVIOUR, STEERABLE}
 *  - <em>type</em> in {Flock, Pilot, Behaviour, Steerable}
 *  - <em>subtype</em>: the classname of the subclass to <em>type</em>.
 *  - <em>factoryFunction</em>: a function pointer
 *
 * More generally speaking, a Plugin has to define the following functions:
 *  - int <em>typename</em>pluginBranchname(): the branch name of this libufo (different branches are deemed not to be compatible)
 *  - int <em>typename</em>pluginVersion()
 *  - std::string <em>typename</em>pluginTypename()
 *  - void <em>typename</em>pluginRegister()
 * 
 * Note: the prepending of plugin-functions with the type name allows you to
 * link directly to plugins, bypassing the plugin system. (See test/02testplugins.cpp for details)
 * 
 * @see test/02plugins.cpp
 */

// Macro definitions for Plugins:

#ifdef WIN32
// hopefully, at least this error works on windows:
#error Plugins are not supported on WIN32! \
	Please add plugin code for windows, or use the \
	UfoDB::registerTYPEFactory(SUBTYPENAME,FACTORYFUN) interface.
#else

#define MAKEPLUGIN(UCTYPE, TYPE, SUBTYPENAME, FACTORYFUN) \
	extern "C" { \
		std::string SUBTYPENAME ## pluginBranchname() \
		{ \
			return UFO_BRANCHNAME; \
		} \
		std::string SUBTYPENAME ## pluginVersion() \
		{ \
			return UFO_ ## UCTYPE ## _ID; \
		} \
		std::string SUBTYPENAME ## pluginTypename() \
		{ \
			return #TYPE; \
		} \
		void SUBTYPENAME ## pluginRegister( ) \
		{ \
			UfoDB::the()->register ## TYPE ## Type(#SUBTYPENAME, FACTORYFUN); \
		} \
	}
#endif

#endif /* UFO_PLUGIN_DEF_H */
