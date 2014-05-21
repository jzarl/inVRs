#ifndef UFOPLUGIN_STRONGESTBEHAVIOUR_H
#define UFOPLUGIN_STRONGESTBEHAVIOUR_H

#include <ufo/Behaviour.h>
namespace ufo {
	class Pilot;
	class SteeringDecision;
}

namespace ufoplugin
{

	/**
	 * A StrongestBehaviour chooses the child-behaviour with the "strongest" result.
	 * The strongest result is the one which is biggest in length.
	 * Should 2 Behaviours have the same length, the first in list takes precedence.
	 */
	class StrongestBehaviour : public ufo::Behaviour
	{
		public:
			StrongestBehaviour( const std::vector<ufo::Behaviour *> children);

			ufo::SteeringDecision yield ( const float elapsedTime);
			void print () const;
			void registerPilot( ufo::Pilot *thePilot);
		protected:
			virtual ~StrongestBehaviour();
		private:
			const std::vector<ufo::Behaviour *> children;
	};

	ufo::Behaviour *StrongestBehaviourFactory(std::vector<ufo::Behaviour *> children, std::vector<std::pair<std::string,std::string> > *params);

}

#endif /* UFOPLUGIN_STRONGESTBEHAVIOUR_H */
