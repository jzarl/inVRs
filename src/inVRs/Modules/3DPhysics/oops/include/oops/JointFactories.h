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

#ifndef _JOINTFACTORIES_H
#define _JOINTFACTORIES_H

#include "Joints.h"
#include <inVRs/SystemCore/ClassFactory.h>
#include <inVRs/SystemCore/XmlElement.h>

namespace oops
{

typedef ClassFactory<Joint, const XmlElement*> JointFactory;

//*****************************************************************************

class HingeJointFactory : public JointFactory
{
public:
	virtual ~HingeJointFactory(){};
	virtual Joint* create(std::string className, const XmlElement* element);
}; // HingeJointFactory

//*****************************************************************************

class BallJointFactory : public JointFactory
{
public:
	virtual ~BallJointFactory(){};
	virtual Joint* create(std::string className, const XmlElement* element);
}; // BallJointFactory

//*****************************************************************************

class SliderJointFactory : public JointFactory
{
public:
	virtual ~SliderJointFactory(){};
	virtual Joint* create(std::string className, const XmlElement* element);
}; // SliderJointFactory

//*****************************************************************************

class UniversalJointFactory : public JointFactory
{
public:
	virtual ~UniversalJointFactory(){};
	virtual Joint* create(std::string className, const XmlElement* element);
}; // UniversalJointFactory

//*****************************************************************************

class Hinge2JointFactory : public JointFactory
{
public:
	virtual ~Hinge2JointFactory(){};
	virtual Joint* create(std::string className, const XmlElement* element);
}; // Hinge2JointFactory

//*****************************************************************************

class FixedJointFactory : public JointFactory
{
public:
	virtual ~FixedJointFactory(){};
	virtual Joint* create(std::string className, const XmlElement* element);
}; // FixedJointFactory

//*****************************************************************************

class AMotorJointFactory : public JointFactory
{
public:
	virtual ~AMotorJointFactory(){};
	virtual Joint* create(std::string className, const XmlElement* element);
}; // AMotorJointFactory

//*****************************************************************************

} // oops

#endif // _JOINTFACTORIES_H
