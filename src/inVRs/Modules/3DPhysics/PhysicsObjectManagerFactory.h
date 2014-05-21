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

#ifndef _PHYSICSOBJECTMANAGERFACTORY_H
#define _PHYSICSOBJECTMANAGERFACTORY_H

#include "PhysicsObjectManager.h"
#include <inVRs/SystemCore/ClassFactory.h>
#include <inVRs/SystemCore/ArgumentVector.h>

/** Class template for PhysicsObjectManager Factory classes.
 * This class is a template which defines the class structure of a factory
 * class which creates PhysicsObjectManagers. Every PhysicsObjectManager
 * has to have a factory which creates an instance of it. Therefore a factory
 * class has to be implemented which derives from this class.
 */
class PhysicsObjectManagerFactory : public ClassFactory<PhysicsObjectManager, ArgumentVector*>
{
public:

//***********************************************//
// PUBLIC METHODS INHERITED FROM: ClassFactory: *//
//***********************************************//

	PhysicsObjectManager* create(std::string className, ArgumentVector* args) = 0;

}; // PhysicsObjectManagerFactory

#endif // _PHYSICSOBJECTMANAGERFACTORY_H
