#ifndef UFO_STEERINGDECISION_H
#define UFO_STEERINGDECISION_H

#include <gmtl/Vec.h>
#include <gmtl/Quat.h>

namespace ufo
{
	/**
	 * A struct holding a Steering decision, i.e. a vector for the
	 * speed an a Quaternion for the rotation.
	 */
	class SteeringDecision
	{
		public:
			SteeringDecision();
			SteeringDecision(const gmtl::Vec3f d);
			SteeringDecision(const gmtl::Quatf r);
			SteeringDecision(const gmtl::Vec3f d, const gmtl::Quatf r) ;
			/**
			 * The desired speed, or the vector to the desired navigational target.
			 */
			gmtl::Vec3f direction;
			/**
			 * True, if the direction has been explicitly set.
			 */ 
			bool directionUsed;
			/**
			 * A quaternion describing the rotation needed to obtain the desired orientation.
			 */
			gmtl::Quatf rotation;
			/**
			 * True, if the rotation has been explicitly set.
			 */ 
			bool rotationUsed;
		private:
			//static const gmtl::Vec3f default_direction = gmtl::Vec3f(0.0f,0.0f,0.0f);
			//static const gmtl::Quatf default_rotation = gmtl::Quatf(0.0f, 0.0f, 0.0f, 1.0f );
	};
}

inline ufo::SteeringDecision::SteeringDecision()
	: direction(),directionUsed(false), rotation(),rotationUsed(false) 
{
}

inline ufo::SteeringDecision::SteeringDecision(const gmtl::Vec3f d, const gmtl::Quatf r)
	: direction(d),directionUsed(true), rotation(r),rotationUsed(true) 
{
}

inline ufo::SteeringDecision::SteeringDecision(const gmtl::Vec3f d)
	: direction(d),directionUsed(true), rotation(),rotationUsed(false)
{
}

inline ufo::SteeringDecision::SteeringDecision(const gmtl::Quatf r)
	: direction(),directionUsed(false), rotation(r),rotationUsed(true)
{
}

#endif /* UFO_STEERINGDECISION_H */
