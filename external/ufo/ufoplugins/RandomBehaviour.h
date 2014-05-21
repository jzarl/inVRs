#ifndef UFOPLUGIN_RANDOMBEHAVIOUR_H
#define UFOPLUGIN_RANDOMBEHAVIOUR_H

#include <gmtl/Point.h>

#include <ufo/Behaviour.h>

namespace ufo
{
	class SteeringDecision;
}

namespace ufoplugin
{

	/**
	 * The RandomBehaviour returns a random vector in the "front-hemisphere" of the pilot.
	 *
	 * The result vector lies somewhere between maxUp, maxSide and localForward.
	 */
	class RandomBehaviour : public ufo::Behaviour
	{
		public:
			RandomBehaviour(const bool verbose, const gmtl::Vec3f maxUp, const gmtl::Vec3f maxSide, const gmtl::Vec3f localForward);

			ufo::SteeringDecision yield ( const float elapsedTime);
			void print () const;
		protected:
			virtual ~RandomBehaviour();
		private:
			const bool verbose;
			const gmtl::Vec3f maxUp,maxSide,localForward;
	};

	ufo::Behaviour *RandomBehaviourFactory(std::vector<ufo::Behaviour *> children, std::vector<std::pair<std::string,std::string> > *params);

}

#endif /* UFOPLUGIN_RANDOMBEHAVIOUR_H */
