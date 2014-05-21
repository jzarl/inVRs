#ifndef UFOPLUGIN_FIXEDVELOCITYBEHAVIOUR_H
#define UFOPLUGIN_FIXEDVELOCITYBEHAVIOUR_H

#include <gmtl/Vec.h>
#include <ufo/Behaviour.h>
namespace ufo {
	class Pilot;
	class SteeringDecision;
}

namespace ufoplugin
{

	/**
	 * A FixedVelocityBehaviour returns a fixed speed.
	 *
	 * Parameters:
	 *   <code>velocity</code>: Vec3f
	 */
	class FixedVelocityBehaviour : public ufo::Behaviour
	{
		public:
			FixedVelocityBehaviour( const gmtl::Vec3f &velocity );

			ufo::SteeringDecision yield ( const float elapsedTime);
			void print () const;
		protected:
			virtual ~FixedVelocityBehaviour();
		private:
			const gmtl::Vec3f velocity;
	};

	ufo::Behaviour *FixedVelocityBehaviourFactory(std::vector<ufo::Behaviour *> children, std::vector<std::pair<std::string,std::string> > *params);

}

#endif /* UFOPLUGIN_FIXEDVELOCITYBEHAVIOUR_H */
