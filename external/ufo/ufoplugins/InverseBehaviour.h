#ifndef UFOPLUGIN_INVERSEBEHAVIOUR_H
#define UFOPLUGIN_INVERSEBEHAVIOUR_H

#include <gmtl/Point.h>

#include <ufo/Behaviour.h>

namespace ufo
{
	class SteeringDecision;
}

namespace ufoplugin
{

	/**
	 * A InverseBehaviour inverts the Behaviour of its child.
	 * The InverseBehaviour steers into the opposite direction as
	 * the original Behaviour, with a speed of <code>VMax</code>
	 * minus the speed of the original Behaviour.
	 *
	 * The resulting Behaviour always has a speed between 0 (original
	 * speed &gt;= <code>VMax</code>) and <code>VMax</code> (original
	 * speed = 0).
	 *
	 * Applying the InverseBehaviour, one can make a "Flee" Behaviour out of a
	 * "Seek" Behaviour, or an "Evade" Behaviour from a "Pursuit" Behaviour.
	 *
	 * @param VMax maximum speed of the original/inverse Behaviour (default:1.0f)
	 */
	class InverseBehaviour : public ufo::Behaviour
	{
		public:
			InverseBehaviour(ufo::Behaviour * child, const float VMax);

			void registerPilot( ufo::Pilot *thePilot);
			ufo::SteeringDecision yield ( const float elapsedTime);
			void print () const;
		protected:
			virtual ~InverseBehaviour();
		private:
			ufo::Behaviour *child;
			float VMax;
	};

	ufo::Behaviour *InverseBehaviourFactory(std::vector<ufo::Behaviour *> children, std::vector<std::pair<std::string,std::string> > *params);

}

#endif /* UFOPLUGIN_INVERSEBEHAVIOUR_H */
