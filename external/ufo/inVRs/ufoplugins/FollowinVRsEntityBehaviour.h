#ifndef UFOPLUGIN_FOLLOWINVRSENTITYBEHAVIOUR_H
#define UFOPLUGIN_FOLLOWINVRSENTITYBEHAVIOUR_H

#include <gmtl/Point.h>

#include <ufo/Behaviour.h>

class Entity;
namespace ufo
{
	class SteeringDecision;
}

namespace ufoplugin
{

	/**
	 * A FollowinVRsEntityBehaviour following an inVRs Entity.
	 *
	 * If <code>verbose</code> is set, a message is emitted whenever the
	 * distance changes its tendency. I.e. if we were approaching, and now 
	 * leave, or vice versa.
	 *
	 * The Entity can be defined just as with InVRsSteerable.
	 *
	 * @see InVRsSteerableFactory
	 */
	class FollowinVRsEntityBehaviour : public ufo::Behaviour
	{
		public:
			FollowinVRsEntityBehaviour(Entity *ent, const bool verbose);

			ufo::SteeringDecision yield ( const float elapsedTime);
			void print () const;
		protected:
			virtual ~FollowinVRsEntityBehaviour();
		private:
			Entity *theEntity;
			const bool verbose;
	};

	ufo::Behaviour *FollowinVRsEntityBehaviourFactory(std::vector<ufo::Behaviour *> children, std::vector<std::pair<std::string,std::string> > *params);

}


#endif /* UFOPLUGIN_FOLLOWINVRSENTITYBEHAVIOUR_H */
