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

#include "Constraint.h"
#include "Joints.h"
#include "JointInteraction.h"
#include <inVRs/SystemCore/DebugOutput.h>

Constraint::Constraint(JointInteraction* jointInteraction)
{
	this->jointInteraction = jointInteraction;
	myJoint = NULL;
	jointID = 0;
	entityID = 0;
	condition = C_NOTSET;
	actionType = A_NOTSET;
	angle = 0;
	position = 0;
	isActive = false;
	isGrabbed = false;
} // Constraint

Constraint::~Constraint() {}

/***
 * The following methods all check conditions and return
 * true or false according to the result.
 **/

/**
 * This method checks, if another Joint is active or not
 * (according to the settings in the configuration-file)
 **/
bool Constraint::checkActive()
{
	Joint* otherJoint = jointInteraction->getJointWithID(jointID);
	if (!otherJoint)
	{
		printd(WARNING, "Constraint::checkActive(): WARNING: JOINT WITH ID %i NOT FOUND (JointID = %i)!!!\n", jointID, myJoint->getId());
		return false;
	} // if
	if (isActive)
		return otherJoint->isActive();
	return !otherJoint->isActive();
} // checkActive

/**
 * This method checks, if an EntityTransform is grabbed or
 * not (according to the settings in the configuration-file)
 **/
bool Constraint::checkGrabbed()
{
	if (entityID <= 0)
		printd(WARNING, "Constraint::checkGrabbed(): WARNING: CHECKING ENTITYTRANSFORM WITH ID %i\n", entityID);

	if (jointInteraction->isGrabbing && jointInteraction->grabbedObject->entity->getEnvironmentBasedId() == (unsigned)entityID)
		return isGrabbed;

	return !isGrabbed;
} // checkGrabbed

/**
 * This method checks, if the angle of a Joint's axis
 * is below or above a certain value. The value to compare
 * with has to be set in the configuration-file.
 * @param axis Axis-number (e.g. 1 for Axis1, 2 for Axis2)
 * @param lt if true, check for angle less than value, else
 *				check for angle greater than value
 **/
bool Constraint::checkAngle(int axis, bool lt)
{
	float otherAngle;
	char axisString[6];
	sprintf(axisString, "axis%i", axis);

	Joint* otherJoint = jointInteraction->getJointWithID(jointID);

	if (!otherJoint)
	{
		printd(WARNING, "Constraint::checkActive(): WARNING: JOINT WITH ID %i NOT FOUND (JointID = %i)!!!\n", jointID, myJoint->getId());
		return false;
	} // if

	otherJoint->getMemberValue(axisString, &otherAngle);
	if (lt)
		return (otherAngle <= angle);
	else
		return (otherAngle >= angle);
} // checkAngle

/**
 * This method checks, if the position of a SliderJoint
 * is below or above a certain value. This value has to
 * be set in the configuration-file.
 * @param lt if true, check for position less than value,
 *			 else check for position greater than value
 **/
bool Constraint::checkPosition(bool lt)
{
	float otherPosition;

	Joint* otherJoint = jointInteraction->getJointWithID(jointID);

	if (!otherJoint)
	{
		printd(WARNING, "Constraint::checkActive(): WARNING: JOINT WITH ID %i NOT FOUND (JointID = %i)!!!\n", jointID, myJoint->getId());
		return false;
	} // if

	otherJoint->getMemberValue("position", &otherPosition);
	if (lt)
		return (otherPosition <= position);
	else
		return (otherPosition >= position);
} // checkPosition

/**
 * This method checks, if the condition of the constraint
 * is true or false and passes the result to the setAction-
 * method.
 **/
void Constraint::check()
{
	bool result = false;

	if ((actionType == ACTIVATEIF && myJoint->isActive()) || (actionType == DEACTIVATEIF && !myJoint->isActive()))
		return;

	switch(condition)
	{
		case C_NOTSET:
		{
			printd(WARNING, "Constraint::check(): WARNING: CONDITION NOT SET (JointID = %i)!!!\n", myJoint->getId());
			break;
		} // case
		case ISACTIVE:
		{
			result = checkActive();
			break;
		} // case
		case ISGRABBED:
		{
			result = checkGrabbed();
			break;
		} // case
		case ANGLE1LT:
		{
			result = checkAngle(1, true);
			break;
		} // case
		case ANGLE1GT:
		{
			result = checkAngle(1, false);
			break;
		} // case
		case ANGLE2LT:
		{
			result = checkAngle(2, true);
			break;
		} // case
		case ANGLE2GT:
		{
			result = checkAngle(2, false);
			break;
		} // case
		case POSITIONLT:
		{
			result = checkPosition(true);
			break;
		} // case
		case POSITIONGT:
		{
			result = checkPosition(false);
			break;
		} // case
		default:
		{
			printd(WARNING, "Constraint::check(): WARNING: CONDITION NOT SET (JointID = %i)!!!\n", myJoint->getId());
			break;
		} // default
	} // switch
	setAction(result);
} // check

/**
 * This method activates or deactivates the Joint according to
 * the parameter and the action-type.
 * @param action says, if the condition is true or false
 **/
void Constraint::setAction(bool action)
{
	if (actionType == A_NOTSET)
		printd(WARNING, "Constraint::setAction(): WARNING: ACTION NO SET (JointID = %i)!!!\n", myJoint->getId());
	if (actionType == ACTIVATEIF && action)
		myJoint->setActive(1);
	else if (actionType == DEACTIVATEIF && action)
		myJoint->setActive(0);
	else if (actionType == ACTIVEIF)
	{
		if (action && !myJoint->isActive())
			myJoint->setActive(1);
		else if (!action && myJoint->isActive())
			myJoint->setActive(0);
	} // else if
	else if (actionType == INACTIVEIF)
	{
		if (action && myJoint->isActive())
			myJoint->setActive(0);
		else if (!action && !myJoint->isActive())
			myJoint->setActive(1);
	} // else if
} // setAction

/***
 * The following methods are simple setter-functions.
 **/

void Constraint::setActionType(ACTIONS action)
{
	this->actionType = action;
} // setAction

void Constraint::setOwner(Joint* joint)
{
	this->myJoint = joint;
} // setOwner

void Constraint::setJointId(int jointID)
{
	this->jointID = jointID;
} // setJointId

void Constraint::setEntityId(int entityID)
{
	this->entityID = entityID;
} // setEntityId

void Constraint::setCondition(CONDITIONS cond)
{
	this->condition = cond;
} // setCondition

void Constraint::setAngle(float angle)
{
	this->angle = angle*gmtl::Math::PI/180.0f;;
} // setAngle

void Constraint::setPosition(float position)
{
	this->position = position;
} // setPosition

void Constraint::setActive(int isActive)
{
	if (isActive)
		this->isActive = true;
	else
		this->isActive = false;
} // setActive

void Constraint::setGrabbed(int isGrabbed)
{
	if (isGrabbed)
		this->isGrabbed = true;
	else
		this->isGrabbed = false;
} // setGrabbed
