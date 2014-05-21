#include <iostream>
#include <sstream>
#include <string>

#include <gmtl/gmtl.h>

#include <ufo/Debug.h>
#include <ufo/Pilot.h>
#include <ufo/SteeringDecision.h>
#include <ufo/Plugin_def.h>

#include "FaceForwardBehaviour.h"

using namespace gmtl;
using namespace ufo;
using namespace std;

	ufoplugin::FaceForwardBehaviour::FaceForwardBehaviour( const bool v, const gmtl::Vec3f gravitationalUp, const gmtl::Vec3f localForward,
			const float rawBW, const float rawUW)
: verbose(v), gravUp(gravitationalUp), localForward(localForward),
	bankingWeight(0.0f),
	oldSpeed(0.0f), oldDirection(Vec3f(0.0f,0.0f,0.0f)), oldPosition(Point3f(0.0f,0.0f,0.0f))
{
	//reference directions must have unit-length:
	normalize(gravUp);
	normalize(this->localForward);

	float rawBankingWeight = rawBW;
	float rawUprightWeight = rawUW;

	// all weights must be > 0.0f!
	if ( rawBankingWeight < 0.0f )
	{
		rawUprightWeight += rawBankingWeight;
		rawBankingWeight = 0.0f;
	}
	if ( rawUprightWeight < 0.0f )
	{
		rawBankingWeight += rawUprightWeight;
		rawUprightWeight = 0.0f;
	}

	//"normalise" weights:
	float totalWeight = rawBankingWeight + rawUprightWeight;
	if ( totalWeight <= 0.00001f )
	{
		// all components are ~0.0f, i.e. equal:
		rawBankingWeight = 1.0f;
		rawUprightWeight = 1.0f;
	} else {
		rawBankingWeight /= totalWeight;
		rawUprightWeight /= totalWeight;
	}
	// all weights are now in interval [0,1]

	// guard against rawBankingWeight=0 && rawUprightWeight=0
	if (rawUprightWeight+rawBankingWeight <= 0.00001f)
		bankingWeight = 0.0f;
	else
		bankingWeight = rawBankingWeight / (rawUprightWeight+rawBankingWeight);
}

ufoplugin::FaceForwardBehaviour::~FaceForwardBehaviour()
{
}

ufo::SteeringDecision ufoplugin::FaceForwardBehaviour::yield ( const float elapsedTime)
{
	assert( myPilot != 0 );

	// update old position and direction:
	updateOld(elapsedTime);

	SteeringDecision retval;
	retval.rotationUsed = true;

	Vec3f curDirection = myPilot->getVelocity();
	//float curSpeed = 
	normalize(curDirection);
	// the "forward"-direction, i.e. where we are pointing to:
	Vec3f forward = localForward;
	// base rotation lets us face in the direction we are moving:
	retval.rotation = makeRot<Quatf>(forward,curDirection);

	////
	// Some words about blending:
	// base rotation (=face forward) is always fully done.
	// the other components are done additionally, but the components 
	// themselfes are blended into another.
	// 
	// Example:
	// all components: 
	//  - faceForward
	//  - banking
	//  - upright
	//  - acceleration
	//
	// We must always face forward unconditionally, the others are "optional".
	// let's say the weights for banking/upright/acceleration are equal.
	//
	// -> retval.rotation = faceForward * otherComponents;
	// with otherComponents as follows:
	// otherComponents = slerp(banking, acceleration, upright);
	////
	
	Quatf otherComponents;

	// "upright Rotation":
	// create a Quaternion that rotates from "up" to gravitational up:
	otherComponents = makeRot<Quatf>(Vec3f(0.0f,1.0f,0.0f), gravUp);
	
	// bankingRotation describes a rotation towards the center of
	// an imaginary circle on which we are moving.
	// The circle, and our current and old positions and directions
	// lie on the same plane (even though in practice, that may not be
	// 100% accurate).
	// the circleUp-direction is a normal vector on the circle-plane
	// N.B.: since cur and old Direction are unit-length, circleUp is also unit-length
	Vec3f circleUp;
	cross(circleUp, curDirection, oldDirection);
	// if we don't move linearly, i.e. cur and old Direction are not colinear("parallel"), then
	// banking applies:
	if ( lengthSquared(circleUp) >= 0.01f )
	{
		//if ( isEqual(curDirection,oldDirection, 0.01f) )
		//	cout << "using colinear vectors!!!" << endl;

		// radiusVec is a vector pointing towards the center of the circle
		// it is perpendicular to both direction and circleUp, which is NOT the same
		// as oldDirection (oldDirection is not perpendicular to curDirection)!
		// N.B.: since curDirection and circleUp are unit-length, radiusVec is also unit-length
		Vec3f radiusVec;
		cross(radiusVec,curDirection,circleUp);

		// to get the radius length, we set up a plane perpendicular to oldDirection,
		// which goes through oldPos. We then intersect it with a ray from our current 
		// position towards the center, thus giving the radius length.
		Planef centerPlane = Planef(oldPosition, oldDirection);
		Rayf radiusRay = Rayf(myPilot->getPosition(), radiusVec);

		float radius;
		intersect(centerPlane, radiusRay, radius);
		if (radius < 0.0f)
		{
			//invert radiusVec, if it points away from the center:
			radius *= -1;
			radiusVec *= -1;
		}

		// the banking rotation converts a circleUp vector towards radiusVec
		// it is stronger when the radius is smaller
		// -> we interpolate between circleUp and radiusVec, and store it in radiusVec
		lerp(radiusVec, 1.0f/(1.0f+radius), circleUp, radiusVec);

		// (pre-lerp) radiusVec and circleUp are perpendicular
		// -> maximum rotation is 90 degrees

		// incorporate bankingRotation:
		slerp(otherComponents,bankingWeight,otherComponents, makeRot<Quatf>(circleUp,radiusVec),true);
	}

	return retval;
}

void ufoplugin::FaceForwardBehaviour::updateOld ( const float elapsedTime )
{
	// updateOld sets the oldDirection and oldPosition  of the Pilot some time (0.1 seconds) ago
	// time is separated into slots. As long as a slot is active
	// resolution is 0.02 seconds, so a bufferSize of 5 is needed:
	static Vec3f speedBuffer[] = { Vec3f(0.0f,0.0f,0.0f), Vec3f(0.0f,0.0f,0.0f), 
		Vec3f(0.0f,0.0f,0.0f), Vec3f(0.0f,0.0f,0.0f), Vec3f(0.0f,0.0f,0.0f) }; 
	static Point3f posBuffer[] = { Point3f(0.0f,0.0f,0.0f), Point3f(0.0f,0.0f,0.0f), 
		Point3f(0.0f,0.0f,0.0f), Point3f(0.0f,0.0f,0.0f), Point3f(0.0f,0.0f,0.0f) }; 
	static float timeBuffer[] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	static int idx = 0;
	
	// first, check if the old time-slot is full and we should overwrite the next one:
	if ( timeBuffer[idx] >= 0.2 )
	{
		idx = (idx + 1) % 5;
		// clear new buffer-cell:
		timeBuffer[idx] = 0.0f;
		speedBuffer[idx] = Vec3f(0.0f,0.0f,0.0f);
		posBuffer[idx] = Point3f(0.0f,0.0f,0.0f);
	}

	// let timeBuffers age:
	for (int i=0; i < 5 ; i++)
		timeBuffer[i] += elapsedTime;
	// a weight between 0 and 1 for the linear interpolation between new and old speed
	// weight=0 -> only old speed counts; weight=1 -> only new speed counts
	// N.B.: timeBuffer[idx] := timeBuffer[idx] + elapsedTime!
	float weight = (elapsedTime < 0.00001f) 
		? 0.0f
		: elapsedTime / timeBuffer[idx];
	// adopt direction to incorporate new value for the timeslot:
	lerp(speedBuffer[idx], weight, speedBuffer[idx], myPilot->getVelocity());
	lerp(posBuffer[idx], weight, posBuffer[idx], myPilot->getPosition());

	// at this point, speedBuffer[idx+1] contains the oldest data (which is at least 
	// (0.1 seconds - timeBuffer[idx]) old).
	// XXX: it would be more accurate to get the youngest data older than 0.1 seconds, and
	// interpolate with the oldest data younger than 0.1 seconds. But who needs accuracy, here?

	oldPosition =  posBuffer[(idx + 1) % 5];
	oldDirection =  speedBuffer[(idx + 1) % 5];
	// oldDirection is stored in normalized form:
	oldSpeed = normalize(oldDirection);
}

void ufoplugin::FaceForwardBehaviour::print () const
{ 
	cout << "BEGIN FaceForwardBehaviour ( verbose = " << boolalpha << verbose << " )" <<endl;
	cout << "END FaceForwardBehaviour" <<endl;
}

ufo::Behaviour *ufoplugin::FaceForwardBehaviourFactory(std::vector<ufo::Behaviour *> children, std::vector<std::pair<std::string,std::string> > *params)
{
	bool verbose = false;
	float bankingWeight = 1.0f, uprightWeight = 1.0f;
	Vec3f gravitationalUp = Vec3f(0.0f,1.0f,0.0f);
	Vec3f localForward = Vec3f(0.0f,0.0f,1.0f);
	if ( children.size() != 0 )
	{
		cout << "WARNING: FaceForwardBehaviour with children!\n" << endl;
	}
	for ( std::vector<std::pair<std::string,std::string> >::const_iterator it = params->begin();
			it != params->end(); ++it)
	{
		if ( it->first == "verbose" )
		{
			stringstream value;
			value.str(it->second);
			cout << "FaceForwardBehaviour parameter: verbose = " << it->second <<endl;
			value >> boolalpha >> verbose;
		} else if ( it->first == "bankingWeight" )
		{
			stringstream value;
			value.str(it->second);
			cout << "FaceForwardBehaviour parameter: bankingWeight = " << it->second <<endl;
			value >> bankingWeight;
		} else if ( it->first == "uprightWeight" )
		{
			stringstream value;
			value.str(it->second);
			cout << "FaceForwardBehaviour parameter: uprightWeight = " << it->second <<endl;
			value >> uprightWeight;
		} else if ( it->first == "localForward" )
		{
			float x,y,z;
			stringstream value;
			value.str(it->second);
			cout << "FaceForwardBehaviour parameter: localForward = " << it->second <<endl;
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
		} else if ( it->first == "gravitationalUp" )
		{
			float x,y,z;
			stringstream value;
			value.str(it->second);
			cout << "FaceForwardBehaviour parameter: gravitationalUp = " << it->second <<endl;
			value >> x >> y >> z;
			if ( value.bad() )
			{
				cout << "WARNING: bad gravitationalUp coordinates. Ignoring gravitationalUp..." << endl;
				cout << "Not accepted coordinates are: " << x << ", " << y << ", " << z <<"."<<endl;
			} else {
				gravitationalUp[0] = x;
				gravitationalUp[1] = y;
				gravitationalUp[2] = z;
			}
		} else {
			cout << "WARNING: unknown parameter to FaceForwardBehaviour: " << it->first <<endl;
		}
	}
	return new FaceForwardBehaviour(verbose,gravitationalUp,localForward,bankingWeight,uprightWeight);
}

MAKEPLUGIN(BEHAVIOUR, Behaviour, FaceForwardBehaviour, &ufoplugin::FaceForwardBehaviourFactory)
