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

#include "OpenSGEnvironmentSceneGraphAttachment.h"

#include "../../SystemCore/DebugOutput.h"

OSG_USING_NAMESPACE

OpenSGEnvironmentSceneGraphAttachment::OpenSGEnvironmentSceneGraphAttachment() {
	envGroupeNode = Node::create();
	envTransNode = Node::create();
	envTransCore = Transform::create();

#if OSG_MAJOR_VERSION >= 2
	envGroupeNode->setCore(Group::create());

	Matrix4f temp;
	temp.setIdentity();
	envTransCore->setMatrix(temp);

	envTransNode->setCore(envTransCore);
	envTransNode->addChild(envGroupeNode);

#else //OpenSG1:
	beginEditCP(envGroupeNode, Node::CoreFieldMask);
		envGroupeNode->setCore(Group::create());
	endEditCP(envGroupeNode, Node::CoreFieldMask);

	Matrix4f temp;
	temp.setIdentity();
	beginEditCP(envTransCore);
		envTransCore->setMatrix(temp);
	endEditCP(envTransCore);

	beginEditCP(envTransNode, Node::CoreFieldMask | Node::ChildrenFieldMask);
		envTransNode->setCore(envTransCore);
		envTransNode->addChild(envGroupeNode);
	endEditCP(envTransNode, Node::CoreFieldMask | Node::ChildrenFieldMask);

	addRefCP(envTransNode);
#endif
} // OpenSGEnvironmentSceneGraphAttachment

OpenSGEnvironmentSceneGraphAttachment::~OpenSGEnvironmentSceneGraphAttachment() {
	if (envGroupeNode->getNChildren() > 0) {
		printd(ERROR,
				"OpenSGEnvironmentSceneGraphAttachment::destructor(): Environment Group-Node still has children attached! FORCING REMOVE!\n");
		for (int i=envGroupeNode->getNChildren()-1; i >= 0 ; i--) {
#if OSG_MAJOR_VERSION >= 2
			envGroupeNode->subChild(i);
#else //OpenSG1:
			beginEditCP(envGroupeNode);
				envGroupeNode->subChild(i);
			endEditCP(envGroupeNode);
#endif
		} // for
	} // if

#if OSG_MAJOR_VERSION >= 2
	if (envTransNode->getParent() != NULL) {
		printd(ERROR,
				"OpenSGEnvironmentSceneGraphAttachment::destructor(): Environment Transformation Node still in SceneGraph! FORCING REMOVE!\n");
		NodeRecPtr parentNode = envTransNode->getParent();
		parentNode->subChild(envTransNode);
	} // if

#else //OpenSG1:
	if (envTransNode->getParent() != NullFC) {
		printd(ERROR,
				"OpenSGEnvironmentSceneGraphAttachment::destructor(): Environment Transformation Node still in SceneGraph! FORCING REMOVE!\n");
		NodePtr parentNode = envTransNode->getParent();
		beginEditCP(parentNode);
			parentNode->subChild(envTransNode);
		endEditCP(parentNode);
	} // if

	subRefCP(envTransNode);
#endif
} // ~OpenSGEnvironmentSceneGraphAttachment
