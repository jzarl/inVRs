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

#include "oops/JointFactories.h"
#include <inVRs/SystemCore/DebugOutput.h>

#include <irrXML.h>

using namespace irr;
using namespace io;

namespace oops {

//*****************************************************************************

Joint* HingeJointFactory::create(std::string className, const XmlElement* element) {
	if (className != "hingeJoint")
		return NULL;

//TODO
	printd(ERROR,
			"HingeJointFactory::create(): NOT UPDATED TO NEW XML CONFIG FILE LOADING YET!\n");
	return NULL;
/*
	IrrXMLReader *xml = (IrrXMLReader*)args;
	HingeJoint* result= NULL;
	bool finished = false;
	int anchorBody = 1;
	bool minAngleSet = false;
	bool maxAngleSet = false;
	float minAngle, maxAngle;
	gmtl::Vec3f offset(0, 0, 0);
	gmtl::Vec3f axis(0, 0, 0);
	gmtl::Vec3f anchorPoint(0, 0, 0);

	while (!finished && xml && xml->read()) {
		switch (xml->getNodeType()) {
		case EXN_ELEMENT:
			if (!strcmp("anchorBody", xml->getNodeName()))
				anchorBody = xml->getAttributeValueAsInt("number");
			else if (!strcmp("globalOffset", xml->getNodeName())) {
				offset[0] = xml->getAttributeValueAsFloat("x");
				offset[1] = xml->getAttributeValueAsFloat("y");
				offset[2] = xml->getAttributeValueAsFloat("z");
			} // else if
			else if (!strcmp("globalAxis1", xml->getNodeName())) {
				axis[0] = xml->getAttributeValueAsFloat("x");
				axis[1] = xml->getAttributeValueAsFloat("y");
				axis[2] = xml->getAttributeValueAsFloat("z");
			} // else if
			else if (!strcmp("hingeAngles", xml->getNodeName())) {
				if (xml->getAttributeValue("minAngleDeg")) {
					minAngle = xml->getAttributeValueAsFloat("minAngleDeg") * M_PI / 180.f;
					minAngleSet = true;
				} // if
				if (xml->getAttributeValue("maxAngleDeg")) {
					maxAngle = xml->getAttributeValueAsFloat("maxAngleDeg") * M_PI / 180.f;
					maxAngleSet = true;
				} // if
			} // else if
			else
				printd(
						WARNING,
						"HingeJointFactory::create(): WARNING: unknown element %s found!\n",
						xml->getNodeName());
			break;

		case EXN_ELEMENT_END:
			if (!strcmp("hingeJoint", xml->getNodeName())) {
				result = new HingeJoint(anchorBody, offset, axis);
				if (minAngleSet)
					result->setMinAngle(minAngle);
				if (maxAngleSet)
					result->setMaxAngle(maxAngle);
				finished = true;
			} // if
			else
				printd(
						WARNING,
						"HingeJointFactory::create(): WARNING: unknown end-element %s found!\n",
						xml->getNodeName());
			break;

		default:
			break;
		} // switch
	} // while

	if (!finished)
		printd(ERROR,
				"HingeJointFactory::create(): ERROR while loading HingeJoint object!\n");

	return result;
*/
} // create

//*****************************************************************************

Joint* BallJointFactory::create(std::string className, const XmlElement* element) {
	if (className != "ballJoint")
		return NULL;

//TODO
	printd(ERROR,
			"BallJointFactory::create(): NOT UPDATED TO NEW XML CONFIG FILE LOADING YET!\n");
	return NULL;
/*
	IrrXMLReader *xml = (IrrXMLReader*)args;
	BallJoint* result= NULL;
	bool finished = false;
	int anchorBody = 1;
	gmtl::Vec3f offset(0, 0, 0);
	gmtl::Vec3f anchorPoint(0, 0, 0);

	while (!finished && xml && xml->read()) {
		switch (xml->getNodeType()) {
		case EXN_ELEMENT:
			if (!strcmp("anchorBody", xml->getNodeName()))
				anchorBody = xml->getAttributeValueAsInt("number");
			else if (!strcmp("globalOffset", xml->getNodeName())) {
				offset[0] = xml->getAttributeValueAsFloat("x");
				offset[1] = xml->getAttributeValueAsFloat("y");
				offset[2] = xml->getAttributeValueAsFloat("z");
			} // else if
			else
				printd(
						WARNING,
						"BallJointFactory::create(): WARNING: unknown element %s found!\n",
						xml->getNodeName());
			break;

		case EXN_ELEMENT_END:
			if (!strcmp("ballJoint", xml->getNodeName())) {
				result = new BallJoint(anchorBody, offset);
				finished = true;
			} // if
			else
				printd(
						WARNING,
						"BallJointFactory::create(): WARNING: unknown end-element %s found!\n",
						xml->getNodeName());
			break;

		default:
			break;
		} // switch
	} // while

	if (!finished)
		printd(ERROR,
				"BallJointFactory::create(): ERROR while loading BallJoint object!\n");

	return result;
*/
} // create

//*****************************************************************************

Joint* SliderJointFactory::create(std::string className, const XmlElement* element) {
	if (className != "sliderJoint")
		return NULL;

//TODO
	printd(ERROR,
			"SliderJointFactory::create(): NOT UPDATED TO NEW XML CONFIG FILE LOADING YET!\n");
	return NULL;

/*
	IrrXMLReader *xml = (IrrXMLReader*)args;
	SliderJoint* result= NULL;
	bool finished = false;
	int anchorBody = 1;
	gmtl::Vec3f axis = gmtl::Vec3f(0, 0, 0);
	bool minBoundarySet = false;
	bool maxBoundarySet = false;
	float minBoundary;
	float maxBoundary;

	while (!finished && xml && xml->read()) {
		switch (xml->getNodeType()) {
		case EXN_ELEMENT:
			if (!strcmp("anchorBody", xml->getNodeName()))
				anchorBody = xml->getAttributeValueAsInt("number");
			else if (!strcmp("globalAxis1", xml->getNodeName())) {
				axis[0] = xml->getAttributeValueAsFloat("x");
				axis[1] = xml->getAttributeValueAsFloat("y");
				axis[2] = xml->getAttributeValueAsFloat("z");
			} // else if
			else if (!strcmp("sliderBounds", xml->getNodeName())) {
				if (xml->getAttributeValue("minBoundary")) {
					minBoundary = xml->getAttributeValueAsFloat("minBoundary");
					minBoundarySet = true;
				} // if
				if (xml->getAttributeValue("maxBoundary")) {
					maxBoundary = xml->getAttributeValueAsFloat("maxBoundary");
					maxBoundarySet = true;
				} // if
			} // else if
			else
				printd(
						WARNING,
						"SliderJointFactory::create(): WARNING: unknown element %s found!\n",
						xml->getNodeName());
			break;

		case EXN_ELEMENT_END:
			if (!strcmp("sliderJoint", xml->getNodeName())) {
				result = new SliderJoint(axis);
				if (minBoundarySet)
					result->setMinBoundary(minBoundary);
				if (maxBoundarySet)
					result->setMaxBoundary(maxBoundary);
				finished = true;
			} // if
			else
				printd(
						WARNING,
						"SliderJointFactory::create(): WARNING: unknown end-element %s found!\n",
						xml->getNodeName());
			break;

		default:
			break;
		} // switch
	} // while

	if (!finished)
		printd(ERROR,
				"SliderJointFactory::create(): ERROR while loading SliderJoint object!\n");

	return result;
*/
} // create

//*****************************************************************************

Joint* UniversalJointFactory::create(std::string className, const XmlElement* element) {
	if (className != "universalJoint")
		return NULL;

//TODO
	printd(ERROR,
			"UniversalJointFactory::create(): NOT UPDATED TO NEW XML CONFIG FILE LOADING YET!\n");
	return NULL;

/*
	IrrXMLReader *xml = (IrrXMLReader*)args;
	UniversalJoint* result= NULL;
	bool finished = false;
	int anchorBody = 1;
	gmtl::Vec3f offset(0, 0, 0);
	gmtl::Vec3f axis1(0, 0, 0);
	gmtl::Vec3f axis2(0, 0, 0);
	gmtl::Vec3f anchorPoint(0, 0, 0);

	while (!finished && xml && xml->read()) {
		switch (xml->getNodeType()) {
		case EXN_ELEMENT:
			if (!strcmp("anchorBody", xml->getNodeName()))
				anchorBody = xml->getAttributeValueAsInt("number");
			else if (!strcmp("globalOffset", xml->getNodeName())) {
				offset[0] = xml->getAttributeValueAsFloat("x");
				offset[1] = xml->getAttributeValueAsFloat("y");
				offset[2] = xml->getAttributeValueAsFloat("z");
			} // else if
			else if (!strcmp("globalAxis1", xml->getNodeName())) {
				axis1[0] = xml->getAttributeValueAsFloat("x");
				axis1[1] = xml->getAttributeValueAsFloat("y");
				axis1[2] = xml->getAttributeValueAsFloat("z");
			} // else if
			else if (!strcmp("globalAxis2", xml->getNodeName())) {
				axis2[0] = xml->getAttributeValueAsFloat("x");
				axis2[1] = xml->getAttributeValueAsFloat("y");
				axis2[2] = xml->getAttributeValueAsFloat("z");
			} // else if
			else
				printd(
						WARNING,
						"UniversalJointFactory::create(): WARNING: unknown element %s found!\n",
						xml->getNodeName());
			break;

		case EXN_ELEMENT_END:
			if (!strcmp("universalJoint", xml->getNodeName())) {
				result = new UniversalJoint(anchorBody, offset, axis1, axis2);
				finished = true;
			} // if
			else
				printd(
						WARNING,
						"UniversalJointFactory::create(): WARNING: unknown end-element %s found!\n",
						xml->getNodeName());
			break;

		default:
			break;
		} // switch
	} // while

	if (!finished)
		printd(ERROR,
				"UniversalJointFactory::create(): ERROR while loading UniversalJoint object!\n");

	return result;
*/
} // create

//*****************************************************************************

Joint* Hinge2JointFactory::create(std::string className, const XmlElement* element) {
	if (className != "hinge2Joint")
		return NULL;

//TODO
	printd(ERROR,
			"Hinge2JointFactory::create(): NOT UPDATED TO NEW XML CONFIG FILE LOADING YET!\n");
	return NULL;

/*
	IrrXMLReader *xml = (IrrXMLReader*)args;
	Hinge2Joint* result= NULL;
	bool finished = false;
	int anchorBody = 1;
	gmtl::Vec3f offset(0, 0, 0);
	gmtl::Vec3f axis1(0, 0, 0);
	gmtl::Vec3f axis2(0, 0, 0);
	gmtl::Vec3f anchorPoint(0, 0, 0);

	while (!finished && xml && xml->read()) {
		switch (xml->getNodeType()) {
		case EXN_ELEMENT:
			if (!strcmp("anchorBody", xml->getNodeName()))
				anchorBody = xml->getAttributeValueAsInt("number");
			else if (!strcmp("globalOffset", xml->getNodeName())) {
				offset[0] = xml->getAttributeValueAsFloat("x");
				offset[1] = xml->getAttributeValueAsFloat("y");
				offset[2] = xml->getAttributeValueAsFloat("z");
			} // else if
			else if (!strcmp("globalAxis1", xml->getNodeName())) {
				axis1[0] = xml->getAttributeValueAsFloat("x");
				axis1[1] = xml->getAttributeValueAsFloat("y");
				axis1[2] = xml->getAttributeValueAsFloat("z");
			} // else if
			else if (!strcmp("globalAxis2", xml->getNodeName())) {
				axis2[0] = xml->getAttributeValueAsFloat("x");
				axis2[1] = xml->getAttributeValueAsFloat("y");
				axis2[2] = xml->getAttributeValueAsFloat("z");
			} // else if
			else
				printd(
						WARNING,
						"Hinge2JointFactory::create(): WARNING: unknown element %s found!\n",
						xml->getNodeName());
			break;

		case EXN_ELEMENT_END:
			if (!strcmp("hinge2Joint", xml->getNodeName())) {
				result = new Hinge2Joint(anchorBody, offset, axis1, axis2);
				finished = true;
			} // if
			else
				printd(
						WARNING,
						"Hinge2JointFactory::create(): WARNING: unknown end-element %s found!\n",
						xml->getNodeName());
			break;

		default:
			break;
		} // switch
	} // while


	if (!finished)
		printd(ERROR,
				"Hinge2JointFactory::create(): ERROR while loading Hinge2Joint object!\n");

	return result;
*/
} // create

//*****************************************************************************

Joint* FixedJointFactory::create(std::string className, const XmlElement* element) {
	if (className != "fixedJoint")
		return NULL;

//TODO
	printd(ERROR,
			"FixedJointFactory::create(): NOT UPDATED TO NEW XML CONFIG FILE LOADING YET!\n");
	return NULL;

/*
	// 	IrrXMLReader *xml = (IrrXMLReader*)args;
	FixedJoint* result = new FixedJoint();

	return result;
*/
} // create

//*****************************************************************************

Joint* AMotorJointFactory::create(std::string className, const XmlElement* element) {

	if (className != "aMotorJoint")
		return NULL;

//TODO
	printd(ERROR,
			"AMotorJointFactory::create(): NOT UPDATED TO NEW XML CONFIG FILE LOADING YET!\n");
	return NULL;

/*
	IrrXMLReader *xml = (IrrXMLReader*)args;
	AMotorJoint* result= NULL;
	bool finished = false;
	int anchorBody = 1;
	int axis1Obj=-1, axis2Obj=-1, axis3Obj=-1;
	gmtl::Vec3f axis1(0, 0, 0);
	gmtl::Vec3f axis2(0, 0, 0);
	gmtl::Vec3f axis3(0, 0, 0);
	int mode = -1;
	std::string modeString;

	while (!finished && xml && xml->read()) {
		switch (xml->getNodeType()) {
		case EXN_ELEMENT:
			if (!strcmp("aMotorMode", xml->getNodeName())) {
				modeString = xml->getAttributeValue("value");
				if (modeString == "dAMotorEuler")
					mode = dAMotorEuler;
				else
					mode = dAMotorUser;

				if (mode == dAMotorUser && modeString != "dAMotorUser")
					printd(
							WARNING,
							"AMotorJointFactory::create(): WARNING: unknown AMotor-Mode %s\n",
							modeString.c_str());
			} // if
			else if (!strcmp("anchorBody", xml->getNodeName()))
				anchorBody = xml->getAttributeValueAsInt("number");
			else if (!strcmp("globalAxis1", xml->getNodeName())) {
				axis1Obj = anchorBody;
				axis1[0] = xml->getAttributeValueAsFloat("x");
				axis1[1] = xml->getAttributeValueAsFloat("y");
				axis1[2] = xml->getAttributeValueAsFloat("z");
			} // else if
			else if (!strcmp("globalAxis2", xml->getNodeName())) {
				axis2Obj = anchorBody;
				axis2[0] = xml->getAttributeValueAsFloat("x");
				axis2[1] = xml->getAttributeValueAsFloat("y");
				axis2[2] = xml->getAttributeValueAsFloat("z");
			} // else if
			else if (!strcmp("globalAxis3", xml->getNodeName())) {
				axis3Obj = anchorBody;
				axis3[0] = xml->getAttributeValueAsFloat("x");
				axis3[1] = xml->getAttributeValueAsFloat("y");
				axis3[2] = xml->getAttributeValueAsFloat("z");
			} // else if
			else
				printd(
						WARNING,
						"AMotorJointFactory::create(): WARNING: unknown element %s found!\n",
						xml->getNodeName());
			break;

		case EXN_ELEMENT_END:
			if (!strcmp("aMotorJoint", xml->getNodeName())) {
				if (axis3Obj != -1) {
					if (mode != -1)
						result = new AMotorJoint(axis1Obj, axis1, axis2Obj, axis2, axis3Obj,
								axis3, mode);
					else
						result
								= new AMotorJoint(axis1Obj, axis1, axis2Obj, axis2, axis3Obj, axis3);
				} // if
				else if (axis2Obj != -1) {
					if (mode != -1)
						result = new AMotorJoint(axis1Obj, axis1, axis2Obj, axis2, mode);
					else
						result = new AMotorJoint(axis1Obj, axis1, axis2Obj, axis2);
				} // else if
				else
					result = new AMotorJoint(axis1Obj, axis1);
				finished = true;
			} // if
			else
				printd(
						WARNING,
						"AMotorJointFactory::create(): WARNING: unknown end-element %s found!\n",
						xml->getNodeName());
			break;

		default:
			break;
		} // switch
	} // while

	if (!finished)
		printd(ERROR,
				"AMotorJointFactory::create(): ERROR while loading AMotorJoint object!\n");

	return result;
*/
} // create

//*****************************************************************************

} // oops
