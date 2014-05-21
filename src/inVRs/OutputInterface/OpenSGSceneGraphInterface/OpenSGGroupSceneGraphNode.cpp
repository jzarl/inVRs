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

#include <assert.h>

#include <gmtl/MatrixOps.h>
#include <gmtl/Xforms.h>
#include <gmtl/External/OpenSGConvert.h>

#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGTransform.h>
#include <OpenSG/OSGSharePtrGraphOp.h>
#include <OpenSG/OSGSimpleSceneManager.h>
#include <OpenSG/OSGSceneFileHandler.h>
#include <OpenSG/OSGSharePtrGraphOp.h>
#if OSG_MAJOR_VERSION < 2
#include <OpenSG/OSGSimpleAttachments.h>
#endif

#include "OpenSGGroupSceneGraphNode.h"
#include "../../SystemCore/DebugOutput.h"
#include "../../SystemCore/WorldDatabase/WorldDatabase.h"
#include "../../SystemCore/XMLTools.h"


#if OSG_MAJOR_VERSION >= 2
OpenSGGroupSceneGraphNode::OpenSGGroupSceneGraphNode(OSG::NodeRecPtr& node) 
#else //OpenSG1:
OpenSGGroupSceneGraphNode::OpenSGGroupSceneGraphNode(OSG::NodePtr& node) 
#endif
	: OpenSGSceneGraphNode(node) 
{

	this->nodeType = SceneGraphNodeInterface::GROUP_NODE;
} // OpenSGGroupSceneGraphNode

OpenSGGroupSceneGraphNode::~OpenSGGroupSceneGraphNode() {
} // ~OpenSGGeometrySceneGraphNode
