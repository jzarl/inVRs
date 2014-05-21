#ifndef UFOPLUGIN_SAMPLESTEERABLE_H
#define UFOPLUGIN_SAMPLESTEERABLE_H

#include <ufo/Steerable.h>
namespace ufo
{
	class SteeringDecision;
}

namespace ufoplugin
{
	/**
	 * SampleSteerable provides a really basic Steerable implementation.
	 * Instead of interfacing with a graphics or scenegraph library, a
	 * SampleSteerable assumes that on every call to steer(), 0.1 seconds have
	 * elapsed (and adjusts its position accordingly by the current velocity). 
	 * SampleSteerable::steer(gmtl::Vec3f) directly sets the velocity.
	 */
	class SampleSteerable : public ufo::Steerable
	{
		public:
			SampleSteerable();

			void steer( const ufo::SteeringDecision &d, const float elapsedTime);
			gmtl::Point3f getPosition();
			gmtl::Quatf getOrientation();
			gmtl::Vec3f getVelocity();

			void print() const;
		protected:
			virtual ~SampleSteerable();
		private:
			gmtl::Point3f position;
			gmtl::Quatf orientation;
			gmtl::Vec3f velocity;
	};

	// maybe this has to be moved outside the namespace:
	ufo::Steerable *SampleSteerableFactory ( std::vector<std::pair<std::string,std::string> > *params);
}

#endif /* UFOPLUGIN_SAMPLESTEERABLE_H */
