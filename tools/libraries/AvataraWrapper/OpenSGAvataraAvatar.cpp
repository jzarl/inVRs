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

#include "OpenSGAvataraAvatar.h"

#include <avatara/OSGAvatar.h>

#include <gmtl/External/OpenSGConvert.h>

#include <inVRs/SystemCore/Configuration.h>
#include <inVRs/OutputInterface/OpenSGSceneGraphInterface/OpenSGSceneGraphInterface.h>

#if OSG_MAJOR_VERSION >= 2
	#define NullFC NULL
#endif

OSG_USING_NAMESPACE

OpenSGAvataraAvatar::OpenSGAvataraAvatar() :
	avatarOffsetCore(NullFC), avatarTrans(NullFC), avatarNode(NullFC) {
}

OpenSGAvataraAvatar::~OpenSGAvataraAvatar() {
}

Avatara::Avatar* OpenSGAvataraAvatar::buildAvatar() {
	return new Avatara::OSGAvatar();
}

void OpenSGAvataraAvatar::setAvatarTransformation(gmtl::Matrix44f matrix) {
	Matrix m;
	gmtl::set(m, matrix);

#if OSG_MAJOR_VERSION < 2
	beginEditCP(avatarTrans, Transform::MatrixFieldMask);
#endif
	avatarTrans->setMatrix(m);
#if OSG_MAJOR_VERSION < 2
	endEditCP(avatarTrans, Transform::MatrixFieldMask);
#endif
}

void OpenSGAvataraAvatar::setAvatarOffset(gmtl::Matrix44f matrix) {
	Matrix m;
	gmtl::set(m, matrix);

#if OSG_MAJOR_VERSION < 2
	beginEditCP(avatarOffsetCore, Transform::MatrixFieldMask);
#endif
	avatarOffsetCore->setMatrix(m);
#if OSG_MAJOR_VERSION < 2
	endEditCP(avatarOffsetCore, Transform::MatrixFieldMask);
#endif
}

void OpenSGAvataraAvatar::updateAvatarGeo(Avatara::Avatar* a) {
	Avatara::OSGAvatar* avatar = dynamic_cast<Avatara::OSGAvatar*>(a);
	avatar->UpdateAvatarGeo();
}

ModelInterface* OpenSGAvataraAvatar::buildAvatarModel() {
#if OSG_MAJOR_VERSION < 2
	NodePtr avatarOffsetNode = Node::create();
#else
	NodeRecPtr avatarOffsetNode = Node::create();
#endif
	avatarOffsetCore = Transform::create();

#if OSG_MAJOR_VERSION < 2
	ImagePtr image = Image::create(); // load texture
#else
	ImageRecPtr image = Image::create(); // load texture
#endif

#if OSG_MAJOR_VERSION < 2
	beginEditCP(image);
#endif
	image->read((Configuration::getPath("Avatars") + imageFile).c_str());
#if OSG_MAJOR_VERSION < 2
	endEditCP(image);
#endif

#if OSG_MAJOR_VERSION < 2
	NodePtr n = Node::create(); // put the avatar geometry into a node
#else
	NodeRecPtr n = Node::create(); // put the avatar geometry into a node
#endif
	Avatara::OSGAvatar* oavatar = dynamic_cast<Avatara::OSGAvatar*>(avatar);
#if OSG_MAJOR_VERSION < 2
	beginEditCP(n, Node::CoreFieldMask);
#endif
	n->setCore(oavatar->MakeAvatarGeo(image)); // make geometry with texture
#if OSG_MAJOR_VERSION < 2
	endEditCP(n, Node::CoreFieldMask);
#endif

	avatarTrans = Transform::create();

	avatarNode = Node::create();
#if OSG_MAJOR_VERSION < 2
	beginEditCP(avatarOffsetNode, Node::CoreFieldMask | Node::ChildrenFieldMask);
#endif
	avatarOffsetNode->setCore(avatarOffsetCore);
	avatarOffsetNode->addChild(n);
#if OSG_MAJOR_VERSION < 2
	endEditCP(avatarOffsetNode, Node::CoreFieldMask | Node::ChildrenFieldMask);
#endif

#if OSG_MAJOR_VERSION < 2
	beginEditCP(avatarNode, Node::CoreFieldMask | Node::ChildrenFieldMask);
#endif
	avatarNode->setCore(avatarTrans);
	avatarNode->addChild(avatarOffsetNode);
#if OSG_MAJOR_VERSION < 2
	endEditCP(avatarNode, Node::CoreFieldMask | Node::ChildrenFieldMask);
#endif

	return new OpenSGModel(avatarNode);
} // buildAvatar



AvatarInterface* OpenSGAvataraAvatarFactory::create(std::string className, std::string configFile) {
	if (className != "AvataraAvatar")
		return NULL;

	AvatarInterface *avatar = new OpenSGAvataraAvatar();
	avatar->loadConfig(configFile);
	return avatar;
}
