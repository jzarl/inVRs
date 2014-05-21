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

#include "Joints.h"
#include "JointInteraction.h"
#include "Events/JointInteractionEvent.h"
#include <inVRs/SystemCore/DebugOutput.h>
#include <gmtl/AxisAngle.h>
#include <gmtl/Generate.h>

//----------------------------------------------------------------------
// JointFactory
//----------------------------------------------------------------------

/**
 * This method returns the joint with the given name.
 * @param type name of the Joint
 * @return Joint with the passed name if found, NULL if not
 **/
Joint* JointFactory::create(std::string type)
{
	Joint* joint;
	if (!type.compare("hinge") || !type.compare("Hinge") || !type.compare("HINGE"))
		joint = new HingeJoint();
	else if (!type.compare("ball") || !type.compare("Ball") || !type.compare("BALL"))
		joint = new BallJoint();
	else if (!type.compare("slider") || !type.compare("Slider") || !type.compare("SLIDER"))
		joint = new SliderJoint();
	else if (!type.compare("universal") || !type.compare("Universal") || !type.compare("UNIVERSAL"))
		joint = new UniversalJoint();
	else if (!type.compare("hinge2") || !type.compare("Hinge2") || !type.compare("HINGE2"))
		joint = new Hinge2Joint();
	else
	{
		joint = NULL;
		printd(WARNING, "JointFactory::create(): WARNING: Unknown joint-type %s\n", type.c_str());
	} // else
	return joint;
} // create

//----------------------------------------------------------------------
// Joint
//----------------------------------------------------------------------

Joint::Joint()
{
	entityID1 = 0;
	entityID2 = 0;
	joint = NULL;
	mainEntity = NULL;
	alreadyAttached = false;
	active = true;
} // Joint

Joint::~Joint()
{
	if (joint)
		dJointDestroy(joint);

	for (int i=0; i < (int)constraints.size(); i++)
		delete constraints[i];
} // ~Joint

/**
 * This method sets the ODE-worldID. This is needed
 * for the creation of ODE-joints.
 * @param world ODE-worldID
 **/
void Joint::setWorld(dWorldID world)
{
	this->world = world;
} // setWorld

/**
 * This method sets the main EntityTransform, where
 * the Joint should be attached to.
 * @param id 1 for the first EntityTransform,
 *			 2 for the second one
 **/
void Joint::setMainEntity(int id)
{
	if (id == 1)
		mainEntity = jointInteraction->getEntityWithID(entityID1);
	else if (id == 2)
		mainEntity = jointInteraction->getEntityWithID(entityID2);
	else
		printd(ERROR, "Joint::setMainEntity(): ERROR: mainEntity has to be 1 or 2!\n");
} // setMainEntity

void Joint::setJointInteractionClass(JointInteraction* jointInteraction)
{
	this->jointInteraction = jointInteraction;
} // setJointInteractionClass

/**
 * This method sets the IDs of the EntityTransforms
 * which should be connected by the Joint. If one ID is
 * 0 it meens, that the other EntityTransform is connected
 * to the fixed world.
 * @param id1,id2 IDs of the EntityTransforms which should be connected
 **/
void Joint::setEntities(int id1, int id2)
{
	entityID1 = id1;
	entityID2 = id2;
} // setEntities

/**
 * This method returns the IDs of the connected
 * objects.
 * @param id1,id2 IDs of the connected EntityTransforms
 **/
void Joint::getEntities(int& id1, int& id2)
{
	id1 = entityID1;
	id2 = entityID2;
} // getEntities

/**
 * This method is called if the Joint should be detached.
 * It first calls the virtual method detachJoint, so that
 * each type of Joint can store relevant information (e.g axis-angles)
 * before the joint is deleted.
 **/
void Joint::detach()
{
	detachJoint();
	if (joint)
	{
		dJointDestroy(joint);
		joint = NULL;
	} // if
	alreadyAttached = false;
} // detach

/**
 * This method is called if the Joint should be attached.
 * It gets the ODE-bodies from the JointInteraction-class and
 * then either attaches the Joint by calling the virtual
 * method attachJoint or attaches a fixed ODE-joint if
 * the joint is not active.
 **/
void Joint::attach()
{
	if (alreadyAttached)
		return;

	alreadyAttached = true;
	dBodyID body1, body2;
	body1 = jointInteraction->getBodyWithID(entityID1);
	body2 = jointInteraction->getBodyWithID(entityID2);

	if (active)
		attachJoint(body1, body2);
	else
	{
		joint = dJointCreateFixed(world, 0);
		dJointAttach(joint, body1, body2);
		dJointSetFixed(joint);
	} // else

	jointInteraction->attachedJoints.push_back(this);
} // attachJoint

/**
 * This method sets the ID of the Joint
 * @param id the Joint's ID
 **/
void Joint::setId(int id)
{
	this->jointID = id;
} // setId

/**
 * This method returns the ID of the joint.
 * @return the Joint's ID
 **/
int Joint::getId()
{
	return jointID;
} // getId

/**
 * This method is used for activating or deactivating
 * the Joint. If the Joint was inactive and should be
 * activated the fixed ODE-joint is removed and the
 * virtual attachJoint-method is called.
 * If the Joint was active and should be deactivated
 * the virtual detachJoint-method is called and then
 * a fixed ODE-joint is created and attached to the bodies.
 * @param isActive             0 .. not active
 *				   anything else .. active
 **/
void Joint::setActive(int isActive)
{
	dBodyID body1, body2;
	if (!active && isActive && joint)
	{
		dJointDestroy(joint);
		body1 = jointInteraction->getBodyWithID(entityID1);
		body2 = jointInteraction->getBodyWithID(entityID2);
		attachJoint(body1, body2);
	} // if
	else if (active && !isActive && joint)
	{
// first detach joint e.g. for saving axis angles
		detachJoint();
		dJointDestroy(joint);
		body1 = jointInteraction->getBodyWithID(entityID1);
		body2 = jointInteraction->getBodyWithID(entityID2);
		joint = dJointCreateFixed(world, 0);
		dJointAttach(joint, body1, body2);
		dJointSetFixed(joint);
	} // else if

	if (isActive)
		this->active = true;
	else
		this->active = false;
} // setActive

/**
 * This method returns if the Joint is active.
 * @return true, if the Joint is active.
 **/
bool Joint::isActive()
{
	return active;
} // isActive

/**
 * This method adds a Constraint to the constraint-list.
 * @param constraint new Constraint to add
 **/
void Joint::addConstraint(Constraint* constraint)
{
	this->constraints.push_back(constraint);
} // addConstraint

/**
 * This method checks all Constraints in the constraint-list.
 **/
void Joint::checkConstraints()
{
	for (int i=0; i < (int)constraints.size(); i++)
	{
// 		printf("Checking constraint %i\n", i);
		constraints[i]->check();
// 		printf("Check DONE!\n");
	} // for
} // checkConstraint

//----------------------------------------------------------------------
// HingeJoint
//----------------------------------------------------------------------

HingeJoint::HingeJoint()
{
	deltaAngle = 0;
	deltaAngleODE = 0;
	minAngle = 0;
	maxAngle = 0;
	anglesSet = false;
	firstIsMain = false;
} // HingeJoint

/**
 * This method sets the main EntityTransform, where
 * the Joint should be attached to.
 * @param id 1 for the first EntityTransform,
 *			 2 for the second one
 **/
void HingeJoint::setMainEntity(int id)
{
	// calling setMainEntity-method of Joint-class
	this->Joint::setMainEntity(id);
	if (id == 1)
		firstIsMain = true;
	else
		firstIsMain = false;
} // setMainEntity

/**
 * This method sets the Anchor of the Joint in OBJECT-COORDINATES!!!
 * This means that the Anchor is relative to the position of
 * the main EntityTransform. Take care that the Anchor-offset is
 * also scaled by the scale-Value of the EntityTransform. The reason
 * for using object coordinates is because modelers can easily read
 * the offset out of the modelling application.
 * @param anchorX X-position of the Anchor
 * @param anchorY Y-position of the Anchor
 * @param anchorZ Z-position of the Anchor
 **/
void HingeJoint::setAnchor(float anchorX, float anchorY, float anchorZ)
{
	this->anchor[0] = anchorX;
	this->anchor[1] = anchorY;
	this->anchor[2] = anchorZ;
} // setAnchor

/**
 * This method sets the Axis of the Joint in OBJECT-COORDINATES!!!
 * This means that the axis is rotated by the rotation-value
 * of the main EntityTransform.
 * @param axisX X-value of the Axis
 * @param axisY Y-value of the Axis
 * @param axisZ Z-value of the Axis
 **/
void HingeJoint::setAxis(float axisX, float axisY, float axisZ)
{
	this->axis[0] = axisX;
	this->axis[1] = axisY;
	this->axis[2] = axisZ;
} // setAxis

/**
 * This method sets the maximum and minimum angle of
 * the Axis.
 * @param min minimum Angle in rad
 * @param max maximum Angle in rad
 **/
void HingeJoint::setAngles(float min, float max)
{
	this->minAngle = min*gmtl::Math::PI/180.0f;
	this->maxAngle = max*gmtl::Math::PI/180.0f;
	anglesSet = true;
} // setAngles

/**
 * This method is called if the joint should be attached.
 * It creates the ODE-joint, calculates the current anchor-position
 * and axis-orientation and attaches the Joint.
 * @param obj1 first ODE-object to attach with
 * @param obj2 second ODE-object to attach with
 **/
void HingeJoint::attachJoint(dBodyID obj1, dBodyID obj2)
{
	gmtl::Vec3f newAnchor, newAxis, scaleVec;
	gmtl::Quatf entityRot;
	gmtl::AxisAnglef axAng;
	TransformationData entityTrans;

	joint = dJointCreateHinge(world, 0);
	dJointAttach(joint, obj1, obj2);

	newAnchor = anchor;
	newAxis = axis;
	if (mainEntity != NULL)
	{
		entityTrans = mainEntity->getEnvironmentTransformation();
// get the scale values of the mainEntity
		scaleVec = entityTrans.scale;

// scale Anchor-offset by mainEntity-scale value
		newAnchor[0] *= scaleVec[0];
		newAnchor[1] *= scaleVec[1];
		newAnchor[2] *= scaleVec[2];

// scale Axis by mainEntity-scale value because of possible distortion
		newAxis[0] *= scaleVec[0];
		newAxis[1] *= scaleVec[1];
		newAxis[2] *= scaleVec[2];
		gmtl::normalize(newAxis);

// get the Rotation of the mainEntity
		entityRot = entityTrans.orientation;

// rotate Axis by mainEntity-rotation
		newAxis *= entityRot;

// rotate Anchor-offset by mainEntity-rotation
		newAnchor *= entityRot;

// transform new Anchor to world coordinates
		newAnchor += entityTrans.position;

	} // if
	dJointSetHingeAnchor(joint, newAnchor[0], newAnchor[1], newAnchor[2]);
	dJointSetHingeAxis(joint, newAxis[0], newAxis[1], newAxis[2]);

// ODE parameters for minimizing failure in Hinge Joint
// It should not be correct this way but it works better than without it ;-)
	dJointSetHingeParam(joint, dParamCFM, 0);
	dJointSetHingeParam(joint, dParamStopERP, 0);
	dJointSetHingeParam(joint, dParamStopCFM, 0);

	oldAngle = 0;

// set the maximum and minimum Joint-angles (if set).
	if (anglesSet)
		setODEAngles();
} // attachJoint

/**
 * This method checks the angle of the HingeJoint for correctness
 * and corrects it if needed. This is necessary because ODE
 * stores the hinge-angle between -180 degrees and +180 degrees.
 * If the angle gets above +180 deg ODE swaps to -180 deg
 * and vice versa.
 * If a user wants to use a HingeJoint with a maximum angle above
 * 180 deg or a minimum angle below 180 deg the method corrects
 * the ODE-swapping.
 **/
void HingeJoint::checkAngles()
{
	if (active && joint && anglesSet)
	{
		float totalAngle;
		dReal angle = dJointGetHingeAngle(joint);
	// ODE swapped from -180� to +180� or from +180� to -180�
		if ((angle - oldAngle) < -gmtl::Math::PI)
		{
			deltaAngleODE += 2*gmtl::Math::PI;
			setODEAngles();
		} else if ((angle - oldAngle) > gmtl::Math::PI)
		{
			deltaAngleODE -= 2*gmtl::Math::PI;
			setODEAngles();
		} // else if

		totalAngle = angle + deltaAngle + deltaAngleODE;
		if (totalAngle > 2*gmtl::Math::PI || totalAngle < -2*gmtl::Math::PI)
		{
			deltaAngleODE = 0;
			setODEAngles();
		} // if

	// store currentAngle for test in next frame
 		oldAngle = angle;
	} // if
} // checkAngles
/**
 * This method is called if the joint should be detached.
 * If the maximum and minimum angles are set the method
 * stores the angle-offset for correct angles in later use.
 **/
void HingeJoint::detachJoint()
{
	if (active && joint)
	{
		dReal angle = dJointGetHingeAngle(joint);

		if (!anglesSet)
		{
			deltaAngle += angle;
			if (deltaAngle > gmtl::Math::PI)
				deltaAngle -= 2*gmtl::Math::PI;
			else if (deltaAngle < -gmtl::Math::PI)
				deltaAngle +=  2*gmtl::Math::PI;
		} // if
		else
		{
			deltaAngle += angle;
			if (deltaAngle > gmtl::Math::PI)
			{
				deltaAngle -= 2*gmtl::Math::PI;
				deltaAngleODE += 2*gmtl::Math::PI;
			} // if
			else if (deltaAngle < -gmtl::Math::PI)
			{
				deltaAngle += 2*gmtl::Math::PI;
				deltaAngleODE -= 2*gmtl::Math::PI;
			} // else if
		} // else
	} // if
} // detachJoint

/**
 * This method copies the value of the field with the passed name
 * to the result-pointer. It is a little helper to avoid type-casts
 * when accessing joint-pointers.
 * @param type name of the field
 * @param result destination where the value should be stored
 **/
void HingeJoint::getMemberValue(std::string type, void* result)
{
	float angle = 0;
	if (type == "axis1")
	{
		angle = deltaAngle + deltaAngleODE;
		if (joint && active)
			angle += dJointGetHingeAngle(joint);

		if (!anglesSet)
		{
			if (angle > gmtl::Math::PI || angle < -gmtl::Math::PI)
				printd(ERROR, "HingeJoint::GetMemberValue(): ERROR: THIS SHOULD NEVER HAPPEN!!!!!!\n");
			if (angle > gmtl::Math::PI)
				angle -= 2*gmtl::Math::PI;
			else if (angle < -gmtl::Math::PI)
				angle += 2*gmtl::Math::PI;
		} // if
		if (firstIsMain)
			angle *= -1;

		*((float*)result) = angle;
	} // if
	else if (type == "internalChanges")
	{
		JointInteractionEndEvent* evt = (JointInteractionEndEvent*)result;
		evt->addJointID(jointID);
		evt->addFloat(deltaAngle);
		evt->addFloat(deltaAngleODE);
	} // else if
} // getMemberValue

void HingeJoint::setMemberValue(std::string type, void* source)
{
	if (type == "internalChanges")
	{
		JointInteractionEndEvent* evt = (JointInteractionEndEvent*)source;
		deltaAngle = evt->getFloat();
		deltaAngleODE = evt->getFloat();
	} // if
} // setMemberValue

/**
 * This method sets the minimum and maximum angle of the ODE-hinge joint.
 * The method checks if the first or the second EntityTransform is the
 * mainEntity, because then the angles have to be set into different
 * directions. The maxAngle-parameter is set two times, because ODE only
 * accepts it if the maximum angle is above the minimum angle. To avoid
 * checking this we easily set it two times.
 **/
void HingeJoint::setODEAngles()
{
	if (!firstIsMain)
	{
		dJointSetHingeParam(joint, dParamHiStop, maxAngle - deltaAngle - deltaAngleODE);
		dJointSetHingeParam(joint, dParamLoStop, minAngle - deltaAngle - deltaAngleODE);
		dJointSetHingeParam(joint, dParamHiStop, maxAngle - deltaAngle - deltaAngleODE);

// 		printf("\nHingeJoint::setODEAngles(): HiStop = %f LoStop = %f\n",  maxAngle - deltaAngle - deltaAngleODE, minAngle - deltaAngle - deltaAngleODE);
	} else
	{
		dJointSetHingeParam(joint, dParamHiStop, maxAngle + deltaAngle + deltaAngleODE);
		dJointSetHingeParam(joint, dParamLoStop, minAngle + deltaAngle + deltaAngleODE);
		dJointSetHingeParam(joint, dParamHiStop, maxAngle + deltaAngle + deltaAngleODE);

// 		printf("\nHingeJoint::setODEAngles(): HiStop = %f LoStop = %f\n",  maxAngle + deltaAngle + deltaAngleODE, minAngle + deltaAngle + deltaAngleODE);
	} // else
} // setODEAngles

//----------------------------------------------------------------------
// BallJoint
//----------------------------------------------------------------------

BallJoint::BallJoint() {}

/**
 * This method sets the Anchor of the Joint in OBJECT-COORDINATES!!!
 * This means that the Anchor is relative to the position of
 * the main EntityTransform. Take care that the Anchor-offset is
 * also scaled by the scale-Value of the EntityTransform. The reason
 * for using object coordinates is because modelers can easily read
 * the offset out of the modelling application.
 * @param anchorX X-position of the Anchor
 * @param anchorY Y-position of the Anchor
 * @param anchorZ Z-position of the Anchor
 **/
void BallJoint::setAnchor(float anchorX, float anchorY, float anchorZ)
{
	this->anchor[0] = anchorX;
	this->anchor[1] = anchorY;
	this->anchor[2] = anchorZ;
} // setAnchor

/**
 * This method is called if the joint should be attached.
 * It creates the ODE-joint, calculates the current anchor-position
 * and attaches the Joint.
 * @param obj1 first ODE-object to attach with
 * @param obj2 second ODE-object to attach with
 **/
void BallJoint::attachJoint(dBodyID obj1, dBodyID obj2)
{
	gmtl::Vec3f newAnchor;
	gmtl::Quatf entityRot;
	gmtl::AxisAnglef axAng;
	TransformationData entityTrans;

	joint = dJointCreateBall(world, 0);
	dJointAttach(joint, obj1, obj2);

	newAnchor = anchor;
	if (mainEntity != NULL)
	{
// scale Anchor-offset by mainEntity-scale value
		entityTrans = mainEntity->getEnvironmentTransformation();
/*		newAnchor[0] *= mainEntity->getXScale();
		newAnchor[1] *= mainEntity->getYScale();
		newAnchor[2] *= mainEntity->getZScale();*/
		newAnchor[0] *= entityTrans.scale[0];
		newAnchor[1] *= entityTrans.scale[1];
		newAnchor[2] *= entityTrans.scale[2];

// get the Rotation of the mainEntity
// 		axAng[0] = mainEntity->getRotAngle();
// 		axAng[1] = mainEntity->getXRot();
// 		axAng[2] = mainEntity->getYRot();
// 		axAng[3] = mainEntity->getZRot();
// 		gmtl::set(entityRot, axAng);
		entityRot = entityTrans.orientation;

// rotate Anchor-offset by mainEntity-rotation
		newAnchor *= entityRot;

// transform new Anchor to world coordinates
// 		newAnchor[0] += mainEntity->getXTrans();
// 		newAnchor[1] += mainEntity->getYTrans();
// 		newAnchor[2] += mainEntity->getZTrans();
		newAnchor += entityTrans.position;

	}
	dJointSetBallAnchor(joint, newAnchor[0], newAnchor[1], newAnchor[2]);
} // attachJoint

/**
 * This method is called if the joint should be detached.
 **/
void BallJoint::detachJoint() {}

/**
 * This method copies the value of the field with the passed name
 * to the result-pointer. It is a little helper to avoid type-casts
 * when accessing joint-pointers.
 * @param type name of the field
 * @param result destination where the value should be stored
 **/
void BallJoint::getMemberValue(std::string type, void* result) {}

void BallJoint::setMemberValue(std::string type, void* source) {}

//----------------------------------------------------------------------
// SliderJoint
//----------------------------------------------------------------------

SliderJoint::SliderJoint()
{
	posSet = false;
	deltaPos = 0;
}

/**
 * This method sets the Axis of the Joint in OBJECT-COORDINATES!!!
 * This means that the axis is rotated by the rotation-value
 * of the main EntityTransform.
 * @param axisX X-value of the Axis
 * @param axisY Y-value of the Axis
 * @param axisZ Z-value of the Axis
 **/
void SliderJoint::setAxis(float axisX, float axisY, float axisZ)
{
	this->axis[0] = axisX;
	this->axis[1] = axisY;
	this->axis[2] = axisZ;
} // setAxis

/**
 * This method sets the minimum and maximum position
 * of the Joint.
 * @param min minimum position of the Joint
 * @param max maximum position of the Joint
 **/
void SliderJoint::setPositions(float min, float max)
{
	this->minPos = min;
	this->maxPos = max;
	posSet = true;
} // setPositions

/**
 * This method is called if the joint should be attached.
 * It creates the ODE-joint, calculates the current
 * axis-orientation and attaches the Joint.
 * @param obj1 first ODE-object to attach with
 * @param obj2 second ODE-object to attach with
 **/
void SliderJoint::attachJoint(dBodyID obj1, dBodyID obj2)
{
	gmtl::Vec3f newAxis;
	gmtl::Quatf entityRot;
	gmtl::AxisAnglef axAng;
	gmtl::Vec3f scale = gmtl::Vec3f(1,1,1);
	TransformationData entityTrans;

	joint = dJointCreateSlider(world, 0);
	dJointAttach(joint, obj1, obj2);

	newAxis = axis;
	if (mainEntity != NULL)
	{
		entityTrans = mainEntity->getEnvironmentTransformation();
// scale Axis by mainEntity-scale value because of possible distortion
/*		scale[0] = mainEntity->getXScale();
		scale[1] = mainEntity->getYScale();
		scale[2] = mainEntity->getZScale();*/
		scale = entityTrans.scale;

		newAxis[0] *= scale[0];
		newAxis[1] *= scale[1];
		newAxis[2] *= scale[2];
		gmtl::normalize(newAxis);

// get the Rotation of the mainEntity
// 		axAng[0] = mainEntity->getRotAngle();
// 		axAng[1] = mainEntity->getXRot();
// 		axAng[2] = mainEntity->getYRot();
// 		axAng[3] = mainEntity->getZRot();
// 		gmtl::set(entityRot, axAng);
		entityRot = entityTrans.orientation;

// rotate Axis by mainEntity-rotation
		newAxis *= entityRot;
	} // if
	dJointSetSliderAxis(joint, newAxis[0], newAxis[1], newAxis[2]);

// set the minimum and maximum joint-positions (if set).
	if (posSet)
	{
		gmtl::Vec3f minDistVec = newAxis * minPos;
		gmtl::Vec3f maxDistVec = newAxis * maxPos;
		minDistVec[0] *= scale[0];
		minDistVec[1] *= scale[1];
		minDistVec[2] *= scale[2];
		maxDistVec[0] *= scale[0];
		maxDistVec[1] *= scale[1];
		maxDistVec[2] *= scale[2];
		dJointSetSliderParam(joint, dParamLoStop, gmtl::dot(minDistVec,newAxis));
		dJointSetSliderParam(joint, dParamHiStop, gmtl::dot(maxDistVec,newAxis));
	} // if
} // attachJoint

/**
 * This method is called if the joint should be detached.
 * If the maximum and minimum positions are set the method
 * stores the position-offset for correct positions in later use.
 **/
void SliderJoint::detachJoint()
{
	TransformationData entityTrans;
	if (active && joint && posSet)
	{
		dReal position = dJointGetSliderPosition(joint);
		if (mainEntity != NULL)
		{
			entityTrans = mainEntity->getEnvironmentTransformation();
			gmtl::Vec3f newAxis = axis;
			gmtl::Vec3f scale;
/*			scale[0] = mainEntity->getXScale();
			scale[1] = mainEntity->getYScale();
			scale[2] = mainEntity->getZScale();*/
			scale = entityTrans.scale;
			newAxis[0] *= scale[0];
			newAxis[1] *= scale[1];
			newAxis[2] *= scale[2];
			gmtl::normalize(newAxis);
			gmtl::Vec3f newPosVec = newAxis * (float)position;
			newPosVec[0] /= scale[0];
			newPosVec[1] /= scale[1];
			newPosVec[2] /= scale[2];
			position = gmtl::dot(newPosVec, newAxis);
		} // if
		maxPos -= position;
		minPos -= position;
		deltaPos += position;
	} // if
} // detachJoint

/**
 * This method copies the value of the field with the passed name
 * to the result-pointer. It is a little helper to avoid type-casts
 * when accessing joint-pointers.
 * @param type name of the field
 * @param result destination where the value should be stored
 **/
void SliderJoint::getMemberValue(std::string type, void* result)
{
	float pos = 0;
	if (type == "position")
	{
		if (joint && active) pos = dJointGetSliderPosition(joint);
		*((float*)result) = pos+deltaPos;
	} // if
	else if (type == "internalChanges")
	{
		JointInteractionEndEvent* evt = (JointInteractionEndEvent*)result;
		evt->addJointID(jointID);
		evt->addFloat(maxPos);
		evt->addFloat(minPos);
		evt->addFloat(deltaPos);
	} // else if
} // getMemberValue

void SliderJoint::setMemberValue(std::string type, void* source)
{
	if (type == "internalChanges")
	{
		JointInteractionEndEvent* evt = (JointInteractionEndEvent*)source;
		maxPos = evt->getFloat();
		minPos = evt->getFloat();
		deltaPos = evt->getFloat();
	} // if
} // setMemberValue

//----------------------------------------------------------------------
// UniversalJoint
//----------------------------------------------------------------------

UniversalJoint::UniversalJoint() {}

/**
 * This method sets the Anchor of the Joint in OBJECT-COORDINATES!!!
 * This means that the Anchor is relative to the position of
 * the main EntityTransform. Take care that the Anchor-offset is
 * also scaled by the scale-Value of the EntityTransform. The reason
 * for using object coordinates is because modelers can easily read
 * the offset out of the modelling application.
 * @param anchorX X-position of the Anchor
 * @param anchorY Y-position of the Anchor
 * @param anchorZ Z-position of the Anchor
 **/
void UniversalJoint::setAnchor(float anchorX, float anchorY, float anchorZ)
{
	this->anchor[0] = anchorX;
	this->anchor[1] = anchorY;
	this->anchor[2] = anchorZ;
} // setAnchor

/**
 * This method sets the first Axis of the Joint in OBJECT-COORDINATES!!!
 * This means that the axis is rotated by the rotation-value
 * of the main EntityTransform.
 * @param axisX X-value of the Axis
 * @param axisY Y-value of the Axis
 * @param axisZ Z-value of the Axis
 **/
void UniversalJoint::setAxis1(float axisX, float axisY, float axisZ)
{
	this->axis1[0] = axisX;
	this->axis1[1] = axisY;
	this->axis1[2] = axisZ;
} // setAxis1

/**
 * This method sets the second Axis of the Joint in OBJECT-COORDINATES!!!
 * This means that the axis is rotated by the rotation-value
 * of the main EntityTransform.
 * @param axisX X-value of the Axis
 * @param axisY Y-value of the Axis
 * @param axisZ Z-value of the Axis
 **/
void UniversalJoint::setAxis2(float axisX, float axisY, float axisZ)
{
	this->axis2[0] = axisX;
	this->axis2[1] = axisY;
	this->axis2[2] = axisZ;
} // setAxis2

/**
 * This method is called if the joint should be attached.
 * It creates the ODE-joint, calculates the current anchor-position
 * and axis-orientation and attaches the Joint.
 * @param obj1 first ODE-object to attach with
 * @param obj2 second ODE-object to attach with
 **/
void UniversalJoint::attachJoint(dBodyID obj1, dBodyID obj2)
{
	gmtl::Vec3f newAnchor, newAxis1, newAxis2, scaleVec;
	gmtl::Quatf entityRot;
	gmtl::AxisAnglef axAng;
	TransformationData entityTrans;

	joint = dJointCreateUniversal(world, 0);
	dJointAttach(joint, obj1, obj2);

	newAxis1 = axis1;
	newAxis2 = axis2;
	newAnchor = anchor;
	if (mainEntity != NULL)
	{
		entityTrans = mainEntity->getEnvironmentTransformation();
// get the scale values of the mainEntity
// 		scaleVec[0] = mainEntity->getXScale();
// 		scaleVec[1] = mainEntity->getYScale();
// 		scaleVec[2] = mainEntity->getZScale();
		scaleVec = entityTrans.scale;

// scale Anchor-offset by mainEntity-scale value
		newAnchor[0] *= scaleVec[0];
		newAnchor[1] *= scaleVec[1];
		newAnchor[2] *= scaleVec[2];

// scale Axes by mainEntity-scale value because of possible distortion
		newAxis1[0] *= scaleVec[0];
		newAxis1[1] *= scaleVec[1];
		newAxis1[2] *= scaleVec[2];
		gmtl::normalize(newAxis1);
		newAxis2[0] *= scaleVec[0];
		newAxis2[1] *= scaleVec[1];
		newAxis2[2] *= scaleVec[2];
		gmtl::normalize(newAxis2);

// get the Rotation of the mainEntity
// 		axAng[0] = mainEntity->getRotAngle();
// 		axAng[1] = mainEntity->getXRot();
// 		axAng[2] = mainEntity->getYRot();
// 		axAng[3] = mainEntity->getZRot();
// 		gmtl::set(entityRot, axAng);
		entityRot = entityTrans.orientation;

// rotate Axes by mainEntity-rotation
		newAxis1 *= entityRot;
		newAxis2 *= entityRot;

// rotate Anchor-offset by mainEntity-rotation
		newAnchor *= entityRot;

// transform new Anchor to world coordinates
// 		newAnchor[0] += mainEntity->getXTrans();
// 		newAnchor[1] += mainEntity->getYTrans();
// 		newAnchor[2] += mainEntity->getZTrans();
		newAnchor += entityTrans.position;

	} // if
	dJointSetUniversalAnchor(joint, newAnchor[0], newAnchor[1], newAnchor[2]);
	dJointSetUniversalAxis1(joint, newAxis1[0], newAxis1[1], newAxis1[2]);
	dJointSetUniversalAxis2(joint, newAxis2[0], newAxis2[1], newAxis2[2]);
} // attachJoint

/**
 * This method is called if the joint should be detached.
 * It updates the new axis information since the axis can
 * change their orientation in UniversalJoints
 **/
void UniversalJoint::detachJoint() {
	TransformationData entityTrans;
	if (active && joint)
	{
		dVector3 vec;
		dJointGetUniversalAxis1(joint, vec);
		axis1[0] = vec[0];
		axis1[1] = vec[1];
		axis1[2] = vec[2];
		dJointGetUniversalAxis2(joint, vec);
		axis2[0] = vec[0];
		axis2[1] = vec[1];
		axis2[2] = vec[2];
		if (mainEntity != NULL)
		{
			entityTrans = mainEntity->getEnvironmentTransformation();
			gmtl::Quatf entityRot;
			gmtl::Vec3f scaleVec;
			gmtl::AxisAnglef axAng;

	// get the inverse scale values of the mainEntity
/*			scaleVec[0] = 1.0f/mainEntity->getXScale();
			scaleVec[1] = 1.0f/mainEntity->getYScale();
			scaleVec[2] = 1.0f/mainEntity->getZScale();*/
			scaleVec = entityTrans.scale;
	// get the inverse Rotation of the mainEntity
// 			axAng[0] = -mainEntity->getRotAngle();
// 			axAng[1] = mainEntity->getXRot();
// 			axAng[2] = mainEntity->getYRot();
// 			axAng[3] = mainEntity->getZRot();
// 			gmtl::set(entityRot, axAng);
			entityRot = entityTrans.orientation;
			gmtl::invert(entityRot);

			axis1 *= entityRot;
			axis2 *= entityRot;

			axis1[0] *= scaleVec[0];
			axis1[1] *= scaleVec[1];
			axis1[2] *= scaleVec[2];

			axis2[0] *= scaleVec[0];
			axis2[1] *= scaleVec[1];
			axis2[2] *= scaleVec[2];

			gmtl::normalize(axis1);
			gmtl::normalize(axis2);
		} // if
	} // if
} // detachJoint

/**
 * This method copies the value of the field with the passed name
 * to the result-pointer. It is a little helper to avoid type-casts
 * when accessing joint-pointers.
 * @param type name of the field
 * @param result destination where the value should be stored
 **/
void UniversalJoint::getMemberValue(std::string type, void* result)
{
	if (type == "internalChanges")
	{
		JointInteractionEndEvent* evt = (JointInteractionEndEvent*)result;
		evt->addJointID(jointID);
		evt->addFloat(axis1[0]);
		evt->addFloat(axis1[1]);
		evt->addFloat(axis1[2]);
		evt->addFloat(axis2[0]);
		evt->addFloat(axis2[1]);
		evt->addFloat(axis2[2]);
	} // else if
}

void UniversalJoint::setMemberValue(std::string type, void* source)
{
	if (type == "internalChanges")
	{
		JointInteractionEndEvent* evt = (JointInteractionEndEvent*)source;
		axis1[0] = evt->getFloat();
		axis1[1] = evt->getFloat();
		axis1[2] = evt->getFloat();
		axis2[0] = evt->getFloat();
		axis2[1] = evt->getFloat();
		axis2[2] = evt->getFloat();
	} // if
} // setMemberValue

//----------------------------------------------------------------------
// Hinge2Joint
//----------------------------------------------------------------------

Hinge2Joint::Hinge2Joint() {
	usedHelperJoint = false;
	helperBody = 0;
	helperJoint = 0;
} // Hinge2Joint

/**
 * This method sets the Anchor of the Joint in OBJECT-COORDINATES!!!
 * This means that the Anchor is relative to the position of
 * the main EntityTransform. Take care that the Anchor-offset is
 * also scaled by the scale-Value of the EntityTransform. The reason
 * for using object coordinates is because modelers can easily read
 * the offset out of the modelling application.
 * @param anchorX X-position of the Anchor
 * @param anchorY Y-position of the Anchor
 * @param anchorZ Z-position of the Anchor
 **/
void Hinge2Joint::setAnchor(float anchorX, float anchorY, float anchorZ)
{
	this->anchor[0] = anchorX;
	this->anchor[1] = anchorY;
	this->anchor[2] = anchorZ;
} // setAnchor

/**
 * This method sets the first Axis of the Joint in OBJECT-COORDINATES!!!
 * This means that the axis is rotated by the rotation-value
 * of the main EntityTransform.
 * @param axisX X-value of the Axis
 * @param axisY Y-value of the Axis
 * @param axisZ Z-value of the Axis
 **/
void Hinge2Joint::setAxis1(float axisX, float axisY, float axisZ)
{
	this->axis1[0] = axisX;
	this->axis1[1] = axisY;
	this->axis1[2] = axisZ;
} // setAxis1

/**
 * This method sets the second Axis of the Joint in OBJECT-COORDINATES!!!
 * This means that the axis is rotated by the rotation-value
 * of the main EntityTransform.
 * @param axisX X-value of the Axis
 * @param axisY Y-value of the Axis
 * @param axisZ Z-value of the Axis
 **/
void Hinge2Joint::setAxis2(float axisX, float axisY, float axisZ)
{
	this->axis2[0] = axisX;
	this->axis2[1] = axisY;
	this->axis2[2] = axisZ;
} // setAxis2

/**
 * This method is called if the joint should be attached.
 * It creates the ODE-joint, calculates the current anchor-position
 * and axis-orientation and attaches the Joint.
 * @param obj1 first ODE-object to attach with
 * @param obj2 second ODE-object to attach with
 **/
void Hinge2Joint::attachJoint(dBodyID obj1, dBodyID obj2)
{
	TransformationData entityTrans;
	gmtl::Vec3f newAnchor, newAxis1, newAxis2, scaleVec;
	gmtl::Quatf entityRot;
	gmtl::AxisAnglef axAng;

	joint = dJointCreateHinge2(world, 0);
// Attaching the Joint is done after the position and
// orientation calculation!
//	dJointAttach(joint, obj1, obj2);

	newAxis1 = axis1;
	newAxis2 = axis2;
	newAnchor = anchor;
	if (mainEntity != NULL)
	{
		entityTrans = mainEntity->getEnvironmentTransformation();
// get the scale values of the mainEntity
// 		scaleVec[0] = mainEntity->getXScale();
// 		scaleVec[1] = mainEntity->getYScale();
// 		scaleVec[2] = mainEntity->getZScale();
		scaleVec = entityTrans.scale;

// scale Anchor-offset by mainEntity-scale value
		newAnchor[0] *= scaleVec[0];
		newAnchor[1] *= scaleVec[1];
		newAnchor[2] *= scaleVec[2];

// scale Axes by mainEntity-scale value because of possible distortion
		newAxis1[0] *= scaleVec[0];
		newAxis1[1] *= scaleVec[1];
		newAxis1[2] *= scaleVec[2];
		gmtl::normalize(newAxis1);
		newAxis2[0] *= scaleVec[0];
		newAxis2[1] *= scaleVec[1];
		newAxis2[2] *= scaleVec[2];
		gmtl::normalize(newAxis2);

// get the Rotation of the mainEntity
// 		axAng[0] = mainEntity->getRotAngle();
// 		axAng[1] = mainEntity->getXRot();
// 		axAng[2] = mainEntity->getYRot();
// 		axAng[3] = mainEntity->getZRot();
// 		gmtl::set(entityRot, axAng);
		entityRot = entityTrans.orientation;

// rotate Axes by mainEntity-rotation
		newAxis1 *= entityRot;
		newAxis2 *= entityRot;

// rotate Anchor-offset by mainEntity-rotation
		newAnchor *= entityRot;

// transform new Anchor to world coordinates
/*		newAnchor[0] += mainEntity->getXTrans();
		newAnchor[1] += mainEntity->getYTrans();
		newAnchor[2] += mainEntity->getZTrans();*/
		newAnchor += entityTrans.position;
	} // if

// create a helper body when necessary to avoid the
// Segmentation Fault that is thrown by ODE (v0.5)
// when attaching a Hinge2Joint to the static environment
	if (obj1 == 0 || obj2 == 0)
	{
		helperBody = dBodyCreate(world);
		dBodySetPosition(helperBody, newAnchor[0], newAnchor[1], newAnchor[2]);
		helperJoint = dJointCreateFixed(world, 0);
		dJointAttach(helperJoint, helperBody, 0);
		dJointSetFixed(helperJoint);
		if (obj1 == 0)
			obj1 = helperBody;
		else
			obj2 = helperBody;
		usedHelperJoint = true;
	} // if

	dJointAttach(joint, obj1, obj2);

	dJointSetHinge2Anchor(joint, newAnchor[0], newAnchor[1], newAnchor[2]);
	dJointSetHinge2Axis1(joint, newAxis1[0], newAxis1[1], newAxis1[2]);
	dJointSetHinge2Axis2(joint, newAxis2[0], newAxis2[1], newAxis2[2]);
} // attachJoint

/**
 * This method is called if the joint should be detached. It stores
 * the actual Joint-axis, which can change their orientation at
 * Hinge2Joints, and destroys the helper body and joint.
 **/
void Hinge2Joint::detachJoint() {
	if (active && joint)
	{
		TransformationData entityTrans;
		dVector3 vec;
		dJointGetHinge2Axis1(joint, vec);
		axis1[0] = vec[0];
		axis1[1] = vec[1];
		axis1[2] = vec[2];
		dJointGetHinge2Axis2(joint, vec);
		axis2[0] = vec[0];
		axis2[1] = vec[1];
		axis2[2] = vec[2];
		if (mainEntity != NULL)
		{
			entityTrans = mainEntity->getEnvironmentTransformation();
			gmtl::Quatf entityRot;
			gmtl::Vec3f scaleVec;
			gmtl::AxisAnglef axAng;

	// get the inverse scale values of the mainEntity
/*			scaleVec[0] = 1.0f/mainEntity->getXScale();
			scaleVec[1] = 1.0f/mainEntity->getYScale();
			scaleVec[2] = 1.0f/mainEntity->getZScale();*/
			scaleVec[0] = 1.0f/entityTrans.scale[0];
			scaleVec[1] = 1.0f/entityTrans.scale[1];
			scaleVec[2] = 1.0f/entityTrans.scale[2];
	// get the inverse Rotation of the mainEntity
// 			axAng[0] = -mainEntity->getRotAngle();
// 			axAng[1] = mainEntity->getXRot();
// 			axAng[2] = mainEntity->getYRot();
// 			axAng[3] = mainEntity->getZRot();
// 			gmtl::set(entityRot, axAng);
			entityRot = entityTrans.orientation;
			gmtl::invert(entityRot);

			axis1 *= entityRot;
			axis2 *= entityRot;

			axis1[0] *= scaleVec[0];
			axis1[1] *= scaleVec[1];
			axis1[2] *= scaleVec[2];

			axis2[0] *= scaleVec[0];
			axis2[1] *= scaleVec[1];
			axis2[2] *= scaleVec[2];

			gmtl::normalize(axis1);
			gmtl::normalize(axis2);
		} // if
	} // if

	if (usedHelperJoint)
	{
		dJointDestroy(helperJoint);
		dBodyDestroy(helperBody);
		usedHelperJoint = false;
	} // if
} // detachJoint

/**
 * This method copies the value of the field with the passed name
 * to the result-pointer. It is a little helper to avoid type-casts
 * when accessing joint-pointers.
 * @param type name of the field
 * @param result destination where the value should be stored
 **/
void Hinge2Joint::getMemberValue(std::string type, void* result)
{
	if (type == "internalChanges")
	{
		JointInteractionEndEvent* evt = (JointInteractionEndEvent*)result;
		evt->addJointID(jointID);
		evt->addFloat(axis1[0]);
		evt->addFloat(axis1[1]);
		evt->addFloat(axis1[2]);
		evt->addFloat(axis2[0]);
		evt->addFloat(axis2[1]);
		evt->addFloat(axis2[2]);
	} // else if
}

void Hinge2Joint::setMemberValue(std::string type, void* source)
{
	if (type == "internalChanges")
	{
		JointInteractionEndEvent* evt = (JointInteractionEndEvent*)source;
		axis1[0] = evt->getFloat();
		axis1[1] = evt->getFloat();
		axis1[2] = evt->getFloat();
		axis2[0] = evt->getFloat();
		axis2[1] = evt->getFloat();
		axis2[2] = evt->getFloat();
	} // if
} // setMemberValue
