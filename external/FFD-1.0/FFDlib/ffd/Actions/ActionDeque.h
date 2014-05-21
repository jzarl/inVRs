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


#ifndef ACTIONDEQUE_H_INCLUDED
#define ACTIONDEQUE_H_INCLUDED

#include <deque>

#include "DAction.h"
#include "ExecuteDAction.h"

using std::deque;

/*******************************************************************************
 * @class ActionDeque
 * @brief Base container which holds all actions for one node in the order they
 *        were created.
 */
class ActionDeque
{
    public:
        typedef deque<DAction* > aDeque;

        ActionDeque();
        virtual ~ActionDeque();

        /**
         * @return index of the last occurrence of the given action
         * @param skip number of given actions
         */
        template <typename TAction>
        int getIndexOfLastAction(unsigned int skip = 0)
        const
        {
            int index = actions.size() - 1;
            while (index >= 0)
            {
                if ((dynamic_cast<TAction> (actions[index])) != 0)
                {
                    if(skip != 0)
                        --skip;
                    else
                        return index;
                }
                --index;
            }

            return -1;
        }

        /**
         * @return last action in the deque
         */
        const DAction* getLastAction() const;

        /**
         * @return action at the given index
         */
        const DAction* getActionByIndex(unsigned int index) const;

        /**
         * @return deque size, which is the number of actions
         */
        int getDequeSize() const;

        /**
         * @param action to insert at the top of the deque
         */
        void insertAction(DAction* daction);

        /**
         * Deletes last action from the top of the deque
         */
        void removeLastAction();

        /**
         * Optimizes memory and future processing time by reducing redundant
         * information in the action deque for a given node.
         * For example it will merge all consecutive @see setPoint actions.
         */
        int optimizeActionDeque();

    protected:
        aDeque actions;

        int optimize(const int numOfRemoved, const int index);
};

inline std::ostream& operator<< (std::ostream& out,
            const ActionDeque::aDeque& aDeque)
{
    out << "Actions: " << std::endl;
    int as = aDeque.size() - 1;
    for (int i = as; i >= 0; --i)
        out << " " << (*aDeque[i]) << std::endl;
    return out;
}

#endif // ACTIONDEQUE_H_INCLUDED
