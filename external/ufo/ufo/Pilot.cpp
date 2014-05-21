#include "Pilot.h"

#include <map>
#include <string>
#include <iostream>

#include <gmtl/Vec.h>
#include <gmtl/Point.h>
#include <gmtl/Quat.h>

#include "UfoDB.h"

#include "SteeringDecision.h"
#include "Behaviour.h"
#include "Steerable.h"
#include "Flock.h"
#include "Debug.h"

using namespace std;
using namespace ufo;

ufo::Pilot::Pilot( std::map<std::string,std::string> att, Behaviour *attitude, Steerable *obj, Flock *theFlock )
: attributes(att), attitude(attitude), vehicle(obj), myFlock(theFlock)
{
	assert(attitude != 0);
	//register with UfoDB and get serial number:
	SN = UfoDB::the()->addPilot(this,myFlock);
	attitude->registerPilot(this);
}
ufo::Pilot::~Pilot()
{
	//PRINT("ufo::Pilot::~Pilot()\n");
	//PRINT("attitude->destroy()\n");
	attitude->destroy();
	//PRINT("vehicle->destroy()\n");
	vehicle->destroy();
	//PRINT("fini\n");
}

void ufo::Pilot::destroy(const bool deregisterFromDB)
{
	if ( deregisterFromDB && myFlock == 0 )
	{
		//PRINT("UfoDB::the()->removePilot(this);\n");
		UfoDB::the()->removePilot(this);
	}
	delete this;
}

const std::string ufo::Pilot::getAttribute(const std::string &key) const
{
	map<string,string>::const_iterator it = attributes.find(key);
	if ( it == attributes.end() )
	{
		return "";
	}
	return (*it).second;
}

void ufo::Pilot::setAttribute(std::string key, std::string value)
{
	if ( value.empty() )
	{
		attributes.erase(key);
	} else {
		attributes[key] = value;
	}
}

void ufo::Pilot::steer( const float elapsedTime )
{
	vehicle->steer(attitude->yield(elapsedTime), elapsedTime);
}

// TODO: make this caching?
gmtl::Point3f ufo::Pilot::getPosition() const
{
	return vehicle->getPosition();
}

// TODO: make this caching?
gmtl::Quatf ufo::Pilot::getOrientation() const 
{
	return vehicle->getOrientation();
}

// TODO: make this caching?
gmtl::Vec3f ufo::Pilot::getVelocity() const
{
	return vehicle->getVelocity();
}

ufo::Flock * ufo::Pilot::getFlock() const
{
	return myFlock;
}

const unsigned int ufo::Pilot::getSN() const
{
	return SN;
}

void ufo::Pilot::print() const
{
	cout << "BEGIN Pilot " << SN <<endl;
	cout << "Steerable:" <<endl;
	vehicle->print();
	cout << "Behaviour:" <<endl;
	attitude->print();
	cout << "END Pilot" <<endl;
}

// Factory function:
ufo::Pilot* ufo::PilotFactory(std::vector<std::pair<std::string,std::string> >* att, Behaviour *b, Steerable *s, Flock *f)
{
	return new Pilot( std::map<std::string,std::string>(att->begin(), att->end()), b, s,f);
}

