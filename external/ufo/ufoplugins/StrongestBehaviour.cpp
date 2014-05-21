#include <iostream>
#include <sstream>
#include <string>

#include <gmtl/VecOps.h>

#include <ufo/Debug.h>
#include <ufo/Pilot.h>
#include <ufo/SteeringDecision.h>
#include <ufo/Plugin_def.h>

#include "StrongestBehaviour.h"

using namespace gmtl;
using namespace ufo;
using namespace std;

	ufoplugin::StrongestBehaviour::StrongestBehaviour( const std::vector<ufo::Behaviour *> children)
: children(children)
{
}

ufoplugin::StrongestBehaviour::~StrongestBehaviour()
{
}

SteeringDecision ufoplugin::StrongestBehaviour::yield ( const float elapsedTime)
{
	assert( myPilot != 0 );

	SteeringDecision retval;
	float maxls=0;
	// whenever we find a "stronger" Behaviour, use that instead of the current one:
	for (vector<Behaviour *>::const_iterator it = children.begin();
			it != children.end(); ++it)
	{
		SteeringDecision tmp = (*it)->yield(elapsedTime);
		float tmpls = lengthSquared(tmp.direction);
		if ( tmpls > maxls )
		{
			retval= tmp;
			maxls = tmpls;
		}
	}

	return retval;
}

void ufoplugin::StrongestBehaviour::print () const
{
	cout << "BEGIN StrongestBehaviour " <<endl;
	cout << "Children (" << children.size() << "):" <<endl;
	for (vector<Behaviour *>::const_iterator it = children.begin();
			it != children.end(); ++it)
	{
		(*it)->print();
	}
	cout << "END StrongestBehaviour" <<endl;
}

void ufoplugin::StrongestBehaviour::registerPilot( Pilot *p )
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

ufo::Behaviour *ufoplugin::StrongestBehaviourFactory(std::vector<ufo::Behaviour *> children, std::vector<std::pair<std::string,std::string> > *params)
{
	if ( children.size() == 0 )
	{
		cout << "ERROR: StrongestBehaviour without children!\n" << endl;
		return 0;
	}
	if ( params->size() > 0 )
	{
		cout << "WARNING: StrongestBehaviour has parameters" << endl;
	}
	return new StrongestBehaviour(children);
}

MAKEPLUGIN(BEHAVIOUR, Behaviour, StrongestBehaviour, &ufoplugin::StrongestBehaviourFactory)
