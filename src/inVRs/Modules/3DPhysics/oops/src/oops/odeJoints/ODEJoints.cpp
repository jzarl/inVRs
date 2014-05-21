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

#include "oops/odeJoints/ODEJoints.h"

#include <ode/ode.h>
#include <ode/odemath.h>
#include <ode/rotation.h>
#include <ode/matrix.h>

enum {
	dJointTypeSpring = 100,
	dJointTypeSingleAxisRotate = 101
};

static void setFixedOrientation(dxJointSpring *joint, dxJoint::Info2 *info, dQuaternion qrel, int start_row)
{
  int s = info->rowskip;
  int start_index = start_row * s;

  dReal angularERP = -1;
  dReal angularCFM = -1;
  
	if (joint->angularSpringConstant >= 0)
	{
		angularERP = info->fps*joint->angularSpringConstant/(info->fps*joint->angularSpringConstant + joint->angularDampingConstant);
		angularCFM = 1/(info->fps*joint->angularSpringConstant + joint->angularDampingConstant);
	} // if

  
  // 3 rows to make body rotations equal
  info->J1a[start_index] = 1;
  info->J1a[start_index + s + 1] = 1;
  info->J1a[start_index + s*2+2] = 1;
  if (joint->node[1].body) {
    info->J2a[start_index] = -1;
    info->J2a[start_index + s+1] = -1;
    info->J2a[start_index + s*2+2] = -1;
  }

  // compute the right hand side. the first three elements will result in
  // relative angular velocity of the two bodies - this is set to bring them
  // back into alignment. the correcting angular velocity is
  //   |angular_velocity| = angle/time = erp*theta / stepsize
  //                      = (erp*fps) * theta
  //    angular_velocity  = |angular_velocity| * u
  //                      = (erp*fps) * theta * u
  // where rotation along unit length axis u by theta brings body 2's frame
  // to qrel with respect to body 1's frame. using a small angle approximation
  // for sin(), this gives
  //    angular_velocity  = (erp*fps) * 2 * v
  // where the quaternion of the relative rotation between the two bodies is
  //    q = [cos(theta/2) sin(theta/2)*u] = [s v]

  // get qerr = relative rotation (rotation error) between two bodies
  dQuaternion qerr,e;
  if (joint->node[1].body) {
    dQuaternion qq;
    dQMultiply1 (qq,joint->node[0].body->q,joint->node[1].body->q);
    dQMultiply2 (qerr,qq,qrel);
  }
  else {
    dQMultiply3 (qerr,joint->node[0].body->q,qrel);
  }
  if (qerr[0] < 0) {
    qerr[1] = -qerr[1];		// adjust sign of qerr to make theta small
    qerr[2] = -qerr[2];
    qerr[3] = -qerr[3];
  }
  dMULTIPLY0_331 (e,joint->node[0].body->posr.R,qerr+1); // @@@ bad SIMD padding!

  dReal k;
  if (angularERP < 0)
	  k = info->fps * info->erp;
  else
	  k = info->fps * angularERP;
  
	if (angularCFM >= 0)
	{
		info->cfm[3] = angularCFM;
		info->cfm[4] = angularCFM;
		info->cfm[5] = angularCFM;
	} // if
  
  info->c[start_row] = 2*k * e[0];
  info->c[start_row+1] = 2*k * e[1];
  info->c[start_row+2] = 2*k * e[2];
}

static inline void initObject (dObject *obj, dxWorld *w)
{
  obj->world = w;
  obj->next = 0;
  obj->tome = 0;
  obj->userdata = 0;
  obj->tag = 0;
}

static inline void addObjectToList (dObject *obj, dObject **first)
{
  obj->next = *first;
  obj->tome = first;
  if (*first) (*first)->tome = &obj->next;
  (*first) = obj;
}

static void dJointInit (dxWorld *w, dxJoint *j)
{
  dIASSERT (w && j);
  initObject (j,w);
  j->vtable = 0;
  j->flags = 0;
  j->node[0].joint = j;
  j->node[0].body = 0;
  j->node[0].next = 0;
  j->node[1].joint = j;
  j->node[1].body = 0;
  j->node[1].next = 0;
  dSetZero (j->lambda,6);
  addObjectToList (j,(dObject **) &w->firstjoint);
  w->nj++;
}

/*******************************************************************************
 * Spring-Joint:
 ******************************************************************************/

static void springInit (dxJointSpring *j)
{
	dSetZero (j->offset,4);
	dSetZero (j->qrel,4);
	j->linearSpringConstant = -1;
	j->linearDampingConstant = -1;
	j->angularSpringConstant = -1;
	j->angularDampingConstant = -1;
} // springInit

static void springGetInfo1 (dxJointSpring *j, dxJoint::Info1 *info)
{
	info->m = 6;
	if (j->linearSpringConstant<0 && j->angularSpringConstant<0)
		info->nub = 6;
	else if (j->linearSpringConstant>=0 && j->angularSpringConstant>=0)
		info->nub = 0;
	else
		info->nub = 3;
} // springGetInfo1

static void springGetInfo2 (dxJointSpring *joint, dxJoint::Info2 *info)
{
	int s = info->rowskip;
	dReal linearERP = -1;
	dReal linearCFM = -1;

	if (joint->linearSpringConstant >= 0)
	{
		linearERP = info->fps*joint->linearSpringConstant/(info->fps*joint->linearSpringConstant + joint->linearDampingConstant);
		linearCFM = 1/(info->fps*joint->linearSpringConstant + joint->linearDampingConstant);
	} // if
	
	// Three rows for orientation
	setFixedOrientation(joint, info, joint->qrel, 3);

	// Three rows for position.
	// set jacobian
	info->J1l[0] = 1;
	info->J1l[s+1] = 1;
	info->J1l[2*s+2] = 1;

	dVector3 ofs;
	dMULTIPLY0_331 (ofs,joint->node[0].body->posr.R,joint->offset);
	if (joint->node[1].body) {
		dCROSSMAT (info->J1a,ofs,s,+,-);
		info->J2l[0] = -1;
		info->J2l[s+1] = -1;
		info->J2l[2*s+2] = -1;
	} // if

	// set right hand side for the first three rows (linear)
	dReal k;
	if (linearERP < 0)
		k = info->fps * info->erp;
	else
		k = info->fps * linearERP;

	if (linearCFM >= 0)
	{
		info->cfm[0] = linearCFM;
		info->cfm[1] = linearCFM;
		info->cfm[2] = linearCFM;
	} // if
	
	if (joint->node[1].body) {
		for (int j=0; j<3; j++)
			info->c[j] = k * (joint->node[1].body->posr.pos[j] -
					joint->node[0].body->posr.pos[j] + ofs[j]);
	} // if
	else {
		for (int j=0; j<3; j++)
			info->c[j] = k * (joint->offset[j] - 
					joint->node[0].body->posr.pos[j]);
	} // else
} // springGetInfo2


void dJointSetSpring (dJointID j)
{
  dxJointSpring* joint = (dxJointSpring*)j;
  dUASSERT(joint,"bad joint argument");
  dUASSERT(joint->vtable == &__dspring_vtable,"joint is not spring");
  int i;

  // This code is taken from sJointSetSliderAxis(), we should really put the
  // common code in its own function.
  // compute the offset between the bodies
  if (joint->node[0].body) {
    if (joint->node[1].body) {
      dQMultiply1 (joint->qrel,joint->node[0].body->q,joint->node[1].body->q);
      dReal ofs[4];
      for (i=0; i<4; i++) ofs[i] = joint->node[0].body->posr.pos[i];
      for (i=0; i<4; i++) ofs[i] -= joint->node[1].body->posr.pos[i];
      dMULTIPLY1_331 (joint->offset,joint->node[0].body->posr.R,ofs);
    }
    else {
      // set joint->qrel to the transpose of the first body's q
      joint->qrel[0] = joint->node[0].body->q[0];
      for (i=1; i<4; i++) joint->qrel[i] = -joint->node[0].body->q[i];
      for (i=0; i<4; i++) joint->offset[i] = joint->node[0].body->posr.pos[i];
    }
  }
}


dxJoint::Vtable __dspring_vtable = {
  sizeof(dxJointSpring),
  (dxJoint::init_fn*) springInit,
  (dxJoint::getInfo1_fn*) springGetInfo1,
  (dxJoint::getInfo2_fn*) springGetInfo2,
  dJointTypeSpring};

dxJoint* dJointCreateSpring (dWorldID w, dJointGroupID group)
{
  dAASSERT (w);
  dxJoint *j;
  if (group) {
    j = (dxJoint*) group->stack.alloc (__dspring_vtable.size);
    group->num++;
  }
  else j = (dxJoint*) dAlloc (__dspring_vtable.size);
  dJointInit (w,j);
  j->vtable = &__dspring_vtable;
  if (group) j->flags |= dJOINT_INGROUP;
  if (__dspring_vtable.init) __dspring_vtable.init(j);
  j->feedback = 0;
  return j;
}

void dJointSetSpringLinearConstants(dJointID j, dReal spring, dReal damping)
{
	dxJointSpring* joint = (dxJointSpring*)j;
	dUASSERT(joint,"bad joint argument");
	dUASSERT(joint->vtable == &__dspring_vtable,"joint is not spring");
	joint->linearSpringConstant = spring;
	joint->linearDampingConstant = damping;
} // dJointSetSpringLinearConstants

void dJointSetSpringAngularConstants(dJointID j, dReal spring, dReal damping)
{
	dxJointSpring* joint = (dxJointSpring*)j;
	dUASSERT(joint,"bad joint argument");
	dUASSERT(joint->vtable == &__dspring_vtable,"joint is not spring");
	joint->angularSpringConstant = spring;
	joint->angularDampingConstant = damping;
} // dJointSetSpringAngularConstants

/*******************************************************************************
 * Single-Axis rotation joint:
 ******************************************************************************/

static void singleAxisRotateInit (dxJointSingleAxisRotate *j)
{
	dSetZero (j->fixedAxis1,4);
	dSetZero (j->fixedAxis2,4);
} // singleAxisRotateInit

static void singleAxisRotateGetInfo1 (dxJointSingleAxisRotate *j, 
		dxJoint::Info1 *info)
{
  info->m = 2;
  info->nub = 2;
}

static void singleAxisRotateGetInfo2 (dxJointSingleAxisRotate *joint, 
		dxJoint::Info2 *info)
{
  if (joint->node[1].body) 
	  printf("SingleAxisRotation-joint does not support second body!\n");
  int s = info->rowskip;

  // set jacobian
  info->J1a[0] = joint->fixedAxis1[0];
  info->J1a[1] = joint->fixedAxis1[1];
  info->J1a[2] = joint->fixedAxis1[2];
  info->J1a[s+0] = joint->fixedAxis2[0];
  info->J1a[s+1] = joint->fixedAxis2[1];
  info->J1a[s+2] = joint->fixedAxis2[2];
}

dxJoint::Vtable __dsingleaxis_vtable = {
  sizeof(dxJointSingleAxisRotate),
  (dxJoint::init_fn*) singleAxisRotateInit,
  (dxJoint::getInfo1_fn*) singleAxisRotateGetInfo1,
  (dxJoint::getInfo2_fn*) singleAxisRotateGetInfo2,
  dJointTypeSingleAxisRotate};

void dJointSetSingleAxisRotateFixedAxis(dJointID j, dVector3 fixedAxis1, 
		dVector3 fixedAxis2) {
	dxJointSingleAxisRotate* joint = (dxJointSingleAxisRotate*)j;
	dUASSERT(joint,"bad joint argument");
	dUASSERT(joint->vtable == &__dsingleaxis_vtable,"joint is not SingleAxisRotate");
	joint->fixedAxis1[0] = fixedAxis1[0];
	joint->fixedAxis1[1] = fixedAxis1[1];
	joint->fixedAxis1[2] = fixedAxis1[2];
	joint->fixedAxis2[0] = fixedAxis2[0];
	joint->fixedAxis2[1] = fixedAxis2[1];
	joint->fixedAxis2[2] = fixedAxis2[2];
} // dJointSetSingleAxisRotateFixedAxis

dxJoint * dJointCreateSingleAxisRotate (dWorldID w, dJointGroupID group)
{
	dAASSERT (w);
	dxJoint *j;
	if (group) {
		j = (dxJoint*) group->stack.alloc (__dsingleaxis_vtable.size);
		group->num++;
	}
	else j = (dxJoint*) dAlloc (__dspring_vtable.size);
	dJointInit (w,j);
	j->vtable = &__dsingleaxis_vtable;
	if (group) j->flags |= dJOINT_INGROUP;
	if (__dsingleaxis_vtable.init) __dsingleaxis_vtable.init(j);
	j->feedback = 0;
	return j;
}
