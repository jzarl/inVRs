#ifndef UFO_BEHAVIOUR_H
#define UFO_BEHAVIOUR_H

#include <string>
#include <vector>

namespace ufo
{
	class Pilot;
	class SteeringDecision;

	class Behaviour
	{
		public:
			/**
			 * The type to be used for factory functions for Behaviours.
			 * The children parameter may be ignored.
			 */
			typedef Behaviour* (*factoryType)(std::vector<Behaviour *>, std::vector<std::pair<std::string,std::string> >*); 

			/**
			 * Delete the object.
			 * In contrast to using delete, this also works beyond DSO boundaries.
			 * I.e., you need this for using plugins.
			 */
			virtual void destroy();

			/**
			 * Calculates a steering decision and returns it.
			 */
			virtual SteeringDecision yield( const float elapsedTime) = 0;

			/**
			 * Print the name and information about the Behaviour.
			 */
			virtual void print() const = 0;

			/**
			 * This function allows the Pilot to register itself with the Behaviour.
			 * This enables the Behaviour to access its Pilots methods.
			 * Obviously, the Pilot has to register itself before calling yield(float)!
			 * 
			 * CAVEAT: if your Behaviour has sub-Behaviours, you have to propagate a call 
			 * to registerPilot to all sub-Behaviours!
			 */
			virtual void registerPilot( Pilot *thePilot);
		protected:
			/**
			 * The destructor.
			 */
			virtual ~Behaviour();

			Pilot *myPilot;
		private:
	};
}

#endif /* UFO_BEHAVIOUR_H*/
