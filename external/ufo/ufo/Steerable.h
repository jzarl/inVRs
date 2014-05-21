#ifndef UFO_STEERABLE_H
#define UFO_STEERABLE_H

#include <string>
#include <vector>
#include <map>

#include <gmtl/Vec.h>
#include <gmtl/Point.h>
#include <gmtl/Quat.h>

namespace ufo
{
	class SteeringDecision;

	/**
	 * Steerable defines an interface between the Pilot and the object it steers.
	 * I.e., it is the interface between UFO and your framework.
	 * It provides a method for advertising the actual steering decision, as well as
	 * means for getting the actual position and speed of the steered object.
	 */
	class Steerable
	{
		public:
			/**
			 * The type to be used for factory functions for Steerable objects.
			 * This factory has to find the appropriate steered object from your
			 * framework, and build the Steerable adaptor accordingly.
			 */
			typedef Steerable* (*factoryType)(std::vector<std::pair<std::string,std::string> >*); 

			/**
			 * Delete the object.
			 * In contrast to using delete, this also works beyond DSO boundaries.
			 * I.e., you need this for using plugins.
			 */
			virtual void destroy();

			/**
			 * Steer the object towards a certain direction and orientation.
			 * Depending on the steered object and its implementation,
			 * the new speed/orientation might equal the given direction/orientation, or not.
			 *
			 * The elapsedTime parameter should not influence the direction parameter.
			 * Use the elapsedTime only, if you need to compute time-dependent values 
			 * (e.g. velocity, when the steered object doesn't provide the velocity).
			 */
			virtual void steer( const SteeringDecision &d, const float elapsedTime) = 0;
			/**
			 * Returns the actual position of the steered object.
			 */
			virtual gmtl::Point3f getPosition() = 0;
			/**
			 * Returns the actual orientation of the steered object.
			 * If the steered object does not have a notion of orientation,
			 * it may return any orientation value, and must ignore any orientation value given 
			 * to the steer() method.
			 */
			virtual gmtl::Quatf getOrientation() = 0;
			/**
			 * Returns the actual speed of the steered object.
			 */
			virtual gmtl::Vec3f getVelocity() = 0;

			/**
			 * Print information about the Steerable.
			 */
			virtual void print() const = 0;

		protected:
			/**
			 * The destructor.
			 */
			virtual ~Steerable();
	};
}

#endif /* UFO_STEERABLE_H*/
