#ifndef UFOPLUGIN_DELTAMINBEHAVIOUR_H
#define UFOPLUGIN_DELTAMINBEHAVIOUR_H

#include <ufo/Behaviour.h>
namespace ufo {
	class Pilot;
	class SteeringDecision;
}

namespace ufoplugin
{

	/**
	 * A DeltaMinBehaviour "clamps down" a SteeringDecision to zero, if its direction length is lower than
	 * a certain bound. 
	 * It yields a null-vector for the direction, if its child returns a vector shorter than deltaMin.
	 * If there is more than one child, the first child returning a vector >= deltaMin is used.
	 * Behaviour::yield() is <em>NOT</em> executed for the remaining children.
	 * With deltaMin == 0.0f, the DeltaMinBehaviour is equivalent to just using the first child-Beehaviour.
	 */
	class DeltaMinBehaviour : public ufo::Behaviour
	{
		public:
			DeltaMinBehaviour( const std::vector<ufo::Behaviour *> children, const float &deltaMin = 0.0f);

			ufo::SteeringDecision yield ( const float elapsedTime);
			void print () const;
			void registerPilot( ufo::Pilot *thePilot);
		protected:
			virtual ~DeltaMinBehaviour();
		private:
			const std::vector<ufo::Behaviour *> children;
			const float deltaMinSquared;
	};

	ufo::Behaviour *DeltaMinBehaviourFactory(std::vector<ufo::Behaviour *> children, std::vector<std::pair<std::string,std::string> > *params);

}

#endif /* UFOPLUGIN_DELTAMINBEHAVIOUR_H */
