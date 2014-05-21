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

#ifndef _SYSTEMTHREADLISTENERINTERFACE_H
#define _SYSTEMTHREADLISTENERINTERFACE_H

class Physics;

/** Interface for SystemThreadListener in Physics Module
 * This interface can be used to implement an object which listenes on the
 * <code>update</code> method call of the Physics Module. This method is
 * called from the system thread. An example for this Interface would be to
 * update the transformation from an physics object to the Entity transformation
 * which will be used for rendering afterwards. NOTE: since this update-call
 * is executed from the System Thread you have to take care to Lock data
 * which is also accessible out of the Physics Thread!!! The registration of the
 * Listener also has to take place from the System Thread. An opportunity for
 * registering the listener would be in the constructor of the object which
 * implements this Interface. Doing so allows the objects to register themself
 * automatically on creation. Take care that when doing so it can happen that
 * the System Thread callback occurs before the object was actually added to
 * the physics simulation!!!
 */
class SystemThreadListenerInterface
{
public:

	/** Empty destructor.
	 */
	virtual ~SystemThreadListenerInterface(){};

protected:
	friend class Physics;

//*********************//
// PROTECTED METHODS: *//
//*********************//

	/** Callback function when Physics::update is called.
	 * The method is called from the System Thread when the Physics::update
	 * method is called. It allows to update data which is needed by the
	 * System Thread, for example the transformation of an object. The method
	 * call runs parallel to the Physics Thread so take care to Lock data
	 * which can be accessible from both Threads!
	 * @param dt elapsed time since last call in seconds
	 */
	virtual void systemUpdate(float dt) = 0;
}; // SystemThreadListenerInterface

#endif // _SYSTEMTHREADLISTENERINTERFACE_H
