#include <iostream>
#include <sstream>
#include <string>

#include <gmtl/VecOps.h>
#include <gmtl/QuatOps.h>

#include <ufo/Debug.h>
#include <ufo/Pilot.h>
#include <ufo/SteeringDecision.h>
#include <ufo/Plugin_def.h>

#include "DeltaMinBehaviour.h"

using namespace gmtl;
using namespace ufo;
using namespace std;

	ufoplugin::DeltaMinBehaviour::DeltaMinBehaviour( const std::vector<ufo::Behaviour *> children, const float &d)
: children(children), deltaMinSquared(d*d)
{
}

ufoplugin::DeltaMinBehaviour::~DeltaMinBehaviour()
{
}

ufo::SteeringDecision ufoplugin::DeltaMinBehaviour::yield ( const float elapsedTime)
{
	assert( myPilot != 0 );

	// use first behaviour with a length >= deltaMin
	for (vector<Behaviour *>::const_iterator it = children.begin();
			it != children.end(); ++it)
	{
		SteeringDecision tmp = (*it)->yield(elapsedTime);
		if ( lengthSquared(tmp.direction) >= deltaMinSquared )
		{
			// no need to consider the rest of child-behaviours
			return tmp;
		}
	}

	return SteeringDecision();
}

void ufoplugin::DeltaMinBehaviour::print () const
{
	cout << "BEGIN DeltaMinBehaviour (deltaMinSquared = " << deltaMinSquared << " )" <<endl;
	cout << "Children (" << children.size() << "):" <<endl;
	for (vector<Behaviour *>::const_iterator it = children.begin();
			it != children.end(); ++it)
	{
		(*it)->print();
	}
	cout << "END DeltaMinBehaviour" <<endl;
}

void ufoplugin::DeltaMinBehaviour::registerPilot( Pilot *p )
{
	assert(p != 0);
	// call default implementation:
	ufo::Behaviour::registerPilot( p );
	// propagate call:
	for (vector<Behaviour *>::const_iterator it = children.begin();
			it != children.end(); ++it)
	{
		(*it)->registerPilot(p);
	}
}

ufo::Behaviour *ufoplugin::DeltaMinBehaviourFactory(std::vector<ufo::Behaviour *> children, std::vector<std::pair<std::string,std::string> > *params)
{
	float deltaMin = -1.0;
	if ( children.size() == 0 )
	{
		cout << "ERROR: DeltaMinBehaviour without children!\n" << endl;
		return 0;
	}
	for ( std::vector<std::pair<std::string,std::string> >::const_iterator it = params->begin();
			it != params->end(); ++it)
	{
		if ( it->first == "deltaMin" )
		{
			stringstream value;
			value.str(it->second);
			cout << "DeltaMinBehaviour parameter: deltaMin = " << it->second <<endl;
			value >> deltaMin;
		}else {
			cout << "WARNING: unknown parameter to DeltaMinBehaviour: " << it->first <<endl;
		}
	}
	// if deltaMin is set in configuration, use this instead of default deltaMin:
	if ( deltaMin != -1.0 )
	{
		return new DeltaMinBehaviour(children, deltaMin);
	}
	cout << "WARNING: default value used for parameter deltaMin!" <<endl;
	return new DeltaMinBehaviour(children);
}

MAKEPLUGIN(BEHAVIOUR, Behaviour, DeltaMinBehaviour, &ufoplugin::DeltaMinBehaviourFactory)
