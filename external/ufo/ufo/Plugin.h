#ifndef UFO_PLUGIN_H
#define UFO_PLUGIN_H

#include <string>
#include <stdexcept>
#include <ufo/Ufo.h>

/**
 * @file Plugin interface versions.
 *
 * @see Plugin_def.h
 */

// check if build-system is ok:
#ifndef UFO_BRANCHNAME
#error Build system has to define UFO_BRANCHNAME
#endif
#ifndef UFO_FLOCK_ID
#error Build system has to define UFO_FLOCK_ID
#endif
#ifndef UFO_PILOT_ID
#error Build system has to define UFO_PILOT_ID
#endif
#ifndef UFO_BEHAVIOUR_ID
#error Build system has to define UFO_BEHAVIOUR_ID
#endif
#ifndef UFO_STEERABLE_ID
#error Build system has to define UFO_STEERABLE_ID
#endif

namespace ufo
{

	/**
	 * Function signature for the pluginBranchname method of plugins.
	 * The branchname of a plugin must be an exact match with UFO_BRANCHNAME
	 * in the library.
	 */
	typedef std::string (*pluginBranchname_t)();
	/**
	 * Function signature for the pluginVersion method of plugins.
	 * The pluginVersion method must return the UFO_#TYPE#_ID 
	 * as defined at compile-time of the plugin, <em>NOT</em>
	 * the ufo::VERSION_#TYPE# of the library.
	 */
	typedef std::string (*pluginVersion_t)();
	/**
	 * Function signature for the pluginTypename method of plugins.
	 * The Typename must be the name of the class produced by the 
	 * plugin's factory method.
	 */
	typedef std::string (*pluginTypename_t)();

	typedef void (*pluginRegister_t)();

	// The signatures of the factory functions of plugins are defined in 
	// the files Flock.h, Pilot.h, Behaviour.h, and Steerable.h respectively.

	/**
	 * The Plugin class encapsulates the loading of DSOs to
	 * allow dynamic addition of new functionality at runtime.
	 * Without this, you would have to anticipate any types that could be
	 * encountered in your configuration.
	 */
	class Plugin
	{
		public:
			// XXX: maybe merge this with the "type" enum in ConfigurationElement?
			enum pluginType { FLOCK, PILOT, BEHAVIOUR, STEERABLE };
			/**
			 * Create a plugin object from a shared library file.
			 * If the plugin couldn't be loaded, or if any of the required
			 * functions is not found, a std::runtime_error is thrown.
			 * @param filename a DSO file
			 * @param id a prefix to the function-names
			 * @param the type of the plugin, so the proper version can be compared
			 */
			Plugin(std::string filename, std::string id, pluginType t);

			/**
			 * Return the filename used to open the plugin.
			 */
			std::string pluginFilename() const;
			/**
			 * Returns the branch name of the plugin.
			 * Obviously, this is always the same as UFO_BRANCHNAME.
			 */
			std::string pluginBranchname() const;
			/**
			 * Returns the plugin version.
			 */
			std::string pluginVersion() const;

			/**
			 * Returns the type of the plugin.
			 * @return "Flock"|"Pilot"|"Behaviour"|"Steerable"
			 */
			std::string pluginTypename() const;

			/**
			 * Close the dynamic library file.
			 */
			~Plugin();
		protected:
		private:
			const std::string filename;
			std::string subtype;
			void *pluginhandle;
			pluginBranchname_t b_fun;
			pluginVersion_t v_fun;
			pluginTypename_t t_fun;
			pluginRegister_t r_fun;
	};
}
#endif /* UFO_PLUGIN_H */
