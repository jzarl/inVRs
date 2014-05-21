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

#include "OpenSGTileSceneGraphAttachment.h"

#include "../../SystemCore/DebugOutput.h"

OSG_USING_NAMESPACE

OpenSGTileSceneGraphAttachment::OpenSGTileSceneGraphAttachment() {
	tileTransNode = Node::create();
	tileTransCore = Transform::create();

	Matrix4f temp;
	temp.setIdentity();
#if OSG_MAJOR_VERSION >= 2
	tileTransCore->setMatrix(temp);

	tileTransNode->setCore(tileTransCore);
#else //OpenSG1:
	beginEditCP(tileTransCore);
		tileTransCore->setMatrix(temp);
	endEditCP(tileTransCore);

	beginEditCP(tileTransNode, Node::CoreFieldMask);
		tileTransNode->setCore(tileTransCore);
	endEditCP(tileTransNode, Node::CoreFieldMask);
	addRefCP(tileTransNode);
#endif
}

OpenSGTileSceneGraphAttachment::~OpenSGTileSceneGraphAttachment() {
#if OSG_MAJOR_VERSION >= 2
	if (tileTransNode->getParent() != NULL) {
		printd(ERROR,
				"OpenSGTileSceneGraphAttachment::destructor(): Tile Transformation Node still in SceneGraph! FORCING REMOVE!\n");
		NodeRecPtr parentNode = tileTransNode->getParent();
		parentNode->subChild(tileTransNode);
	}
#else //OpenSG1:
	if (tileTransNode->getParent() != NullFC) {
		printd(ERROR,
				"OpenSGTileSceneGraphAttachment::destructor(): Tile Transformation Node still in SceneGraph! FORCING REMOVE!\n");
		NodePtr parentNode = tileTransNode->getParent();
		beginEditCP(parentNode);
			parentNode->subChild(tileTransNode);
		endEditCP(parentNode);
	} // if

	subRefCP(tileTransNode);
#endif
}
