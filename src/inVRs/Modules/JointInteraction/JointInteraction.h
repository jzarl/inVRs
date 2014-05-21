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

#ifndef _JOINTINTERACTION_H
#define _JOINTINTERACTION_H

#include <string>
#include <gmtl/Quat.h>
#include <ode/ode.h>

#include "JointInteractionMath.h"
#include <irrXML.h>
#include "Joints.h"
#include "Constraint.h"
#include <inVRs/SystemCore/WorldDatabase/Entity.h>
#include <inVRs/SystemCore/ComponentInterfaces/ModuleInterface.h>
#include <inVRs/SystemCore/TransformationManager/TransformationPipe.h>
#include <inVRs/SystemCore/EventManager/EventManager.h>

#include "JointInteractionSharedLibraryExports.h"


typedef std::vector<Joint*> jointVector;


struct ODEObject {
  
	dBodyID body;			// the body
	Entity* entity;			// matching Entity-object
};

/**
 * \todo Add documentation
 * This module always has to be initialized before the Interaction-module!!!
 */
class INVRS_JOINTINTERACTION_API JointInteraction : public ModuleInterface
{
protected:
	friend class Joint;
	friend class Constraint;
	friend class JointInteractionBeginEvent;
	friend class JointInteractionEndEvent;
	bool isInitialized;
	std::string configFile;
	std::map< unsigned, Entity* > entityMap;
	std::vector<Joint*> jointList;
	std::vector<Joint*> attachedJoints;
	std::map< int, jointVector* > jointMap;
	std::map< int, ODEObject* > linkedObjMap;

	EventPipe* incomingEvents;

	std::map< unsigned, TransformationPipe* > linkedObjPipes;
	TransformationData userTrans;
	TransformationData deltaTrans;
	ODEObject* grabbedObject;
	bool isGrabbing;
	float maxDist, maxRotDist;

// Helper methods for config-file-loading
	void loadConfig(IrrXMLReader* xml);
	void loadJoints(IrrXMLReader* xml);
	Joint* xmlParseJoint(IrrXMLReader* xml, std::string& jointType);
	void xmlParseEntities(IrrXMLReader* xml, Joint* joint);
	void xmlParseConstraintAttributes(IrrXMLReader* xml, Constraint* constraint);

// Method to register TransformationPipes via Event
	void registerTransformationPipe(unsigned envBasedId, TransformationPipe* pipe);

// ODE Part
	dWorldID world;
	dSpaceID space;
	int stepsPerFrame;

	void attachJoints(int entityID);
	void grabEntity(Entity* entity, TransformationData offset);
	void readConfig(std::string fileName);

	dBodyID getBodyWithID(int entityID);
	Entity* getEntityWithID(int entID);


public:
	JointInteraction();
	virtual ~JointInteraction();

	virtual bool loadConfig(std::string configFile);
	virtual void cleanup();
	virtual std::string getName();

	virtual void step();

	void init();
	bool grabbing();
	bool grab(unsigned entityID, TransformationData offset);
	bool ungrab();
	TransformationData update(TransformationData trans);

	Joint* getJointWithID(int jointID);
};

#endif
