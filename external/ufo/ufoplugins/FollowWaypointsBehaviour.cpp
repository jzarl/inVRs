#include <iostream>
#include <sstream>
#include <string>

#include <gmtl/VecOps.h>
#include <gmtl/Output.h>

#include <ufo/Debug.h>
#include <ufo/Pilot.h>
#include <ufo/SteeringDecision.h>
#include <ufo/Plugin_def.h>

#include "FollowWaypointsBehaviour.h"

using namespace std;
using namespace gmtl;
using namespace ufo;

	ufoplugin::FollowWaypointsBehaviour::FollowWaypointsBehaviour( const bool v,const std::vector<gmtl::Point3f> path, const float e, const gmtl::Vec3f i)
: epsilon_squared(e*e),path(path),idx(0),verbose(v),ignore(i)
{
	assert (path.size() >= 1 );
	wp=path[idx];
}

ufoplugin::FollowWaypointsBehaviour::~FollowWaypointsBehaviour()
{
}

ufo::SteeringDecision ufoplugin::FollowWaypointsBehaviour::yield ( const float elapsedTime)
{
	assert( myPilot != 0 );

	SteeringDecision retval;
	// we are only interested in the direction:
	retval.directionUsed = true;
	retval.direction = wp - myPilot->getPosition();
	// mask ignored components from direction:
	for (int i = 0; i<3; i++)
	{
		retval.direction[i] *= ignore[i];
	}

	// are we getting near?
	if ( lengthSquared(retval.direction) <= epsilon_squared )
	{
		// select next waypoint,
		// wrap WP on path's end:
		if ( ++idx >= path.size() )
			idx=0;
		wp = path[idx];
		if ( verbose )
		{
			PRINT("Selecting next waypoint (%2.2f,%2.2f,%2.2f)\n"
					, wp[0] , wp[1] , wp[2]);
		}
	}
	return retval;
}

void ufoplugin::FollowWaypointsBehaviour::print () const
{
	cout << "BEGIN FollowWaypointsBehaviour ( verbose = " << boolalpha << verbose 
		<< " , epsilon_squared = " << epsilon_squared 
		<< " , ignore = " << ignore << " )" <<endl;
	cout << "Waypoints: "<<endl;
	for (vector<Point3f>::const_iterator it = path.begin();
			it != path.end(); ++it)
	{
		cout << (*it) <<endl;
	}
	cout << "END FollowWaypointsBehaviour" <<endl;
}

ufo::Behaviour *ufoplugin::FollowWaypointsBehaviourFactory(std::vector<ufo::Behaviour *> children, std::vector<std::pair<std::string,std::string> > *params)
{
	float epsilon = 1.0f; //default value
	bool verbose = false;
	Vec3f ignore = Vec3f(1.0f,1.0f,1.0f); //default: use all components
	std::vector<Point3f> path;
	if ( children.size() != 0 )
	{
		cout << "WARNING: FollowWaypointsBehaviour with children!\n" << endl;
	}
	for ( std::vector<std::pair<std::string,std::string> >::const_iterator it = params->begin();
			it != params->end(); ++it)
	{
		if ( it->first == "epsilon" )
		{
			stringstream value;
			value.str(it->second);
			cout << "FollowWaypointsBehaviour parameter: epsilon = " << it->second <<endl;
			value >> epsilon;
		} else if ( it->first == "verbose" )
		{
			stringstream value;
			value.str(it->second);
			cout << "FollowWaypointsBehaviour parameter: verbose = " << it->second <<endl;
			value >> boolalpha >> verbose;
		} else if ( it->first == "ignore" )
		{
			cout << "FollowWaypointsBehaviour parameter: ignore = " << it->second <<endl;
			if ( it->second == "x" || it->second == "X" )
			{
				ignore[0] = 0.0f;
			} else if ( it->second == "y" || it->second == "Y" )
			{
				ignore[1] = 0.0f;
			} else if ( it->second == "z" || it->second == "Z" )
			{
				ignore[2] = 0.0f;
			} else {
				cout << "WARNING: invalid component specified. Use 'x', 'y,', or 'z'." <<endl;
			}
		} else if ( it->first == "waypoint" )
		{
			float x,y,z;
			stringstream value;
			value.str(it->second);
			cout << "FollowWaypointsBehaviour parameter: waypoint(raw) = " << it->second <<endl;
			value >> x >> y >> z;
			//cout << "FollowWaypointsBehaviour parameter: waypointp(real) = " << x << " " << y << " " << z <<endl;
			if ( value.bad() )
			{
				cout << "WARNING: bad waypoint coordinates. Ignoring waypoint..." << endl;
				cout << "Not accepted coordinates are: " << x << ", " << y << ", " << z <<"."<<endl;
			} else {
				path.push_back(Point3f(x,y,z));
			}
		}else {
			cout << "WARNING: unknown parameter to FollowWaypointsBehaviour: " << it->first <<endl;
		}
	}
	if ( path.size() < 1 )
	{
		cout << "ERROR: FollowWaypointsBehaviour needs at least one waypoint!" <<endl;
		return 0;
	}
	return new FollowWaypointsBehaviour(verbose, path, epsilon, ignore);
}

MAKEPLUGIN(BEHAVIOUR, Behaviour, FollowWaypointsBehaviour, &ufoplugin::FollowWaypointsBehaviourFactory)
