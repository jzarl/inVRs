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

#ifndef OPENSGTRANSFORMATIONSCENEGRAPHNODE_H_
#define OPENSGTRANSFORMATIONSCENEGRAPHNODE_H_

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
class INVRS_OPENSGSCENEGRAPHINTERFACE_API OpenSGTransformationSceneGraphNode : public OpenSGSceneGraphNode,
		public TransformationSceneGraphNodeInterface {
public:
#if OSG_MAJOR_VERSION >= 2
	OpenSGTransformationSceneGraphNode(OSG::NodeRecPtr& nodePtr);
#else //OpenSG1:
	OpenSGTransformationSceneGraphNode(OSG::NodePtr& nodePtr);
#endif
	~OpenSGTransformationSceneGraphNode();

	void setTransformation(TransformationData trans);
	void setTransformationMatrix(gmtl::Matrix44f trans);

	TransformationData getTransformation();
	gmtl::Matrix44f getTransformationMatrix();

protected:
#if OSG_MAJOR_VERSION >= 2
	OSG::TransformRecPtr transformCore;
#else //OpenSG1:
	OSG::TransformPtr transformCore;
#endif
}; // OpenSGTransformationSceneGraphNode

#endif
