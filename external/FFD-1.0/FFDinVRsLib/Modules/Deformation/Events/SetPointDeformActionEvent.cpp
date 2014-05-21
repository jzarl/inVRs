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


#include "SetPointDeformActionEvent.h"

#include "../InVRsDeformActionManager.h"

#include <inVRs/SystemCore/MessageFunctions.h>
#include <inVRs/SystemCore/EventManager/EventManager.h>

SetPointDeformActionEvent::SetPointDeformActionEvent(unsigned int
    environmentBasedId, gmtl::Vec3i index, gmtl::Vec3f position) :
    ActionEvent("SetPointDeformActionEvent"),
    SetPointDeformAction(index, position),
    environmentBasedId(environmentBasedId)
{
}

SetPointDeformActionEvent::SetPointDeformActionEvent() :
    ActionEvent("SetPointDeformActionEvent"),
    //SetPointDeformAction(gmtl::Vec3i::Vec(), gmtl::Vec3f::Vec()),
    SetPointDeformAction(gmtl::Vec3i(), gmtl::Vec3f()),
    environmentBasedId(environmentBasedId)
{
}

SetPointDeformActionEvent::~SetPointDeformActionEvent()
{
}

void SetPointDeformActionEvent::encode(NetMessage *message)
{
    msgFunctions::encode(environmentBasedId, message);
	for (int i = 0; i < 3; i++)
        msgFunctions::encode(index[i], message);
	msgFunctions::encode(position, message);
}

void SetPointDeformActionEvent::decode(NetMessage *message)
{
    msgFunctions::decode(environmentBasedId, message);
	for (int i = 0; i < 3; i++)
        msgFunctions::decode(index[i], message);
	msgFunctions::decode(position, message);
}

void SetPointDeformActionEvent::execute()
{
    Entity* entity =
        WorldDatabase::getEntityWithEnvironmentId(environmentBasedId);
    InVRsDeformActionManager::GetInstance()->setPointLocal(entity, index,
        position);
}
