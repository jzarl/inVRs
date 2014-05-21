#ifndef UFOPLUGIN_CACHINGBEHAVIOUR_H
#define UFOPLUGIN_CACHINGBEHAVIOUR_H

#include <ufo/Behaviour.h>
namespace ufo {
	class Pilot;
	class SteeringDecision;
}

namespace ufoplugin
{

	/**
	 * A CachingBehaviour assures that a minimum amount of time passes before recalculating a Behaviour.
	 * In the meantime, the cached result is returned.
	 */
	class CachingBehaviour : public ufo::Behaviour
	{
		public:
			CachingBehaviour( ufo::Behaviour *child, const float &deltaTMin);

			ufo::SteeringDecision yield ( const float elapsedTime);
			void print () const;
			void registerPilot( ufo::Pilot *thePilot);
		protected:
			virtual ~CachingBehaviour();
		private:
			ufo::Behaviour *child;
			ufo::SteeringDecision cachedResult;
			float deltaT;
			const float deltaTMin;
	};

	ufo::Behaviour *CachingBehaviourFactory(std::vector<ufo::Behaviour *> children, std::vector<std::pair<std::string,std::string> > *params);

}


#endif /* UFOPLUGIN_CACHINGBEHAVIOUR_H */
