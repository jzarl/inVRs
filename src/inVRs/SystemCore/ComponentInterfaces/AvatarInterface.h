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

#ifndef _AVATARINTERFACE_H
#define _AVATARINTERFACE_H

#include <irrXML.h>

#include "../ClassFactory.h"
#include "../DataTypes.h"
#include "../../OutputInterface/SceneGraphInterface.h"

using namespace irr;
using namespace io;

class INVRS_SYSTEMCORE_API User;

// not derived from ModuleInterface
/******************************************************************************
 * AvatarInterface provides the basic functionality of an avatar.
 * An avatar is normally owned by a User.
 */
class INVRS_SYSTEMCORE_API AvatarInterface {
public:
	AvatarInterface() {
		owner = NULL;
		avatarName = "-unknown-";
	}

	/**
	 *
	 */
	virtual ~AvatarInterface() {};

	/**
	 *
	 */
	virtual void setName(std::string name) {
		this->avatarName = name;
	} // setName

	/**
	 *
	 */
	virtual std::string getName() {
		return avatarName;
	} // setName

	/**
	 *
	 */
	virtual bool loadConfig(std::string configFile) = 0;

	/**
	 *
	 */
	virtual ModelInterface* getModel() = 0;

	/**
	 *
	 */
	virtual void showAvatar(bool active) = 0;

	/**
	 *
	 */
	virtual bool isVisible() = 0;

	/**
	 *
	 */
	virtual void setTransformation(TransformationData trans) = 0;

	/**
	 * This function can be used for updating the animation-phase of the Avatar.
	 */
	virtual void update(float dt) {};

protected:
	friend class User;

	/**
	 *
	 */
	virtual void setOwner(User* user) {
		this->owner = user;
	} // setOwner

	/**
	 *
	 */
	virtual User* getOwner() {
		return owner;
	} // getOwner

	User* owner;
	std::string avatarName;
};

typedef ClassFactory<AvatarInterface, std::string> AvatarFactory;
#endif
