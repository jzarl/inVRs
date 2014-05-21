#include "InverseBehaviour.h"
#include <iostream>
#include <sstream>
#include <string>

#include <gmtl/VecOps.h>

#include <ufo/Debug.h>
#include <ufo/Pilot.h>
#include <ufo/SteeringDecision.h>
#include <ufo/Plugin_def.h>

using namespace gmtl;
using namespace ufo;
using namespace std;

	ufoplugin::InverseBehaviour::InverseBehaviour( Behaviour *c,const float VMax)
: child(c),VMax(VMax)
{
}

ufoplugin::InverseBehaviour::~InverseBehaviour()
{
}

void ufoplugin::InverseBehaviour::registerPilot( Pilot *p )
{
	assert(p != 0);
	// call default implementation:
	ufo::Behaviour::registerPilot( p );
	// propagate call:
	child->registerPilot(p);
}

ufo::SteeringDecision ufoplugin::InverseBehaviour::yield ( const float elapsedTime)
{
	assert( myPilot != 0 );

	// query original Behaviour:
	SteeringDecision retval(child->yield(elapsedTime));

	//normalize returns the original Vector length as side-effect:
	float scale = normalize(retval.direction);
	if ( scale != 0.0f )
	{
		scale -= VMax;

		if ( scale >= 0.0f )
		{
			scale = 0.0f;
		}
		retval.direction *= scale;
	} else { 
		// the original Behaviour is satisfied with this place,
		// so for us, _any_ place is better than this:
		// fixme: add randomness?
		retval.direction = Vec3f(1.0f,1.0f,1.0f) * VMax;
	}
	return retval;
}

void ufoplugin::InverseBehaviour::print () const
{ 
	cout << "BEGIN InverseBehaviour ( VMax = " << VMax << " )" <<endl;
	child->print();
	cout << "END InverseBehaviour" <<endl;
}

ufo::Behaviour *ufoplugin::InverseBehaviourFactory(std::vector<ufo::Behaviour *> children, std::vector<std::pair<std::string,std::string> > *params)
{
	float VMax = 1.0;
	if ( children.size() != 1 )
	{
		cout << "ERROR: InverseBehaviour must have exactly one  child!\n" << endl;
		return 0;
	}
	for ( std::vector<std::pair<std::string,std::string> >::const_iterator it = params->begin();
			it != params->end(); ++it)
	{
		if ( it->first == "VMax" )
		{
			stringstream value;
			value.str(it->second);
			cout << "InverseBehaviour parameter: VMax = " << it->second <<endl;
			value >> VMax;
		}else {
			cout << "WARNING: unknown parameter to InverseBehaviour: " << it->first <<endl;
		}
	}
	return new InverseBehaviour(children[0],VMax);
}

MAKEPLUGIN(BEHAVIOUR, Behaviour, InverseBehaviour, &ufoplugin::InverseBehaviourFactory)
