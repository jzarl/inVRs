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


#include "TaperDeformActionEvent.h"

#include "../InVRsDeformActionManager.h"

#include <inVRs/SystemCore/MessageFunctions.h>
#include <inVRs/SystemCore/EventManager/EventManager.h>

TaperDeformActionEvent::TaperDeformActionEvent(unsigned int environmentBasedId,
    float taperFactor, int axis) :
    ActionEvent("TaperDeformActionEvent"),
    environmentBasedId(environmentBasedId), TaperDeformAction(taperFactor, axis)
{
}

TaperDeformActionEvent::TaperDeformActionEvent() :
    ActionEvent("TaperDeformActionEvent"),
    environmentBasedId(environmentBasedId), TaperDeformAction(0, 0)
{
}

TaperDeformActionEvent::~TaperDeformActionEvent()
{
}

void TaperDeformActionEvent::encode(NetMessage *message)
{
    msgFunctions::encode(environmentBasedId, message);
	msgFunctions::encode(taperFactor, message);
	msgFunctions::encode(axis, message);
}

void TaperDeformActionEvent::decode(NetMessage *message)
{
    msgFunctions::decode(environmentBasedId, message);
	msgFunctions::decode(taperFactor, message);
	msgFunctions::decode(axis, message);
}

void TaperDeformActionEvent::execute()
{
    Entity* entity =
        WorldDatabase::getEntityWithEnvironmentId(environmentBasedId);
    InVRsDeformActionManager::GetInstance()->taperLocal(entity, taperFactor,
        axis);
}
