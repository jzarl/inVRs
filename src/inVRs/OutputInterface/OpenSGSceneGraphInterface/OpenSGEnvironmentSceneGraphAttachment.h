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


#ifndef OPENSGENVIRONMENTSCENEGRAPHATTACHMENT_H_
#define OPENSGENVIRONMENTSCENEGRAPHATTACHMENT_H_

#include "OpenSGSceneGraphNode.h"
#include "../SceneGraphAttachment.h"

class INVRS_OPENSGSCENEGRAPHINTERFACE_API OpenSGEnvironmentSceneGraphAttachment: public SceneGraphAttachment {
public:
	OpenSGEnvironmentSceneGraphAttachment();
	virtual ~OpenSGEnvironmentSceneGraphAttachment();

#if OSG_MAJOR_VERSION >= 2
	OSG::NodeRecPtr envGroupeNode;
	OSG::TransformRecPtr envTransCore;
	OSG::NodeRecPtr envTransNode;
#else //OpenSG1:
	OSG::NodePtr envGroupeNode;
	OSG::TransformPtr envTransCore;
	OSG::NodePtr envTransNode;
#endif
};

#endif /* OPENSGENVIRONMENTSCENEGRAPHATTACHMENT_H_ */
