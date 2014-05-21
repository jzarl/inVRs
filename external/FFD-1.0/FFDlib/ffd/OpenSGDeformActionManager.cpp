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


#include "OpenSGDeformActionManager.h"

#include <gmtl/External/OpenSGConvert.h>

#include "Actions/InsertLatticeDAction.h"
#include "Actions/RemoveLatticeDAction.h"
#include "Actions/ExecuteDAction.h"
#include "Actions/GlobalDeformAction.h"
#include "Actions/SetPointDeformAction.h"
#include "Actions/TaperDeformAction.h"
#include "Actions/TwistDeformAction.h"
#include "Actions/BendDeformAction.h"

OSG_USING_NAMESPACE

OpenSGDeformActionManager* OpenSGDeformActionManager::osgDamInstance = 0;

void OpenSGDeformActionManager::Destroy()
{
    if(osgDamInstance != 0)
    {
        size_t dns = osgDamInstance->deformNodes.size();
        for (size_t i = 0; i < dns; ++i)
            delete osgDamInstance->deformNodes[i];
        delete osgDamInstance;
        osgDamInstance = 0;
    }
}

void OpenSGDeformActionManager::setShowAll(NodePtr subScene, bool showAll)
{
    OpenSGLatticeActionDeque* daDeque = getDaDeque(subScene);
    if (daDeque != 0)
        daDeque->setShowAll(showAll);
}

void OpenSGDeformActionManager::setInstantExecution(NodePtr subScene, bool
    doExecute)
{
    OpenSGLatticeActionDeque* daDeque = getDaDeque(subScene);
    if ((daDeque != 0 ) && (daDeque->getInstantExecution() != doExecute))
    {
        if (doExecute == true)
        {
            if((dynamic_cast<const ExecuteDAction* >
                    (daDeque->getLastAction()) == 0) &&
                (dynamic_cast<const InsertLatticeDAction* >
                    (daDeque->getLastAction()) == 0))
                executeFfd(subScene);
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
                executeFfd(subScene);
            }
        }
    }
}

void OpenSGDeformActionManager::setOptimize(bool doOptimize)
{
    OpenSGDeformActionManager::doOptimize = doOptimize;
}

void OpenSGDeformActionManager::setPoint(NodePtr subScene, gmtl::Vec3i index,
    const gmtl::Vec3f position)
{
    OpenSGLatticeActionDeque* daDeque = getDaDeque(subScene);
    if ((daDeque != 0) && (isDeformActionAllowed(daDeque)))
    {
        SetPointDeformAction* dAction = new
            SetPointDeformAction(index, position);
        handleAction(subScene, dAction, daDeque);
    }
}

void OpenSGDeformActionManager::setPoints(NodePtr subScene, const
    vector<gmtl::Vec3i>& indices, const vector<gmtl::Vec3f>& positions)
{
    OpenSGLatticeActionDeque* daDeque = getDaDeque(subScene);
    if ((daDeque != 0) && (isDeformActionAllowed(daDeque)))
    {
        size_t pSize = indices.size();
        if (pSize == positions.size())
        {
            for (size_t i = 0; i < pSize; ++i)
            {
                SetPointDeformAction* dAction = new
                    SetPointDeformAction(indices[i], positions[i]);
                dAction->deform(daDeque->getLattice());
                daDeque->insertAction(dAction);
            }
            if (daDeque->getInstantExecution())
                execute(subScene, *daDeque);
            update(*daDeque);
        }
    }
}

void OpenSGDeformActionManager::setPoints(NodePtr subScene, const
    vector<gmtl::Vec3i>& indices, const gmtl::Vec3f addPosition)
{
    OpenSGLatticeActionDeque* daDeque = getDaDeque(subScene);
    if ((daDeque != 0) && (isDeformActionAllowed(daDeque)))
    {
        size_t pSize = indices.size();
        for (size_t i = 0; i < pSize; ++i)
        {
            SetPointDeformAction* dAction = new
                SetPointDeformAction(indices[i], addPosition);
            dAction->deform(daDeque->getLattice());
            daDeque->insertAction(dAction);
        }
        if (daDeque->getInstantExecution())
            execute(subScene, *daDeque);
        update(*daDeque);
    }
}

gmtl::Vec3i OpenSGDeformActionManager::getCellDivisions(NodePtr subScene)
{
    gmtl::Vec3i cDivs;
    OpenSGLatticeActionDeque* daDeque = getDaDeque(subScene);
    if (daDeque != 0)
        cDivs = daDeque->getLattice().getCellDivisions();
    return cDivs;
}

vector<gmtl::Vec3i> OpenSGDeformActionManager::getSelection(
    NodePtr subScene)
{
    OpenSGLatticeActionDeque* daDeque = getDaDeque(subScene);
    return daDeque->getLatticeGeometry().getSelection();
}

gmtl::AABoxf OpenSGDeformActionManager::getAabb(NodePtr subScene)
{
    gmtl::AABoxf aabb;
    Pnt3f aabbMin, aabbMax;
#if OSG_MAJOR_VERSION >= 2
	subScene->editVolume(true).getBounds(aabbMin, aabbMax);
#else
	subScene->getVolume(true).getBounds(aabbMin, aabbMax);
#endif
    aabb.setMin(gmtl::Vec3f(aabbMin[0], aabbMin[1], aabbMin[2]));
    aabb.setMax(gmtl::Vec3f(aabbMax[0], aabbMax[1], aabbMax[2]));
    return aabb;
}

bool OpenSGDeformActionManager::getHitPoint(SimpleSceneManager* mgr,
    OpenSGLatticeActionDeque* daDeque, int x, int y, gmtl::Vec3f& hitPoint)
{
	Line ray = mgr->calcViewRay(x, y);
#if OSG_MAJOR_VERSION >= 2
	IntersectAction::ObjRefPtr iAct;
#else
    IntersectAction *iAct;
#endif
    iAct = IntersectAction::create();
    iAct->setLine(ray);
    iAct->apply(daDeque->getLatticeNodePtr()->getParent());
    Pnt3f p = iAct->getHitPoint();

    gmtl::Vec3i cellIndex;
    if (iAct->didHit())
    {
        NodePtr n = iAct->getHitObject();
        //mgr->setHighlight(iAct->getHitObject());
        std::cout << "hit: " << n << std::endl;

        hitPoint = gmtl::Vec3f(p[0], p[1], p[2]);
        return true;
    }
    return false;
}

gmtl::Vec3i OpenSGDeformActionManager::getNearestNeighbourIndex(
    gmtl::Vec3f& selection, NodePtr subScene)
{
    gmtl::Vec3i index;
    OpenSGLatticeActionDeque* daDeque = getDaDeque(subScene);
    if (daDeque != 0)
        index = daDeque->getLattice().getNearestNeighbourLattice(selection);

    return index;
}

int OpenSGDeformActionManager::getNumOfDeques() const
{
    return osgDamInstance->deformNodes.size();
}

OpenSGLatticeActionDeque* OpenSGDeformActionManager::getDaDeque(NodePtr node)
{
    for (Davec::iterator dav = deformNodes.begin(); dav != deformNodes.end();
        ++dav)
    {
        if ((*dav)->getNodePtr() == node)
            return (*dav);
    }
    return 0;
}

bool OpenSGDeformActionManager::getPoint(NodePtr subScene,
    const gmtl::Vec3i& index, gmtl::Vec3f& position)
{
    OpenSGLatticeActionDeque* daDeque = getDaDeque(subScene);
    if (daDeque != 0)
        return daDeque->getLattice().getCellPoint(index, position);
    return false;
}

const OpenSGLatticeActionDeque* OpenSGDeformActionManager::
    getDequeByIndex(int index) const
{
    if ((index >= 0) && (index < (int) osgDamInstance->deformNodes.size()))
        return osgDamInstance->deformNodes[index];
    return 0;
}

bool OpenSGDeformActionManager::getOptimize()
{
    return doOptimize;
}

bool OpenSGDeformActionManager::isInstantExecution(NodePtr subScene)
{
    OpenSGLatticeActionDeque* daDeque = getDaDeque(subScene);
    if (daDeque != 0)
        return daDeque->getInstantExecution();
    else
        return false;
}

int OpenSGDeformActionManager::optimize()
{
    int sumRemovedActions = 0;
    for (Davec::iterator dav = osgDamInstance->deformNodes.begin();
        dav != osgDamInstance->deformNodes.end(); ++dav)
            sumRemovedActions += (*dav)->optimizeActionDeque();
    return sumRemovedActions;
}

bool OpenSGDeformActionManager::insertLattice(NodePtr subScene, int height,
    int width, int length, size_t dim, bool isMasked, gmtl::AABoxf aabb,
    float epsilon)
{
    if (subScene == NullFC)
        return false;

    bool insertAllowed = false;
    OpenSGLatticeActionDeque* daDeque = getDaDeque(subScene);
    // if deque for this node exists, the last action has to be remove
    if (daDeque != 0)
    {
        if((dynamic_cast<const RemoveLatticeDAction* > (
            daDeque->getLastAction()) != 0) || (daDeque->getLastAction() == 0))
            insertAllowed = true;
    }
    else
    {
        // deque does not exist: check that there is no lattice in the subScene
        if(!hasSubSceneLattice(subScene) && (!hasParentLattice(subScene)))
        {
            // create a new deque and set the NodePtr and add it
            daDeque = new OpenSGLatticeActionDeque();
            if (daDeque != 0)
            {
                daDeque->setNodePtr(subScene);
                deformNodes.push_back(daDeque);
                insertAllowed = true;
            }
        }
    }
    if (insertAllowed && (!hasSubSceneLattice(subScene)) &&
        (!hasParentLattice(subScene)))
    {
        // if AABB is not customized, get the AABB of the subScene
        if (!isMasked)
            aabb = getAabb(subScene);

        daDeque->setLattice(insert(subScene, height, width, length, dim,
            isMasked, aabb, epsilon, *daDeque));
        daDeque->insertAction(new InsertLatticeDAction(height, width,
            length, dim, isMasked, aabb.getMin(), aabb.getMax(), epsilon));

        if (daDeque->getInstantExecution())
            daDeque->getModelPointManager().createSavepoint();

        update(*daDeque);
        return true;
    }
    return false;
}

bool OpenSGDeformActionManager::removeLattice(NodePtr subScene)
{
    OpenSGLatticeActionDeque* daDeque = getDaDeque(subScene);
    if(daDeque != 0)
    {
        if(remove(subScene))
        {
            daDeque->insertAction(new RemoveLatticeDAction());
            // with instant execution and no execute call savepoint has to be
            // restored
            if (daDeque->getInstantExecution())
            {
                daDeque->getModelPointManager().restoreSavepoint();
                if(daDeque->getModelPointManager().getMasked())
                    daDeque->getModelPointManager().assignModelPointsMasked();
                else
                    daDeque->getModelPointManager().assignModelPoints();
            }
            daDeque->setLatticeRemoved();
            return true;
        }
    }
    return false;
}

void OpenSGDeformActionManager::executeFfd(NodePtr subScene)
{
    if (subScene != NullFC)
    {
        OpenSGLatticeActionDeque* daDeque = getDaDeque(subScene);
        if(isDeformActionAllowed(daDeque))
        {
            gmtl::AABoxf aabb;

            // if doExecute is true execute FFD and overwrite the savepoint
            // with the current state
            if (daDeque->getInstantExecution())
            {
                // execute and reset AABB like normal execution
                aabb = execute(subScene, *daDeque, true);
                daDeque->getModelPointManager().createSavepoint();
            }
            else
                aabb = execute(subScene, *daDeque);

            daDeque->insertAction(new ExecuteDAction(aabb.getMin(),
                aabb.getMax()));

            update(*daDeque);
        }
    }
}

void OpenSGDeformActionManager::bend(NodePtr subScene, const float bendFactor,
    const float center, const int axis)
{
    OpenSGLatticeActionDeque* daDeque = getDaDeque(subScene);
    if ((daDeque != 0) && (isDeformActionAllowed(daDeque)))
    {
        BendDeformAction* dAction = new BendDeformAction(bendFactor, center,
            axis);
        handleAction(subScene, dAction, daDeque);
    }
}

void OpenSGDeformActionManager::twist(NodePtr subScene, const float twistFactor,
    const int axis)
{
    OpenSGLatticeActionDeque* daDeque = getDaDeque(subScene);
    if ((daDeque != 0) && (isDeformActionAllowed(daDeque)))
    {
        TwistDeformAction* dAction = new TwistDeformAction(twistFactor, axis);
        handleAction(subScene, dAction, daDeque);
    }
}

void OpenSGDeformActionManager::taper(NodePtr subScene, const float taperFactor,
    const int axis)
{
    OpenSGLatticeActionDeque* daDeque = getDaDeque(subScene);
    if ((daDeque != 0) && (isDeformActionAllowed(daDeque)))
    {
        TaperDeformAction* dAction = new TaperDeformAction(taperFactor, axis);
        handleAction(subScene, dAction, daDeque);
    }
}

void OpenSGDeformActionManager::deform(NodePtr subScene,
    const gmtl::Matrix44f dMatrix, const bool center)
{
    OpenSGLatticeActionDeque* daDeque = getDaDeque(subScene);
    if ((daDeque != 0) && (isDeformActionAllowed(daDeque)))
    {
        GlobalDeformAction* dAction = new GlobalDeformAction(dMatrix, center);
        handleAction(subScene, dAction, daDeque);
    }
}

bool OpenSGDeformActionManager::undo(NodePtr subScene)
{
    OpenSGLatticeActionDeque* daDeque = getDaDeque(subScene);
    if (daDeque != 0)
    {
        const DAction* dAction = dynamic_cast<const DAction* >
            (daDeque->getLastAction());
        const DeformAction* deformAction = dynamic_cast<const DeformAction* >
            (dAction);

        if (deformAction != 0)
            return undoDeformAction(subScene, *(const_cast<DeformAction* >
                    (deformAction)), *daDeque);

        if (dAction != 0)
            return undoDaction(dAction, subScene, *daDeque);
    }
    return false;
}

void OpenSGDeformActionManager::update(OpenSGLatticeActionDeque& daDeque)
{
    daDeque.getLatticeGeometry().updateGeometry(daDeque.getShowAll());
}

void OpenSGDeformActionManager::selectLatticeCellPoints(const
    vector<gmtl::Vec3i>& selectPoints, NodePtr subScene, bool append)
{
    OpenSGLatticeActionDeque* daDeque = getDaDeque(subScene);
    if ((daDeque != 0) && (isDeformActionAllowed(daDeque)))
    {
        daDeque->getLatticeGeometry().setSelected(selectPoints, append);
    }
    update(*daDeque);
}

bool OpenSGDeformActionManager::selectLatticeCellPoint(SimpleSceneManager* mgr,
    NodePtr subScene, int x, int y, bool append)
{
    OpenSGLatticeActionDeque* daDeque = getDaDeque(subScene);
    gmtl::Vec3f hitPoint;

    if ((daDeque != 0) && (isDeformActionAllowed(daDeque)))
    {
        if (getHitPoint(mgr, daDeque, x, y, hitPoint))
        {
            daDeque->getLatticeGeometry().setSelected(
                getNearestNeighbourIndex(hitPoint, subScene), append);
            return true;
        }
    }
    return false;
}

void OpenSGDeformActionManager::unselectLatticeCellPoints(NodePtr subScene)
{
    OpenSGLatticeActionDeque* daDeque = getDaDeque(subScene);
    if ((daDeque != 0) && (isDeformActionAllowed(daDeque)))
        daDeque->getLatticeGeometry().setUnselected();
    update(*daDeque);
}

bool OpenSGDeformActionManager::unselectLatticeCellPoint(SimpleSceneManager*
    mgr, NodePtr subScene, int x, int y)
{
    OpenSGLatticeActionDeque* daDeque = getDaDeque(subScene);
    gmtl::Vec3f hitPoint;

    if ((daDeque != 0) && (isDeformActionAllowed(daDeque)))
    {
        if (getHitPoint(mgr, daDeque, x, y, hitPoint))
        {
            daDeque->getLatticeGeometry().removeSelected(
                getNearestNeighbourIndex(hitPoint, subScene));
            return true;
        }
    }
    return false;
}

void OpenSGDeformActionManager::dump()
{
    for (Davec::iterator dav = deformNodes.begin(); dav != deformNodes.end();
        ++dav)
    {
        cout << "*** ActionDeque ***\n" << **dav << endl;
    }
}

void OpenSGDeformActionManager::dump(NodePtr subScene)
{
    OpenSGLatticeActionDeque* daDeque = getDaDeque(subScene);
    cout << "*** ActionDeque ***\n" << *daDeque << endl;
}

bool OpenSGDeformActionManager::hasSubSceneLattice(NodePtr subScene)
{
    latticeExists = false;
#if OSG_MAJOR_VERSION >= 2
	traverse(subScene, boost::bind(&OpenSGDeformActionManager::findLattice,this,_1));
#else //OpenSG1:
	traverse(subScene, osgTypedMethodFunctor1ObjPtrCPtrRef
        <Action::ResultE, OpenSGDeformActionManager, NodePtr>
             (this, &OpenSGDeformActionManager::findLattice)
    );
#endif

    if(latticeExists)
        return true;
    return false;
}

bool OpenSGDeformActionManager::hasParentLattice(NodePtr subScene)
{
    NodePtr tmp = subScene;

    while (tmp != NullFC)
    {
        if (getName(tmp) && (getName(tmp) == latticeNodeName))
            return true;
        tmp = tmp->getParent();
    }

    return false;
}

bool OpenSGDeformActionManager::isDeformActionAllowed(
    OpenSGLatticeActionDeque* daDeque)
{
    if ((daDeque != 0) && (daDeque->getLastAction() != 0) &&
        (dynamic_cast<const RemoveLatticeDAction* >
            (daDeque->getLastAction()) == 0))
            return true;
    return false;
}


bool OpenSGDeformActionManager::isInsertActionAllowed(
    OpenSGLatticeActionDeque* daDeque)
{
    if (daDeque != 0)
    {
        if((daDeque->getDequeSize() == 0) ||
           ((daDeque->getLastAction() != 0) &&
            (dynamic_cast<const RemoveLatticeDAction* >
            (daDeque->getLastAction()) != 0)))
                return true;
    }
    return false;
}

bool OpenSGDeformActionManager::isRemoveActionAllowed(
    OpenSGLatticeActionDeque* daDeque)
{
    if ((daDeque != 0) && (daDeque->getDequeSize() != 0) &&
        (daDeque->getIndexOfLastAction<InsertLatticeDAction*>() >
         daDeque->getIndexOfLastAction<RemoveLatticeDAction*>()))
        return true;
    return false;
}

bool OpenSGDeformActionManager::isInvertible(int fromInd, int toInd,
    OpenSGLatticeActionDeque& daDeque)
{
    bool isInvertible = true;
    for (;fromInd >= toInd; --fromInd)
    {
        DeformAction* deformAction = dynamic_cast<DeformAction* >
            (const_cast<DAction* >(daDeque.getActionByIndex(fromInd)));
        if ((deformAction != 0) && (!deformAction->isInvertible()))
            isInvertible = false;
    }

    return isInvertible;
}

bool OpenSGDeformActionManager::undoDaction(const DAction* dAction,
    NodePtr subScene, OpenSGLatticeActionDeque& daDeque)
{
    // INSERT: if last action was insert, just remove the lattice and reset
    //         parameters
    if ((dynamic_cast<const InsertLatticeDAction* > (dAction)) != 0)
    {
        if(remove(subScene))
        {
            daDeque.removeLastAction();
            daDeque.setLatticeRemoved();
        }
        return true;
    }

    // REMOVE: if last action was remove lattice, reconstruct the lattice
    if ((dynamic_cast<const RemoveLatticeDAction*> (dAction)) != 0)
    {
        int lastInsInd =
            daDeque.getIndexOfLastAction<InsertLatticeDAction*>();
        int lastExInd = daDeque.getIndexOfLastAction<ExecuteDAction*>();
        int prevAction = daDeque.getDequeSize() - 2;
        int undoToInd = 1;
        gmtl::AABoxf aabb;

        // create a new lattice with parameters of last inserted or via execute
        // set lattice
        InsertLatticeDAction* iAction =
            (dynamic_cast<InsertLatticeDAction* >
            (const_cast<DAction *> (daDeque.getActionByIndex(lastInsInd))));
        ExecuteDAction* eAction =
            (dynamic_cast<ExecuteDAction* >
            (const_cast<DAction *> (daDeque.getActionByIndex(lastExInd))));

        if (lastExInd > lastInsInd)
        {
            aabb = eAction->getAabb();
            undoToInd += lastExInd;
        }
        else
        {
            aabb = iAction->getAabb();
            undoToInd += lastInsInd;
        }

        // insert the lattice
        daDeque.setLattice(insert(subScene, iAction->getHdiv(),
                iAction->getWdiv(), iAction->getLdiv(), iAction->getDim(),
                iAction->isMasked(), aabb, iAction->getEpsilon(), daDeque));

        // simple case: remove was exactly after insert or execute, just remove
        //              the last action
        if ((lastInsInd == prevAction) || (lastExInd == prevAction))
        {
            daDeque.removeLastAction();
            update(daDeque);
            return true;
        }
    }

    // EXECUTE: if last action was the execute action
    if ((dynamic_cast<const ExecuteDAction* > (dAction)) != 0)
    {
        const int prevAction = daDeque.getDequeSize() - 2;

        // last action was insert or also execute: just remove the last action
        if (((dynamic_cast<const InsertLatticeDAction* >
                (daDeque.getActionByIndex(prevAction))) != 0) ||
            ((dynamic_cast<const ExecuteDAction* >
                (daDeque.getActionByIndex(prevAction))) != 0))
        {
            daDeque.removeLastAction();
            return true;
        }
    }

    return false;
}

bool OpenSGDeformActionManager::undoDeformAction(NodePtr subScene, DeformAction&
    deformAction, OpenSGLatticeActionDeque& daDeque)
{
    if (deformAction.isInvertible())
    {
        deformAction.deformInverse(daDeque.getLattice());
        if (daDeque.getInstantExecution())
            execute(subScene, daDeque);
        daDeque.removeLastAction();
        update(daDeque);
        return true;
    }
    return false;
}

void OpenSGDeformActionManager::handleAction(NodePtr subScene, DeformAction*
    dAction, OpenSGLatticeActionDeque* daDeque)
{
    dAction->deform(daDeque->getLattice());
    daDeque->insertAction(dAction);
    if (daDeque->getInstantExecution())
        execute(subScene, *daDeque);
    update(*daDeque);
}

Lattice* OpenSGDeformActionManager::insert(NodePtr subScene, int height,
    int width, int length, size_t dim, bool isMasked, const gmtl::AABoxf& aabb,
    float epsilon, OpenSGLatticeActionDeque& daDeque)
{
    Lattice* lattice = new Lattice(gmtl::Vec3i(height, width, length), aabb,
    dim, epsilon);

    if (lattice != 0)
    {
        OpenSGLatticeGeometry* osglg = new OpenSGLatticeGeometry(*lattice);
        GeometryPtr geo = osglg->createGeometry();

        if ((geo != NullFC) && (osglg != 0))
        {
            NodePtr latticeNode = Node::create();
            setName(latticeNode, latticeNodeName);
            NodePtr parent = subScene->getParent();

            beginEditCP(latticeNode, Node::CoreFieldMask);
                latticeNode->setCore(geo);
            endEditCP(latticeNode, Node::CoreFieldMask);

            addRefCP(subScene);
            beginEditCP(parent, Node::ChildrenFieldMask);
                parent->subChild(subScene);
                parent->addChild(latticeNode);
            endEditCP(parent, Node::ChildrenFieldMask);

            beginEditCP(latticeNode, Node::ChildrenFieldMask);
                latticeNode->addChild(subScene);
            endEditCP(latticeNode, Node::ChildrenFieldMask);
            subRefCP(subScene);

            daDeque.setLatticeGeometry(osglg);
            daDeque.setGeometryPtr(geo);
            daDeque.setLatticeNode(latticeNode);

            OpenSGModelPointManager* osgmpm = new
                OpenSGModelPointManager(subScene);

            if (osgmpm != 0)
            {
                if (isMasked)
                    osgmpm->collectModelPointsMasked(aabb);
                else
                    osgmpm->collectModelPoints();

                lattice->assignModelPointsToLatticeCells(osgmpm->accessModelPoints());
                daDeque.setModelPointManager(osgmpm);

                return lattice;
            }
        }
    }
    return 0;
}

bool OpenSGDeformActionManager::remove(NodePtr subScene)
{
    // remove the node only if it is a lattice node
    if (getName(subScene->getParent()) &&
        getName(subScene->getParent()) == latticeNodeName)
    {
        // hang out the lattice node
        NodePtr latticeNode = subScene->getParent();
        NodePtr parent = latticeNode->getParent();
        NodePtr child = latticeNode->getChild(0);

        addRefCP(child);
        beginEditCP(latticeNode, Node::ChildrenFieldMask);
            latticeNode->subChild(child);
        endEditCP(latticeNode, Node::ChildrenFieldMask);

        beginEditCP(parent, Node::ChildrenFieldMask);
            parent->subChild(latticeNode);
            parent->addChild(child);
        endEditCP(parent, Node::ChildrenFieldMask);
        subRefCP(child);

        return true;
    }
    return false;
}

gmtl::AABoxf OpenSGDeformActionManager::execute(NodePtr subScene,
    OpenSGLatticeActionDeque& daDeque, bool resetAabb)
{
    // get the lattice and run the FFD on the modelpoints
    gmtl::AABoxf aabb;
    Lattice& lattice = daDeque.getLattice();

    // 1. execute FFD and assign deformed modelpoints to model(s)
    if (daDeque.getInstantExecution())
        lattice.executeFFD(daDeque.getModelPointManager().accessModelPointsSavepoint(),
            daDeque.getModelPointManager().accessModelPoints());
    else
        lattice.executeFFD(daDeque.getModelPointManager().accessModelPoints(),
            daDeque.getModelPointManager().accessModelPoints());

    if (daDeque.getModelPointManager().getMasked())
        daDeque.getModelPointManager().assignModelPointsMasked();
    else
        daDeque.getModelPointManager().assignModelPoints();

    // 2. recollect modelpoints for the new lattice (if it has changed)
    if ((!daDeque.getInstantExecution()) || (resetAabb))
    {
        // assign deformed modelPoints to the model(s)
        if (daDeque.getModelPointManager().getMasked())
        {
            // aabb stays the same, but affected model points have changed,
            // so collect affected points again
            aabb = daDeque.getModelPointManager().getAffectedVolume();
            daDeque.getModelPointManager().collectModelPointsMasked(aabb);
        }
        else
        {
            aabb = getAabb(subScene);
        }

        daDeque.getLattice().setBounds(aabb);
        // and set the current lattice points to the old lattice points
        lattice.shiftLastCellPoints();
        // reassign model points to the new cells
        daDeque.getLattice().assignModelPointsToLatticeCells(
            daDeque.getModelPointManager().accessModelPoints());
    }

    return aabb;
}

void OpenSGDeformActionManager::createSavePoint(NodePtr subScene)
{
    OpenSGLatticeActionDeque* daDeque = getDaDeque(subScene);
    if(daDeque != 0)
        daDeque->getModelPointManager().createSavepoint();
}

void OpenSGDeformActionManager::deleteSavePoint(NodePtr subScene)
{
    OpenSGLatticeActionDeque* daDeque = getDaDeque(subScene);
    if(daDeque != 0)
        daDeque->getModelPointManager().deleteSavepoint();
}

void OpenSGDeformActionManager::restoreSavePoint(NodePtr subScene)
{
    OpenSGLatticeActionDeque* daDeque = getDaDeque(subScene);
    if(daDeque != 0)
        daDeque->getModelPointManager().restoreSavepoint();
}


#if OSG_MAJOR_VERSION >= 2
	Action::ResultE OpenSGDeformActionManager::findLattice(Node* node) 
#else //OpenSG1:
	Action::ResultE OpenSGDeformActionManager::findLattice(NodePtr& node)
#endif
{

	if (getName(node) && (getName(node) == latticeNodeName))
    {
        latticeExists = true;
        return Action::Quit;
    }
    return Action::Continue;
}

