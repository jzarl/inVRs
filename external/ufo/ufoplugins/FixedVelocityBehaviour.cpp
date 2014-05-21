#include "FixedVelocityBehaviour.h"
#include <iostream>
#include <sstream>
#include <string>

#include <gmtl/VecOps.h>
#include <gmtl/Quat.h>
#include <gmtl/QuatOps.h>
#include <gmtl/Output.h>

#include <ufo/Debug.h>
#include <ufo/Pilot.h>
#include <ufo/SteeringDecision.h>
#include <ufo/Plugin_def.h>


using namespace std;
using namespace gmtl;
using namespace ufo;

	ufoplugin::FixedVelocityBehaviour::FixedVelocityBehaviour( const gmtl::Vec3f &velocity )
: velocity(velocity)
{
}

ufoplugin::FixedVelocityBehaviour::~FixedVelocityBehaviour()
{
}

ufo::SteeringDecision ufoplugin::FixedVelocityBehaviour::yield ( const float elapsedTime)
{
	assert( myPilot != 0 );
	return SteeringDecision (velocity);
}

void ufoplugin::FixedVelocityBehaviour::print () const
{
	cout << "BEGIN FixedVelocityBehaviour ( velocity = " << velocity
		<< " )" <<endl;
	cout << "END FixedVelocityBehaviour" <<endl;
}

ufo::Behaviour *ufoplugin::FixedVelocityBehaviourFactory(std::vector<ufo::Behaviour *> children, std::vector<std::pair<std::string,std::string> > *params)
{
	Vec3f v = Vec3f(0.0f,0.0f,0.0f);
	if ( children.size() != 0 )
	{
		cout << "WARNING: FixedVelocityBehaviour with children!\n" << endl;
	}
	for ( std::vector<std::pair<std::string,std::string> >::const_iterator it = params->begin();
			it != params->end(); ++it)
	{
		if ( it->first == "velocity" )
		{
			float x,y,z;
			stringstream value;
			value.str(it->second);
			cout << "FixedVelocityBehaviour parameter: velocity = " << it->second <<endl;
			value >> x >> y >> z;
			if ( value.bad() )
			{
				cout << "WARNING: bad velocity coordinates. Ignoring velocity..." << endl;
				cout << "Not accepted coordinates are: " << x << ", " << y << ", " << z <<"."<<endl;
			} else {
				v[0] = x;
				v[1] = y;
				v[2] = z;
			}
		}else {
			cout << "WARNING: unknown parameter to FixedVelocityBehaviour: " << it->first <<endl;
		}
	}
	if ( lengthSquared(v) == 0.0f )
	{
		cout << "WARNING: FixedVelocityBehaviour has velocity 0." <<endl;
	}
	return new FixedVelocityBehaviour(v);
}

MAKEPLUGIN(BEHAVIOUR, Behaviour, FixedVelocityBehaviour, &ufoplugin::FixedVelocityBehaviourFactory)
