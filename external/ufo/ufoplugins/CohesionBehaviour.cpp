#include "ufoplugins/CohesionBehaviour.h"
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

	ufoplugin::CohesionBehaviour::CohesionBehaviour(const float nd, const float na)
: neighbourDistance(nd), neighbourAngle(na)
{
}

ufoplugin::CohesionBehaviour::~CohesionBehaviour()
{
}

ufo::SteeringDecision ufoplugin::CohesionBehaviour::yield ( const float elapsedTime)
{
	assert( myPilot != 0 );
	
	Flock *flock = myPilot->getFlock();
	assert( flock != 0 );

	SteeringDecision retval;
	int neighbours = 0;

	//accumulate positions
	for ( Flock::const_iterator it=flock->begin(); it != flock->end(); ++it)
	{
		// TODO: test angle:
		Vec3f distance = (*it)->getPosition() - myPilot->getPosition();
		if ( neighbourDistance < 0.0f || lengthSquared(distance) <= neighbourDistance*neighbourDistance )
		{
			retval.direction += (*it)->getPosition();
			neighbours++;
		}
	}
	if ( neighbours > 0 )
	{
		// we are only interested in the direction:
		retval.directionUsed = true;
		// compute average position:
		retval.direction /= neighbours;
		// compute direction to that position:
		retval.direction -= myPilot->getPosition();
	}
	return retval;
}

void ufoplugin::CohesionBehaviour::print () const
{
	cout << "BEGIN CohesionBehaviour ( "
		<< "neighbourDistance = " << neighbourDistance << ", neighbourAngle = " << neighbourAngle
		<< " )" << endl;
	cout << "END CohesionBehaviour" <<endl;
}

ufo::Behaviour *ufoplugin::CohesionBehaviourFactory(std::vector<ufo::Behaviour *> children, std::vector<std::pair<std::string,std::string> > *params)
{
	float neighbourDistance = -1.0f;
	float neighbourAngle = gmtl::Math::TWO_PI;
	if ( children.size() != 0 )
	{
		cout << "WARNING: CohesionBehaviour with children!\n" << endl;
	}
	for ( std::vector<std::pair<std::string,std::string> >::const_iterator it = params->begin();
			it != params->end(); ++it)
	{
		if ( it->first == "neighbourDistance" )
		{
			stringstream value;
			value.str(it->second);
			cout << "CohesionBehaviour parameter: neighbourDistance = " << it->second <<endl;
			value >> neighbourDistance;
		}else if ( it->first == "neighbourAngle" )
		{
			stringstream value;
			value.str(it->second);
			cout << "CohesionBehaviour parameter: neighbourAngle = " << it->second <<endl;
			value >> neighbourAngle;
		}else {
			cout << "WARNING: unknown parameter to CohesionBehaviour: " << it->first <<endl;
		}
	}
	return new CohesionBehaviour(neighbourDistance,neighbourAngle);
}

MAKEPLUGIN(BEHAVIOUR, Behaviour, CohesionBehaviour, &ufoplugin::CohesionBehaviourFactory)
