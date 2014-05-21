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

#ifndef OPENSGGEOMETRYSCENEGRAPHNODE_H_
#define OPENSGGEOMETRYSCENEGRAPHNODE_H_

#include <map>
#include <vector>

#include <OpenSG/OSGTransform.h>	// for TransformPtr
#include <OpenSG/OSGNode.h>
#include <OpenSG/OSGVector.h>
#include <OpenSG/OSGSharePtrGraphOp.h>
#include <OpenSG/OSGGraphOpSeq.h>

#include "../../SystemCore/DataTypes.h"
#include "../../SystemCore/ClassFactory.h"
#include "../SceneGraphInterface.h"
#include "OpenSGSceneGraphNode.h"

/**
 *
 */
class INVRS_OPENSGSCENEGRAPHINTERFACE_API OpenSGGeometrySceneGraphNode : public OpenSGSceneGraphNode,
		public GeometrySceneGraphNodeInterface {
public:
#if OSG_MAJOR_VERSION >= 2
	OpenSGGeometrySceneGraphNode(OSG::NodeRecPtr& nodePtr);
#else //OpenSG1:
	OpenSGGeometrySceneGraphNode(OSG::NodePtr& nodePtr);
#endif
	~OpenSGGeometrySceneGraphNode();
}; // OpenSGGeometrySceneGraphNode

#endif
