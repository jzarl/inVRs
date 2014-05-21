#include <iostream>
#include <sstream>
#include <string>

#include <gmtl/VecOps.h>

#include <ufo/Debug.h>
#include <ufo/Pilot.h>
#include <ufo/SteeringDecision.h>
#include <ufo/Plugin_def.h>

#include "CachingBehaviour.h"

using namespace gmtl;
using namespace ufo;
using namespace std;

	ufoplugin::CachingBehaviour::CachingBehaviour( ufo::Behaviour * child, const float &dtm)
: child(child), cachedResult(), deltaT(dtm), deltaTMin(dtm)
{
}

ufoplugin::CachingBehaviour::~CachingBehaviour()
{
}

ufo::SteeringDecision ufoplugin::CachingBehaviour::yield ( const float elapsedTime)
{
	assert( myPilot != 0 );

	deltaT += elapsedTime;
	// is cached result stale?
	if ( deltaT >= deltaTMin )
	{
		// fetch current result:
		cachedResult = child->yield(deltaT);
		// reset time:
		deltaT = 0.0f;
	}
	
	return cachedResult;
}

void ufoplugin::CachingBehaviour::print () const
{
	cout << "BEGIN CachingBehaviour ( deltaTMin = " 
		<< deltaTMin  << " )" <<endl;
	cout << "Child: "<<endl;
	child->print();
	cout << "END CachingBehaviour" <<endl;
}

void ufoplugin::CachingBehaviour::registerPilot( Pilot *p )
{
	assert(p != 0);
	// call default implementation:
	ufo::Behaviour::registerPilot( p );
	// propagate call:
	child->registerPilot(p);
}

ufo::Behaviour *ufoplugin::CachingBehaviourFactory(std::vector<ufo::Behaviour *> children, std::vector<std::pair<std::string,std::string> > *params)
{
	Behaviour *child = 0;
	float deltaTMin = 0.0;

	if ( children.size() != 1 )
	{
		cout << "ERROR: CachingBehaviour must have exactly 1 child!\n" << endl;
		return 0;
	}
	child = children.front();

	for ( std::vector<std::pair<std::string,std::string> >::const_iterator it = params->begin();
			it != params->end(); ++it)
	{
		if ( it->first == "deltaTMin" )
		{
			stringstream value;
			value.str(it->second);
			cout << "CachingBehaviour parameter: deltaTMin = " << it->second <<endl;
			value >> deltaTMin;
		} else {
			cout << "WARNING: unknown parameter to CachingBehaviour: " << it->first <<endl;
		}
	}
	return new CachingBehaviour(child, deltaTMin);
}

MAKEPLUGIN(BEHAVIOUR, Behaviour, CachingBehaviour, &ufoplugin::CachingBehaviourFactory)
