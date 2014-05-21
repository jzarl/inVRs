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

#ifndef _ARTICULATEDBODYFACTORY_H
#define _ARTICULATEDBODYFACTORY_H

#include <string>
#include <irrXML.h>
#include <inVRs/SystemCore/DataTypes.h>

#include "Interfaces/ArticulatedBodyFactory.h"

using namespace irr;
using namespace io;

namespace oops
{

class ArticulatedBody;
class XMLLoader;

class DefaultArticulatedBodyFactory : public ArticulatedBodyFactory
{
public:
	DefaultArticulatedBodyFactory(XMLLoader* xmlLoader);
	virtual ~DefaultArticulatedBodyFactory();
	virtual ArticulatedBody* create(std::string className, const XmlElement* element);

protected:
//	bool parseSubBodies(IrrXMLReader* xml, ArticulatedBody* artBody);
//	bool parseJoints(IrrXMLReader* xml, ArticulatedBody* artBody);
//	bool parseCollisionRules(IrrXMLReader* xml, ArticulatedBody* artBody);
//
//	ArticulatedBody* loadArticulatedBody(IrrXMLReader* xml);

	XMLLoader* xmlLoader;
}; // DefaultArticulatedBodyFactory

} // oops

#endif // _ARTICULATEDBODYFACTORY_H
