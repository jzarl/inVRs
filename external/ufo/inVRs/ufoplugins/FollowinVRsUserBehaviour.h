#ifndef UFOPLUGIN_FOLLOWINVRSUSERBEHAVIOUR_H
#define UFOPLUGIN_FOLLOWINVRSUSERBEHAVIOUR_H

#include <gmtl/Point.h>

#include <ufo/Behaviour.h>

class AbstractUserConnectCB;
class AbstractUserDisconnectCB;

class User;
namespace ufo
{
	class SteeringDecision;
}

namespace ufoplugin
{

	/**
	 * A FollowinVRsUserBehaviour following an inVRs User.
	 *
	 * If <code>verbose</code> is set, a message is emitted whenever the
	 * distance changes its tendency. I.e. if we were approaching, and now 
	 * leave, or vice versa.
	 *
	 * If the User is a remote User, then the User is followed until a disconnect.
	 * Whenever a User with the same userId connects, the User is (again) followed.
	 *
	 * If there is no User matching the username, yield() returns the null-vector.
	 *
	 * The username "localUser" is handled specially and always denotes the local User.
	 *
	 * If pilotAttribute is non-empty, then the username is taken from the corresponding 
	 * attribute of the Pilot. The attribute is reevaluated only when a new User connects.
	 *
	 * If the User denoted by pilotAttribute is not connected, or if the Pilot does not 
	 * have the attribute set, then the username parameter is used as a fallback.
	 *
	 * @see FollowinVRsUserBehaviourFactory
	 */
	class FollowinVRsUserBehaviour : public ufo::Behaviour
	{
		public:
			FollowinVRsUserBehaviour(const std::string name, const std::string pilotLink, const bool verbose);

			ufo::SteeringDecision yield ( const float elapsedTime);
			void print () const;

			/**
			 * When the Pilot registers itself, we can use the pilotAttribute.
			 */
			virtual void registerPilot( ufo::Pilot *thePilot);

			/**
			 * Set theUser and register callbacks, for
			 * when the User disconnects.
			 * If the given users getId() doesn't math our uid, no action is taken.
			 *
			 * Note: this function may be called by another thread!
			 */
			void registerUser ( User *u);
			/**
			 * Invalidate theUser and register callbacks, for
			 * when the User reconnects.
			 * If the given users getId() doesn't math our uid, no action is taken.
			 *
			 * Note: this function may be called by another thread!
			 */
			void unregisterUser ( User *u);
		protected:
			virtual ~FollowinVRsUserBehaviour();
		private:
			const std::string username;
			const std::string pilotAttribute;
			const bool verbose;
			volatile User *theUser;
			// used internally, so the unregisterUser function stalls User deletion until yield() returns.
			volatile bool crit_readUser;
			// used internally, so the yield function doesn't access User when its unstable:
			volatile bool crit_writeUser;
			// variables to store callback objects:
			AbstractUserConnectCB *ucCB;
			AbstractUserDisconnectCB *udCB;

			// helper-function to switch theUser threadsafely:
			void setUser(User *u);
			// helper-function to get the correct user from the database (checking both pilotAttribute and username)
			User *getUserFromDB() const;
	};

	/**
	 * Create a FollowinVRsUserBehaviour.
	 * Parameters:
	 *  - username = name of the User which should be followed
	 *    the special value "localUser" always denotes the local User, regardless of
	 *    its name, even if there are other users with the same name.
	 *  - pilotAtttribute:
	 *    get the name of the User from the Pilot attribute with specified key. 
	 */
	ufo::Behaviour *FollowinVRsUserBehaviourFactory(std::vector<ufo::Behaviour *> children, std::vector<std::pair<std::string,std::string> > *params);

}


#endif /* UFOPLUGIN_FOLLOWINVRSUSERBEHAVIOUR_H */
