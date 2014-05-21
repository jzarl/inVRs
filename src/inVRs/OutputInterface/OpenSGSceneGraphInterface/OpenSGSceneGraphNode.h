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

#ifndef OPENSGSCENEGRAPHNODE_H_
#define OPENSGSCENEGRAPHNODE_H_

// fix problems with Visual Studio 2008
#ifdef WIN32
#include <stdio.h>
#endif

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

#ifdef WIN32
	#ifdef INVRSOPENSGSCENEGRAPHINTERFACE_EXPORTS
	#define INVRS_OPENSGSCENEGRAPHINTERFACE_API __declspec(dllexport)
	#else
	#define INVRS_OPENSGSCENEGRAPHINTERFACE_API __declspec(dllimport)
	#endif
#else
#define INVRS_OPENSGSCENEGRAPHINTERFACE_API
#endif

/**
 *
 */
class INVRS_OPENSGSCENEGRAPHINTERFACE_API OpenSGSceneGraphNode : public virtual SceneGraphNodeInterface {

public:
	/**
	 *
	 */
#if OSG_MAJOR_VERSION >= 2
	OpenSGSceneGraphNode(OSG::NodeRecPtr& nodePtr);
#else //OpenSG1:
	OpenSGSceneGraphNode(OSG::NodePtr& nodePtr);
#endif

	/**
	 *
	 */
	~OpenSGSceneGraphNode();

	/**
	 *
	 */
	bool equals(const OpenSGSceneGraphNode& src);

	/**
	 *
	 */
	virtual SceneGraphNodeInterface* detachChild(int index);

	/**
	 *
	 */
#if OSG_MAJOR_VERSION >= 2
	static OpenSGSceneGraphNode* getOpenSGNode(OSG::NodeRecPtr& nodePtr);
#else //OpenSG1:
	static OpenSGSceneGraphNode* getOpenSGNode(OSG::NodePtr& nodePtr);
#endif

	/**
	 *
	 */
#if OSG_MAJOR_VERSION >= 2
	OSG::NodeRecPtr getNodePtr();
#else //OpenSG1:
	OSG::NodePtr getNodePtr();
#endif

	/**
	 *
	 */
	SceneGraphNodeInterface::NODE_TYPE getNodeType();

	/**
	 *
	 */
	int getNumberOfChildren();

	/**
	 *
	 */
	SceneGraphNodeInterface* getChild(int index);

	/**
	 *
	 */
	SceneGraphNodeInterface* getParent();


protected:
	OpenSGSceneGraphNode();

#if OSG_MAJOR_VERSION >= 2
	OSG::NodeRecPtr node;
#else //OpenSG1:
	OSG::NodePtr node;
#endif
	NODE_TYPE nodeType;
}; // OpenSGSceneGraphNode

#endif
