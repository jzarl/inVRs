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


#ifndef OPENSGLATTICEACTIONDEQUE_H_INCLUDED
#define OPENSGLATTICEACTIONDEQUE_H_INCLUDED

#include <deque>

#include <gmtl/Vec.h>
#include <gmtl/Matrix.h>
#include <gmtl/AABox.h>

#include <OpenSG/OSGNode.h>
#if OSG_MAJOR_VERSION >= 2
#  include <OpenSG/OSGNameAttachment.h>
#endif

#include "Lattice.h"
#include "LatticeGeometry.h"
#include "OpenSGModelPointManager.h"
#include "OpenSGLatticeGeometry.h"
#include "Actions/ActionDeque.h"

using std::deque;

/*******************************************************************************
 * @class OpenSGLatticeActionDeque
 * @brief
 *
 *
 */
class OpenSGLatticeActionDeque : public ActionDeque
{
    public:
        OpenSGLatticeActionDeque();
        virtual ~OpenSGLatticeActionDeque();

        void setNodePtr(OSG::NodePtr& node);
        void setLattice(Lattice* lattice);
        void setGeometryPtr(OSG::GeometryPtr latticeGeo);
        void setLatticeNode(OSG::NodePtr latticeNode);
        void setLatticeGeometry(OpenSGLatticeGeometry* osglg);
        void setModelPointManager(OpenSGModelPointManager* osgmpm);
        void setShowAll(bool showAll);
        void setLatticeRemoved();
        void setInstantExecution(bool doExecute);

        OSG::NodePtr getNodePtr();
        OSG::NodePtr getLatticeNodePtr();
        Lattice& getLattice();
        const OSG::GeometryPtr getGeometryPtr() const;
        OpenSGLatticeGeometry& getLatticeGeometry();
        OpenSGModelPointManager& getModelPointManager();
        bool getShowAll() const;
        bool getInstantExecution() const;
        bool isDequeActive() const;

        friend std::ostream& operator<< (std::ostream& out,
            const OpenSGLatticeActionDeque& osglad);

    protected:
        OSG::NodePtr node;
        OSG::NodePtr latticeNode;
        OSG::GeometryPtr latticeGeo;
        Lattice* lattice;
        OpenSGLatticeGeometry* osglg;
        OpenSGModelPointManager* osgmpm;
        bool doExecute;
        bool showAll;
        bool isActive;
};

inline std::ostream& operator<< (std::ostream& out,
    const OpenSGLatticeActionDeque& osglad)
{
    out << "NodePtr foo";
    if (OSG::getName(osglad.node))
        out << " (" << getName(osglad.node) << ")";
    else
        out << "(no name set)";
    out << ": " << osglad.node << std::endl;
    if (osglad.latticeGeo != OSG::NullFC)
        out << "GeometryPtr: " << osglad.latticeGeo << std::endl;
    out << osglad.actions << std::endl;
    return out;
}
#endif // OPENSGLATTICEACTIONDEQUE_H_INCLUDED
