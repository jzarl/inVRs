#include "Flock.h"
#include "Pilot.h"
#include "UfoDB.h"

using namespace std;
using namespace ufo;

ufo::Flock::Flock(std::map<std::string,std::string> att)
	: attributes(att.begin(), att.end())
{
	//register with the UfoDB:
	SN = UfoDB::the()->addFlock(this);
}
ufo::Flock::~Flock()
{
	// we contain only pointers to Pilots, so
	// the default behaviour of an STL container would be
	// to leave them alone.
	// in our case though, we claim ownership of the Pilots:
	for ( Flock::const_iterator it= this->begin(); it != this->end(); ++it)
	{
		(*it)->destroy();
	}
}
void ufo::Flock::destroy(const bool deregisterFromDB)
{
	if ( deregisterFromDB )
	{
		//deregister with the UfoDB:
		UfoDB::the()->removeFlock(this);
	}
	delete this;
}

void ufo::Flock::update( const float elapsedTime) const
{
	for( Flock::const_iterator it= this->begin(); it != this->end(); ++it)
	{
		(*it)->steer(elapsedTime);
	}
}

const unsigned int ufo::Flock::getSN() const
{
	return SN;
}

const std::string ufo::Flock::getAttribute(const std::string &key) const
{
	map<string,string>::const_iterator it = attributes.find(key);
	if ( it == attributes.end() )
	{
		return "";
	}
	return (*it).second;
}

void ufo::Flock::setAttribute(std::string key, std::string value)
{
	if ( value.empty() )
	{
		attributes.erase(key);
	} else {
		attributes[key] = value;
	}
}

// Factory function:
ufo::Flock* ufo::FlockFactory(std::vector<std::pair<std::string,std::string> >* att)
{
	return new Flock( std::map<std::string,std::string>(att->begin(), att->end()));
}
