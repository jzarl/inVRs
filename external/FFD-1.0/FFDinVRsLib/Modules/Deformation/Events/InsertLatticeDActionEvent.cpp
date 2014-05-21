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


#include "InsertLatticeDActionEvent.h"

#include <gmtl/AABox.h>

#include "../InVRsDeformActionManager.h"

#include <inVRs/SystemCore/MessageFunctions.h>
#include <inVRs/SystemCore/EventManager/EventManager.h>

InsertLatticeDActionEvent::InsertLatticeDActionEvent(unsigned int
    environmentBasedId, int hDiv, int wDiv, int lDiv, size_t dim, bool masked,
    gmtl::Vec3f aabbMin, gmtl::Vec3f aabbMax, float epsilon) :
    ActionEvent("InsertLatticeDActionEvent"),
    InsertLatticeDAction(hDiv, wDiv, lDiv, dim, masked, aabbMin, aabbMax,
        epsilon), environmentBasedId(environmentBasedId)
{
}

InsertLatticeDActionEvent::InsertLatticeDActionEvent() :
    ActionEvent("InsertLatticeDActionEvent"),
    InsertLatticeDAction(1, 1, 1, 3, false, gmtl::Vec3f(0.0f, 0.0f, 0.0f),
        gmtl::Vec3f(0.0f, 0.0f, 0.0f), 0.0001f),
    environmentBasedId(environmentBasedId)
{
}

InsertLatticeDActionEvent::~InsertLatticeDActionEvent()
{
}

void InsertLatticeDActionEvent::encode(NetMessage *message)
{
    msgFunctions::encode(environmentBasedId, message);
    msgFunctions::encode(hDiv, message);
    msgFunctions::encode(wDiv, message);
    msgFunctions::encode(lDiv, message);
    msgFunctions::encode(dim, message);
    msgFunctions::encode(masked, message);
    msgFunctions::encode(aabbMin, message);
    msgFunctions::encode(aabbMax, message);
    msgFunctions::encode(epsilon, message);
}

void InsertLatticeDActionEvent::decode(NetMessage *message)
{
    msgFunctions::decode(environmentBasedId, message);
    msgFunctions::decode(hDiv, message);
    msgFunctions::decode(wDiv, message);
    msgFunctions::decode(lDiv, message);
    msgFunctions::decode(dim, message);
    msgFunctions::decode(masked, message);
    msgFunctions::decode(aabbMin, message);
    msgFunctions::decode(aabbMax, message);
    msgFunctions::decode(epsilon, message);
}

void InsertLatticeDActionEvent::execute()
{
    Entity* entity =
        WorldDatabase::getEntityWithEnvironmentId(environmentBasedId);
    InVRsDeformActionManager::GetInstance()->insertLatticeLocal(entity, hDiv,
        wDiv, lDiv, dim, masked, gmtl::AABoxf(aabbMin, aabbMax), epsilon);
}
