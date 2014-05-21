#include <iostream>
#include <sstream>
#include <string>
#include <cmath>

#include <gmtl/VecOps.h>
#include <gmtl/Output.h>

#include <inVRs/SystemCore/UserDatabase/UserDatabase.h>
#include <inVRs/SystemCore/UserDatabase/User.h>

#include <ufo/Debug.h>
#include <ufo/Pilot.h>
#include <ufo/SteeringDecision.h>
#include <ufo/Plugin_def.h>

#include "FollowinVRsUserBehaviour.h"

using namespace std;
using namespace gmtl;
using namespace ufo;

	ufoplugin::FollowinVRsUserBehaviour::FollowinVRsUserBehaviour(const std::string name, const std::string pilotLink, const bool v)
: username(name), pilotAttribute(pilotLink), verbose(v), theUser(0), crit_readUser(false),crit_writeUser(false),
	ucCB( new UserConnectCB<FollowinVRsUserBehaviour>(this,&ufoplugin::FollowinVRsUserBehaviour::registerUser) ) ,
	udCB( new UserDisconnectCB<FollowinVRsUserBehaviour>(this,&ufoplugin::FollowinVRsUserBehaviour::unregisterUser) )
{
	// N.B.:we do not have a Pilot during initialisation (only after registerPilot is called)
	// because the pilotLink may be used, and theUser is not needed before the Pilot is set,
	// we only set the User when the pilot is registered.

	// register callbacks for when a user (dis)connects:
	UserDatabase::registerUserConnectCallback( *ucCB );
	UserDatabase::registerUserDisconnectCallback( *udCB );
}

ufoplugin::FollowinVRsUserBehaviour::~FollowinVRsUserBehaviour()
{
	// deregister callbacks for when a user (dis)connects:
	UserDatabase::unregisterUserConnectCallback( *ucCB );
	UserDatabase::unregisterUserDisconnectCallback( *udCB );
	delete ucCB;
	delete udCB;
}

void ufoplugin::FollowinVRsUserBehaviour::registerPilot(Pilot *p)
{
	//call super:
	Behaviour::registerPilot(p);

	theUser = getUserFromDB();
	if ( verbose )
	{
		if (theUser)
		{
			User *u = const_cast<User *>(theUser);
			cout << "FollowinVRsUserBehaviour: Connected to user: " << u->getName() << "(id: "<<u->getId() <<")"<<endl;
		} else {
			cout << "FollowinVRsUserBehaviour: No user available (yet)!" <<endl;
		}
	}
}

ufo::SteeringDecision ufoplugin::FollowinVRsUserBehaviour::yield ( const float elapsedTime)
{
	static float oldDistanceSquared = 0.0;
	static float approaching = true;
	// for user-change message only:
	static User* oldUser = 0;
	assert( myPilot != 0 );

	// threadsafety guard:
	crit_readUser = true;
	if ( !crit_writeUser && theUser )
	{
		if ( verbose && oldUser != theUser )
		{
			User *u = const_cast<User *>(theUser);
			oldUser = u;
			cout << "FollowinVRsUserBehaviour: Connected to user: " << u->getName() << "(id: "<<u->getId() <<")"<<endl;
		}

		TransformationData td = const_cast<User *>(theUser)->getWorldUserTransformation();
		crit_readUser = false; // ! beyond this point no interaction with User!

		// fixme: can we just subtract Quaternions?
		ufo::SteeringDecision retval ( td.position - myPilot->getPosition());
		if ( verbose )
		{
			float distanceSquared = lengthSquared(retval.direction);
			// we were previously getting nearer?
			if ( approaching )
			{
				// print data about nearest approach:
				if ( oldDistanceSquared <= 0.05f + distanceSquared )
				{
					cout << "FollowinVRsUserBehaviour: Nearest approach just before " << myPilot->getPosition() <<endl;
					cout << "FollowinVRsUserBehaviour: Distance of nearest approach: " << sqrt(oldDistanceSquared) <<endl;
					approaching = false;
				}
			} else {
				// print data about largest distance:
				if ( oldDistanceSquared +0.05f >= distanceSquared )
				{
					cout << "FollowinVRsUserBehaviour: Longest distance reached just before " << myPilot->getPosition() <<endl;
					cout << "FollowinVRsUserBehaviour: Longest distance was: " << sqrt(oldDistanceSquared) <<endl;
					approaching = true;
				}
			}
			oldDistanceSquared = distanceSquared;
		}

		return retval;
	} else {
		crit_readUser = false; // remove threadsafety guard
		// no User with getId() == uid available.
		return SteeringDecision();
	}
}

void ufoplugin::FollowinVRsUserBehaviour::print () const
{
	cout << "BEGIN FollowinVRsUserBehaviour ( verbose = " << boolalpha << verbose << " )" << endl;
	cout << "END FollowinVRsUserBehaviour" <<endl;
}

// this method can be called from another thread!
void ufoplugin::FollowinVRsUserBehaviour::setUser (User *u)
{
	// the following code works under these assumptions:
	// - The User is not deleted until after all callbacks have returned
	// - the compiler doesn't reorder reads and/or writes to volatile variables
	//
	crit_writeUser = true;
	// wait for yield to leave its critical section:
	while ( crit_readUser )
		; // busy wait - FIXME
	theUser = u;
	// leave our critical section:
	crit_writeUser = true;
}

User * ufoplugin::FollowinVRsUserBehaviour::getUserFromDB () const
{
	// pilot && pilotAttribute | username || which to use:
	// set                     | set      || pilotAttribute
	// set                     | unset    || pilotAttribute
	// unset                   | set      || username
	// unset                   | unset    || 0 (wait for pilot)
	
	string tmpname = username;
	if ( myPilot && ! pilotAttribute.empty() )
	{
		tmpname = myPilot->getAttribute(pilotAttribute);
	}
	if ( tmpname.empty() )
	{
		// can't get a User for empty name:
		return 0;
	}

	// local or remote User?
	if ( tmpname == "localUser" || tmpname == UserDatabase::getLocalUser()->getName() )
	{
		// -> localUser:
		return UserDatabase::getLocalUser();
	} else {
		// -> remote User
		for ( int i=0; i<UserDatabase::getNumberOfRemoteUsers(); i++)
		{ //search remoteUsers
			User *u = UserDatabase::getRemoteUserByIndex(i);
			if ( u->getName() == tmpname )
			{
				return u;
			}
		}
		// no user found:
		return 0;
	}
}

void ufoplugin::FollowinVRsUserBehaviour::registerUser (User * u)
{
	if ( u == getUserFromDB() ) // "our" User?
	{
		if ( verbose )
		{
			cout << "FollowinVRsUserBehaviour: (re)registered User " << username 
				<< " with uid " << u->getId() <<endl;
		}
		setUser(u);
	}
}

void ufoplugin::FollowinVRsUserBehaviour::unregisterUser (User * u)
{
	if ( u && u == theUser ) // "our" User?
	{
		setUser(0);
		if ( verbose )
		{
			cout << "FollowinVRsUserBehaviour: lost User " << username 
				<< " with uid " << u->getId() <<endl;
		}
	}
}

ufo::Behaviour *ufoplugin::FollowinVRsUserBehaviourFactory(std::vector<ufo::Behaviour *> children, std::vector<std::pair<std::string,std::string> > *params)
{
	string username;
	string pilotAttribute;
	bool verbose = false;
	if ( children.size() != 0 )
	{
		cout << "WARNING: FollowinVRsUserBehaviour with children!\n" << endl;
	}
	for ( std::vector<std::pair<std::string,std::string> >::const_iterator it = params->begin();
			it != params->end(); ++it)
	{
		if ( it->first == "username" )
		{
			cout << "InVRsSteerable parameter: username = " << it->second <<endl;
			username = it->second;
		} else if ( it->first == "pilotAttribute" )
		{
			cout << "InVRsSteerable parameter: pilotAttribute = " << it->second <<endl;
			pilotAttribute = it->second;
		}else if ( it->first == "verbose" )
		{
			stringstream value;
			value.str(it->second);
			cout << "FollowinVRsUserBehaviour parameter: verbose = " << it->second <<endl;
			value >> boolalpha >> verbose;
		}else {
			cout << "WARNING: unknown parameter to FollowinVRsUserBehaviour: " << it->first <<endl;
		}
	}
	if ( username.empty() && pilotAttribute.empty() )
	{
		cout << "ERROR: FollowinVRsUserBehaviour: parameter 'username' or 'pilotAttribute'  must be set!" <<endl;
		return 0;
	}
	return new FollowinVRsUserBehaviour(username, pilotAttribute, verbose);
}

MAKEPLUGIN(BEHAVIOUR, Behaviour, FollowinVRsUserBehaviour, &ufoplugin::FollowinVRsUserBehaviourFactory)
