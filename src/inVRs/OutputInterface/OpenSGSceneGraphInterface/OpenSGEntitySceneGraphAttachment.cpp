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

#include "OpenSGEntitySceneGraphAttachment.h"

#include "../../SystemCore/DebugOutput.h"

OSG_USING_NAMESPACE

OpenSGEntitySceneGraphAttachment::OpenSGEntitySceneGraphAttachment() {
	entityTransNode = Node::create();
	entityTransCore = Transform::create();

#if OSG_MAJOR_VERSION >= 2
	Matrix4f temp;
	temp.setIdentity();
	entityTransCore->setMatrix(temp);

	entityTransNode->setCore(entityTransCore);
#else //OpenSG1:
	Matrix4f temp;
	temp.setIdentity();
	beginEditCP(entityTransCore);
		entityTransCore->setMatrix(temp);
	endEditCP(entityTransCore);

	beginEditCP(entityTransNode, Node::CoreFieldMask | Node::ChildrenFieldMask);
		entityTransNode->setCore(entityTransCore);
	endEditCP(entityTransNode, Node::CoreFieldMask | Node::ChildrenFieldMask);

	addRefCP(entityTransNode);
#endif
} // OpenSGEntitySceneGraphAttachment

OpenSGEntitySceneGraphAttachment::~OpenSGEntitySceneGraphAttachment() {
#if OSG_MAJOR_VERSION >= 2
	if (entityTransNode->getParent() != NULL) {
		printd(ERROR,
				"OpenSGEntitySceneGraphAttachment::destructor(): Entity Transformation Node still in SceneGraph! FORCING REMOVE!\n");
		NodeRecPtr parentNode = entityTransNode->getParent();
		parentNode->subChild(entityTransNode);
	} // if
#else //OpenSG1:
	if (entityTransNode->getParent() != NullFC) {
		printd(ERROR,
				"OpenSGEntitySceneGraphAttachment::destructor(): Entity Transformation Node still in SceneGraph! FORCING REMOVE!\n");
		NodePtr parentNode = entityTransNode->getParent();
		beginEditCP(parentNode);
			parentNode->subChild(entityTransNode);
		endEditCP(parentNode);
	} // if
	subRefCP(entityTransNode);
#endif
} // ~OpenSGEntitySceneGraphAttachment


