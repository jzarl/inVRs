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


#include "UndoDActionEvent.h"

#include "../InVRsDeformActionManager.h"

#include <inVRs/SystemCore/MessageFunctions.h>
#include <inVRs/SystemCore/EventManager/EventManager.h>

UndoDActionEvent::UndoDActionEvent(unsigned int
    environmentBasedId) :
    ActionEvent("UndoDActionEvent"),
    environmentBasedId(environmentBasedId)
{
}

UndoDActionEvent::UndoDActionEvent() :
    ActionEvent("UndoDActionEvent"),
    environmentBasedId(environmentBasedId)
{
}

UndoDActionEvent::~UndoDActionEvent()
{
}

void UndoDActionEvent::encode(NetMessage *message)
{
    msgFunctions::encode(environmentBasedId, message);
}

void UndoDActionEvent::decode(NetMessage *message)
{
    msgFunctions::decode(environmentBasedId, message);
}

void UndoDActionEvent::execute()
{
    Entity* entity =
        WorldDatabase::getEntityWithEnvironmentId(environmentBasedId);
    InVRsDeformActionManager::GetInstance()->undoLocal(entity);
}
