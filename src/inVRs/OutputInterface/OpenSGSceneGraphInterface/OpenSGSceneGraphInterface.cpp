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

#include "OpenSGSceneGraphInterface.h"
#include "OpenSGEntitySceneGraphAttachment.h"
#include "OpenSGEnvironmentSceneGraphAttachment.h"
#include "OpenSGTileSceneGraphAttachment.h"
#include "../../SystemCore/DebugOutput.h"
#include "../../SystemCore/WorldDatabase/WorldDatabase.h"
#include "../../SystemCore/XMLTools.h"
#include "../../SystemCore/SystemCore.h"

#include "../OutputInterface.h"
#include <inVRs/SystemCore/ComponentInterfaces/module_core_api.h>

OSG_USING_NAMESPACE

OpenSGSceneGraphInterface::OpenSGSceneGraphInterface() :
#if OSG_MAJOR_VERSION >= 2
	rootNode(0),
	rootCore(0),
	cameraNode(0),
#else //OpenSG1:
	rootNode(NullFC),
	rootCore(NullFC),
	cameraNode(NullFC),
#endif
	optimizedLoadingEnabled(false),
	graphOpSeq(NULL),
	sharePtrGraphOp(NULL) {
	init();
} // OpenSGSceneGraphInterface

OpenSGSceneGraphInterface::~OpenSGSceneGraphInterface() {
//	if (rootNode != NullFC) {
//		int nChildren = rootNode->getNChildren();
// Child nodes should be freed by OpenSG!
//		if (nChildren > 0)
//			printd(
//					WARNING,
//					"OpenSGSceneGraphInterface destructor: there are still %d children attached!\n",
//					nChildren);
//	}

#if OSG_MAJOR_VERSION >= 2
	graphOpSeq = NULL;
	sharePtrGraphOp = NULL;
#else //OpenSG1:
	if (graphOpSeq)
		delete graphOpSeq;
	if (sharePtrGraphOp)
		delete sharePtrGraphOp;
#endif
}

bool OpenSGSceneGraphInterface::init() {
	rootCore = Group::create();
	rootNode = Node::create();
	cameraNode = Node::create();
	cameraCore = Transform::create();
	OSG::Matrix m;
	m.setIdentity();

#if OSG_MAJOR_VERSION >= 2
	cameraCore->setMatrix(m);
	cameraNode->setCore(cameraCore);

	rootNode->setCore(rootCore);
	rootNode->addChild(cameraNode);
	graphOpSeq = OSG::GraphOpSeq::create(); 
	sharePtrGraphOp = OSG::SharePtrGraphOp::create();
#else //OpenSG1:
	beginEditCP(cameraCore);
		cameraCore->setMatrix(m);
	endEditCP(cameraCore);
	beginEditCP(cameraNode);
		cameraNode->setCore(cameraCore);
	endEditCP(cameraNode);

	beginEditCP(rootNode, Node::CoreFieldMask | Node::ChildrenFieldMask);
		rootNode->setCore(rootCore);
		rootNode->addChild(cameraNode);
	endEditCP(rootNode, Node::CoreFieldMask | Node::ChildrenFieldMask);
	graphOpSeq = new OSG::GraphOpSeq;
	sharePtrGraphOp = new OSG::SharePtrGraphOp;
#endif
	sharePtrGraphOp->setIncludes("Material, TextureChunk");
	// 	printd("OpenSGSceneGraphInterface::init(): usage of SharePtrGraphOp:\n%s\n", sharePtrGraphOp->usage().c_str());
	graphOpSeq->addGraphOp(sharePtrGraphOp);

	return true;
}

bool OpenSGSceneGraphInterface::loadConfig(std::string configFile) {
	printd(INFO, "OpenSGSceneGraphInterface::loadConfig(): Nothing to configure!\n",
			configFile.c_str());
	return true;
} // loadConfig

void OpenSGSceneGraphInterface::cleanup() {
	// nothing to do here?
} // cleanup

std::string OpenSGSceneGraphInterface::getName() {
	return "OpenSGSceneGraphInterface";
} // getName

ModelInterface* OpenSGSceneGraphInterface::loadModel(std::string fileType, std::string url) {
#if OSG_MAJOR_VERSION >= 2
	NodeRecPtr modelNode;
#else //OpenSG1:
	NodePtr modelNode;
#endif
	OpenSGModel* ret;

	if (!fileExists(url)) {
		printd(
				ERROR,
				"OpenSGSceneGraphInterface::loadModel(): either file %s doesn't exist or it cannot be read!\n",
				url.c_str());
		return NULL;
	}

#if OSG_MAJOR_VERSION >= 2
	if (optimizedLoadingEnabled) {
		modelNode = SceneFileHandler::the()->read(url.c_str(), graphOpSeq);
	} else {
		modelNode = SceneFileHandler::the()->read(url.c_str());
	}		
#else //OpenSG1:
	if (optimizedLoadingEnabled) {
		modelNode = SceneFileHandler::the().read(url.c_str(), graphOpSeq);
	} else {
		modelNode = SceneFileHandler::the().read(url.c_str());
	}
#endif

	if (!modelNode) {
		printd(ERROR, "OpenSGSceneGraphInterface::loadModel(): could not load file %s\n",
				url.c_str());
		return NULL;
	} // if

	ret = new OpenSGModel(modelNode);
	ret->setFilePath(url);

	return ret;
}

bool OpenSGSceneGraphInterface::attachEnvironment(Environment* env) {
	OpenSGEnvironmentSceneGraphAttachment* envAttachment;

	assert(env);

	if (env->sceneGraphAttachment != NULL) {
		printd(ERROR,
				"OpenSGSceneGraphInterface::attachEnvironment(): sceneGraphAttachment already set. Is Environment already in the scene graph?\n");
		return false;
	} // if

	envAttachment = new OpenSGEnvironmentSceneGraphAttachment();
#if OSG_MAJOR_VERSION >= 2
	rootNode->addChild(envAttachment->envTransNode);
#else //OpenSG1:
	beginEditCP(rootNode, Node::ChildrenFieldMask);
		rootNode->addChild(envAttachment->envTransNode);
	endEditCP(rootNode, Node::ChildrenFieldMask);
#endif

	env->sceneGraphAttachment = envAttachment;

	updateEnvironment(env);

	return true;
} // attachEnvironment

bool OpenSGSceneGraphInterface::attachEntity(Entity* ent) {
	OpenSGEntitySceneGraphAttachment* entityAttachment;
	OpenSGEnvironmentSceneGraphAttachment* envAttachment;
#if OSG_MAJOR_VERSION >= 2
	NodeRecPtr modelTransNode;
#else //OpenSG1:
	NodePtr modelTransNode;
#endif
	Environment* parentEnv;
	ModelInterface* entModel;

	assert(ent);

	unsigned entityId;
	unsigned short entId1, entId2;
	entityId = ent->getEnvironmentBasedId();
	split(entityId, entId1, entId2);
	printd(INFO, "OpenSGSceneGraphInterface::attachEntity(): called for entity %u %u\n", entId1,
			entId2);

	entModel = ent->getVisualRepresentation();

	if (entModel == NULL) {
		printd(INFO,
				"OpenSGSceneGraphInterface::attachEntity(): entity doesn't have a visual representation\n");
		return false; // or do something else, i.e. creating a dummy node ...
	}

	parentEnv = ent->getEnvironment();
	assert(parentEnv);
	envAttachment = (OpenSGEnvironmentSceneGraphAttachment*)parentEnv->sceneGraphAttachment;
	assert(envAttachment);

	modelTransNode = ((OpenSGModel*)entModel)->getNodePtr();

	entityAttachment = (OpenSGEntitySceneGraphAttachment*)ent->sceneGraphAttachment;

	if (!entityAttachment) {
		printd(INFO, "OpenSGSceneGraphInterface::addEntity(): adding new Entity to SceneGraph!\n");
		entityAttachment = new OpenSGEntitySceneGraphAttachment;
#if OSG_MAJOR_VERSION >= 2
		entityAttachment->entityTransNode->addChild(modelTransNode);
#else //OpenSG1:
		beginEditCP(entityAttachment->entityTransNode, Node::ChildrenFieldMask);
			entityAttachment->entityTransNode->addChild(modelTransNode);
		endEditCP(entityAttachment->entityTransNode, Node::ChildrenFieldMask);
#endif

		ent->sceneGraphAttachment = entityAttachment;
	} else if (entityAttachment->entityTransNode->getParent() ) {
		printd(
				ERROR,
				"OpenSGSceneGraphInterface::attachEntity(): Entity seems to be already attached to the SceneGraph!\n");
		return false;
	} // else if

#if OSG_MAJOR_VERSION >= 2
	envAttachment->envGroupeNode->addChild(entityAttachment->entityTransNode);
#else //OpenSG1:
	beginEditCP(envAttachment->envGroupeNode, Node::ChildrenFieldMask);
		envAttachment->envGroupeNode->addChild(entityAttachment->entityTransNode);
	endEditCP(envAttachment->envGroupeNode, Node::ChildrenFieldMask);
#endif

	updateEntity(ent);

	return true;
}

bool OpenSGSceneGraphInterface::attachTile(Environment* env, Tile* tle, const TransformationData& trans) {
	OpenSGTileSceneGraphAttachment* tileAttachment;
	OpenSGEnvironmentSceneGraphAttachment* envAttachment;
#if OSG_MAJOR_VERSION >= 2
	NodeRecPtr modelTransNode;
#else //OpenSG1:
	NodePtr modelTransNode;
#endif
	ModelInterface* tileModel;

	assert(env);
	assert(tle);

	tileModel = tle->getModel();
	if (tileModel == NULL) {
		printd(ERROR, "OpenSGSceneGraphInterface::attachTile(): tile has no model set\n");
		return false;
	} // if

	if (tle->sceneGraphAttachment != NULL) {
		printd(ERROR,
				"OpenSGSceneGraphInterface::attachTile(): sceneGraphAttachment already set. Is Tile already in the scene graph?\n");
		return false;
	} // if

	envAttachment = (OpenSGEnvironmentSceneGraphAttachment*)env->sceneGraphAttachment;
	assert(envAttachment);

	modelTransNode = ((OpenSGModel*)tileModel)->getNodePtr();
	tileAttachment = new OpenSGTileSceneGraphAttachment();
#if OSG_MAJOR_VERSION >= 2
	tileAttachment->tileTransNode->addChild(modelTransNode);
	envAttachment->envGroupeNode->addChild(tileAttachment->tileTransNode);
#else //OpenSG1:
	beginEditCP(tileAttachment->tileTransNode, Node::ChildrenFieldMask);
		tileAttachment->tileTransNode->addChild(modelTransNode);
	endEditCP(tileAttachment->tileTransNode, Node::ChildrenFieldMask);

	beginEditCP(envAttachment->envGroupeNode, Node::ChildrenFieldMask);
		envAttachment->envGroupeNode->addChild(tileAttachment->tileTransNode);
	endEditCP(envAttachment->envGroupeNode, Node::ChildrenFieldMask);
#endif

	tle->sceneGraphAttachment = tileAttachment;

	updateTile(tle, trans);

	return true;
}

bool OpenSGSceneGraphInterface::attachModelToEntity(Entity* ent, ModelInterface* model,
		const TransformationData& trans) {
#if OSG_MAJOR_VERSION >= 2
	OSG::NodeRecPtr aboveModelNode;
	OSG::TransformRecPtr aboveModelCore;
	OSG::NodeRecPtr modelTransNode;
#else //OpenSG1:
	OSG::NodePtr aboveModelNode;
	OSG::TransformPtr aboveModelCore;
	OSG::NodePtr modelTransNode;
#endif
	OpenSGEntitySceneGraphAttachment* entityAttachment;

	if (!model) {
		printd(ERROR, "OpenSGSceneGraphInterface::attachModelToEntity(): NULL pointer passed\n");
		return false;
	}

	modelTransNode = ((OpenSGModel*)model)->getNodePtr();

	if (isNodeInSceneGraph(modelTransNode, rootNode)) {
		printd(
				ERROR,
				"OpenSGSceneGraphInterface::attachModelToEntity(): model already in the scene graph, doing nothing\n");
		return false;
	} else {
#if OSG_MAJOR_VERSION >= 2
		assert(modelTransNode->getParent() == NULL);
#else //OpenSG1:
		assert(modelTransNode->getParent() == NullFC);
#endif
	}

	entityAttachment = (OpenSGEntitySceneGraphAttachment*)ent->sceneGraphAttachment;
	if (!entityAttachment) {
		printd(
				ERROR,
				"OpenSGSceneGraphInterface::attachModelToEntity(): entity seems not to be attached to scene graph!\n");
		return false;
	} // if

	aboveModelNode = Node::create();
	aboveModelCore = Transform::create();

	Matrix4f temp;
	gmtl::Matrix44f matGMTL;
	temp.setIdentity();
	transformationDataToMatrix(trans, matGMTL);
	temp.setValue(matGMTL.getData());
#if OSG_MAJOR_VERSION >= 2
	aboveModelCore->setMatrix(temp);

	aboveModelNode->setCore(aboveModelCore);
	aboveModelNode->addChild(modelTransNode);
	
	entityAttachment->entityTransNode->addChild(aboveModelNode);
#else //OpenSG1:
	beginEditCP(aboveModelCore);
		aboveModelCore->setMatrix(temp);
	endEditCP(aboveModelCore);

	beginEditCP(aboveModelNode, Node::CoreFieldMask | Node::ChildrenFieldMask);
		aboveModelNode->setCore(aboveModelCore);
		aboveModelNode->addChild(modelTransNode);
	endEditCP(aboveModelNode, Node::CoreFieldMask | Node::ChildrenFieldMask);

	beginEditCP(entityAttachment->entityTransNode, Node::ChildrenFieldMask);
		entityAttachment->entityTransNode->addChild(aboveModelNode);
	endEditCP(entityAttachment->entityTransNode, Node::ChildrenFieldMask);
#endif

	return true;
}

bool OpenSGSceneGraphInterface::attachModel(ModelInterface* model, const TransformationData& trans) {
#if OSG_MAJOR_VERSION >= 2
	OSG::NodeRecPtr aboveModelNode;
	OSG::TransformRecPtr aboveModelCore;
	OSG::NodeRecPtr modelTransNode;
#else //OpenSG1:
	OSG::NodePtr aboveModelNode;
	OSG::TransformPtr aboveModelCore;
	OSG::NodePtr modelTransNode;
#endif

	if (!model) {
		printd(ERROR, "OpenSGSceneGraphInterface::attachModel(): NULL pointer passed\n");
		return false;
	}

	if (!rootNode) {
		printd(ERROR, "OpenSGSceneGraphInterface::attachModel(): root node was not set yet!\n");
		return false;
	} // if

	modelTransNode = ((OpenSGModel*)model)->getNodePtr();

	if (isNodeInSceneGraph(modelTransNode, rootNode)) {
		printd(ERROR,
				"OpenSGSceneGraphInterface::attachModel(): model already in the scene graph, doing nothing\n");
		return false;
	} else {
#if OSG_MAJOR_VERSION >= 2
		assert(modelTransNode->getParent() == NULL);
#else //OpenSG1:
		assert(modelTransNode->getParent() == NullFC);
#endif
	}

	aboveModelNode = Node::create();
	aboveModelCore = Transform::create();

	Matrix4f temp;
	gmtl::Matrix44f matGMTL;
	temp.setIdentity();
	transformationDataToMatrix(trans, matGMTL);
	temp.setValue(matGMTL.getData());
#if OSG_MAJOR_VERSION >= 2
	aboveModelCore->setMatrix(temp);

	aboveModelNode->setCore(aboveModelCore);
	aboveModelNode->addChild(modelTransNode);

	rootNode->addChild(aboveModelNode);
#else //OpenSG1:
	beginEditCP(aboveModelCore);
	aboveModelCore->setMatrix(temp);
	endEditCP(aboveModelCore);

	beginEditCP(aboveModelNode, Node::CoreFieldMask | Node::ChildrenFieldMask);
	aboveModelNode->setCore(aboveModelCore);
	aboveModelNode->addChild(modelTransNode);
	endEditCP(aboveModelNode, Node::CoreFieldMask | Node::ChildrenFieldMask);

	beginEditCP(rootNode, Node::ChildrenFieldMask);
	rootNode->addChild(aboveModelNode);
	endEditCP(rootNode, Node::ChildrenFieldMask);
#endif

	return true;
}

bool OpenSGSceneGraphInterface::detachEnvironment(Environment* env) {
	OpenSGEnvironmentSceneGraphAttachment* envAttachment;
	int nChildren;

	assert(env);
	envAttachment = (OpenSGEnvironmentSceneGraphAttachment*)env->sceneGraphAttachment;
	if (!envAttachment) {
		printd(ERROR,
				"OpenSGSceneGraphInterface::detachEnvironment(): environment is not in the scene graph\n");
		return false;
	} // if

	nChildren = envAttachment->envGroupeNode->getNChildren();
	if (nChildren > 0) {
		printd(WARNING,
				"OpenSGSceneGraphInterface::detachEnvironment(): environment has still %d children attached\n",
				nChildren);
	} // if

#if OSG_MAJOR_VERSION >= 2
	rootNode->subChild(envAttachment->envTransNode);
#else //OpenSG1:
	beginEditCP(rootNode, Node::ChildrenFieldMask);
		rootNode->subChild(envAttachment->envTransNode);
	endEditCP(rootNode, Node::ChildrenFieldMask);
#endif

	delete envAttachment;
	env->sceneGraphAttachment = NULL;

	return true;
}

bool OpenSGSceneGraphInterface::detachEntity(Entity* ent) {
	OpenSGEntitySceneGraphAttachment* entityAttachment;
	OpenSGEnvironmentSceneGraphAttachment* envAttachment;
	Environment* parentEnv;

	unsigned entityId;
	unsigned short entId1, entId2;
	entityId = ent->getEnvironmentBasedId();
	split(entityId, entId1, entId2);
	printd(INFO, "OpenSGSceneGraphInterface::detachEntity(): called for entity %u %u\n", entId1,
			entId2);

	assert(ent);
	entityAttachment = (OpenSGEntitySceneGraphAttachment*)ent->sceneGraphAttachment;
	if (!entityAttachment) {
		printd(ERROR, "OpenSGSceneGraphInterface::detachEntity(): entity not in the scene graph\n");
		return false;
	}
	parentEnv = ent->getEnvironment();
	assert(parentEnv);
	envAttachment = (OpenSGEnvironmentSceneGraphAttachment*)parentEnv->sceneGraphAttachment;
	assert(envAttachment);

#if OSG_MAJOR_VERSION >= 2
	envAttachment->envGroupeNode->subChild(entityAttachment->entityTransNode);
#else //OpenSG1:
	beginEditCP(envAttachment->envGroupeNode, Node::ChildrenFieldMask);
		envAttachment->envGroupeNode->subChild(entityAttachment->entityTransNode);
	endEditCP(envAttachment->envGroupeNode, Node::ChildrenFieldMask);
#endif

	return true;
}

bool OpenSGSceneGraphInterface::detachTile(Environment* env, Tile* tle) {
	OpenSGTileSceneGraphAttachment* tileAttachment;
	OpenSGEnvironmentSceneGraphAttachment* envAttachment;

	assert(tle);
	assert(env);
	tileAttachment = (OpenSGTileSceneGraphAttachment*)tle->sceneGraphAttachment;
	if (!tileAttachment) {
		printd(ERROR, "OpenSGSceneGraphInterface::detachTile(): Tile not in the scene graph!\n");
		return false;
	}
	envAttachment = (OpenSGEnvironmentSceneGraphAttachment*)env->sceneGraphAttachment;
	assert(envAttachment);

#if OSG_MAJOR_VERSION >= 2
	envAttachment->envGroupeNode->subChild(tileAttachment->tileTransNode);
#else //OpenSG1:
	beginEditCP(envAttachment->envGroupeNode, Node::ChildrenFieldMask);
		envAttachment->envGroupeNode->subChild(tileAttachment->tileTransNode);
	endEditCP(envAttachment->envGroupeNode, Node::ChildrenFieldMask);
#endif

	delete tileAttachment;
	tle->sceneGraphAttachment = NULL;

	return true;
}

bool OpenSGSceneGraphInterface::detachModel(ModelInterface* model) {
#if OSG_MAJOR_VERSION >= 2
	OSG::NodeRecPtr aboveModelNode;
	OSG::NodeRecPtr modelParent;
#else //OpenSG1:
	OSG::NodePtr aboveModelNode;
	OSG::NodePtr modelParent;
#endif

	if (!model) {
		printd(ERROR, "OpenSGSceneGraphInterface::detachModel(): NULL pointer passed\n");
		return false;
	}

	aboveModelNode = ((OpenSGModel*)model)->modelTransNode->getParent();
	if (!aboveModelNode) {
		printd(ERROR,
				"OpenSGSceneGraphInterface::detachModel(): model is not in the scene graph!\n");
		return false;
	}

	modelParent = aboveModelNode->getParent(); // modelParent is either an entity transform node or root
	assert(modelParent);

#if OSG_MAJOR_VERSION >= 2
	modelParent->subChild(aboveModelNode);
#else //OpenSG1:
	beginEditCP(modelParent, Node::ChildrenFieldMask);
	modelParent->subChild(aboveModelNode);
	endEditCP(modelParent, Node::ChildrenFieldMask);
#endif

	return true;
}

bool OpenSGSceneGraphInterface::rayIntersect(ModelInterface* model, gmtl::Vec3f position,
		gmtl::Vec3f direction, float* dist, float maxDist) {
	int i;
#if OSG_MAJOR_VERSION >= 2
	IntersectAction::ObjRefPtr iAct = NULL;
	NodeRecPtr objNode;
#else
	IntersectAction *iAct = NULL;
	NodePtr objNode;
#endif
	Line ray;
	gmtl::Vec4f worldPosition, transformedPosition;
	gmtl::Vec4f worldDirection, transformedDirection;
	Vec4f osgDirection;
	Vec4f osgPosition;
	bool ret;
	gmtl::Matrix44f worldToModel;

	if (!model) {
		printd(ERROR, "OpenSGSceneGraphInterface::rayIntersect(): model == NULL\n");
		return false;
	}

	objNode = ((OpenSGModel*)model)->getNodePtr();

	if (!objNode) {
		printd(INFO,
				"OpenSGSceneGraphInterface::rayIntersect(): cannot intersect with me, im a NullFC\n");
		return false;
	} // if

	worldToModel.set(objNode->getToWorld().getValues());
	gmtl::invert(worldToModel);

	for (i = 0; i < 3; i++) {
		worldPosition[i] = position[i];
		worldDirection[i] = direction[i];
	} // for

	worldPosition[3] = 1;
	worldDirection[3] = 0;

	transformedPosition = worldToModel * worldPosition;
	transformedDirection = worldToModel * worldDirection;

	ray = Line(Pnt3f(transformedPosition[0], transformedPosition[1], transformedPosition[2]),
			Vec3f(transformedDirection[0], transformedDirection[1], transformedDirection[2]));

	iAct = IntersectAction::create();
	if (maxDist > 0)
		iAct->setLine(ray, maxDist);
	else
		iAct->setLine(ray); // no maxDist

	iAct->apply(objNode);

	if (iAct->didHit()) {
		if (dist)
			*dist = iAct->getHitT();
		ret = true;
	} else {
		ret = false;
	}

#if OSG_MAJOR_VERSION >= 2
	// iAct is reference counted
#else
	delete iAct;
#endif
	return ret;
}

void OpenSGSceneGraphInterface::updateModel(ModelInterface* model, const TransformationData& trans) {
#if OSG_MAJOR_VERSION >= 2
	OSG::NodeRecPtr aboveModelNode;
	OSG::TransformRecPtr aboveModelCore; // assuming the node above is a transform node which places the model in the world
#else //OpenSG1:
	OSG::NodePtr aboveModelNode;
	OSG::TransformPtr aboveModelCore; // assuming the node above is a transform node which places the model in the world
#endif
	OSG::Matrix matOSG;
	gmtl::Matrix44f matGMTL;

	if (!model) {
		printd(ERROR, "OpenSGSceneGraphInterface::updateModel(): NULL model passed\n");
		return;
	}

	aboveModelNode = ((OpenSGModel*)model)->modelTransNode->getParent();
	if (!aboveModelNode) {
		printd(ERROR,
				"OpenSGSceneGraphInterface::updateModel(): model is not in the scene graph!\n");
		return;
	}

#if OSG_MAJOR_VERSION >= 2
	NodeCoreRecPtr nc = aboveModelNode->getCore();
	aboveModelCore = dynamic_pointer_cast<Transform>( nc );
#else //OpenSG1:
	aboveModelCore = TransformPtr::dcast(aboveModelNode->getCore());
#endif

	transformationDataToMatrix(trans, matGMTL);
	matOSG.setValue(matGMTL.getData());

#if OSG_MAJOR_VERSION >= 2
	aboveModelCore->setMatrix(matOSG);
#else //OpenSG1:
	beginEditCP(aboveModelCore, OSG::Transform::MatrixFieldMask);
	aboveModelCore->setMatrix(matOSG);
	endEditCP(aboveModelCore, OSG::Transform::MatrixFieldMask);
#endif
}

void OpenSGSceneGraphInterface::updateEntity(Entity* ent) {
	OpenSGEntitySceneGraphAttachment* entityAttachment;
	OSG::Matrix matOSG;
	gmtl::Matrix44f matGMTL;
	TransformationData trans;

	assert(ent);

	entityAttachment = (OpenSGEntitySceneGraphAttachment*)ent->sceneGraphAttachment;
	if (entityAttachment == NULL) {
		printd(INFO,
				"OpenSGSceneGraphInterface::updateEntity(): entity not (yet) attached to the scene graph\n");
		return;
	} // if

	trans = ent->getEnvironmentTransformation();

	transformationDataToMatrix(trans, matGMTL);
	matOSG.setValue(matGMTL.getData());

#if OSG_MAJOR_VERSION >= 2
	entityAttachment->entityTransCore->setMatrix(matOSG);
#else //OpenSG1:
	beginEditCP(entityAttachment->entityTransCore, OSG::Transform::MatrixFieldMask);
	entityAttachment->entityTransCore->setMatrix(matOSG);
	endEditCP(entityAttachment->entityTransCore, OSG::Transform::MatrixFieldMask);
#endif
}

void OpenSGSceneGraphInterface::updateEnvironment(Environment* env) {
	Matrix4f translate;
	float posX, posY, xSpacing, ySpacing;
	OpenSGEnvironmentSceneGraphAttachment* envAttachment;

	assert(env);
	envAttachment = (OpenSGEnvironmentSceneGraphAttachment*)env->sceneGraphAttachment;
	assert(envAttachment);

	posX = (float)env->getXPosition();
	posY = (float)env->getZPosition();
	xSpacing = (float)env->getXSpacing();
	ySpacing = (float)env->getZSpacing();

	translate.setIdentity();
	translate.setTranslate(Vec3f(posX * xSpacing, 0, posY * ySpacing));
#if OSG_MAJOR_VERSION >= 2
	envAttachment->envTransCore->setMatrix(translate);
#else //OpenSG1:
	beginEditCP(envAttachment->envTransCore, OSG::Transform::MatrixFieldMask);
		envAttachment->envTransCore->setMatrix(translate);
	endEditCP(envAttachment->envTransCore, OSG::Transform::MatrixFieldMask);
#endif
} // updateEnvironment

void OpenSGSceneGraphInterface::updateTile(Tile* tle, const TransformationData& trans) {
	OpenSGTileSceneGraphAttachment* tileAttachment;
	OSG::Matrix matOSG;
	gmtl::Matrix44f matGMTL;

	assert(tle);
	tileAttachment = (OpenSGTileSceneGraphAttachment*)tle->sceneGraphAttachment;
	assert(tileAttachment);

	transformationDataToMatrix(trans, matGMTL);
	matOSG.setValue(matGMTL.getData());

#if OSG_MAJOR_VERSION >= 2
	tileAttachment->tileTransCore->setMatrix(matOSG);
#else //OpenSG1:
	beginEditCP(tileAttachment->tileTransCore, OSG::Transform::MatrixFieldMask);
		tileAttachment->tileTransCore->setMatrix(matOSG);
	endEditCP(tileAttachment->tileTransCore, OSG::Transform::MatrixFieldMask);
#endif
}

/**
 * @param showOrHide <code>true</code>: show; <code>false</code>: hide.
 */
void OpenSGSceneGraphInterface::showEnvironment(Environment* env, bool showOrHide) {
	OpenSGEnvironmentSceneGraphAttachment* envAttachment;

	assert(env);
	envAttachment = (OpenSGEnvironmentSceneGraphAttachment*)env->sceneGraphAttachment;
	if (!envAttachment) {
		printd(ERROR, "OpenSGSceneGraphInterface::showEnvironment(): environment not attached!\n");
		return;
	} // if

#if OSG_MAJOR_VERSION >= 2
	// ZaJ: not sure about this, but should work...
	envAttachment->envGroupeNode->setTravMask(showOrHide?1:0);
#else //OpenSG1:
	beginEditCP(envAttachment->envGroupeNode);
		envAttachment->envGroupeNode->setActive(showOrHide);
	endEditCP(envAttachment->envGroupeNode);
#endif
} // showEnvironment

/**
 * @param showOrHide <code>true</code>: show; <code>false</code>: hide.
 */
void OpenSGSceneGraphInterface::showModel(ModelInterface* model, bool showOrHide) {
#if OSG_MAJOR_VERSION >= 2
	NodeRecPtr modelNode;
#else //OpenSG1:
	NodePtr modelNode;
#endif

	if (!model)
		return;

	modelNode = ((OpenSGModel*)model)->getNodePtr();
	if (!modelNode)
		return;

	// TODO: check wheter model is in the scene graph

#if OSG_MAJOR_VERSION >= 2
	// ZaJ: not sure about this, but should work...
	modelNode->setTravMask(showOrHide?1:0);
#else //OpenSG1:
	beginEditCP(modelNode);
	modelNode->setActive(showOrHide);
	endEditCP(modelNode);
#endif
}

void OpenSGSceneGraphInterface::enableOptimizedLoading() {
	optimizedLoadingEnabled = true;
}

void OpenSGSceneGraphInterface::disableOptimizedLoading() {
	optimizedLoadingEnabled = false;
}

void OpenSGSceneGraphInterface::optimize() {
#if OSG_MAJOR_VERSION >= 2
	OSG::GraphOpSeqRefPtr graphOpSeq;
	OSG::SharePtrGraphOpRefPtr sharePtrGraphOp;	
#else //OpenSG1:
	OSG::GraphOpSeq * graphOpSeq;
	OSG::SharePtrGraphOp * sharePtrGraphOp;
#endif

	if (!rootNode)
		return;

#if OSG_MAJOR_VERSION >= 2
	graphOpSeq = OSG::GraphOpSeq::create();
	sharePtrGraphOp = OSG::SharePtrGraphOp::create();
#else //OpenSG1:
	graphOpSeq = new OSG::GraphOpSeq;
	sharePtrGraphOp = new OSG::SharePtrGraphOp;
#endif
	sharePtrGraphOp->setIncludes("Material, TextureChunk");
	// 	printd("opimizeSubtree: usage of SharePtrGraphOp:\n%s\n", sharePtrGraphOp->usage().c_str());
	graphOpSeq->addGraphOp(sharePtrGraphOp);

#if OSG_MAJOR_VERSION >= 2
	graphOpSeq->traverse(rootNode);
#else //OpenSG1:
	graphOpSeq->run(rootNode);
#endif

#if OSG_MAJOR_VERSION >= 2
	graphOpSeq=NULL;
	sharePtrGraphOp=NULL;
#else //OpenSG1:
	delete graphOpSeq;
	delete sharePtrGraphOp;
#endif
}

void OpenSGSceneGraphInterface::setCameraMatrix(OSG::Matrix m) {
#if OSG_MAJOR_VERSION >= 2
	cameraCore->setMatrix(m);
#else //OpenSG1:
	beginEditCP(cameraCore);
		cameraCore->setMatrix(m);
	endEditCP(cameraCore);
#endif
} // setCameraMatrix


#if OSG_MAJOR_VERSION >= 2
OSG::NodeRecPtr OpenSGSceneGraphInterface::getNodePtr() {
	return rootNode;
}

OSG::NodeRecPtr OpenSGSceneGraphInterface::getCameraTransformationNode() {
	return cameraNode;
} // getCameraTransformationNode
#else //OpenSG1:
OSG::NodePtr OpenSGSceneGraphInterface::getNodePtr() {
	return rootNode;
}

OSG::NodePtr OpenSGSceneGraphInterface::getCameraTransformationNode() {
	return cameraNode;
} // getCameraTransformationNode
#endif

#if OSG_MAJOR_VERSION >= 2
bool OpenSGSceneGraphInterface::isNodeInSceneGraph(OSG::NodeRecPtr node, OSG::NodeRecPtr root)
#else //OpenSG1:
bool OpenSGSceneGraphInterface::isNodeInSceneGraph(OSG::NodePtr node, OSG::NodePtr root)
#endif
{
	int nChildren = root->getNChildren();
	bool ret = false;
	int i;

	if (!node)
		return false;

	if (root == node)
		return true;

	for (i = 0; i < nChildren; i++) {
		ret = isNodeInSceneGraph(node, root->getChild(i));
		if (ret)
			return true;
	}
	return false;
}

MAKEMODULEPLUGIN(OpenSGSceneGraphInterface, OutputInterface)

