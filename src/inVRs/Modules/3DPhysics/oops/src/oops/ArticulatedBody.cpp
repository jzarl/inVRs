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

#include "oops/ArticulatedBody.h"
#include <assert.h>
#include "oops/RigidBody.h"
#include "oops/Joints.h"

namespace oops
{

/******************************************************************************/
// CLASS: ArticulatedBody

ArticulatedBody::ArticulatedBody()
{
	offsetTransformation = identityTransformation();
	bodySpace = 0;
	mainBody = NULL;
	className = "ArticulatedBody";
} // ArticulatedBody

ArticulatedBody::~ArticulatedBody()
{
} // ~ArticulatedBody

void ArticulatedBody::cloneData(ArticulatedBody* dst)
{
	int i;
	int mainBodyIndex = -1;
	RigidBody* cloneBody;
	Joint* cloneJoint;

// TODO: check what we should do with the ID
	dst->id = this->id;
	dst->name = this->name;
	for (i=0; i < (int)rigidBodyList.size(); i++)
	{
		cloneBody = this->rigidBodyList[i]->clone();
		dst->rigidBodyList.push_back(cloneBody);
		dst->rigidBodyMap[cloneBody->getID()] = cloneBody;

		if (this->rigidBodyList[i] == this->mainBody)
			mainBodyIndex = i;
	} // for
	for (i=0; i < (int)jointList.size(); i++)
	{
		cloneJoint = this->jointList[i]->clone();
		cloneJoint->setObject1(dst->rigidBodyMap[this->jointList[i]->getObject1()->getID()]);
		cloneJoint->setObject2(dst->rigidBodyMap[this->jointList[i]->getObject2()->getID()]);
		dst->jointList.push_back(cloneJoint);
		dst->jointMap[cloneJoint->getID()] = cloneJoint;
	} // for

	assert(mainBodyIndex >= 0);
	dst->mainBody = dst->rigidBodyList[mainBodyIndex];
	dst->invMainBodyOffset = this->invMainBodyOffset;
	dst->offsetTransformation = this->offsetTransformation;
} // cloneData

ArticulatedBody* ArticulatedBody::clone()
{
	ArticulatedBody* result = new ArticulatedBody;
	cloneData(result);
	return result;
} // clone

void ArticulatedBody::setID(uint64_t id)
{
	int i;
	uint32_t articulatedBodyID;
	uint32_t objectID;
	uint32_t temp;
	uint64_t newID;

	if (addedToSimulation)
	{
		printf("ArticulatedBody::setID(): ERROR: can't change ID of Object which is already added to Simulation!\n");
		return;
	} // if
	if (id == this->id)
		return;

	articulatedBodyID = (uint32_t)id;

	SimulationObjectInterface::setID(articulatedBodyID);

	for (i=0; i < (int)rigidBodyList.size(); i++)
	{
		split(rigidBodyList[i]->getID(), temp, objectID);
		newID = join(articulatedBodyID, objectID);
		rigidBodyList[i]->setID(newID);
	} // for
	for (i=0; i < (int)jointList.size(); i++)
	{
		split(jointList[i]->getID(), temp, objectID);
		newID = join(articulatedBodyID, objectID);
		jointList[i]->setID(newID);
	} // for
} // setID

bool ArticulatedBody::render()
{
	int i;
	bool result = false;

	for (i=0; i < (int)rigidBodyList.size(); i++)
		result = rigidBodyList[i]->render() || result;

	return result;
} // render

void ArticulatedBody::setActive(bool active)
{
	int i;
	for (i=0; i < (int)rigidBodyList.size(); i++)
		rigidBodyList[i]->setActive(active);
} // setActive

void ArticulatedBody::setVisible(bool visible)
{
	int i;
	for (i=0; i < (int)rigidBodyList.size(); i++)
		rigidBodyList[i]->setVisible(visible);
} // setVisible

void ArticulatedBody::setOffsetTransformation(TransformationData& trans)
{
	this->offsetTransformation = trans;
} // setOffsetTransformation

const TransformationData& ArticulatedBody::getOffsetTransformation()
{
	return offsetTransformation;
} // getOffsetTransformation

RigidBody* ArticulatedBody::getMainBody()
{
	return mainBody;
} // getMainBody

RigidBody* ArticulatedBody::getRigidBodyByID(unsigned id)
{
	return rigidBodyMap[id];
} // getRigidBodyByID

void ArticulatedBody::getRigidBodies(std::vector<RigidBody*>& rigidBodyList) {
	int i;
	for (i=0; i < (int)this->rigidBodyList.size(); i++)
		rigidBodyList.push_back(this->rigidBodyList[i]);
} // getRigidBodies

Joint* ArticulatedBody::getJointByID(unsigned id)
{
	return jointMap[id];
} // getJointByID

void ArticulatedBody::getJoints(std::vector<Joint*>& jointList) {
	int i;
	for (i=0; i < (int)this->jointList.size(); i++)
		jointList.push_back(this->jointList[i]);
} // getJoints

/***
 *  FINALTRANS = GLOBALOFFSET * CURRENTTRANS
 *  GLOBALOFFSET = FINALTRANS * CURRENTTRANS^-1
 *
 * RIGIDBODYTRANS = ARTBODYTRANS * RIGIDBODYOFFSET
 * ARTBODYTRANS = RIGIDBODYTRANS * RIGIDBODYOFFSET^-1
 *
 ***/
void ArticulatedBody::setTransformation(TransformationData& trans)
{
	int i;
	TransformationData artBodyTrans, bodyTrans, globalOffset, finalTrans;

	bodyTrans = mainBody->getTransformation();
	multiply(artBodyTrans, bodyTrans, invMainBodyOffset);
	invert(artBodyTrans);
	multiply(globalOffset, trans, artBodyTrans);
	
	for (i=0; i < (int)rigidBodyList.size(); i++)
	{
		bodyTrans = rigidBodyList[i]->getTransformation();
		multiply(finalTrans, globalOffset, bodyTrans);
		rigidBodyList[i]->setTransformation(finalTrans);
	} // for
} // setTransformation

TransformationData ArticulatedBody::getTransformation()
{
	TransformationData result, bodyTrans, artBodyTrans;

	bodyTrans = mainBody->getTransformation();
	multiply(artBodyTrans, bodyTrans, invMainBodyOffset);
	multiply(result, artBodyTrans, offsetTransformation);
	return result;
} // getTransformation

dBodyID ArticulatedBody::getODEBody()
{
	return mainBody->getODEBody();
} // getODEBody
	
std::string ArticulatedBody::getType()
{
	return className;
} // getType

} // oops
