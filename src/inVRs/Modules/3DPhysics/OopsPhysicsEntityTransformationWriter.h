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

#ifndef _OOPSPHYSICSENTITYTRANSFORMATIONWRITER_H
#define _OOPSPHYSICSENTITYTRANSFORMATIONWRITER_H

#include "PhysicsEntity.h"
#ifdef INVRS_BUILD_TIME
#include "oops/Interfaces/TransformationWriterInterface.h"
#else
#include <inVRs/Modules/3DPhysics/oops/Interfaces/TransformationWriterInterface.h>
#endif

//using namespace oops;

/** Class to write RigidBody-Transformation to PhysicsEntity.
 * The class is used to write the Transformation of an oops::RigidBody class to
 * a PhysicsEntity. It calls the PhysicsEntity::setPhysicsTransformation method.
 */
class OopsPhysicsEntityTransformationWriter : public oops::TransformationWriterInterface
{
public:

//*****************//
// PUBLIC METHODS: //
//*****************//

	/** Constructor sets the destination PhysicsEntity.
	 * The constructor initializes the offset-Transformation as identity-
	 * Transformation and sets the PhysicsEntity where to write the
	 * Transformation to.
	 * @param entity destination Entity for Transformation
	 */
	OopsPhysicsEntityTransformationWriter(PhysicsEntity* entity);

//********************************************************************//
// PUBLIC METHODS INHERITED FROM: oops::TransformationWriterInterface //
//********************************************************************//

	virtual void updateTransformation(TransformationData trans);
	virtual void setTransformationOffset(TransformationData trans, TransformationData offset);

protected:

//********************//
// PROTECTED MEMBERS: //
//********************//
	/// offset to get from RigidBody to PhysicsEntity Transformation
	TransformationData localOffset;
	/// PhysicsEntity where to write the Transformation to
	PhysicsEntity* physicsEntity;

}; // OopsPhysicsEntityTransformationWriter

#endif // _OOPSPHYSICSENTITYTRANSFORMATIONWRITER_H
