#include "InVRsPhysicsSteerable.h"

#include <iostream>
#include <sstream>
#include <string>

#include <gmtl/gmtl.h>

#include <inVRs/SystemCore/WorldDatabase/Entity.h>
#include <inVRs/SystemCore/WorldDatabase/WorldDatabase.h>
#include <inVRs/Modules/3DPhysics/SimplePhysicsEntity.h>
#ifdef INVRS_BUILD_TIME
#  include "oops/RigidBody.h"
#else
#  include <inVRs/Modules/3DPhysics/oops/RigidBody.h>
#endif

#include <ufo/SteeringDecision.h>
#include <ufo/Plugin_def.h>

using namespace std;
using namespace gmtl;
using namespace ufo;

ufoplugin::InVRsPhysicsSteerable::InVRsPhysicsSteerable( SimplePhysicsEntity *ent,  const float VMax, 
		const float dMin, const bool deactivateGravity )
: myEntity(ent), VMax(VMax), deltaMinSquared(dMin*dMin), 
	deactivateGravity(deactivateGravity)
{
}

ufoplugin::InVRsPhysicsSteerable::~InVRsPhysicsSteerable()
{
}

void ufoplugin::InVRsPhysicsSteerable::steer( const ufo::SteeringDecision &decision, const float elapsedTime)
{
	static bool wasFixed=false;
	if ( myEntity->isFixed() && ! wasFixed )
	{
		cout << "WARNING: InVRsPhysicsSteerable: Entity is fixed!" <<endl;
	}
	// update wasFixed and 
	// don't try to move a fixed Entity:
	wasFixed = myEntity->isFixed();
	if ( wasFixed )
	{
		return;
	}

	// we can just add a force, physics will do the rest:
	// !!! TODO: add angular force to request a certain orientation!
	// get the RigidBody:
	oops::RigidBody *body = myEntity->getRigidBody();

	// read velocity _before_ setting force, so we apply correct force!
	// read position from transformationData:
	position = myEntity->getWorldTransformation().position;
	orientation = myEntity->getWorldTransformation().orientation;
	// read back the velocity as reported by the RigidBody:
	body->getLinearVelocity(velocity );

	// if deactivateGravity is set, then gravity only works as long as
	// we don't want to go anywhere
	if ( deactivateGravity )
		body->setGravityMode(!decision.directionUsed);

	if ( decision.directionUsed )
	{
		gmtl::Vec3f force; 
		// limit desired speed if we are faster than VMax and don't want to decelerate
		if ( VMax >= 0.0 && lengthSquared(velocity) > VMax*VMax )
		{
			gmtl::Vec3f v = velocity;
			gmtl::Vec3f d = decision.direction;
			// how to check if we are accelerating:
			// 1) normalise current speed(v) and desired speed(d)
			// 2) add the normalised vectors
			// 3) if the result lies inside the unit-sphere, we decelerate
			//    if the result lies outside the unit-sphere, we accelerate
			normalize(v);
			normalize(d);
			v += d;
			if ( lengthSquared(v) > 1.0f )
			{ // we want to accelerate beyond VMax
				// -> adopt force, so we just aim for VMax
				d *= VMax;
				force = d - velocity;
			} else {
				// we are already decelerating (or keeping our speed) 
				// -> do as when V < VMax
				force = decision.direction - velocity;
			}
		} else {
			// the force is the difference between current speed and desired speed:
			force = decision.direction - velocity;
		}
		// help keep the physics simulation quiet:
		if ( lengthSquared(force) >= deltaMinSquared )
		{
			// scale force with the body-mass:
			// TODO: maximum force, ...
			force *= body->getMass();
			// add time-factor: 
			// ATTENTION: evil hack: Physics simulation runs at ~60FPS, resetting the force at each simulation step:
			force *= elapsedTime * 60;
			// API: addForce( force, isRelative):
			//body->addForce( force, false );
			myEntity->addForce(force,false);
		}
	}
}


gmtl::Point3f ufoplugin::InVRsPhysicsSteerable::getPosition()
{
	return position;
}

gmtl::Quatf ufoplugin::InVRsPhysicsSteerable::getOrientation()
{
	return orientation;
}

gmtl::Vec3f ufoplugin::InVRsPhysicsSteerable::getVelocity()
{
	return velocity;
}

void ufoplugin::InVRsPhysicsSteerable::print() const
{
	cout << "InVRsPhysicsSteerable ( typeBasedId = " << myEntity->getTypeBasedId() 
		<< ", environmentBasedId = " << myEntity->getEnvironmentBasedId() << " )" <<endl;
}

ufo::Steerable *ufoplugin::InVRsPhysicsSteerableFactory ( std::vector<std::pair<std::string,std::string> > *params)
{
	if ( params->size() == 0 )
	{
		cout << "ERROR: InVRsPhysicsSteerable without parameters!\n" << endl;
		return 0;
	}
	Entity *ent=0;
	float VMax = -1.0f;
	unsigned int environmentBasedId=0, typeInstanceId=0;
	unsigned short environmentId=0, entityId=0;
	unsigned short entityTypeId=0, instanceId=0;
	float deltaMin = 0.0f;
	bool deactivateGravity=false;
	for ( std::vector<std::pair<std::string,std::string> >::const_iterator it = params->begin();
			it != params->end(); ++it)
	{
		if ( it->first == "environmentBasedId" )
		{
			stringstream value;
			value.str(it->second);
			cout << "InVRsPhysicsSteerable parameter: environmentBasedId = " << it->second <<endl;
			value >> environmentBasedId;
		}else if ( it->first == "typeInstanceId" )
		{
			stringstream value;
			value.str(it->second);
			cout << "InVRsPhysicsSteerable parameter: typeInstanceId = " << it->second <<endl;
			value >> typeInstanceId;
		}else if ( it->first == "environmentId" )
		{
			stringstream value;
			value.str(it->second);
			cout << "InVRsPhysicsSteerable parameter: environmentId = " << it->second <<endl;
			value >> environmentId;
		}else if ( it->first == "entityId" )
		{
			stringstream value;
			value.str(it->second);
			cout << "InVRsPhysicsSteerable parameter: entityId = " << it->second <<endl;
			value >> entityId;
		}else if ( it->first == "entityTypeId" )
		{
			stringstream value;
			value.str(it->second);
			cout << "InVRsPhysicsSteerable parameter: entityTypeId = " << it->second <<endl;
			value >> entityTypeId;
		}else if ( it->first == "instanceId" )
		{
			stringstream value;
			value.str(it->second);
			cout << "InVRsPhysicsSteerable parameter: instanceId = " << it->second <<endl;
			value >> instanceId;
		}else if ( it->first == "VMax" )
		{
			stringstream value;
			value.str(it->second);
			cout << "InVRsPhysicsSteerable parameter: VMax = " << it->second <<endl;
			value >> VMax;
		}else if ( it->first == "deltaMin" )
		{
			stringstream value;
			value.str(it->second);
			cout << "InVRsPhysicsSteerable parameter: deltaMin = " << it->second <<endl;
			value >> deltaMin;
		}else if ( it->first == "deactivateGravity" )
		{
			stringstream value;
			value.str(it->second);
			cout << "InVRsPhysicsSteerable parameter: deactivateGravity = " << it->second <<endl;
			value >> boolalpha >> deactivateGravity;
		}else {
			cout << "WARNING: unknown parameter to InVRsPhysicsSteerable: " << it->first <<endl;
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
	SimplePhysicsEntity *pent = dynamic_cast<SimplePhysicsEntity*>(ent);
	if (!pent)
	{
		if (ent) // if ent exists, the problem was the cast:
			cout << "WARNING: Casting the entity as SimplePhysicsEntity failed!" << endl;
		cout << "ERROR: WorldDatabase doesn't have an entity with environmentBasedId = "
			<< environmentBasedId << ", with typeInstanceId = " 
			<< typeInstanceId << ", (environmentId,entityId) = ( "
			<< environmentId << " , " << entityId << " ), or with (entityTypeId,instanceId) = ( "
			<< entityTypeId << " , " << instanceId << " )!" <<endl;
		cout << "Worlddatabase::dump():" <<endl;
		WorldDatabase::dump();
		return 0;
	}
	return new InVRsPhysicsSteerable( pent, VMax, deltaMin, deactivateGravity);
}

MAKEPLUGIN(STEERABLE, Steerable, InVRsPhysicsSteerable, &ufoplugin::InVRsPhysicsSteerableFactory)
