#ifndef UFOPLUGIN_SAMPLEBEHAVIOUR_H
#define UFOPLUGIN_SAMPLEBEHAVIOUR_H

#include <gmtl/Point.h>

#include <ufo/Behaviour.h>

namespace ufo
{
	class SteeringDecision;
}

namespace ufoplugin
{

	/**
	 * A SampleBehaviour following a predefined path.
	 * When the distance to a waypoint is smaller than or equal to epsilon, select the next waypoint.
	 * Set verbose = true, if you want to know whenever a new waypoint is selected.
	 */
	class SampleBehaviour : public ufo::Behaviour
	{
		public:
			SampleBehaviour(const bool verbose, const float epsilon = 1.0f);

			ufo::SteeringDecision yield ( const float elapsedTime);
			void print () const;
		protected:
			virtual ~SampleBehaviour();
		private:
			float epsilon_squared;
			std::vector<gmtl::Point3f> path;
			std::vector<gmtl::Point3f>::iterator nextWP;
			bool verbose;
	};

	ufo::Behaviour *SampleBehaviourFactory(std::vector<ufo::Behaviour *> children, std::vector<std::pair<std::string,std::string> > *params);

}

#endif /* UFOPLUGIN_SAMPLEBEHAVIOUR_H */
