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

#ifndef _OPENSGTRANSFORMATIONWRITERFACTORY_H
#define _OPENSGTRANSFORMATIONWRITERFACTORY_H

#include <OpenSG/OSGTransform.h>
#include "Interfaces/TransformationWriterFactory.h"

namespace oops
{

class OpenSGTransformationWriterFactory : public TransformationWriterFactory
{
protected:
	std::map<std::string, OSG::TransformPtr> destinationMap;
	void addTransformationNodes(std::string groupName, OSG::NodePtr node);
public:
	virtual ~OpenSGTransformationWriterFactory();
	void addDestination(std::string name, OSG::TransformPtr core);
	void addDestinations(std::string groupName, OSG::NodePtr root);
	virtual TransformationWriterInterface* getTransformationWriter(std::string destination);
	
}; // OpenSGTransformationWriterFactory

} // oops

#endif // _OPENSGTRANSFORMATIONWRITERFACTORY_H
