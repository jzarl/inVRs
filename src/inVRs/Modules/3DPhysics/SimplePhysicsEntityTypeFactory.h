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

#ifndef _SIMPLEPHYSICSENTITYTYPEFACTORY_H
#define _SIMPLEPHYSICSENTITYTYPEFACTORY_H

#include <inVRs/SystemCore/WorldDatabase/EntityTypeFactory.h>
#include "SimplePhysicsEntityType.h"

#ifdef INVRS_BUILD_TIME
#include "oops/XMLLoader.h"
#else
#include <inVRs/Modules/3DPhysics/oops/XMLLoader.h>
#endif

/** Factory class for SimplePhysicsEntityType.
 * This class is used as factory for the SimplePhysicsEntityType class. It
 * creates a new SimplePhysicsEntityType instance and configures it in the
 * parseXML-method.
 */
class SimplePhysicsEntityTypeFactory: public EntityTypeFactory
{
public:
	SimplePhysicsEntityTypeFactory(oops::XMLLoader* xmlLoader);
//**************************************************//
// PUBLIC METHODS INHERITED FROM: EntityTypeFactory //
//**************************************************//

	virtual EntityType* create(std::string className, unsigned short id);
	virtual bool parseXML(EntityType* entType, const XmlElement* element);

protected:
	oops::XMLLoader* xmlLoader;
};

#endif // _SIMPLEPHYSICSENTITYTYPEFACTORY_H
