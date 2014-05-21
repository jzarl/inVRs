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

#include "oops/OpenSGTransformationWriterFactory.h"
#include "oops/OpenSGTransformationWriter.h"

#include <OpenSG/OSGConfig.h>
#if OSG_MAJOR_VERSION >= 2
#include "OSGNameAttachment.h"
#include "OSGVoidPAttachment.h"
#else
#include <OpenSG/OSGSimpleAttachments.h>
#endif

namespace oops
{

OpenSGTransformationWriterFactory::~OpenSGTransformationWriterFactory()
{
	destinationMap.clear();
} // OpenSGTransformationWriterFactory

void OpenSGTransformationWriterFactory::addDestination(std::string name, OSG::TransformPtr core)
{
	destinationMap[name] = core;
} // addDestination

void OpenSGTransformationWriterFactory::addTransformationNodes(std::string groupName, OSG::NodePtr node)
{
	OSG::NodeCorePtr core;
	OSG::NodePtr result;
	OSG::TransformPtr trans;
	int i;
	int nChildren = node->getNChildren();
	std::string key;
	std::string temp;
	
	core = node->getCore();
	trans = OSG::TransformPtr::dcast(core);
	if (trans != OSG::NullFC && OSG::getName(node))
	{
		temp = OSG::getName(node);
		key = groupName + "." + temp;
		printf("Adding %s to destinationMap!\n", key.c_str());
		destinationMap[key] = trans;
	} // if

	for (i=0; i < nChildren; i++)
	{
		addTransformationNodes(groupName, node->getChild(i));
	} // for
} // addTransformationNodes

void OpenSGTransformationWriterFactory::addDestinations(std::string groupName, OSG::NodePtr root)
{
	addTransformationNodes(groupName, root);
} // addDestinations

TransformationWriterInterface* OpenSGTransformationWriterFactory::getTransformationWriter(std::string destination)
{
	OSG::TransformPtr core = destinationMap[destination];
	printf("Trying to find core to %s\n", destination.c_str());
	assert(core != OSG::NullFC);
	return new OpenSGTransformationWriter(core);
} // getTransformationWriter

} // oops
