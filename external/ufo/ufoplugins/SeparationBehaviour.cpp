#include "ufoplugins/SeparationBehaviour.h"
#include <iostream>
#include <sstream>
#include <string>

#include <gmtl/VecOps.h>
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

	ufoplugin::SeparationBehaviour::SeparationBehaviour(const float nd, const float na)
: neighbourDistance(nd), neighbourAngle(na)
{
}

ufoplugin::SeparationBehaviour::~SeparationBehaviour()
{
}

ufo::SteeringDecision ufoplugin::SeparationBehaviour::yield ( const float elapsedTime)
{
	assert( myPilot != 0 );
	
	Flock *flock = myPilot->getFlock();
	assert( flock != 0 );

	SteeringDecision retval;
	int neighbours = 0;

	//accumulate evasive actions
	for ( Flock::const_iterator it=flock->begin(); it != flock->end(); ++it)
	{
		// TODO: test radius and angle:
		// direction should point away from neighbour:
		gmtl::Vec3f direction = myPilot->getPosition() - (*it)->getPosition();
		if ( neighbourDistance < 0.0f || lengthSquared(direction) <= neighbourDistance*neighbourDistance )
		{
			// weigh direction by proximity:
			//if ( lengthSquared(direction) >= 0.001f )
			//	direction /= lengthSquared(direction); // = normalize(direction); direction /= length;
			retval.direction += direction;
			neighbours++;
		}
	}
	// compute average action::
	if ( neighbours > 0 )
	{
		// we are only interested in the direction:
		retval.directionUsed = true;
		retval.direction /= neighbours;
	}
	return retval;
}

void ufoplugin::SeparationBehaviour::print () const
{
	cout << "BEGIN SeparationBehaviour ( "
		<< "neighbourDistance = " << neighbourDistance << ", neighbourAngle = " << neighbourAngle
		<< " )" << endl;
	cout << "END SeparationBehaviour" <<endl;
}

ufo::Behaviour *ufoplugin::SeparationBehaviourFactory(std::vector<ufo::Behaviour *> children, std::vector<std::pair<std::string,std::string> > *params)
{
	float neighbourDistance = -1.0f;
	float neighbourAngle = gmtl::Math::TWO_PI;
	if ( children.size() != 0 )
	{
		cout << "WARNING: SeparationBehaviour with children!\n" << endl;
	}
	for ( std::vector<std::pair<std::string,std::string> >::const_iterator it = params->begin();
			it != params->end(); ++it)
	{
		if ( it->first == "neighbourDistance" )
		{
			stringstream value;
			value.str(it->second);
			cout << "SeparationBehaviour parameter: neighbourDistance = " << it->second <<endl;
			value >> neighbourDistance;
		}else if ( it->first == "neighbourAngle" )
		{
			stringstream value;
			value.str(it->second);
			cout << "SeparationBehaviour parameter: neighbourAngle = " << it->second <<endl;
			value >> neighbourAngle;
		}else {
			cout << "WARNING: unknown parameter to SeparationBehaviour: " << it->first <<endl;
		}
	}
	return new SeparationBehaviour(neighbourDistance,neighbourAngle);
}

MAKEPLUGIN(BEHAVIOUR, Behaviour, SeparationBehaviour, &ufoplugin::SeparationBehaviourFactory)
