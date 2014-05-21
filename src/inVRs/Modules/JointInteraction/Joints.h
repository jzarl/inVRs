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

#ifndef _JOINTS_H
#define _JOINTS_H
#include <string>
#include <ode/ode.h>
#include <gmtl/VecOps.h>
#include <gmtl/QuatOps.h>
#include <inVRs/SystemCore/WorldDatabase/Entity.h>
#include "Constraint.h"

#include "JointInteractionSharedLibraryExports.h"

class JointInteraction;

/**
 * This class is the super-class of all Joints. It contains
 * all general methods and fields. Every new Joint simply has
 * to implement the methods attachJoint, detachJoint and
 * getMemberValue.
 **/
class INVRS_JOINTINTERACTION_API Joint
{
protected:
	int jointID;
	int entityID1, entityID2;
	bool alreadyAttached;
	bool active;
	dWorldID world;
	dJointID joint;
	Entity* mainEntity;
	JointInteraction* jointInteraction;
	std::vector<Constraint*> constraints;

	virtual void attachJoint(dBodyID obj1, dBodyID obj2) = 0;
	virtual void detachJoint() = 0;

public:
	Joint();
	Joint(dWorldID world);
	virtual ~Joint();
	virtual void setMainEntity(int id);
	virtual void setEntities(int id1, int id2);
	virtual void getEntities(int& id1, int& id2);
	virtual void setWorld(dWorldID world);
	virtual void setJointInteractionClass(JointInteraction* jointInteraction);
	virtual void getMemberValue(std::string type, void* result) = 0;
	virtual void setMemberValue(std::string type, void* source) = 0;
	virtual void debugOutput() {};
	void attach();
	void detach();
	void setId(int id);
	int getId();
	void setActive(int isActive);
	bool isActive();
	void addConstraint(Constraint* constraint);
	void checkConstraints();
};

/**
 * This class represents an hinge joint which can be
 * defined by an Anchor of the Axis and the Axis itself.
 * There is also the option to set the maximum and minimum
 * angle of the joint.
 **/
class INVRS_JOINTINTERACTION_API HingeJoint : public Joint
{
protected:
	gmtl::Vec3f anchor;
	gmtl::Vec3f axis;
	float minAngle, maxAngle, deltaAngle, deltaAngleODE;
	float oldAngle;
	bool anglesSet, firstIsMain;

	virtual void attachJoint(dBodyID obj1, dBodyID obj2);
	virtual void detachJoint();
	void setODEAngles();

public:
	HingeJoint();
	virtual ~HingeJoint(){};

	void setMainEntity(int id);
	void setAnchor(float anchorX, float anchorY, float anchorZ);
	void setAxis(float axisX, float axisY, float axisZ);
	void setAngles(float min, float max);
	void checkAngles();
	virtual void getMemberValue(std::string type, void* result);
	virtual void setMemberValue(std::string type, void* source);

}; // HingeJoint

/**
 * This class represents a ball joint, which is simply
 * fixed connection in one point where the rotation around
 * this point is not limited. It can be defined by an
 * Anchor-point.
 **/
class INVRS_JOINTINTERACTION_API BallJoint : public Joint
{
protected:
	gmtl::Vec3f anchor;

	virtual void attachJoint(dBodyID obj1, dBodyID obj2);
	virtual void detachJoint();

public:
	BallJoint();
	virtual ~BallJoint(){};

	void setAnchor(float anchorX, float anchorY, float anchorZ);
	virtual void getMemberValue(std::string type, void* result);
	virtual void setMemberValue(std::string type, void* source);

};

/**
 * This class represents a slider joint, which is like 
 * the piston of a hydraulic cylinder. It can be defined
 * by setting the axis of movement. There is also the option
 * to set the maximum and minimum position of the joint.
 **/
class INVRS_JOINTINTERACTION_API SliderJoint : public Joint
{
protected:
	gmtl::Vec3f axis;
	float minPos, maxPos, deltaPos;
	bool posSet;

	virtual void attachJoint(dBodyID obj1, dBodyID obj2);
	virtual void detachJoint();

public:
	SliderJoint();
	virtual ~SliderJoint(){};

	void setAxis(float axisX, float axisY, float axisZ);
	void setPositions(float min, float max);
	virtual void getMemberValue(std::string type, void* result);
	virtual void setMemberValue(std::string type, void* source);
};

/**
 * This joint is built out of 2 hinge joints, which are
 * perpendicular to each other. This joint can be defined
 * by setting the Anchor point and the 2 Axes.
 **/
class INVRS_JOINTINTERACTION_API UniversalJoint : public Joint
{
protected:
	gmtl::Vec3f anchor;
	gmtl::Vec3f axis1, axis2;
/***
 * It seems that ODE's (v0.5) getter-functions for UniversalJoint-Angles 
 * are not stable yet (since they are not mentioned in the Documentation!)
 ***/
//	float minAngleAxis1, maxAngleAxis1;
//	float minAngleAxis2, maxAngleAxis2;
//	bool axis1AnglesSet, axis2AnglesSet;

	virtual void attachJoint(dBodyID obj1, dBodyID obj2);
	virtual void detachJoint();

public:
	UniversalJoint();
	virtual ~UniversalJoint(){};

	void setAnchor(float anchorX, float anchorY, float anchorZ);
	void setAxis1(float axisX, float axisY, float axisZ);
	void setAxis2(float axisX, float axisY, float axisZ);
//	void setAxis1Angles(float min, float max);
//	void setAxis2Angles(float min, float max);
	virtual void getMemberValue(std::string type, void* result);
	virtual void setMemberValue(std::string type, void* source);
}; // UniversalJoint

/**
 * This class represents a joint which is built out of
 * 2 HingeJoints (which don't have to be perpendicular).
 * It can be defined by setting the Anchor and the two
 * hinge-Axes.
 * The members helperBody and helperJoint are needed to
 * avoid problems with ODE (v0.5), when a Hinge2-Joint is 
 * connected to the static environment. Since ODE always
 * throws a Segmentation Fault in this case i added a
 * helperBody which is put between the Hinge2Joint and
 * the fixed environment.
 **/
class INVRS_JOINTINTERACTION_API Hinge2Joint : public Joint
{
protected:
	gmtl::Vec3f anchor;
	gmtl::Vec3f axis1, axis2;
	bool usedHelperJoint;
	dBodyID helperBody;
	dJointID helperJoint;
/***
 * Angle-functions for Hinge2 are not supported since ODE 0.5 doesn't
 * provide a function for getting the Angle of Axis 2
 ***/
//	float minAngleAxis1, maxAngleAxis1;
//	float minAngleAxis2, maxAngleAxis2;
//	bool axis1AnglesSet, axis2AnglesSet;

	virtual void attachJoint(dBodyID obj1, dBodyID obj2);
	virtual void detachJoint();

public:
	Hinge2Joint();
	virtual ~Hinge2Joint(){};

	void setAnchor(float anchorX, float anchorY, float anchorZ);
	void setAxis1(float axisX, float axisY, float axisZ);
	void setAxis2(float axisX, float axisY, float axisZ);
//	void setAxis1Angles(float min, float max);
//	void setAxis2Angles(float min, float max);
	virtual void getMemberValue(std::string type, void* result);
	virtual void setMemberValue(std::string type, void* source);
}; // Hinge2Joint

/**
 * This class is a little helper for the XML-Parser (class Interaction)
 * to build Joints out of their class-names.
 **/
class INVRS_JOINTINTERACTION_API JointFactory
{
public:
	static Joint* create(std::string type);
}; // JointFactory

#endif // _JOINTS_H
