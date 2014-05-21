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


#include "GlobalDeformActionEvent.h"

#include "../InVRsDeformActionManager.h"

#include <inVRs/SystemCore/MessageFunctions.h>
#include <inVRs/SystemCore/EventManager/EventManager.h>

GlobalDeformActionEvent::GlobalDeformActionEvent(unsigned int
    environmentBasedId, gmtl::Matrix44f dMatrix, bool center) :
    ActionEvent("GlobalDeformActionEvent"),
    GlobalDeformAction(dMatrix, center), environmentBasedId(environmentBasedId)
{
}

GlobalDeformActionEvent::GlobalDeformActionEvent() :
    ActionEvent("GlobalDeformActionEvent"),
    GlobalDeformAction(gmtl::MAT_IDENTITY44F, true),
    environmentBasedId(environmentBasedId)
{
}

GlobalDeformActionEvent::~GlobalDeformActionEvent()
{
}

void GlobalDeformActionEvent::encode(NetMessage *message)
{
    msgFunctions::encode(environmentBasedId, message);
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            msgFunctions::encode(dMatrix[i][j], message);
	msgFunctions::encode(center, message);
}

void GlobalDeformActionEvent::decode(NetMessage *message)
{
    msgFunctions::decode(environmentBasedId, message);
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            msgFunctions::decode(dMatrix[i][j], message);
    msgFunctions::decode(center, message);
}

void GlobalDeformActionEvent::execute()
{
    Entity* entity =
        WorldDatabase::getEntityWithEnvironmentId(environmentBasedId);
    InVRsDeformActionManager::GetInstance()->deformLocal(entity, dMatrix,
        center);
}
