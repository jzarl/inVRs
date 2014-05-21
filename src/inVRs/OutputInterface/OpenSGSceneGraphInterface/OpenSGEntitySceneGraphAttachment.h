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

#ifndef OPENSGENTITYSCENEGRAPHATTACHMENT_H_
#define OPENSGENTITYSCENEGRAPHATTACHMENT_H_

#include "OpenSGSceneGraphNode.h"
#include "../SceneGraphAttachment.h"

class INVRS_OPENSGSCENEGRAPHINTERFACE_API OpenSGEntitySceneGraphAttachment : public SceneGraphAttachment {
public:
	OpenSGEntitySceneGraphAttachment();
	virtual ~OpenSGEntitySceneGraphAttachment();

#if OSG_MAJOR_VERSION >= 2
	OSG::NodeRecPtr entityTransNode;
	OSG::TransformRecPtr entityTransCore;
#else //OpenSG1:
	OSG::NodePtr entityTransNode;
	OSG::TransformPtr entityTransCore;
#endif
}; // OpenSGEntitySceneGraphAttachment


#endif /* OPENSGENTITYSCENEGRAPHATTACHMENT_H_ */
