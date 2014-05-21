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

#ifndef _PHYSICSENTITY_H
#define _PHYSICSENTITY_H

#include "PhysicsObjectInterface.h"
#include <inVRs/SystemCore/WorldDatabase/Entity.h>
#ifdef INVRS_BUILD_TIME
#include "oops/RigidBody.h"
#include "oops/Joints.h"
#else
#include <inVRs/Modules/3DPhysics/oops/RigidBody.h>
#include <inVRs/Modules/3DPhysics/oops/Joints.h>
#endif

class TransformationPipe;
class PhysicsObjectManager;

/** Physically simulated Entity.
 * This class represents an Entity which can be simulated by the Physics Module.
 * It therfore inherits from the Entity class and from the
 * PhysicsObjectInterface. The class does not implement the methods inherited
 * from the PhysicsObjectInterface so that different classes of PhysicsEntities
 * can be implemented. The class stores a pointer to the PhysicsObjectManager
 * which is used to distribute client input to the responsible server. It
 * furthermore has a pointer to the TransformationPipe in which the resulting
 * Entity Transformation is written.
 */
class PhysicsEntity : public Entity, public PhysicsObjectInterface
{
public:

//******************//
// PUBLIC METHODS: *//
//******************//

	/** Constructor initializes Entity and zeroes Pointers.
	 * @param id ID of the Entity
	 * @param environmentId ID of the Environment where the Entity is created
	 * @param instanceId ID for the instance this Entity is from the EntityType
	 * @param type EntityType this Entity is an instance of
	 */
	PhysicsEntity(unsigned short id, unsigned short environmentId,
		unsigned short instanceId, EntityType *type);

	/** Destructor closes TransformationPipe if open.
	 */
	virtual ~PhysicsEntity();

	/** Sets the TransformationPipe for Physics Entity.
	 * The Method sets the TransformationPipe in which the new Entity
	 * transformation can be written. This TransformationPipe will be used
	 * by the EntityTransformationWriter to write the simulation results.
	 * @param pipe TransformationPipe where to put the transformation
	 */
	virtual void setPhysicsTransformationPipe(TransformationPipe* pipe);

	/** Returns the TransformationPipe for Physics Entity.
	 * The method returns the TransformationPipe in which the Entity
	 * transformation will be written. This TransformationPipe will be used
	 * by the EntityTransformationWriter to write the simluation results.
	 * @return TransformationPipe of the Physics Entity
	 */
	virtual TransformationPipe* getPhysicsTransformationPipe();

	/** Stores a pointer to the PhysicsObjectManager.
	 * The method stores a pointer to the used PhysicsObjectManager. This is
	 * needed when a client input should be sent to the physics server. The
	 * method has to encode the method call and call the
	 * <code>handleClientInput</code> method of the PhysicsObjectManager.
	 */
	virtual void setPhysicsObjectManager(PhysicsObjectManager* objectManager);

//*******************************************************//
// PUBLIC METHODS INHERITED FROM: PhysicsObjectInterface //
//*******************************************************//

	virtual void getRigidBodies(std::vector<oops::RigidBody*>& rigidBodyList) = 0;
	virtual void getJoints(std::vector<oops::Joint*>& jointList) = 0;
	virtual PhysicsObjectID getPhysicsObjectID() = 0;

//*****************************//
// DEPRECATED PUBLIC METHODS: *//
//*****************************//

//	virtual bool addToSimulation(oops::Simulation* simulation) = 0;
//	virtual bool removeFromSimulation(oops::Simulation* simulation) = 0;

//	/** Handles incoming messages containing remote method calls.
//	 * The method receives an incoming message containing a remote method call.
//	 * First it decodes the type of the message. If the type is not a negative
//	 * number it is not handled by the PhysicsEntity-class itself and therfore
//	 * the message is passed to the
//	 * <code>handleMessage(unsigned type,OSG::NetMessage* msg)</code>
//	 * method which has to be implemented in the derived class. If the
//	 * method-type is a negative number it is proven which method call is
//	 * encoded by comparing the type with the <code>REMOTE_FUNCTIONS</code>
//	 * enumeration member. If a matching function is found it is called.
//	 * @param msg Message containing remote method call.
//	 * @return true if the method call could be executed.
//	 */
//	virtual bool handleMessage(NetMessage* msg);

protected:

	friend class CursorSpringConnector;
	friend class OopsPhysicsEntityTransformationWriter;
	friend class PhysicsEntityTransformationWriterModifier;

//********************//
// PROTECTED METHODS: //
//********************//

	/** Stores the Transformation of the physical representation.
	 * The method is called by the PhysicsEntityTransformation Writer which is
	 * registered at the physical representation of this PhysicsEntity. It
	 * stores the passed Transformation locally and passes it to the
	 * physicsTransformationPipe.
	 * @param trans Transformation of the physical representation
	 */
	virtual void setPhysicsTransformation(TransformationData trans);

	/** Sets the Transformation of the Entity.
	 * The method is used to update the Enitity-Transformation usually with the
	 * Transformation obtained from the physical representation. The method is
	 * called by the PhysicsEntityTransformationWriterModifier.
	 * @param trans new Transformation of the Entity.
	 */
	virtual void setEntityWorldTransformation(TransformationData trans);

	/** Opens a default PhysicsTransformationPipe.
	 * The method opens the default PhysicsTransformationPipe. The default pipe
	 * should only consist of one PhysicsEntityTransformationWriterModifier.
	 */
	virtual void openPhysicsTransformationPipe();

	/** Closes the current PhysicsTransformationPipe.
	 * The method closes the current PhysicsTransformationPipe if there is one
	 * opened. It no pipe is opened it prints a warning.
	 */
	virtual void closePhysicsTransformationPipe();

	/** Requests the PhysicsTransformationPipe from the TransformationManager
	 * The method tries to get the TransformationPipe from the
	 * TransformationManager if already present.
	 */
	virtual void requestPhysicsTransformationPipe();

	/** Returns the current Transformation set by the Physics simulation.
	 */
	virtual TransformationData getPhysicsTransformation();

//	/** DEPRECATED: Handles an incoming SynchroniseAllToClient message.
//	 *
//	 */
//	virtual void handleSyncAllToClientMsg(NetMessage* msg);
//
//	/** DEPRECATED: Handles an incoming SynchroniseAllToDRClient message.
//	 *
//	 */
//	virtual void handleSyncAllToDRClientMsg(NetMessage* msg);
//
//	/** DEPRECATED: Handles an incoming SynchroniseAllToServer message.
//	 *
//	 */
//	virtual void handleSyncAllToServerMsg(NetMessage* msg);
//
//	/** DEPRECATED: Handles an incoming SynchroniseChangesToClient message.
//	 *
//	 */
//	virtual void handleSyncChangesToClientMsg(NetMessage* msg);
//
//	/** DEPRECATED: Handles an incoming SynchroniseChangesToDRClient message.
//	 *
//	 */
//	virtual void handleSyncChangesToDRClientMsg(NetMessage* msg);
//
//	/** DEPRECATED: Handles an incoming SynchroniseChangesToServer message.
//	 *
//	 */
//	virtual void handleSyncChangesToServerMsg(NetMessage* msg);
//
//	/** DEPRECATED: Handles an incoming SynchroniseObjectTransformation message.
//	 * @param msg incomming message
//	 */
//	virtual void handleSyncObjectTransformationMsg(NetMessage* msg) = 0;
//
//	/** Handles a remote method call.
//	 * The method receives a message containing an remote method call identified
//	 * by the parameter <code>type</code>. The datatype unsigned is chosen
//	 * because negative method types are handled by the PhysicsEntity itself.
//	 * These are used for the <code>synchroniseXXX</code> methods and the
//	 * <code>handleSyncXXX</code> methods. When a more data has to be
//	 * synchronised these methods can be overloaded in derived classes.
//	 * @param function Method type
//	 * @param msg Message containing the method call
//	 * @return true if the remote method call could be executed
//	 */
//	virtual bool handleMessage(unsigned function, NetMessage* msg) = 0;

//**********************************************************//
// PROTECTED METHODS INHERITED FROM: PhysicsObjectInterface //
//**********************************************************//

//	virtual void synchroniseAllToClient(NetMessage* msg);
//	virtual void synchroniseAllToDRClient(NetMessage* msg);
//	virtual void synchroniseAllToServer(NetMessage* msg);
//	virtual void synchroniseChangesToClient(NetMessage* msg);
//	virtual void synchroniseChangesToDRClient(NetMessage* msg);
//	virtual void synchroniseChangesToServer(NetMessage* msg);
//	virtual void synchroniseObjectTransformation(NetMessage* msg);
//
//	virtual std::vector<RigidBody*>& getRigidBodiesForSync(std::vector<RigidBody*> &rigidBodyList);

//*********************//
// PROTECTED MEMBERS: *//
//*********************//

//	/// List of all synchronisation methods handled by the PhysicsEntity.
//	enum REMOTE_FUNCTIONS {
//		FUNCTION_syncAllToClient = -1,
//		FUNCTION_syncAllToDRClient = -2,
//		FUNCTION_syncAllToServer = -3,
//		FUNCTION_syncChangesToClient = -4,
//		FUNCTION_syncChangesToDRClient = -5,
//		FUNCTION_syncChangesToServer = -6,
//		FUNCTION_syncObjectTransformation = -7 };

	bool physicsPipeCreationInitiated;
	/// Pointer to the TransformationPipe of the PhysicsEntity
	TransformationPipe* physicsTransformationPipe;
	/// Pointer to the PhysicsObjectManager used by the Physics Module
	PhysicsObjectManager* objectManager;
	/// Transformation of the physical representation
	TransformationData physicsTransformation;

}; // PhysicsEntity

#endif // _PHYSICSENTITY_H
