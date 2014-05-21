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

#include "JointInteraction.h"

#include <assert.h>
#include <gmtl/VecOps.h>
#include <gmtl/QuatOps.h>
#include <gmtl/AxisAngle.h>
#include <gmtl/Generate.h>
#include <gmtl/Xforms.h>

#include <inVRs/SystemCore/WorldDatabase/WorldDatabase.h>
#include <inVRs/SystemCore/WorldDatabase/Environment.h>
#include <inVRs/SystemCore/DataTypes.h>
#include <inVRs/SystemCore/TransformationManager/TransformationManager.h>
#include <inVRs/SystemCore/ModuleIds.h>
#include "Events/JointInteractionEvent.h"
#include <inVRs/SystemCore/DebugOutput.h>
#include <inVRs/SystemCore/XMLTools.h>
#include <inVRs/SystemCore/SystemCore.h>
#include <inVRs/SystemCore/UtilityFunctions.h>
#include <inVRs/SystemCore/ComponentInterfaces/module_core_api.h>

/***
 *	Methods of class JointInteraction
 **/

JointInteraction::JointInteraction()
: ModuleInterface()
{
	isInitialized = false;
	incomingEvents = NULL;
//	SystemCore::registerModuleInterface(this);
} // JointInteraction

JointInteraction::~JointInteraction()
{
//	SystemCore::unregisterModuleInterface(this);
} // ~JointInteraction

bool JointInteraction::loadConfig(std::string configFile)
{
	world = dWorldCreate();
	space = dSimpleSpaceCreate(0);
	isGrabbing = false;
	stepsPerFrame = 50;
	maxDist = 20;
	deltaTrans = identityTransformation();
	this->configFile = configFile;
	return true;
} // loadConfig

void JointInteraction::cleanup()
{

} // cleanup

std::string JointInteraction::getName()
{
	return "JointInteraction";
} // getName

/**
 *	This method initializes the JointInteraction class. It therefore retrieves
 *	a copy of the Entity-list from the WorldDatabase and
 *	initializes the variables needed for ODE-simulation.
 *	@param configFile location of the config-file
 */
void JointInteraction::init()
{
	incomingEvents = EventManager::getPipe(JOINT_INTERACTION_MODULE_ID);
	EventManager::registerEventFactory("JointInteractionBeginEvent", new JointInteractionBeginEvent::Factory());
	EventManager::registerEventFactory("JointInteractionEndEvent", new JointInteractionEndEvent::Factory());
	readConfig(configFile);
	isInitialized = true;
} // init

/**
 * This method reads the config-file for the JointJointInteraction-module
 * @param fileName location of the config-file
 **/
void JointInteraction::readConfig(std::string fileName)
{
	std::string ConcatenatedPath = getConcatenatedPath(fileName, "JointConfiguration");

	if(!fileExists(ConcatenatedPath))
	{
		printd(ERROR, "JointInteraction::readConfig(): cannot read from file %s\n", ConcatenatedPath.c_str());
		return;
	} // if

	IrrXMLReader* xml = createIrrXMLReader(ConcatenatedPath.c_str());

	if (!xml)
	{
		printd(ERROR, "JointInteraction::readConfig(): ERROR: could not find config-file %s\n", ConcatenatedPath.c_str());
		return;
	} // if

	printd(INFO, "JointInteraction::readConfig(): loading from file: %s\n", fileName.c_str());
	while (xml && xml->read())
	{
		switch (xml->getNodeType())
		{
			case EXN_ELEMENT:
				if (!strcmp("jointInteraction", xml->getNodeName()))
				{
					// Root node
				} // if
				else if (!strcmp("config", xml->getNodeName()))
				{
					loadConfig(xml);
				} // else if
				else if (!strcmp("joints", xml->getNodeName()))
				{
					loadJoints(xml);
				} // else if
				else
				{
					printd(WARNING, "JointInteraction::readConfig(): WARNING: unknown element with name %s found!\n", xml->getNodeName());
				} // else
				break;
			default:
				break;
		} // switch
	} // while
	delete xml;
} // readConfig

void JointInteraction::loadConfig(IrrXMLReader* xml)
{
	bool finished = false;
	while (!finished && xml && xml->read())
	{
		switch (xml->getNodeType())
		{
			case EXN_ELEMENT:
				if (!strcmp("stepsPerFrame", xml->getNodeName()))
				{
					stepsPerFrame = atoi(xml->getAttributeValue("value"));
				} // if
				else if (!strcmp("maxDist", xml->getNodeName()))
				{
					maxDist = atof(xml->getAttributeValue("value"));
				} // else if
				else if (!strcmp("maxRotDist", xml->getNodeName()))
				{
					maxRotDist = atof(xml->getAttributeValue("value"));
				} // else if
				break;
			case EXN_ELEMENT_END:
				if (!strcmp("config", xml->getNodeName()))
					finished = true;
				break;
			default:
				break;
		} // switch
	} // while
	assert(xml);
} // loadConfig

/**
 *	This method loads and parses the config-file. It searches for
 *	joint entries in the configuration, creates corresponding objects
 *	and pushes them into a Joint-list.
 *	@param fileName: URL of the config-file
 */
void JointInteraction::loadJoints(IrrXMLReader* xml)
{
	Joint* joint;
	jointVector* jList;
	std::string jointType;
	int id1, id2;

	bool finished = false;

	while(!finished && xml && xml->read())
	{
		switch (xml -> getNodeType())
		{
			case EXN_ELEMENT:
				if(!strcmp("joint", xml->getNodeName()))
				{
					joint = xmlParseJoint(xml, jointType);
					jointList.push_back(joint);
					joint->getEntities(id1, id2);
//					printf("Adding joint between %i and %i\n", id1, id2);
					if (id1 != 0)
					{
						jList = jointMap[id1];
						if (!jList)
						{
							jList = new jointVector();
							jointMap[id1] = jList;
						} // if
						jList->push_back(joint);
					} // if
					if (id2 != 0)
					{
						jList = jointMap[id2];
						if (!jList)
						{
							jList = new jointVector();
							jointMap[id2] = jList;
						} // if
						jList->push_back(joint);
					} // if
					joint = NULL;
				} // if
				break;
			case EXN_ELEMENT_END:
				if(!strcmp("joints", xml->getNodeName()))
					finished = true;
				break;
			default:
				break;
		} // switch
	} // while
	assert (xml);
} // loadJoints

Joint* JointInteraction::xmlParseJoint(IrrXMLReader* xml, std::string& jointType)
{
	bool finished = false;
	Constraint* constraint = NULL;

	Joint* joint = JointFactory::create(xml->getAttributeValue("type"));
	jointType = xml->getAttributeValue("type");
	joint->setWorld(world);
	joint->setId(atoi(xml->getAttributeValue("id")));
	joint->setJointInteractionClass(this);
	if (xml->getAttributeValue("active"))
		joint->setActive(atoi(xml->getAttributeValue("active")));

	while (!finished && xml && xml->read())
	{
		switch (xml->getNodeType())
		{
			case EXN_ELEMENT:
				if (!strcmp("entities", xml->getNodeName()))
				{
					xmlParseEntities(xml, joint);
				} // else if
				else if (!strcmp("anchor", xml->getNodeName()))
				{
					if (!jointType.compare("ball") || !jointType.compare("Ball") || !jointType.compare("BALL"))
					{
						BallJoint* ball = dynamic_cast<BallJoint*>(joint);
						ball->setAnchor(atof(xml->getAttributeValue("xPos")), atof(xml->getAttributeValue("yPos")), atof(xml->getAttributeValue("zPos")));
					} // if
					else if (!jointType.compare("hinge") || !jointType.compare("Hinge") || !jointType.compare("HINGE"))
					{
						HingeJoint* hinge = dynamic_cast<HingeJoint*>(joint);
						hinge->setAnchor(atof(xml->getAttributeValue("xPos")), atof(xml->getAttributeValue("yPos")), atof(xml->getAttributeValue("zPos")));
					} // else if
					else if (!jointType.compare("universal") || !jointType.compare("Universal") || !jointType.compare("UNIVERSAL"))
					{
						UniversalJoint* universal = dynamic_cast<UniversalJoint*>(joint);
						universal->setAnchor(atof(xml->getAttributeValue("xPos")), atof(xml->getAttributeValue("yPos")), atof(xml->getAttributeValue("zPos")));
					} // else if
					else if (!jointType.compare("hinge2") || !jointType.compare("Hinge2") || !jointType.compare("HINGE2"))
					{
						Hinge2Joint* hinge2 = dynamic_cast<Hinge2Joint*>(joint);
						hinge2->setAnchor(atof(xml->getAttributeValue("xPos")), atof(xml->getAttributeValue("yPos")), atof(xml->getAttributeValue("zPos")));
					} // else if
					else
						printd(WARNING, "JointInteraction::loadJoints(): WARNING: JOINT-TYPE  %s NEEDS NO anchor ELEMENT!!!\n", jointType.c_str());
				} // else if
				else if (!strcmp("axis", xml->getNodeName()))
				{
					if (!jointType.compare("hinge") || !jointType.compare("Hinge") || !jointType.compare("HINGE"))
					{
						HingeJoint* hinge = dynamic_cast<HingeJoint*>(joint);
						hinge->setAxis(atof(xml->getAttributeValue("xDir")), atof(xml->getAttributeValue("yDir")), atof(xml->getAttributeValue("zDir")));
					} // if
					else if (!jointType.compare("slider") || !jointType.compare("Slider") || !jointType.compare("SLIDER"))
					{
						SliderJoint* slider = dynamic_cast<SliderJoint*>(joint);
						slider->setAxis(atof(xml->getAttributeValue("xDir")), atof(xml->getAttributeValue("yDir")), atof(xml->getAttributeValue("zDir")));
					} // else if
					else if (!jointType.compare("universal") || !jointType.compare("Universal") || !jointType.compare("UNIVERSAL")) {
						UniversalJoint* universal = dynamic_cast<UniversalJoint*>(joint);
						if (atoi(xml->getAttributeValue("index")) == 1)
							universal->setAxis1(atof(xml->getAttributeValue("xDir")), atof(xml->getAttributeValue("yDir")), atof(xml->getAttributeValue("zDir")));
						else if (atoi(xml->getAttributeValue("index")) == 2)
							universal->setAxis2(atof(xml->getAttributeValue("xDir")), atof(xml->getAttributeValue("yDir")), atof(xml->getAttributeValue("zDir")));
						else
							printd(WARNING, "JointInteraction::loadJoints(): WARNING: WRONG ATTRIBUTE FOUND IN axis ELEMENT of UniversalJoint!!!\n");
					} // else if
					else if (!jointType.compare("hinge2") || !jointType.compare("Hinge2") || !jointType.compare("HINGE2")) {
						Hinge2Joint* hinge2 = dynamic_cast<Hinge2Joint*>(joint);
						if (atoi(xml->getAttributeValue("index")) == 1)
							hinge2->setAxis1(atof(xml->getAttributeValue("xDir")), atof(xml->getAttributeValue("yDir")), atof(xml->getAttributeValue("zDir")));
						else if (atoi(xml->getAttributeValue("index")) == 2)
							hinge2->setAxis2(atof(xml->getAttributeValue("xDir")), atof(xml->getAttributeValue("yDir")), atof(xml->getAttributeValue("zDir")));
						else
							printd(WARNING, "JointInteraction::loadJoints(): WARNING: WRONG ATTRIBUTE FOUND IN axis ELEMENT of Hinge2Joint!!!\n");
					} // else if
					else
						printd(WARNING, "JointInteraction::loadJoints(): WARNING: JOINT-TYPE  %s NEEDS NO axis ELEMENT!!!\n", jointType.c_str());
				} // else if
				else if (!strcmp("angles", xml->getNodeName()))
				{
					if (!jointType.compare("hinge") || !jointType.compare("Hinge") || !jointType.compare("HINGE"))
					{
						HingeJoint* hinge = dynamic_cast<HingeJoint*>(joint);
						hinge->setAngles(atof(xml->getAttributeValue("min")), atof(xml->getAttributeValue("max")));
					} // if
					else
						printd(WARNING, "JointInteraction::loadJoints(): WARNING: JOINT-TYPE  %s HAS NO angles ELEMENT!!!\n", jointType.c_str());
				} // else if
				else if (!strcmp("positions", xml->getNodeName()))
				{
					SliderJoint* slider = dynamic_cast<SliderJoint*>(joint);
					slider->setPositions(atof(xml->getAttributeValue("min")), atof(xml->getAttributeValue("max")));
				} // else if
				else if (!strcmp("activateIF", xml->getNodeName()))
				{
					constraint = new Constraint(this);
					constraint->setActionType(Constraint::ACTIVATEIF);
					constraint->setOwner(joint);
					xmlParseConstraintAttributes(xml, constraint);
					joint->addConstraint(constraint);
					constraint = NULL;
				} // else if
				else if (!strcmp("deactivateIF", xml->getNodeName()))
				{
					constraint = new Constraint(this);
					constraint->setActionType(Constraint::DEACTIVATEIF);
					constraint->setOwner(joint);
					xmlParseConstraintAttributes(xml, constraint);
					joint->addConstraint(constraint);
					constraint = NULL;
				} // else if
				else if (!strcmp("activeIF", xml->getNodeName()))
				{
					constraint = new Constraint(this);
					constraint->setActionType(Constraint::ACTIVEIF);
					constraint->setOwner(joint);
					xmlParseConstraintAttributes(xml, constraint);
					joint->addConstraint(constraint);
					constraint = NULL;
				} // else if
				else if (!strcmp("inactiveIF", xml->getNodeName()))
				{
					constraint = new Constraint(this);
					constraint->setActionType(Constraint::INACTIVEIF);
					constraint->setOwner(joint);
					xmlParseConstraintAttributes(xml, constraint);
					joint->addConstraint(constraint);
					constraint = NULL;
				} // else if
				break;
			case EXN_ELEMENT_END:
				if (!strcmp("joint", xml->getNodeName()))
					finished = true;
				break;
			default:
				break;
		} // switch
	} // while
	assert(xml);
	return joint;
} // xmlParseJoint

void JointInteraction::xmlParseEntities(IrrXMLReader* xml, Joint* joint)
{
	Entity* entity = NULL;
	unsigned short environmentId, entityId1, entityId2;
	unsigned combinedEntityId1, combinedEntityId2;

	environmentId = xml->getAttributeValueAsInt("environmentId");
	entityId1 = xml->getAttributeValueAsInt("entityId1");
	entityId2 = xml->getAttributeValueAsInt("entityId2");
	if (entityId1 != 0)
	{
		combinedEntityId1 = join(environmentId, entityId1);
		if (!entityMap[combinedEntityId1])
		{
			entity = WorldDatabase::getEntityWithEnvironmentId(combinedEntityId1);
			assert(entity);
			entityMap[combinedEntityId1] = entity;
			entity = NULL;
		} // if
	}
	else
		combinedEntityId1 = 0;

	if (entityId2 != 0)
	{
		combinedEntityId2 = join(environmentId, entityId2);
		if (!entityMap[combinedEntityId2])
		{
			entity = WorldDatabase::getEntityWithEnvironmentId(combinedEntityId2);
			assert(entity);
			entityMap[combinedEntityId2] = entity;
			entity = NULL;
		} // if
	} // if
	else
		combinedEntityId2 = 0;

	joint->setEntities(combinedEntityId1, combinedEntityId2);
	joint->setMainEntity(xml->getAttributeValueAsInt("anchorEntity"));
} // xmlParseEntities

/**
 *	This method does the parsing of Constraint-types in the configuration.
 *	It should only be called by the loadJoints-method!!!
 *	@param xml: the used XML-Reader
 *	@param constraint: the current constraint
 */
void JointInteraction::xmlParseConstraintAttributes(IrrXMLReader* xml, Constraint* constraint)
{
	if (xml->getAttributeValue("joint"))
		constraint->setJointId(atoi(xml->getAttributeValue("joint")));
	else if (xml->getAttributeValue("entity"))
		constraint->setEntityId(atoi(xml->getAttributeValue("entity")));
	if (xml->getAttributeValue("isActive")) {
		constraint->setCondition(Constraint::ISACTIVE);
		constraint->setActive(atoi(xml->getAttributeValue("isActive")));
	} else if (xml->getAttributeValue("isGrabbed")) {
		constraint->setCondition(Constraint::ISGRABBED);
		constraint->setGrabbed(atoi(xml->getAttributeValue("isGrabbed")));
	} else if (xml->getAttributeValue("angle1GT")) {
		constraint->setCondition(Constraint::ANGLE1GT);
		constraint->setAngle(atof(xml->getAttributeValue("angle1GT")));
	} else if (xml->getAttributeValue("angle1LT")) {
		constraint->setCondition(Constraint::ANGLE1LT);
		constraint->setAngle(atof(xml->getAttributeValue("angle1LT")));
	} else if (xml->getAttributeValue("angle2GT")) {
		constraint->setCondition(Constraint::ANGLE2GT);
		constraint->setAngle(atof(xml->getAttributeValue("angle2GT")));
	} else if (xml->getAttributeValue("angle2LT")) {
		constraint->setCondition(Constraint::ANGLE2LT);
		constraint->setAngle(atof(xml->getAttributeValue("angle2LT")));
	} else if (xml->getAttributeValue("positionGT")) {
		constraint->setCondition(Constraint::POSITIONGT);
		constraint->setPosition(atof(xml->getAttributeValue("positionGT")));
	} else if (xml->getAttributeValue("positionLT")) {
		constraint->setCondition(Constraint::POSITIONLT);
		constraint->setPosition(atof(xml->getAttributeValue("positionLT")));
	} // else if
} // xmlParseConstraintAttributes

void JointInteraction::registerTransformationPipe(unsigned envBasedId, TransformationPipe* pipe)
{
	linkedObjPipes[envBasedId] = pipe;
} // registerTransformationPipe

/**
 * This method returns, if the user is grabbing an object or not
 * @return true, if an object is grabbed
 *			false if not
 **/
bool JointInteraction::grabbing()
{
	return isGrabbing;
} // grabbing

void JointInteraction::step()
{
	std::deque<Event*>* incomingEventsLocal;
	Event* evt;

	incomingEventsLocal = incomingEvents->makeCopyAndClear();
	while(!incomingEventsLocal->empty())
	{
		printd(INFO, "JointInteraction::step(): found an Event!\n");
		evt = (*incomingEventsLocal)[0];
		incomingEventsLocal->pop_front();
		evt->execute();
		delete evt;
		printd(INFO, "JointInteraction::step(): Event executed and deleted!\n");
	}
	delete incomingEventsLocal;
} // step

/**
 *	This method updates the position and orientation of the grabbed
 *	object and all connected objects. If the grabbed object has
 *	no connections to other objects (via joints) the method simply
 *	applies the passed position and orientation to the grabbed object.
 *	Otherwise the calculation of the new transformation takes place in
 *	two steps:
 *	First the orientational difference between the grabbed object and
 *	and the passed orientation is calculated and applied to the object
 *	as torque. Then a simulation-step takes place, where the timestep
 *	is set to 0.5, what should leed to the half orientation correction.
 *	After the simulation step the objects velocities are set to zero.
 *	In the second step the difference of the objects position and the
 *	passed one are calculated and applied to the object as force. Like
 *	in step one a simulation-step with dt=0.5 takes place and the
 *	velocities are set to zero again.
 *	After the computation of the new transformations, the method iterates
 *	over all objects, which are grabbed or are linked with the grabbed
 *	object and copies their transformation from the ODE-representation
 *	to the Entity-members.
 *	At last the method calls the checkConstraints-method of all currently
 *	used joints, so that the joints can be activated or deactivated.
 *	@param position: position of the manipulating object
 *	@param orientation: orientation of the manipulating object
 */
TransformationData JointInteraction::update(TransformationData trans)
{
	if (!isInitialized)
		init();

	gmtl::AxisAnglef AxAng;
	gmtl::Vec3f diff;
	dQuaternion quat;

	userTrans.position = trans.position;
	userTrans.orientation = trans.orientation;

	if (!isGrabbing)
		return identityTransformation();

	int numJoints = dBodyGetNumJoints(grabbedObject->body);
	if (!numJoints)
	{ // Simple code for objects without Joints
		convert(quat, userTrans.orientation);
		dBodySetPosition(grabbedObject->body, userTrans.position[0], userTrans.position[1], userTrans.position[2]);
		dBodySetQuaternion(grabbedObject->body, quat);
	}
	else
	{ // Code for object with Joints
		TransformationData objectTrans;
		gmtl::Quatf rotation;
		gmtl::Vec3f force, torque;

		const dReal* pos = dBodyGetPosition(grabbedObject->body);
		const dReal* rot = dBodyGetQuaternion(grabbedObject->body);

		convert(objectTrans.position, pos);
		convert(objectTrans.orientation, rot);

// TODO: move this code to manipulationTerminationModel
		// ungrab object if distance is too high
// 		diff = userTrans.position - objectTrans.position;
// 		if (gmtl::length(diff) > maxDist)
// 		{
// 			printf("JointInteraction::update(): distance %f too high!\n", gmtl::length(diff));
// 			ungrab();
// 			return identityTransformation();
// 		} // if

// STEP 1: apply Torque
		// calculate Torque = angular offset from object orientation to current orientation
		rotation = userTrans.orientation;
		rotation *= invert(objectTrans.orientation);
		gmtl::set(AxAng, rotation);
		torque[0] = AxAng[0]*AxAng[1];  // rotAngle*rotX
		torque[1] = AxAng[0]*AxAng[2];  // rotAngle*rotY
		torque[2] = AxAng[0]*AxAng[3];  // rotAngle*rotZ

// TODO: move this code to manipulationTerminationModel
		// ungrab object if angular distance is too high
// 		if (gmtl::length(torque) > maxRotDist)
// 		{
// 			printf("JointInteraction::update(): rotation %f too high!\n", gmtl::length(torque));
// 			ungrab();
// 			return identityTransformation();
// 		} // if

		// create a Ball Joint to adjust orientation without changing the position
		dJointID universalJoint = dJointCreateBall(world, 0);
		dJointAttach(universalJoint, grabbedObject->body, 0);
		dJointSetBallAnchor(universalJoint, pos[0], pos[1], pos[2]);

//		printf("Torque = %f %f %f\n", torque[0], torque[1], torque[2]);
/***
 * old way --> is very instable due to high timestep!!!
 ***
 *		dBodySetTorque(grabbedObject->body, torque[0], torque[1], torque[2]);
 * // Half way to destination
 *		dWorldStep(world, 0.5);
 **/
		// take some simulation-steps for adjusting object's orientation
		for (int i=0; i < stepsPerFrame; i++)
		{
			dBodySetTorque(grabbedObject->body, torque[0], torque[1], torque[2]);
			dWorldStep(world, 1.0f/stepsPerFrame);
		} // for

		dJointDestroy(universalJoint);

// Reset speed and angular velocity to 0
		dBodySetLinearVel(grabbedObject->body, 0, 0, 0);
		dBodySetAngularVel(grabbedObject->body, 0, 0, 0);

// STEP 2: apply force into wanted direction
		// calculate Force = vector from object position to current position
		force = (userTrans.position - objectTrans.position);

//		printf("Force = %f %f %f\n", force[0], force[1], force[2]);
/***
 * old way --> is very instable due to high timestep!!!
 ***
 *		dBodySetForce(grabbedObject->body, force[0], force[1], force[2]);
 * // Half way to destination
 *		dWorldStep(world, 0.5);
 **/
		// take some simulation-steps for adjusting object's position
		for (int i=0; i < stepsPerFrame; i++)
		{
			dBodySetForce(grabbedObject->body, force[0], force[1], force[2]);
			dWorldStep(world, 1.0f/stepsPerFrame);
		} // for

// Reset speed and angular velocity to 0
		dBodySetLinearVel(grabbedObject->body, 0, 0, 0);
		dBodySetAngularVel(grabbedObject->body, 0, 0, 0);
	}

// Get position and Orientation from simulated object
	TransformationData newTransform = identityTransformation();
	const dReal* pos = dBodyGetPosition(grabbedObject->body);
	const dReal* rot = dBodyGetQuaternion(grabbedObject->body);

	convert(newTransform.orientation, rot);
	convert(newTransform.position, pos);

// Get position and Orientation from simulated objects
	std::map<int, ODEObject*>::iterator it = linkedObjMap.begin();
	ODEObject* object;
	TransformationData linkedObjectTrans = identityTransformation();
	while (it != linkedObjMap.end())
	{
		object = linkedObjMap[(*it).first];
		if (object != grabbedObject)
		{
			linkedObjectTrans = object->entity->getWorldTransformation();
			const dReal* pos1 = dBodyGetPosition(object->body);
			const dReal* rot1 = dBodyGetQuaternion(object->body);

			convert (linkedObjectTrans.orientation, rot1);
			convert (linkedObjectTrans.position, pos1);

			linkedObjPipes[it->first]->push_back(linkedObjectTrans);
// 			gmtl::set(AxAng, newRot);

/*			object->entity->setTranslation(pos1[0], pos1[1], pos1[2]);
			object->entity->setRotation(AxAng[1], AxAng[2], AxAng[3], AxAng[0]);*/
// 			assert(false);
// 			object->entity->setEnvironmentTransformation(linkedObjectTrans);
// 			object->entity->update();
		} // if
		++it;
	} // while

// Update angles in HingeJoints and check joint-constraints
	HingeJoint* hinge;
	for (int i=0; i < (int)attachedJoints.size(); i++)
	{
		hinge = dynamic_cast<HingeJoint*>(attachedJoints[i]);
		if (hinge)
			hinge->checkAngles();

		assert(attachedJoints[i]);
		attachedJoints[i]->checkConstraints();
	} // for

	TransformationData result;
	multiply(result, newTransform, deltaTrans);

	TransformationPipe* pipe = linkedObjPipes[grabbedObject->entity->getEnvironmentBasedId()];
	if (pipe)
		pipe->push_back(result);
	else
		printd(WARNING, "JointInteraction::update(): Could not find Pipe for manipulated Object!\n");

	return newTransform;
} // update

/**
 * This method is called if the user wants to grab an object.
 * It checks if the user is already grabbing and if not it searches
 * for an object where the actual user-position is inside the bounding
 * box of this object. If an object is found it is pushed into a list
 * of possible results. Finally the object with the smallest bounding
 * box in the list will be grabbed by calling the grabEntity-method.
 * @return true if an object is grabbed
 **/
bool JointInteraction::grab(unsigned entityID, TransformationData offset)
{
	if (isGrabbing)
		return false;

	Entity* entity = entityMap[entityID];
	if (!entity)
		entity = WorldDatabase::getEntityWithEnvironmentId(entityID);

	if (!entity || entity->getEntityType()->isFixed())
		return false;

	grabEntity(entity, offset);
	return true;
} // grab

/**
 * This method is called if the user wants to release a grabbed object.
 * All attached joints in the world are detached and all used
 * ODE-objects are destroyed.
 * @return true, if an object is released
 **/
bool JointInteraction::ungrab()
{
	if (!isGrabbing)
		return false;

	unsigned userId;
	std::vector<unsigned> entityIds;
	JointInteractionEndEvent* evt;
	int i;
	ODEObject* object;
	isGrabbing = false;
	Entity* ent;

	for (i=0; i < (int)attachedJoints.size(); i++)
		attachedJoints[i]->detach();

	std::map< int, ODEObject*>::iterator it = linkedObjMap.begin();
	while (it != linkedObjMap.end())
	{
		object = linkedObjMap[(*it).first];
// TODO: open and close pipes via events
/*		if (object != grabbedObject)
		{*/
		ent = entityMap[it->first];
		if (!ent)
			ent = object->entity;
		entityIds.push_back(ent->getTypeBasedId());
		printd(INFO, "JointInteraction::ungrab(): adding Entity with ID %u\n", ent->getTypeBasedId());
	//  			TransformationManager::closePipe(linkedObjPipes[it->first]);
	// 		} // if
		dBodyDestroy(object->body);
		delete object;
		++it;
	} // while

	// send EndJointInteractionEvent to all users
	userId = UserDatabase::getLocalUserId();
	evt = new JointInteractionEndEvent(userId, entityIds);
	for (i=0; i < (int)attachedJoints.size(); i++)
		attachedJoints[i]->getMemberValue("internalChanges", evt);

	for (it = linkedObjMap.begin(); it != linkedObjMap.end(); ++it)
	{
		object = it->second;
/*		if (object != grabbedObject)
		{*/
			ent = entityMap[it->first];
			if (!ent)
				ent = object->entity;
			evt->addTransformationData(ent->getEnvironmentTransformation());
// 			evt->addEntityId(ent->getTypeBasedId());
// 		} // if
	} // for

	EventManager::sendEvent(evt, EventManager::EXECUTE_GLOBAL);

	linkedObjMap.clear();
	linkedObjPipes.clear();
	attachedJoints.clear();

//	dBodyDestroy(grabbedObject->body);
	return true;
} // ungrab

/**
 * This method initializes the grabbing of an object.
 * It creates an ODE-object with the current position and
 * orientation and stores the offset to the grabbed object
 * for correct update of the Entity-transformation
 * in the update method. It also creates all joints and
 * linked objects by calling the attachJoints-method.
 * @param entity Entity that should be grabbed
 **/
void JointInteraction::grabEntity(Entity* entity, TransformationData offset)
{
	unsigned entityID;
	unsigned typeBasedID;
	Entity* ent;
	dQuaternion quat;
	std::map< int, ODEObject*>::iterator it;
//	TransformationPipe* linkedObjectPipe;
//	unsigned short type, id;
	std::vector<unsigned> entityIds;
	unsigned userId;
	JointInteractionBeginEvent* evt;

	isGrabbing = true;

// // store position and orientation difference between user and object
	deltaTrans = offset;

// Create ODE-representation of grabbed object by simply creating an object with
// user's position and orientation
	ODEObject* odeObj = new ODEObject;
	odeObj->body = dBodyCreate(world);
	odeObj->entity = entity;

	dBodySetPosition(odeObj->body, userTrans.position[0], userTrans.position[1], userTrans.position[2]);
	convert(quat, userTrans.orientation);
	dBodySetQuaternion(odeObj->body, quat);

// attach all joints to object
	entityID = entity->getEnvironmentBasedId();
	linkedObjMap[entityID] = odeObj;
	attachJoints(entityID);

	grabbedObject = odeObj;

	entityIds.push_back(entity->getTypeBasedId());

// open Pipes for all linked objects
	for (it = linkedObjMap.begin(); it != linkedObjMap.end(); ++it)
	{
		if (it->second == grabbedObject)
			continue;
		ent = WorldDatabase::getEntityWithEnvironmentId(it->first);
		assert(ent->getEnvironmentBasedId() != entityID);
		typeBasedID = ent->getTypeBasedId();
		entityIds.push_back(typeBasedID);
/*
		split(typeBasedID, type, id);
// TODO: open pipes via Event
		linkedObjectPipe = TransformationManager::openPipe(JOINT_INTERACTION_MODULE_ID, WORLD_DATABASE_ID, 0, 0, type, id, 0, false);
		linkedObjPipes[it->first] = linkedObjectPipe;
*/
	} // for

	userId = UserDatabase::getLocalUserId();
	evt = new JointInteractionBeginEvent(userId, entityIds);
	EventManager::sendEvent(evt, EventManager::EXECUTE_GLOBAL);

} // grabEntity

/**
 * This method attaches all joints to the object with
 * the matching entityID.
 * @param entityID ID of the EntityTransform to which the
 * 				   joints should be attached.
 **/
void JointInteraction::attachJoints(int entityID)
{
	Joint* joint;
	jointVector* jList = jointMap[entityID];

	if (jList)
	{
		for (int i=0; i < (int)jList->size(); i++)
		{
			joint = jList->at(i);
			joint->attach();
		} // for
	} // if

} // attachJoints

/**
 * This method returns the ODE-representation of the
 * Entity with the passed ID. It searches
 * in the map of connected objects for a matching
 * ODEObject. If no object is found the method
 * creates one and stores it in the map.
 * @param entityID ID of the EntityTransform
 * @return ODE representation of the EntityTransform
 **/
dBodyID JointInteraction::getBodyWithID(int entityID)
{
	if (entityID == 0)
		return 0;

// 	TransformationPipe* objectPipe;
	Entity* entity;
	TransformationData trans;
	gmtl::Vec3f pos;
	gmtl::Quatf ori;
	gmtl::AxisAnglef axAng;
	dQuaternion quat;

	ODEObject* object = linkedObjMap[entityID];
	if (object == NULL)
	{
		entity = getEntityWithID(entityID);
		if (entity == NULL)
		{
			printd(ERROR, "JointInteraction::getBodyWithID(): ERROR: FOUND BODY WITHOUT MATCHING ENTITY OBJECT!!!\n");
			return 0;
		} // if

		object = new ODEObject;
		object->body = dBodyCreate(world);
		object->entity = entity;

		trans = entity->getEnvironmentTransformation();
		pos = trans.position;
		ori = trans.orientation;
	// get current object position and orientation
// 		pos[0] = entity->getXTrans();
// 		pos[1] = entity->getYTrans();
// 		pos[2] = entity->getZTrans();
// 		axAng.set(entity->getRotAngle(), entity->getXRot(), entity->getYRot(), entity->getZRot());
// 		gmtl::set(ori, axAng);

		convert(quat, ori);
		dBodySetPosition(object->body, pos[0], pos[1], pos[2]);
		dBodySetQuaternion(object->body, quat);

		linkedObjMap[entityID] = object;

// TODO: open and close pipes via events
// open TransformationPipe for handling transformations of connected entities
// 		split(entity->getTypeBasedId(), type, id);
// 		objectPipe = TransformationManager::openPipe(JOINT_INTERACTION_MODULE_ID, WORLD_DATABASE_ID, 0, 0, type, id, 0, 0);
// 		linkedObjPipes[entityID] = objectPipe;

// ingnore request if Entity is not movable
		if (!entity->getEntityType()->isFixed())
			attachJoints(entityID);
	} // if
	if (object->entity->getEntityType()->isFixed())
		return 0;

	return object->body;
} // getBodyWithID

/**
 * This method returns an Entity with the matching
 * ID.
 * @param entID ID of the requested EntityTransform
 * @return the EntityTransform if found, NULL if not
 **/
Entity* JointInteraction::getEntityWithID(int entID)
{
	Entity* result = NULL;
	result = entityMap[entID];
	return result;
} // getEntityWithID

/**
 * This method returns the joint with the matching ID.
 * @param jointID ID of the requested Joint
 * @return the Joint if found, NULL if not.
 **/
Joint* JointInteraction::getJointWithID(int jointID)
{
	int i;
	Joint* result = NULL;
	for (i=0; i < (int)jointList.size(); i++)
	{
		if (jointList[i]->getId() == jointID)
		{
			result = jointList[i];
			break;
		} // if
	} // for
	return result;
} // getJointWithID

MAKEMODULEPLUGIN(JointInteraction, SystemCore)
