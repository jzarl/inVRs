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


#ifndef SETPOINTDEFORMACTIONEVENT_H_INCLUDED
#define SETPOINTDEFORMACTIONEVENT_H_INCLUDED

#include <gmtl/Vec.h>

#include <inVRs/SystemCore/DataTypes.h>
#include <inVRs/SystemCore/EventManager/Event.h>
#include <inVRs/SystemCore/EventManager/EventFactory.h>

#include "ffd/Actions/SetPointDeformAction.h"
#include "ActionEvent.h"

/*******************************************************************************
 * @class SetPointDeformActionEvent
 * @brief Encodes, decodes, and executes the set point deformation on a lattice.
 */
class SetPointDeformActionEvent : public ActionEvent, public SetPointDeformAction
{
    public:
        SetPointDeformActionEvent(unsigned int environmentBasedId, gmtl::Vec3i
            index, gmtl::Vec3f position);
        SetPointDeformActionEvent();
        virtual ~SetPointDeformActionEvent();

        typedef EventFactory<SetPointDeformActionEvent> Factory;

        virtual void encode(NetMessage *message);
        virtual void decode(NetMessage *message);
        virtual void execute();

    private:
        unsigned int environmentBasedId;
};

#endif // SETPOINTDEFORMACTIONEVENT_H_INCLUDED
