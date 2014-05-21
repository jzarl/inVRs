/*---------------------------------------------------------------------------*\
 *           interactive networked Virtual Reality system (inVRs)            *
 *                                                                           *
 *    Copyright (C) 2005-2009 by the Johannes Kepler University, Linz        *
 *                                                                           *
 *                            www.inVRs.org                                  *
 *                                                                           *
 *              contact: canthes@inVRs.org, rlander@inVRs.org                *
 \*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *                                License                                    *
 *                                                                           *
 * This library is free software; you can redistribute it and/or modify it   *
 * under the terms of the GNU Library General Public License as published    *
 * by the Free Software Foundation, version 2.                               *
 *                                                                           *
 * This library is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Library General Public License for more details.                          *
 *                                                                           *
 * You should have received a copy of the GNU Library General Public         *
 * License along with this library; if not, write to the Free Software       *
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 *
 \*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *                                Changes                                    *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 \*---------------------------------------------------------------------------*/

#ifndef _USERDATABASE_H
#define _USERDATABASE_H

#include <assert.h>

#include "User.h"
#include "../ComponentInterfaces/CursorRepresentationInterface.h"

class UserDatabaseEventsFactory;

class INVRS_SYSTEMCORE_API AbstractUserConnectCB {
public:
	virtual ~AbstractUserConnectCB() {
	}
	;

	virtual AbstractUserConnectCB* clone() {
		assert(false);
		return NULL;
	}
	;

	virtual bool operator==(AbstractUserConnectCB& src) {
		assert(false);
		return false;
	}
	;

	virtual void call(User* user) {
		assert(false);
	}
	;

protected:
	AbstractUserConnectCB() {
	}
	;
};

class INVRS_SYSTEMCORE_API AbstractUserDisconnectCB {
public:
	virtual ~AbstractUserDisconnectCB() {
	}
	;

	virtual AbstractUserDisconnectCB* clone() {
		assert(false);
		return NULL;
	}
	;

	virtual bool operator==(AbstractUserDisconnectCB& src) {
		assert(false);
		return false;
	}
	;

	virtual void call(User* user) {
		assert(false);
	}
	;

protected:
	AbstractUserDisconnectCB() {
	}
	;
};

template<class TClass> class UserConnectCB : public AbstractUserConnectCB {
public:
	UserConnectCB(TClass* object, void(TClass::*function)(User*)) {
		this->object = object;
		this->function = function;
	}
	;

	UserConnectCB(UserConnectCB<TClass>& src) {
		this->object = src.object;
		this->function = src.function;
	}

	virtual ~UserConnectCB() {
	}
	;

	virtual AbstractUserConnectCB* clone() {
		return new UserConnectCB<TClass> (this->object, this->function);
	} // clone

	virtual bool operator==(AbstractUserConnectCB& src) {
		UserConnectCB* cb = dynamic_cast<UserConnectCB*> ((&src));
		assert(cb);
		return (cb->object == this->object && cb->function == this->function);
	}
	;

	virtual void call(User* user) {
		(*object.*function)(user);
	}
	;

private:
	TClass* object;
	void (TClass::*function)(User*);
}; // UserConnectCB

template<class TClass> class UserDisconnectCB : public AbstractUserDisconnectCB {
public:
	UserDisconnectCB(TClass* object, void(TClass::*function)(User*)) {
		this->object = object;
		this->function = function;
	}
	;

	UserDisconnectCB(UserDisconnectCB<TClass>& src) {
		this->object = src.object;
		this->function = src.function;
		class Tutorial30;
	}
	;

	~UserDisconnectCB() {
	}
	;

	virtual AbstractUserDisconnectCB* clone() {
		return new UserDisconnectCB<TClass> (this->object, this->function);
	} // clone

	bool operator==(AbstractUserDisconnectCB& src) {
		UserDisconnectCB* cb = dynamic_cast<UserDisconnectCB*> ((&src));
		assert(cb);
		return (cb->object == this->object && cb->function == this->function);
	}
	;

	void call(User* user) {
		(*object.*function)(user);
	}
	;

private:
	TClass* object;
	void (TClass::*function)(User*);
}; // UserDisconnectCB

/******************************************************************************
 * @class UserDatabase
 * @brief The UserDatabase stores the local User and possible remote Users.
 *
 * The UserDatabase is one of the two databases of the SystemCore thus it is
 * considered a core component. This class is used to store all Users which are
 * registered via the template <class TClass> in the world. It provides methods
 * to receive User objects and to add or remove Users from the world. Callbacks
 * can be registered which are triggered once a User is added or removed from
 * the UserDatabase.
 */
class INVRS_SYSTEMCORE_API UserDatabase {
public:
	/**
	 * The method registers the default CursorModelFactory (which is the
	 * VirtualHandCursorModel) in the User-class and creates a new User-object for
	 * the local user.
	 */
	static void init();

	/**
	 * Delete the local User object and call User::cleanup(). Call this method
	 * before WorldDatabase::cleanup().
	 * @see SystemCore::cleanup(), WorldDatabase::cleanup()
	 */
	static void cleanup();

	/**
	 * The method adds a new User to the UserDatabase. It therefore checks if the
	 * User is already connected and adds the User to the remoteUserList if not.
	 * @param remoteUser pointer to the new User object
	 */
	static void addRemoteUser(User* remoteUser);

	/**
	 * Delete the local User object and call User::cleanup(). Call this method
	 * before WorldDatabase::cleanup().
	 * @see SystemCore::cleanup(), WorldDatabase::cleanup()
	 */
	static void removeRemoteUser(unsigned userId);

	/**
	 * This method is used to register a callback which is triggered once a User
	 * is added to the UserDatabase
	 * @param callback the callback which is to be triggered
	 */
	static void registerUserConnectCallback(AbstractUserConnectCB& callback);

	/**
	 * This method is used to unregister a callback which is triggered once a User
	 * is added to the UserDatabase.
	 * @param callback the callback which is to be unregistered
	 */
	static void unregisterUserConnectCallback(AbstractUserConnectCB& callback);

	/**
	 * This method is used to register a callback which is triggered once a User
	 * is removed from the UserDatabase.
	 * @param callback the callback which is to be registered
	 */
	static void registerUserDisconnectCallback(AbstractUserDisconnectCB& callback);

	/**
	 * This method is used to unregister a callback which is triggered once a User
	 * is removed from the UserDatabase.
	 * @param callback the callback which is to be unregistered
	 */
	static void unregisterUserDisconnectCallback(AbstractUserDisconnectCB& callback);

	/**
	 * Registers an CursorRepresentationFactory in the UserDatabase.
	 * All registered CursorRepresentationFactory will be delete'd automatically when
	 * <code>cleanup()</code> is invoked
	 * @author rlander
	 * @param factory CursorRepresentationFactory that should be added
	 */
	static void registerCursorRepresentationFactory(CursorRepresentationFactory* factory);

	/**
	 * Updates the cursors of all users
	 */
	static void updateCursors(float dt);

	/**
	 * The method removes a User from the UserDatabase. It therefore gets the User
	 * by it's Id, removes it from the remoteUserList and deletes the User-object
	 * @param userId Id of the User to remove
	 */
	static unsigned getLocalUserId();

	/**
	 * The method returns a pointer to the local User object.
	 * @return local User object
	 */
	static User* getLocalUser();

	/**
	 * The method returns the number of users currently registered in the system.
	 * @return number of connected users
	 */
	static int getNumberOfRemoteUsers();

	/**
	 * The method returns the User with the passed Id if it is registered in the
	 * UserDatabase.
	 * @param userId the Id of the searched User
	 * @return pointer to the User with the matching Id, NULL if no User is found
	 */
	static User* getUserById(unsigned userId);

	/**
	 * The method returns the connected User which is on the passed index in the
	 * remoteUserList.
	 * @param idx the index in the remoteUserList
	 * @return user at the passed index of the remoteUserList, NULL if the index
	 * is out of bounds
	 */
	static User* getRemoteUserByIndex(int idx);

private:
	//TODO: avoid friend class, maybe by parsing the userDatabase-configuration in this class!
	friend class User;

	static CursorRepresentationInterface* loadCursorRepresentation(std::string configFile);

	static std::vector<User*> remoteUserList;
	static User* localUser;
	static std::vector<CursorRepresentationFactory*> cursorRepresentationFactories;
	static std::vector<AbstractUserConnectCB*> userConnectCallbacks;
	static std::vector<AbstractUserDisconnectCB*> userDisconnectCallbacks;

//*****************************************************************************
// Configuration loading
//*****************************************************************************
private:
	static XmlConfigurationLoader cursorXmlConfigLoader;

};

#endif // _USERDATABASE_H
