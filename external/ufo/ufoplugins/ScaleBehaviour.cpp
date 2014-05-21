#include <iostream>
#include <sstream>
#include <string>

#include <gmtl/VecOps.h>

#include <ufo/Debug.h>
#include <ufo/Pilot.h>
#include <ufo/SteeringDecision.h>
#include <ufo/Plugin_def.h>

#include "ScaleBehaviour.h"

using namespace gmtl;
using namespace ufo;
using namespace std;

	ufoplugin::ScaleBehaviour::ScaleBehaviour( ufo::Behaviour * child, const bool n, const float &f)
: child(child), normalise(n), scaleFactor(f)
{
}

ufoplugin::ScaleBehaviour::~ScaleBehaviour()
{
}

ufo::SteeringDecision ufoplugin::ScaleBehaviour::yield ( const float elapsedTime)
{
	assert( myPilot != 0 );

	SteeringDecision retval(child->yield(elapsedTime));
	
	// don't stretch vectors with length <= 1:
	if ( normalise && lengthSquared(retval.direction) > 1.0f )
		gmtl::normalize(retval.direction);

	retval.direction *= scaleFactor;

	return retval;
}

void ufoplugin::ScaleBehaviour::print () const
{
	cout << "BEGIN ScaleBehaviour ( normalise = " << boolalpha << normalise
		<< " , scaleFactor = " << scaleFactor << " )" <<endl;
	cout << "Child: "<<endl;
	child->print();
	cout << "END ScaleBehaviour" <<endl;
}

void ufoplugin::ScaleBehaviour::registerPilot( Pilot *p )
{
	assert(p != 0);
	// call default implementation:
	ufo::Behaviour::registerPilot( p );
	// propagate call:
	child->registerPilot(p);
}

ufo::Behaviour *ufoplugin::ScaleBehaviourFactory(std::vector<ufo::Behaviour *> children, std::vector<std::pair<std::string,std::string> > *params)
{
	Behaviour *child = 0;
	float scaleFactor = 0.0;
	bool normalise = false;

	if ( children.size() != 1 )
	{
		cout << "ERROR: ScaleBehaviour must have exactly 1 child!\n" << endl;
		return 0;
	}
	child = children.front();

	for ( std::vector<std::pair<std::string,std::string> >::const_iterator it = params->begin();
			it != params->end(); ++it)
	{
		if ( it->first == "scaleFactor" )
		{
			stringstream value;
			value.str(it->second);
			cout << "ScaleBehaviour parameter: scaleFactor = " << it->second <<endl;
			value >> scaleFactor;
		} else if ( it->first == "normalise" ) 
		{
			stringstream value;
			value.str(it->second);
			cout << "ScaleBehaviour parameter: normalise = " << it->second <<endl;
			value >> boolalpha >> normalise;
		} else {
			cout << "WARNING: unknown parameter to ScaleBehaviour: " << it->first <<endl;
		}
	}
	// if scaleFactor is set in configuration, use this instead of default scaleFactor:
	if ( scaleFactor != 0.0 )
	{
		return new ScaleBehaviour(child, normalise, scaleFactor);
	}
	return new ScaleBehaviour(child, normalise);
}

MAKEPLUGIN(BEHAVIOUR, Behaviour, ScaleBehaviour, &ufoplugin::ScaleBehaviourFactory)
