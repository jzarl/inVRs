#include "ufoplugins/AlignmentBehaviour.h"
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

	ufoplugin::AlignmentBehaviour::AlignmentBehaviour(const float nd, const float na)
: neighbourDistance(nd), neighbourAngle(na)
{
}

ufoplugin::AlignmentBehaviour::~AlignmentBehaviour()
{
}

ufo::SteeringDecision ufoplugin::AlignmentBehaviour::yield ( const float elapsedTime)
{
	assert( myPilot != 0 );
	
	Flock *flock = myPilot->getFlock();
	assert( flock != 0 );

	SteeringDecision retval;
	int neighbours = 0;

	//accumulate direction
	for ( Flock::const_iterator it=flock->begin(); it != flock->end(); ++it)
	{
		// TODO: test angle:
		Vec3f distance = (*it)->getPosition() - myPilot->getPosition();
		// only consider "near" neighbors:
		if ( neighbourDistance < 0.0f || lengthSquared(distance) <= neighbourDistance*neighbourDistance )
		{
			retval.direction += (*it)->getVelocity();
			neighbours++;
		}
	}
	if ( neighbours > 0 )
	{
		// we are only interested in the direction:
		retval.directionUsed = true;
		// compute average direction:
		retval.direction /= neighbours;
	}
	return retval;
}

void ufoplugin::AlignmentBehaviour::print () const
{
	cout << "BEGIN AlignmentBehaviour ( "
		<< "neighbourDistance = " << neighbourDistance << ", neighbourAngle = " << neighbourAngle
		<< " )" << endl;
	cout << "END AlignmentBehaviour" <<endl;
}

ufo::Behaviour *ufoplugin::AlignmentBehaviourFactory(std::vector<ufo::Behaviour *> children, std::vector<std::pair<std::string,std::string> > *params)
{
	float neighbourDistance = -1.0f;
	float neighbourAngle = gmtl::Math::TWO_PI;
	if ( children.size() != 0 )
	{
		cout << "WARNING: AlignmentBehaviour with children!\n" << endl;
	}
	for ( std::vector<std::pair<std::string,std::string> >::const_iterator it = params->begin();
			it != params->end(); ++it)
	{
		if ( it->first == "neighbourDistance" )
		{
			stringstream value;
			value.str(it->second);
			cout << "AlignmentBehaviour parameter: neighbourDistance = " << it->second <<endl;
			value >> neighbourDistance;
		}else if ( it->first == "neighbourAngle" )
		{
			stringstream value;
			value.str(it->second);
			cout << "AlignmentBehaviour parameter: neighbourAngle = " << it->second <<endl;
			value >> neighbourAngle;
		}else {
			cout << "WARNING: unknown parameter to AlignmentBehaviour: " << it->first <<endl;
		}
	}
	return new AlignmentBehaviour(neighbourDistance,neighbourAngle);
}

MAKEPLUGIN(BEHAVIOUR, Behaviour, AlignmentBehaviour, &ufoplugin::AlignmentBehaviourFactory)
