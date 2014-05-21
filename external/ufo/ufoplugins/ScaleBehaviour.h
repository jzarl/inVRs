#ifndef UFOPLUGIN_SCALEBEHAVIOUR_H
#define UFOPLUGIN_SCALEBEHAVIOUR_H

#include <ufo/Behaviour.h>
namespace ufo {
	class Pilot;
	class SteeringDecision;
}

namespace ufoplugin
{

	/**
	 * A ScaleBehaviour applies a scale-factor to another Behaviour.
	 * Optionally, the vector can be normalised first by setting the "normalise" parameter.
	 */
	class ScaleBehaviour : public ufo::Behaviour
	{
		public:
			ScaleBehaviour( ufo::Behaviour *child, const bool normalise, const float &scaleFactor = 1.0f);

			ufo::SteeringDecision yield ( const float elapsedTime);
			void print () const;
			void registerPilot( ufo::Pilot *thePilot);
		protected:
			virtual ~ScaleBehaviour();
		private:
			ufo::Behaviour *child;
			const bool normalise;
			const float scaleFactor;
	};

	ufo::Behaviour *ScaleBehaviourFactory(std::vector<ufo::Behaviour *> children, std::vector<std::pair<std::string,std::string> > *params);

}


#endif /* UFOPLUGIN_SCALEBEHAVIOUR_H */
