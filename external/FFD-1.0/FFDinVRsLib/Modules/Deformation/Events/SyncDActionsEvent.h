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

/*---------------------------------------------------------------------------*\
 *                                Changes                                    *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
\*---------------------------------------------------------------------------*/


#ifndef SYNCDACTIONSEVENT_H_INCLUDED
#define SYNCDACTIONSEVENT_H_INCLUDED

#include "../InVRsDeformActionManager.h"

#include <inVRs/SystemCore/DataTypes.h>
#include <inVRs/SystemCore/EventManager/Event.h>
#include <inVRs/SystemCore/EventManager/EventFactory.h>

#include "ActionEvent.h"

/*******************************************************************************
 * @class SyncDActionsEvent
 * @brief Encodes, decodes and executes all deformations which have been applied
 *        to any nodes in the past.
 */
class SyncDActionsEvent : public ActionEvent
{
    public:
        SyncDActionsEvent();
        virtual ~SyncDActionsEvent();
        InVRsDeformActionManager* getInVRsDamInstance();

        typedef EventFactory<SyncDActionsEvent> Factory;

        enum ACTIONID
        {
            NO_ACTION = 0,
            INSERT_ACTION = 1,
            EXECUTE_ACTION,
            REMOVE_ACTION,
            BEND_ACTION,
            TWIST_ACTION,
            TAPER_ACTION,
            SETPOINT_ACTION,
            GLOBAL_ACTION,
            UNKNOWN_ACTION,
        };

        virtual void encode(NetMessage *message);
        virtual void decode(NetMessage *message);

        virtual void execute();
};

#endif // SYNCDACTIONSEVENT_H_INCLUDED
