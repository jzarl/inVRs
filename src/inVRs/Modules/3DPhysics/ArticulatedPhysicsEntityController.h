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

#ifndef _ARTICULATEDPHYSICSENTITYCONTROLLER_H
#define _ARTICULATEDPHYSICSENTITYCONTROLLER_H

#include "ArticulatedPhysicsEntity.h"

/** Controller-class for ArticulatedPhysicsEntities.
 * This class can be used to implement Controller-classes for
 * ArticulatedPhysicsEntities. It provides access to ArticulatedBodies
 * of a passed ArticulatedPhysicsEntity.
 */
class ArticulatedPhysicsEntityController
{
public:

//*****************//
// PUBLIC METHODS: //
//*****************//

	/** Empty Constructor.
	 */
	ArticulatedPhysicsEntityController();

	/** Empty Destructor.
	 */
	virtual ~ArticulatedPhysicsEntityController();

protected:

//********************//
// PROTECTED METHODS: //
//********************//

	/** Provides access to the ArticulatedBody of an ArticulatedPhysicsEntity.
	 * @param entity pointer to the ArticulatedPhysicsEntity
	 * @return ArticulatedBody of the passed Entity
	 */
	virtual oops::ArticulatedBody* getArticulatedBody(ArticulatedPhysicsEntity* entity);

}; // ArticulatedPhysicsEntityController

#endif // _ARTICULATEDPHYSICSENTITYCONTROLLER_H
