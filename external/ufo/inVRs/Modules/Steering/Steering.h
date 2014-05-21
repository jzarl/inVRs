#ifndef INVRS_STEERING_H
#define INVRS_STEERING_H

#include <inVRs/SystemCore/ComponentInterfaces/ModuleInterface.h>

#ifdef WIN32
#	ifdef UFO_BUILD_INTERNAL
#		define UFO_INVRS_STEERING_API __declspec(dllexport)
#	else /* ufo build */
#		define UFO_INVRS_STEERING_API __declspec(dllimport)
#	endif
#else
#	define UFO_INVRS_STEERING_API
#endif

// inVRs does not use namespaces yet.
// TODO: add namespace declaration once they introduce one
class UFO_INVRS_STEERING_API Steering : public ModuleInterface
{
	public:
		Steering();
		virtual ~Steering();

		/**
		 * Instruct the module to read its configuration from a file.
		 * @return true on success.
		 * @param configFile path of the XML-file.
		 */
		virtual bool loadConfig(std::string configFile);

		/**
		 * Instruct the module to free all memory allocated internally.
		 */
		virtual void cleanup();

		/**
		 * Define wether the module should be configured before or after the
		 * core components are configured. By default the module is configured
		 * before the core components are configured.
		 */
		virtual CONFIGURATION_ORDER getConfigurationOrder();

		/**
		 * Return the unique name of the module.
		 */
		virtual std::string getName();

		/**
		 * Update all ufo::Flocks and ufo::Pilots.
		 */
		virtual void update(const float dt);
};

#endif /* INVRS_STEERING_H */
