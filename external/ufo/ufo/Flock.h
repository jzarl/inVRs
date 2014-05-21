#ifndef UFO_FLOCK_H
#define UFO_FLOCK_H

#include <vector>
#include <map>
#include <string>


namespace ufo
{
	class Pilot;

	/**
	 * A Flock really just is an std::vector of Pilots with some additional methods.
	 *
	 * <h3>CAVEAT</h3> 
	 * Unlike an std::vector<Pilot*>, the flock claims ownership of the Pilots.
	 * I.e. when deleting the Flock, the Pilots are also deleted.
	 *
	 */
	class Flock : public std::vector< Pilot*>
	{
		public:
			/**
			 * The type to be used for factory functions for Flock.
			 */
			typedef Flock* (*factoryType)(std::vector<std::pair<std::string,std::string> >*); 

			/**
			 * Constructor for use with the factory method.
			 */
			Flock(std::map<std::string,std::string> attributes);

			/**
			 * Add a Pilot to the Flock.
			 */
			//virtual void add (Pilot *p);

			/**
			 * Removes the Pilot with the given serial number.
			 * The removed Pilot is deleted.
			 * @return <code>true</code> on success, <code>false</code> if no Pilot with SN is in the Flock.
			 */
			//virtual bool erase (unsigned int sn);

			/**
			 * Removes the Pilot at iterator first and all Pilots following excluding iterator last.
			 * The removed Pilots are deleted.
			 * @return the number of removed Pilots.
			 */
			//virtual unsigned int erase(iterator first, iterator last);

			/**
			 * Delete the object.
			 * In contrast to using delete, this also works beyond DSO boundaries.
			 * I.e., you need this for using plugins.
			 * @param deregisterFromDB also remove the Flock from the UfoDB
			 */
			virtual void destroy(const bool deregisterFromDB=true);

			/**
			 * Call Pilot::steer(elapsedTime) for each Pilot in the Flock.
			 */
			virtual void update( const float elapsedTime) const;

			/**
			 * Returns the Pilot's serial number.
			 * The serial number is unique for all Pilots, and
			 * is assigned at Pilot creation.
			 */
			const unsigned int getSN() const;

			/**
			 * Returns an attribute value corresponding to key.
			 * Attributes are initialised with the parameters given to the Pilot.
			 * If the key doesn't exist, return "" (the empty string);
			 */
			const std::string getAttribute(const std::string &key) const;

			/**
			 * Sets an attribute value.
			 * If an attribute is already set, the value is overwritten.
			 * If value is empty, the attribute is removed.
			 */
			virtual void setAttribute( std::string key, std::string value );

		protected:
			/**
			 * The destructor.
			 * Delete all Pilots belonging to the Flock.
			 */
			virtual ~Flock();

			/**
			 * This allows for things like identifying a flock by id etc.
			 */
			std::map<std::string,std::string> attributes;
		private:
			unsigned int SN;
	};

	// this has to match factoryType:
	Flock* FlockFactory(std::vector<std::pair<std::string,std::string> >*);
}

#endif /* UFO_FLOCK_H */
