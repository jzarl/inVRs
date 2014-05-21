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

#ifndef _CONSTRAINT_H
#define _CONSTRAINT_H

#include "JointInteractionSharedLibraryExports.h"

// Forward declaration of class Joint
class Joint;
class JointInteraction;

/**
 * This class is used for activating or deactivating a
 * Joint under certain conditions. The conditions are listed
 * in the enumeration CONDITIONS. The kinds of actions if 
 * a condition is true are listed in the enumeration ACTIONS.
 * The method check checks, if the condition is true and
 * if so the action is accomplished.
 **/
class INVRS_JOINTINTERACTION_API Constraint
{
public:
	enum CONDITIONS {C_NOTSET=0, ISGRABBED, ISACTIVE, ANGLE1LT, ANGLE1GT, ANGLE2LT, ANGLE2GT, POSITIONLT, POSITIONGT};
	enum ACTIONS {A_NOTSET=0, ACTIVATEIF, DEACTIVATEIF, ACTIVEIF, INACTIVEIF};

protected:
	Joint* myJoint;
	JointInteraction* jointInteraction;
	int jointID, entityID;
	CONDITIONS condition;
	ACTIONS actionType;
	float angle, position;
	bool isActive, isGrabbed;

	bool checkActive();
	bool checkGrabbed();
	bool checkAngle(int axis, bool lt);
	bool checkPosition(bool lt);

	void setAction(bool active);
	
public:
	Constraint(JointInteraction* jointInteraction);
	~Constraint();

	void setActionType(ACTIONS action);
	void setOwner(Joint* joint);
	void setJointId(int jointID);
	void setEntityId(int entityID);
	void setCondition(CONDITIONS cond);
	void setAngle(float angle);
	void setPosition(float position);
	void setActive(int isActive);
	void setGrabbed(int isGrabbed);

	void check();
}; // Constraint

#endif
