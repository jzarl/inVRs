#include <iostream>
#include <sstream>
#include <string>

#include <gmtl/gmtl.h>

#include <ufo/Debug.h>
#include <ufo/Pilot.h>
#include <ufo/SteeringDecision.h>
#include <ufo/Plugin_def.h>

#include "RandomBehaviour.h"

using namespace gmtl;
using namespace ufo;
using namespace std;

	ufoplugin::RandomBehaviour::RandomBehaviour( const bool v, const gmtl::Vec3f maxUp, const gmtl::Vec3f maxSide, const gmtl::Vec3f localForward)
: verbose(v), maxUp(maxUp), maxSide(maxSide), localForward(localForward)
{
}

ufoplugin::RandomBehaviour::~RandomBehaviour()
{
}

ufo::SteeringDecision ufoplugin::RandomBehaviour::yield ( const float elapsedTime)
{
	assert( myPilot != 0 );

	SteeringDecision retval;
	// we are only interested in the direction:
	retval.directionUsed = true;

	float weightV,weightH;
	float signV,signH;

	weightV = gmtl::Math::rangeRandom(-1.0f,1.0f);
	signV = gmtl::Math::sign(weightV);
	// the weight should be positive:
	weightV *= signV;

	weightH = gmtl::Math::rangeRandom(-1.0f,1.0f);
	signH = gmtl::Math::sign(weightH);
	// the weight should be positive:
	weightH *= signH;

	Vec3f H;
	lerp(H,weightH,localForward,maxSide);
	Vec3f V;
	lerp(V,weightV,localForward,maxUp);

	retval.direction = H + V;
	retval.direction *= myPilot->getOrientation();

	return retval;
}

void ufoplugin::RandomBehaviour::print () const
{ 
	cout << "BEGIN RandomBehaviour ( verbose = " << boolalpha << verbose 
		<< " , maxUp = " << maxUp 
		<< " , maxSide = " << maxSide 
		<< " , localForward = " << localForward 
		<< " )" <<endl;
	cout << "END RandomBehaviour" <<endl;
}

ufo::Behaviour *ufoplugin::RandomBehaviourFactory(std::vector<ufo::Behaviour *> children, std::vector<std::pair<std::string,std::string> > *params)
{
	bool verbose = false;
	Vec3f maxUp = Vec3f(0.0f,1.0f,0.0f);
	Vec3f maxSide = Vec3f(1.0f,0.0f,0.0f);
	Vec3f localForward = Vec3f(0.0f,0.0f,1.0f);
	if ( children.size() != 0 )
	{
		cout << "WARNING: RandomBehaviour with children!\n" << endl;
	}
	for ( std::vector<std::pair<std::string,std::string> >::const_iterator it = params->begin();
			it != params->end(); ++it)
	{
		if ( it->first == "maxUp" )
		{
			float x,y,z;
			stringstream value;
			value.str(it->second);
			cout << "RandomBehaviour parameter: maxUp = " << it->second <<endl;
			value >> x >> y >> z;
			if ( value.bad() )
			{
				cout << "WARNING: bad maxUp coordinates. Ignoring maxUp..." << endl;
				cout << "Not accepted coordinates are: " << x << ", " << y << ", " << z <<"."<<endl;
			} else {
				maxUp[0] = x;
				maxUp[1] = y;
				maxUp[2] = z;
			}
		} else if ( it->first == "maxSide" )
		{
			float x,y,z;
			stringstream value;
			value.str(it->second);
			cout << "RandomBehaviour parameter: maxSide = " << it->second <<endl;
			value >> x >> y >> z;
			if ( value.bad() )
			{
				cout << "WARNING: bad maxSide coordinates. Ignoring maxSide..." << endl;
				cout << "Not accepted coordinates are: " << x << ", " << y << ", " << z <<"."<<endl;
			} else {
				maxSide[0] = x;
				maxSide[1] = y;
				maxSide[2] = z;
			}
		} else if ( it->first == "localForward" )
		{
			float x,y,z;
			stringstream value;
			value.str(it->second);
			cout << "RandomBehaviour parameter: localForward = " << it->second <<endl;
			value >> x >> y >> z;
			if ( value.bad() )
			{
				cout << "WARNING: bad localForward coordinates. Ignoring localForward..." << endl;
				cout << "Not accepted coordinates are: " << x << ", " << y << ", " << z <<"."<<endl;
			} else {
				localForward[0] = x;
				localForward[1] = y;
				localForward[2] = z;
			}
		} else if ( it->first == "verbose" )
		{
			stringstream value;
			value.str(it->second);
			cout << "RandomBehaviour parameter: verbose = " << it->second <<endl;
			value >> boolalpha >> verbose;
		} else {
			cout << "WARNING: unknown parameter to RandomBehaviour: " << it->first <<endl;
		}
	}
	return new RandomBehaviour(verbose,maxUp,maxSide,localForward);
}

MAKEPLUGIN(BEHAVIOUR, Behaviour, RandomBehaviour, &ufoplugin::RandomBehaviourFactory)
