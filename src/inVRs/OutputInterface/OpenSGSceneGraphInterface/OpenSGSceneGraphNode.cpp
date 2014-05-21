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

#include "OpenSGSceneGraphNode.h"

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

#include "../../SystemCore/DebugOutput.h"
#include "../../SystemCore/WorldDatabase/WorldDatabase.h"
#include "../../SystemCore/XMLTools.h"
#include "OpenSGTransformationSceneGraphNode.h"
#include "OpenSGGeometrySceneGraphNode.h"
#include "OpenSGGroupSceneGraphNode.h"

OSG_USING_NAMESPACE

OpenSGSceneGraphNode::OpenSGSceneGraphNode() {
}

#if OSG_MAJOR_VERSION >= 2
OpenSGSceneGraphNode::OpenSGSceneGraphNode(OSG::NodeRecPtr& node)
#else //OpenSG1:
OpenSGSceneGraphNode::OpenSGSceneGraphNode(OSG::NodePtr& node)
#endif
{
	this->node = node;
	this->nodeType = SceneGraphNodeInterface::UNDEFINED_NODE;

#if OSG_MAJOR_VERSION >= 2
#else //OpenSG1:
	if (node != NullFC)
		addRefCP(node);
#endif
} // OpenSGSceneGraphNode

OpenSGSceneGraphNode::~OpenSGSceneGraphNode() {
#if OSG_MAJOR_VERSION >= 2
#else //OpenSG1:
	if (node != NullFC)
		subRefCP(node);
#endif
} // ~OpenSGSceneGraphNode

bool OpenSGSceneGraphNode::equals(const OpenSGSceneGraphNode& src) {
	return (node == src.node);
} // equals

#if OSG_MAJOR_VERSION >= 2
OpenSGSceneGraphNode* OpenSGSceneGraphNode::getOpenSGNode(OSG::NodeRecPtr& nodePtr) {
#else //OpenSG1:
OpenSGSceneGraphNode* OpenSGSceneGraphNode::getOpenSGNode(OSG::NodePtr& nodePtr) {
#endif

	OpenSGSceneGraphNode* result = NULL;

#if OSG_MAJOR_VERSION >= 2
	if (nodePtr != NULL)
#else //OpenSG1:
	if (nodePtr != OSG::NullFC) 
#endif
	{
#if OSG_MAJOR_VERSION >= 2
		OSG::NodeCoreRecPtr core = nodePtr->getCore();
#else //OpenSG1:
		OSG::NodeCorePtr core = nodePtr->getCore();
#endif

		if (core->getType().isDerivedFrom(Transform::getClassType())) {
			result = new OpenSGTransformationSceneGraphNode(nodePtr);
		} // if
		else if (core->getType().isDerivedFrom(Geometry::getClassType())) {
			result = new OpenSGGeometrySceneGraphNode(nodePtr);
		} // else if
		else if (core->getType().isDerivedFrom(Group::getClassType())) {
			result = new OpenSGGroupSceneGraphNode(nodePtr);
		} // else if
		else {
			result = new OpenSGSceneGraphNode(nodePtr);
		} // else
	} // if
	else {
		printd(WARNING, "OpenSGSceneGraphNode::getOpenSGNode(): passed node is NULL!\n");
	} // else
	return result;
} // getOpenSGNode

#if OSG_MAJOR_VERSION >= 2
OSG::NodeRecPtr OpenSGSceneGraphNode::getNodePtr() {
	return node;
} // getNodePtr
#else //OpenSG1:
OSG::NodePtr OpenSGSceneGraphNode::getNodePtr() {
	return node;
} // getNodePtr
#endif

SceneGraphNodeInterface::NODE_TYPE OpenSGSceneGraphNode::getNodeType() {
	return nodeType;
} // getNodeType

int OpenSGSceneGraphNode::getNumberOfChildren() {
	return this->node->getNChildren();
} // getNumberOfChildren

SceneGraphNodeInterface* OpenSGSceneGraphNode::getChild(int index) {

	if (index > (int)this->node->getNChildren()) {
		printd(ERROR, "OpenSGSceneGraphNode::getChild(): index %i out of range!\n", index);
		return NULL;
	} // if

#if OSG_MAJOR_VERSION >= 2
	OSG::NodeRecPtr childNode = this->node->getChild(index);
#else //OpenSG1:
	OSG::NodePtr childNode = this->node->getChild(index);
#endif

	OpenSGSceneGraphNode* result = getOpenSGNode(childNode);

	return result;
} // getChild

SceneGraphNodeInterface* OpenSGSceneGraphNode::getParent() {
	OpenSGSceneGraphNode* result = NULL;
#if OSG_MAJOR_VERSION >= 2
	OSG::NodeRecPtr parentNode = this->node->getParent();

	if (parentNode != NULL) {
		result = getOpenSGNode(parentNode);
	} // if
#else //OpenSG1:
	OSG::NodePtr parentNode = this->node->getParent();

	if (parentNode != NullFC) {
		result = getOpenSGNode(parentNode);
	} // if
#endif
	return result;
} // getParent

SceneGraphNodeInterface* OpenSGSceneGraphNode::detachChild(int index) {

	OpenSGSceneGraphNode* result = NULL;
	SceneGraphNodeInterface* sceneGraphNode = getChild(index);

	if (sceneGraphNode) {
		result = dynamic_cast<OpenSGSceneGraphNode*> (sceneGraphNode);
		assert(result);
#if OSG_MAJOR_VERSION >= 2
		this->node->subChild(index);
#else //OpenSG1:
		beginEditCP(this->node, Node::ChildrenFieldMask);
		this->node->subChild(index);
		endEditCP(this->node, Node::ChildrenFieldMask);
#endif
	} // if

	return result;
} // detachChild
