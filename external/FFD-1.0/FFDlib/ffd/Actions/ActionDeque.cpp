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


#include "ActionDeque.h"

ActionDeque::ActionDeque()
{
}

ActionDeque::~ActionDeque()
{
    int as = actions.size() - 1;
    for (int i = as; i >= 0; --i)
        delete actions[i];
}

const DAction* ActionDeque::getLastAction() const
{
    if (actions.size() > 0)
        return actions.back();

    return 0;
}

const DAction* ActionDeque::getActionByIndex(unsigned int index)
    const
{
    if (index < actions.size())
        return actions[index];

    return 0;
}

int ActionDeque::getDequeSize() const
{
    return actions.size();
}

void ActionDeque::insertAction(DAction* daction)
{
    actions.push_back(daction);
}

void ActionDeque::removeLastAction()
{
    actions.pop_back();
}

int ActionDeque::optimizeActionDeque()
{
    int removed = optimize(0, (actions.size() - 1));
    return removed;
}

int ActionDeque::optimize(const int numOfRemoved, const int index)
{
    // OPTIMIZE THE DEQUE BY FOLLOWING RULES:
    // ---------------------------------------
    //
    // R<n>                 is a rule, n its number

    // i                    insert lattice action
    // r                    remove lattice action
    // x                    execute action

    // b(a,2)               bend on axis a with factor 2
    // w(a,2)               twist on axis a with factor 2
    // t(a,2)               taper on axis a with factor 2
    // dm                   global deform action with given deformation matrix
    // s(1,2)               setpoint deform action with point index 1 and new
    //                      position 2

    // d: {b, w, t, dm, s}  deform actions
    // l: {i, r, x}         lattice actions

    // FFD RULES:
    // ---------------------------------------
    // R1: (b(a,1) , b(a,2)) -> b(a,1+2))
    // R2: (w(a,1) , w(a,2)) -> w(a,1+2))
    // consecutive bend actions along the same axis can be merged by summing up
    // the bend factors - applies also to the twist action

    // R3: (dm,dm) -> (dm * dm)
    // use assosiativity of matrix multiplication

    // R4: s(1,2), s(1,3) -> s(1,3)
    // merge all consecutive setpoint actions if they apply to the same point

    // LATTICE RULES:
    // ---------------------------------------
    // R5: (i,r) -> ()
    // removing a lattice after inserting directly, both can be deleted

    // R6: (x, x) -> (x)
    // consecutive execute actions can be reduced to one

    // R7:

    return 0;
}
