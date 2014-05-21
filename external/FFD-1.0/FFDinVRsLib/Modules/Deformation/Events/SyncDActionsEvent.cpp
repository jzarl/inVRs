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


#include "SyncDActionsEvent.h"

#include "ffd/Actions/RemoveLatticeDAction.h"
#include "ffd/Actions/DeformAction.h"
#include "ffd/Actions/ExecuteDAction.h"
#include "ffd/Actions/GlobalDeformAction.h"
#include "ffd/Actions/SetPointDeformAction.h"
#include "ffd/Actions/TaperDeformAction.h"
#include "ffd/Actions/TwistDeformAction.h"
#include "ffd/Actions/BendDeformAction.h"
#include "ffd/Actions/InsertLatticeDAction.h"

#include <inVRs/SystemCore/DebugOutput.h>
#include <inVRs/SystemCore/MessageFunctions.h>
#include <inVRs/SystemCore/EventManager/EventManager.h>

SyncDActionsEvent::SyncDActionsEvent() : ActionEvent("SyncDActionsEvent")
{
}

SyncDActionsEvent::~SyncDActionsEvent()
{
}

InVRsDeformActionManager* SyncDActionsEvent::getInVRsDamInstance()
{
    InVRsDeformActionManager* invrsDam = InVRsDeformActionManager::GetInstance();

    if (invrsDam == 0)
        printd(ERROR, "SyncDActionsEvent::SyncDActionsEvent(): cannot get InVRsDeformActionManager instance!\n");
    return invrsDam;
}

// encode event for all actions of all entities

// MESSAGE:
// 1. [numOfDeques]
//     -- for all deques --
// 2.   [environmentBasedID]
// 3.   [numOfActions]
//          -- for all actions --
// 4.       [action]
void SyncDActionsEvent::encode(NetMessage *message)
{
    printd(INFO, "SyncDActionsEvent::encode(): Encoding sync event\n");
    InVRsDeformActionManager* invrsDam = getInVRsDamInstance();

    DAction* dAction;
    int numOfDeques = invrsDam->getNumOfDeques();

    // 1. [numOfDeques]
    msgFunctions::encode(numOfDeques, message);

    for (int e = 0; e < numOfDeques; ++e)
    {
        const OpenSGLatticeActionDeque* osglad =
            invrsDam->getDequeByIndex(e);

        Entity* entity = invrsDam->getEntityByIndex(e);

        // 2. [environmentBasedID]
        msgFunctions::encode(entity->getEnvironmentBasedId(), message);

        // 3. [numOfActions]
        int numOfActions = osglad->getDequeSize();
        msgFunctions::encode(numOfActions, message);

        // 4. [actions]
        for (int a = 0; a < numOfActions; ++a)
        {
            dAction = const_cast<DAction *> (osglad->getActionByIndex(a));

            BendDeformAction* bda = dynamic_cast<BendDeformAction*>
                (const_cast<DAction*> (dAction));
            TwistDeformAction* wda = dynamic_cast<TwistDeformAction*>
                (const_cast<DAction*> (dAction));
            TaperDeformAction* tda = dynamic_cast<TaperDeformAction*>
                (const_cast<DAction*> (dAction));
            SetPointDeformAction* sda = dynamic_cast<SetPointDeformAction*>
                (const_cast<DAction*> (dAction));
            GlobalDeformAction* gda = (dynamic_cast<GlobalDeformAction*>
                (const_cast<DAction*>(dAction)));
            InsertLatticeDAction* ida = dynamic_cast<InsertLatticeDAction*>
                (const_cast<DAction*> (dAction));
            RemoveLatticeDAction* rda = dynamic_cast<RemoveLatticeDAction*>
                (const_cast<DAction*> (dAction));
            ExecuteDAction* eda = dynamic_cast<ExecuteDAction*>
                (const_cast<DAction*> (dAction));

            // BendDeformActionEvent(float bendFactor, int axis);
            if (bda != 0)
            {
                msgFunctions::encode((int)BEND_ACTION, message);
                msgFunctions::encode(bda->getBendFactor(), message);
                msgFunctions::encode(bda->getCenter(), message);
                msgFunctions::encode(bda->getAxis(), message);
            }
            // TwistDeformActionEvent(float twistFactor, int axis);
            else if (wda != 0)
            {
                msgFunctions::encode((int)TWIST_ACTION, message);
                msgFunctions::encode(wda->getTwistFactor(), message);
                msgFunctions::encode(wda->getAxis(), message);
            }
            // TaperDeformActionEvent(float taperFactor, int axis);
            else if (tda != 0)
            {
                msgFunctions::encode((int)TAPER_ACTION, message);
                msgFunctions::encode(tda->getTaperFactor(), message);
                msgFunctions::encode(tda->getAxis(), message);
            }

            // SetPointDeformActionEvent(gmtl::Vec3i index,
            //    gmtl::Vec3f position);
            else if (sda != 0)
            {
                msgFunctions::encode((int)SETPOINT_ACTION, message);
                gmtl::Vec3i index = sda->getIndex();
                for (int i = 0; i < 3; ++i)
                    msgFunctions::encode(index[i], message);
                msgFunctions::encode(sda->getPosition(), message);
            }
            // GlobalDeformActionEvent(gmtl::Matrix44f dMatrix, bool center);
            else if (gda != 0)
            {
                msgFunctions::encode((int)GLOBAL_ACTION, message);
                gmtl::Matrix44f dMatrix = gda->getMatrix();
                for (int r = 0; r < 4; ++r)
                    for (int c = 0; c < 4; ++c)
                        msgFunctions::encode(dMatrix[r][c], message);
                msgFunctions::encode(gda->isCenter(), message);
            }
            // InsertLatticeDActionEvent(unsigned int environmentBasedId,
            // int hDiv, int wDiv, int lDiv, size_t dim, bool masked,
            // gmtl::Vec3f aabbMin, gmtl::Vec3f aabbMax, float epsilon)
            else if (ida != 0)
            {
                msgFunctions::encode((int)INSERT_ACTION, message);
                msgFunctions::encode(ida->getHdiv(), message);
                msgFunctions::encode(ida->getWdiv(), message);
                msgFunctions::encode(ida->getLdiv(), message);
                msgFunctions::encode((unsigned) ida->getDim(), message);
                msgFunctions::encode(ida->isMasked(), message);
                gmtl::AABoxf aabb = ida->getAabb();
                msgFunctions::encode(aabb.getMin(), message);
                msgFunctions::encode(aabb.getMax(), message);
                msgFunctions::encode(ida->getEpsilon(), message);
            }
            // RemoveLatticeDActionEvent();
            else if (rda != 0)
            {
                msgFunctions::encode((int)REMOVE_ACTION, message);
            }
            // ExecuteDActionEvent(bool resetAabb,
            //    gmtl::Vec3f aabbMin, gmtl::Vec3f aabbMax); // not to send
            else if (eda != 0)
            {
                msgFunctions::encode((int)EXECUTE_ACTION, message);
            }
        }
    }
} // encode

void SyncDActionsEvent::decode(NetMessage *message)
{
    printd(INFO, "SyncDActionsEvent::decode(): Decoding sync event\n");
    InVRsDeformActionManager* invrsDam = getInVRsDamInstance();

    // 1. [numOfDeques]
    int numOfDeques;
    msgFunctions::decode(numOfDeques, message);

    for (int e = 0; e < numOfDeques; ++e)
    {
        // 2. [environmentBasedID]
        unsigned int environmentBasedID = 0;
        msgFunctions::decode(environmentBasedID, message);

        Entity* entity = WorldDatabase::getEntityWithEnvironmentId(
            environmentBasedID);

        // 3. [numOfActions]
        int numOfActions = 0;
        msgFunctions::decode(numOfActions, message);

        // 4. [actions]
        for (int a = 0; a < numOfActions; ++a)
        {
            int actionID = 0;
            msgFunctions::decode(actionID, message);
            DAction* dAction = 0;

            switch ((ACTIONID)actionID)
            {
                // BendDeformActionEvent(float bendFactor, int axis);
                case BEND_ACTION:
                {
                    float bendFactor, center;
                    int axis;

                    msgFunctions::decode(bendFactor, message);
                    msgFunctions::decode(center, message);
                    msgFunctions::decode(axis, message);

                    invrsDam->bendLocal(entity, bendFactor, center, axis);
                    break;
                }
                // TwistDeformActionEvent(float twistFactor, int axis);
                case TWIST_ACTION:
                {
                    float twistFactor;
                    int axis;

                    msgFunctions::decode(twistFactor, message);
                    msgFunctions::decode(axis, message);

                    invrsDam->twistLocal(entity, twistFactor , axis);
                    break;
                }
                // TaperDeformActionEvent(float taperFactor, int axis);
                case TAPER_ACTION:
                {
                    float taperFactor;
                    int axis;

                    msgFunctions::decode(taperFactor, message);
                    msgFunctions::decode(axis, message);

                    invrsDam->taperLocal(entity, taperFactor, axis);
                    break;
                }
                // SetPointDeformActionEvent(gmtl::Vec3i index,
                //    gmtl::Vec3f position);
                case SETPOINT_ACTION:
                {
                    gmtl::Vec3f position;
                    gmtl::Vec3i index;

                    for (int i = 0; i < 3; ++i)
                        msgFunctions::decode(index[i], message);
                    msgFunctions::decode(position, message);

                    invrsDam->setPointLocal(entity, index, position);
                    break;
                }
                // GlobalDeformActionEvent(gmtl::Matrix44f dMatrix,
                //    bool center);
                case GLOBAL_ACTION:
                {
                    gmtl::Matrix44f dMatrix;
                    bool center;

                    for (int r = 0; r < 4; ++r)
                        for (int c = 0; c < 4; ++c)
                            msgFunctions::decode(dMatrix[r][c], message);
                    msgFunctions::decode(center, message);

                    invrsDam->deformLocal(entity, dMatrix, center);
                    break;
                }
                // InsertLatticeDActionEvent(unsigned int environmentBasedId,
                // int hDiv, int wDiv, int lDiv, size_t dim, bool masked,
                // gmtl::Vec3f aabbMin, gmtl::Vec3f aabbMax, float epsilon)
                case INSERT_ACTION:
                {
                    unsigned dim;
                    int hDiv, wDiv, lDiv;
                    bool masked;
                    gmtl::Vec3f aabbMin, aabbMax;
                    float epsilon;

                    msgFunctions::decode(hDiv, message);
                    msgFunctions::decode(wDiv, message);
                    msgFunctions::decode(lDiv, message);
                    msgFunctions::decode(dim, message);
                    msgFunctions::decode(masked, message);
                    msgFunctions::decode(aabbMin, message);
                    msgFunctions::decode(aabbMax, message);
                    msgFunctions::decode(epsilon, message);

                    invrsDam->insertLatticeLocal(entity, hDiv, wDiv, lDiv,
                        (size_t) dim, masked, gmtl::AABoxf(aabbMin, aabbMax),
                        epsilon);

                    break;
                }
                // RemoveLatticeDActionEvent();
                case REMOVE_ACTION:
                {
                    invrsDam->removeLatticeLocal(entity);
                    break;
                }
                // ExecuteDActionEvent(gmtl::Vec3f aabbMin,gmtl::Vec3f aabbMax);
                case EXECUTE_ACTION:
                {
                    invrsDam->executeFfdLocal(entity);
                    break;
                }
                default:
                    break;
            }
        }
    }
}

void SyncDActionsEvent::execute()
{
}
