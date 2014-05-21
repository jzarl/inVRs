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

#ifndef _ENTITYTYPEFACTORY_H
#define _ENTITYTYPEFACTORY_H

#include "EntityType.h"
#include "../ArgumentVector.h"
#include "../ClassFactory.h"
#include "../XMLTools.h"

/******************************************************************************
 * This class is used to create an object of the class-type EntityType. Whenever
 * you want to create new Types of Entities with more functions than the default
 * Entities you can create a new EntityType and a new EntityTypeFactory which
 * inherits from this class. The parseXML-method allows you to parse your own
 * elements in the configuration of the Entity-file. The method is used to parse
 * the elements under the <Implementation> element in the XML-File.
 */
class INVRS_SYSTEMCORE_API EntityTypeFactory : public ClassFactory<EntityType, unsigned short> {
public:
	/**
	 * Creates a new object of class type EntityType if the passed className
	 * is EntityType.
	 * @param className className of the desired object
	 * @param args arguments passed to the factory
	 * @return new EntityType object if matching to className, NULL otherwise
	 */
	virtual EntityType* create(std::string className, unsigned short id);

	/**
	 * Parses the implementationClass-section in the XML-configuration file.
	 * This can be used to parse EntityType specific information.
	 *
	 * @param entType EntityType that should be configured
	 * @param xml IrrXMLReader to read from
	 * @return true if no error occured
	 */
	virtual bool parseXML(EntityType* entType, const XmlElement* element);
};

#endif
