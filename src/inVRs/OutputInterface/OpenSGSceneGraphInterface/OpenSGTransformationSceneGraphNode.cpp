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

#include "OpenSGTransformationSceneGraphNode.h"
#include "../../SystemCore/DebugOutput.h"
#include "../../SystemCore/WorldDatabase/WorldDatabase.h"
#include "../../SystemCore/XMLTools.h"


OSG_USING_NAMESPACE

#if OSG_MAJOR_VERSION >= 2
OpenSGTransformationSceneGraphNode::OpenSGTransformationSceneGraphNode(OSG::NodeRecPtr& node) 
#else //OpenSG1:
OpenSGTransformationSceneGraphNode::OpenSGTransformationSceneGraphNode(OSG::NodePtr& node) 
#endif
	: OpenSGSceneGraphNode(node) 
{

	this->nodeType = SceneGraphNodeInterface::TRANSFORMATION_NODE;
#if OSG_MAJOR_VERSION >= 2
	NodeCoreRecPtr nc = node->getCore();
	transformCore = dynamic_pointer_cast<Transform>( nc );
#else //OpenSG1:
	transformCore = TransformPtr::dcast(node->getCore());
#endif
} // OpenSGTransformationSceneGraphNode

OpenSGTransformationSceneGraphNode::~OpenSGTransformationSceneGraphNode() {
} // OpenSGTransformationSceneGraphNode

void OpenSGTransformationSceneGraphNode::setTransformation(TransformationData trans) {
	gmtl::Matrix44f matrix;
	transformationDataToMatrix(trans, matrix);
	setTransformationMatrix(matrix);
} // setTransformation

void OpenSGTransformationSceneGraphNode::setTransformationMatrix(gmtl::Matrix44f trans) {
	OSG::Matrix matrix;

	gmtl::set(matrix, trans);

#if OSG_MAJOR_VERSION >= 2
	transformCore->setMatrix(matrix);
#else //OpenSG1:
	beginEditCP(transformCore, Transform::MatrixFieldMask);
	transformCore->setMatrix(matrix);
	endEditCP(transformCore, Transform::MatrixFieldMask);
#endif
} // setTransformationMatrix

TransformationData OpenSGTransformationSceneGraphNode::getTransformation() {
	TransformationData result;
	gmtl::Matrix44f matrix = getTransformationMatrix();
	matrixToTransformationData(matrix, result);
	return result;
} // getTransformation

gmtl::Matrix44f OpenSGTransformationSceneGraphNode::getTransformationMatrix() {
	gmtl::Matrix44f result;
	OSG::Matrix matrix;

#if OSG_MAJOR_VERSION >= 2
	matrix = transformCore->getMatrix();
#else //OpenSG1:
	beginEditCP(transformCore, Transform::MatrixFieldMask);
	matrix = transformCore->getMatrix();
	endEditCP(transformCore, Transform::MatrixFieldMask);
#endif

	gmtl::set(result, matrix);
	return result;
}
