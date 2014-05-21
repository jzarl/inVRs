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


#ifndef DEFORMATION_H_INCLUDED
#define DEFORMATION_H_INCLUDED

#include "Events/SyncDActionsEvent.h"
#include <inVRs/SystemCore/EventManager/EventManager.h>

#include <inVRs/SystemCore/ComponentInterfaces/ModuleInterface.h>

#define DEFORMATION_MODULE_ID 30

/*******************************************************************************
 * @class Deformation
 * @brief This deformation module for inVRs takes care about synchronization in
 *        multiuser applications. Documentation about the synchronization can
 *        be found in the @see SyncDActionsEvent.h
 *
 */
class Deformation : public ModuleInterface
{
    public:

       	/**
         * Constructor.
         */
        Deformation();

       	/**
         * Destructor.
         */
        virtual ~Deformation();

       	/**
         * Loads the configuration for synchronisation which is empty at the
         * moment.
         */
        virtual bool loadConfig(std::string configFile);

       	/**
         * Cleanup ressources.
         */
        virtual void cleanup();

       	/**
         * Returns the name of the class: "DeformationSyncModule"
         */
        virtual std::string getName();

       	/**
         * Creates a synchronisation event.
         */
        virtual Event* createSyncEvent();

        /**
         * Step
         */
        void step(float dt);

    private:
        EventPipe* incomingEvents;


};

#endif // DEFORMATION_H_INCLUDED
