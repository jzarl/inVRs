#include <iostream>
#include <sstream>
#include <string>

#include <gmtl/VecOps.h>

#include <ufo/Debug.h>
#include <ufo/Pilot.h>
#include <ufo/SteeringDecision.h>
#include <ufo/Plugin_def.h>

#include "SampleBehaviour.h"

using namespace gmtl;
using namespace ufo;
using namespace std;

	ufoplugin::SampleBehaviour::SampleBehaviour( const bool v,const float e)
: epsilon_squared(e*e),path(),verbose(v)
{
	path.push_back(Point3f( 0.0, 0.0, 0.0));
	path.push_back(Point3f(10.0, 0.0, 0.0));
	path.push_back(Point3f(10.0,10.0, 0.0));
	path.push_back(Point3f(10.0,10.0,10.0));
	nextWP = path.begin();
}

ufoplugin::SampleBehaviour::~SampleBehaviour()
{
}

ufo::SteeringDecision ufoplugin::SampleBehaviour::yield ( const float elapsedTime)
{
	assert( myPilot != 0 );

	SteeringDecision retval((*nextWP) - myPilot->getPosition());

	// are we getting near?
	if ( lengthSquared(retval.direction) <= epsilon_squared )
	{
		// select next waypoint,
		// wrap WP on path's end:
		if ( ++nextWP == path.end() )
		{
			nextWP = path.begin();
		}
		if ( verbose )
		{
			PRINT("Selecting next waypoint (%2.2f,%2.2f,%2.2f)\n"
					, (*nextWP)[0] , (*nextWP)[1] , (*nextWP)[2]);
		}
	}
	return retval;
}

void ufoplugin::SampleBehaviour::print () const
{ 
	cout << "BEGIN SampleBehaviour ( verbose = " << boolalpha << verbose 
		<< " , epsilon_squared = " << epsilon_squared << " )" <<endl;
	cout << "END SampleBehaviour" <<endl;
}

ufo::Behaviour *ufoplugin::SampleBehaviourFactory(std::vector<ufo::Behaviour *> children, std::vector<std::pair<std::string,std::string> > *params)
{
	float epsilon = -1.0;
	bool verbose = false;
	if ( children.size() != 0 )
	{
		cout << "WARNING: SampleBehaviour with children!\n" << endl;
	}
	for ( std::vector<std::pair<std::string,std::string> >::const_iterator it = params->begin();
			it != params->end(); ++it)
	{
		if ( it->first == "epsilon" )
		{
			stringstream value;
			value.str(it->second);
			cout << "SampleBehaviour parameter: epsilon = " << it->second <<endl;
			value >> epsilon;
		} if ( it->first == "verbose" )
		{
			stringstream value;
			value.str(it->second);
			cout << "SampleBehaviour parameter: verbose = " << it->second <<endl;
			value >> boolalpha >> verbose;
		}else {
			cout << "WARNING: unknown parameter to SampleBehaviour: " << it->first <<endl;
		}
	}
	// if epsilon is set in configuration, use this instead of default epsilon:
	if ( epsilon != -1.0 )
	{
		return new SampleBehaviour(verbose,epsilon);
	}
	return new SampleBehaviour(verbose);
}

MAKEPLUGIN(BEHAVIOUR, Behaviour, SampleBehaviour, &ufoplugin::SampleBehaviourFactory)
