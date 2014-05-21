#ifndef UFOPLUGIN_INVRsPhysicsSteerable_H
#define UFOPLUGIN_INVRsPhysicsSteerable_H

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

class SimplePhysicsEntity;

namespace ufoplugin {

	/**
	 * InVRsPhysicsSteerable can steer a SimplePhysicsEntity of the inVRs framework.
	 * The entity is steered by calling SimplePhysicsEntity::addForce().
	 *
	 * If the Entity is a PhysicsEntity and deltaMin is set, then the force is only 
	 * applied, if the length of the direction vector is bigger than deltaMin.
	 * The gravitational pull on a PhysicsEntity can be turned off (as long as it is steered)
	 * by setting deactivateGravity.
	 *
	 * The speed of an InVRsPhysicsSteerable is bounded by VMax (in units per second). 
	 * If VMax should not be applied, set it to -1.0f (=default).
	 *
	 * @see InVRsPhysicsSteerableFactory
	 */
	class InVRsPhysicsSteerable
		: public ufo::Steerable
	{
		public:

			InVRsPhysicsSteerable( SimplePhysicsEntity *ent, const float VMax, const float deltaMin, const bool deactivateGravity ); 

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
			 * The velocity returned is the same as of the RigidBody.
			 */
			gmtl::Vec3f getVelocity();

			void print() const;
		protected:
			virtual ~InVRsPhysicsSteerable();
		private:
			SimplePhysicsEntity *myEntity;
			// maximum speed value:
			float VMax;
			float deltaMinSquared;
			bool deactivateGravity;
			gmtl::Point3f position;
			gmtl::Quatf orientation;
			gmtl::Vec3f velocity;

	};

	/** The factory function.  This factory gets an Entity from the inVRs
	 * WorldDatabase and creates an ufoplugin::InVRsPhysicsSteerable coupled with it.
	 *
	 * The Entity can be defined by an environmentBasedId, a typeInstanceId, by
	 * an (environmentId,entityId) pair, or by an (entityTypeId,instanceId)
	 * pair. If two or more of these are defined, they are tried in the order 
	 * mentioned above.
	 */
	ufo::Steerable *InVRsPhysicsSteerableFactory ( std::vector<std::pair<std::string,std::string> > *params);

}

#endif /* UFOPLUGIN_INVRsPhysicsSteerable_H */
