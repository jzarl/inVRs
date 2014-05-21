/***************************************************************************
 *   Copyright (C) 2007 by Thomas Webernorfer                              *
 *   thomas_weberndorfer@hotmail.com                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "ThreadQueue.h"
#include <OpenSG/OSGGLUT.h>

OSG_USING_NAMESPACE;

std::vector<QueuedThread*> ThreadQueue::store;
bool ThreadQueue::running = false;

void ThreadQueue::addToThreadQueue ( QueuedThread *thread )
{
    if ( !running )
    {
        glutTimerFunc ( 50, ThreadQueue::work, 0 ); running = true;
    }
    store.push_back ( thread );
}

void ThreadQueue::work ( int )
{

    std::vector<QueuedThread*>::iterator iter = store.begin();
    while ( iter != store.end() )
    {
        QueuedThread *qt = *iter;
        bool result = qt->run ( glutGet ( GLUT_ELAPSED_TIME ) );
        if ( !result )
        {
            store.erase ( iter );
            delete qt;
            // the iterator is invalid, end the loop
            iter = store.end();
        }
        else
        {
            iter ++;
        }
    }
    if ( store.size() == 0 )
    {
        // nothing to do anymore
        running = false;
    }
    else
    {
        // work left
        glutTimerFunc ( 50, ThreadQueue::work, 0 );
    }
}
