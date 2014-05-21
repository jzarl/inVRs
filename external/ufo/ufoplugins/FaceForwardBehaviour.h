#ifndef UFOPLUGIN_FACEFORWARDBEHAVIOUR_H
#define UFOPLUGIN_FACEFORWARDBEHAVIOUR_H

#include <gmtl/Vec.h>

#include <ufo/Behaviour.h>

namespace ufo
{
	class SteeringDecision;
}

namespace ufoplugin
{

	/**
	 * The FaceForwardBehaviour rotates the Pilot to face the direction it is heading.
	 *
	 * <code>gravitationalUp</code> should be a vector pointing upwards (default: 0,1,0).
	 * <code>localForward</code> should be a vector pointing forward w.r.t. the Entity (default: 0,0,1).
	 *
	 * Three weights can be assigned to the three components of the orientation:
	 *  - <code>uprightWeight</code> default: 1
	 *  - <code>bankingWeight</code> default: 1
	 *
	 */
	class FaceForwardBehaviour : public ufo::Behaviour
	{
		public:
			FaceForwardBehaviour(const bool verbose, const gmtl::Vec3f gravitationalUp, const gmtl::Vec3f localForward,
					const float rawBankingWeight, const float rawUprightWeight);

			ufo::SteeringDecision yield ( const float elapsedTime);
			void print () const;
		protected:
			virtual ~FaceForwardBehaviour();
		private:
			const bool verbose;
			gmtl::Vec3f gravUp;
			gmtl::Vec3f localForward;
			float bankingWeight;

			float oldSpeed;
			gmtl::Vec3f oldDirection;
			gmtl::Point3f oldPosition;
			void updateOld( const float elapsedTime );
	};

	ufo::Behaviour *FaceForwardBehaviourFactory(std::vector<ufo::Behaviour *> children, std::vector<std::pair<std::string,std::string> > *params);

}

#endif /* UFOPLUGIN_FACEFORWARDBEHAVIOUR_H */
