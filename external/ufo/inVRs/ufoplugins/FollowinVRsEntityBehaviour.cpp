#include <iostream>
#include <sstream>
#include <string>
#include <cmath>

#include <gmtl/VecOps.h>
#include <gmtl/Output.h>

#include <inVRs/SystemCore/WorldDatabase/Entity.h>
#include <inVRs/SystemCore/WorldDatabase/WorldDatabase.h>

#include <ufo/Debug.h>
#include <ufo/Pilot.h>
#include <ufo/SteeringDecision.h>
#include <ufo/Plugin_def.h>

#include "FollowinVRsEntityBehaviour.h"

using namespace std;
using namespace gmtl;
using namespace ufo;

	ufoplugin::FollowinVRsEntityBehaviour::FollowinVRsEntityBehaviour( Entity *ent, const bool v)
: theEntity (ent), verbose(v)
{
}

ufoplugin::FollowinVRsEntityBehaviour::~FollowinVRsEntityBehaviour()
{
}

ufo::SteeringDecision ufoplugin::FollowinVRsEntityBehaviour::yield ( const float elapsedTime)
{
	static float oldDistanceSquared = 0.0;
	static float approaching = true;
	assert( myPilot != 0 );
	TransformationData td = theEntity->getWorldTransformation();

	SteeringDecision retval( td.position - myPilot->getPosition());
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

	return retval;
}

void ufoplugin::FollowinVRsEntityBehaviour::print () const
{
	cout << "BEGIN FollowinVRsEntityBehaviour ( verbose = " << boolalpha << verbose << " )" << endl;
	cout << "END FollowinVRsEntityBehaviour" <<endl;
}

ufo::Behaviour *ufoplugin::FollowinVRsEntityBehaviourFactory(std::vector<ufo::Behaviour *> children, std::vector<std::pair<std::string,std::string> > *params)
{
	Entity *ent=0;
	unsigned int environmentBasedId=0, typeInstanceId=0;
	unsigned short environmentId=0, entityId=0;
	unsigned short entityTypeId=0, instanceId=0;
	bool verbose = false;
	if ( children.size() != 0 )
	{
		cout << "WARNING: FollowinVRsEntityBehaviour with children!\n" << endl;
	}
	for ( std::vector<std::pair<std::string,std::string> >::const_iterator it = params->begin();
			it != params->end(); ++it)
	{
		if ( it->first == "environmentBasedId" )
		{
			stringstream value;
			value.str(it->second);
			cout << "InVRsSteerable parameter: environmentBasedId = " << it->second <<endl;
			value >> environmentBasedId;
		}else if ( it->first == "typeInstanceId" )
		{
			stringstream value;
			value.str(it->second);
			cout << "InVRsSteerable parameter: typeInstanceId = " << it->second <<endl;
			value >> typeInstanceId;
		}else if ( it->first == "environmentId" )
		{
			stringstream value;
			value.str(it->second);
			cout << "FollowinVRsEntityBehaviour parameter: environmentId = " << it->second <<endl;
			value >> environmentId;
		}else if ( it->first == "entityId" )
		{
			stringstream value;
			value.str(it->second);
			cout << "FollowinVRsEntityBehaviour parameter: entityId = " << it->second <<endl;
			value >> entityId;
		}else if ( it->first == "entityTypeId" )
		{
			stringstream value;
			value.str(it->second);
			cout << "FollowinVRsEntityBehaviour parameter: entityTypeId = " << it->second <<endl;
			value >> entityTypeId;
		}else if ( it->first == "instanceId" )
		{
			stringstream value;
			value.str(it->second);
			cout << "FollowinVRsEntityBehaviour parameter: instanceId = " << it->second <<endl;
			value >> instanceId;
		}else if ( it->first == "verbose" )
		{
			stringstream value;
			value.str(it->second);
			cout << "FollowinVRsEntityBehaviour parameter: verbose = " << it->second <<endl;
			value >> boolalpha >> verbose;
		}else {
			cout << "WARNING: unknown parameter to FollowinVRsEntityBehaviour: " << it->first <<endl;
		}
	}
	// first try environmentBasedId, then typeInstanceId:
	if ( environmentBasedId )
	{
		ent = WorldDatabase::getEntityWithEnvironmentId( environmentBasedId );
		if ( !ent )
		{
			cout << "WARNING: Couldn't get Entity with environmentBasedId = "
				<< environmentBasedId << "." << endl;
		}
	}
	if ( !ent && typeInstanceId )
	{
		ent = WorldDatabase::getEntityWithTypeInstanceId( typeInstanceId );
		if ( !ent )
		{
			cout << "WARNING: Couldn't get Entity with typeInstanceId = " 
				<< typeInstanceId << "." << endl;
		}
	}
	// at this point we must have either a pairing environmentId/entityId or entityTypeId/instanceId:
	// if both are defined, try environmentId/entityId first:
	if ( !ent && environmentId && entityId )
	{
		ent = WorldDatabase::getEntityWithEnvironmentId( environmentId, entityId );
		if ( !ent )
		{
			cout << "WARNING: Couldn't get Entity with (environmentId,entityId) = ( "
				<< environmentId << " , " << entityId << " )." <<endl;
		}
	} 
	if ( !ent && entityTypeId && instanceId )
	{
		ent = WorldDatabase::getEntityWithTypeInstanceId( entityTypeId, instanceId );
		if ( !ent )
		{
			cout << "WARNING: Couldn't get Entity with (entityTypeId,instanceId) = ( "
				<< entityTypeId << " , " << instanceId << " )." <<endl;
		}
	}
	if ( ent )
	{
		return new FollowinVRsEntityBehaviour(ent, verbose);
	} 
	cout << "ERROR: WorldDatabase doesn't have an entity with environmentBasedId = "
		<< environmentBasedId << ", with typeInstanceId = " 
		<< typeInstanceId << ", (environmentId,entityId) = ( "
		<< environmentId << " , " << entityId << " ), or with (entityTypeId,instanceId) = ( "
		<< entityTypeId << " , " << instanceId << " )!" <<endl;
	WorldDatabase::dump();
	return 0;
}

MAKEPLUGIN(BEHAVIOUR, Behaviour, FollowinVRsEntityBehaviour, &ufoplugin::FollowinVRsEntityBehaviourFactory)
