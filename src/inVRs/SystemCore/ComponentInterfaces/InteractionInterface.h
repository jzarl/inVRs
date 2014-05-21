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

#ifndef _INTERACTIONINTERFACE_H
#define _INTERACTIONINTERFACE_H

#include <string>

#include <gmtl/Matrix.h>
#include <gmtl/Quat.h>

#include "ModuleInterface.h"
#include "../WorldDatabase/Environment.h"
#include "../WorldDatabase/Entity.h"

class INVRS_SYSTEMCORE_API InteractionInterface : public ModuleInterface {
public:

	enum INTERACTIONSTATE {
		STATE_IDLE = 0, STATE_SELECTION = 1, STATE_MANIPULATION = 2
	};

	// Methods derived from ModuleInterface:
	virtual std::string getName() = 0;
	virtual void update(float dt) = 0;
	virtual void cleanup() = 0;

	virtual void updateEntityTransformList(Environment* env) {};
	virtual void destroyEntityNotification(Entity* entity) {};

	virtual INTERACTIONSTATE getState() = 0;

	// 	virtual void init(int selButtonIdx, int terButtonIdx) {};
	// 	virtual void setPositionAndOrientation(gmtl::Vec3f position, gmtl::Quatf orientation) {};
};

#endif
