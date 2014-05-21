#include "InVRsSteerable.h"

#include <iostream>
#include <sstream>
#include <string>

#include <gmtl/gmtl.h>

#include <inVRs/SystemCore/WorldDatabase/Entity.h>
#include <inVRs/SystemCore/WorldDatabase/WorldDatabase.h>

#include <ufo/SteeringDecision.h>
#include <ufo/Plugin_def.h>

using namespace std;
using namespace gmtl;
using namespace ufo;

	ufoplugin::InVRsSteerable::InVRsSteerable( Entity *ent, const float VMax)
: myEntity(ent), VMax(VMax)
{
}

ufoplugin::InVRsSteerable::~InVRsSteerable()
{
}

void ufoplugin::InVRsSteerable::steer( const ufo::SteeringDecision &decision, const float elapsedTime)
{
	static bool wasFixed=false;
	if ( myEntity->isFixed() && ! wasFixed )
	{
		cout << "WARNING: InVRsSteerable: Entity is fixed!" <<endl;
	}
	// update wasFixed and 
	// don't try to move a fixed Entity:
	wasFixed = myEntity->isFixed();
	if ( wasFixed )
	{
		return;
	}

	TransformationData td = myEntity->getWorldTransformation();
	if ( decision.directionUsed )
	{
		// -> normal Entity or useSimplePhysicsEntity disabled:
		velocity = decision.direction;
		// are we faster than VMax?
		if ( VMax >= 0.0 && lengthSquared(velocity) > VMax*VMax )
		{
			// scale Velocity to VMax:
			normalize(velocity);
			velocity *= VMax;
		}
		td.position += velocity * elapsedTime; 
	}
	if ( decision.rotationUsed )
	{
		// TODO: max Rotation:
		// elapsedTime should normally be somewhere between 0 and 1.
		// we use it as weight, which seems to yield acceptable results
		slerp(td.orientation,elapsedTime,td.orientation, decision.rotation) ;
	}
	// write transformationData:
	myEntity->setWorldTransformation(td);

	// update current position
	position = td.position;
	orientation = td.orientation;
}

gmtl::Point3f ufoplugin::InVRsSteerable::getPosition()
{
	return position;
}

gmtl::Quatf ufoplugin::InVRsSteerable::getOrientation()
{
	return orientation;
}

gmtl::Vec3f ufoplugin::InVRsSteerable::getVelocity()
{
	return velocity;
}

void ufoplugin::InVRsSteerable::print() const
{
	cout << "InVRsSteerable ( typeBasedId = " << myEntity->getTypeBasedId() 
		<< ", environmentBasedId = " << myEntity->getEnvironmentBasedId() << " )" <<endl;
}

#if false
void ufoplugin::InVRsSteerable::updateVelocity(const float elapsedTime)
{
	// position for "current" velocity:
	static Point3f p0 = Point3f(0.0f,0.0f,0.0f);
	// time for "current" velocity:
	static float t0 = 0.0f;
	// index in ringbuffer (moves backwards):
	static int ci = 0;
	// ringbuffer containing "old" velocities:
	static gmtl::Vec3f vn[3] = { gmtl::Vec3f(0.0f,0.0f,0.0f), gmtl::Vec3f(0.0f,0.0f,0.0f), gmtl::Vec3f(0.0f,0.0f,0.0f) };
	// weights for velocities:
	static const float weight[4] = {0.4f,0.3f,0.2f,0.1f};

	gmtl::Vec3f tmp;
	// compute "current" velocity and scale it by its weight:
	t0 += elapsedTime;
	tmp = ( weight[0] * ( position - p0 )) / t0;

	// add weighted "old" velocities:
	for ( int i=0 ; i < 3 ; i++ )
	{
		tmp += weight[1+i] * vn[ (ci + i) % 3 ];
	}

	if ( t0 >= deltaTMin )
	{
		// "finalise current velocity", i.e. put it in the ringbuffer:
		ci = (3 + ci - 1) % 3;
		vn[ci] = ( position - p0 ) / t0 ;
	}
}
#endif

ufo::Steerable *ufoplugin::InVRsSteerableFactory ( std::vector<std::pair<std::string,std::string> > *params)
{
	if ( params->size() == 0 )
	{
		cout << "ERROR: InVRsSteerable without parameters!\n" << endl;
		return 0;
	}
	Entity *ent=0;
	float VMax = -1.0f;
	unsigned int environmentBasedId=0, typeInstanceId=0;
	unsigned short environmentId=0, entityId=0;
	unsigned short entityTypeId=0, instanceId=0;
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
			cout << "InVRsSteerable parameter: environmentId = " << it->second <<endl;
			value >> environmentId;
		}else if ( it->first == "entityId" )
		{
			stringstream value;
			value.str(it->second);
			cout << "InVRsSteerable parameter: entityId = " << it->second <<endl;
			value >> entityId;
		}else if ( it->first == "entityTypeId" )
		{
			stringstream value;
			value.str(it->second);
			cout << "InVRsSteerable parameter: entityTypeId = " << it->second <<endl;
			value >> entityTypeId;
		}else if ( it->first == "instanceId" )
		{
			stringstream value;
			value.str(it->second);
			cout << "InVRsSteerable parameter: instanceId = " << it->second <<endl;
			value >> instanceId;
		}else if ( it->first == "VMax" )
		{
			stringstream value;
			value.str(it->second);
			cout << "InVRsSteerable parameter: VMax = " << it->second <<endl;
			value >> VMax;
		}else {
			cout << "WARNING: unknown parameter to InVRsSteerable: " << it->first <<endl;
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
	if (ent)
	{
		return new InVRsSteerable( ent, VMax );
	}
	cout << "ERROR: WorldDatabase doesn't have an entity with environmentBasedId = "
		<< environmentBasedId << ", with typeInstanceId = " 
		<< typeInstanceId << ", (environmentId,entityId) = ( "
		<< environmentId << " , " << entityId << " ), or with (entityTypeId,instanceId) = ( "
		<< entityTypeId << " , " << instanceId << " )!" <<endl;
	cout << "Worlddatabase::dump():" <<endl;
	WorldDatabase::dump();
	return 0;
}

MAKEPLUGIN(STEERABLE, Steerable, InVRsSteerable, &ufoplugin::InVRsSteerableFactory)
