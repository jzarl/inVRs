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

#ifndef _MODULEIDS_H
#define _MODULEIDS_H

#include "DebugOutput.h"
#include <string>

// external and internal modules/components used by EventManager, TransformationManager
#define USER_DEFINED_ID				0

#define EVENT_MANAGER_ID			1
#define TRANSFORMATION_MANAGER_ID	2

#define USER_DATABASE_ID			3
#define WORLD_DATABASE_ID			4

#define INPUT_INTERFACE_ID			5
#define OUTPUT_INTERFACE_ID			6

#define INTERACTION_MODULE_ID		7
#define NAVIGATION_MODULE_ID		8
#define NETWORK_MODULE_ID			9
#define PHYSICS_MODULE_ID			10

#define JOINT_INTERACTION_MODULE_ID	11

#define SYSTEM_CORE_ID				12

inline unsigned getModuleIdByName(std::string name) {
	if (name == "EventManager")
		return EVENT_MANAGER_ID;
	if (name == "TransformationManager")
		return TRANSFORMATION_MANAGER_ID;
	if (name == "UserDatabase")
		return USER_DATABASE_ID;
	if (name == "WorldDatabase")
		return WORLD_DATABASE_ID;
	if (name == "InputInterface")
		return INPUT_INTERFACE_ID;
	if (name == "OutputInterface")
		return OUTPUT_INTERFACE_ID;
	if (name == "InteractionModule")
		return INTERACTION_MODULE_ID;
	if (name == "NavigationModule")
		return NAVIGATION_MODULE_ID;
	if (name == "NetworkModule")
		return NETWORK_MODULE_ID;
	if (name == "PhysicsModule")
		return PHYSICS_MODULE_ID;
	if (name == "JointInteractionModule")
		return JOINT_INTERACTION_MODULE_ID;
	if (name == "SystemCore")
		return SYSTEM_CORE_ID;

	printd(WARNING, "getModuleIdByName(): Module %s unknown, returning USER_DEFINED_ID!\n",
			name.c_str());
	return USER_DEFINED_ID;
} // getModuleIdByName

inline std::string getModuleNameById(unsigned id) {
	std::string result;
	switch (id) {
		case USER_DEFINED_ID:
			result = "UserDefined";
			break;
		case EVENT_MANAGER_ID:
			result = "EventManager";
			break;
		case TRANSFORMATION_MANAGER_ID:
			result = "TransformationManager";
			break;
		case USER_DATABASE_ID:
			result = "UserDatabase";
			break;
		case WORLD_DATABASE_ID:
			result = "WorldDatabase";
			break;
		case INPUT_INTERFACE_ID:
			result = "InputInterface";
			break;
		case OUTPUT_INTERFACE_ID:
			result = "OutputInterface";
			break;
		case INTERACTION_MODULE_ID:
			result = "InteractionModule";
			break;
		case NAVIGATION_MODULE_ID:
			result = "NavigationModule";
			break;
		case NETWORK_MODULE_ID:
			result = "NetworkModule";
			break;
		case PHYSICS_MODULE_ID:
			result = "PhysicsModule";
			break;
		case JOINT_INTERACTION_MODULE_ID:
			result = "JointInteractionModule";
			break;
		case SYSTEM_CORE_ID:
			result = "SystemCore";
			break;
		default:
			result = "unknown Module";
			break;
	} // switch
	return result;
} // getModuleNameById

#endif
