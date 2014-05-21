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

#ifndef _OPENSGSCENEGRAPHINTERFACE_H
#define _OPENSGSCENEGRAPHINTERFACE_H

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
#include "OpenSGTransformationSceneGraphNode.h"
#include "OpenSGGeometrySceneGraphNode.h"
#include "OpenSGGroupSceneGraphNode.h"
#include "OpenSGModel.h"

class INVRS_OPENSGSCENEGRAPHINTERFACE_API OpenSGSceneGraphInterface : public SceneGraphInterface {
public:

	OpenSGSceneGraphInterface();
	virtual ~OpenSGSceneGraphInterface();

	// inherited:
	virtual bool init();
	virtual bool loadConfig(std::string configFile);
	virtual void cleanup();
	virtual std::string getName();

	//	virtual ModelInterface* loadRepresentation(IrrXMLReader* xml, std::string representationType, bool deepCloneMode, std::string resourcePath);
	virtual ModelInterface* loadModel(std::string fileType, std::string url);
	virtual bool attachEnvironment(Environment* env);
	virtual bool attachEntity(Entity* ent);
	virtual bool attachTile(Environment* env, Tile* tle, const TransformationData& trans);
	// 	virtual bool attachAvatar(AvatarInterface* avatar, TransformationData& trans);
	virtual bool attachModelToEntity(Entity* ent, ModelInterface* model, const TransformationData& trans);
	virtual bool attachModel(ModelInterface* model, const TransformationData& trans);
	virtual bool detachEnvironment(Environment* env);
	virtual bool detachEntity(Entity* ent);
	virtual bool detachTile(Environment* env, Tile* tle);
	virtual bool detachModel(ModelInterface* model);
	virtual bool rayIntersect(ModelInterface* model, gmtl::Vec3f position, gmtl::Vec3f direction,
			float* dist, float maxDist);
	virtual void updateModel(ModelInterface* model, const TransformationData& trans);
	virtual void updateEntity(Entity* ent);
	// 	virtual void updateAvatar(AvatarInterface* avatar, float dt);
	virtual void updateEnvironment(Environment* env);
	virtual void updateTile(Tile* tle, const TransformationData& trans);

	virtual void showEnvironment(Environment* env, bool showOrHide);
	virtual void showModel(ModelInterface* model, bool showOrHide);

	void enableOptimizedLoading();
	void disableOptimizedLoading();
	void optimize();

	void setCameraMatrix(OSG::Matrix m);

#if OSG_MAJOR_VERSION >= 2
	// OpenSG specific:
	OSG::NodeRecPtr getNodePtr();
	OSG::NodeRecPtr getCameraTransformationNode();
#else //OpenSG1:
	// OpenSG specific:
	OSG::NodePtr getNodePtr();
	OSG::NodePtr getCameraTransformationNode();
#endif


protected:

#if OSG_MAJOR_VERSION >= 2
	static bool isNodeInSceneGraph(OSG::NodeRecPtr node, OSG::NodeRecPtr root);

	OSG::NodeRecPtr rootNode;
	OSG::GroupRecPtr rootCore;
	OSG::NodeRecPtr cameraNode;
	OSG::TransformRecPtr cameraCore;
#else //OpenSG1:
	static bool isNodeInSceneGraph(OSG::NodePtr node, OSG::NodePtr root);

	OSG::NodePtr rootNode;
	OSG::GroupPtr rootCore;
	OSG::NodePtr cameraNode;
	OSG::TransformPtr cameraCore;
#endif
	bool optimizedLoadingEnabled;
#if OSG_MAJOR_VERSION >= 2
	OSG::GraphOpSeqRefPtr graphOpSeq;
	OSG::SharePtrGraphOpRefPtr sharePtrGraphOp;
#else //OpenSG1:
	OSG::GraphOpSeq* graphOpSeq;
	OSG::SharePtrGraphOp* sharePtrGraphOp;
#endif

};

#endif
