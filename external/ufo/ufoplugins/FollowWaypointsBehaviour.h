#ifndef UFOPLUGIN_FOLLOWWAYPOINTS_H
#define UFOPLUGIN_FOLLOWWAYPOINTS_H

#include <gmtl/Point.h>

#include <ufo/Behaviour.h>

namespace ufo
{
	class SteeringDecision;
}

namespace ufoplugin
{

	/**
	 * A FollowWaypointsBehaviour following a configurable path.
	 * When the distance to a waypoint is smaller than or equal to epsilon, select the next waypoint.
	 * Set verbose = true, if you want to know whenever a new waypoint is selected.
	 */
	class FollowWaypointsBehaviour : public ufo::Behaviour
	{
		public:
			FollowWaypointsBehaviour(const bool verbose, const std::vector<gmtl::Point3f> path, const float epsilon, const gmtl::Vec3f ignore);

			ufo::SteeringDecision yield ( const float elapsedTime);
			void print () const;
		protected:
			virtual ~FollowWaypointsBehaviour();
		private:
			const float epsilon_squared;
			const std::vector<gmtl::Point3f> path;
			unsigned int idx;
			gmtl::Point3f wp;
			const bool verbose;
			/**
			 * Specifies which fields of direction are ignored, when computing the distance to the current waypoint.
			 * Component values: 1.0f to consider that component, 0.0f to ignore it.
			 */
			const gmtl::Vec3f ignore;
	};

	/**
	 * The factory function for FollowWaypointsBehaviour.
	 *
	 * Parameters:
	 *  - verbose = ["true"|"false"]: print message when reaching a waypoint
	 *  - waypoint = "floatx floaty floatz": add a waypoint
	 *  - epsilot = "float": required distance to consider a wapoint as "reached" (Default: 1.0f)
	 *  - ignoreComponent = ["x"|"y"|"z"]: don't consider coordinate component when determining if a waypoint has been
	 *    reached. For the steering decision, all components are always used. Can be used more than once.
	 */
	ufo::Behaviour *FollowWaypointsBehaviourFactory(std::vector<ufo::Behaviour *> children, std::vector<std::pair<std::string,std::string> > *params);

}


#endif /* UFOPLUGIN_FOLLOWWAYPOINTS_H */
