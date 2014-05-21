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

#ifndef _ARTICULATEDBODY_H
#define _ARTICULATEDBODY_H

#include <string>
#include <vector>
#include <map>
#include <inVRs/SystemCore/DataTypes.h>
#include "Interfaces/SimulationObjectInterface.h"
#include <ode/ode.h>

namespace oops
{

class RigidBody;
class Joint;

class ArticulatedBody : public SimulationObjectInterface
{
protected:
	friend class Simulation;
	friend class DefaultArticulatedBodyFactory;

	std::vector<RigidBody*> rigidBodyList;
	std::vector<Joint*> jointList;
	std::map<unsigned, RigidBody*> rigidBodyMap;
	std::map<unsigned, Joint*> jointMap;
	TransformationData offsetTransformation;
	TransformationData invMainBodyOffset;
	RigidBody* mainBody;

	dSpaceID bodySpace;

	std::string className;

	virtual void cloneData(ArticulatedBody* dst);
// protected:
// // Methods derived from SimulationObject
// 	virtual void build(dWorldID world, dSpaceID space = 0);
// 	virtual void destroy();

public:
	ArticulatedBody();
	virtual ~ArticulatedBody();
	virtual ArticulatedBody* clone();

	// overwrittten method from SimulationObjectInterface
	virtual void setID(uint64_t id);

	virtual bool render();
	virtual void setActive(bool active);
	virtual void setVisible(bool visible);
	virtual dBodyID getODEBody();

	virtual void setTransformation(TransformationData& trans);
	virtual void setOffsetTransformation(TransformationData& trans);

	virtual TransformationData getTransformation();
	virtual const TransformationData& getOffsetTransformation();

	virtual RigidBody* getMainBody();
	virtual RigidBody* getRigidBodyByID(unsigned id);
	virtual void getRigidBodies(std::vector<RigidBody*>& rigidBodyList);
	virtual Joint* getJointByID(unsigned id);
	virtual void getJoints(std::vector<Joint*>& jointList);

	virtual std::string getType();
}; // ArticulatedBody

} // oops

#endif // _ARTICULATEDBODY_H
