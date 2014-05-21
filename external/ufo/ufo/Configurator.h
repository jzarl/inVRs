#ifndef UFO_CONFIGURATOR_H
#define UFO_CONFIGURATOR_H

#include <string>
#include <vector>
#include <map>

#include "Flock.h"
#include "Pilot.h"
#include "Behaviour.h"
#include "Steerable.h"
#include "ConfigurationElement.h"

namespace ufo
{
	class Plugin;
	
	class Configurator
	{
		public:
			/**
			 * The default constructor.
			 */
			Configurator();

			/**
			 * Bind the configuration to actual Flock/Pilot/Behaviour/Steerable objects.
			 * After the configuration is complete, and will not be altered anymore, this
			 * method is called to instantiate the configuration.
			 *
			 * The finished Flocks will be placed in the <code>flocks</code> vector,
			 * Pilots without associated Flocks will be placed in the <code>pilots</code>
			 * vector.
			 * Failure to create any Object will delete any object created thus far.
			 * If everything went fine, all Flocks and Pilots will be registered with the UfoDB.
			 *
			 * @return <code>true</code> on success, <code>false</code> on errors.
			 */
			const bool bind();

#if 0
			/**
			 * Returns the Flocks as a vector.
			 */
			const std::vector <Flock *> getFlocks() const;
			/**
			 * Returns the Pilots as a vector.
			 */
			const std::vector <Pilot *> getPilots() const;
#endif
			/**
			 * Print out the configuration in a human-readable form.
			 */
			void print() const;

			/**
			 * Destructor.
			 * Deleting the Configurator will delete any ConfigurationElements and templates.
			 * Any Pilots and Flocks created will <em>not</em> be deleted! (I.e. you can savely
			 * delete the Configurator once you have got your Pilots and Flocks.
			 */
			virtual ~Configurator();

		protected:
		private:
			// Friend classes are _not_ inherited!
			friend class ConfigurationReader;

			/**
			 * Find the correct typename for a ConfigurationElement.
			 */
			std::string lookupTemplateName (ConfigurationElement *elem) const;
			/**
			 * Find and assemble Parameters for a ConfigurationElement.
			 * The ConfigurationElement itself, as well as any templates may
			 * add parameters.
			 * @return A new std::vector<std::pair<std::string,std::string> > containing all parameters combined.
			 * \note This method creates a std::vector<std::pair<std::string,std::string> > with <code>new</code>,
			 *       so you are responsible for deleting the return value!
			 */
			std::vector<std::pair<std::string,std::string> >* lookupTemplateParameters (ConfigurationElement *elem) const;
			/**
			 * Find and assemble Children for a ConfigurationElement.
			 * The ConfigurationElement itself, as well as any templates may
			 * add children.
			 * @return A new std::vector <ConfigurationElement*> containing all children combined.
			 * \note This method creates a std::vector <ConfigurationElement*> with <code>new</code>,
			 *       so you are responsible for deleting the return value!
			 */
			std::vector <ConfigurationElement*> * lookupTemplateChildren (ConfigurationElement *elem) const;

			Flock * bindFlock (ConfigurationElement *elem);
			Pilot * bindPilot (ConfigurationElement *elem, Flock *flock=0);
			Behaviour * bindBehaviour (ConfigurationElement *elem);
			Steerable * bindSteerable (ConfigurationElement *elem);
			void internalCleanup();

			/**
			 * Plugin Directories as read by the ConfigurationReader.
			 */
			std::vector<std::string> plugindirs;
			/**
			 * A temporary, used to reset the plugin directories if bind fails.
			 */
			std::vector<std::string> plugindirs_save;
			/**
			 * The ConfigurationElements as read by the ConfigurationReader.
			 */
			std::vector <ConfigurationElement*> elements;
			/**
			 * Template Elements can be used to write shorter Configuration files.
			 */
			std::map <std::string,ConfigurationElement *> templates;


			/**
			 * All Flocks defined in the configuration.
			 */
			std::vector <Flock *> flocks;
			/**
			 * All Pilots defined in the configuration.
			 */
			std::vector <Pilot *> pilots;
	};
}

#endif /* UFO_CONFIGURATOR_H */
