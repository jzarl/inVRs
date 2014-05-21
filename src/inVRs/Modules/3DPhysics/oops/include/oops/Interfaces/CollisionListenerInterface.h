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

#ifndef _COLLISIONLISTENERINTERFACE_H
#define _COLLISIONLISTENERINTERFACE_H

#include "../Simulation.h"

namespace oops
{

class RigidBody;

/** Interface for collision listener classes.
 * This interface can be used to implement classes which can listen on
 * collisions of a RigidBody. The class has to be registered at the RigidBody
 * it wants to listen to.
 */
class CollisionListenerInterface
{
public:

	/** Empty destructor.
	 */
	virtual ~CollisionListenerInterface(){};

	/** Callback-method when a collision occurs.
	 * This method is called from the RigidBody where the listener is registered
	 * when it collides.
	 * @param body1 The RigidBody at which the listener is registered.
	 * @param body2 The RigidBody with which the first RigidBody collides
	 */
	virtual void notifyCollision(RigidBody* body1, RigidBody* body2, std::vector<ContactData>& contacts) = 0;

}; // CollisionListenerInterface

} // oops

#endif // _COLLISIONLISTENERINTERFACE_H
