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

#ifndef _SIMPLEPHYSICSENTITYCONTROLLER_H
#define _SIMPLEPHYSICSENTITYCONTROLLER_H

#include "SimplePhysicsEntity.h"

/** Controller-class for SimplePhysicsEntities.
 * The class can be used to implement Controller-classes for
 * SimplePhysicsEntities. It provides access to the RigidBody
 * of a passed SimplePhysicsEntity.
 */
class SimplePhysicsEntityController
{
public:

//*****************//
// PUBLIC METHODS: //
//*****************//

	/** Empty Constructor.
	 */
	SimplePhysicsEntityController();

	/** Empty Destructor.
	 */
	virtual ~SimplePhysicsEntityController();

protected:

//********************//
// PROTECTED METHODS: //
//********************//

	/** Provides access to the RigidBody of an SimplePhysicsEntity.
	 * @param entity pointer to the SimplePhysicsEntity
	 * @return RigidBody of the passed Entity
	 */
	virtual oops::RigidBody* getRigidBody(SimplePhysicsEntity* entity);

}; // SimplePhysicsEntityController

#endif // _SIMPLEPHYSICSENTITYCONTROLLER_H
