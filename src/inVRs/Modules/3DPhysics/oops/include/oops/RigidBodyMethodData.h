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

#ifndef _RIGIDBODYMETHODDATA_H_
#define _RIGIDBODYMETHODDATA_H_

#include "Simulation.h"
#include <inVRs/SystemCore/NetMessage.h>
#include <inVRs/SystemCore/MessageFunctions.h>

namespace oops {

enum RIGIDBODY_METHODTYPE {
	RB_UNKNOWN = 0,
	RB_SETACTIVE,
	RB_SETVISIBLE,
	RB_SETTRANSFORMATION,
	RB_SETMASS,
	RB_ADDFORCE,
	RB_ADDTORQUE,
	RB_ADDFORCEATPOSITION,
	RB_SETFORCE,
	RB_SETTORQUE,
	RB_SETSTATICFORCE,
	RB_SETSTATICTORQUE,
	RB_SETLINEARVELOCITY,
	RB_SETANGULARVELOCITY,
	RB_SETFIXED,
	RB_SETGRAVITYMODE,
//	RB_COLLIDEWITHRIGIDBODY,
//	RB_DONTCOLLIDEWITHRIGIDBODY
};

//class MP_collideWithRigidBody : public MethodParameter {
//public:
//	uint64_t rigidBodyID;
//
//	MP_collideWithRigidBody(RigidBody* obj) {
//		this->rigidBodyID = obj->getID();
//	} // MP_collideWithRigidBody
//
//	MP_collideWithRigidBody(NetMessage* msg) {
//		msgFunctions::decode(rigidBodyID, msg);
//	} // MP_collideWithRigidBody
//
//	virtual void encode(NetMessage* msg) {
//		msgFunctions::encode(rigidBodyID, msg);
//	} // encode
//};
//
//typedef MP_collideWithRigidBody MP_dontCollideWithRigidBody;

class MP_setActive : public MethodParameter {
public:
	bool active;

	MP_setActive(bool active) {
		this->active = active;
	}

	MP_setActive(NetMessage* msg) {
		msgFunctions::decode(active, msg);
	} // MP_setActive

	virtual void encode(NetMessage* msg) {
		msgFunctions::encode(active, msg);
	} // encode

};

class MP_setVisible : public MethodParameter {
public:
	bool visible;

	MP_setVisible(bool visible) {
		this->visible = visible;
	} // MP_setVisible

	MP_setVisible(NetMessage* msg) {
		msgFunctions::decode(visible, msg);
	} // MP_setVisible

	virtual void encode(NetMessage* msg) {
		msgFunctions::encode(visible, msg);
	} // encode
};

class MP_setTransformation : public MethodParameter {
public:
	TransformationData trans;
	bool forwardToWriter;

	MP_setTransformation(TransformationData& trans, bool forwardToWriter) {
		this->trans = trans;
		this->forwardToWriter = forwardToWriter;
	} //MP_setTransformation

	MP_setTransformation(NetMessage* msg) {
		msgFunctions::decode(trans, msg);
		msgFunctions::decode(forwardToWriter, msg);
	} // MP_setTransformation

	virtual void encode(NetMessage* msg) {
		msgFunctions::encode(trans, msg);
		msgFunctions::encode(forwardToWriter, msg);
	} // encode
};

class MP_setMass : public MethodParameter {
public:
	float mass;

	MP_setMass(float mass) {
		this->mass = mass;
	} // MP_setMass

	MP_setMass(NetMessage* msg) {
		msgFunctions::decode(mass, msg);
	} // MP_setMass

	virtual void encode(NetMessage* msg) {
		msgFunctions::encode(mass, msg);
	} // encode
};

class MP_addForce : public MethodParameter {
public:
	gmtl::Vec3f force;
	bool relative;

	MP_addForce(gmtl::Vec3f& force, bool relative) {
		this->force = force;
		this->relative = relative;
	} // MP_addForce

	MP_addForce(NetMessage* msg) {
		msgFunctions::decode(force, msg);
		msgFunctions::decode(relative, msg);
	} // MP_addForce

	virtual void encode(NetMessage* msg) {
		msgFunctions::encode(force, msg);
		msgFunctions::encode(relative, msg);
	} // encode
};

class MP_addTorque : public MethodParameter {
public:
	gmtl::Vec3f torque;
	bool relative;

	MP_addTorque(gmtl::Vec3f& torque, bool relative) {
		this->torque = torque;
		this->relative = relative;
	} // MP_addTorque

	MP_addTorque(NetMessage* msg) {
		msgFunctions::decode(torque, msg);
		msgFunctions::decode(relative, msg);
	} // MP_addTorque

	virtual void encode(NetMessage* msg) {
		msgFunctions::encode(torque, msg);
		msgFunctions::encode(relative, msg);
	} // encode
};

class MP_addForceAtPosition : public MethodParameter {
public:
	gmtl::Vec3f force, position;
	bool relativeForce, relativePosition;

	MP_addForceAtPosition(gmtl::Vec3f& force, gmtl::Vec3f& position, bool relativeForce, bool relativePosition) {
		this->force = force;
		this->position = position;
		this->relativeForce = relativeForce;
		this->relativePosition = relativePosition;
	} // MP_addForceAtPosition

	MP_addForceAtPosition(NetMessage* msg) {
		msgFunctions::decode(force, msg);
		msgFunctions::decode(position, msg);
		msgFunctions::decode(relativeForce, msg);
		msgFunctions::decode(relativePosition, msg);
	} // MP_addForceAtPosition

	virtual void encode(NetMessage* msg) {
		msgFunctions::encode(force, msg);
		msgFunctions::encode(position, msg);
		msgFunctions::encode(relativeForce, msg);
		msgFunctions::encode(relativePosition, msg);
	} // encode
};

class MP_setForce : public MethodParameter {
public:
	gmtl::Vec3f force;

	MP_setForce(gmtl::Vec3f& force) {
		this->force = force;
	} // MP_setForce

	MP_setForce(NetMessage* msg) {
		msgFunctions::decode(force, msg);
	} // MP_setForce

	virtual void encode(NetMessage* msg) {
		msgFunctions::encode(force, msg);
	} // encode
}; // MP_setForce


class MP_setTorque : public MethodParameter {
public:
	gmtl::Vec3f torque;

	MP_setTorque(gmtl::Vec3f& torque) {
		this->torque = torque;
	} // MP_setTorque

	MP_setTorque(NetMessage* msg) {
		msgFunctions::decode(torque, msg);
	} // MP_setTorque

	virtual void encode(NetMessage* msg) {
		msgFunctions::encode(torque, msg);
	} // encode
};

typedef MP_setForce MP_setStaticForce;

typedef MP_setTorque MP_setStaticTorque;

class MP_setLinearVelocity : public MethodParameter {
public:
	gmtl::Vec3f velocity;
	bool relative;

	MP_setLinearVelocity(gmtl::Vec3f velocity, bool relative) {
		this->velocity = velocity;
		this->relative = relative;
	} // MP_setLinearVelocity

	MP_setLinearVelocity(NetMessage* msg) {
		msgFunctions::decode(velocity, msg);
		msgFunctions::decode(relative, msg);
	} // MP_setLinearVelocity

	virtual void encode(NetMessage* msg) {
		msgFunctions::encode(velocity, msg);
		msgFunctions::encode(relative, msg);
	} // encode
};

typedef MP_setLinearVelocity MP_setAngularVelocity;

class MP_setFixed : public MethodParameter {
public:
	bool fixed;

	MP_setFixed(bool fixed) {
		this->fixed = fixed;
	} // MP_setFixed

	MP_setFixed(NetMessage* msg) {
		msgFunctions::decode(fixed, msg);
	} // MP_setFixed

	virtual void encode(NetMessage* msg) {
		msgFunctions::encode(fixed, msg);
	} // encode
};

class MP_setGravityMode : public MethodParameter {
public:
	bool onOff;

	MP_setGravityMode(bool onOff) {
		this->onOff = onOff;
	} // MP_setGravityMode

	MP_setGravityMode(NetMessage* msg) {
		msgFunctions::decode(onOff, msg);
	} // MP_setGravityMode

	virtual void encode(NetMessage* msg) {
		msgFunctions::encode(onOff, msg);
	} // encode
};

static void callMethod(RigidBody* rigidBody, RIGIDBODY_METHODTYPE method, NetMessage* msg, bool notifyListener=true) {
	switch (method) {

		case RB_SETACTIVE: {
			MP_setActive pSetActive(msg);
			if (notifyListener)
				rigidBody->setActive(pSetActive.active);
			else
				rigidBody->setActive_unchecked(pSetActive.active);
		}
		break;

		case RB_SETVISIBLE: {
			MP_setVisible param(msg);
			if (notifyListener)
				rigidBody->setVisible(param.visible);
			else
				rigidBody->setVisible_unchecked(param.visible);
		}
		break;

		case RB_SETTRANSFORMATION: {
			MP_setTransformation param(msg);
			if (notifyListener)
				rigidBody->setTransformation(param.trans, param.forwardToWriter);
			else
				rigidBody->setTransformation_unchecked(param.trans, param.forwardToWriter);
		}
		break;

		case RB_SETMASS: {
			MP_setMass param(msg);
			if (notifyListener)
				rigidBody->setMass(param.mass);
			else
				rigidBody->setMass_unchecked(param.mass);
		}
		break;

		case RB_ADDFORCE: {
			MP_addForce param(msg);
			if (notifyListener)
				rigidBody->addForce(param.force, param.relative);
			else
				rigidBody->addForce_unchecked(param.force, param.relative);
		}
		break;

		case RB_ADDTORQUE: {
			MP_addTorque param(msg);
			if (notifyListener)
				rigidBody->addTorque(param.torque, param.relative);
			else
				rigidBody->addTorque_unchecked(param.torque, param.relative);
		}
		break;

		case RB_ADDFORCEATPOSITION: {
			MP_addForceAtPosition param(msg);
			if (notifyListener)
				rigidBody->addForceAtPosition(param.force, param.position, param.relativeForce, param.relativePosition);
			else
				rigidBody->addForceAtPosition_unchecked(param.force, param.position, param.relativeForce, param.relativePosition);
		}
		break;

		case RB_SETFORCE: {
			MP_setForce param(msg);
			if (notifyListener)
				rigidBody->setForce(param.force);
			else
				rigidBody->setForce_unchecked(param.force);
		}
		break;

		case RB_SETTORQUE: {
			MP_setTorque param(msg);
			if (notifyListener)
				rigidBody->setTorque(param.torque);
			else
				rigidBody->setTorque_unchecked(param.torque);
		}
		break;

		case RB_SETSTATICFORCE: {
			MP_setStaticForce param(msg);
			if (notifyListener)
				rigidBody->setStaticForce(param.force);
			else
				rigidBody->setStaticForce_unchecked(param.force);
		}
		break;

		case RB_SETSTATICTORQUE: {
			MP_setStaticTorque param(msg);
			if (notifyListener)
				rigidBody->setStaticTorque(param.torque);
			else
				rigidBody->setStaticTorque_unchecked(param.torque);
		}
		break;

		case RB_SETLINEARVELOCITY: {
			MP_setLinearVelocity param(msg);
			if (notifyListener)
				rigidBody->setLinearVelocity(param.velocity, param.relative);
			else
				rigidBody->setLinearVelocity_unchecked(param.velocity, param.relative);
		}
		break;

		case RB_SETANGULARVELOCITY: {
			MP_setAngularVelocity param(msg);
			if (notifyListener)
				rigidBody->setAngularVelocity(param.velocity, param.relative);
			else
				rigidBody->setAngularVelocity_unchecked(param.velocity, param.relative);
		}
		break;

		case RB_SETFIXED: {
			MP_setFixed param(msg);
			if (notifyListener)
				rigidBody->setFixed(param.fixed);
			else
				rigidBody->setFixed_unchecked(param.fixed);
		}
		break;

		case RB_SETGRAVITYMODE: {
			MP_setGravityMode param(msg);
			if (notifyListener)
				rigidBody->setGravityMode(param.onOff);
			else
				rigidBody->setGravityMode_unchecked(param.onOff);
		}
		break;

// NOT IMPLEMENTED YET (MAYBE NEVER WILL!!!)
//		case RB_COLLIDEWITHRIGIDBODY: {
//			MP_collideWithRigidBody param(msg);
//			// TODO: { How to get RigidBody from ID
//			rigidBody->collideWithRigidBody()
//		}
//		break;
//
//		case RB_DONTCOLLIDEWITHRIGIDBODY: {
//		}
//		break;

		default: {
			fprintf(stderr, "callMethod(): ERROR: unknown method type %u found!\n", method);
		}
		break;

	} // switch
} // callMethod

class Dummy {
public:
	Dummy() {
		callMethod(0, RB_UNKNOWN, 0, false);
	}
};

} // oops

#endif /*_RIGIDBODYMETHODDATA_H_*/
