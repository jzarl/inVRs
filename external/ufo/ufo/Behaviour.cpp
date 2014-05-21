#include "Behaviour.h"
#include "Pilot.h"
#include "Debug.h"

using namespace ufo;

ufo::Behaviour::~Behaviour()
{
}
void ufo::Behaviour::destroy()
{
	delete this;
}

void ufo::Behaviour::registerPilot( Pilot *p )
{
	assert(p != 0);
	myPilot = p;
}
