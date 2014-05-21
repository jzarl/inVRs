#ifndef UFO_PILOT_H
#define UFO_PILOT_H

#include <string>
#include <vector>
#include <map>

#include <gmtl/Vec.h>
#include <gmtl/Point.h>
#include <gmtl/Quat.h>

namespace ufo 
{
	class Behaviour;
	class Flock;
	class Steerable;

	/**
	 * A Pilot steers an object, but doesn't (need to) know about its physical properties.
	 * The Pilot registers itself with its Behaviour, so it can access the Pilots position, velocity, Flock, etc.
	 */
	class Pilot {
		public:
			/**
			 * The type to be used for factory functions for Pilot.
			 */
			typedef Pilot* (*factoryType)(std::vector<std::pair<std::string,std::string> >*, Behaviour *, Steerable*, Flock*); 
			/**
			 * Create a new Pilot.
			 * @param att attributes to identify individual Pilots or store data.
			 * @param theFlock is a pointer to the associated Flock, if there is one.
			 * @param obj the vehicle steered by the Pilot
			 * @param attitude defines the Behaviour of the Pilot.
			 */
			Pilot ( std::map<std::string,std::string> att, Behaviour *attitude, Steerable *obj, Flock *theFlock = 0 );
			/**
			 * Delete the object.
			 * In contrast to using delete, this also works beyond DSO boundaries.
			 * I.e., you need this for using plugins.
			 * @param deregisterFromDB also deregister from UfoDB
			 */
			virtual void destroy(const bool deregisterFromDB=true);

			/**
			 * Calculate a steering decision and propagate it to the Steerable object.
			 */
			virtual void steer( const float elapsedTime);

			/**
			 * Returns the position of the steered object.
			 */
			gmtl::Point3f getPosition() const;

			/**
			 * Returns a quaternion describing the orientation of the steered object.
			 */
			gmtl::Quatf getOrientation() const;

			/**
			 * Returns the speed of the steered object.
			 */
			gmtl::Vec3f getVelocity() const;

			/**
			 * Returns an attribute value corresponding to key.
			 * Attributes are initialised with the parameters given to the Pilot.
			 * If the key doesn't exist, return "" (the empty string);
			 */
			virtual const std::string getAttribute(const std::string &key) const;

			/**
			 * Sets an attribute value.
			 * If an attribute is already set, the value is overwritten.
			 * If value is empty, the attribute is removed.
			 */
			virtual void setAttribute( std::string key, std::string value );

			/**
			 * Returns the Pilot's Flock, or 0 if the Pilot doesn't belong to any Flock.
			 */
			Flock *getFlock() const;

			/**
			 * Returns the Pilot's serial number.
			 * The serial number is unique for all Pilots, and
			 * is assigned at Pilot creation.
			 */
			const unsigned int getSN() const;

			/**
			 * Print information about the Pilot and its Behaviour.
			 */
			virtual void print() const;
		protected:
			/**
			 * The destructor.
			 */
			virtual ~Pilot();
		private:
			unsigned int SN;
			std::map<std::string,std::string> attributes;
			Behaviour *attitude;
			Steerable *vehicle;
			Flock *myFlock;
	};

	// this must match Pilot::factoryType:
	Pilot* PilotFactory (std::vector<std::pair<std::string,std::string> > *, Behaviour *, Steerable*, Flock*); 
} /* namespace */
#endif /* UFO_PILOT_H*/
