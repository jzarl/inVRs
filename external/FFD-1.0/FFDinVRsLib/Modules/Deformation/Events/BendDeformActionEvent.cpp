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
 *                           Project: FFD                                    *
 *                                                                           *
 * The FFD library was developed during a practical at the Johannes Kepler   *
 * University, Linz in 2009 by Marlene Hochrieser                            *
 *                                                                           *
\*---------------------------------------------------------------------------*/


#include "BendDeformActionEvent.h"

#include "../InVRsDeformActionManager.h"

#include <inVRs/SystemCore/MessageFunctions.h>
#include <inVRs/SystemCore/EventManager/EventManager.h>

BendDeformActionEvent::BendDeformActionEvent(unsigned int environmentBasedId,
    float bendFactor, float center, int axis) :
    ActionEvent("BendDeformActionEvent"),
    BendDeformAction(bendFactor, center, axis),
    environmentBasedId(environmentBasedId)
{
}

BendDeformActionEvent::BendDeformActionEvent() :
    ActionEvent("BendDeformActionEvent"),
    BendDeformAction(0.0f, 0.0f, 0), environmentBasedId(environmentBasedId)
{
}

BendDeformActionEvent::~BendDeformActionEvent()
{
}

void BendDeformActionEvent::encode(NetMessage *message)
{
    msgFunctions::encode(environmentBasedId, message);
	msgFunctions::encode(bendFactor, message);
	msgFunctions::encode(center, message);
	msgFunctions::encode(axis, message);
}

void BendDeformActionEvent::decode(NetMessage *message)
{
    msgFunctions::decode(environmentBasedId, message);
	msgFunctions::decode(bendFactor, message);
	msgFunctions::decode(center, message);
	msgFunctions::decode(axis, message);
}

void BendDeformActionEvent::execute()
{
    Entity* entity =
        WorldDatabase::getEntityWithEnvironmentId(environmentBasedId);
    InVRsDeformActionManager::GetInstance()->bendLocal(entity, bendFactor,
        center, axis);
}
