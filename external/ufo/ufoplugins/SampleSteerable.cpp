#include <iostream>

#include <gmtl/VecOps.h>
#include <gmtl/Quat.h>

#include <ufo/Debug.h>
#include <ufo/SteeringDecision.h>
#include <ufo/Plugin_def.h>

#include "SampleSteerable.h"

using namespace gmtl;
using namespace ufo;
using namespace std;

	ufoplugin::SampleSteerable::SampleSteerable()
: position(Point3f(0.0,0.0,0.0)), velocity(Vec3f(0.0,0.0,0.0))
{
}

ufoplugin::SampleSteerable::~SampleSteerable()
{
}

void ufoplugin::SampleSteerable::steer( const ufo::SteeringDecision &d, const float elapsedTime)
{
	position += velocity * elapsedTime;
	velocity = d.direction;
	orientation = d.rotation;
}

gmtl::Point3f ufoplugin::SampleSteerable::getPosition()
{
	return position;
}

gmtl::Quatf ufoplugin::SampleSteerable::getOrientation()
{
	return orientation;
}

gmtl::Vec3f ufoplugin::SampleSteerable::getVelocity()
{
	return velocity;
}

void ufoplugin::SampleSteerable::print() const
{
	cout << "SampleSteerable" <<endl;
}

ufo::Steerable *ufoplugin::SampleSteerableFactory ( std::vector<std::pair<std::string,std::string> > *params)
{
	if ( params->size() != 0 )
	{
		cout << "WARNING: SampleBehaviour with parameters!\n" << endl;
	}
	return new SampleSteerable();
}

MAKEPLUGIN(STEERABLE, Steerable, SampleSteerable,&ufoplugin::SampleSteerableFactory)
