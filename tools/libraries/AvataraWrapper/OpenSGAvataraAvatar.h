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

#ifndef _OPENSGAVATARAAVATAR_H
#define _OPENSGAVATARAAVATAR_H

#include "AvataraAvatarBase.h"
#include "AvataraWrapperExports.h"

#include <OpenSG/OSGNode.h>
#include <OpenSG/OSGTransform.h>

class INVRS_AVATARAWRAPPER_API OpenSGAvataraAvatar : public AvataraAvatarBase {
public:
	OpenSGAvataraAvatar();
	~OpenSGAvataraAvatar();

protected:
	Avatara::Avatar* buildAvatar();
	ModelInterface* buildAvatarModel();

	void updateAvatarGeo(Avatara::Avatar* avatar);

	void setAvatarTransformation(gmtl::Matrix44f m);
	void setAvatarOffset(gmtl::Matrix44f m);

#if OSG_MAJOR_VERSION >= 2
	OSG::TransformRecPtr avatarOffsetCore;
	OSG::TransformRecPtr avatarTrans;
	OSG::NodeRecPtr avatarNode;
#else
	OSG::TransformPtr avatarOffsetCore; // OpenSG core for avatar offset node
	OSG::TransformPtr avatarTrans; // OpenSG core for avatar transformation node
	OSG::NodePtr avatarNode; // OpenSG node for avatar model
#endif
};

/******************************************************************************
 * Factory class for the AvataraAvatar
 */
class INVRS_AVATARAWRAPPER_API OpenSGAvataraAvatarFactory : public AvatarFactory {
public:
	/**
	 * Creates a new AvataraAvatar if the className matches
	 */
	AvatarInterface* create(std::string className, std::string configFile);
}; // AvataraAvatarFactory

#endif //_AVATARAAVATAR_H
