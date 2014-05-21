#ifndef UFOPLUGIN_FOLLOWPILOTBEHAVIOUR_H
#define UFOPLUGIN_FOLLOWPILOTBEHAVIOUR_H

#include <gmtl/Point.h>
#include <gmtl/Vec.h>

#include <ufo/Behaviour.h>

namespace ufo
{
	class Pilot;
	class SteeringDecision;
}

namespace ufoplugin
{

	/**
	 * A FollowPilotBehaviour follows a Pilot.
	 *
	 * If <code>verbose</code> is set, a message is emitted whenever the
	 * distance changes its tendency. I.e. if we were approaching, and now 
	 * leave, or vice versa.
	 *
	 * The Pilot can be identified by an arbitrary attribute/value combination.
	 * If more than one Pilot matches the attribute/value combination, the first one is used.
	 * If no Pilot matches the criteria, and <code>monitorForChange</code> is set, the search
	 * is re-tried each frame -- during that time a null-vector is returned.
	 *
	 * If another Pilot should be choosen as soon as the attribute changes, set 
	 * <code>monitorForChange</code> to true.
	 *
	 * Parameters:
	 *  - attribute: attribute name identifying the Pilot (string)
	 *  - value: attribute value identifying the Pilot (string)
	 *  - monitorForChange: whether the attribute of the Pilot should be monitored
	 *    for change during each frame. (bool, default: false)
	 *  - extrapolate: should we just follow the Pilot, or extrapolate its
	 *    future position using its speed? (bool, default:false)
	 *  - offset: don't follow the Pilot directly, but follow at an offset (Vector)
	 *  - verbose: print debug information, whenever the Pilot changes.
	 *
	 *  TODO: implement offset pursuit
	 *
	 *  The FollowPilot by default resembles the "Seek" Behaviour as described by Reynolds.
	 *  If extrapolate (and optionally offset) is set, it implements the "Pursuit" (or "OffsetPursuit")
	 *  Behaviour described by Reynolds.
	 *
	 */
	class FollowPilotBehaviour : public ufo::Behaviour
	{
		public:
			FollowPilotBehaviour(const std::string att, const std::string val, const bool mfc
					, const bool extrapolate, const gmtl::Vec3f off, const bool verbose);

			ufo::SteeringDecision yield ( const float elapsedTime);
			void print () const;
		protected:
			virtual ~FollowPilotBehaviour();
		private:
			ufo::Pilot *followedPilot;
			const std::string attribute;
			const std::string value;
			const bool monitorForChange;
			const bool extrapolate;
			const gmtl::Vec3f offset;
			const bool verbose;

			ufo::Pilot *searchForPilot( const std::string &attribute, const std::string &value) const;
	};

	ufo::Behaviour *FollowPilotBehaviourFactory(std::vector<ufo::Behaviour *> children, std::vector<std::pair<std::string,std::string> > *params);

}



#endif /* UFOPLUGIN_FOLLOWPILOTBEHAVIOUR_H */
