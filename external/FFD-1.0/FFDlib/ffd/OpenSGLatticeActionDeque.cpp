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


#include "OpenSGLatticeActionDeque.h"

OSG_USING_NAMESPACE

OpenSGLatticeActionDeque::OpenSGLatticeActionDeque() : ActionDeque()
{
    latticeGeo = NullFC;
    latticeNode = NullFC;
    actions = aDeque(0);
    lattice = 0;
    osglg = 0;
    osgmpm = 0;
    showAll = true;
    isActive = false;
    doExecute = false;
}

OpenSGLatticeActionDeque::~OpenSGLatticeActionDeque()
{
    delete osglg;
    delete osgmpm;
    delete lattice;
}

void OpenSGLatticeActionDeque::setNodePtr(NodePtr& node)
{
    this->node = node;
}

void OpenSGLatticeActionDeque::setLattice(Lattice* lattice)
{
    isActive = true;
    this->lattice = lattice;
}

void OpenSGLatticeActionDeque::setGeometryPtr(GeometryPtr latticeGeo)
{
    this->latticeGeo = latticeGeo;
}

void OpenSGLatticeActionDeque::setLatticeNode(NodePtr latticeNode)
{
    this->latticeNode = latticeNode;
}

void OpenSGLatticeActionDeque::setLatticeGeometry(OpenSGLatticeGeometry* osglg)
{
    this->osglg = osglg;
}

void OpenSGLatticeActionDeque::setModelPointManager(
    OpenSGModelPointManager* osgmpm)
{
    this->osgmpm = osgmpm;
}

void OpenSGLatticeActionDeque::setShowAll(bool showAll)
{
    this->showAll = showAll;
    if (osglg != 0)
        osglg->updateGeometry(showAll);
}

void OpenSGLatticeActionDeque::setLatticeRemoved()
{
    latticeGeo = NullFC;
    latticeNode = NullFC;
    delete lattice;
    lattice = 0;
    delete osglg;
    osglg = 0;
    delete osgmpm;
    osgmpm = 0;
    isActive = false;
}

void OpenSGLatticeActionDeque::setInstantExecution(bool doExecute)
{
    this->doExecute = doExecute;
}

NodePtr OpenSGLatticeActionDeque::getNodePtr()
{
    return node;
}

NodePtr OpenSGLatticeActionDeque::getLatticeNodePtr()
{
    return latticeNode;
}

Lattice& OpenSGLatticeActionDeque::getLattice()
{
    return *lattice;
}

const GeometryPtr OpenSGLatticeActionDeque::getGeometryPtr() const
{
    return latticeGeo;
}

OpenSGLatticeGeometry& OpenSGLatticeActionDeque::getLatticeGeometry()
{
    return *osglg;
}

OpenSGModelPointManager& OpenSGLatticeActionDeque::getModelPointManager()
{
    return *osgmpm;
}

bool OpenSGLatticeActionDeque::getShowAll() const
{
    return showAll;
}

bool OpenSGLatticeActionDeque::getInstantExecution() const
{
    return doExecute;
}

bool OpenSGLatticeActionDeque::isDequeActive() const
{
    if (isActive)
        return true;
    return false;
}
