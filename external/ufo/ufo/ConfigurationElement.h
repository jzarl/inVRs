#ifndef UFO_CONFIGURATIONELEMENT_H
#define UFO_CONFIGURATIONELEMENT_H

#include <string>
#include <map>
#include <vector>

namespace ufo 
{
	/**
	 * Internal class for storing the configuration.
	 */
	class ConfigurationElement 
	{
		public:
			enum type {FLOCK,PILOT,BEHAVIOUR,STEERABLE};
			ConfigurationElement (type t, bool fromTemplate = false);
			~ConfigurationElement();
			/**
			 * Print the configuration element in a human-readable form.
			 */
			void print() const;

			type elementType;
			/**
			 * Whether to use template lookup instead of parameters.
			 */
			bool fromTemplate; 
			/**
			 * Normally the class-name.
			 * When templating is used, this equals the template-name.
			 */
			std::string name;
			/**
			 * The parameters as key-value pairs.
			 * Yes, this is a vector of pairs, and not a map! (Ordering may be important)
			 */
			std::vector<std::pair<std::string,std::string> > parameters;
		   	std::vector <ConfigurationElement*> children;
	};
}

#endif /* UFO_CONFIGURATIONELEMENT_H */
