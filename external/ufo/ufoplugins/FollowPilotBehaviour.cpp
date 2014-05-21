#include "ufoplugins/FollowPilotBehaviour.h"
#include <iostream>
#include <sstream>
#include <string>

#include <gmtl/VecOps.h>
#include <gmtl/Quat.h>
#include <gmtl/QuatOps.h>
#include <gmtl/Output.h>
#include <gmtl/Xforms.h>

#include <ufo/Debug.h>
#include <ufo/Pilot.h>
#include <ufo/SteeringDecision.h>
#include <ufo/Plugin_def.h>
#include <ufo/UfoDB.h>

using namespace std;
using namespace gmtl;
using namespace ufo;

	ufoplugin::FollowPilotBehaviour::FollowPilotBehaviour(
			const std::string att, const std::string val, const bool mfc, const bool exp,  const gmtl::Vec3f off, const bool v)
: followedPilot(0), attribute(att), value(val), monitorForChange(mfc)
	, extrapolate(exp), offset(off), verbose(v)
{
	// don't search for the Pilot yet, because it probably doesn't exist anyways
}

ufoplugin::FollowPilotBehaviour::~FollowPilotBehaviour()
{
}

ufo::SteeringDecision ufoplugin::FollowPilotBehaviour::yield ( const float elapsedTime)
{
	static float oldDistanceSquared = 0.0;
	static float approaching = true;
	assert( myPilot != 0 );
	if ( followedPilot )
	{
		if ( monitorForChange )
		{
			if ( followedPilot->getAttribute(attribute) != value )
			{
				// Pilot was "invalidated" -> search again
				followedPilot = searchForPilot(attribute,value);
				if ( verbose )
				{
					cout << "FollowPilotBehaviour(" << myPilot->getSN() << ": "
						<< attribute << "=" << value << ": ";
					if ( followedPilot )
					{
						cout << "new Pilot has SN " << followedPilot->getSN() << endl;
					} else {
						cout << "new Pilot is null" <<endl;
					}
				}
			}
		}
	} else {
		// search Pilot:
		followedPilot = searchForPilot(attribute,value);
		if ( followedPilot && verbose )
		{
			cout << "FollowPilotBehaviour(" << myPilot->getSN() << ": "
				<< attribute << "=" << value << ": found Pilot with SN " 
				<< followedPilot->getSN() << endl;
		}
	}

	SteeringDecision retval;
	// we are only interested in the direction:
	retval.directionUsed = true;
	// follow the Pilot, if possible
	if ( followedPilot )
	{
		retval.direction = followedPilot->getPosition();
		if ( offset != Vec3f(0.0f,0.0f,0.0f) ) //do we use offset?
		{
			// why doesn't operator* work here???
			Vec3f tmp;
			retval.direction += xform(tmp,myPilot->getOrientation(),offset);
		}
		if ( extrapolate )
		{
			// where is the Pilot going?
			retval.direction += followedPilot->getVelocity() * elapsedTime;
		}
		retval.direction -= myPilot->getPosition();
		if ( verbose )
		{
			float distanceSquared = lengthSquared(retval.direction);
			// we were previously getting nearer?
			if ( approaching )
			{
				// print data about nearest approach:
				if ( oldDistanceSquared <= distanceSquared + 0.05f )
				{
					cout << "Nearest approach just before " << myPilot->getPosition() <<endl;
					cout << "Distance of nearest approach: " << sqrt(oldDistanceSquared) <<endl;
					approaching = false;
				}
			} else {
				// print data about largest distance:
				if ( oldDistanceSquared + 0.05f >= distanceSquared )
				{
					cout << "Longest distance reached just before " << myPilot->getPosition() <<endl;
					cout << "Longest distance was: " << sqrt(oldDistanceSquared) <<endl;
					approaching = true;
				}
			}
			oldDistanceSquared = distanceSquared;
		}
	}
	return retval;
}

void ufoplugin::FollowPilotBehaviour::print () const
{
	cout << "BEGIN FollowPilotBehaviour ( "
		<< "attribute = " << attribute << ", value = " << value
		<< ", monitorForChange = " << boolalpha << monitorForChange
		<< ", extrapolate = " << boolalpha << extrapolate 
		<< ", offset = " << offset
		<< ", verbose = " << boolalpha << verbose << " )" << endl;
	cout << "END FollowPilotBehaviour" <<endl;
}

ufo::Pilot *ufoplugin::FollowPilotBehaviour::searchForPilot( const std::string &attribute, const std::string &value) const
{
	std::vector <Pilot *> pilots = UfoDB::the()->getPilotsByAttribute(attribute, value);
	if ( pilots.size() > 0 )
	{
		return pilots[0];
	}
	// no Pilot found
	return 0;
}

ufo::Behaviour *ufoplugin::FollowPilotBehaviourFactory(std::vector<ufo::Behaviour *> children, std::vector<std::pair<std::string,std::string> > *params)
{
	std::string attribute;
	std::string value;
	bool monitorForChange = false;
	bool extrapolate = false;
	Vec3f offset = Vec3f(0.0f,0.0f,0.0f);
	bool verbose = false;
	if ( children.size() != 0 )
	{
		cout << "WARNING: FollowPilotBehaviour with children!\n" << endl;
	}
	for ( std::vector<std::pair<std::string,std::string> >::const_iterator it = params->begin();
			it != params->end(); ++it)
	{
		if ( it->first == "attribute" )
		{
			cout << "FollowPilotBehaviour parameter: attribute = " << it->second <<endl;
			attribute = it->second;
		}else if ( it->first == "value" )
		{
			cout << "FollowPilotBehaviour parameter: value = " << it->second <<endl;
			value = it->second;
		}else if ( it->first == "offset" )
		{
			float x,y,z;
			stringstream value;
			value.str(it->second);
			cout << "FollowPilotBehaviour parameter: offset = " << it->second <<endl;
			value >> x >> y >> z;
			if ( value.bad() )
			{
				cout << "WARNING: bad offset coordinates. Ignoring offset..." << endl;
				cout << "Not accepted coordinates are: " << x << ", " << y << ", " << z <<"."<<endl;
			} else {
				offset = Vec3f(x,y,z);
			}
		}else if ( it->first == "monitorForChange" )
		{
			stringstream value;
			value.str(it->second);
			cout << "FollowPilotBehaviour parameter: monitorForChange = " << it->second <<endl;
			value >> boolalpha >> monitorForChange;
		}else if ( it->first == "extrapolate" )
		{
			stringstream value;
			value.str(it->second);
			cout << "FollowPilotBehaviour parameter: extrapolate = " << it->second <<endl;
			value >> boolalpha >> extrapolate;
		}else if ( it->first == "verbose" )
		{
			stringstream value;
			value.str(it->second);
			cout << "FollowPilotBehaviour parameter: verbose = " << it->second <<endl;
			value >> boolalpha >> verbose;
		}else {
			cout << "WARNING: unknown parameter to FollowPilotBehaviour: " << it->first <<endl;
		}
	}
	if ( attribute == "" )
	{
		//note: an empty value should be fine
		cout << "ERROR: FollowPilotBehaviour: parameter 'attribute' must be set!" <<endl;
		return 0;

	}
	return new FollowPilotBehaviour(attribute,value,monitorForChange,extrapolate,offset,verbose);
}

MAKEPLUGIN(BEHAVIOUR, Behaviour, FollowPilotBehaviour, &ufoplugin::FollowPilotBehaviourFactory)
