#ifndef UFOPLUGIN_INVRSSTEERABLE_H
#define UFOPLUGIN_INVRSSTEERABLE_H

#include <gmtl/Point.h>
#include <gmtl/Vec.h>

namespace ufo
{
	class SteeringDecision;
}

#ifndef UFO_BUILD_INTERNAL
// get consistent defines:
#include <ufo/Ufo.h>
#endif

#include <ufo/Steerable.h>

class Entity;

namespace ufoplugin {

	/**
	 * InVRsSteerable can steer an Entity of the inVRs framework.
	 * It directly sets the TransformationData of the steered Entity.
	 *
	 * The speed of an InVRsSteerable is bounded by VMax (in units per second). 
	 * If VMax should not be applied, set it to -1.0f (=default).
	 *
	 * @see InVRsSteerableFactory
	 */
	class InVRsSteerable
		: public ufo::Steerable
	{
		public:

			InVRsSteerable( Entity *ent, const float VMax );

			void steer( const ufo::SteeringDecision& d, const float elapsedTime);
			/**
			 * Returns the position part of the TransformationData.
			 * The position is cached and only updated when steer() is called.
			 */
			gmtl::Point3f getPosition();
			/**
			 * Returns the orientation part of the TransformationData.
			 * The orientation is cached and only updated when steer() is called.
			 */
			gmtl::Quatf getOrientation();
			/**
			 * Returns computed velocity of the Entity.
			 * The velocity is cached and only updated when steer() is called.
			 * 
			 * If useSimplePhysicsEntity is used, then the velocity of the RigidBody is used.
			 */
			gmtl::Vec3f getVelocity();

			void print() const;
		protected:
			virtual ~InVRsSteerable();
		private:
			Entity *myEntity;
			// maximum speed value:
			float VMax;
			float deltaMinSquared;
			bool deactivateGravity;
			gmtl::Point3f position;
			gmtl::Quatf orientation;
			gmtl::Vec3f velocity;

	};

	/** The factory function.  This factory gets an Entity from the inVRs
	 * WorldDatabase and creates an ufoplugin::InVRsSteerable coupled with it.
	 *
	 * The Entity can be defined by an environmentBasedId, a typeInstanceId, by
	 * an (environmentId,entityId) pair, or by an (entityTypeId,instanceId)
	 * pair. If two or more of these are defined, they are tried in the order 
	 * mentioned above.
	 */
	ufo::Steerable *InVRsSteerableFactory ( std::vector<std::pair<std::string,std::string> > *params);

}

#endif /* UFOPLUGIN_INVRSSTEERABLE_H */
