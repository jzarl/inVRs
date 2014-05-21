#ifndef UFO_UFODB_H
#define UFO_UFODB_H

#include <string>
#include <vector>
#include <map>

#include "Flock.h"
#include "Pilot.h"
#include "Behaviour.h"
#include "Steerable.h"

namespace ufo
{
	class Plugin;

	class UfoDB
	{
		public:
			/**
			 * Get the UfoDB singleton.
			 * The preferred way of access is to always use this method, not caching 
			 * its returned pointer.
			 */
			static UfoDB *the();

			/**
			 * Delete the UfoDB and free internal memory.
			 * 
			 * Implementation note: For Flocks and Pilots the destroy() method is called with
			 * deregisterFromDB set to false, i.e. the Flocks and Pilots must not try to 
			 * communicate with the UfoDB.
			 */
			static void reset();

			/**
			 * Add a path to search for plugins.
			 */
			void addPluginDirectory(std::string path);
			/**
			 * Set the paths to search for plugins.
			 * The original value will be stored in the vector previously storing the new value.
			 * You can reset the plugin directories using this with an empty vector<string>.
			 */
			void swapPluginDirectories(std::vector<std::string> &dirs );

			/**
			 * Register a custom Flock type without using the plugin-mechanism.
			 */
			void registerFlockType( std::string name, Flock::factoryType factory);
			/**
			 * Register a custom Pilot type without using the plugin-mechanism.
			 */
			void registerPilotType( std::string name, Pilot::factoryType factory);
			/**
			 * Register a custom Behaviour type without using the plugin-mechanism.
			 */
			void registerBehaviourType( std::string name, Behaviour::factoryType factory);
			/**
			 * Register a custom Steerable type without using the plugin-mechanism.
			 */
			void registerSteerableType( std::string name, Steerable::factoryType factory);
			/**
			 * Get the factory for a given FlockType.
			 * If neccessary, load a Plugin.
			 * @return the factory, or 0.
			 */
			Flock::factoryType getFlockFactory( std::string name);
			/**
			 * Get the factory for a given PilotType.
			 * If neccessary, load a Plugin.
			 * @return the factory, or 0.
			 */
			Pilot::factoryType getPilotFactory( std::string name);
			/**
			 * Get the factory for a given BehaviourType.
			 * If neccessary, load a Plugin.
			 * @return the factory, or 0.
			 */
			Behaviour::factoryType getBehaviourFactory( std::string name);
			/**
			 * Get the factory for a given SteerableType.
			 * If neccessary, load a Plugin.
			 * @return the factory, or 0.
			 */
			Steerable::factoryType getSteerableFactory( std::string name);

			/**
			 * Update all Flocks and Pilots.
			 * @param dt seconds since the last update
			 */
			void update(const float dt) const;

			/**
			 * Returns the Flocks as a vector.
			 */
			const std::vector <Flock *> &getFlocks() const;

			/**
			 * Return the Flock with the given serial number.
			 * If the serial number is not found, 0 is returned.
			 *
			 * Note: The serial number is unique for all Flocks.
			 */
			Flock * getFlockBySN(const unsigned int sn) const;

			/**
			 * Returns the Pilots not belonging to any Flock as a vector.
			 */
			const std::vector <Pilot *> &getPilots() const;

			/**
			 * Returns the Pilot with the given serial number.
			 * If the serial number is not found, 0 is returned.
			 *
			 * The Pilot returned may or may not be part of a Flock.
			 *
			 * Note: The serial number is unique for all Pilots.
			 */
			Pilot * getPilotBySN(const unsigned int sn) const;

			/**
			 * Returns any Pilots having an attribute with the given value.
			 *
			 * The Pilots returned may or may not be part of a Flock.
			 */
			std::vector <Pilot *> getPilotsByAttribute(const std::string &key, const std::string &value) const;

			/**
			 * Print information about the UfoDB and its Flocks and Pilots.
			 */
			void print() const;
		protected:
		private:
			static ufo::UfoDB *theSingleton;
			/**
			 * The default constructor.
			 */
			UfoDB();
			/**
			 * Destructor.
			 */
			virtual ~UfoDB();

			friend class Flock;
			friend class Pilot;
			unsigned int nextFlockSN;
			unsigned int nextPilotSN;
			const unsigned int addFlock(Flock *);
			const unsigned int addPilot(Pilot *,Flock *);
			void removeFlock(Flock *);
			void removePilot(Pilot *);

			/**
			 * Search the pluginDirectories for a suitable plugin file name
			 * for the given type name.
			 * @param name the filename without ending.
			 * @return the first suitable filename, or the empty string.
			 */
			const std::string findPluginFile(const std::string name) const;

			/**
			 * The directories to search for plugins.
			 */
			std::vector<std::string> pluginDirectories;
			/**
			 * A vector of loaded plugins.
			 */
			std::vector<Plugin *> plugins;

			typedef std::map <std::string, Flock::factoryType> flockFMap;
			typedef std::map <std::string, Pilot::factoryType> pilotFMap;
			typedef std::map <std::string, Behaviour::factoryType> behaviourFMap;
			typedef std::map <std::string, Steerable::factoryType> steerableFMap;

			flockFMap flockFactories;
			pilotFMap pilotFactories;
			behaviourFMap behaviourFactories;
			steerableFMap steerableFactories;

			/// global registry of all Flocks:
			std::vector<Flock *> flocks;
			/// global registry of all Pilots without Flocks:
			std::vector<Pilot *> pilots;
	};
}

#endif /* UFO_UFODB_H */
