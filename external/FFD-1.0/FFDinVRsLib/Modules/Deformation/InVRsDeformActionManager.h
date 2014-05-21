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


#ifndef INVRSDEFORMACTIONMANAGER_H_INCLUDED
#define INVRSDEFORMACTIONMANAGER_H_INCLUDED

#include <utility>

#include "ffd/OpenSGDeformActionManager.h"
#include <inVRs/SystemCore/WorldDatabase/WorldDatabase.h>


/*******************************************************************************
 * @class InVRsDeformActionManager
 * @brief This is an adapter of the OpenSGDeformActionManager class. It holds an
 *        OpenSGDeformActionManager instance and delegates all functions to the
 *        OpenSG version of this class, which means all actions are still
 *        related to NodePtr.
 *        OpenSGDeformActionManager.h has the methods documented, here only
 *        inVRs specific documentation will be found.
 */

class InVRsDeformActionManager : public OpenSGDeformActionManager
{
    typedef vector<Entity* > EntityVec;

    public:
       	/**
         * Get singleton instance of the InVRsDeformActionManager.
         */
        static InVRsDeformActionManager* GetInstance()
        {
            if (invrsDamInstance != 0)
                return invrsDamInstance;
            else
            {
                osgDamInstance = OpenSGDeformActionManager::GetInstance();
                if (osgDamInstance != 0)
                {
                    invrsDamInstance = new InVRsDeformActionManager();
                    return invrsDamInstance;
                }
            }
            return 0;
        }

        /**
         * Destroy the instance of InVRsDeformActionManager and clean up.
         */
        static void Destroy();

        void setShowAll(Entity* entity, bool showAll);

        void setInstantExecution(Entity* entity, bool doExecute);

        void setPointLocal(Entity* entity, const gmtl::Vec3i index,
            const gmtl::Vec3f position);

        void setPoint(Entity* entity, const gmtl::Vec3i index,
            const gmtl::Vec3f position);

        vector<gmtl::Vec3i> getSelection(Entity* entity);

        gmtl::AABoxf getAabb(Entity* entity);

        bool getPoint(Entity* entity, const gmtl::Vec3i& index,
            gmtl::Vec3f& position);

        Entity* getEntityByIndex(int index);

        gmtl::Vec3i getNearestNeighbourIndex(gmtl::Vec3f& selection,
            Entity* entity);

        bool isInstantExecution(Entity* entity);

        void insertLatticeLocal(Entity* entity, int height, int width,
            int length, size_t dim = 3, bool isMasked = false,
            gmtl::AABoxf aabb = gmtl::AABoxf(gmtl::Vec3f(0, 0, 0),
                                             gmtl::Vec3f(0, 0, 0)),
            float epsilon = 0.0001f);

        void insertLattice(Entity* entity, int height, int width, int length,
             size_t dim = 3, bool isMasked = false,
            gmtl::AABoxf aabb = gmtl::AABoxf(gmtl::Vec3f(0, 0, 0),
                                             gmtl::Vec3f(0, 0, 0)),
            float epsilon = 0.0001f);

        void removeLatticeLocal(Entity* entity);

        void removeLattice(Entity* entity);

        void executeFfdLocal(Entity* entity);

        void executeFfd(Entity* entity);

        void bendLocal(Entity* entity, const float bendFactor,
            const float center, const int axis);

        void bend(Entity* entity, const float bendFactor, const float center,
            const int axis);

        void twistLocal(Entity* entity, const float twistFactor, const int axis);

        void twist(Entity* entity, const float twistFactor, const int axis);

        void taperLocal(Entity* entity, const float taperFactor, const int axis);

        void taper(Entity* entity, const float taperFactor, const int axis);

        void deformLocal(Entity* entity, const gmtl::Matrix44f dMatrix,
            const bool center);

        void deform(Entity* entity, const gmtl::Matrix44f dMatrix,
            const bool center);

        bool undoLocal(Entity* entity);

        bool undo(Entity* entity);

        void selectLatticeCellPoints(const vector<gmtl::Vec3i>& selectPoints,
            Entity* entity, bool append = true);

        bool selectLatticeCellPoint(OSG::SimpleSceneManager* mgr, Entity* entity,
            int x, int y, bool append = true);

        void unselectLatticeCellPoints(Entity* entity);

        bool unselectLatticeCellPoint(OSG::SimpleSceneManager* mgr, Entity* entity,
            int x, int y);

        void dump(Entity* entity);
        void dump();

        private:
            InVRsDeformActionManager()
            {
            }
            ~InVRsDeformActionManager()
            {
            }
            InVRsDeformActionManager(const OpenSGDeformActionManager& dam);
            InVRsDeformActionManager& operator=(OpenSGDeformActionManager
                const&);

            static InVRsDeformActionManager* invrsDamInstance;
            static OpenSGDeformActionManager* osgDamInstance;
            EntityVec entities;

            OSG::NodePtr getNodePtr(Entity* entity);

};

#endif // INVRSDEFORMACTIONMANAGER_H_INCLUDED
