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
#else
#include <OpenSG/OSGNameAttachment.h>
#endif

#include "OpenSGSceneGraphInterface.h"
#include "../../SystemCore/DebugOutput.h"
#include "../../SystemCore/WorldDatabase/WorldDatabase.h"
#include "../../SystemCore/XMLTools.h"


OSG_USING_NAMESPACE

OpenSGModel::OpenSGModel() {
#if OSG_MAJOR_VERSION >= 2
	modelTransCore = NULL;
	modelTransNode = NULL;
#else //OpenSG1:
	modelTransCore = NullFC;
	modelTransNode = NullFC;
#endif
	searchedNodeName = "";
	visible = true;
	filePath = "";
}

#if OSG_MAJOR_VERSION >= 2
OpenSGModel::OpenSGModel(NodeRecPtr subtree) 
#else //OpenSG1:
OpenSGModel::OpenSGModel(NodePtr subtree) 
#endif
{
	Matrix m;
	modelTransNode = Node::create();
	modelTransCore = Transform::create();
	m.setIdentity();
	visible = true;

	if (subtree) {
		printd("OpenSGModel::OpenSGModel(): passed a NullFC!\n");
	}

#if OSG_MAJOR_VERSION >= 2
	modelTransCore->setMatrix(m);
	modelTransNode->setCore(modelTransCore);
	if (subtree != NULL)
		modelTransNode->addChild(subtree);

	modelTransformation = identityTransformation();

#else //OpenSG1:
	beginEditCP(modelTransCore, Transform::MatrixFieldMask);
		modelTransCore->setMatrix(m);
	endEditCP(modelTransCore, Transform::MatrixFieldMask);
	beginEditCP(modelTransNode, Node::CoreFieldMask | Node::ChildrenFieldMask);
		modelTransNode->setCore(modelTransCore);
		if (subtree != NullFC)
			modelTransNode->addChild(subtree);
	endEditCP(modelTransNode, Node::CoreFieldMask | Node::ChildrenFieldMask);

	modelTransformation = identityTransformation();

	addRefCP(modelTransNode); // will be subtracted in OpenSGModel destructor
#endif

	searchedNodeName = "";
}

OpenSGModel::~OpenSGModel() {
#if OSG_MAJOR_VERSION >= 2
#else //OpenSG1:
	if (modelTransNode != NullFC)
		subRefCP(modelTransNode);
#endif
}

ModelInterface* OpenSGModel::clone() {
	OpenSGModel* ret = NULL;

	ret = new OpenSGModel();

#if OSG_MAJOR_VERSION >= 2
	assert(modelTransNode != NULL);
	assert(modelTransCore != NULL);
#else //OpenSG1:
	assert(modelTransNode != NullFC);
	assert(modelTransCore != NullFC);
#endif

	if (deepClone) {
		ret->modelTransNode = deepCloneTree(modelTransNode);
	} else {
		ret->modelTransNode = cloneTree(modelTransNode);
	}
#if OSG_MAJOR_VERSION >= 2
	NodeCoreRecPtr nc = modelTransNode->getCore();
	ret->modelTransCore = dynamic_pointer_cast<Transform>( nc );
#else //OpenSG1:
	ret->modelTransCore = TransformPtr::dcast(modelTransNode->getCore());

	addRefCP(ret->modelTransNode); // will be subtracted in OpenSGModel destructor
#endif

	ret->modelTransformation = modelTransformation;

	ret->setVisible(this->visible);
	return ret;
}

void OpenSGModel::setModelTransformation(TransformationData& trans) {
	OSG::Matrix matOSG;
	gmtl::Matrix44f matGMTL;

	modelTransformation = trans;

#if OSG_MAJOR_VERSION >= 2
	if ((modelTransCore == NULL) || (modelTransNode == NULL)) 
#else //OpenSG1:
	if ((modelTransCore == NullFC) || (modelTransNode == NullFC)) 
#endif
	{
		printd(ERROR, "OpenSGModel::setModelTransformation(): no model set\n");
		return;
	}

	transformationDataToMatrix(trans, matGMTL);
	matOSG.setValue(matGMTL.getData());

#if OSG_MAJOR_VERSION >= 2
	modelTransCore->setMatrix(matOSG);
#else //OpenSG1:
	beginEditCP(modelTransCore, OSG::Transform::MatrixFieldMask);
	modelTransCore->setMatrix(matOSG);
	endEditCP(modelTransCore, OSG::Transform::MatrixFieldMask);
#endif
}


/**
 * This method returns the Axis Aligned Bounding Box
 * of the Entity in World Coordinates. If there is no
 * root-node set then it returns a Box of size 0
 * @return AABB of Entity (in World Coordinates)
 */
AABB OpenSGModel::getAABB() {
	AABB bbox;
	Pnt3f min, max;
#if OSG_MAJOR_VERSION >= 2
	BoxVolume vol;
#else //OpenSG1:
	DynamicVolume vol;
#endif

#if OSG_MAJOR_VERSION >= 2
	if (modelTransNode == NULL) 
#else //OpenSG1:
	if (modelTransNode == NullFC) 
#endif
	{
		bbox.p0 = gmtl::Vec3f(0, 0, 0);
		bbox.p1 = gmtl::Vec3f(0, 0, 0);
	} // if
	else {
		modelTransNode->getWorldVolume(vol);
		vol.getBounds(min, max);
		bbox.p0[0] = min[0];
		bbox.p0[1] = min[1];
		bbox.p0[2] = min[2];
		bbox.p1[0] = max[0];
		bbox.p1[1] = max[1];
		bbox.p1[2] = max[2];
	} // else
	return bbox;
}

TransformationData OpenSGModel::getModelTransformation() {
	return modelTransformation;
}


SceneGraphNodeInterface* OpenSGModel::getSubNodeByName(std::string nodeName) {
	searchedNodeName = nodeName;
#if OSG_MAJOR_VERSION >= 2
	foundNode = NULL;
	OSG::traverse(modelTransNode, boost::bind(&OpenSGModel::traversalEnter,this,_1));
#else //OpenSG1:
	foundNode = OSG::NullFC;
	OSG::traverse(modelTransNode, osgTypedMethodFunctor1ObjPtrCPtrRef<Action::ResultE, OpenSGModel,
			NodePtr> (this, &OpenSGModel::traversalEnter));
#endif

	OpenSGSceneGraphNode* result = OpenSGSceneGraphNode::getOpenSGNode(foundNode);

	return result;
} // getSubNodeByName

#if OSG_MAJOR_VERSION >= 2
NodeRecPtr OpenSGModel::getNodePtr() {
	return modelTransNode;
}
#else //OpenSG1:
NodePtr OpenSGModel::getNodePtr() {
	return modelTransNode;
}
#endif

void OpenSGModel::setVisible(bool visibility) {
	if (this->visible == visibility)
		return;

#if OSG_MAJOR_VERSION >= 2
	if (modelTransNode != NULL) {
		// ZaJ: not sure about this, but should work...
		modelTransNode->setTravMask(visibility?1:0);
	} // if
#else //OpenSG1:
	if (modelTransNode != NullFC) {
		beginEditCP(modelTransNode);
			modelTransNode->setActive(visibility);
		endEditCP(modelTransNode);
	} // if
#endif

	this->visible = visibility;
} // setVisible

bool OpenSGModel::isVisible() {
	return visible;
} // isVisible

void OpenSGModel::setFilePath(std::string path) {
	this->filePath = path;
} //setFilePath

std::string OpenSGModel::getFilePath() {
	return filePath;
} // getFilePath

#if OSG_MAJOR_VERSION >= 2
OSG::Action::ResultE OpenSGModel::traversalEnter(OSG::Node* node) {

	if (OSG::getName(node) && OSG::getName(node) == searchedNodeName) {
		this->foundNode = node;
		return Action::Quit;
	} // if

	return Action::Continue;
} // traversalEnter		
#else //OpenSG1:
OSG::Action::ResultE OpenSGModel::traversalEnter(OSG::NodePtr& node) {
	if (OSG::getName(node) && OSG::getName(node) == searchedNodeName) {
		this->foundNode = node;
		return Action::Quit;
	} // if

	return Action::Continue;
} // traversalEnter
#endif
