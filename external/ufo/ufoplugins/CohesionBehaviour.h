#ifndef UFOPLUGIN_COHESIONBEHAVIOUR_H
#define UFOPLUGIN_COHESIONBEHAVIOUR_H

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
	 * An CohesionBehaviour tries to steer to the Flock center.
	 *
	 * Parameters:
	 * -----------
	 *  - neighbourDistance: distance within which a Pilot is considered a neighbour, -1.0 for no limit (float; default -1.0f)
	 *  - neighbourAngle: opening-angle in radians of the "cone of neighbourhood". Only Pilots within this cone are
	 *                    considered neighbours. A value of 2 PI is equal to a full sphere.(float; default: 2 PI)
	 */
	class CohesionBehaviour : public ufo::Behaviour
	{
		public:
			CohesionBehaviour(const float neighbourDistance, const float neighourAngle);

			ufo::SteeringDecision yield ( const float elapsedTime);
			void print () const;
		protected:
			virtual ~CohesionBehaviour();
		private:
			float neighbourDistance;
			float neighbourAngle;
	};

	ufo::Behaviour *CohesionBehaviourFactory(std::vector<ufo::Behaviour *> children, std::vector<std::pair<std::string,std::string> > *params);

}


#endif /* UFOPLUGIN_COHESIONBEHAVIOUR_H */
