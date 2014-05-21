#include <iostream>
#include <sstream>
#include <string>

#include <gmtl/VecOps.h>
#include <gmtl/Quat.h>
#include <gmtl/QuatOps.h>

#include <ufo/Debug.h>
#include <ufo/Pilot.h>
#include <ufo/SteeringDecision.h>
#include <ufo/Plugin_def.h>

#include "AverageBehaviour.h"

using namespace gmtl;
using namespace ufo;
using namespace std;

	ufoplugin::AverageBehaviour::AverageBehaviour( const std::vector<ufo::Behaviour *> children, const bool skip)
: children(children), skip(skip)
{
	PRINT("INFO: AverageBehaviour has %u children.\n",(unsigned int)children.size());
}

ufoplugin::AverageBehaviour::~AverageBehaviour()
{
}

SteeringDecision ufoplugin::AverageBehaviour::yield ( const float elapsedTime)
{
	assert( myPilot != 0 );

	SteeringDecision retval;
	int used_d=0;
	int used_r=0;
	// first compute sum of Behaviours
	for (vector<Behaviour *>::const_iterator it = children.begin();
			it != children.end(); ++it)
	{
		SteeringDecision tmp = (*it)->yield(elapsedTime);
		// should we use the direction value?
		if ( tmp.directionUsed && ! ( skip && tmp.direction == Vec3f(0.0,0.0,0.0)) )
		{
			used_d++;
			retval.direction += tmp.direction;
		}
		// should we use the orientation value?
		if ( tmp.rotationUsed && !( skip && tmp.rotation == Quatf()) ) //default value?
		{
			// interpolate between new and existing rotations:
			slerp(retval.rotation, 1.0f/(1.0f+used_r), retval.rotation, tmp.rotation);
			used_r++;
		}
	}
	// then divide by number of (used) children
	if ( used_d > 0 )
	{
		retval.directionUsed = true;
		retval.direction /= used_d;
	}
	if ( used_r > 0 )
	{
		retval.rotationUsed = true;
	}

	return retval;
}

void ufoplugin::AverageBehaviour::print () const
{
	cout << "BEGIN AverageBehaviour (skip = " << boolalpha << skip << " )" <<endl;
	cout << "Children (" << children.size() << "):" <<endl;
	for (vector<Behaviour *>::const_iterator it = children.begin();
			it != children.end(); ++it)
	{
		(*it)->print();
	}
	cout << "END AverageBehaviour" <<endl;
}

void ufoplugin::AverageBehaviour::registerPilot( Pilot *p )
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

ufo::Behaviour *ufoplugin::AverageBehaviourFactory(std::vector<ufo::Behaviour *> children, std::vector<std::pair<std::string,std::string> > *params)
{
	bool skip = false;
	if ( children.size() == 0 )
	{
		cout << "ERROR: AverageBehaviour without children!\n" << endl;
		return 0;
	}
	for ( std::vector<std::pair<std::string,std::string> >::const_iterator it = params->begin();
			it != params->end(); ++it)
	{
		if ( it->first == "skip" )
		{
			stringstream value;
			value.str(it->second);
			cout << "AverageBehaviour parameter: skip = " << it->second <<endl;
			value >> boolalpha >> skip;
		}else {
			cout << "WARNING: unknown parameter to AverageBehaviour: " << it->first <<endl;
		}
	}
	return new AverageBehaviour(children,skip);
}

MAKEPLUGIN(BEHAVIOUR, Behaviour, AverageBehaviour, &ufoplugin::AverageBehaviourFactory)
