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
#include "InVRsDeformActionManager.h"

#include "ffd/Actions/BendDeformAction.h"
#include "ffd/Actions/TwistDeformAction.h"
#include "ffd/Actions/TaperDeformAction.h"
#include "ffd/Actions/GlobalDeformAction.h"
#include "ffd/Actions/SetPointDeformAction.h"

#include "Events/BendDeformActionEvent.h"
#include "Events/TwistDeformActionEvent.h"
#include "Events/TaperDeformActionEvent.h"
#include "Events/GlobalDeformActionEvent.h"
#include "Events/SetPointDeformActionEvent.h"
#include "Events/InsertLatticeDActionEvent.h"
#include "Events/ExecuteDActionEvent.h"
#include "Events/RemoveLatticeDActionEvent.h"
#include "Events/UndoDActionEvent.h"

#include <inVRs/SystemCore/SystemCore.h>
#include <inVRs/SystemCore/EventManager/EventManager.h>
#include <inVRs/OutputInterface/OpenSGSceneGraphInterface/OpenSGSceneGraphInterface.h>

OSG_USING_NAMESPACE

InVRsDeformActionManager* InVRsDeformActionManager::invrsDamInstance = 0;
OpenSGDeformActionManager* InVRsDeformActionManager::osgDamInstance = 0;

void InVRsDeformActionManager::Destroy()
{
    OpenSGDeformActionManager::Destroy();

    delete invrsDamInstance;
    invrsDamInstance = 0;
}

void InVRsDeformActionManager::setInstantExecution(Entity* entity,
    bool doExecute)
{
    OpenSGLatticeActionDeque* daDeque =
        osgDamInstance->getDaDeque(getNodePtr(entity));
    if ((daDeque != 0 ) && (daDeque->getInstantExecution() != doExecute))
    {
        if (doExecute == true)
        {
            if((dynamic_cast<const ExecuteDAction* >
                    (daDeque->getLastAction()) == 0) &&
                (dynamic_cast<const InsertLatticeDAction* >
                    (daDeque->getLastAction()) == 0))
                executeFfd(entity);
            if (daDeque->isDequeActive())
            {
                daDeque->getModelPointManager().createSavepoint();
                daDeque->setInstantExecution(doExecute);
            }
        }
        else
        {
            if (daDeque->isDequeActive())
            {
                daDeque->setInstantExecution(doExecute);
                executeFfd(entity);
            }
        }
    }
}

void InVRsDeformActionManager::setShowAll(Entity* entity, bool showAll)
{
    osgDamInstance->setShowAll(getNodePtr(entity), showAll);

}

void InVRsDeformActionManager::setPointLocal(Entity* entity, const gmtl::Vec3i
    index, const gmtl::Vec3f position)
{
    osgDamInstance->setPoint(getNodePtr(entity), index, position);
}

void InVRsDeformActionManager::setPoint(Entity* entity, const gmtl::Vec3i index,
    const gmtl::Vec3f position)
{
    EventManager::sendEvent(new SetPointDeformActionEvent(
        entity->getEnvironmentBasedId(), index, position),
        EventManager::EXECUTE_GLOBAL);
}

bool InVRsDeformActionManager::getPoint(Entity* entity,
    const gmtl::Vec3i& index, gmtl::Vec3f& position)
{
    return osgDamInstance->getPoint(getNodePtr(entity), index, position);
}

vector<gmtl::Vec3i> InVRsDeformActionManager::getSelection(Entity* entity)
{
    return osgDamInstance->getSelection(getNodePtr(entity));
}

gmtl::AABoxf InVRsDeformActionManager::getAabb(Entity* entity)
{
    return osgDamInstance->getAabb(getNodePtr(entity));
}

Entity* InVRsDeformActionManager::getEntityByIndex(int index)
{
    if ((index >= 0) && (index < entities.size()))
        return entities[index];
}

bool InVRsDeformActionManager::isInstantExecution(Entity* entity)
{
    return osgDamInstance->isInstantExecution(getNodePtr(entity));
}

void InVRsDeformActionManager::selectLatticeCellPoints(const
    vector<gmtl::Vec3i>& selectPoints, Entity* entity, bool append)
{
    osgDamInstance->selectLatticeCellPoints(selectPoints, getNodePtr(entity),
        append);
}

bool InVRsDeformActionManager::selectLatticeCellPoint(SimpleSceneManager* mgr,
    Entity* entity, int x, int y, bool append)
{
    return osgDamInstance->selectLatticeCellPoint(mgr, getNodePtr(entity), x,
        y, append);
}

void InVRsDeformActionManager::unselectLatticeCellPoints(Entity* entity)
{
    osgDamInstance->unselectLatticeCellPoints(getNodePtr(entity));
}

bool InVRsDeformActionManager::unselectLatticeCellPoint(SimpleSceneManager* mgr,
    Entity* entity, int x, int y)
{
    return osgDamInstance->unselectLatticeCellPoint(mgr, getNodePtr(entity), x,
        y);
}

gmtl::Vec3i InVRsDeformActionManager::getNearestNeighbourIndex(gmtl::Vec3f&
    selection, Entity* entity)
{
    osgDamInstance->getNearestNeighbourIndex(selection, getNodePtr(entity));
}

void InVRsDeformActionManager::insertLatticeLocal(Entity* entity, int height,
    int width, int length, size_t dim, bool isMasked, gmtl::AABoxf aabb,
    float epsilon)
{
    NodePtr entityNode = getNodePtr(entity);
    if (entityNode != NullFC)
    {
        OpenSGLatticeActionDeque* daDeque =
            osgDamInstance->getDaDeque(entityNode);

        if (osgDamInstance->insertLattice(entityNode, height, width, length,
            dim, isMasked, aabb, epsilon))
        {
            if (daDeque == 0)
                entities.push_back(entity);
        }
    }
}

void InVRsDeformActionManager::insertLattice(Entity* entity, int height,
    int width, int length, size_t dim, bool isMasked, gmtl::AABoxf aabb,
    float epsilon)
{
    EventManager::sendEvent(new InsertLatticeDActionEvent(
        entity->getEnvironmentBasedId(), height, width, length, dim, isMasked,
                aabb.getMin(), aabb.getMax(), epsilon),
                EventManager::EXECUTE_GLOBAL);
}

void InVRsDeformActionManager::removeLatticeLocal(Entity* entity)
{
    osgDamInstance->removeLattice(getNodePtr(entity));
}

void InVRsDeformActionManager::removeLattice(Entity* entity)
{
    EventManager::sendEvent(new RemoveLatticeDActionEvent
        (entity->getEnvironmentBasedId()), EventManager::EXECUTE_GLOBAL);
}

void InVRsDeformActionManager::executeFfdLocal(Entity* entity)
{
    osgDamInstance->executeFfd(getNodePtr(entity));
}

void InVRsDeformActionManager::executeFfd(Entity* entity)
{
    EventManager::sendEvent(new ExecuteDActionEvent(
        entity->getEnvironmentBasedId(), gmtl::Vec3f(0.0f, 0.0f, 0.0f),
        gmtl::Vec3f(0.0f, 0.0f, 0.0f)), EventManager::EXECUTE_GLOBAL);
}

void InVRsDeformActionManager::bendLocal(Entity* entity, const float bendFactor,
    const float center, const int axis)
{
    osgDamInstance->bend(getNodePtr(entity), bendFactor, center, axis);
}

void InVRsDeformActionManager::bend(Entity* entity, const float bendFactor,
    const float center, const int axis)
{
    EventManager::sendEvent(new BendDeformActionEvent(
        entity->getEnvironmentBasedId(), bendFactor, center, axis),
        EventManager::EXECUTE_GLOBAL);
}

void InVRsDeformActionManager::twistLocal(Entity* entity, const float twistFactor,
    const int axis)
{
    osgDamInstance->twist(getNodePtr(entity), twistFactor, axis);
}

void InVRsDeformActionManager::twist(Entity* entity, const float twistFactor,
    const int axis)
{
    EventManager::sendEvent(new TwistDeformActionEvent(
        entity->getEnvironmentBasedId(), twistFactor, axis),
        EventManager::EXECUTE_GLOBAL);
}

void InVRsDeformActionManager::taperLocal(Entity* entity, const float taperFactor,
    const int axis)
{
    osgDamInstance->taper(getNodePtr(entity), taperFactor, axis);
}

void InVRsDeformActionManager::taper(Entity* entity, const float taperFactor,
    const int axis)
{
    EventManager::sendEvent(new TaperDeformActionEvent(
        entity->getEnvironmentBasedId(), taperFactor, axis),
        EventManager::EXECUTE_GLOBAL);
}

void InVRsDeformActionManager::deformLocal(Entity* entity,
    const gmtl::Matrix44f dMatrix, const bool center)
{
    osgDamInstance->deform(getNodePtr(entity), dMatrix, center);
}

void InVRsDeformActionManager::deform(Entity* entity,
    const gmtl::Matrix44f dMatrix, const bool center)
{
    EventManager::sendEvent(new GlobalDeformActionEvent(
        entity->getEnvironmentBasedId(), dMatrix, center),
        EventManager::EXECUTE_GLOBAL);
}

bool InVRsDeformActionManager::undoLocal(Entity* entity)
{
    return osgDamInstance->undo(getNodePtr(entity));
}

bool InVRsDeformActionManager::undo(Entity* entity)
{
    EventManager::sendEvent(new UndoDActionEvent(
        entity->getEnvironmentBasedId()), EventManager::EXECUTE_GLOBAL);
}

void InVRsDeformActionManager::dump()
{
    osgDamInstance->dump();
}

void InVRsDeformActionManager::dump(Entity* entity)
{
    osgDamInstance->dump(getNodePtr(entity));
}

NodePtr InVRsDeformActionManager::getNodePtr(Entity* entity)
{
    OpenSGModel* osgmodel = dynamic_cast<OpenSGModel*>
        (entity->getVisualRepresentation());
    return osgmodel->getNodePtr();
}
