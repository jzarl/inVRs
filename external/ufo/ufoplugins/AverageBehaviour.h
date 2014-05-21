#ifndef UFOPLUGIN_AVERAGEBEHAVIOUR_H
#define UFOPLUGIN_AVERAGEBEHAVIOUR_H

namespace ufo {
	class Pilot;
	class SteeringDecision;
}
#include <ufo/Behaviour.h>

namespace ufoplugin
{

	/**
	 * An AverageBehaviour computes the arithmetic mean of its children.
	 * If <code>skip</code> is set, children yielding a null-vector are ignored.
	 * The rotation and direction parts of the SteeringDecision are considered separately.
	 */
	class AverageBehaviour : public ufo::Behaviour
	{
		public:
			AverageBehaviour( const std::vector<ufo::Behaviour *> children, const bool skip);

			ufo::SteeringDecision yield ( const float elapsedTime);
			void print () const;
			void registerPilot( ufo::Pilot *thePilot);
		protected:
			virtual ~AverageBehaviour();
		private:
			const std::vector<ufo::Behaviour *> children;
			const bool skip;
	};

	ufo::Behaviour *AverageBehaviourFactory(std::vector<ufo::Behaviour *> children, std::vector<std::pair<std::string,std::string> > *params);

}

#endif /* UFOPLUGIN_AVERAGEBEHAVIOUR_H */
