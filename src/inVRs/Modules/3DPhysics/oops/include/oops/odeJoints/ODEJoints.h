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

#ifndef _ODEJOINTS_H_
#define _ODEJOINTS_H_

// ODE-internal header file; in this case needed because we implement our own joint-type:

#if defined ODESOURCEPATH_JOINT_H
#warning "Using ode joint.h set in ODESOURCEPATH_JOINT_H..."
// to adjust this, define ODESOURCEPATH_JOINT_H in your make/config-*.mk file:
#include ODESOURCEPATH_JOINT_H
#else
#include <oops/odeJoints/oops-include-ode-joint.h>
#endif

#ifndef _ODE_JOINT_H_
#	error "Include directive for ode/src/joint.h failed! Please provide a correct include statement!"
#endif

/*******************************************************************************
 * Spring-Joint:
 * This joint is mainly equal to ODE's fixed joint with the difference that it
 * supports CFM and ERP. This allows to create a spring behaviour by the error
 * correction calculations.
 * The possible use case would be to attach this joint to a movable object and
 * a second not movable (maybe realized via infinite mass) one. Setting the
 * position of the not movable one would then force the movable body to follow
 * it like it was connected to it via a spring.
 ******************************************************************************/

struct dxJointSpring : public dxJoint {
	dQuaternion qrel;		// initial relative rotation body1 -> body2
	dVector3 offset;		// relative offset between the bodies
	dReal linearSpringConstant;
	dReal angularSpringConstant;
	dReal linearDampingConstant;
	dReal angularDampingConstant;
};
extern struct dxJoint::Vtable __dspring_vtable;

void dJointSetSpring (dJointID j);
void dJointSetSpringLinearConstants(dJointID j, dReal spring, dReal damping);
void dJointSetSpringAngularConstants(dJointID j, dReal spring, dReal damping);
dxJoint* dJointCreateSpring (dWorldID w, dJointGroupID group);

/*******************************************************************************
 * Single-Axis rotation joint:
 * This joint allows to rotate a rigid body only around one axis, but not around
 * others. This can be useful for characters in games where the character can
 * rotate around the up-axis but not around the horizontal ones.
 ******************************************************************************/

struct dxJointSingleAxisRotate : public dxJoint {
	dVector3 fixedAxis1;
	dVector3 fixedAxis2;
};
extern struct dxJoint::Vtable __dsingleaxis_vtable;

void dJointSetSingleAxisRotateFixedAxis(dJointID j, dVector3 fixedAxis1, 
		dVector3 fixedAxis2);
dxJoint* dJointCreateSingleAxisRotate(dWorldID w, dJointGroupID group);

#endif /*_ODEJOINTS_H_*/
