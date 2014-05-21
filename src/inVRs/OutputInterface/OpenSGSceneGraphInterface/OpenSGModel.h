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

#ifndef OPENSGMODEL_H_
#define OPENSGMODEL_H_

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


/**
 *
 */
class INVRS_OPENSGSCENEGRAPHINTERFACE_API OpenSGModel : public ModelInterface {

public:

	OpenSGModel();
#if OSG_MAJOR_VERSION >= 2
	OpenSGModel(OSG::NodeRecPtr subtree);
#else //OpenSG1:
	OpenSGModel(OSG::NodePtr subtree);
#endif
	virtual ~OpenSGModel();

	virtual ModelInterface* clone();

	virtual SceneGraphNodeInterface* getSubNodeByName(std::string nodeName);

	virtual void setModelTransformation(TransformationData& trans);

	// inherited:
	virtual AABB getAABB();
	virtual TransformationData getModelTransformation();

	virtual void setVisible(bool visibility);
	virtual bool isVisible();

	virtual void setFilePath(std::string path);
	virtual std::string getFilePath();

	// OpenSG specific:
#if OSG_MAJOR_VERSION >= 2
	OSG::NodeRecPtr getNodePtr();
#else //OpenSG1:
	OSG::NodePtr getNodePtr();
#endif

protected:
#if OSG_MAJOR_VERSION >= 2
	OSG::Action::ResultE traversalEnter(OSG::Node* node);	
#else //OpenSG1:
	OSG::Action::ResultE traversalEnter(OSG::NodePtr& node);
#endif

	std::string searchedNodeName;
#if OSG_MAJOR_VERSION >= 2
	OSG::NodeRecPtr foundNode;
	OSG::TransformRecPtr modelTransCore;
	OSG::NodeRecPtr modelTransNode;
#else //OpenSG1:
	OSG::NodePtr foundNode;
	OSG::TransformPtr modelTransCore;
	OSG::NodePtr modelTransNode;
#endif
	TransformationData modelTransformation;

	bool visible;

	std::string filePath;	

	friend class OpenSGSceneGraphInterface;
};

#endif
